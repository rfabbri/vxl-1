// Instantiation of IO for vcl_map<unsigned, vcl_map<unsigned, unsigned> >
#include<dsm/io/dsm_io_map_map.txx>

DSM_IO_MAP_MAP_INSTANTIATE(unsigned,unsigned,unsigned, vcl_less<unsigned>, vcl_less<unsigned>);
