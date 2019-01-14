#include <bmcsd/bmcsd_util.h>
#include <mw/app/ctspheres_app.h>
#include <vul/vul_arg.h>


int
main(int argc, char **argv)
{

  vul_arg<double> a_theta("-theta", "Perturbation theta (degrees)",0.0);
  vul_arg<double> a_phi("-phi", "Perturbation phi (degrees)",0.0);

  vul_arg_parse(argc,argv);

  std::cout << "t: " <<  a_theta() << "\tp: " << a_phi() << std::endl;
  mw_sphere_apparent_contour_app1(a_theta(),a_phi(),false,true);

  return 0;
}
