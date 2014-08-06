#ifndef _midge_rf_ascii_consumer_hh_
#define _midge_rf_ascii_consumer_hh_

#include "consumer.hh"
#include "rf_data.hh"
#include "typelist.hh"
#include "macros.hh"

#include <fstream>
using std::ofstream;

namespace midge
{

    class rf_ascii_consumer :
        public _consumer< rf_ascii_consumer, typelist_1( rf_data )>
    {
        public:
            rf_ascii_consumer();
            virtual ~rf_ascii_consumer();

        public:
            accessible( string, file )

        private:
            ofstream f_stream;

            count_t f_size;
            real_t f_interval;
            const real_t* f_in;

        protected:
            bool start_consumer();
            bool execute_consumer();
            bool stop_consumer();
    };

}

#endif
