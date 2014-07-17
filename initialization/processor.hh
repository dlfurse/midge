#ifndef _midge_processor_hh_
#define _midge_processor_hh_

#include "token.hh"
#include "error.hh"

namespace midge
{

    class processor
    {
        public:
            processor();
            virtual ~processor();

            //*********
            //structure
            //*********

        public:
            static void connect( processor* p_parent, processor* p_child );
            static void disconnect( processor* p_parent, processor* p_child );

            void insert_before( processor* p_target );
            void insert_after( processor* p_target );
            void remove();

            processor* get_first_parent();
            processor* get_parent();

            processor* get_last_child();
            processor* get_child();

        protected:
            processor* f_parent;
            processor* f_child;

            //*********
            //callbacks
            //*********

        public:
            virtual void process_key( token* p_token );
            virtual void process_string( token* p_token );
            virtual void process_boolean( token* p_token );
            virtual void process_numerical( token* p_token );
            virtual void process_object_start();
            virtual void process_object_stop();
            virtual void process_array_start();
            virtual void process_array_stop();
            virtual void process_start();
            virtual void process_stop();
    };

}

#endif
