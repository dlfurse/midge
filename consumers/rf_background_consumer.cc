#include "rf_background_consumer.hh"

#include "plot.hh"

namespace midge
{

    rf_background_consumer::rf_background_consumer() :
            f_file( "" ),
            f_length( 0 ),
            f_window( NULL ),
            f_plot( false ),
            f_plot_key( "" ),
            f_plot_name( "" ),
            f_chart_title( "" ),
            f_axis_title( "" ),
            f_stream( NULL ),
            f_label( NULL ),
            f_tree( NULL ),
            f_frequency_point( 0. ),
            f_value_point( 0. ),
            f_size( 0 ),
            f_interval( 1. ),
            f_in( NULL ),
            f_average( NULL ),
            f_background( NULL ),
            f_count( 0 )
    {
    }
    rf_background_consumer::~rf_background_consumer()
    {
    }

    bool rf_background_consumer::start_consumer()
    {
        if( f_plot == true )
        {
            plot::get_instance()->initialize();
        }

        f_stream = new TFile( f_file.c_str(), "RECREATE" );

        f_label = new TObjString( "midge_background" );

        f_tree = new TTree( get_name().c_str(), get_name().c_str() );
        f_tree->Branch( "frequency", &f_frequency_point, 64000, 99 );
        f_tree->Branch( "value", &f_value_point, 64000, 99 );

        f_size = in< 0 >()->get_size();
        f_interval = in< 0 >()->get_interval();
        f_in = in< 0 >()->raw();
        f_count = 0;

        f_average = new real_t[ f_size ];
        for( count_t t_index = 0; t_index < f_size; t_index++ )
        {
            f_average[ t_index ] = 0.;
        }

        if( f_window == NULL )
        {
            f_window = new window();
        }
        f_window->initialize( f_size );

        return true;
    }

    bool rf_background_consumer::execute_consumer()
    {
        for( count_t t_index = 0; t_index < f_size; t_index++ )
        {
            f_average[ t_index ] += f_in[ t_index ];
        }
        f_count++;

        return true;
    }

    bool rf_background_consumer::stop_consumer()
    {
        for( count_t t_index = 0; t_index < f_size; t_index++ )
        {
            f_frequency_point = t_index * f_interval;
            f_value_point = f_average[ t_index ] / f_count;
            f_tree->Fill();
        }

        f_stream->cd();
        f_label->Write();
        f_tree->Write();
        f_stream->Close();
        delete f_stream;

        if( f_plot == true )
        {
            msg_debug( coremsg, "  rf average root consumer <" << get_name() << "> plotting" << eom );

            TFile* t_file = new TFile( f_file.c_str(), "READ" );

            TTree* t_tree = (TTree*) (t_file->Get( get_name().c_str() ));
            Long64_t t_entries = t_tree->GetEntries();
            real_t t_frequency;
            real_t t_value;
            t_tree->SetBranchAddress( "frequency", &t_frequency );
            t_tree->SetBranchAddress( "value", &t_value );

            plot::abscissa t_frequencies( t_entries );
            t_frequencies.title() = string( "Frequency [Hz]" );
            t_frequencies.count() = f_size;
            t_frequencies.low() = 0.;
            t_frequencies.high() = f_interval * (f_size - 1);

            plot::ordinate t_values( t_entries );
            t_values.title() = f_axis_title;

            for( Long64_t t_index = 0; t_index < t_entries; t_index++ )
            {
                t_tree->GetEntry( t_index );

                t_frequencies.values().at( t_index ) = t_frequency;
                t_values.values().at( t_index ) = t_value;
            }

            t_file->Close();
            delete t_file;

            plot::get_instance()->plot_one_dimensional( f_plot_key, f_plot_name, f_chart_title, t_frequencies, t_values );

            plot::get_instance()->finalize();
        }

        f_size = 0;
        f_interval = 1.;
        f_in = NULL;
        f_count = 0;

        delete[] f_average;

        return true;
    }

}
