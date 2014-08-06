#ifndef _midge_rt_ascii_consumer_hh_
#define _midge_rt_ascii_consumer_hh_

#include "consumer.hh"
#include "rt_data.hh"
#include "typelist.hh"
#include "macros.hh"

#include <fstream>
using std::ofstream;

namespace midge
{

    class rt_ascii_consumer :
        public _consumer< rt_ascii_consumer, typelist_1( rt_data )>
    {
        public:
            rt_ascii_consumer();
            virtual ~rt_ascii_consumer();

        public:
            accessible( string, file )

        private:
            ofstream f_stream;

            count_t f_size;
            real_t f_interval;
            const real_t* f_in;
            count_t f_index;

        protected:
            bool start_consumer();
            bool execute_consumer();
            bool stop_consumer();
    };

}

#endif
