#include <xscan/xscan_dummy_scan.h>
#include <xscan/xscan_uniform_orbit.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>
#include <cali/cali_simulated_scan_resource.h>
#include <cali/cali_vnl_least_squares_function.h>
#include <cali/cali_param.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>

class vil_math_bool_invert_functor
{
 private:
 public:
  bool operator()(bool x)     const { return !x; }
};



int main(int argc, char* argv[]) {
  if(argc < 2){
    vcl_cerr << "usage " << argv[0] << " <parameter file>\n";
    exit(1);
  }

  vcl_string path = argv[1];
  cali_param par(path);
  vcl_string fname = par.LOGFILE;
  vcl_FILE *fp = vcl_fopen(fname.data(),"r");
  assert(fp != NULL);
  imgr_skyscan_log_header skyscan_log_header(fp);
  vcl_fclose(fp);
  
  imgr_skyscan_log skyscan_log(fname.data());
  xscan_scan scan;
  scan = skyscan_log.get_scan();
  vcl_cout << scan << vcl_endl;

  scan.set_scan_size(par.END - par.START + 1);

  int nviews = scan.scan_size();
  // change the scan
  vpgl_calibration_matrix<double> kk(scan.kk());
  vcl_cout << "scan SIZE---->" << nviews << vcl_endl;
  vcl_vector<vil_image_resource_sptr> resources(nviews);
  for (int i=0; i<nviews; i++) {
       resources[i] = vil_new_image_resource(skyscan_log_header.number_of_columns_,skyscan_log_header.number_of_rows_,1,  VIL_PIXEL_FORMAT_BYTE);
  }

  vnl_quaternion<double> artf_rot; 
  vgl_point_3d<double> artf_trans;
  vnl_quaternion<double> turn_table_rot;
  vnl_double_3 turn_table_trans;
  double x_scale;
  double y_scale;
  //unused variable double xy_scale;
  vgl_point_2d<double> princp_point;

  
  // read the x values from the text file
  vcl_string txt_file = par.CONVERGEDVALUES;
  vcl_cout << "reading in values from " <<  txt_file << "\n";
  vcl_ifstream fstream(txt_file.c_str(),vcl_ios::in);
  vnl_vector<double> x(par.SIZE_OF_X);

  double val;
  char val_string[256];
  int x_size=0;
  while (!fstream.eof()) {
    fstream.getline(val_string, 256);
    val = vcl_atof(val_string);
    x[x_size++] = val;
    vcl_cout << x_size << "=" << val << vcl_endl;
  }


  cali_vnl_least_squares_function::gen_scan_lsqr_fn_params(x, artf_rot,
    artf_trans,turn_table_rot,turn_table_trans,                     
    x_scale,y_scale,princp_point);

  kk.set_x_scale(x_scale);
  kk.set_y_scale(y_scale);
  kk.set_principal_point(princp_point);
  scan.set_calibration_matrix(kk);

  turn_table_rot.normalize();
  xscan_orbit_base_sptr orbit_base = (scan.orbit());
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(orbit.rot(),
                        orbit.t(),
                        turn_table_rot,
                        turn_table_trans);
  scan.set_orbit(new_orbit);

  vcl_vector<double>x_coord_tol;

  x_coord_tol.push_back(0);
  x_coord_tol.push_back(0);
  x_coord_tol.push_back(-0.0947263);
  x_coord_tol.push_back(0.0404068);
  x_coord_tol.push_back(0.050888);
  x_coord_tol.push_back(0.296285);
  x_coord_tol.push_back(0.286996);
  x_coord_tol.push_back(0.109258);
  x_coord_tol.push_back(0.125204);
  x_coord_tol.push_back(-0.103531);
  x_coord_tol.push_back(-0.0743285);
  x_coord_tol.push_back(0);
  x_coord_tol.push_back(0);

  vcl_vector<double>y_coord_tol;

  y_coord_tol.push_back(0);
  y_coord_tol.push_back(0);
  y_coord_tol.push_back(0.00245647);
  y_coord_tol.push_back(0.0188508);
  y_coord_tol.push_back(-0.183153);
  y_coord_tol.push_back(-0.376836);
  y_coord_tol.push_back(-0.182137);
  y_coord_tol.push_back(-0.164995);
  y_coord_tol.push_back(0.128703);
  y_coord_tol.push_back(0.243105);
  y_coord_tol.push_back(-0.00153744);
  y_coord_tol.push_back(0);
  y_coord_tol.push_back(0);

  vcl_vector<double>z_coord_tol;
  vcl_vector<double>rad_tol;

   for (int i = 0;i<13;i++)
   {
       z_coord_tol.push_back(0);
       rad_tol.push_back(0);
   }


   cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan,resources,par,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);
  vcl_cout << "Rotation---------------------------->" << vcl_endl;
  vcl_cout << artf_rot << vcl_endl;
  vcl_cout << "Translation------------------------->" << vcl_endl;
  vcl_cout << artf_trans << vcl_endl;
  vcl_vector<vil_image_resource_sptr> images = scan_res.simulate_scan(".", "test_images", artf_rot, artf_trans,par.INTERVAL);



  int i = 0;
  for (int k = par.START; k < par.END && i < images.size(); k += par.INTERVAL,i++) {  
          vcl_string sourcename = cali_artf_corresponder::gen_read_fname( par.CONICS_BIN_FILE_BASE, k);

          vcl_string dir = vul_file::dirname(sourcename);
          vcl_string ext = vul_file::extension(sourcename);
          vcl_string base = vul_file::basename(sourcename,ext.c_str());
          sourcename = dir + "/" + base + ".tif";

          vil_image_view<vxl_uint_16> source = vil_load(sourcename.c_str());
          vcl_cerr << "loaded " << sourcename << " size " << source.ni() << " " << source.nj() << "\n";
          vil_image_view<vxl_byte> synthetic = images[i]->get_view();
          vcl_cerr << "synthetic " << " size " << synthetic.ni() << " " << synthetic.nj() << "\n";

          vil_image_view<vxl_byte > threeplane_img(synthetic.ni(),synthetic.nj(),3);
          vil_image_view<vxl_byte> r = vil_plane(threeplane_img,0);
          vil_image_view<vxl_byte> g = vil_plane(threeplane_img,1);
          vil_image_view<vxl_byte> b = vil_plane(threeplane_img,2);


          //convert greyscale uint_16 to rgb byte
          vil_convert_stretch_range(source,r);
          vil_convert_stretch_range(source,g);
          vil_convert_stretch_range(source,b);

          vil_image_view<bool> mask_balls_on;
          vil_image_view<bool> mask_balls_off;
          //synthetic image is 0 on balls, 255 elsewhere.  
          //invert synthetic so that balls are 255, bg is 0
          vil_math_scale_and_offset_values(synthetic,-1,255);
          //cast to bool, masks in balls
          vil_convert_cast(synthetic,mask_balls_on);
          //invert, masks out balls
          mask_balls_off.deep_copy(mask_balls_on);
          vil_transform(mask_balls_off,vil_math_bool_invert_functor());


          //desired overlay is equal to : 
          //source*mask_balls_off + 0.5*source*mask_balls_on + 0.5*synthetic

          vil_image_view<vxl_byte> r_with_balls;
          vil_image_view<vxl_byte> g_with_balls;
          vil_image_view<vxl_byte> b_with_balls;
          vil_image_view<vxl_byte> r_without_balls;
          vil_image_view<vxl_byte> g_without_balls;
          vil_image_view<vxl_byte> b_without_balls;
          vil_math_image_product(r,mask_balls_on,r_with_balls);
          vil_math_image_product(g,mask_balls_on,g_with_balls);
          vil_math_image_product(b,mask_balls_on,b_with_balls);
          vil_math_image_product(r,mask_balls_off,r_without_balls);
          vil_math_image_product(g,mask_balls_off,g_without_balls);
          vil_math_image_product(b,mask_balls_off,b_without_balls);

          //alpha blend the synthetic balls against the source image
          vil_math_scale_and_offset_values(synthetic,0.5,0);
          vil_math_scale_and_offset_values(r_with_balls,0.5,0);
          vil_math_scale_and_offset_values(g_with_balls,0.5,0);
          vil_math_scale_and_offset_values(b_with_balls,0.5,0);

          vil_image_view<vxl_byte> byte_mask_balls_on;
          vil_image_view<vxl_byte> byte_mask_balls_off;
          vil_convert_cast(mask_balls_off,byte_mask_balls_off);
          vil_convert_cast(mask_balls_on,byte_mask_balls_on);
          vil_math_scale_and_offset_values(byte_mask_balls_on,255,0);
          vil_math_scale_and_offset_values(byte_mask_balls_off,255,0);
          vil_math_image_sum(synthetic,r_with_balls,r);
          vil_math_image_sum(r,r_without_balls,r);
          vil_math_image_sum(g_with_balls,g_without_balls,g);
          vil_math_image_sum(b_with_balls,b_without_balls,b);


          vcl_string outname = cali_artf_corresponder::gen_read_fname("./overlay0000.tif", k);
          dir = vul_file::dirname(outname);
          ext = vul_file::extension(outname);
          base = vul_file::basename(outname,ext.c_str());
          outname = dir + "/" + base + ".png";
          vcl_cout << "save to " << outname << "\n";
          vil_save(threeplane_img,outname.c_str());
  }

}

