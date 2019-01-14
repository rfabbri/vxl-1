//Instantiation of IO for std::map<unsigned, std::map<unsigned,
//                          std::map<unsigned, double> > >
#include<dsm/io/dsm_io_map_map_map.hxx>

DSM_IO_MAP_MAP_MAP_INSTANTIATE(unsigned,unsigned,unsigned,double,std::less<unsigned>,std::less<unsigned>,std::less<unsigned>);
