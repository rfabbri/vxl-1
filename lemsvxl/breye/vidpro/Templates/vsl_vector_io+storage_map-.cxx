#include <bpro/bpro_storage_sptr.h>
#include <string>
#include <vector>
#include <vsl/vsl_map_io.h>
#include <vsl/vsl_vector_io.hxx>
 
typedef std::map< std::string, std::vector< bpro_storage_sptr > > storage_map;

VSL_VECTOR_IO_INSTANTIATE(storage_map);
