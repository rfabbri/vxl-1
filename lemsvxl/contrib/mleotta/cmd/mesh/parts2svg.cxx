// This is mleotta/cmd/mesh/parts2svg.cxx


#include <iostream>
#include <vul/vul_arg.h>
#include <modrec/modrec_vehicle_parts.h>




// The Main Function
int main(int argc, char** argv)
{
  vul_arg<std::string>  a_in_file("-i", "input parts file", "");
  vul_arg<std::string>  a_out_file("-o", "output parts file", "");
  vul_arg_parse(argc, argv);

  if(!a_out_file.set()){
    std::cerr << "output file required" << std::endl;
    return -1;
  }

  typedef std::map<std::string, vgl_polygon<double> > pmap;

  pmap parts = modrec_read_vehicle_parts(a_in_file());

  modrec_write_svg(a_out_file(),parts);

  return 0;

}
