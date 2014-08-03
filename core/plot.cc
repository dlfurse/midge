#include "plot.hh"

#include "core_message.hh"

#include <limits>
using std::numeric_limits;

namespace midge
{

    plot::ordinate::ordinate( const count_t& p_size ) :
            f_title( "" ),
            f_values( p_size )
    {
    }
    plot::ordinate::~ordinate()
    {
    }

    string& plot::ordinate::title()
    {
        return f_title;
    }
    const string& plot::ordinate::title() const
    {
        return f_title;
    }

    plot::data& plot::ordinate::values()
    {
        return f_values;
    }
    const plot::data& plot::ordinate::values() const
    {
        return f_values;
    }

    plot::abscissa::abscissa( const count_t& p_size ) :
            f_title( "" ),
            f_count( 0 ),
            f_low( 0. ),
            f_high( 0. ),
            f_values( p_size )
    {
    }
    plot::abscissa::~abscissa()
    {
    }

    string& plot::abscissa::title()
    {
        return f_title;
    }
    const string& plot::abscissa::title() const
    {
        return f_title;
    }

    count_t& plot::abscissa::count()
    {
        return f_count;
    }
    const count_t& plot::abscissa::count() const
    {
        return f_count;
    }

    real_t& plot::abscissa::low()
    {
        return f_low;
    }
    const real_t& plot::abscissa::low() const
    {
        return f_low;
    }

    real_t& plot::abscissa::high()
    {
        return f_high;
    }
    const real_t& plot::abscissa::high() const
    {
        return f_high;
    }

    plot::data& plot::abscissa::values()
    {
        return f_values;
    }
    const plot::data& plot::abscissa::values() const
    {
        return f_values;
    }

    plot::plot() :
            f_count( 0 ),
            f_application( NULL ),
            f_canvases(),
            f_one_dimensional_plots(),
            f_two_dimensional_plots()
    {

    }
    plot::~plot()
    {
        for( vector< TH1D* >::iterator t_it = f_one_dimensional_plots.begin(); t_it != f_one_dimensional_plots.end(); ++t_it )
        {
            delete *t_it;
        }
        for( vector< TH2D* >::iterator t_it = f_two_dimensional_plots.begin(); t_it != f_two_dimensional_plots.end(); ++t_it )
        {
            delete *t_it;
        }
        for( vector< TCanvas* >::iterator t_it = f_canvases.begin(); t_it != f_canvases.end(); ++t_it )
        {
            delete *t_it;
        }
        delete f_application;
    }
    void plot::initialize()
    {
        if( (f_count++ == 0) && (f_application == NULL) )
        {
            f_application = new TApplication( "", 0, NULL );
        }
        return;
    }
    void plot::plot_one_dimensional( const string& p_label, const string& p_title, const abscissa& p_x, const ordinate& p_y )
    {
        if( p_x.values().size() != p_y.values().size() )
        {
            throw error() << "plot one dimensional was given x values with size <" << p_x.values().size() << "> and y values with size <" << p_y.values().size() << ">";
        }

        msg_normal( coremsg, "making one dimensional plot <" << p_label << ">" << eom );

        real_t t_x_increment = (p_x.high() - p_x.low()) / p_x.count();

        TCanvas* t_canvas = new TCanvas( (p_label + string( "_canvas" )).c_str(), (p_label + string( "_canvas" )).c_str(), 0, 0, 1024, 768 );
        TH1D* t_histogram = new TH1D( (p_label + string( "_histogram" )).c_str(), (p_label + string( "_histogram" )).c_str(), p_x.count(), p_x.low() - .5 * t_x_increment, p_x.high() + .5 * t_x_increment );
        t_histogram->SetDirectory( NULL );
        for( count_t t_index = 0; t_index < p_x.values().size(); t_index++ )
        {
            t_histogram->Fill( p_x.values().at( t_index ), p_y.values().at( t_index ) );
        }

        t_canvas->cd( 0 );
        t_histogram->SetStats( kFALSE );
        t_histogram->SetTitle( p_title.c_str() );
        t_histogram->GetXaxis()->SetTitle( p_x.title().c_str() );
        t_histogram->GetYaxis()->SetTitle( p_y.title().c_str() );
        t_histogram->Draw( "LP" );

        f_canvases.push_back( t_canvas );
        f_one_dimensional_plots.push_back( t_histogram );

        return;
    }
    void plot::plot_two_dimensional( const string& p_label, const string& p_title, const abscissa& p_x, const abscissa& p_y, const ordinate& p_z )
    {
        if( p_z.values().size() != p_x.values().size() )
        {
            throw error() << "time frequency plotter was given z values with size <" << p_z.values().size() << "> and x values with size <" << p_x.values().size() << ">";
        }

        if( p_z.values().size() != p_y.values().size() )
        {
            throw error() << "time frequency plotter was given z values with size <" << p_z.values().size() << "> and y values with size <" << p_y.values().size() << ">";
        }

        msg_normal( coremsg, "making two dimensional plot <" << p_label << ">" << eom );

        real_t t_x_increment = (p_x.high() - p_x.low()) / p_x.count();
        real_t t_y_increment = (p_y.high() - p_y.low()) / p_y.count();

        TCanvas* t_canvas = new TCanvas( (p_label + string( "_canvas" )).c_str(), (p_label + string( "_canvas" )).c_str(), 0, 0, 1024, 768 );
        TH2D* t_histogram = new TH2D( (p_label + string( "_histogram" )).c_str(), (p_label + string( "_histogram" )).c_str(), p_x.count(), p_x.low() - .5 * t_x_increment, p_x.high() + .5 * t_x_increment, p_y.count(), p_y.low() - .5 * t_y_increment, p_y.high() + .5 * t_y_increment );
        t_histogram->SetDirectory( NULL );
        for( count_t t_index = 0; t_index < p_z.values().size(); t_index++ )
        {
            t_histogram->Fill( p_x.values().at( t_index ), p_y.values().at( t_index ), p_z.values().at( t_index ) );
        }

        t_canvas->cd( 0 );
        t_histogram->SetStats( kFALSE );
        t_histogram->SetTitle( p_title.c_str() );
        t_histogram->GetXaxis()->SetTitle( p_x.title().c_str() );
        t_histogram->GetYaxis()->SetTitle( p_y.title().c_str() );
        t_histogram->GetZaxis()->SetTitle( p_z.title().c_str() );
        t_histogram->Draw( "COLZ" );

        f_canvases.push_back( t_canvas );
        f_two_dimensional_plots.push_back( t_histogram );

        return;
    }
    void plot::finalize()
    {
        if( (--f_count == 0) && (f_canvases.size() != 0) )
        {
            f_application->Run( kTRUE );
        }
        return;
    }
}
