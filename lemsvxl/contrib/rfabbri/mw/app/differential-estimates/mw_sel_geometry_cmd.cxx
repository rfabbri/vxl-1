#include <mw/algo/mw_test_sel_geometry.h>

#include <vul/vul_arg.h>

int
main(int argc, char **argv)
{

  vul_arg<double> rad_ini("-rad_ini", "initial radius",3.0);
  vul_arg<double> rad_end("-rad_end", "final   radius",100.0);
  vul_arg<unsigned> nrad("-nrad", "neighborhood radius",3);
  vul_arg<double> dx("-dx", "position uncertainty",0.1);
  vul_arg<double> dt("-dt", "angle uncertainty(Deg)",5.0);

  vul_arg_parse(argc,argv);

  vcl_cout << "rad_ini: " << rad_ini() << " rad_end: " << rad_end()
    << " nrad: " << nrad()
    << " dx: " << dx() << " dt: " << dt() 
    << vcl_endl;


  vcl_cerr << "Code is currently being updated to support new SEL\n";
  abort();
// XXX mw_test_sel_geometry::test_circle( rad_ini(), rad_end(), nrad(), dx(), dt());

  

  return 0;
}

