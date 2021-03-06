#include "rt_ascii_producer_builder.hh"
#include "midge_builder.hh"

namespace midge
{

    static const int s_rt_ascii_producer =
        rt_ascii_producer_builder::lingual< string >( &rt_ascii_producer::set_name, "name" ) +
        rt_ascii_producer_builder::lingual< string >( &rt_ascii_producer::set_file, "file" ) +
        rt_ascii_producer_builder::numerical< count_t >( &rt_ascii_producer::set_length, "length" ) +
        midge_builder::object< rt_ascii_producer >( &midge::add, "rt_ascii_producer" );

}
