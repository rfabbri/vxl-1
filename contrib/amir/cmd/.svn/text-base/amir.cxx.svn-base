// This is 
//:
// \file

#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <pdf1d/pdf1d_calc_mean_var.h>

#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>

#include <dbdet/sel/dbdet_sel_base.h>
#include <dbdet/sel/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/sel/dbdet_sel.h>
#include <dbdet/sel/dbdet_sel_utils.h>

#include <dbdet/edge/dbdet_gen_edge_det.h>
#include <dbdet/edge/dbdet_third_order_edge_det.h>


//forward declarations
int comp(const void* n, const void* m);

int main(int argc, char** argv)
{
  if (argc < 3) {
    vcl_cout << "USAGE: amir <image file name> <num of edges>\n";
    return 1;
  }

  //get the image filename
  vcl_string image_filename(argv[1]);
  int number = atoi(argv[2]);

  //load image
  vil_image_view<vxl_byte> image  = vil_load(image_filename.c_str());

  //compute edges
  double sigma = 1;
  double threshold = 1;
  int N = 1;
  dbdet_edgemap_sptr edgemap = dbdet_detect_third_order_edges(image, sigma, threshold, N, dbdet_nms_params::PFIT_9_POINTS);

  //make an array for sorting
  unsigned num_edgels = edgemap->num_edgels;
  double *dat = new double[4*num_edgels];

  int cnt = 0;
  dbdet_edgemap_const_iter it = edgemap->edge_cells.begin();
  for (; it!=edgemap->edge_cells.end(); it++){
    for (unsigned j=0; j<(*it).size(); j++){
      dbdet_edgel* edgel = (*it)[j];

      dat[4*cnt] = edgel->strength;
      dat[4*cnt+1] = edgel->pt.x();
      dat[4*cnt+2] = edgel->pt.y();
      dat[4*cnt+3] = edgel->tangent*180.0/vnl_math::pi;
      cnt++;
    }
  }

  assert(cnt==num_edgels);

  // Sort by strength
  qsort((void*) dat, num_edgels, 4*sizeof(double), comp);

  //write out the edgels
  for(int i=0; i< ((number < num_edgels) ? number : num_edgels); i++)
  {
    vcl_cout << dat[4*i+1] << " " << dat[4*i+2] << " " << dat[4*i+3] << " " << dat[4*i] << "\n";
  }

  return true;
}

int comp(const void* n, const void* m)
{
  double *x = (double*) n;
  double *y = (double*) m;

  if (*x > *y)
    return -1;
  if (*x < *y)
    return 1;
  return 0;
}

