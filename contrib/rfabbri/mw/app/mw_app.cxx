//:
//\file
//\brief multiview interactive application
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Sat Apr 16 22:49:07 EDT 2005
//

#include <vcl_sstream.h>
#include <vcl_iostream.h>

#include <vul/vul_file.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>

#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_cross.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>
#include <dvpgl/pro/dvpgl_camera_storage_sptr.h>
#include <dbsol/algo/dbsol_geno.h>
#include <dbgl/algo/dbgl_eulerspiral.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>

#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbgl/algo/dbgl_curve_smoothing.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_selector_tableau.h>

#include <dbsol/dbsol_file_io.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>

#include <bvis1/bvis1_manager.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <bvis1/bvis1_view_tableau.h>
#include <dbdif/dbdif_rig.h>
#include <dbdif/dbdif_analytic.h>
#include <mw/mw_util.h>
#include <mw/dbmcs_view_set.h>
#include <mw/algo/mw_data.h>
#include <mw/app/differential-estimates/mw_gradient_descent.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>
#include "mw_app.h"

//: loads list of edge maps into all frames. Doesn't add frames.
static void
load_edgemaps_into_frames(const vcl_vector<vcl_string> &edgemaps_fnames);

//: loads list of curve frags into all frames. Doesn't add frames.
static void
load_curve_frags_into_frames(const vcl_vector<vcl_string> &cfrags_fnames);

static void load_cams_into_frames(
    const vcl_vector<vcl_string> &cams_fnames, 
    mw_util::camera_file_type cam_type);

static void
load_imgs_into_frames(const vcl_vector<vcl_string> &imgs_fnames);

//: example of callback for menu "Examples". Insert any application commands
//here as a shortcut.
void 
mw_misc()
{
  //---- Just testing 

  vcl_vector<vgl_point_3d<double> > pts;
  vcl_string fname("/home/rfabbri/work/ming-signal.dat");
  bool retval = myreadv(fname,pts);
  assert(retval);

  dbgl_curve_shorten_3d(pts, 1, 1);

  vcl_string out_name("/home/rfabbri/work/ming-signal-smoothed.dat");
  retval =mywritev(out_name,pts);
  if (!retval)
    vcl_cerr << "Could not write file " << out_name << vcl_endl;
  assert(retval);
}

#include <vgui/vgui_find.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>

// Sets my current working repository. I call it from the menu Apps, but when I
// work exhaustively on a single dataset I could also place a call to this in
// mw_stereo_app's main or some place to load at startup.
void 
mw_load_current_working_repository()
{
  // Set this to whatever I'm working with right now.
  mw_load_current_working_repository_curve_tracing_tool();
}

//: Loads the data of an mcs_stereo_instance into frames.
void 
mw_load_mcs_instance()
{
  vcl_string data_dir="./";
//  mw_util::camera_file_type cam_type = mw_util::MW_INTRINSIC_EXTRINSIC;
  mw_util::camera_file_type cam_type = mw_util::MW_3X4;
  unsigned instance_id = 0;

  mw_curve_stereo_data_path dpath;
  bool retval = 
    mw_data::read_frame_data_list_txt(data_dir, &dpath, cam_type);
  if (!retval) return;
  vcl_cout << "Dpath:\n" << dpath << vcl_endl;
  dbmcs_stereo_instance_views all_instances_;

  retval = dbmcs_view_set::read_txt(
      data_dir + vcl_string("/mcs_stereo_instances.txt"), 
      &all_instances_);
  assert(retval == true);
  vcl_cout << "Instances:\n" << all_instances_ << vcl_endl;

  dbmcs_stereo_instance_views one_instance_;
  one_instance_.add_instance(all_instances_.instance(instance_id));
  vcl_cout << "Loading instance[" <<  instance_id << "]: " 
    << one_instance_ << vcl_endl;

  vcl_vector<vcl_string> edgemaps_fnames, cfrags_fnames, cams_fnames, imgs_fnames;

  dbmcs_stereo_views_sptr inst = one_instance_.instance(0);

  edgemaps_fnames.push_back(dpath[inst->stereo0()].edg_full_path());
  edgemaps_fnames.push_back(dpath[inst->stereo1()].edg_full_path());

  cfrags_fnames.push_back(dpath[inst->stereo0()].frag_full_path());
  cfrags_fnames.push_back(dpath[inst->stereo1()].frag_full_path());

  cams_fnames.push_back(dpath[inst->stereo0()].cam_full_path());
  cams_fnames.push_back(dpath[inst->stereo1()].cam_full_path());

  imgs_fnames.push_back(dpath[inst->stereo0()].img_full_path());
  imgs_fnames.push_back(dpath[inst->stereo1()].img_full_path());

  MANAGER->add_new_frame();

  for (unsigned i=0; i < inst->num_confirmation_views(); ++i) {
    unsigned k = inst->confirmation_view(i);
    edgemaps_fnames.push_back(dpath[k].edg_full_path());
    cfrags_fnames.push_back(dpath[k].frag_full_path());
    cams_fnames.push_back(dpath[k].cam_full_path());
    imgs_fnames.push_back(dpath[k].img_full_path());

    MANAGER->add_new_frame();
  }

  MANAGER->first_frame();

  load_imgs_into_frames(imgs_fnames);
  load_edgemaps_into_frames(edgemaps_fnames);
  load_curve_frags_into_frames(cfrags_fnames);
  load_cams_into_frames(cams_fnames, cam_type);
  /* todo
  compute_dt_label(edgemaps_fnames);
  prune
  */
  MANAGER->first_frame();
  MANAGER->post_redraw();

  // TODO dialog option to not load repos.
  vgui_dialog io_dialog("initializer options" );

  io_dialog.message("select initialization options:");

  // create 3 views
  int ngrids_wanted = 3;

  io_dialog.field("#views", ngrids_wanted);
  if (!io_dialog.ask()) {
    vcl_cout << "Canceled\n";
    return;
  }
  vcl_cout << "Desired #views: " << ngrids_wanted << vcl_endl;

  // add grid if only one present
  vgui_grid_tableau_sptr pgrid;
  pgrid.vertical_cast(vgui_find_below_by_type_name(MANAGER,"vgui_grid_tableau"));

  unsigned ngrids = 0;

  vcl_cout << "Initial ngrids: " << ngrids << vcl_endl;
  for (int i=0; i+1 < ngrids_wanted; ++i) {
    ngrids += 1;
    MANAGER->add_new_view(ngrids, false);
  }
  MANAGER->display_current_frame();

  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();
  // make curves active
  for (unsigned v=0; v < views.size(); ++v) {
    vgui_selector_tableau &selector = *(views[v]->selector());
    selector.set_active("original_cfrags");
    selector.active_to_top();
  }

  MANAGER->display_current_frame();
  MANAGER->post_redraw();
}

// Loads my working repository and sets visibility and active members so that I
// can work with the curve tracing tools immediately.
void 
mw_load_current_working_repository_curve_tracing_tool()
{
  // load a working repository
  vcl_vector<vcl_string> edgemaps_fnames;

  /*
  vcl_string data_dir = "./Downtown/video/";

  vcl_string rep_fname
    = "instance1/rep/current.rep";
    */

//  vcl_string data_dir = "/home/rfabbri/lib/data/right-dome-subset-local/";
//  vcl_string data_dir = "/home/rfabbri/lib/data/dino/";
//  vcl_string data_dir = "/home/rfabbri/lib/data/capitol-mcs-work/";

  vcl_string data_dir = "./";

  vcl_string rep_fname
    = "rep/current.rep";

  MANAGER->load_repository_from_file(data_dir + rep_fname);

  mw_util::camera_file_type cam_type = mw_util::MW_INTRINSIC_EXTRINSIC;
  unsigned instance_id = 0;

  mw_curve_stereo_data_path dpath;
  bool retval = 
    mw_data::read_frame_data_list_txt(data_dir, &dpath, cam_type);
  if (!retval) return;
  vcl_cout << "Dpath:\n" << dpath << vcl_endl;
  dbmcs_stereo_instance_views all_instances_;

  retval = dbmcs_view_set::read_txt(
      data_dir + vcl_string("/mcs_stereo_instances.txt"), 
      &all_instances_);
  assert(retval == true);
  vcl_cout << "Instances:\n" << all_instances_ << vcl_endl;

  dbmcs_stereo_instance_views one_instance_;
  one_instance_.add_instance(all_instances_.instance(instance_id));
  vcl_cout << "Loading instance[" <<  instance_id << "]: " 
    << one_instance_ << vcl_endl;
  dbmcs_stereo_views_sptr inst = one_instance_.instance(0);

  // Fill edgemaps with instances
  edgemaps_fnames.push_back(dpath[inst->stereo0()].edg_full_path());
  edgemaps_fnames.push_back(dpath[inst->stereo1()].edg_full_path());
  for (unsigned i=0; i < inst->num_confirmation_views(); ++i) {
    unsigned k = inst->confirmation_view(i);
    edgemaps_fnames.push_back(dpath[k].edg_full_path());
  }
  
  // TODO dialog option to not load repos.
  vgui_dialog io_dialog("initializer options");

  io_dialog.message("select initialization options:");

  // create 3 views
  int ngrids_wanted = 3;

  io_dialog.field("#views", ngrids_wanted);
  if (!io_dialog.ask()) {
    vcl_cout << "Canceled\n";
    return;
  }
  vcl_cout << "Desired #views: " << ngrids_wanted << vcl_endl;

  // add grid if only one present
  vgui_grid_tableau_sptr pgrid;
  pgrid.vertical_cast(vgui_find_below_by_type_name(MANAGER,"vgui_grid_tableau"));

  unsigned ngrids = 0;

  vcl_cout << "Initial ngrids: " << ngrids << vcl_endl;
  for (int i=0; i+1 < ngrids_wanted; ++i) {
    ngrids += 1;
    MANAGER->add_new_view(ngrids, false);
  }

  MANAGER->display_current_frame();

  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();

  assert (edgemaps_fnames.size()==0 || views.size() <= edgemaps_fnames.size());

  load_edgemaps_into_frames(edgemaps_fnames);

  // make just a certain vsol visible, if any
  // 
  // Search for storage named broken_vsols. If any, make it active.

  for (unsigned v=0; v < views.size(); ++v) {
    vgui_selector_tableau &selector = *(views[v]->selector());

    // activate broken_vsols[i] with the largest possible i

    vcl_string active_name;
    for (unsigned i=50; i != static_cast<unsigned>(-1); --i) {
      vcl_ostringstream s;
      if (i != 0) {
        s << i;
      }

      vcl_string putative_name = vcl_string("broken_vsols") + s.str();
      vgui_tableau_sptr t = selector.get_tableau(putative_name);
      if (t) {
        active_name = putative_name;
        break;
      }
    }

    selector.set_active(active_name);
    selector.active_to_top();

    if (!selector.is_visible("mw_curve_tracer"))
      selector.toggle("mw_curve_tracer");

    // unset_visible all useless tableaux
    for (unsigned i=0; i < selector.child_names().size(); ++i) {
      vcl_string name = selector.child_names()[i];
      if (v==0 || v == 1) {
        if (name != active_name && name != "mw_curve_tracer" &&
            name != "original_image" && 
  //         selector.get_tableau(name)->type_name() == "bgui_vsol2D_tableau" &&
           selector.is_visible(name)) {
          selector.toggle(name);
        }
      } else {
        if (name != "mw_curve_tracer" 
            && name != "edgemap113" 
            && name != "edgemap112"
            && name != "edgemap114"
            && name != "edgemap115"
            && name != "edgemap116"
            && name != "edgeimage0")
          selector.toggle(name);
      }
    }
  }
  

  MANAGER->first_frame();
  MANAGER->post_redraw();

  // Prune curves by length;
  // TODO
//  for (unsigned v=0; v < views.size(); ++v) {
//  }
}

//: loads list of edge maps into all frames. Doesn't add frames.
void
load_edgemaps_into_frames(const vcl_vector<vcl_string> &edgemaps_fnames)
{
  for (unsigned v=0; v < edgemaps_fnames.size(); ++v) {
    vcl_cout << "Reading " << edgemaps_fnames[v] << vcl_endl;
    bool bSubPixel = true;
    double scale=1.0;
    dbdet_edgemap_sptr em;

    bool retval = dbdet_load_edg(
        edgemaps_fnames[v],
        bSubPixel,
        scale,
        em);

    if (!retval) {
      vcl_cerr << "Could not open edge file " << edgemaps_fnames[v] << vcl_endl;
      return;
    }
    vcl_cout << "N edgels: " << em->num_edgels() << vcl_endl;

    dbdet_edgemap_storage_sptr es = dbdet_edgemap_storage_new();
    es->set_edgemap(em);
    es->set_name("edgemap116");

    MANAGER->repository()->store_data(es);
    MANAGER->add_to_display(es);
    MANAGER->next_frame();
  }
  MANAGER->first_frame();
}

void
load_imgs_into_frames(const vcl_vector<vcl_string> &imgs_fnames)
{
  for (unsigned v=0; v < imgs_fnames.size(); ++v) {

    vcl_string image_filename = imgs_fnames[v];

    vil_image_resource_sptr loaded_image 
      = vil_load_image_resource( image_filename.c_str() );
    if( !loaded_image ) {
      vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
      return;
    }
    vidpro1_image_storage_sptr is = vidpro1_image_storage_new();
    is->set_image(loaded_image);
    is->set_name("original_image");

    MANAGER->repository()->store_data(is);
    MANAGER->add_to_display(is);
    MANAGER->next_frame();
  }
  MANAGER->first_frame();
}

void
load_cams_into_frames(
    const vcl_vector<vcl_string> &cams_fnames, 
    mw_util::camera_file_type cam_type)
{
  for (unsigned v=0; v < cams_fnames.size(); ++v) {


    vpgl_perspective_camera<double> *pcam = new vpgl_perspective_camera<double>;
    vpgl_perspective_camera<double> &cam = *pcam;

    mw_util::read_cam_anytype(cams_fnames[v], cam_type, &cam);

    dvpgl_camera_storage_sptr 
       cam_storage = dvpgl_camera_storage_new();
    
    cam_storage->set_camera(pcam);
    cam_storage->set_name("my_vpgl_cam");

    MANAGER->repository()->store_data(cam_storage);
    MANAGER->next_frame();
  }
  MANAGER->first_frame();
}


void
load_curve_frags_into_frames(const vcl_vector<vcl_string> &cfrags_fnames)
{
  for (unsigned v=0; v < cfrags_fnames.size(); ++v) {
    vcl_vector< vsol_spatial_object_2d_sptr > contours;

    vcl_string ext = vul_file::extension(cfrags_fnames[v]);
    if (ext == ".vsl") {
      vsl_b_ifstream bp_in(cfrags_fnames[v].c_str());
      if (!bp_in) {
        vcl_cout << " Error opening file  " << cfrags_fnames[v] << vcl_endl;
        return;
      }

      vcl_cout << "Opened vsl file " << cfrags_fnames[v] <<  " for reading" << vcl_endl;

      vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
      output_vsol->b_read(bp_in);

      //: clone

      vidpro1_vsol2D_storage_sptr output_vsol_2;
      output_vsol_2.vertical_cast(output_vsol->clone());

      output_vsol_2->set_frame(-10); //:< means its not in rep
      // try to copy by hand if doesnt work

      MANAGER->repository()->store_data(output_vsol_2);
      MANAGER->add_to_display(output_vsol);
    } else {
      bool retval = dbsol_load_cem(contours, cfrags_fnames[v]);
      if (!retval) {
        vcl_cerr << "Could not open frag file " << cfrags_fnames[v] << vcl_endl;
        return;
      }
      vcl_cout << "N curves: " << contours.size() << vcl_endl;

      vidpro1_vsol2D_storage_sptr cs = vidpro1_vsol2D_storage_new();
      cs->add_objects(contours, cfrags_fnames[v]);
      cs->set_name("original_cfrags");

      MANAGER->repository()->store_data(cs);
      MANAGER->add_to_display(cs);
    }
    MANAGER->next_frame();
  }
  MANAGER->first_frame();
}

//: the following seem wrong cause focal length has not been considerd.
void
test_point_reconstruct()
{
   vgl_homg_point_2d<double> p1,p2,p3;
 
   vcl_string fname1("curr/p1010049.jpg");
   vcl_string fname2("curr/p1010053.jpg");
   vcl_string fname3("curr/p1010069.jpg");
   // Reading cameras
   vpgl_perspective_camera <double> Pr1,Pr2,Pr3;
 
   if (!read_cam(fname1,fname2,&Pr1,&Pr2)) {
      vcl_cerr << "epip_interactive: error reading cam\n";
      return;
   }
 
   if (!read_cam(fname3,&Pr3)) {
      vcl_cerr << "epip_interactive: error reading cam\n";
      return;
   }
 
   vpgl_fundamental_matrix<double> f13(Pr1,Pr3);
   vpgl_fundamental_matrix<double> f23(Pr2,Pr3);
 
 
   // origin of board
   //  p1.set(1395,211);
   //  p2.set(1811,319);
   // 1st corner to right of orig.
   p1.set(1482,217);
   p2.set(1895,307);
 
   p3 = mw_epipolar_point_transfer(p1,p2,f13,f23);
   vcl_cout << "P3: " << p3.x()/p3.w() << "  " << p3.y()/p3.w() <<  vcl_endl;
 
 
   // Now do by least squares and compare:
 
   vnl_matrix_fixed<double,4,4> Rc;
   vnl_matrix_fixed<double,3,3> Rct;
   vnl_matrix_fixed<double,3,3> Rcs;
 
   Pr1.get_rotation().as_h_matrix_3d().get(&Rc);
 
   vcl_cout << "Rotation:\n" << Rc;
 
   // transpose and reduce to 3x3:
   for (unsigned i=0; i<3; ++i) for (unsigned j=0; j<3; ++j) {
      Rcs(i,j) = Rc(i,j);
      Rct(i,j) = Rc(j,i);
   }
   vcl_cout << "Rotation transposed 3x3 :\n" << Rct;
 
 
   //
   // Camera 1 : write things in 3D common coordinate system
   //
 
   vgl_point_3d<double> c1_pt = Pr1.get_camera_center();
 
   mw_vector_3d 
      c1(c1_pt.x(),c1_pt.y(),c1_pt.z()),
      //: world coordinates
      e11, e12, e13, gama1, F1,
      //: camera coordinates
      e11_cam(1,0,0), e12_cam(0,1,0), e13_cam(0,0,1);
 
 
   e11 = Rct*e11_cam;
   vcl_cout << "E11:\n" << e11  << vcl_endl;
   e12 = Rct*e12_cam;
   vcl_cout << "E12:\n" << e12  << vcl_endl;
   e13 = F1 = Rct*e13_cam;
   vcl_cout << "E13:\n" << e13  << vcl_endl;
 
   double x_scale, y_scale, u, v;
 
   x_scale = Pr1.get_calibration().x_scale();
   y_scale = Pr1.get_calibration().y_scale();
 
   u = p1.x();
   v = p1.y();
 
   vgl_point_2d<double> uv0 = Pr1.get_calibration().principal_point();
 
   gama1 = ((u - uv0.x())/x_scale)*e11 + ((v - uv0.y())/y_scale)*e12  + e13;
 
   // =========
   //
   //
   
   vgl_homg_point_3d<double> paux1(gama1(0),gama1(1),gama1(2));
   vgl_homg_point_3d<double> paux2(gama1(0)+c1(0),gama1(1)+c1(1),gama1(2)+c1(2));
   vgl_homg_point_3d<double> w_origin(0,0,0);
   vgl_homg_line_3d_2_points<double> ray1(paux1,paux2);
 
 
   double d=vgl_homg_operators_3d<double>::perp_dist_squared(ray1,w_origin);
   
   // test distance of (0,0,0) to line
   vcl_cout << "Distance: " << d << vcl_endl;
 
   //
   // Camera 2 : translate things to 3D common coordinate system
   //
   vnl_matrix_fixed<double,3,3> Rct2;
   vnl_matrix_fixed<double,4,4> Rc2;
 
   Pr2.get_rotation().as_h_matrix_3d().get(&Rc2);
 
   vcl_cout << "Rotation2:\n" << Rc2;
 
   // transpose and reduce to 3x3:
   for (unsigned i=0; i<3; ++i) for (unsigned j=0; j<3; ++j)
      Rct2(i,j) = Rc2(j,i);
   vcl_cout << "Rotation transposed 3x3  2 :\n" << Rct2;
 
   vgl_point_3d<double> c2_pt = Pr2.get_camera_center();
 
   mw_vector_3d 
      c2(c2_pt.x(),c2_pt.y(),c2_pt.z()),
      //: world coordinates
      e21, e22, e23, gama2, F2,
      //: camera coordinates
      e21_cam(1,0,0), e22_cam(0,1,0), e23_cam(0,0,1);
 
   e21 = Rct2*e21_cam;
   vcl_cout << "E11:\n" << e21  << vcl_endl;
   e22 = Rct2*e22_cam;
   vcl_cout << "E12:\n" << e22  << vcl_endl;
   e23 = F2 = Rct2*e23_cam;
   vcl_cout << "E13:\n" << e23  << vcl_endl;
 
   x_scale = Pr2.get_calibration().x_scale();
   y_scale = Pr2.get_calibration().y_scale();
   u = p2.x();
   v = p2.y();
 
 
   uv0 = Pr2.get_calibration().principal_point();
 
   gama2 = ((u - uv0.x())/x_scale)*e21 + ((v - uv0.y())/y_scale)*e22  + e23;
 
   // =========
   //
   //
   
   vgl_homg_point_3d<double> paux12(gama2(0),gama2(1),gama2(2));
   vgl_homg_point_3d<double> paux22(gama2(0)+c2(0),gama2(1)+c2(1),gama2(2)+c2(2));
   vgl_homg_line_3d_2_points<double> ray2(paux12,paux22);
 
   d=vgl_homg_operators_3d<double>::perp_dist_squared(ray2,w_origin);
   
   // test distance of (0,0,0) to line
   vcl_cout << "Distance 2: " << d << vcl_endl;
 
   vcl_cout << "====================================" << vcl_endl;
 
   vcl_cout << "Gama1:\n" << gama1 << "\nGama2: \n" <<  gama2 << vcl_endl
            << "c1:\n " << c1 << "\nc2:\n" << c2 << vcl_endl;
 
   // Least squares reconstr.
   vnl_matrix_fixed<double,3,2> A;
 
   for (unsigned i=0; i<3; ++i)
      A(i,0) = gama1(i);
 
   for (unsigned i=0; i<3; ++i)
      A(i,1) = -gama2(i);
 
   vnl_svd<double> svd(A);
   vnl_vector<double> lambda = svd.solve(c2-c1);
 
   vcl_cout << "Lambda:\n" << lambda << vcl_endl;
   vcl_cout << "Norm: " << svd.norm() << vcl_endl
      << "Mynorm2:" << (A*lambda +c1 - c2).two_norm() << vcl_endl;
 
 
   mw_vector_3d Cpt_v = c1 + lambda(0)*gama1;
   vgl_homg_point_3d<double> Cpt(Cpt_v(0), Cpt_v(1), Cpt_v(2));
   vcl_cout << "Reconstructed point: " << Cpt << vcl_endl;
 
   // Project to 3rd:
   vgl_homg_point_2d<double> p3_lsqr; 
 
   p3_lsqr = Pr3.project(Cpt);
   vcl_cout << "Tranferred origin to 3rd view: \n" 
      "Least squares: " << p3_lsqr.x()/p3_lsqr.w() << " " << p3_lsqr.y()/p3_lsqr.w() << vcl_endl
   << "Epip Transfer: " << p3.x()/p3.w() << "  " << p3.y()/p3.w() <<  vcl_endl;
 
   //=========== Tangents
 
 
   mw_vector_3d t1,
      T(0,1,0); //:< test 
   mw_vector_3d t1_cam;
   mw_vector_2d t1_img;
 
   t1 = T - dot_product(T, F1)*gama1;
   vcl_cout << "t1: " << t1 << vcl_endl;
 
   t1_cam = Rcs * t1; // 3rd component now 0
 
   vcl_cout << "t1_cam: " << t1_cam << vcl_endl;
   vcl_cout << "x,y scales: " << x_scale << " " << y_scale << vcl_endl;
 

   t1_img[0] = t1_cam[0]*x_scale;
   t1_img[1] = t1_cam[1]*y_scale;
   t1_img.normalize();
 
   vcl_cout << "t1_img: " << t1_img << vcl_endl;
 
   vcl_cout << "p1: " << p1 << vcl_endl;
 
   vcl_cout << "p1 + 88t1: \n" << p1.x()/p1.w() + 88*t1_img[0] << " " << p1.y()/p1.w() + 88*t1_img[1] << vcl_endl;
 
   vcl_cout << "================= Project point test: =======================" << vcl_endl;
 
   vgl_homg_point_3d<double> orig(0,0,0), other(53.3,0,0);
   vgl_homg_point_2d<double> aux;
 
   aux = Pr1.project(orig);
   vcl_cout << "Origin in retina 1: " << aux.x()/aux.w() << "  " <<   aux.y()/aux.w() << vcl_endl;
 
   aux = Pr1.project(other);
   vcl_cout << "Other in retina 1: " << aux.x()/aux.w() << "  " <<   aux.y()/aux.w() << vcl_endl;
 
 
   vcl_cout << "\n\n\n";
   vcl_cout << "================= Tangent reconstruction: =======================" << vcl_endl;
 
   // Camera 1:
   mw_vector_3d t1_cam_bkwd;
   mw_vector_3d t1_world_bkwd;
 
   t1_cam_bkwd[0] = t1_img[0]/x_scale;   //:< take off streching;
   t1_cam_bkwd[1] = t1_img[1]/y_scale;
   t1_cam_bkwd[2] = 0;
   t1_cam_bkwd.normalize();
 
   t1_world_bkwd = Rct*t1_cam_bkwd;
 
   // Camera 2:
   mw_vector_3d t2_cam_bkwd;
   mw_vector_3d t2_world_bkwd;
   mw_vector_3d T_rec;
 
   t2_cam_bkwd[0] = (1982 - p2.x())/x_scale;   //:< determined by the eye
   t2_cam_bkwd[1] = (295 - p2.y())/y_scale;
   t2_cam_bkwd[2] = 0;
   t2_cam_bkwd.normalize();
 
   t2_world_bkwd = Rct2*t2_cam_bkwd;
 
   vcl_cout << "Test t1 dot F1 zero: " << dot_product(t1_world_bkwd,F1) << vcl_endl << vcl_endl;
   vcl_cout << "Test t1 dot F2 zero: " << dot_product(t2_world_bkwd,F2) << vcl_endl << vcl_endl;
 
   T_rec = vnl_cross_3d( vnl_cross_3d(t1_world_bkwd,gama1), vnl_cross_3d(t2_world_bkwd,gama2) );
 
   vcl_cout << "T reconstructed: \n" << T_rec << vcl_endl;
}


// projects a parametric curve into a cam
void
example_project()
{
   // Sample 3D circle
   double t,theta, step=1;
   
   double sz=360/step;
   vcl_vector< vgl_homg_point_3d<double> > Ps;
   Ps.reserve((int) sz);
      
   for (theta=0; theta<360-step; theta+=step) {
      t = (theta/180.0)*vnl_math::pi;
      vgl_homg_point_3d<double> pt(4*cos(t),4*sin(t),2000);
      Ps.push_back(pt);       
   }

   // project then output
   vnl_double_3x3 m;
   m[0][0]=1000;m[0][1]=0;   m[0][2]=0;
   m[1][0]=0;   m[1][1]=1000;m[1][2]=0;
   m[2][0]=0;   m[2][1]=0;   m[2][2]=1;
 
   vpgl_calibration_matrix<double> K(m);

   vgl_rotation_3d<double> rot; // identity
   vgl_homg_point_3d<double> center(0,0,0.0);
   vpgl_perspective_camera<double> P(K, center, rot);

   for (unsigned i=0; i<Ps.size(); ++i) {
      vgl_point_2d<double> gama = P.project(Ps[i]);
      vcl_cout << gama << vcl_endl;
   }


   vcl_cout << "Done!" << vcl_endl;
}

void
example_project_and_reconstruct()
{
}

/* only here for illustrating history.
//: projects a parametric curve into two cams then recover it
void
example_project_and_reconstruct()
{
  vcl_ofstream fC_orig, fgama1,fgama2,fC_rec, ft;

  // Sample 3D circle
  double theta=0, step=0.01;
  
  double sz=360.0/step;
  vcl_vector< vgl_homg_point_3d<double> > Ps;
  vcl_vector<double> t;
  Ps.reserve((int) sz);
  t.reserve((int) sz);
     
  fC_orig.open("/tmp/C_orig.dat");
  ft.open("/tmp/t.dat");

  for (unsigned i=0; theta<360-step; theta+=step, ++i) {
     t.push_back((theta/180.0)*vnl_math::pi);
     double x = 500*cos(t[i]);
     double y = 500*sin(t[i]);
     double z = 10;
     vgl_homg_point_3d<double> pt(x,y,10);
     Ps.push_back(pt);       
     ft << t[i] << vcl_endl;
     fC_orig << x << " " << y << " " << z << vcl_endl;
  }

  ft.close(); fC_orig.close();

  dbdif_rig rig;
  rig.read("curr2/255-crop.jpg","curr2/261-crop.jpg");


  vcl_vector <vsol_point_2d_sptr> gama1_img, gama2_img;

  fgama1.open("/tmp/gama1.dat");
  fgama2.open("/tmp/gama2.dat");
  gama1_img.reserve(Ps.size());
  gama2_img.reserve(Ps.size());
  for (unsigned i=0; i<Ps.size(); ++i) {
     vgl_point_2d<double> p1 = rig.cam[0].Pr_.project(Ps[i]);
     vgl_point_2d<double> p2 = rig.cam[1].Pr_.project(Ps[i]);
     gama1_img.push_back(new vsol_point_2d(p1));
     gama2_img.push_back(new vsol_point_2d(p2));
     fgama1 << p1.x() << " " << p1.y() << vcl_endl;
     fgama2 << p2.x() << " " << p2.y() << vcl_endl;
  }
  fgama1.close();
  fgama2.close();
  vcl_vector<mw_vector_3d> C_rec;
  rig.reconstruct_3d_curve(&C_rec,gama1_img,gama2_img);

  fC_rec.open("/tmp/C_rec.dat");
  for (unsigned i=0; i< C_rec.size(); ++i) {
    fC_rec << C_rec[i][0] << " " << C_rec[i][1] << " " << C_rec[i][2] << vcl_endl;
  }
  fC_rec.close();


  vcl_cout << "Done!" << vcl_endl;
}
*/

// Calls process on a filename, without dialog
void 
call_show_contours_process(char *fname)
{
   bpro1_process_sptr pro= MANAGER->process_manager()->get_process_by_name("Show Contours");

   vcl_set<bpro1_storage_sptr> modified;

   // set the parameter
   vcl_string sfname(fname);
   bpro1_filepath fp(sfname);
   vcl_string pname("-image_filename");
   pro->parameters()->set_value(pname,fp);
   vcl_cout << sfname << vcl_endl;

   vcl_vector<vcl_string> out_name(pro->output_names());
   out_name.push_back("image0");
   pro->set_output_names(out_name);

   //now run the process
   MANAGER->process_manager()->run_process_on_current_frame(pro, &modified);

   // update the display for any modified storage objects
   for ( vcl_set<bpro1_storage_sptr>::iterator itr = modified.begin();
         itr != modified.end(); ++itr ) {
     bvis1_manager::instance()->add_to_display(*itr);
   }

   bvis1_manager::instance()->display_current_frame();
}



void
test_point_reconstruct_rig()
{
}
/* Obsolete TODO see if can remove this
void
test_point_reconstruct_rig()
{
 
   vcl_string fname1("curr/p1010049.jpg");
   vcl_string fname2("curr/p1010053.jpg");

   dbdif_rig rig;
   rig.read(fname1,fname2);
   
   // origin of board
   //  p1.set(1395,211);
   //  p2.set(1811,319);
   // 1st corner to right of orig.
   vgl_homg_point_2d<double> p1,p2;

   p1.set(1482,217);
   p2.set(1895,307);

   mw_vector_3d gama1, gama2;

   rig.cam[0].get_gama(p1.x(),p1.y(),&gama1);
   rig.cam[1].get_gama(p2.x(),p2.y(),&gama2);

   
   // Least squares reconstr
   mw_vector_3d Cpt_v;
   rig.reconstruct_point_lsqr(gama1,gama2,&Cpt_v);

   vcl_cout << "Reconstructed point: " << Cpt_v << vcl_endl;


   // Forward tangent projection


   mw_vector_3d t1,
      T(0,1,0); //:< test 

   mw_vector_2d t1_img;

   rig.cam[0].project_t(T,gama1,&t1);
   
   double x,y;
   x = y = 0;//:< not used

   rig.cam[0].world_to_img_vector(t1, &t1_img[0],&t1_img[1]);

   vcl_cout << "t1_img: " << t1_img << vcl_endl;
 
   vcl_cout << "p1: " << p1 << vcl_endl;
 
   vcl_cout << "p1 + 88t1: \n" << p1.x()/p1.w() + 88*t1_img[0] << " " << p1.y()/p1.w() + 88*t1_img[1] << vcl_endl;
   

   // Tangent reconstruction
   vgl_vector_2d<double> 
      t2vgl(1982 - p2.x(), 295 - p2.y()),
      t1vgl(t1[0],t1[1]);

   mw_vector_3d T_rec;

   rig.reconstruct_tangent(gama1,gama2,t1vgl,t2vgl, &T_rec);
   T_rec.normalize();

   vcl_cout << "T reconstructed: \n" << T_rec << vcl_endl;
}
*/

void
mw_load_cvmatch_input()
{
   // Repository loads curves and matrices
   vcl_string fname_rep("c.rep");
}

void
test_k_formula_circle_old()
{
}
/* TODO: remove this if possible.
 *
//: defines a circle in world then project it and compute its curvature &
//derivative from the 3D measures.
void
test_k_formula_circle_old()
{
  vcl_ofstream 
    fC_orig, fgama1, fgama2, fgama1_img, fgama2_img, fC_rec, ft, ftt, fn, fk, fkdot,
    frec, frec_T, frec_N, frec_B, frec_K, frec_Kdot, frec_Tau, faux;

  // Sample 3D circle
  double theta=0, step=0.01;
  
  double sz=360.0/step;
  vcl_vector< vgl_homg_point_3d<double> > Ps;
  vcl_vector<double> t;
  Ps.reserve((int) sz);
  t.reserve((int) sz);
     
  fC_orig.open("/tmp/C_orig.dat");
  ft.open("/tmp/theta.dat");

//  double r=4*53.3;
  double r=2000;
//  double r=0.5;
  for (unsigned i=0; theta<360-step; theta+=step, ++i) {
     t.push_back((theta/180.0)*vnl_math::pi);
     double x = r*cos(t[i]);
     double y = r*sin(t[i]);
     double z = 0;
     vgl_homg_point_3d<double> pt(x,y,z);
     Ps.push_back(pt);       
     ft << t[i] << vcl_endl;
     fC_orig << x << " " << y << " " << z << vcl_endl;
  }

  ft.close(); fC_orig.close();

  dbdif_rig rig;
//  rig.read("curr2/255.jpg","curr2/261.jpg");
  rig.read("curr2/255.jpg","curr2/235.jpg");


  // Project all points ----------
  vcl_vector <vsol_point_2d_sptr> gama1_img, gama2_img;
  vcl_vector <mw_vector_3d> gama1,gama2;
  dbdif_3rd_order_point_2d frame1,frame2;

  // FILES for debugging
  fgama1.open("/tmp/gama1.dat",vcl_ios::out | vcl_ios::binary);
  fgama2.open("/tmp/gama2.dat",vcl_ios::out | vcl_ios::binary);
  fgama1_img.open("/tmp/gama1_img.txt");
  fgama2_img.open("/tmp/gama2_img.txt");
  fC_orig.open("/tmp/C_orig2.txt");
  fk.open("/tmp/k.dat",vcl_ios::out | vcl_ios::binary);
  fkdot.open("/tmp/kdot.dat",vcl_ios::out | vcl_ios::binary);
  ft.open("/tmp/t.dat",vcl_ios::out | vcl_ios::binary);
  ftt.open("/tmp/ttext.txt");
  fn.open("/tmp/n.dat",vcl_ios::out | vcl_ios::binary);
  faux.open("/tmp/aux.dat",vcl_ios::out | vcl_ios::binary);

  frec.open("/tmp/rec.dat",vcl_ios::out | vcl_ios::binary);
  frec_T.open("/tmp/rec_T.dat",vcl_ios::out | vcl_ios::binary);
  frec_N.open("/tmp/rec_N.dat",vcl_ios::out | vcl_ios::binary);
  frec_B.open("/tmp/rec_B.dat",vcl_ios::out | vcl_ios::binary);
  frec_K.open("/tmp/rec_K.dat",vcl_ios::out | vcl_ios::binary);
  frec_Kdot.open("/tmp/rec_Kdot.dat",vcl_ios::out | vcl_ios::binary);
  frec_Tau.open("/tmp/rec_Tau.dat",vcl_ios::out | vcl_ios::binary);

  gama1_img.reserve(Ps.size()); gama2_img.reserve(Ps.size()); gama1.reserve(Ps.size()); gama2.reserve(Ps.size());

  for (unsigned i=0; i<Ps.size(); ++i) {
     vgl_point_2d<double> p1 = rig.cam[0].Pr_.project(Ps[i]);
     vgl_point_2d<double> p2 = rig.cam[1].Pr_.project(Ps[i]);
     gama1_img.push_back(new vsol_point_2d(p1));
     gama2_img.push_back(new vsol_point_2d(p2));

     mw_vector_3d Pt_inhomg;
     Pt_inhomg[0] = Ps[i].x()/Ps[i].w();
     Pt_inhomg[1] = Ps[i].y()/Ps[i].w();
     Pt_inhomg[2] = Ps[i].z()/Ps[i].w();

     gama1.push_back(rig.cam[0].project(Pt_inhomg - rig.cam[0].c));
     gama2.push_back(rig.cam[1].project(Pt_inhomg - rig.cam[1].c));

     fC_orig << rig.cam[0].F << " " << rig.cam[0].c << vcl_endl;

     fgama1_img << p1.x() << " " << p1.y() << vcl_endl;
     fgama2_img << p2.x() << " " << p2.y() << vcl_endl;

// LOSS IN PRECISION:    fgama1 << gama1[i] + rig.cam[0].c << vcl_endl;
//     fgama1 << gama1[i] << vcl_endl;
     fgama1.write((char *)((gama1[i] + rig.cam[0].c).data_block()),3*sizeof(double));

//     fgama2 << gama2[i] + rig.cam[1].c << vcl_endl;
     fgama2.write((char *)((gama2[i] + rig.cam[1].c).data_block()),3*sizeof(double));

     // Project curvatures of 3D points ----------
     dbdif_3rd_order_point_3d Frame;
     
     Frame.N = -Pt_inhomg;
     Frame.Gama = Pt_inhomg;

     Frame.N.normalize();

     Frame.T[0] =  Frame.N[1];
     Frame.T[1] = -Frame.N[0];
     Frame.T[2] =  Frame.N[2];

     Frame.B = vnl_cross_3d(Frame.T,Frame.N);
     Frame.K = 1/r;

     Frame.Kdot = 0;
     Frame.Tau  = 0;

//     rig.cam[0].project_k(Frame,&frame1);
//     rig.cam[1].project_k(Frame,&frame2);
     rig.cam[0].project_3rd(Frame,&frame1);
     rig.cam[1].project_3rd(Frame,&frame2);

//     frame1.kdot = frame1.kdot*r*sqrt(2)*(Frame.T - dot_product(Frame.T,rig.cam[0].F)*frame1.gama).two_norm()/ dot_product(Frame.Gama,rig.cam[0].F);

//     frame1.kdot /= dot_product(Frame.Gama,rig.cam[0].F);

     //fk << frame.k << vcl_endl;
     fk.write((char *)&(frame1.k),sizeof(double));
     ft.write((char *)(frame1.t.data_block()),3*sizeof(double));
     fkdot.write((char *)&(frame1.kdot),sizeof(double));
     fn.write((char *)(frame1.n.data_block()),3*sizeof(double));
     faux.write((char *)(Frame.N.data_block()),3*sizeof(double));
//     ftt << frame.t << vcl_endl;

     // ------- RECONSTRUCTION ------------------------------------------------ 

     dbdif_3rd_order_point_3d Frame_rec;
     
     rig.reconstruct_3rd_order(frame1,frame2,&Frame_rec);

     frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
     frec_T.write((char *)(Frame_rec.T.data_block()), 3*sizeof(double));
     frec_N.write((char *)(Frame_rec.N.data_block()), 3*sizeof(double));
     frec_B.write((char *)(Frame_rec.B.data_block()), 3*sizeof(double));
     frec_K.write((char *)&(Frame_rec.K),sizeof(double));
     frec_Kdot.write((char *)&(Frame_rec.Kdot),sizeof(double));
     frec_Tau.write((char *)&(Frame_rec.Tau),sizeof(double));
  }
  fgama1_img.close();
  fgama2_img.close();
  fgama1.close();
  fgama2.close();
  fC_orig.close();
  fk.close();
  faux.close();
  fkdot.close();
  ft.close();
  ftt.close();
  fn.close();
  frec.close();
  frec_T.close();
  frec_N.close();
  frec_B.close();
  frec_K.close();
}
*/

/* TODO: remove this if possible
//: defines a helix in world then project it and compute its curvature &
// derivative from the 3D measures.
void
test_k_formula()
{
  vcl_ofstream 
    fC_orig, fgama1, fgama2, fgama1_img, fgama2_img, fC_rec, ft, ftt, fn, fk, fkdot,
    frec, frec_T, frec_N, frec_B, frec_K, frec_Kdot, frec_Tau, faux;

  // Sample 3D curve
  double theta=0, step=0.06, range=3*360.0;
  
  double sz=range/step;
  vcl_vector< vgl_homg_point_3d<double> > Ps;
  vcl_vector<double> t;
  Ps.reserve((int) sz);
  t.reserve((int) sz);
     
  fC_orig.open("/tmp/C_orig.dat");
  ft.open("/tmp/theta.dat");

//  double r=4*53.3;
  double r=20, alpha=1/4.0;
//  double r=0.5;
  for (unsigned i=0; theta<range-step; theta+=step, ++i) {
     t.push_back((theta/180.0)*vnl_math::pi);
     double x = r*cos(t[i]);
     double y = r*sin(t[i]);
     double z = r*alpha*t[i];
     vgl_homg_point_3d<double> pt(x,y,z);
     Ps.push_back(pt);       
     ft << t[i] << vcl_endl;
     fC_orig << x << " " << y << " " << z << vcl_endl;
  }

  ft.close(); fC_orig.close();

  dbdif_rig rig;
//  rig.read("curr2/255.jpg","curr2/261.jpg");
  rig.read("curr2/255.jpg","curr2/235.jpg");


  // Project all points ----------
  vcl_vector <vsol_point_2d_sptr> gama1_img, gama2_img;
  vcl_vector <mw_vector_3d> gama1,gama2;
  dbdif_3rd_order_point_2d frame1,frame2;

  // FILES for debugging
  fgama1.open("/tmp/gama1.dat",vcl_ios::out | vcl_ios::binary);
  fgama2.open("/tmp/gama2.dat",vcl_ios::out | vcl_ios::binary);
  fgama1_img.open("/tmp/gama1_img.txt");
  fgama2_img.open("/tmp/gama2_img.txt");
  fC_orig.open("/tmp/C_orig2.txt");
  fk.open("/tmp/k.dat",vcl_ios::out | vcl_ios::binary);
  fkdot.open("/tmp/kdot.dat",vcl_ios::out | vcl_ios::binary);
  ft.open("/tmp/t.dat",vcl_ios::out | vcl_ios::binary);
  ftt.open("/tmp/ttext.txt");
  fn.open("/tmp/n.dat",vcl_ios::out | vcl_ios::binary);
  faux.open("/tmp/g1.dat",vcl_ios::out | vcl_ios::binary);

  frec.open("/tmp/rec.dat",vcl_ios::out | vcl_ios::binary);
  frec_T.open("/tmp/rec_T.dat",vcl_ios::out | vcl_ios::binary);
  frec_N.open("/tmp/rec_N.dat",vcl_ios::out | vcl_ios::binary);
  frec_B.open("/tmp/rec_B.dat",vcl_ios::out | vcl_ios::binary);
  frec_K.open("/tmp/rec_K.dat",vcl_ios::out | vcl_ios::binary);
  frec_Kdot.open("/tmp/rec_Kdot.dat",vcl_ios::out | vcl_ios::binary);
  frec_Tau.open("/tmp/rec_Tau.dat",vcl_ios::out | vcl_ios::binary);

  gama1_img.reserve(Ps.size()); gama2_img.reserve(Ps.size()); gama1.reserve(Ps.size()); gama2.reserve(Ps.size());

  for (unsigned i=0; i<Ps.size(); ++i) {
     vgl_point_2d<double> p1 = rig.cam[0].Pr_.project(Ps[i]);
     vgl_point_2d<double> p2 = rig.cam[1].Pr_.project(Ps[i]);
     gama1_img.push_back(new vsol_point_2d(p1));
     gama2_img.push_back(new vsol_point_2d(p2));

     mw_vector_3d Pt_inhomg;
     Pt_inhomg[0] = Ps[i].x()/Ps[i].w();
     Pt_inhomg[1] = Ps[i].y()/Ps[i].w();
     Pt_inhomg[2] = Ps[i].z()/Ps[i].w();

     gama1.push_back(rig.cam[0].project(Pt_inhomg - rig.cam[0].c));
     gama2.push_back(rig.cam[1].project(Pt_inhomg - rig.cam[1].c));

     fC_orig << rig.cam[0].F << " " << rig.cam[0].c << vcl_endl;

     fgama1_img << p1.x() << " " << p1.y() << vcl_endl;
     fgama2_img << p2.x() << " " << p2.y() << vcl_endl;

     fgama1.write((char *)((gama1[i] + rig.cam[0].c).data_block()),3*sizeof(double));
     fgama2.write((char *)((gama2[i] + rig.cam[1].c).data_block()),3*sizeof(double));

     // Project curvatures of 3D points ----------
     dbdif_3rd_order_point_3d Frame;
     
     Frame.Gama = Pt_inhomg;
     Frame.T[0] = -Pt_inhomg[1];
     Frame.T[1] = Pt_inhomg[0];
     Frame.T[2] = alpha*r;
     Frame.T.normalize();

     Frame.N[0] =  -cos(t[i]);
     Frame.N[1] =  -sin(t[i]);
     Frame.N[2] =  0;

     Frame.B = vnl_cross_3d(Frame.T,Frame.N);
     Frame.K = 1/((alpha*alpha +1) * r);
     Frame.Tau = alpha*Frame.K;

     Frame.Kdot = 0;

//     rig.cam[0].project_k(Frame,&frame1);
//     rig.cam[1].project_k(Frame,&frame2);
     rig.cam[0].project_3rd(Frame,&frame1);
     rig.cam[1].project_3rd(Frame,&frame2);

//     frame1.kdot = frame1.kdot*r*sqrt(2)*(Frame.T - dot_product(Frame.T,rig.cam[0].F)*frame1.gama).two_norm()/ dot_product(Frame.Gama,rig.cam[0].F);

//     frame1.kdot /= dot_product(Frame.Gama,rig.cam[0].F);

     //fk << frame.k << vcl_endl;
     fk.write((char *)&(frame1.k),sizeof(double));
     ft.write((char *)(frame1.t.data_block()),3*sizeof(double));
     fkdot.write((char *)&(frame1.kdot),sizeof(double));
     fn.write((char *)(frame1.n.data_block()),3*sizeof(double));
     double aux = rig.cam[0].speed(Frame);
     faux.write((char *)&aux,sizeof(double));
//     ftt << frame.t << vcl_endl;

     // ------- RECONSTRUCTION ------------------------------------------------ 

     dbdif_3rd_order_point_3d Frame_rec;
     
     rig.reconstruct_3rd_order(frame1,frame2,&Frame_rec);

     frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
     frec_T.write((char *)(Frame_rec.T.data_block()), 3*sizeof(double));
     frec_N.write((char *)(Frame_rec.N.data_block()), 3*sizeof(double));
     frec_B.write((char *)(Frame_rec.B.data_block()), 3*sizeof(double));
     frec_K.write((char *)&(Frame_rec.K),sizeof(double));
     frec_Kdot.write((char *)&(Frame_rec.Kdot),sizeof(double));
     frec_Tau.write((char *)&(Frame_rec.Tau),sizeof(double));
  }
  fgama1_img.close();
  fgama2_img.close();
  fgama1.close();
  fgama2.close();
  fC_orig.close();
  fk.close();
  faux.close();
  fkdot.close();
  ft.close();
  ftt.close();
  fn.close();
  frec.close();
  frec_T.close();
  frec_N.close();
  frec_B.close();
  frec_K.close();
  frec_Kdot.close();
  frec_Tau.close();
}
*/

/* TODO: see if can remove this
void
test_formulas_circle()
{
  
  vcl_vector<dbdif_3rd_order_point_3d> C;
  vcl_vector<double> theta;

  dbdif_analytic::circle_curve (2000, C, theta, 0, 0.01, 360);

  vcl_vector <vsol_point_2d_sptr> gama1_img;
  test_formulas(C,theta,gama1_img);
}

void
test_formulas_helix()
{
  
  vcl_vector<dbdif_3rd_order_point_3d> C;
  vcl_vector<double> theta;

  mw_vector_3d translation(0,0,0);
  dbdif_analytic::helix_curve(20, 1.0/4.0, translation, C, theta, 0, 0.06, 3*360);

  vcl_vector <vsol_point_2d_sptr> gama1_img;
  test_formulas(C,theta,gama1_img);
}

void
test_formulas_space_curve1()
{
  
  vcl_vector<dbdif_3rd_order_point_3d> C;
  vcl_vector<double> theta;

  mw_vector_3d translation(100,100,100);

  dbdif_analytic::space_curve1(300, translation, C, theta, 0.01, 0.02, 360);

  vcl_vector <vsol_point_2d_sptr> gama1_img;
  test_formulas(C,theta,gama1_img);
}
*/


/* TODO: see if can remove this

//: this fn could be useful in a matlab interface
// \todo Maybe output 3 dbdif_3rd_order_points: 2 views  plus 1 reconstruction
// \param[in] C vector of 3D geometry at each point
// \param[in] parameter vector for each entry of C
// \param[out] gama1_img image of C in 1st camera
void 
test_formulas(
  vcl_vector<dbdif_3rd_order_point_3d> &C,
  vcl_vector<double> &theta,
  vcl_vector <vsol_point_2d_sptr> &gama1_img
  )
{
  vcl_ofstream 
    fC_orig, fgama1, fgama2, fgama1_img, fgama2_img, fC_rec, ft, ftt, fn, fk, fkdot,
    frec, frec_T, frec_N, frec_B, frec_K, frec_Kdot, frec_Tau, faux, ftheta;

  dbdif_rig rig;
  rig.read("curr2/255.jpg","curr2/235.jpg");
//  rig.read("curr2/255.jpg","curr2/261.jpg");

  //
  // Project all points ----------
  //
  vcl_vector <vsol_point_2d_sptr> gama2_img;
  vcl_vector <mw_vector_3d> gama1,gama2;
  dbdif_3rd_order_point_2d frame1,frame2;

  // FILES for debugging
  fgama1.open("/tmp/gama1.dat",vcl_ios::out | vcl_ios::binary);
  fgama2.open("/tmp/gama2.dat",vcl_ios::out | vcl_ios::binary);
  fgama1_img.open("/tmp/gama1_img.txt");
  fgama2_img.open("/tmp/gama2_img.txt");

  fk.open("/tmp/k.dat",vcl_ios::out | vcl_ios::binary);
  fkdot.open("/tmp/kdot.dat",vcl_ios::out | vcl_ios::binary);
  ft.open("/tmp/t.dat",vcl_ios::out | vcl_ios::binary);
  ftt.open("/tmp/ttext.txt");
  fn.open("/tmp/n.dat",vcl_ios::out | vcl_ios::binary);
  faux.open("/tmp/g1.dat",vcl_ios::out | vcl_ios::binary);

  frec.open("/tmp/rec.dat",vcl_ios::out | vcl_ios::binary);
  frec_T.open("/tmp/rec_T.dat",vcl_ios::out | vcl_ios::binary);
  frec_N.open("/tmp/rec_N.dat",vcl_ios::out | vcl_ios::binary);
  frec_B.open("/tmp/rec_B.dat",vcl_ios::out | vcl_ios::binary);
  frec_K.open("/tmp/rec_K.dat",vcl_ios::out | vcl_ios::binary);
  frec_Kdot.open("/tmp/rec_Kdot.dat",vcl_ios::out | vcl_ios::binary);
  frec_Tau.open("/tmp/rec_Tau.dat",vcl_ios::out | vcl_ios::binary);

  assert(C.size() == theta.size());

  fC_orig.open("/tmp/FCenter.txt");
  fC_orig << rig.cam[0].F << " " << rig.cam[0].c << vcl_endl;
  fC_orig.close();

  fC_orig.open("/tmp/Gama.dat",vcl_ios::out | vcl_ios::binary);
  ftheta.open("/tmp/theta.dat",vcl_ios::out | vcl_ios::binary);

  gama1_img.reserve(C.size()); gama2_img.reserve(C.size()); gama1.reserve(C.size()); gama2.reserve(C.size());


  for (unsigned i=0; i<C.size(); ++i) {
     mw_vector_3d Pt;
     Pt[0] = C[i].Gama[0];
     Pt[1] = C[i].Gama[1];
     Pt[2] = C[i].Gama[2];

     fC_orig.write((char *)(Pt.data_block()),3*sizeof(double));
     ftheta.write((char *)&(theta[i]),sizeof(double));

     vgl_homg_point_3d<double> Pt_hmg(Pt[0],Pt[1],Pt[2]);

     vgl_point_2d<double> p1 = rig.cam[0].Pr_.project(Pt_hmg);
     vgl_point_2d<double> p2 = rig.cam[1].Pr_.project(Pt_hmg);

     gama1_img.push_back(new vsol_point_2d(p1));
     gama2_img.push_back(new vsol_point_2d(p2));

     gama1.push_back(rig.cam[0].project(Pt - rig.cam[0].c));
     gama2.push_back(rig.cam[1].project(Pt - rig.cam[1].c));

     fgama1_img << p1.x() << " " << p1.y() << vcl_endl;
     fgama2_img << p2.x() << " " << p2.y() << vcl_endl;

     fgama1.write((char *)((gama1[i] + rig.cam[0].c).data_block()),3*sizeof(double));
     fgama2.write((char *)((gama2[i] + rig.cam[1].c).data_block()),3*sizeof(double));

     // Project geometry of 3D points ----------

     rig.cam[0].project_3rd(C[i],&frame1);
     rig.cam[1].project_3rd(C[i],&frame2);

     fk.write((char *)&(frame1.k),sizeof(double));
     ft.write((char *)(frame1.t.data_block()),3*sizeof(double));
     fkdot.write((char *)&(frame1.kdot),sizeof(double));
     fn.write((char *)(frame1.n.data_block()),3*sizeof(double));
     double aux = rig.cam[0].speed(C[i]);
     faux.write((char *)&aux,sizeof(double));

     // ------- RECONSTRUCTION ------------------------------------------------ 

     dbdif_3rd_order_point_3d Frame_rec;
     
     rig.reconstruct_3rd_order(frame1,frame2,&Frame_rec);

     frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
     frec_T.write((char *)(Frame_rec.T.data_block()), 3*sizeof(double));
     frec_N.write((char *)(Frame_rec.N.data_block()), 3*sizeof(double));
     frec_B.write((char *)(Frame_rec.B.data_block()), 3*sizeof(double));
     frec_K.write((char *)&(Frame_rec.K),sizeof(double));
     frec_Kdot.write((char *)&(Frame_rec.Kdot),sizeof(double));
     frec_Tau.write((char *)&(Frame_rec.Tau),sizeof(double));
  }
  fgama1_img.close();
  fgama2_img.close();
  fgama1.close();
  fgama2.close();
  fC_orig.close();
  fk.close();
  faux.close();
  fkdot.close();
  ft.close();
  ftt.close();
  fn.close();
  frec.close();
  frec_T.close();
  frec_N.close();
  frec_B.close();
  frec_K.close();
  frec_Kdot.close();
  frec_Tau.close();
  ftheta.close();
}
*/

/*
void
test_geometry_numerics()
{
  
  vcl_vector<dbdif_3rd_order_point_3d> C;
  vcl_vector<double> theta;

  mw_vector_3d translation(100,100,100);
  dbdif_analytic::space_curve1(300, translation, C, theta, 0.01, 0.02, 360);

//  circle_curve (2000, C, theta, 0, 0.05, 360-0.05);

  vcl_vector <vsol_point_2d_sptr> gama1_img;
  test_formulas(C,theta,gama1_img);

  vcl_ofstream  fgama1_round;
  fgama1_round.open("dat/gama1_round.txt");

  // round and remove dups
  vcl_vector <vsol_point_2d_sptr> gama1_round;

  gama1_round.reserve(gama1_img.size());
  gama1_img[0]->set_x(MW_ROUND(gama1_img[0]->x()));
  gama1_img[0]->set_y(MW_ROUND(gama1_img[0]->y()));
  gama1_round.push_back(gama1_img[0]);
  fgama1_round << gama1_round[0]->x() << " " << gama1_round[0]->y() << vcl_endl;

  for (unsigned i=1; i<gama1_img.size(); ++i) {
    gama1_img[i]->set_x(MW_ROUND(gama1_img[i]->x()));
    gama1_img[i]->set_y(MW_ROUND(gama1_img[i]->y()));
    if (*gama1_img[i] != *gama1_round.back()) {
      gama1_round.push_back(gama1_img[i]);
      fgama1_round << gama1_round.back()->x() << " " << gama1_round.back()->y() << vcl_endl;
    }
  }

  fgama1_round.close();


  // Interpolate with GENO

  dbsol_geno_curve_2d gc;
  dbsol_geno::interpolate3_approx(&gc,gama1_round,false);
//  dbsol_geno::interpolate(&gc,gama1_round,false);

  // Sample points, tangents, curvatures, and output to files to be plotted in
  // Matlab

  vcl_ofstream fgama1_geno, ftgt_geno,ft_angle_geno;

  fgama1_geno.open("dat/gama1_geno.dat",vcl_ios::out | vcl_ios::binary);
  ftgt_geno.open("dat/tgt_geno.dat",vcl_ios::out | vcl_ios::binary);
  ft_angle_geno.open("dat/t_angle_geno.dat",vcl_ios::out | vcl_ios::binary);

  double step=0.00001, t;
  for (double prct=0; prct<=1; prct+=step) {
    t = prct*gc.length();
    
    vsol_point_2d_sptr pt;
    pt = gc.point_at(t);
    // frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
    double dummy = pt->x();
    fgama1_geno.write((char *) & (dummy),sizeof(double));
    dummy = pt->y();
    fgama1_geno.write((char *) & (dummy),sizeof(double));

    vgl_vector_2d<double> tgt;
    tgt = gc.tangent_at(t);

    // frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
    dummy = tgt.x();
    ftgt_geno.write((char *) & (dummy),sizeof(double));
    dummy = tgt.y();
    ftgt_geno.write((char *) & (dummy),sizeof(double));

    dummy = gc.tangent_angle_at(t);
    ft_angle_geno.write((char *) & (dummy),sizeof(double));
  }
  fgama1_geno.close();
  ft_angle_geno.close();
  ftgt_geno.close();

}
*/

//: interpolate a smoothed curve read from a file.
// OBS: this is an old version; I'm using test_geometry_numerics2_2 and
// eventually commenting out the use of analytic tangents if I want.
void
test_geometry_numerics2()
{
  double dummy,t;

  vcl_vector<vsol_point_2d_sptr> pts;
  (void) myreadv(vcl_string("dat/gama1-sm.dat"), pts);

  
  // Interpolate with GENO

  dbsol_geno_curve_2d gc, gc_circle;
  dbsol_geno::interpolate3_approx(&gc,pts,false);
//  dbsol_geno::interpolate(&gc,pts,false);
  dbsol_geno::interpolate(&gc_circle,pts,false);

  // Sample points, tangents, curvatures, and output to files to be plotted in
  // Matlab

  vcl_ofstream fgama1_geno, fgama1_gcircle, ftgt_geno,ft_angle_geno,fk_geno,fkdot_geno, flen,
               ft_delta, fk_delta, fkdot_delta, fgama1_g_super_sample;

  fgama1_geno.open("dat/gama1_geno-sm.dat",vcl_ios::out | vcl_ios::binary);
  fgama1_gcircle.open("dat/gama1_geno-circle-sm.dat",vcl_ios::out | vcl_ios::binary);
  fgama1_g_super_sample.open("dat/gama1_geno-super-sample-sm.dat",vcl_ios::out | vcl_ios::binary);
  ftgt_geno.open("dat/tgt_geno-sm.dat",vcl_ios::out | vcl_ios::binary);
  ft_angle_geno.open("dat/t_angle_geno-sm.dat",vcl_ios::out | vcl_ios::binary);
  fk_geno.open("dat/k_geno-sm.dat",vcl_ios::out | vcl_ios::binary);
  fkdot_geno.open("dat/kdot_geno-sm.dat",vcl_ios::out | vcl_ios::binary);
  flen.open("dat/len_geno-sm.dat",vcl_ios::out | vcl_ios::binary);
  fk_delta.open("dat/dk_geno-sm.dat",vcl_ios::out | vcl_ios::binary);
  ft_delta.open("dat/dt_geno-sm.dat",vcl_ios::out | vcl_ios::binary);
  fkdot_delta.open("dat/dkdot_geno-sm.dat",vcl_ios::out | vcl_ios::binary);

  // loop on original samples
  for (unsigned i=0; i<=gc.size(); ++i) {
    t = gc.length_at(i);

    dummy = t;
    flen.write((char *) & (dummy),sizeof(double));
    
    vsol_point_2d_sptr pt;
    pt = gc.point_at(t);
    // frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
    dummy = pt->x();
    fgama1_geno.write((char *) & (dummy),sizeof(double));
    dummy = pt->y();
    fgama1_geno.write((char *) & (dummy),sizeof(double));

    
    vgl_vector_2d<double> tgt;
    tgt = gc.tangent_at(t);

    // frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
    double tgt_x = tgt.x();
    double tgt_y = tgt.y();

    double t_angle = gc.tangent_angle_at(t);
    double kk = gc.curvature_at(t);

    double kkdot = gc.curvature_derivative_at(t);


    // Write differences from left + right interpolations

    double dt=0, dk=0, dkdot=0;

    if (i != 0 && i != gc.size()) {
      const dbgl_eulerspiral * c_i=0;
      const dbgl_eulerspiral * c_i_min_1=0;
      c_i = dynamic_cast<const dbgl_eulerspiral*>(gc.interval(i));
      c_i_min_1 = dynamic_cast<const dbgl_eulerspiral*>(gc.interval(i-1));
      assert(c_i && c_i_min_1);

      dt    = gc_circle.interval(i)->tangent_angle_at(0) - gc_circle.interval(i-1)->tangent_angle_at(1);
      dt    = vcl_fabs(dt);
      dk    = c_i->curvature_at(0) - c_i_min_1->curvature_at(1);
      dk    = vcl_fabs(dk);
      dkdot = c_i->gamma() - c_i_min_1->gamma();
      dkdot = vcl_fabs(dkdot);

      kk = (c_i->curvature_at(0) + c_i_min_1->curvature_at(1))*0.5;
      kkdot = (c_i->gamma() + c_i_min_1->gamma())*0.5;
      t_angle = (c_i->tangent_angle_at(0) + c_i_min_1->tangent_angle_at(1)) *0.5;
      vgl_vector_2d<double> t_i = c_i->tangent_at(0);
      vgl_vector_2d<double> t_i_min_1 = c_i_min_1->tangent_at(1);
      tgt_x = (t_i.x() + t_i_min_1.x())/0.5;
      tgt_y = (t_i.y() + t_i_min_1.y())/0.5;
    }

    fkdot_delta.write((char *) &(dkdot),sizeof(double));
    fk_delta.write((char *) &(dk),sizeof(double));
    ft_delta.write((char *) &(dt),sizeof(double));

    ftgt_geno.write((char *) & (tgt_x),sizeof(double));
    ftgt_geno.write((char *) & (tgt_y),sizeof(double));
    ft_angle_geno.write((char *) & (t_angle),sizeof(double));
    fk_geno.write((char *) & (kk),sizeof(double));
    fkdot_geno.write((char *) & (kkdot),sizeof(double));

  }

  fgama1_geno.close();
  ft_angle_geno.close();
  ftgt_geno.close();
  fk_geno.close();
  fkdot_geno.close();
  flen.close();
  fkdot_delta.close();
  ft_delta.close();
  fk_delta.close();


  // fine supersampling loop
  double step=0.00001, tcirc;
  for (double prct=0; prct<=1; prct+=step) {
    double dummy;
    tcirc = prct*gc_circle.length();

    vsol_point_2d_sptr mypt;
    mypt = gc_circle.point_at(tcirc);
    dummy = mypt->x();
    fgama1_gcircle.write((char *) & (dummy),sizeof(double));
    dummy = mypt->y();
    fgama1_gcircle.write((char *) & (dummy),sizeof(double));

    mypt = gc.point_at(prct*gc.length());
    dummy = mypt->x();
    fgama1_g_super_sample.write((char *) & (dummy),sizeof(double));
    dummy = mypt->y();
    fgama1_g_super_sample.write((char *) & (dummy),sizeof(double));
  }
  fgama1_gcircle.close();
  fgama1_g_super_sample.close();
}

//: Default test routine; this one is currently interpolating point using
// approximated eulerspiral-geno and uses gradient descent for refining tangents
// and position of the points.
//
void
test_geometry_numerics2_2(unsigned n_iter, unsigned n_iter_position)
{
  vcl_vector<vsol_point_2d_sptr> pts;
  (void) myreadv(vcl_string("dat/gama1-sm.dat"), pts);

  vcl_vector<double> t_angles;
  (void) myread(vcl_string("dat/t_angle_el.dat"), t_angles);
  
  // Interpolate with GENO

  dbsol_geno_curve_2d *pgc, *pgc_refined, *gc_circle;

//  dbsol_geno::interpolate3_from_tangents(&gc,pts,t_angles,false);
//  dbsol_geno::interpolate(&gc,pts,false);

  pgc = new dbsol_geno_curve_2d();
  dbsol_geno::interpolate3_approx(pgc,pts,false);

  gc_circle = new dbsol_geno_curve_2d();
  dbsol_geno::interpolate(gc_circle,pts,false);

  write_geno_info(*pgc,*gc_circle,"sm");
  write_geno_info_super_sample(*pgc,"sm");


//  double delta_angle = 0.0001, psi=0.001;
//  double delta_angle = 1e-5, psi=1e-7;

// Good for 10e-6 order error (fine sampling, 3k points)
//  double delta_angle = 1e-6, psi=1e-6;


  /*
  for (unsigned i=0; i < pts.size(); ++i) {
    pts[i]->set_x(pts[i]->x() + 1e-2*curvature_at);
    pts[i]->set_y(pts[i]->y() + 1e-2*curvature_at);
  }
  */

// Good for 10e-3 order angle error, 70 points
//  double delta_angle = 1e-4, psi=1e-4;
//  double delta_angle = 5e-3, psi=1e-3;
//  double delta_angle = 1e-6, psi=1e-6;
  double delta_angle = 5e-4, psi=1e-3;

  refine_tangents(&pgc, &pgc_refined,delta_angle,psi,n_iter);
  write_geno_info(*pgc_refined,*gc_circle,"ref");
  write_geno_info_super_sample(*pgc_refined,"ref");

  // --- Refine positions (smooth)
  //
  double 
    delta_norm = 5e-3, 
    psi_pos = 0.01;

    /* FIXME Gives bug in dbgl_arc:
    delta_norm = 5e-5, 
    psi_pos = 1e-4;
    */

  vcl_vector<double> grad;
  for (unsigned i=0; i<n_iter_position; ++i) {
    if (pgc) 
      delete pgc;

    pgc = pgc_refined;
    pgc_refined = new dbsol_geno_curve_2d();

    gradient_descent_positional(pts,*pgc,delta_norm,psi_pos,grad);
    delete pgc;

    dbsol_geno::interpolate3_approx(pgc_refined,pts,false);
    
    pgc = pgc_refined;
    // refine tangents again
    refine_tangents(&pgc, &pgc_refined,delta_angle,psi, n_iter);
  }

  delete gc_circle;
  gc_circle = new dbsol_geno_curve_2d();
  dbsol_geno::interpolate(gc_circle,pts,false);

  write_geno_info(*pgc_refined,*gc_circle,"posref");
  write_geno_info_super_sample(*pgc_refined,"posref");
  write_geno_info_super_sample(*gc_circle,"circle-sm");
}

//:
// Does arc-geno positional gradient descent optimization.
// Fri Sep 30 11:53:40 EDT 2005
void
arc_positional_descent_test(unsigned n_iter, unsigned n_iter_position)
{
  vcl_vector<vsol_point_2d_sptr> pts;
  (void) myreadv(vcl_string("dat/gama1-sm.dat"), pts);

  vcl_vector<double> t_angles;
  (void) myread(vcl_string("dat/t_angle_el.dat"), t_angles);
  
  // Interpolate with GENO


  dbsol_geno_curve_2d *pgc, *pgc_refined, *gc_circle;

  pgc = new dbsol_geno_curve_2d();
  dbsol_geno::interpolate3_approx(pgc,pts,false);

  gc_circle = new dbsol_geno_curve_2d();
  dbsol_geno::interpolate(gc_circle,pts,false);

  write_geno_info(*pgc,*gc_circle,"sm");
  write_geno_info_super_sample(*pgc,"sm");
  write_geno_info_super_sample(*gc_circle,"circle-sm");

  double delta_angle = 5e-4, psi=1e-3;

  refine_tangents(&pgc, &pgc_refined,delta_angle,psi,n_iter);
  write_geno_info(*pgc_refined,*gc_circle,"ref");
  write_geno_info_super_sample(*pgc_refined,"ref");

  delete pgc_refined;
  pgc_refined = gc_circle;

  // --- Refine positions (smooth)
  //
  double 
//    delta_norm = 0.005, 
//    psi_pos = 0.02;
    delta_norm = 0.01, 
    psi_pos = delta_norm/4;

  vcl_vector<double> grad;
  for (unsigned i=0; i<n_iter_position; ++i) {
    delete pgc;

    pgc = pgc_refined;
    pgc_refined = new dbsol_geno_curve_2d();

    gradient_descent_positional(pts,*pgc,delta_norm,psi_pos,grad);

    dbsol_geno::interpolate(pgc_refined,pts,false);
  }

  
  delete pgc;
  delete pgc_refined;
  pgc_refined = new dbsol_geno_curve_2d();

  dbsol_geno::interpolate3_approx(pgc_refined,pts,false);
  pgc = pgc_refined;
  refine_tangents(&pgc, &pgc_refined,delta_angle,psi, n_iter);

  gc_circle = new dbsol_geno_curve_2d();
  dbsol_geno::interpolate(gc_circle,pts,false);

  write_geno_info(*pgc_refined,*gc_circle,"posref");
  write_geno_info_super_sample(*pgc_refined,"posref");
  write_geno_info_super_sample(*gc_circle,"posref-circle");

  // write gradient for last iteration
  vcl_ofstream fgrad;

  vcl_string fname("dat/grad-pos.dat");
  fgrad.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  for (unsigned i=0; i<grad.size(); ++i) {
    fgrad.write((char *) &(grad[i]),sizeof(double));
  }

  fgrad.close();
}


//: Write info of geno curve onto binary files, with specified suffix.
//
// Info is focused on the sample points;
//
// Basically get points, tangents, curvatures, and output to files to be
// plotted in Matlab
void
write_geno_info(
    const dbsol_geno_curve_2d &gc,  //:< 3rd order (Eulerspiral) geno curve
    const dbsol_geno_curve_2d &gc_circle,  //:< underlying circle interpolation
    const char *ssuffix)
{

  vcl_string suf(ssuffix), fname, ext(".dat");

  // write arc information
  write_geno_arc_info(gc_circle, (suf+vcl_string("-arc")).c_str());


  assert(gc.order() == 3);

  vcl_ofstream fgama1_geno, ftgt_geno,ft_angle_geno,fk_geno,fkdot_geno, flen,
               ft_delta, fk_delta, fkdot_delta;

  fname = vcl_string("dat/gama1_geno-") + suf + ext;
  fgama1_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/tgt_geno-") + suf + ext;
  ftgt_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/t_angle_geno-") + suf + ext;
  ft_angle_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/k_geno-") + suf + ext;
  fk_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/kdot_geno-") + suf + ext;
  fkdot_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/len_geno-") + suf + ext;
  flen.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/dk_geno-") + suf + ext;
  fk_delta.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/dt_geno-") + suf + ext;
  ft_delta.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/dkdot_geno-") + suf + ext;
  fkdot_delta.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  // loop on original samples
  for (unsigned i=0; i<=gc.size(); ++i) {
    double dummy;
    double t = gc.length_at(i);

    dummy = t;
    flen.write((char *) & (dummy),sizeof(double));
    
    vsol_point_2d_sptr pt;
    pt = gc.point_at(t);
    // frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
    dummy = pt->x();
    fgama1_geno.write((char *) & (dummy),sizeof(double));
    dummy = pt->y();
    fgama1_geno.write((char *) & (dummy),sizeof(double));

    
    vgl_vector_2d<double> tgt;
    tgt = gc.tangent_at(t);

    // frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
    double tgt_x = tgt.x();
    double tgt_y = tgt.y();

    double t_angle = gc.tangent_angle_at(t);
    double kk = gc.curvature_at(t);

    double kkdot = gc.curvature_derivative_at(t);


    // Write differences from left + right interpolations

    double dt=0, dk=0, dkdot=0;

    if (i != 0 && i != gc.size()) {
      const dbgl_eulerspiral * c_i=0;
      const dbgl_eulerspiral * c_i_min_1=0;
      c_i = dynamic_cast<const dbgl_eulerspiral*>(gc.interval(i));
      c_i_min_1 = dynamic_cast<const dbgl_eulerspiral*>(gc.interval(i-1));
      assert(c_i && c_i_min_1);

      dt    = gc_circle.interval(i)->tangent_angle_at(0) - gc_circle.interval(i-1)->tangent_angle_at(1);
      dt    = vcl_fabs(dt);
      if (dt > vnl_math::pi)
        dt = 2*vnl_math::pi - dt;

      dk    = c_i->curvature_at(0) - c_i_min_1->curvature_at(1);
      dk    = vcl_fabs(dk);
      dkdot = c_i->gamma() - c_i_min_1->gamma();
      dkdot = vcl_fabs(dkdot);

      kk = (c_i->curvature_at(0) + c_i_min_1->curvature_at(1))*0.5;
      kkdot = (c_i->gamma() + c_i_min_1->gamma())*0.5;
      vgl_vector_2d<double> t_i = gc.interval(i)->tangent_at(0);
      vgl_vector_2d<double> t_i_min_1 = gc.interval(i-1)->tangent_at(1);
      tgt_x = (t_i.x() + t_i_min_1.x())/0.5;
      tgt_y = (t_i.y() + t_i_min_1.y())/0.5;
      double norm_t = hypot(tgt_x,tgt_y);
      tgt_x = tgt_x / norm_t;
      tgt_y = tgt_y / norm_t;

      t_angle = vcl_atan2(tgt_y, tgt_x);
      if (t_angle < 0)
        t_angle = 2*vnl_math::pi + t_angle;
    }

    fkdot_delta.write((char *) &(dkdot),sizeof(double));
    fk_delta.write((char *) &(dk),sizeof(double));
    ft_delta.write((char *) &(dt),sizeof(double));

    ftgt_geno.write((char *) & (tgt_x),sizeof(double));
    ftgt_geno.write((char *) & (tgt_y),sizeof(double));
    ft_angle_geno.write((char *) & (t_angle),sizeof(double));
    fk_geno.write((char *) & (kk),sizeof(double));
    fkdot_geno.write((char *) & (kkdot),sizeof(double));

  }

  fgama1_geno.close();
  ft_angle_geno.close();
  ftgt_geno.close();
  fk_geno.close();
  fkdot_geno.close();
  flen.close();
  fkdot_delta.close();
  ft_delta.close();
  fk_delta.close();
}

//: Write info of geno arc curve onto binary files, with specified suffix.
//
// Info is focused on the sample points;
//
// Basically get points, tangents, curvatures, and output to files to be
// plotted in Matlab
void
write_geno_arc_info(
    const dbsol_geno_curve_2d &gc,
    const char *ssuffix)
{
  vcl_string suf(ssuffix), fname, ext(".dat");

  assert(gc.order() == 2);

  vcl_ofstream fgama1_geno, ftgt_geno,ft_angle_geno,fk_geno,fkdot_geno, flen,
               ft_delta, fk_delta, fkdot_delta;

  fname = vcl_string("dat/gama1_geno-") + suf + ext;
  fgama1_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/tgt_geno-") + suf + ext;
  ftgt_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/t_angle_geno-") + suf + ext;
  ft_angle_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/k_geno-") + suf + ext;
  fk_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/kdot_geno-") + suf + ext;
  fkdot_geno.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/len_geno-") + suf + ext;
  flen.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/dk_geno-") + suf + ext;
  fk_delta.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/dt_geno-") + suf + ext;
  ft_delta.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  fname = vcl_string("dat/dkdot_geno-") + suf + ext;
  fkdot_delta.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  // loop on original samples
  for (unsigned i=0; i<=gc.size(); ++i) {
    double dummy;
    double t = gc.length_at(i);

    dummy = t;
    flen.write((char *) & (dummy),sizeof(double));
    
    vsol_point_2d_sptr pt;
    pt = gc.point_at(t);
    // frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
    dummy = pt->x();
    fgama1_geno.write((char *) & (dummy),sizeof(double));
    dummy = pt->y();
    fgama1_geno.write((char *) & (dummy),sizeof(double));

    
    vgl_vector_2d<double> tgt;
    tgt = gc.tangent_at(t);

    // frec.write((char *)(Frame_rec.Gama.data_block()),3*sizeof(double));
    double tgt_x = tgt.x();
    double tgt_y = tgt.y();

    double t_angle = gc.tangent_angle_at(t);
    double kk = gc.curvature_at(t);

    double kkdot = 0;


    // Write differences from left + right interpolations

    double dt=0, dk=0, dkdot=0;

    if (i != 0 && i != gc.size()) {
      const dbgl_arc *c_i=0;
      const dbgl_arc *c_i_min_1=0;
      c_i = dynamic_cast<const dbgl_arc*>(gc.interval(i));
      c_i_min_1 = dynamic_cast<const dbgl_arc*>(gc.interval(i-1));
      assert(c_i && c_i_min_1);

      dt    = gc.interval(i)->tangent_angle_at(0) - gc.interval(i-1)->tangent_angle_at(1);
      dt    = vcl_fabs(dt);
      if (dt > vnl_math::pi)
        dt = 2*vnl_math::pi - dt;

      dk    = c_i->curvature_at(0) - c_i_min_1->curvature_at(1);
      dk    = vcl_fabs(dk);
      dkdot = 0;

      kk = (c_i->curvature_at(0) + c_i_min_1->curvature_at(1))*0.5;
      kkdot = 0;
      vgl_vector_2d<double> t_i = gc.interval(i)->tangent_at(0);
      vgl_vector_2d<double> t_i_min_1 = gc.interval(i-1)->tangent_at(1);
      tgt_x = (t_i.x() + t_i_min_1.x())/0.5;
      tgt_y = (t_i.y() + t_i_min_1.y())/0.5;
      double norm_t = hypot(tgt_x,tgt_y);
      tgt_x = tgt_x / norm_t;
      tgt_y = tgt_y / norm_t;

      t_angle = vcl_atan2(tgt_y, tgt_x);
      if (t_angle < 0)
        t_angle = 2*vnl_math::pi + t_angle;
    }

    fkdot_delta.write((char *) &(dkdot),sizeof(double));
    fk_delta.write((char *) &(dk),sizeof(double));
    ft_delta.write((char *) &(dt),sizeof(double));

    ftgt_geno.write((char *) & (tgt_x),sizeof(double));
    ftgt_geno.write((char *) & (tgt_y),sizeof(double));
    ft_angle_geno.write((char *) & (t_angle),sizeof(double));
    fk_geno.write((char *) & (kk),sizeof(double));
    fkdot_geno.write((char *) & (kkdot),sizeof(double));
  }

  fgama1_geno.close();
  ft_angle_geno.close();
  ftgt_geno.close();
  fk_geno.close();
  fkdot_geno.close();
  flen.close();
  fkdot_delta.close();
  ft_delta.close();
  fk_delta.close();
}

//: fine sampling of geno information, to verify the interpolation within
// intervals
void
write_geno_info_super_sample(const dbsol_geno_curve_2d &gc, char *suffix)
{
  vcl_ofstream fgama1_g_super_sample;

  vcl_string suf(suffix), fname, ext(".dat");

  fname = vcl_string("dat/gama1_geno-super-sample-") + suf + ext;
  fgama1_g_super_sample.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

  // fine supersampling loop
  double step=0.00001, tcirc;
  for (double prct=0; prct<=1; prct+=step) {
    double dummy;
    tcirc = prct*gc.length();

    vsol_point_2d_sptr mypt;
    mypt = gc.point_at(tcirc);
    dummy = mypt->x();
    fgama1_g_super_sample.write((char *) & (dummy),sizeof(double));
    dummy = mypt->y();
    fgama1_g_super_sample.write((char *) & (dummy),sizeof(double));

  }
  fgama1_g_super_sample.close();
}


/*
void
reproject_curve_sculpture()
{
  // - read 3 contours

   vcl_string fname1("curr2/261-crop.jpg");
   vcl_string fname2("curr2/255-crop.jpg");
   vcl_string fname3("curr2/257-crop.jpg");

   dbdif_rig rig;
   rig.read(fname1,fname2);

   bool isopen;
   vcl_vector<vsol_point_2d_sptr> points1, points2, 

   load_con_file("curr2/261-crop.con", points1, &isopen);
   load_con_file("curr2/255-crop.con", points2, &isopen);

   for (i=0; i<points1.size(); ++i) {
     // 1- find epipolar line in 2nd camera
     // 2- find corresponding point in 2nd camera

     // 3- Reproject into 4th camera
     //    - one way: intersect two epipolar lines
     //     - if they are almost parallel, signalize
     //    - another way: reconstruct then reproject

   }

   // write con file
}
*/







