#include <mw/mw_util.h>
#include <mw/app/ctspheres_app.h>
#include <vul/vul_arg.h>


int
main(int argc, char **argv)
{

  vul_arg<double> a_theta("-theta", "Perturbation theta (degrees)",0.0);
  vul_arg<double> a_phi("-phi", "Perturbation phi (degrees)",0.0);

  vul_arg_parse(argc,argv);

  vcl_cout << "t: " <<  a_theta() << "\tp: " << a_phi() << vcl_endl;
  mw_sphere_apparent_contour_app1(a_theta(),a_phi(),false,true);

  return 0;
}
