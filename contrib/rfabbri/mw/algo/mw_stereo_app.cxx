#include "mw_stereo_app.h"

#include <vcl_sstream.h>
#include <mw/mw_util.h>
#include <mw/algo/mw_point_matcher.h>
#include <mw/algo/mw_match_position_band.h>
#include <mw/algo/mw_match_tangent_band.h>
#include <mw/mw_subpixel_point_set.h>
#include <dbdif/algo/dbdif_data.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_load.h>
#include <vsol/vsol_line_2d.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <bvis1/bvis1_macros.h>
#include <vnl/vnl_random.h>
#include <dvpgl/io/dvpgl_io_cameras.h>


static vnl_random myrand;

void mw_stereo_app_args::
set(
  vul_arg<vcl_string> &a_prefix, 
  vul_arg<vcl_string> &a_out_path, 
  vul_arg<vcl_string> &a_fname1,
  vul_arg<vcl_string> &a_fname2, 
  vul_arg<vcl_string> &a_fname3,
  vul_arg<vcl_vector<vcl_string> > &a_fnames,
  vul_arg<vcl_string> &a_corresp_gt,
  vul_arg<vcl_string> &a_precomputed_epip,
  vul_arg<vcl_string> &a_edgels1,
  vul_arg<vcl_string> &a_edgels2,
  vul_arg<vcl_string> &a_edgels3,
  vul_arg<vcl_string> &a_edgel_type,
  vul_arg<vcl_vector<vcl_string> > &a_edgels,
  vul_arg<bool> &a_run_trinocular,
  vul_arg<bool> &a_run_trinocular_t_diff,
  vul_arg<bool> &a_run_trinocular_k_diff,
  vul_arg<bool> &a_write_epipolar_corresp,
  vul_arg<bool> &a_read_epipolar_corresp,
  vul_arg<vcl_string> &a_corresp_in,
  vul_arg<bool> &a_compare_corresp,
  vul_arg<bool> &a_compute_epipolars,
  vul_arg<bool> &a_symmetric_3,
  vul_arg<bool> &a_symmetric_n,
  vul_arg<bool> &a_run_sel_geometry,
  vul_arg<bool> &a_read_sel,
  vul_arg<vcl_string> &a_sel_in1,
  vul_arg<vcl_string> &a_sel_in2,
  vul_arg<vcl_string> &a_sel_in3,
  vul_arg<bool> &a_synth_data_1,
  vul_arg<bool> &a_synth_data_2,
  vul_arg<bool> &a_synth_data_med,
  vul_arg<bool> &a_synth_data_3,
  vul_arg<bool> &a_run_synth_geometry,
  vul_arg<bool> &a_synth_write_edgels,
  vul_arg<double> &a_angle1,
  vul_arg<double> &a_angle2,
  vul_arg<double> &a_angle3,
  vul_arg<vcl_vector<double> > &a_angles,
  vul_arg<bool> &a_perturb_camera,
  vul_arg<bool> &a_write_perturb_camera,
  vul_arg<unsigned> &a_trinocular_nrad,
  vul_arg<double> &a_err_pos,
  vul_arg<bool> &a_remove_epitangency,
  vul_arg<vcl_string> &a_cam_type
    )
{
  prefix = &a_prefix;
  // XXX problem:  whether the arg "set" or not is not initialized if you don't
  // parse the arguments. When I run the app without parsing any arguments, this
  // will never be set.
  if (!(a_out_path.set())) {
    vcl_cout << "Out path not set\n";
    a_out_path.value_ = a_prefix.value_;
  } else {
    vcl_cout << "Out path SET\n";
  }
  out_path = &a_out_path;
  fname1 = &a_fname1;
  fname2 = &a_fname2;
  fname3 = &a_fname3;
  fnames = &a_fnames;
  corresp_gt = &a_corresp_gt;
  precomputed_epip = &a_precomputed_epip;
  edgels1 = &a_edgels1;
  edgels2 = &a_edgels2;
  edgels3 = &a_edgels3;
  edgels = &a_edgels;
  edgel_type = &a_edgel_type;
  run_trinocular = &a_run_trinocular;
  run_trinocular_t_diff = &a_run_trinocular_t_diff;
  run_trinocular_k_diff = &a_run_trinocular_k_diff;
  write_epipolar_corresp = &a_write_epipolar_corresp;
  read_epipolar_corresp = &a_read_epipolar_corresp;
  corresp_in = &a_corresp_in;
  compare_corresp = &a_compare_corresp;
  compute_epipolars = &a_compute_epipolars;
  symmetric_3 = &a_symmetric_3;
  symmetric_n = &a_symmetric_n;
  run_sel_geometry = &a_run_sel_geometry;
  read_sel = &a_read_sel;
  sel_in1 = &a_sel_in1;
  sel_in2 = &a_sel_in2;
  sel_in3 = &a_sel_in3;
  synth_data_1 = &a_synth_data_1;
  synth_data_2 = &a_synth_data_2;
  synth_data_3 = &a_synth_data_3;
  synth_data_med = &a_synth_data_med;
  run_synth_geometry = &a_run_synth_geometry;
  synth_write_edgels = &a_synth_write_edgels;
  angle1 = &a_angle1;
  angle2 = &a_angle2;
  angle3 = &a_angle3;
  angles = &a_angles;
  perturb_camera = &a_perturb_camera;
  write_perturb_camera = &a_write_perturb_camera;
  trinocular_nrad = &a_trinocular_nrad;
  err_pos = &a_err_pos;
  remove_epitangency = &a_remove_epitangency;
  cam_type = &a_cam_type;
}

//: \return false if any error occurred
bool mw_stereo_app::
init(mw_stereo_app_args &arg)
{
  
  // XXX XXX XXX 
  crop_ = true;

  nviews_ = 0;

  if (arg.fnames->value_.size()) {
    nviews_ = arg.fnames->value_.size();
    //    assert(nviews_ == arg->a_fnames().size());
    fnames_ = arg.fnames->value_;
  } else {
    fnames_.push_back(arg.fname1->value_);
    fnames_.push_back(arg.fname2->value_);
    fnames_.push_back(arg.fname3->value_);
  }

  if (arg.edgels->value_.size()) {
    if(nviews_!=0)
      assert(nviews_ == arg.edgels->value_.size());
    else
      nviews_ = arg.edgels->value_.size();
    edgel_fnames_ = arg.edgels->value_;
  } else {
    edgel_fnames_.push_back(arg.edgels1->value_);
    edgel_fnames_.push_back(arg.edgels2->value_);
    edgel_fnames_.push_back(arg.edgels3->value_);
  }

  edgel_type_ = arg.edgel_type->value_;

  if (arg.angles->value_.size()) {
    if(nviews_!=0)
      assert(nviews_ == arg.angles->value_.size());
    else
      nviews_ = arg.angles->value_.size();
    angles_ = arg.angles->value_;
    vcl_cout << "ANGLES (size " << arg.angles->value_.size() << "):\n";
    print_value(vcl_cout, *(arg.angles));
    vcl_cout << vcl_endl;
  } else {
    angles_.push_back(arg.angle1->value_);
    angles_.push_back(arg.angle2->value_);
    angles_.push_back(arg.angle3->value_);
  }

  if (nviews_ == 0)
    nviews_ = 3;

  symmetric_3_ = arg.symmetric_3->value_;
  symmetric_n_ = arg.symmetric_n->value_;

  assert(!(arg.symmetric_3->value_ && arg.symmetric_n->value_));

  if(   arg.synth_data_1->value_ 
     || arg.synth_data_2->value_ 
     || arg.synth_data_med->value_ 
     || arg.synth_data_3->value_) {
    // - generate 2 cameras + fm
    
    vnl_double_3x3 Kmatrix;

    if (arg.synth_data_1->value_) {
      nrows_ = 314;
      ncols_ = 600;
      dbdif_turntable::internal_calib_ctspheres(Kmatrix, ncols_);
    } else { // valid for synth data 2 and 3 alike
      nrows_ = 400;
      ncols_ = 500;
      unsigned  crop_origin_x_ = 450;
      unsigned  crop_origin_y_ = 1750;
      dbdif_turntable::internal_calib_olympus(Kmatrix, ncols_, crop_origin_x_, crop_origin_y_);
    }

    vpgl_calibration_matrix<double> K(Kmatrix);

    cam_.resize(nviews_);
    
    vpgl_perspective_camera<double> *P;

    if (arg.synth_data_1->value_) {
      for (unsigned i=0; i < nviews_; ++i) {
        P = dbdif_turntable::camera_ctspheres((unsigned)(angles_[i]), K);
        cam_[i].set_p(*P); delete P;
      }
    } else {// valid for synth data 2 and 3 alike
      for (unsigned i=0; i < nviews_; ++i) {
        P = dbdif_turntable::camera_olympus(angles_[i], K);
        cam_[i].set_p(*P); delete P;
      }
    }

    // Perturb 
    cam_gt_.resize(cam_.size());

    /*
    if (yaw_gt_ != 0) {
      for (unsigned i=0; i < nviews_; ++i) {
        cam_gt_[i] = cam_[i]; 
      }

      vpgl_perspective_camera<double> pr0 = app.cam_gt_[0].Pr_;
      //  double yaw = -13.0*vnl_math::pi/180.0;
      vgl_rotation_3d<double> rot(yaw,0,0);
      vgl_rotation_3d<double> rot_result = rot*pr0.get_rotation();
      pr0.set_rotation(rot_result);

      vpgl_perspective_camera<double> pr1 = app.cam_gt_[1].Pr_;
      vgl_rotation_3d<double> rot_result1 = rot*pr1.get_rotation();
      pr1.set_rotation(rot_result);
    }
    */

    for (unsigned i=0; i < nviews_; ++i) {
      cam_gt_[i] = cam_[i]; 
    }

    if (arg.perturb_camera->value_) {
      assert(nviews_ == 3 && arg.angles->value_.size()==0); // n-views not yet supported for this

      const double dtheta=0.5; // deg
      const double angle1_perturb = arg.angle1->value_+dtheta;
      const double angle2_perturb = arg.angle2->value_-dtheta;
      const double angle3_perturb = arg.angle3->value_+dtheta;

      if (arg.synth_data_1->value_) {
        vcl_cerr << "ERROR: perturbation of microCT cameras still not coded\n";
        abort();
      } else {// valid for synth data 2 and 3 alike
        // Using simple perturb criteria for now
        P = dbdif_turntable::camera_olympus(angle1_perturb, K);
        cam_[0].set_p(*P); delete P;
        P = dbdif_turntable::camera_olympus(angle2_perturb, K);
        cam_[1].set_p(*P); delete P;
        P = dbdif_turntable::camera_olympus(angle3_perturb, K);
        cam_[2].set_p(*P); delete P;
      }

      if (arg.write_perturb_camera->value_) {
        // open file
        // write cam vsl; code load cam vsl too. Check cam vsl test code

        // TODO: form string for fname. do for all 3 cams.

        vcl_vector<vcl_string> fnames(3); 
        
        {
        vcl_ostringstream s_angle, s_angle_perturb;
        s_angle << arg.angle1->value_; s_angle_perturb << angle1_perturb;
        fnames[0] = arg.out_path->value_ + vcl_string("out/cam-olympus") 
          + s_angle.str() + vcl_string("-perturb_") + s_angle_perturb.str() + vcl_string(".bvl");
        }

        {
        vcl_ostringstream s_angle, s_angle_perturb;
        s_angle << arg.angle2->value_; s_angle_perturb << angle2_perturb;
        fnames[1] = arg.out_path->value_ + vcl_string("out/cam-olympus") 
          + s_angle.str() + vcl_string("-perturb_") + s_angle_perturb.str() + vcl_string(".bvl");
        }

        {
        vcl_ostringstream s_angle, s_angle_perturb;
        s_angle << arg.angle3->value_; s_angle_perturb << angle3_perturb;
        fnames[2] = arg.out_path->value_ + vcl_string("out/cam-olympus") 
          + s_angle.str() + vcl_string("-perturb_") + s_angle_perturb.str() + vcl_string(".bvl");
        }

        for (unsigned i=0; i < 3; ++i ) {
          vsl_b_ofstream bp_out(fnames[i]);
          vcl_cout << "Writing camera file " << fnames[i] << vcl_endl;
          if (!bp_out) {
            vcl_cerr << "ERROR: while trying to write camera file " << fnames[i] << vcl_endl;
            abort();
          }

          b_write_dvpgl(bp_out, &(cam_[i].Pr_));
          bp_out.close();
        }
      }
    }

    vcl_cout << "Generating curves...\n"; vcl_cout.flush();

    if (arg.synth_data_1->value_) {
      dbdif_data::space_curves_ctspheres( crv3d_gt_ );
    } else {
      if (arg.synth_data_2->value_)
        dbdif_data::space_curves_olympus_turntable( crv3d_gt_ );
      else {
        if (arg.synth_data_3->value_)
          dbdif_data::space_curves_digicam_turntable_sandbox( crv3d_gt_ );
        else if (arg.synth_data_med->value_)
          dbdif_data::space_curves_digicam_turntable_medium_sized( crv3d_gt_ );
      }
    }

    // write to file would be here (todo)

    // If want to use SEL, make sure we have pixel spacing

    // eliminate epipolar tangency 
    if (arg.remove_epitangency->value_) {
      dbdif_data::project_into_cams_without_epitangency(crv3d_gt_, cam_gt_, crv2d_gt_, vnl_math::pi/6.0);
      vcl_cout << "Number of samples after removal of epipolar tangency: " 
        << crv2d_gt_[0].size() << vcl_endl;
    } else {
      dbdif_data::project_into_cams(crv3d_gt_, cam_gt_, crv2d_gt_);
      vcl_cout << "Number of samples INCLUDING epitangencies: " 
        << crv2d_gt_[0].size() << vcl_endl;
    }

    // - fill up vsols_ point vector if wanted

    vsols_.resize(nviews_);
    for (unsigned i=0; i < nviews_; ++i) {
      vsols_[i].resize(crv2d_gt_[i].size());
      for (unsigned  k=0; k < crv2d_gt_[i].size(); ++k) {
        vsols_[i][k] = new vsol_point_2d(crv2d_gt_[i][k].gama[0], crv2d_gt_[i][k].gama[1]);
      }
    }
    vcl_cout << "Done generating curves.\n";

    // Reconstruct from the known correspondence, as a check.

    //    vcl_vector<mw_vector_3d> C_rec;
    //    dbdif_rig rig(cam_gt_[0].Pr_,cam_[1].Pr_);
    //    rig.reconstruct_3d_curve(&C_rec,vsols_[0],vsols_[1]);

    //    mywritev(vcl_string("dat/synth_data3d_rec.dat"), C_rec);

    if (!symmetric_3_) {
      if (!symmetric_n_) {
        gt_.set_size(crv2d_gt_[0].size(), crv2d_gt_[1].size());
        for (unsigned i=0; i < crv2d_gt_[0].size(); ++i) {
          gt_.corresp_[i].push_back(mw_attributed_object(i));
        }
      } else { // N-tuple groundtruth
        vcl_vector<unsigned> npts(nviews_);
        npts[0] = crv2d_gt_[0].size();
        for (unsigned i=1; i < nviews_; ++i) {
          npts[i] = crv2d_gt_[i].size();
          assert (npts[i] == npts[i-1]);
        }
        gt_n_.set_size(npts);

        for (unsigned i=0; i < crv2d_gt_[0].size(); ++i) {
          mw_ntuplet tup(nviews_);
          for (unsigned iv=1; iv < nviews_; ++iv) {
            tup[iv] = i;
          }
          gt_n_.l_.put(tup,mw_match_attribute());
        }
      }
    } else {
      assert(nviews_ == 3);
      gt_3_.set_size(crv2d_gt_[0].size(), crv2d_gt_[1].size(), crv2d_gt_[2].size());
      assert (crv2d_gt_[0].size() == crv2d_gt_[1].size() && crv2d_gt_[1].size() == crv2d_gt_[2].size());

      for (unsigned i=0; i < crv2d_gt_[0].size(); ++i) {
        gt_3_.l_.put(i,i,i,mw_match_attribute());
      }
    }

    if (arg.synth_write_edgels->value_) {
      // generate edgels into vsol storage
      // write to file
      bool retval = write_gt_edgels(arg);

      if (!retval) return false;
    }

  } else {

    bool retval;

    retval = read_images(arg);
    if (!retval) return false;

    retval = read_cams(arg);
    if (!retval) return false;
    
    retval = read_edgels(arg);
    if (!retval) return false;

    // - read GT corresp
    if (arg.compare_corresp->value_) {
      vsl_b_ifstream bfs_in(arg.prefix->value_+arg.corresp_gt->value_);
      if (!bfs_in) {
        vcl_cout << "Error: unable to open file " << arg.prefix->value_ + arg.corresp_gt->value_ << vcl_endl;
        return false;
      }

      if (!symmetric_3_) {
        if (!symmetric_n_) {
          vsl_b_read(bfs_in, gt_3_);
          bfs_in.close();
          vcl_cout << gt_3_;
        } else {
          vsl_b_read(bfs_in, gt_n_);
          bfs_in.close();
          vcl_cout << gt_n_;
        }
      } else {
        vsl_b_read(bfs_in, gt_);
        bfs_in.close();
        vcl_cout << gt_;
      }
      vcl_cout << "Finished reading " << arg.prefix->value_ + arg.corresp_gt->value_ << vcl_endl;
    }
  } // ! arg.synth_data_1 && ! arg.synth_data_2
  
  // - read precomputed Epipolar geometry
  synth_data_1_ = arg.synth_data_1->value_;
  synth_data_2_ = arg.synth_data_2->value_;
  synth_data_3_ = arg.synth_data_3->value_;
  synth_data_med_ = arg.synth_data_med->value_;

  if (arg.read_epipolar_corresp->value_) {
    vsl_b_ifstream bfs_in(arg.prefix->value_+arg.precomputed_epip->value_);
    if (!bfs_in) {
      vcl_cout << "Error: unable to open file " << arg.prefix->value_ + arg.precomputed_epip->value_ << vcl_endl;
      return false;
    }
    have_precomputed_epi_corresp_= true;

    if (!symmetric_3_) {
      if (!symmetric_n_) {
        vsl_b_read(bfs_in, corr_ep_);
        bfs_in.close();
        vcl_cout << corr_ep_;
      } else {
        vsl_b_read(bfs_in, corr_ep_n_);
        bfs_in.close();
        vcl_cout << corr_ep_n_;
      }
    } else {
      vsl_b_read(bfs_in, corr_ep_3_);
      bfs_in.close();
      vcl_cout << corr_ep_3_;
    }
    vcl_cout << "Finished reading " << arg.prefix->value_ + arg.precomputed_epip->value_ << vcl_endl;
  } else {
    if (!symmetric_3_) {
      if (!symmetric_n_) {
        corr_ep_.set_size(vsols_[0].size(),vsols_[1].size());
      } else {
        vcl_vector<unsigned> npts(nviews_);
        for (unsigned i=0; i < nviews_; ++i) {
          npts[i] = vsols_[i].size();
        }
        corr_ep_n_.set_size(npts);
      }
    } else
      corr_ep_3_.set_size(vsols_[0].size(),vsols_[1].size(),vsols_[2].size());
    have_precomputed_epi_corresp_ = false;
  }

  // - read precomputed stereo result 

  if (/*arg.corresp_in->set() ||*/ arg.compare_corresp->value_) {
    vsl_b_ifstream bfs_in(arg.prefix->value_+arg.corresp_in->value_);
    if (!bfs_in) {
      vcl_cout << "Error: unable to open file " << arg.prefix->value_ + arg.corresp_in->value_ << vcl_endl;
      return false;
    }

    if (symmetric_3_) {
      vsl_b_read(bfs_in, corr_out_3_);
      bfs_in.close();
      vcl_cout << corr_out_3_;
    } else {
      if (symmetric_n_) {
        vsl_b_read(bfs_in, corr_out_n_);
        bfs_in.close();
        vcl_cout << corr_out_n_;
      } else {
        vsl_b_read(bfs_in, corr_out_);
        bfs_in.close();
        vcl_cout << corr_out_;
      }
    }
    vcl_cout << "Finished reading " << arg.prefix->value_ + arg.corresp_in->value_ << vcl_endl;

    have_stereo_result_corresp_= true;
  } else {
    if (!symmetric_3_) {
      if (!symmetric_n_) {
        corr_out_.set_size(vsols_[0].size(),vsols_[1].size());
        have_stereo_result_corresp_= true;
      } else {
        vcl_vector<unsigned> npts(nviews_);
        for (unsigned i=0; i < nviews_; ++i) {
          npts[i] = vsols_[i].size();
        }
        corr_out_n_.set_size(npts);
        have_stereo_result_corresp_= true;
      }
    } else {
      corr_out_3_.set_size(vsols_[0].size(),vsols_[1].size(),vsols_[2].size());
      have_stereo_result_corresp_= true;
    }
  }

  // sel - symbolic edge linking
  init_sel(arg);

  run_trinocular_ = arg.run_trinocular->value_;
  run_trinocular_t_diff_ = arg.run_trinocular_t_diff->value_;
  run_trinocular_k_diff_ = arg.run_trinocular_k_diff->value_;
  run_trinocular_DG_ = run_trinocular_t_diff_ || run_trinocular_k_diff_;

  if (run_trinocular_k_diff_ && !(synth_data_2_ || synth_data_3_||synth_data_med_)) {
    vcl_cerr << "ERROR: trinocular tangent difference currently requires synthetic data 2 or 3(turntable)\n";
    return false;
  }

  run_synth_geometry_ = arg.run_synth_geometry->value_;

  write_epipolar_corresp_  = arg.write_epipolar_corresp->value_;

  compare_correspondences_ = arg.compare_corresp->value_;

  compute_epipolars_  = run_trinocular_ || run_trinocular_DG_ 
    || run_sel_geometry_ || arg.compute_epipolars->value_ || run_synth_geometry_;

  trinocular_nrad_ = arg.trinocular_nrad->value_;
  err_pos_= arg.err_pos->value_;
  
  return true;
}

void mw_stereo_app::
run_stereo()
{
  fm_ = new vpgl_fundamental_matrix <double> (cam_[0].Pr_,cam_[1].Pr_);

  mw_point_matcher m(cam_);

  if (!have_precomputed_epi_corresp_) {
    if (compute_epipolars_) {
      if (!symmetric_3_) {
        if (!symmetric_n_) {
          vcl_cout << "Computing epipolar correspondents...";
          vcl_cout.flush();
          m.epipolar_constraint(vsols_, &corr_ep_, 0, 1,*fm_);
          vcl_cout << "done.\n";
          vcl_cout << corr_ep_;
          have_precomputed_epi_corresp_ = true;
        } else {
          vcl_cout << "Computing bucketing...";
          vcl_cout.flush();

          vcl_vector<mw_subpixel_point_set *> sp(nviews_);

          for (unsigned i=0; i < nviews_; ++i) {
            sp[i] = new mw_subpixel_point_set (vsols_[i]);
            sp[i]->build_bucketing_grid(nrows_,ncols_);
          }

          vcl_vector<vcl_vector <vpgl_fundamental_matrix<double> > > fms;


          fms.resize(nviews_);
          for (unsigned i=0; i < nviews_; ++i) {
            fms[i].resize(nviews_);
            for (unsigned k=0; k < nviews_; ++k) {
              if (k == i)
                continue;
              fms[i][k] = vpgl_fundamental_matrix <double> (cam_[i].Pr_,cam_[k].Pr_);
            }
          }

          vcl_cout << "done (bucketing).\n";

          if (!run_trinocular_DG_) {
            vcl_cout << "Computing n-view epipolar correspondents...";
            if (synth_data_2_ || synth_data_3_||synth_data_med_)
              mw_match_position_band(vsols_, &corr_ep_n_, fms, sp, err_pos_,true);
            else
              mw_match_position_band(vsols_, &corr_ep_n_, fms, sp, err_pos_,false);

            vcl_cout << "done (n-tuplet building).\n";
            vcl_cout << corr_ep_n_;
          } else {
            //
            // -symmetric_n -trinocular_tangent_diff 
            // TODO: we're using -trinocular to actually run multiview stereo.
            // Right thing to do would be to have a param -stereo_n
            //
            vcl_cout << "Computing integrated n-view epipolar and tangential correspondents...\n"; 
            vcl_cout.flush();
            const double err_t=vnl_math::pi/180.; //: TODO pass as param

            if (synth_data_2_ || synth_data_1_ || synth_data_3_ || synth_data_med_) {
              mw_match_tangent_band(vsols_, crv2d_gt_, &corr_ep_n_, fms, 
                  cam_, sp, err_pos_, err_t, false);// false = do not debug synthetic
            } else {
              vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > dg_points;
              build_point_tangents_from_edgels(dg_points);
              mw_match_tangent_band(vsols_, dg_points, &corr_ep_n_, fms, 
                  cam_, sp, err_pos_, err_t, false);// false = do not debug synthetic
            }
            vcl_cout << "done (n-tuplet building).\n";

            have_stereo_result_corresp_=true;
          }

          have_precomputed_epi_corresp_ = true;

          for (unsigned i=0; i < nviews_; ++i)
            delete sp[i];
        }
      } else {
        vcl_cout << "Computing trinocular epipolar correspondents...";
        assert (nviews_ == 3);

        vcl_vector<unsigned> npts(nviews_);
        for (unsigned i=0; i < nviews_; ++i) {
          npts[i] = vsols_[i].size();
        }
        corr_ep_n_.set_size(npts);

        vcl_cout << "Computing bucketing...";
        vcl_cout.flush();

        vcl_vector<mw_subpixel_point_set *> sp(nviews_);

        for (unsigned i=0; i < nviews_; ++i) {
          sp[i] = new mw_subpixel_point_set (vsols_[i]);
          sp[i]->build_bucketing_grid(nrows_,ncols_);
        }

        vcl_vector<vcl_vector <vpgl_fundamental_matrix<double> > > fms;

        fms.resize(nviews_);
        for (unsigned i=0; i < nviews_; ++i) {
          fms[i].resize(nviews_);
          for (unsigned k=0; k < nviews_; ++k) {
            if (k == i)
              continue;
            fms[i][k] = vpgl_fundamental_matrix <double> (cam_[i].Pr_,cam_[k].Pr_);
          }
        }

        vcl_cout << "done (bucketing).\n";

        vcl_cout << "Computing 3-view epipolar correspondents...";
        if (synth_data_2_ || synth_data_3_||synth_data_med_)
          mw_match_position_band(vsols_, &corr_ep_n_, fms, 
              sp, err_pos_,true); // true = debug synthetic
        else
          mw_match_position_band(vsols_, &corr_ep_n_, fms, 
              sp, err_pos_,false);// false = do not debug synthetic

        vcl_cout << "done (triplet building).\n";

        // Convert to corr_ep_3.

        vcl_cout << corr_ep_n_;

        vcl_cout << "======== Converting to 3 ========\n";
        corr_ep_3_ = mw_discrete_corresp_3(corr_ep_n_);
        vcl_cout << corr_ep_3_;
        have_precomputed_epi_corresp_ = true;

        for (unsigned i=0; i < nviews_; ++i) {
          delete sp[i];
        }

        /* Old one-way trinocoular reproj
        vcl_cout << "Computing bucketing...";
        vcl_cout.flush();

        mw_subpixel_point_set p_img3(vsols_[2]);
        p_img3.build_bucketing_grid(nrows_,ncols_);
        p_img3.set_nhood_radius(trinocular_nrad_);
        vcl_cout << "done.\n";
        m.epipolar_constraint_3(vsols_, &corr_ep_3_, 0, 1, 2,*fm_, p_img3);
        vcl_cout << "done.\n";
        vcl_cout << corr_ep_3_;
        have_precomputed_epi_corresp_ = true;
        */
      }
    }
  } else {
    vcl_cout << "NOT computing epipolar correspondents\n";
  }

  if (run_trinocular_ || run_trinocular_DG_ || run_sel_geometry_ || 
      run_synth_geometry_ || write_epipolar_corresp_) {
    if (!symmetric_3_) {
      if (!symmetric_n_)
        corr_out_ = corr_ep_;
      else
        corr_out_n_ = corr_ep_n_;
    } else
      corr_out_3_ = corr_ep_3_;
  }

  if (!symmetric_n_ && (run_trinocular_ || run_trinocular_DG_ /*|| run_n_view_DG_*/ )) {
    vcl_cout << "Running trinocular constraints\n";

    if (symmetric_3_) {
      if (synth_data_2_ || synth_data_1_ || synth_data_3_ || synth_data_med_) {
        m.trinocular_DG_costs_3(
          crv2d_gt_,
          &corr_out_3_,
          0,1,2,
          mw_point_matcher::C_THRESHOLD_TANGENT_AND_CURVATURE
         );
      } else {
        vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d;
        build_point_tangents_from_edgels(crv2d);

        m.trinocular_DG_costs_3(
          crv2d,
          &corr_out_3_,
          0,1,2,
          mw_point_matcher::C_THRESHOLD_TANGENT
         );

        // TODO sort??
      }

    } else {
        vcl_cout << "Computing bucketing...";
        mw_subpixel_point_set p_img3(vsols_[2]);
        p_img3.build_bucketing_grid(nrows_,ncols_);
        p_img3.set_nhood_radius(trinocular_nrad_);
        vcl_cout << "done.\n";

        if (run_trinocular_) {
          m.trinocular_costs(vsols_, p_img3, &corr_out_, 0, 1, 2);
        } else if (run_trinocular_t_diff_) {
          vcl_cout << "TANGENT ANGLE diff constraint\n";
          if (synth_data_2_ || synth_data_1_ || synth_data_3_ || synth_data_med_) { 
            m.trinocular_DG_costs(crv2d_gt_, p_img3, &corr_out_, 0, 1, 2, 
                mw_point_matcher::C_MIN_TANGENT);
          } else { 
            //: non-synthetic data, using edgel tangents
            
            vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d;
            build_point_tangents_from_edgels(crv2d);

            m.trinocular_DG_costs(crv2d, p_img3, &corr_out_, 0, 1, 2, 
                mw_point_matcher::C_TANGENT_CLOSEST_EDGEL);
          }
        } else if (run_trinocular_k_diff_) { //:< implies synthetic data
          vcl_cout << "CURVATURE diff constraint\n";
          m.trinocular_DG_costs(crv2d_gt_, p_img3, &corr_out_, 0, 1, 2, 
              mw_point_matcher::C_MIN_CURVATURE);
        }

        vcl_cout << "Sorting...";
        corr_out_.sort();
        vcl_cout << "done.\n";
    }

    have_stereo_result_corresp_=true;
  }

  if (compute_sel_)
    compute_sel();

  if (run_synth_geometry_) {
    if (symmetric_3_ || symmetric_n_) {
      // TODO
      vcl_cerr << "Not yet coded\n";
      abort();
    }
    vcl_cout << "Running geometry constraints on synthetic data\n";
    m.synthetic_geometry_costs(crv2d_gt_, &corr_out_, 0 , 1);
    vcl_cout << "Sorting...";
    corr_out_.sort();
    vcl_cout << "done.\n";
    have_stereo_result_corresp_=true;
  }

  if (compare_correspondences_ && have_stereo_result_corresp_) {
    if (symmetric_3_) {
      corr_out_3_.compare_and_print(&gt_3_);
    } else {
      if (symmetric_n_) {
        abort();
      } else {
        corr_out_.compare_and_print(&gt_);
      }
    }
  }
}

void mw_stereo_app::
build_point_tangents_from_edgels(vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > &crv2d) const
{
  crv2d.resize(nviews_);
  for (unsigned iv=0; iv < nviews_; ++iv) {
    crv2d[iv].resize(vsols_orig_cache_[iv].size());
    for (unsigned ip=0; ip < vsols_orig_cache_[iv].size(); ++ip) {
      crv2d[iv][ip].gama[0]  = vsols_[iv][ip]->x();
      crv2d[iv][ip].gama[1]  = vsols_[iv][ip]->y();
      crv2d[iv][ip].gama[2]  = 0;
      
      vgl_vector_2d<double> dir = normalized(vsols_orig_cache_[iv][ip]->direction());

      crv2d[iv][ip].t[0] = dir.x();
      crv2d[iv][ip].t[1] = dir.y();
      crv2d[iv][ip].t[2] = 0;
      crv2d[iv][ip].n[0] = -dir.y();
      crv2d[iv][ip].n[1] = dir.x();
      crv2d[iv][ip].n[2] = 0;
      crv2d[iv][ip].k = 0; 
      crv2d[iv][ip].kdot = 0;
      crv2d[iv][ip].valid = true;
    }
  }
}

bool mw_stereo_app::
write_results(vcl_string &path_corresp_out, vcl_string &path_precomputed_epip)
{
  // write results of match

  if (run_trinocular_ || run_trinocular_DG_ || run_sel_geometry_ || run_synth_geometry_ /* || any other constraints writing to corr_out_ */)
  {
    vsl_b_ofstream bfs_out(path_corresp_out);
    if (!bfs_out) {
      vcl_cout << "Error: unable to open " << path_corresp_out << vcl_endl;
      return false;
    }

    if (symmetric_3_) {
      vsl_b_write(bfs_out, corr_out_3_);
      bfs_out.close();
      vcl_cout << corr_out_3_;
    } else {
      if (symmetric_n_) {
        vsl_b_write(bfs_out, corr_out_n_);
        bfs_out.close();
        vcl_cout << corr_out_n_;
      } else {
        vsl_b_write(bfs_out, corr_out_);
        bfs_out.close();
        vcl_cout << corr_out_;
      }
    }
    vcl_cout << "Finished writing " << path_corresp_out << vcl_endl;
  }
  
  // write epip. corresp in case no arg provided
  if (write_epipolar_corresp_) {
    vsl_b_ofstream bfs_out(path_precomputed_epip);
    if (!bfs_out) {
      vcl_cout << "Error: unable to open " << path_precomputed_epip << vcl_endl;
      return false;
    }

    if (symmetric_3_) {
      vsl_b_write(bfs_out, corr_out_3_);
    } else {
      if (symmetric_n_)
        vsl_b_write(bfs_out, corr_out_n_);
      else
        vsl_b_write(bfs_out, corr_out_);
    }

    bfs_out.close();
    vcl_cout << "Finished writing " << path_precomputed_epip << vcl_endl;
  }

  return true;
}

//: Outputs vsols_ and vsols_orig_cache_
//
bool mw_stereo_app::
read_edgels(mw_stereo_app_args &arg)
{
  vsols_.resize(nviews_);
  vsols_orig_cache_.resize(nviews_);

  if (arg.edgel_type->value_ == "EDG" || arg.edgel_type->value_ == "edg") {

    for (unsigned v=0; v < nviews_; ++v) {
      bool bSubPixel=true, blines=true;
      double scale=1.0;

      // edgels (vsol)
      vcl_vector< vsol_spatial_object_2d_sptr > edgels;

      bool retval =dbdet_load_edg(
          arg.prefix->value_ + edgel_fnames_[v],
          bSubPixel,
          blines,
          scale,
          edgels
          );

      if (!retval)
        return false;

      // cast edgels to vsol_line_2d_sptr

      vsols_orig_cache_[v].resize(edgels.size());
      vsols_[v].resize(edgels.size());
      for (unsigned i=0; i < edgels.size(); ++i) {
        vsols_orig_cache_[v][i] = new vsol_line_2d(*(edgels[i]->cast_to_curve()->cast_to_line()));
        vsols_[v][i] = vsols_orig_cache_[v][i]->middle();
      }

      vcl_cout << "Number of points in view " << v << ": " << vsols_[v].size() << vcl_endl;
    }
  } else {
    vidpro1_vsol2D_storage_sptr model = vidpro1_vsol2D_storage_new();
    model->register_binary_io();
    for (unsigned v=0; v < nviews_; ++v) {
      vsl_b_ifstream bp_in(arg.prefix->value_ + edgel_fnames_[v]);
      if (!bp_in) {
        vcl_cout << " Error opening file  " << arg.prefix->value_ +edgel_fnames_[v]  << vcl_endl;
        return false;
      }

      vcl_cout << "Opened " << edgel_fnames_[v] <<  " for reading" << vcl_endl;

      vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
      output_vsol->b_read(bp_in);
      bp_in.close();
      vcl_cout << "Finished reading " << edgel_fnames_[v]<<  vcl_endl;

      vcl_cout << "Storage named " << output_vsol->name() << vcl_endl;


      vcl_vector< vsol_spatial_object_2d_sptr > vsol_list= output_vsol->all_data();

      vsols_[v].clear();
      vsols_orig_cache_[v].clear();
      unsigned  max_sz = vsol_list.size();
      vsols_[v].reserve(max_sz);
      vsols_orig_cache_[v].reserve(max_sz);
      unsigned id_line = 0;
      for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) {
        if( vsol_list[b]->cast_to_curve()){
          // Line
          if( vsol_list[b]->cast_to_curve()->cast_to_line() ){
  //            if (v < 3 || (v == 3 && id_line >= 6707 && id_line <= 6707))
              vsol_line_2d_sptr eline = vsol_list[b]->cast_to_curve()->cast_to_line();
              vsols_[v].push_back(eline->middle());
              vsols_orig_cache_[v].push_back(eline);
            ++id_line;
          }
        }
      }

      vcl_cout << "Number of points in view " << v << ": " << vsols_[v].size() << vcl_endl;
    }
  }

  if (crop_) {
    // output random # of vsols_ and vsols_orig_cache_ within crop region
    unsigned nsamples = 10;
    if (vsols_[0].size() <=nsamples)
      return true;
    double ox = 330, oy=15, wx=780, wy=640; // currently specific for Capitol dataset
    vcl_cout << "WARNING: currently specific for capitol dataset\n";

    for (unsigned i=0; i < vsols_.size(); ++i) {
      vcl_cout << "view " << i << ", ini #pts: " << vsols_[i].size() << "; " 
        << vsols_orig_cache_[i].size() << vcl_endl;
    }

//    for (unsigned iv=0; iv < nviews_; ++iv) {
    unsigned iv=0;
      vcl_vector<bool> include(vsols_[iv].size(),false);
      unsigned ns = 0;
      do {
        // pick random index
        unsigned id = myrand.lrand32(vsols_[iv].size()-1);
        if (vsols_[iv][id]->x() >= ox && vsols_[iv][id]->x() <= ox + wx && 
            vsols_[iv][id]->y() >= oy && vsols_[iv][id]->y() <= oy + wy) {
          vcl_cout << "Including random id: " << id << "\n";
          ns++;
          include[id] = true;
        }
      } while (ns < nsamples);

      vcl_vector<vsol_point_2d_sptr> new_vsols_;
      vcl_vector<vsol_line_2d_sptr> new_vsols_orig_;
      new_vsols_.reserve(ns);
      new_vsols_orig_.reserve(ns);

      for (unsigned i=0; i < vsols_[iv].size(); ++i) {
        if (include[i]) {
          new_vsols_.push_back(vsols_[iv][i]);
          new_vsols_orig_.push_back(vsols_orig_cache_[iv][i]);
        }
      }

      vsols_[iv] = new_vsols_;
      vsols_orig_cache_[iv] = new_vsols_orig_;
//    }

    for (unsigned i=0; i < vsols_.size(); ++i) {
      vcl_cout << "view " << i << ", new #pts: " << vsols_[i].size() << "; " 
        << vsols_orig_cache_[i].size() << vcl_endl;
    }
  }

  return true;
}

bool mw_stereo_app::
write_gt_edgels(mw_stereo_app_args &arg)
{
  for (unsigned  iv=0; iv < nviews_; ++iv) {

    vcl_vector<vsol_line_2d_sptr> lines;
    dbdif_data::get_lines(lines, crv2d_gt_[iv], false);

    vcl_vector< vsol_spatial_object_2d_sptr > edgels;

    const double scale=1;
    for (unsigned i=0; i<lines.size(); i++) {
      lines[i]->set_length(lines[i]->length()*scale);
      edgels.push_back(lines[i]->cast_to_spatial_object());
    }

    // create the output storage class
    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    output_vsol->add_objects(edgels, "edgels");
    
    vcl_string fname = arg.out_path->value_ + vcl_string("/") + edgel_fnames_[iv];

    vsl_b_ofstream bp_out(fname);

    if (!bp_out){
      vcl_cout << " Error opening file  " << fname << vcl_endl;
      return false;
    }

    vcl_cout << "Created " << fname << " for writing vsol\n";

    output_vsol->b_write(bp_out);

    bp_out.close();

  }
  return true;
}

void mw_stereo_app::
init_sel(mw_stereo_app_args &arg)
{
  if (arg.read_sel->value_) { 
    vcl_cout << "Reading precomputed SELs\n";
    // TODO
    compute_sel_ = false;
  } else 
    compute_sel_ = arg.run_sel_geometry->value_;

  run_sel_geometry_   = arg.run_sel_geometry->value_;
}

bool mw_stereo_app::
read_images(mw_stereo_app_args &arg)
{
  for (unsigned i=0; i < nviews_; ++i) {
    images_.push_back(vil_load_image_resource( (arg.prefix->value_ + fnames_[i]).c_str() ) );
    if( !images_.back()) {
      vcl_cerr << "Failed to load image file " << arg.prefix->value_ + fnames_[i] << vcl_endl;
      return false;
    }
    ncols_ = images_[i]->ni();
    nrows_ = images_[i]->nj();

    vcl_cout << "Image view # " << i << "  ";
    vcl_cout << "Nrows: " << nrows_;
    vcl_cout << "  Ncols: " << ncols_ << vcl_endl;
  }
  return true;
}

bool mw_stereo_app::
read_cams(mw_stereo_app_args &arg)
{
    // camfiletype parameter:
#define INTRINSIC_EXTRINSIC 1 
#define PROJCAMERA  2 

  int cam_type;

  if (arg.cam_type->value_ == "intrinsic_extrinsic") {
    cam_type = INTRINSIC_EXTRINSIC;
  } else {
    if (arg.cam_type->value_ == "projcamera")
      cam_type = PROJCAMERA;
    else  {
      vcl_cerr << "Error: invalid camera type " << arg.cam_type->value_ << vcl_endl;
      return false;
    }
  }

  cam_.resize(nviews_);

  for (unsigned i=0; i < nviews_; ++i) {

    vpgl_perspective_camera<double> mycam;

    if (cam_type == INTRINSIC_EXTRINSIC ) {
      vcl_string cam_fname = arg.prefix->value_ + fnames_[i];
      if (! read_cam(cam_fname, &mycam) ) {
        vcl_cout << "Error reading cam " << cam_fname << vcl_endl;
         return false;
      }
    } else { //cam_type == PROJCAMERA
      vcl_string cam_fname = arg.prefix->value_ + fnames_[i];
      if (! read_3x4_matrix_into_cam( cam_fname ,&mycam) ) {
        vcl_cout << "Error reading cam " << cam_fname << vcl_endl;
         return false;
      }
    }

    cam_[i].set_p(mycam);
    vcl_cout << "Camera #" << i << vcl_endl << cam_[i].Pr_.get_matrix() << vcl_endl;
  }
  return true;
}

void mw_stereo_app::
compute_sel()
{
    vcl_cout << "Computing SEL geometry" << vcl_endl;
    vcl_cout << "Code not yet updated." << vcl_endl;
    abort();


#if 0
    sel_.resize(nviews_);

    for (unsigned i=0; i < 2 /*nviews_*/; ++i) {

      vcl_cout  << "SEL: doing view " << i << vcl_endl;
      vcl_vector<dbdet_edgel *> all_edgels(vsols_orig_cache_[i].size());

      for (unsigned k=0; k < vsols_orig_cache_[i].size(); ++k) { 
        vsol_line_2d_sptr eline = vsols_orig_cache_[i][k];

        vgl_point_2d<double> spt(eline->p0()->x(), eline->p0()->y());
        vgl_point_2d<double> ept(eline->p1()->x(), eline->p1()->y());
        double tan = dbdet_vPointPoint(spt, ept);
        
        all_edgels[k] = new dbdet_edgel(vsols_[i][k]->get_p(), tan);
      }

      sel_[i] = new dbdet_sel<dbdet_ES_curve_model>(nrows_, ncols_, 3, vnl_math::pi*10.0/180.00, 0.2); //: < for now use hardcoded values
      sel_[i]->build_edgel_neighborhoods(all_edgels);
      sel_[i]->build_pairs();
      sel_[i]->build_triplets();
      sel_[i]->build_quadruplets();
      sel_[i]->report_stats();
    }
#endif

  /*
  if (run_sel_geometry_) {
    if (symmetric_3_ || symmetric_n_) {
      // TODO
      vcl_cerr << "Not yet coded\n";
      abort();
    }

    vcl_cout << "Running sel geometry constraints\n";

    m.sel_geometry_costs(vsols_, sel_, &corr_out_, 0 , 1);
    vcl_cout << "Sorting...";
    corr_out_.sort();
    vcl_cout << "done.\n";
    have_stereo_result_corresp_=true;
  }
  */
}
