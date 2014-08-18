#include "rt_monarch_consumer.hh"

#include <cmath>

namespace midge
{

    rt_monarch_consumer::rt_monarch_consumer() :
            f_file( "" ),
            f_bits( 8 ),
            f_record_length( 4194304 ),
            f_voltage_minimum( -.25 ),
            f_voltage_maximum( .25 )
    {
    }
    rt_monarch_consumer::~rt_monarch_consumer()
    {
    }

    void rt_monarch_consumer::initialize()
    {
        return;
    }

    void rt_monarch_consumer::execute()
    {
        count_t t_index;

        state_t t_in_state;
        const rt_data* t_in_data;
        const real_t* t_in_raw;
        count_t t_in_size;
        real_t t_in_time_interval;
        count_t t_in_time_index;

        real_t t_datum;
        real_t t_voltage_range = f_voltage_maximum - f_voltage_minimum;
        real_t t_voltage_inverse_range = 1. / t_voltage_range;
        real_t t_voltage_levels = 1 << f_bits;
        real_t t_voltage_inverse_levels = 1. / t_voltage_levels;

        monarch::Monarch* t_monarch = NULL;
        monarch::MonarchHeader* t_header = NULL;
        monarch::MonarchRecord< monarch::DataType >* t_record = NULL;

        count_t t_first_unwritten_index;
        count_t t_record_index;
        count_t t_record_count;

        while( true )
        {
            in_stream< 0 >()++;
            t_in_state = in_stream< 0 >().state();
            t_in_data = in_stream< 0 >().data();

            if( t_in_state == stream::s_start )
            {
                t_in_size = t_in_data->get_size();
                t_in_time_interval = t_in_data->get_time_interval();
                t_in_time_index = t_in_data->get_time_index();

                t_monarch = monarch::Monarch::OpenForWriting( f_file );

                t_header = t_monarch->GetHeader();
                t_header->SetFilename( f_file );
                t_header->SetDescription( string( "produced by midge rt monarch consumer <" ) + this->get_name() + string( ">" ) );
                t_header->SetFormatMode( monarch::sFormatSingle );
                t_header->SetAcquisitionMode( monarch::sOneChannel );
                t_header->SetAcquisitionRate( 1.e-6 / t_in_time_interval );
                t_header->SetRunDuration( 0. );
                t_header->SetRunType( monarch::sRunTypeSignal );
                t_header->SetRunSource( monarch::sSourceSimulation );
                t_header->SetRecordSize( f_record_length );
                t_header->SetDataTypeSize( sizeof(monarch::DataType) );
                t_header->SetBitDepth( f_bits );
                t_header->SetVoltageMin( f_voltage_minimum );
                t_header->SetVoltageRange( t_voltage_range );
                t_monarch->WriteHeader();

                t_monarch->SetInterface( monarch::sInterfaceInterleaved );
                t_record = t_monarch->GetRecordInterleaved();

                t_first_unwritten_index = t_in_time_index;
                t_record_index = 0;
                t_record_count = 0;
                continue;
            }
            if( t_in_state == stream::s_run )
            {
                t_in_raw = t_in_data->raw();
                t_in_time_index = t_in_data->get_time_index();

                if( t_in_time_index < t_first_unwritten_index )
                {
                    for( t_index = t_first_unwritten_index; t_index < t_in_time_index + t_in_size; t_index++ )
                    {
                        t_datum = t_in_raw[ t_index - t_in_time_index ];
                        t_datum = (t_datum - f_voltage_minimum) * t_voltage_inverse_range * t_voltage_levels;

                        if( t_datum > (t_voltage_levels - 1.) )
                        {
                            t_datum = t_voltage_levels - 1.;
                        }
                        else if( t_datum < 0. )
                        {
                            t_datum = 0.;
                        }

                        t_record->fData[ t_record_index ] = (monarch::DataType) (t_datum);

                        if( ++t_record_index == f_record_length )
                        {
                            t_record->fAcquisitionId = 0;
                            t_record->fRecordId = t_record_count;
                            t_monarch->WriteRecord();
                            t_record_index = 0;
                            t_record_count++;
                        }
                    }
                }
                else
                {
                    for( t_index = t_in_time_index; t_index < t_in_time_index + t_in_size; t_index++ )
                    {
                        t_datum = t_in_raw[ t_index - t_in_time_index ];
                        t_datum = (t_datum - f_voltage_minimum) * t_voltage_inverse_range * t_voltage_levels;

                        if( t_datum > (t_voltage_levels - 1.) )
                        {
                            t_datum = t_voltage_levels - 1.;
                        }
                        else if( t_datum < 0. )
                        {
                            t_datum = 0.;
                        }

                        t_record->fData[ t_record_index ] = (monarch::DataType) (t_datum);

                        if( ++t_record_index == f_record_length )
                        {
                            t_record->fAcquisitionId = 0;
                            t_record->fRecordId = t_record_count;
                            t_monarch->WriteRecord();
                            t_record_index = 0;
                            t_record_count++;
                        }
                    }
                }

                t_first_unwritten_index = t_in_time_index + t_in_size;

                continue;
            }
            if( t_in_state == stream::s_stop )
            {
                t_monarch->Close();
                delete t_monarch;

                continue;
            }
            if( t_in_state == stream::s_exit )
            {
                break;
            }
        }

        return;
    }

    void rt_monarch_consumer::finalize()
    {
        return;
    }

}
