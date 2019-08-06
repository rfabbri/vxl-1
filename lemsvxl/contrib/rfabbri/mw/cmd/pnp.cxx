#include <vul/vul_arg.h>
#include <buld/buld_arg.h>
#include <vul/vul_file.h>


int main(int argc, char** argv)
{

  vul_arg<std::vector<std::string> > a_cams("-cams", "load camera files (space-separated)");
  vul_arg<std::vector<double> > a_f("-flen", "focal length for cams (space-separated)");
  vul_arg_parse(argc,argv);

  return 0;
}
