#ifndef _midge_plot_hh_
#define _midge_plot_hh_

#include "singleton.hh"
#include "node.hh"

#include "TApplication.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TH2D.h"

#include <set>
using std::set;

#include <vector>
using std::vector;

namespace midge
{


    class plot :
        public singleton< plot >
    {
        public:
            friend class singleton< plot > ;

        public:
            typedef vector< real_t > data;
            typedef vector< real_t >::iterator data_it;
            typedef vector< real_t >::const_iterator data_cit;

        public:
            class ordinate
            {
                public:
                    ordinate();
                    ordinate( const count_t& t_size );
                    ~ordinate();

                    void operator()( const count_t& t_size );

                public:
                    string& title();
                    const string& title() const;

                    data& values();
                    const data& values() const;

                private:
                    ordinate( const ordinate& );

                    string f_title;
                    data f_values;
            };

            class abscissa
            {
                public:
                    abscissa();
                    abscissa( const count_t& t_size );
                    ~abscissa();

                    void operator()( const count_t& t_size );

                public:
                    string& title();
                    const string& title() const;

                    count_t& count();
                    const count_t& count() const;

                    real_t& low();
                    const real_t& low() const;

                    real_t& high();
                    const real_t& high() const;

                    data& values();
                    const data& values() const;

                private:
                    abscissa( const abscissa& );

                    string f_title;
                    count_t f_count;
                    real_t f_low;
                    real_t f_high;
                    data f_values;
            };

        private:
            plot();
            ~plot();

        public:
            void initialize();
            void plot_one_dimensional
            (
                const string& p_key,
                const string& p_name,
                const string& p_title,
                const abscissa& p_x_axis,
                const ordinate& p_y_values
            );
            void plot_two_dimensional
            (
                const string& p_key,
                const string& p_name,
                const string& p_title,
                const abscissa& p_x_axis,
                const abscissa& p_y_axis,
                const ordinate& p_z_values
            );
            void graph_two_dimensional
            (
                const string& p_key,
                const string& p_name,
                const string& p_title,
                const abscissa& p_x_axis,
                const abscissa& p_y_axis
            );
            void finalize();

        private:
            count_t f_count;
            TApplication* f_application;

            typedef map< string, TCanvas* > canvas_map;
            typedef canvas_map::iterator canvas_it;
            typedef canvas_map::const_iterator canvas_cit;
            typedef canvas_map::value_type canvas_entry;

            canvas_map f_canvases;

            typedef map< string, TH1D* > th1_map;
            typedef th1_map::iterator th1_it;
            typedef th1_map::const_iterator th1_cit;
            typedef th1_map::value_type th1_entry;

            th1_map f_th1s;

            typedef map< string, TH2D* > th2_map;
            typedef th2_map::iterator th2_it;
            typedef th2_map::const_iterator th2_cit;
            typedef th2_map::value_type th2_entry;

            th2_map f_th2s;

            typedef map< string, TGraph* > graph_map;
            typedef graph_map::iterator graph_it;
            typedef graph_map::const_iterator graph_cit;
            typedef graph_map::value_type graph_entry;

            graph_map f_graphs;
    };

}

#endif
