#include <string>
#include <vector>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_map_io.hxx>
 
VSL_MAP_IO_INSTANTIATE(std::string, std::vector<double>, std::less<std::string>);
