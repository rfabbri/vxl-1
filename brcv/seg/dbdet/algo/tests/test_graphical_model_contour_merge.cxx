// This is brcv/seg/dbdet/tests/test_graphical_model_contour_merge.cxx
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
#include <dbdet/filter/dbdet_filter_util.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/algo/dbdet_graphical_model_contour_merge.h>
#include <dbtest_root_dir.h>

static const double tolerance=1e-3;

void load_dataset(vil_image_view<vil_rgb<vxl_byte> > &img, 
      dbdet_curve_fragment_graph &frags_ref,
      dbdet_curve_fragment_graph &frags_ori,
      dbdet_edgemap_sptr &edgemap_edg,
      dbdet_edgemap_sptr &edgemap_cem_ref,
      dbdet_edgemap_sptr &edgemap_cem_ori,
      vnl_matrix<unsigned> &tmap,
      vnl_matrix<double> &params_geom,
      vnl_matrix<double> &params_sem
      )
{

  vcl_string root = dbtest_root_dir();
  vcl_string base_path = root + "/brcv/seg/dbdet/algo/tests/test_data/";

  //TODO Gen test data, using wrong test data atm
  vcl_string image_path = base_path + "2018.jpg";
  vcl_string frags_ref_path = base_path + "2018.cem";
  vcl_string frags_ori_path = base_path + "2018_geom.cem";
  vcl_string edge_path = base_path + "2018.edg";
  vcl_string tmap_path = base_path + "2018_tmap.txt";
  vcl_string params_geom_path = base_path + "gPb_SEL_beta_of_geomcon_cue_for_merging.txt";
  vcl_string params_sem_path = base_path + "gPb_SEL_beta_of_cues_for_merging.txt";

  img = vil_convert_to_component_order(vil_convert_to_n_planes(3,
        vil_convert_stretch_range (vxl_byte(), vil_load(image_path.c_str()))));

  dbdet_load_edg(edge_path, true, 1.0, edgemap_edg);
  edgemap_cem_ref = dbdet_load_cem(frags_ref_path, frags_ref);
  edgemap_cem_ori = dbdet_load_cem(frags_ori_path, frags_ori);

  loadFromTabSpaced(tmap_path.c_str(), tmap);

  params_geom = static_cast<vnl_matrix<double> > (vnl_file_matrix<double>(params_geom_path.c_str()));
  params_sem = static_cast<vnl_matrix<double> > (vnl_file_matrix<double>(params_sem_path.c_str()));
}

void graphical_model_contour_merge_test()
{
  vnl_matrix<unsigned> tmap = vnl_matrix<unsigned>(1,1);
  vnl_matrix<double> params_geom = vnl_matrix<double>(1,1);
  vnl_matrix<double> params_sem = vnl_matrix<double>(1,1);

  vil_image_view<vil_rgb<vxl_byte> > img;
  dbdet_curve_fragment_graph cfg_ref, cfg_ori;
  //Need to carry the cem edgemap or CFG edgels are deleted
  dbdet_edgemap_sptr edgemap_sptr, edgemap_cem_ref_sptr, edgemap_cem_ori_sptr, edgemap_cem_sem_sptr;
  load_dataset(img, cfg_ref, cfg_ori, edgemap_sptr, edgemap_cem_ref_sptr, edgemap_cem_ori_sptr, tmap, params_geom, params_sem);

  tmap.inplace_transpose();
  
  y_params_1_vector fmean_geom = y_params_1_vector(params_geom.get_row(0));
  y_params_1_vector fstd_geom = y_params_1_vector(params_geom.get_row(1));
  y_params_1_vector beta_geom = y_params_1_vector(params_geom.get_row(2));

  for (unsigned i = 0; i < y_params_1_size; ++i)
    beta_geom[i] /= fstd_geom[i];

  y_params_0_vector fmean_sem = y_params_0_vector(params_sem.get_row(0));
  y_params_0_vector fstd_sem = y_params_0_vector(params_sem.get_row(1));
  y_params_0_vector beta_sem = y_params_0_vector(params_sem.get_row(2));

  for (unsigned i = 0; i < y_params_0_size; ++i)
    beta_sem[i] /= fstd_sem[i];

  dbdet_graphical_model_contour_merge cm(img, *edgemap_sptr, tmap);

  cm.dbdet_merge_contour(cfg_ori, beta_geom, fmean_geom, beta_sem, fmean_sem);

  TEST("Graphical Model Contour Merge: #chains", cfg_ori.frags.size(), cfg_ref.frags.size());

  bool s_status = true, e_status = true;
  dbdet_edgel_chain_list_iter ref_it = cfg_ref.frags.begin();
  for(dbdet_edgel_chain_list_iter it = cfg_ori.frags.begin(); (it != cfg_ori.frags.end() && ref_it != cfg_ref.frags.end()); it++, ref_it++)
  {
    if((*it)->edgels.size() != (*ref_it)->edgels.size())
    {
      s_status = e_status = false;
      break;
    }
    
    dbdet_edgel_list & el1 = (*it)->edgels;
    dbdet_edgel_list & el2 = (*ref_it)->edgels; 
    for(unsigned i = 0; i < el1.size(); ++i)
    {
      double x_diff = vcl_abs(el1[i]->pt.x() - el2[i]->pt.x());
      double y_diff = vcl_abs(el1[i]->pt.y() - el2[i]->pt.y());
      double t1 = el1[i]->tangent;
      double t2 = el2[i]->tangent;
      double t_diff = vcl_abs(t1 - t2);
      t1 = t1 > vnl_math::pi ? t1 - 2.0 * vnl_math::pi : t1;
      t2 = t2 > vnl_math::pi ? t2 - 2.0 * vnl_math::pi : t2;
      t_diff = vcl_min(t_diff, t1 - t2);
      if(x_diff > tolerance || y_diff > tolerance || t_diff > tolerance)
      {
        e_status = false;
        break;
      }
    }
  }
  TEST("Graphical Model Contour Merge: contours #edgels", s_status, true);
  TEST("Graphical Model Contour Merge: contours (x,y,dir)", e_status, true);
}


MAIN( test_graphical_model_contour_merge )
{ 
  START ("Test dbdet contour breaker");
  graphical_model_contour_merge_test();
  SUMMARY();
}
