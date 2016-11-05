// This is brcv/seg/dbdet/tests/test_compute_cues.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_rgb.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_file_vector.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/algo/dbdet_cem_file_io.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbtest_root_dir.h>

static const double tolerance=1e-3;

void load_dataset(vil_image_view<vil_rgb<vxl_byte> > &img, 
      dbdet_curve_fragment_graph &frags_ref,
      dbdet_curve_fragment_graph &frags_geom,
      dbdet_curve_fragment_graph &frags_sem,
      dbdet_edgemap_sptr &edgemap_edg,
      dbdet_edgemap_sptr &edgemap_cem_ref,
      dbdet_edgemap_sptr &edgemap_cem_geom,
      dbdet_edgemap_sptr &edgemap_cem_sem
      )
{

  vcl_string root = dbtest_root_dir();
  vcl_string base_path = root + "/brcv/seg/dbdet/algo/tests/test_data/";

  vcl_string image_path = base_path + "2018.jpg";
  vcl_string frags_path = base_path + "2018.cem";
  vcl_string frags_geom_path = base_path + "2018_geom.cem";
  vcl_string frags_sem_path = base_path + "2018_sem.cem";
  vcl_string edge_path = base_path + "2018.edg";

  img = vil_convert_to_component_order(vil_convert_to_n_planes(3,
        vil_convert_stretch_range (vxl_byte(), vil_load(image_path.c_str()))));

  dbdet_load_edg(edge_path, true, 1.0, edgemap_edg);
  edgemap_cem_ref = dbdet_load_cem(frags_ref_path, frags_ref);
  edgemap_cem_geom = dbdet_load_cem(frags_geom_path, frags_geom);
  edgemap_cem_sem = dbdet_load_cem(frags_sem_path, frags_sem);
}

void contour_breaker_test()
{
  vil_image_view<vil_rgb<vxl_byte> > img;
  dbdet_curve_fragment_graph cfg_ref, cfg_geom, cfg_sem;
  //Need to carry the cem edgemap or CFG edgels are deleted
  dbdet_edgemap_sptr edgemap_sptr, edgemap_cem_ref_sptr, edgemap_cem_geom_sptr, edgemap_cem_sem_sptr;
  load_dataset(img, cfg_ref, cfg_geom, cfg_sem, edgemap_sptr, edgemap_cem_ref_sptr, edgemap_cem_geom_sptr, edgemap_cem_sem_sptr);
  
  dbdet_contour_breaker cb(img, *edgemap_sptr, tmap);
  
  dbdet_curve_fragment_graph geom = cb.dbdet_contour_breaker_geom(cfg_ref, beta1_geom, fmean_geom);  
  dbdet_curve_fragment_graph sem = cb.dbdet_contour_breaker_semantic(cfg_ref, beta1_sem, fmean_sem);

  //TODO
}


MAIN( test_contour_breaker )
{ 
  START ("Test dbdet contour breaker");
  contour_breaker_test();
  SUMMARY();
}
