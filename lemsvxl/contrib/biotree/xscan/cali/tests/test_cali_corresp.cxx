#include <cali/cali_artf_corresponder.h>
#include <cali/cali_param.h>
#include <cali/cali_simulated_scan_resource.h>
#include <xscan/xscan_scan.h>
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_conic_2d.h>


typedef vcl_vector<vcl_vector<vsol_conic_2d_sptr> > conic_vector_set;

static void test_cali_corresp(int argc, char* argv[] ){

    vcl_string fname = "C:\\latest_amherst\\testing_correspondences\\parameters.txt";
    cali_param par(fname);
  cali_artf_corresponder corr(par);
  vcl_vector<vcl_vector<vsol_conic_2d > >model_conics;
vnl_vector<double> fx(((par.END - par.START/par.INTERVAL)+1)*par.NUM_OF_CONICS*2);
  xscan_scan scan;
  vcl_vector<vil_image_resource_sptr> resources;

  vcl_string file_name = par.LOGFILE;
   vcl_FILE *fp = vcl_fopen(file_name.data(),"r");
  assert(fp != NULL);
  imgr_skyscan_log_header skyscan_log_header(fp);
  vcl_fclose(fp);
  
  imgr_skyscan_log skyscan_log(file_name.data());
  scan = skyscan_log.get_scan();
  scan.set_scan_size(par.END - par.START + 1);
  vcl_cout << scan << vcl_endl;

   for (unsigned i=0; i<scan.n_views(); i+= par.INTERVAL) {
        resources.push_back(vil_new_image_resource((unsigned) (2.0*scan.kk().principal_point().x()), 
          (unsigned) (2.0*scan.kk().principal_point().y()),  1,  VIL_PIXEL_FORMAT_BYTE));
  }

cali_simulated_scan_resource scan_res(scan, resources,par);

   vnl_quaternion<double> artf_rot(double(-0.000310365),double(-0.0010068),double(
-0.575437),double(0.816012));
vgl_point_3d<double>  artf_trans(0.16741, 0.0741517,-5.96577);

model_conics = scan_res.gen_ball_projections(artf_rot,artf_trans,par.INTERVAL);

vcl_vector<conic_vector_set> real_conics;

 for (unsigned i=0; i<scan.n_views(); i += par.INTERVAL) {
    vcl_string file_name = corr.gen_read_fname(par.CONICS_BIN_FILE_BASE, i);
    conic_vector_set saved_conics = corr.read_conics_bin(file_name);
    real_conics.push_back(saved_conics);
  }

corr.masked_corresp_set(model_conics,real_conics,fx);
                                                     
                                                    


//  vcl_vector<vcl_vector<vsol_conic_2d_sptr> > v = corr.fit_conics("\\test_artifact315.jpg");
//  vcl_cout << v.size() << vcl_endl;

 

  
}
TESTMAIN_ARGS(test_cali_corresp);
