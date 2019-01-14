#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <string>
#include <vector>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_map_io.hxx>
 
VSL_MAP_IO_INSTANTIATE(std::string, std::vector<vsol_spatial_object_2d_sptr>, std::less<std::string>);
