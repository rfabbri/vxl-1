#include <string>
#include <vnl/vnl_double_3x3.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vsl/vsl_map_io.hxx>
 
typedef std::map<std::string,vnl_double_3x3> map_type;
VSL_MAP_IO_INSTANTIATE(int, map_type, std::less<int>);
