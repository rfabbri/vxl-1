// create shock patches from a given image and sampled coarse shock graph

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_utilities.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_plane.h>
#include <brip/brip_vil_float_ops.h>

#include <vul/vul_timer.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>

#include <dbil/algo/dbil_color_conversions.h>


vsol_polygon_2d_sptr read_con_from_file(vcl_string fname) {
  double x, y;
  char buffer[2000];
  int nPoints;

  vcl_vector<vsol_point_2d_sptr> inp;
  inp.clear();

  vcl_ifstream fp(fname.c_str());
  if (!fp) {
    vcl_cout<<" Unable to Open "<< fname <<vcl_endl;
    return 0;
  }
  //2)Read in file header.
  fp.getline(buffer,2000); //CONTOUR
  fp.getline(buffer,2000); //OPEN/CLOSE flag (not important, we assume close)
  fp >> nPoints;
#if 0
  vcl_cout << "Number of Points from Contour: " << nPoints << vcl_endl;
#endif     
  for (int i=0;i<nPoints;i++) {
    fp >> x >> y;
    vsol_point_2d_sptr vs = new vsol_point_2d(x, y);
    inp.push_back(vs);
  }
  fp.close();
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(inp);
  return poly;
}

int main(int argc, char *argv[]) {
  vcl_cout << "Take a simple closed contour (shock graph has no loops) and extract its shock patch!\n";

  //: out file contains the wrong mathces if any
  vcl_string con_file, output_dir, image_file;

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (argc != 4) {
    vcl_cout << "Usage: <program name> <image_file> <con_file> <output_dir (binary patch storage will be put here)>\n";
    return -1;
  }
  image_file = argv[1];
  con_file = argv[2];
  output_dir = argv[3];
  
  //: load esfs and create trees
  vsol_polygon_2d_sptr poly = read_con_from_file(con_file.c_str());
  vcl_cout << "loaded con...\n";

  vcl_string::size_type pos = con_file.find_last_of("//");
  vcl_string only_con_name = con_file.substr(pos, con_file.length());

   //: load the image
  vil_image_resource_sptr img_sptr = vil_load_image_resource(image_file.c_str());
  vil_image_resource_sptr img_r, img_g, img_b;
  
  vil_image_view<float> L_, A_, B_;
  vil_image_view<vxl_byte> I_;
  I_ = img_sptr->get_view(0, img_sptr->ni(), 0, img_sptr->nj());
  bool color_image_;

  //make sure these images are one plane images
  if (I_.nplanes() != 3){
    color_image_ = false;
    img_r = vil_plane(img_sptr, 0);
    img_g = vil_plane(img_sptr, 0);
    img_b = vil_plane(img_sptr, 0);
  } else {
    convert_RGB_to_Lab(I_, L_, A_, B_);
    color_image_ = true;
    img_r = vil_plane(img_sptr, 0);
    img_g = vil_plane(img_sptr, 1);
    img_b = vil_plane(img_sptr, 2);
  }
  vcl_cout << "loaded and processed images...\n";


  // create the output storage class
  dbskr_shock_patch_storage_sptr output = dbskr_shock_patch_storage_new();
  vcl_srand(int(vcl_time(NULL)));
  dbskr_shock_patch_sptr sp = new dbskr_shock_patch(int(1000*(vcl_rand()/(RAND_MAX+1.0))), 10);
  sp->set_outer_boundary(poly);

  if (poly) {
    dbinfo_observation_sptr obs_r = new dbinfo_observation(0, img_r, poly, true, false, false);
    dbinfo_observation_sptr obs_g = new dbinfo_observation(0, img_g, poly, true, false, false);
    dbinfo_observation_sptr obs_b = new dbinfo_observation(0, img_b, poly, true, false, false);
    vil_image_resource_sptr out_img_r = obs_r->image_cropped(false);  // no background noise
    vil_image_resource_sptr out_img_g = obs_g->image_cropped(false);  // no background noise
    vil_image_resource_sptr out_img_b = obs_b->image_cropped(false);  // no background noise
    vil_image_view<vil_rgb<vxl_byte> > combined = brip_vil_float_ops::combine_color_planes(out_img_r, out_img_g, out_img_b);
    vil_image_resource_sptr out_img = vil_new_image_resource_of_view(combined);
    
    char buffer[1000];
    sprintf(buffer, "%d", sp->id());
    vcl_string cnt_str = buffer;

    vil_save_image_resource(out_img, (output_dir+only_con_name+"_patch_"+cnt_str+".png").c_str()); 
  }

  output->add_patch(sp);
  vsl_b_ofstream bfs((output_dir+only_con_name+"_patch_storage.bin").c_str());
  output->b_write(bfs);
  bfs.close();

  return 0;
}
