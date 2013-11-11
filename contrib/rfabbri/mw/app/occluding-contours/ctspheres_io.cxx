#include "ctspheres_io.h"

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>
#include <dvpgl/pro/dvpgl_camera_storage_sptr.h>
#include <dbdet/tracer/dbdet_contour_tracer.h>

#include <mw/mw_util.h>
#include <dbdif/algo/dbdif_data.h>
#include <bvis1/bvis1_manager.h>


//: Loads pradeep's CT sphere images and their parameters into vpgl_cameras
void 
load_ct_spheres_dataset()
{


  vnl_double_3x3 m;

  dbdif_turntable::internal_calib_ctspheres(m);

  vpgl_calibration_matrix<double> K(m);


  vcl_vector<vcl_string> fname;

  fname.push_back("ct-spheres/artifact_2_0000_8bpp.tif");
  fname.push_back("ct-spheres/artifact_2_0001_8bpp.tif");
  fname.push_back("ct-spheres/artifact_2_0002_8bpp.tif");
  fname.push_back("ct-spheres/artifact_2_0003_8bpp.tif");
//  fname.push_back("ct-spheres/artifact_2_0004_8bpp.tif");
//  fname.push_back("ct-spheres/artifact_2_0005_8bpp.tif");
//  fname.push_back("ct-spheres/artifact_2_0006_8bpp.tif");
//  fname.push_back("ct-spheres/artifact_2_0007_8bpp.tif");

//  fname.push_back("ct-spheres/artifact_2_0000.tif");
//  fname.push_back("ct-spheres/artifact_2_0001.tif");
//  fname.push_back("ct-spheres/artifact_2_0002.tif");
//  fname.push_back("ct-spheres/artifact_2_0003.tif");
//  fname.push_back("ct-spheres/artifact_2_0004.tif");

  for (unsigned frm_i=0; frm_i<fname.size(); ++frm_i) {
    if (frm_i > 0)
      MANAGER->add_new_frame();


    // -- Image

    vil_image_resource_sptr img = vil_load_image_resource( fname[frm_i].c_str() );
    if( !img ) {
      vcl_cerr << "Failed to load image file" << fname[frm_i] << vcl_endl;
      return;
    }

    // Create the storage data structures
    vcl_string itype("image");
    vcl_string iname(fname[frm_i].c_str());

    bpro1_storage_sptr img_data = MANAGER->repository()->new_data_at(itype,iname,frm_i);
    if (img_data) {
       vidpro1_image_storage_sptr img_storage;
       img_storage.vertical_cast(img_data);
       img_storage->set_image(img);
       MANAGER->add_to_display(img_data);
    } else {
       vcl_cerr << "error: unable to register new data\n";
       return;
    }

    vpgl_perspective_camera<double> *P;
    P = dbdif_turntable::camera_ctspheres(frm_i,K);

    dvpgl_camera_storage_sptr Pstorage = new dvpgl_camera_storage();
    Pstorage->set_camera(P);
    Pstorage->set_name("camera");

    MANAGER->repository()->store_data_at(Pstorage,frm_i);

    // -- Now register camera into repository
  }

  MANAGER->first_frame();
}

// reads levelset segmentation (a 3D image whose 0-levelset are the desired contours)
void
read_levelset(vcl_vector<vil_image_view<float> > &imgv)
{

  vcl_vector<double> v;
  myread("ct-spheres/dat/phi-0-5-600x.dat",v);

  // todo: improve this:
  unsigned nrows = 314;
  unsigned ncols = 600;
  unsigned nslices = 6;

  assert ((v.size()/(314.0*600.0))   == nslices );


  imgv.resize(nslices);

  vcl_vector<double>::const_iterator vitr;

  vitr = v.begin();

  for (unsigned k=0; k<nslices; ++k) {
    imgv[k].set_size(ncols,nrows);
    for (unsigned i=0; i<ncols; ++i) {
      for (unsigned j=0; j<nrows; ++j) {
        imgv[k](i,j) = *vitr; 
        vitr++;
      }
    }
//    printf("Slice: %d  first: %g  second in 0-row: %g\n", k, imgv[k](0,0), imgv[k](1,0));
  }
}



//: trace 0-levelset at given slice
void 
trace_contours(const vcl_vector<vil_image_view<float> > &imgv, 
    vcl_vector<vcl_vector<vsol_point_2d_sptr> >  &final_contours,
    unsigned slice_idx)
{
  dbdet_contour_tracer tracer;

  tracer.trace_sedt_image(imgv[slice_idx]);

  for (unsigned i=0; i<tracer.contours().size(); ++i) {
    if (tracer.contours()[i].size() > 3)
      final_contours.push_back(tracer.contours()[i]);
  }
  vcl_cout << "Found " << final_contours.size() << " large enough contours\n";
}
