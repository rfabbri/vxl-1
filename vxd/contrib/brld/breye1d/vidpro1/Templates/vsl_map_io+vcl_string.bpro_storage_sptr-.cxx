#include <bpro1/bpro1_storage_sptr.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <string>
#include <vsl/vsl_map_io.hxx>
 
VSL_MAP_IO_INSTANTIATE(std::string, bpro1_storage_sptr, std::less<std::string>);
