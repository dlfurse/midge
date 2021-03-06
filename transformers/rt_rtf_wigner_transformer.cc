#include "rt_rtf_wigner_transformer.hh"

#include "fourier.hh"

#include <cmath>

namespace midge
{

    rt_rtf_wigner_transformer::rt_rtf_wigner_transformer() :
            f_window( NULL ),
            f_width( 0 ),
            f_length( 10 )
    {
    }
    rt_rtf_wigner_transformer::~rt_rtf_wigner_transformer()
    {
    }

    void rt_rtf_wigner_transformer::initialize()
    {
        out_buffer< 0 >().initialize( f_length );

        if( f_window == NULL )
        {
            throw error() << "rt rf wigner transformer window unset";
        }

        return;
    }

    void rt_rtf_wigner_transformer::execute()
    {
        index_t t_index;

        enum_t t_in_command;

        rt_data* t_in_data;
        count_t t_in_size;

        rtf_data* t_out_data;
        count_t t_out_size;

        real_t t_time_interval;
        count_t t_time_index;
        real_t t_frequency_interval;
        count_t t_frequency_index;

        count_t t_offset;
        count_t t_under;
        count_t t_nyquist;
        count_t t_over;

        real_t t_norm;

        fourier* t_fourier = fourier::get_instance();
        complex_t* t_signal = NULL;
        complex_t* t_transform = NULL;
        complex_t* t_analytic = NULL;
        complex_t* t_correlation = NULL;
        complex_t* t_distribution = NULL;
        fourier_t* t_forward_generator = NULL;
        fourier_t* t_backward_generator = NULL;
        fourier_t* t_final_generator = NULL;

        while( true )
        {
            t_in_command = in_stream< 0 >().get();
            t_in_data = in_stream< 0 >().data();
            t_out_data = out_stream< 0 >().data();

            if( t_in_command == stream::s_start )
            {
                t_in_size = t_in_data->get_size();
                t_out_size = 2 * f_width;
                t_offset = t_in_size / 2;
                if( t_in_size % 2 == 0 )
                {
                    t_under = (t_in_size / 2) - 1;
                    t_nyquist = (t_in_size / 2);
                    t_over = (t_in_size / 2) + 1;
                }
                else
                {
                    t_under = (t_in_size - 1) / 2;
                    t_nyquist = 0;
                    t_over = (t_in_size + 1) / 2;
                }

                t_time_interval = t_in_data->get_time_interval();
                t_time_index = t_in_data->get_time_index();
                t_frequency_interval = 1. / ( 2. * t_time_interval * t_out_size );
                t_frequency_index = 0;

                f_window->set_size( 2 * f_width - 1 );
                t_norm = 2. / (f_window->sum() * f_window->sum());

                t_signal = t_fourier->allocate< complex_t >( t_in_size );
                t_transform = t_fourier->allocate< complex_t >( t_in_size );
                t_analytic = t_fourier->allocate< complex_t >( t_in_size );
                t_correlation = t_fourier->allocate< complex_t >( t_out_size );
                t_distribution = t_fourier->allocate< complex_t >( t_out_size );
                t_forward_generator = t_fourier->forward( t_in_size, t_signal, t_transform );
                t_backward_generator = t_fourier->backward( t_in_size, t_transform, t_analytic );
                t_final_generator = t_fourier->forward( t_out_size, t_correlation, t_distribution );

                t_out_data->set_size( t_out_size );
                t_out_data->set_time_interval( t_time_interval );
                t_out_data->set_time_index( t_time_index + t_offset );
                t_out_data->set_frequency_interval( t_frequency_interval );
                t_out_data->set_frequency_index( t_frequency_index );

                out_stream< 0 >().set( stream::s_start );
                continue;
            }
            if( t_in_command == stream::s_run )
            {
                t_time_index = t_in_data->get_time_index();

                t_out_data->set_size( t_out_size );
                t_out_data->set_time_interval( t_time_interval );
                t_out_data->set_time_index( t_time_index + t_offset );
                t_out_data->set_frequency_interval( t_frequency_interval );
                t_out_data->set_frequency_index( t_frequency_index );

                for( t_index = 0; t_index < t_in_size; t_index++ )
                {
                    t_signal[ t_index ][ 0 ] = t_in_data->at( t_index );
                    t_signal[ t_index ][ 1 ] = 0.;
                }

                t_fourier->execute( t_forward_generator );

                t_transform[ 0 ][ 0 ] = 1. * t_transform[ 0 ][ 0 ];
                t_transform[ 0 ][ 1 ] = 1. * t_transform[ 0 ][ 1 ];
                for( t_index = 1; t_index <= t_under; t_index++ )
                {
                    t_transform[ t_index ][ 0 ] = 2. * t_transform[ t_index ][ 0 ];
                    t_transform[ t_index ][ 1 ] = 2. * t_transform[ t_index ][ 1 ];
                }
                if( t_nyquist != 0 )
                {
                    t_transform[ t_nyquist ][ 0 ] = 1. * t_transform[ t_nyquist ][ 0 ];
                    t_transform[ t_nyquist ][ 1 ] = 1. * t_transform[ t_nyquist ][ 1 ];
                }
                for( t_index = t_over; t_index < t_in_size; t_index++ )
                {
                    t_transform[ t_index ][ 0 ] = 0.;
                    t_transform[ t_index ][ 1 ] = 0.;
                }

                t_fourier->execute( t_backward_generator );

                real_t t_a;
                real_t t_b;
                real_t t_c;
                real_t t_d;
                for( t_index = 0; t_index < f_width; t_index++ )
                {
                    t_a = t_analytic[ t_offset + t_index ][ 0 ] * f_window->at( f_width - 1 + t_index );
                    t_b = t_analytic[ t_offset + t_index ][ 1 ] * f_window->at( f_width - 1 + t_index );
                    t_c = t_analytic[ t_offset - t_index ][ 0 ] * f_window->at( f_width - 1 - t_index );
                    t_d = t_analytic[ t_offset - t_index ][ 1 ] * f_window->at( f_width - 1 - t_index );

                    t_correlation[ t_index ][ 0 ] = t_norm * (t_a * t_c + t_b * t_d);
                    t_correlation[ t_index ][ 1 ] = t_norm * (t_b * t_c - t_a * t_d);
                }
                t_correlation[ f_width ][ 0 ] = 0.;
                t_correlation[ f_width ][ 1 ] = 0.;
                for( t_index = f_width + 1; t_index < 2 * f_width; t_index++ )
                {
                    t_a = t_analytic[ t_offset + t_index - 2 * f_width ][ 0 ] * f_window->at( f_width - 1 + t_index - 2 * f_width );
                    t_b = t_analytic[ t_offset + t_index - 2 * f_width ][ 1 ] * f_window->at( f_width - 1 + t_index - 2 * f_width );
                    t_c = t_analytic[ t_offset - t_index + 2 * f_width ][ 0 ] * f_window->at( f_width - 1 - t_index + 2 * f_width );
                    t_d = t_analytic[ t_offset - t_index + 2 * f_width ][ 1 ] * f_window->at( f_width - 1 - t_index + 2 * f_width );

                    t_correlation[ t_index ][ 0 ] = t_norm * (t_a * t_c + t_b * t_d);
                    t_correlation[ t_index ][ 1 ] = t_norm * (t_b * t_c - t_a * t_d);
                }

                t_fourier->execute( t_final_generator );

                for( t_index = 0; t_index < t_out_size; t_index++ )
                {
                    t_out_data->at( t_index ) = t_distribution[ t_index ][ 0 ];
                }

                out_stream< 0 >().set( stream::s_run );
                continue;
            }
            if( t_in_command == stream::s_stop )
            {
                t_fourier->free< complex_t >( t_signal );
                t_fourier->free< complex_t >( t_transform );
                t_fourier->free< complex_t >( t_analytic );
                t_fourier->free< complex_t >( t_correlation );
                t_fourier->free< complex_t >( t_distribution );
                t_fourier->destroy( t_forward_generator );
                t_fourier->destroy( t_backward_generator );
                t_fourier->destroy( t_final_generator );

                out_stream< 0 >().set( stream::s_stop );
                continue;
            }
            if( t_in_command == stream::s_exit )
            {
                out_stream< 0 >().set( stream::s_exit );
                return;
            }
        }

        return;
    }

    void rt_rtf_wigner_transformer::finalize()
    {
        out_buffer< 0 >().finalize();

        delete f_window;

        return;
    }

}
