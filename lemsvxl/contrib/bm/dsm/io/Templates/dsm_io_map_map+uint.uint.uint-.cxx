// Instantiation of IO for std::map<unsigned, std::map<unsigned, unsigned> >
#include<dsm/io/dsm_io_map_map.hxx>

DSM_IO_MAP_MAP_INSTANTIATE(unsigned,unsigned,unsigned, std::less<unsigned>, std::less<unsigned>);
