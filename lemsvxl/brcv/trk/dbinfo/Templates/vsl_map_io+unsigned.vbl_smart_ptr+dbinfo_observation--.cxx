// Instantiation of IO for std::map<unsigned, dbinfo_observation_sptr>
#include <dbinfo/dbinfo_observation.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_map_io.hxx>
VSL_MAP_IO_INSTANTIATE(unsigned, dbinfo_observation_sptr, std::less<unsigned>);
