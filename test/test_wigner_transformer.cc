#include "midge.hh"
#include "rt_sinusoidal_producer.hh"
#include "rt_ascii_consumer.hh"
#include "rt_rtf_wigner_transformer.hh"
#include "rtf_ascii_consumer.hh"
#include "window_blackman.hh"
using namespace midge;

#include <fstream>
using std::ofstream;

#include <iostream>
using std::cout;
using std::endl;

#include <cmath>

int main()
{
    ofstream t_file;
    t_file.open( "test_wigner_transformer.log.txt", std::ios_base::trunc );

    messages* t_messages = messages::get_instance();
    t_messages->set_terminal_severity( s_debug );
    t_messages->set_terminal_stream( &cout );
    t_messages->set_log_severity( s_debug );
    t_messages->set_log_stream( &t_file );

    ::midge::midge* t_midge = new ::midge::midge();

    rt_sinusoidal_producer* t_rt_in = new rt_sinusoidal_producer();
    t_rt_in->set_name( "rt_in" );
    t_rt_in->set_impedance_ohm( 50. );
    t_rt_in->set_carrier_power_dbm( -10. );
    t_rt_in->set_carrier_frequency_hz( 150.e6 );
    t_rt_in->set_carrier_phase_deg( 60. );
    t_rt_in->set_signal_amplitude_hz( 50.e6 );
    t_rt_in->set_signal_frequency_hz( 500.e3 );
    t_rt_in->set_signal_phase_deg( 180. );
    t_rt_in->set_begin_sec( 0.e-9 );
    t_rt_in->set_end_sec( 20000.e-9 );
    t_rt_in->set_start_sec( 2000.e-9 );
    t_rt_in->set_stop_sec( 18000.e-9 );
    t_rt_in->set_interval_sec( 1.e-9 );
    t_rt_in->set_length( 10 );
    t_rt_in->set_size( 1000 );
    t_rt_in->set_stride( 2 );
    t_midge->add( t_rt_in );

    rt_ascii_consumer* t_rt_out = new rt_ascii_consumer();
    t_rt_out->set_name( "rt_out" );
    t_rt_out->set_file( "test_wigner_transformer.in.txt" );
    t_midge->add( t_rt_out );

    rt_rtf_wigner_transformer* t_rt_rf = new rt_rtf_wigner_transformer();
    t_rt_rf->set_name( "rt_rf" );
    t_rt_rf->set_length( 10 );
    t_rt_rf->set_width( 200 );
    t_rt_rf->set_window( new window_blackman() );
    t_midge->add( t_rt_rf );

    rtf_ascii_consumer* t_rf_out = new rtf_ascii_consumer();
    t_rf_out->set_name( "rf_out" );
    t_rf_out->set_file( "test_wigner_transformer.out.txt" );
    t_midge->add( t_rf_out );

    t_midge->join( "rt_in.out_0:rt_out.in_0" );
    t_midge->join( "rt_in.out_0:rt_rf.in_0" );
    t_midge->join( "rt_rf.out_0:rf_out.in_0" );

    t_midge->run( "rt_in:rt_out:rt_rf:rf_out" );

    delete t_midge;

    t_file.close();

    return 0;
}
