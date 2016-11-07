
#include <dbdif/dbdif_rig.h>
#include <mw/mw_util.h>
#include <dbdif/dbdif_analytic.h>
#include "mw_load_data.h"
#include <dbdif/algo/dbdif_data.h>
#include <mw/app/occluding-contours/ctspheres_io.h>


#include <dvpgl/pro/dvpgl_camera_storage.h>
#include <dvpgl/pro/dvpgl_camera_storage_sptr.h>

#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <bvis1/bvis1_manager.h>

#include <vgui/vgui_find.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>


//: just an example on how to load 2 perspective cams into the repository
void 
load_cams_into_repository()
{
  vnl_double_3x3 Kmatrix;
  dbdif_turntable::internal_calib_ctspheres(Kmatrix);
  vpgl_calibration_matrix<double> K(Kmatrix);

  vpgl_perspective_camera<double> *P, *P_dt;

  P    = dbdif_turntable::camera_ctspheres(0, K);
  P_dt = dbdif_turntable::camera_ctspheres(1, K);

  dbdif_rig rig(*P,*P_dt);

  // add to repository
  {
  dvpgl_camera_storage_sptr Pstorage = new dvpgl_camera_storage();
  Pstorage->set_camera(P);
  Pstorage->set_name("camera");
  MANAGER->repository()->store_data_at(Pstorage,0);
  }

  MANAGER->add_new_frame();
  {
  dvpgl_camera_storage_sptr Pstorage = new dvpgl_camera_storage();
  Pstorage->set_camera(P_dt);
  Pstorage->set_name("camera");
  MANAGER->repository()->store_data_at(Pstorage,1);
  }
  MANAGER->first_frame();
}

void
mw_load_data::
load_rotational_analytic_pair()
{
  load_rotational_analytic_views(10, 2);
}

void
mw_load_data::
load_rotational_analytic_views()
{
  load_rotational_analytic_views(100, 3);
}

void
mw_load_data::
load_rotational_analytic_views_olympus_turntable()
{
  load_rotational_analytic_views(100, 3, false);
}

//: nviews: number of cameras
//  ngrids: number of viewports in brown eyes
//  \param[in] ctspheres : true if ctspheres data; false if olympus turntable data
void
mw_load_data::
load_rotational_analytic_views(unsigned nviews, unsigned ngrids_wanted, bool ctspheres)
{
  //----------------------------------------------------------------------
  // Define cameras
  
  vnl_double_3x3 Kmatrix;

  if (ctspheres) {
    double x_max_scaled = 600;
    dbdif_turntable::internal_calib_ctspheres(Kmatrix,x_max_scaled);
  } else {
    unsigned  crop_origin_x_ = 450;
    unsigned  crop_origin_y_ = 1750;
//    double x_max_scaled = 255;
    double x_max_scaled = 500;
    dbdif_turntable::internal_calib_olympus(Kmatrix, x_max_scaled, crop_origin_x_, crop_origin_y_);
  }

  vpgl_calibration_matrix<double> K(Kmatrix);

  vcl_vector<dbdif_camera> cam;
  cam.resize(nviews);

  for (unsigned i=0; i < nviews; ++i) { // 1 degree per view
    vpgl_perspective_camera<double> *P;
    if (ctspheres)
      P = dbdif_turntable::camera_ctspheres(2*i, K);
    else {
      P = dbdif_turntable::camera_olympus(i, K);
    }
    cam[i].set_p(*P);

    // add to repository
    if (i > 0)
      MANAGER->add_new_frame();

    dvpgl_camera_storage_sptr Pstorage = new dvpgl_camera_storage();
    Pstorage->set_camera(P);
    Pstorage->set_name("camera");
    MANAGER->repository()->store_data_at(Pstorage,i);
  }
  MANAGER->first_frame();


  //----------------------------------------------------------------------
  // Define space curves

  // crv2d[i][j]  curve i view j
  vcl_vector<vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > > crv2d;
  {
    vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d;
    if (ctspheres)
      dbdif_data::space_curves_ctspheres( crv3d );
    else 
//      dbdif_data::space_curves_olympus_turntable( crv3d );
      dbdif_data::space_curves_digicam_turntable_sandbox( crv3d );


    crv2d.resize(crv3d.size());

    for (unsigned  i=0; i < crv3d.size(); ++i) {
      dbdif_data::project_into_cams(crv3d[i], cam, crv2d[i]);
    }
  }

  /* TODO Limiting distances to subpixel: don't do this for now, since it would
   * have to be done consistently across multiple views in order to keep
   * correspondence information.
  for (unsigned  i=0; i < crv2d.size(); ++i) {
    for (unsigned k=0; k < crv2d[i].size(); ++k) {
      if (k == 0 || k == 4 || k == 59) {
        vcl_vector<dbdif_3rd_order_point_2d> ctmp;
        vcl_cout << "CURVE #" << i+1 << " frame: " << k+1 << vcl_endl;
        vcl_cout << "Before lim dist: " << crv2d[i][k].size() << "\n";
        if (!dbdif_analytic::limit_distance(crv2d[i][k], ctmp))
          vcl_cout << "Warning: some distances are more than sqrt(2)\n";
        vcl_cout << "After lim dist: " << ctmp.size() << vcl_endl;
        vcl_cout << "-------\n";

        crv2d[i][k].clear();
        crv2d[i][k] = ctmp;
      }
    }
  }
  */

  //----------------------------------------------------------------------
  // Project space curves
  // todo: (make sure we have pixel spacing )
  

  //: image coordinates
  // xi[i][k] == curve i at view k
  unsigned  number_of_curves = crv2d.size();
  vcl_vector< vcl_vector<vcl_vector<vsol_point_2d_sptr> > > xi; 

  xi.resize(number_of_curves);
  for (unsigned i=0; i<number_of_curves; ++i) {
    xi[i].resize(nviews);
    for (unsigned  k=0; k < nviews; ++k) {
      xi[i][k].resize(crv2d[i][k].size());
      for (unsigned  j=0; j < crv2d[i][k].size(); ++j) 
        xi[i][k][j] = new vsol_point_2d(crv2d[i][k][j].gama[0], crv2d[i][k][j].gama[1]);
    }
//    project_into_cams( crv3d[i], cam, xi[i]);
  }

  //----------------------------------------------------------------------
  // Reconstruct from the known correspondence, as a check.

  vcl_vector<mw_vector_3d> C_rec;
  dbdif_rig rig(cam[0].Pr_,cam[1].Pr_);
  rig.reconstruct_3d_curve(&C_rec,xi[0][0],xi[0][1]);

  mywritev(vcl_string("dat/crv3drec-1-tracer.dat"), C_rec);

  //----------------------------------------------------------------------
  // Create storages and add to repository


  for (unsigned k=0; k < nviews; ++k) {
    vcl_vector<vsol_spatial_object_2d_sptr> p_xi;

    p_xi.resize(number_of_curves);

    for (unsigned i=0; i<number_of_curves; ++i) {
      p_xi[i] = new vsol_polyline_2d(xi[i][k]);
    }

    if (k > 0)
      MANAGER->next_frame();

    // create the output storage class
    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    output_vsol->add_objects(p_xi, vcl_string("analytic_curves"));
    output_vsol->set_name("analytic_curves");
    MANAGER->repository()->store_data(output_vsol);
  }
  MANAGER->first_frame();
  
  //----------------------------------------------------------------------
  // Add n views


  // add grid if only one present
  vgui_grid_tableau_sptr pgrid;
  pgrid.vertical_cast(vgui_find_below_by_type_name(MANAGER,"vgui_grid_tableau"));

  unsigned ngrids = pgrid->rows()*pgrid->cols();

  while (ngrids < ngrids_wanted) {
    MANAGER->add_new_view(ngrids++, false);
  }

  MANAGER->display_current_frame();
}
