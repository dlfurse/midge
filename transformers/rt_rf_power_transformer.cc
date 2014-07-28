#include "rt_rf_power_transformer.hh"

#include <cmath>

namespace midge
{

    rt_rf_power_transformer::rt_rf_power_transformer() :
            f_impedance_ohm( 1. ),
            f_size( 0 ),
            f_last( 0 ),
            f_nyquist( true ),
            f_in( NULL ),
            f_out( NULL ),
            f_signal( NULL ),
            f_transform( NULL ),
            f_plan( NULL ),
            f_norm( 1. )
    {
    }
    rt_rf_power_transformer::~rt_rf_power_transformer()
    {
    }

    void rt_rf_power_transformer::set_impedance_ohm( const real_t& p_impedance )
    {
        f_impedance_ohm = p_impedance;
        return;
    }
    const real_t& rt_rf_power_transformer::get_impedance_ohm() const
    {
        return f_impedance_ohm;
    }

    bool rt_rf_power_transformer::start_transformer()
    {
        f_size = in< 0 >()->get_size();
        if( f_size % 2 == 0 )
        {
            out< 0 >()->set_size( (f_size / 2) + 1 );
            f_last = (f_size / 2);
            f_nyquist = true;
        }
        else
        {
            out< 0 >()->set_size( (f_size + 1) / 2 );
            f_last = (f_size + 1) / 2;
            f_nyquist = false;
        }
        out< 0 >()->set_interval( 1. / (in< 0 >()->get_interval() * in< 0 >()->get_size()) );

        f_in = in< 0 >()->raw();
        f_out = out< 0 >()->raw();

        f_signal = (fftw_complex*) (fftw_malloc( f_size * sizeof(fftw_complex) ));
        f_transform = (fftw_complex*) (fftw_malloc( f_size * sizeof(fftw_complex) ));
        f_plan = fftw_plan_dft_1d( f_size, f_signal, f_transform, FFTW_FORWARD, FFTW_MEASURE );

        f_norm = f_impedance_ohm * f_size * f_size;

        return true;
    }
    bool rt_rf_power_transformer::execute_transformer()
    {
        // real signal to complex signal
        for( count_t t_index = 0; t_index < f_size; t_index++ )
        {
            f_signal[ t_index ][ 0 ] = f_in[ t_index ];
            f_signal[ t_index ][ 1 ] = 0.;
        }

        // complex signal to complex spectrum
        fftw_execute( f_plan );

        // complex spectrum to power spectrum
        real_t t_power_watt = (f_transform[ 0 ][ 0 ] * f_transform[ 0 ][ 0 ] + f_transform[ 0 ][ 1 ] * f_transform[ 0 ][ 1 ]) / f_norm;
        real_t t_power_dbm = 10. * log10( t_power_watt ) + 30.;
        f_out[ 0 ] = t_power_watt;
        for( count_t t_index = 1; t_index < f_last; t_index++ )
        {
            t_power_watt = 2. * (f_transform[ t_index ][ 0 ] * f_transform[ t_index ][ 0 ] + f_transform[ t_index ][ 1 ] * f_transform[ t_index ][ 1 ]) / f_norm;
            t_power_dbm = 10. * log10( t_power_watt ) + 30.;
            f_out[ t_index ] = t_power_watt;
        }
        if( f_nyquist == true )
        {
            t_power_watt = (f_transform[ f_last ][ 0 ] * f_transform[ f_last ][ 0 ] + f_transform[ f_last ][ 1 ] * f_transform[ f_last ][ 1 ]) / f_norm;
            t_power_dbm = 10. * log10( t_power_watt ) + 30.;
            f_out[ f_last ] = t_power_watt;
        }

        // update time
        out< 0 >()->set_center_time( in< 0 >()->get_start_time() + .5 * in< 0 >()->get_interval() * (real_t) (f_size - 1) );

        return true;
    }
    bool rt_rf_power_transformer::stop_transformer()
    {
        f_size = 0;
        f_last = 0;
        f_nyquist = true;
        f_in = NULL;
        f_out = NULL;

        fftw_free( f_signal );
        f_signal = NULL;

        fftw_free( f_transform );
        f_transform = NULL;

        fftw_destroy_plan( f_plan );
        f_plan = NULL;

        f_norm = 1.;

        return true;
    }

}
