// Instantiation of IO for vcl_map<unsigned, vcl_map<unsigned, double> >
#include<dsm/io/dsm_io_map_map.txx>

DSM_IO_MAP_MAP_INSTANTIATE(unsigned,unsigned,double, vcl_less<unsigned>, vcl_less<unsigned>);
