// This is brcv/mvg/dvpgl/algo/dvpgl_DG_bundle_adjust.cxx

//:
// \file

#include "dvpgl_DG_bundle_adjust.h"
#include <vnl/vnl_math.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_sparse_lm.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>

#include <dbdif/dbdif_camera.h>
#include <mw/algo/mw_algo_util.h>

//: Constructor
dvpgl_DG_bundle_adj_lsqr::
  dvpgl_DG_bundle_adj_lsqr(const vcl_vector<vpgl_calibration_matrix<double> >& K,
                        const vcl_vector<dbdif_3rd_order_point_2d>& image_points,
                        const vcl_vector<vcl_vector<bool> >& mask,
                        bool use_confidence_weights)
 : vnl_sparse_lst_sqr_function(K.size(),6,mask[0].size(),3,0,mask,3,use_gradient),
   K_(K),
   image_points_(image_points),
   use_covars_(false),
   use_weights_(use_confidence_weights),
   weights_(image_points.size(),1.0),
   iteration_count_(0)
{
}

/*

//: Constructor
//  Each image point is assigned an inverse covariance (error projector) matrix
// \note image points are not homogeneous because they require finite points to measure projection error
dvpgl_DG_bundle_adj_lsqr::
dvpgl_DG_bundle_adj_lsqr(const vcl_vector<vpgl_calibration_matrix<double> >& K,
                      const vcl_vector<vgl_point_2d<double> >& image_points,
                      const vcl_vector<vnl_matrix<double> >& inv_covars,
                      const vcl_vector<vcl_vector<bool> >& mask,
                      bool use_confidence_weights)
 : vnl_sparse_lst_sqr_function(K.size(),6,mask[0].size(),3,mask,2,use_gradient),
   K_(K),
   image_points_(image_points),
   use_covars_(true),
   use_weights_(use_confidence_weights),
   weights_(image_points.size(),1.0),
   iteration_count_(0)
{
  vcl_cerr << "Not implemented.\n";
  abort();
  assert(image_points.size() == inv_covars.size());
  vnl_matrix<double> U(2,2,0.0);
  for(unsigned i=0; i<inv_covars.size(); ++i){
    const vnl_matrix<double>& S = inv_covars[i];
    if(S(0,0) > 0.0){
      U(0,0) = vcl_sqrt(S(0,0));
      U(0,1) = S(0,1)/U(0,0);
      double U11 = S(1,1)-S(0,1)*S(0,1)/S(0,0);
      U(1,1) = (U11>0.0)?vcl_sqrt(U11):0.0;
    }
    else if(S(1,1) > 0.0){
      assert(S(0,1) == 0.0);
      U(0,0) = 0.0;
      U(0,1) = 0.0;
      U(1,1) = vcl_sqrt(S(1,1));
    }
    else{
      vcl_cout << "warning: not positive definite"<<vcl_endl;
      U.fill(0.0);
    }
    factored_inv_covars_.push_back(U);
  }
}

*/

//: Compute all the reprojection errors
//  Given the parameter vectors a and b, compute the vector of residuals e.
//  e has been sized appropriately before the call.
//  The parameters in a for each camera are {wx, wy, wz, tx, ty, tz}
//  where w is the Rodrigues vector of the rotation and t is the translation.
//  The parameters in b for each 3D point are {px, py, pz} 
//  the non-homogeneous position.
void
dvpgl_DG_bundle_adj_lsqr::f(vnl_vector<double> const& a, vnl_vector<double> const& b,
                         vnl_vector<double>& e)
{
  typedef vnl_crs_index::sparse_vector::iterator sv_itr;

  vcl_vector<vpgl_perspective_camera<double> > Pi_vec(number_of_a());
  vcl_vector<dbdif_camera> cam(number_of_a());
  for(unsigned int i=0; i<number_of_a(); ++i) {
    Pi_vec[i] = param_to_cam(i,a);
    cam[i].set_p(Pi_vec[i]);
  }

  double total_rms=0;
  unsigned my_n_e=0;
  double dpos_rms=0, dtheta_rms=0, dnormal_plus_rms=0;
  vcl_vector<double> dthetas;
  vcl_vector<double> dnormal_plus_v;
  unsigned n_total_dg_err=0;

  for(unsigned int i=0; i<number_of_a(); ++i)
  {
    //: Construct the ith camera
    vpgl_perspective_camera<double> &Pi = Pi_vec[i];

    vnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for(sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
    {
      unsigned int j = r_itr->second;
      unsigned int k = r_itr->first;

      // Construct the jth point
      vgl_homg_point_3d<double> Xj = param_to_point(j,b);

      // Project jth point with the ith camera
      vgl_homg_point_2d<double> xij = Pi(Xj);

      double* eij = e.data_block()+index_e(k);
      eij[0] = xij.x()/xij.w() - image_points_[k].gama[0];
      eij[1] = xij.y()/xij.w() - image_points_[k].gama[1];

      // ---- Compute DG reprojection error ----

      unsigned i0 = k % residual_indices_.num_cols();
      
      //: Current implementation does not allow for incomplete corresps
//      assert(i0 == (unsigned)residual_indices_.sparse_col(i)[0].first);

      vcl_vector<dbdif_3rd_order_point_2d> pts(number_of_a());
      for (unsigned iv=0; iv < number_of_a(); ++iv) {
        pts[iv] = image_points_[i0 + iv*residual_indices_.num_cols()];
      }
      
      double dpos;
      double dtheta;
      double dnormal_plus;
      double dnormal_minus;
      double dtangential_plus;
      double dtangential_minus;
      double dk;
      double dkdot;
      unsigned n;

      dpos=dtheta=dk=dkdot=0;
      n =0;

      bool one_true = mw_algo_util::dg_reprojection_error(pts,cam,i,
          dpos,dtheta,
          dnormal_plus,
          dnormal_minus,
          dtangential_plus,
          dtangential_minus,
          dk,dkdot,n);
      if (!one_true)
        eij[2] = 0;
      else {
        eij[2] = vcl_sqrt(dtheta/n);

        dthetas.push_back(dtheta/n);
        dnormal_plus_v.push_back(dnormal_plus/n);
//        if (dthetas.size() == 1128)
//          vcl_cout << "*** MAX err (1128):" << dtheta/n << " points: " << vcl_endl;
//        for (unsigned qq=0; qq < pts.size() ; ++qq) {
//          vcl_cout << pts[i] << vcl_endl;
//        }
        n_total_dg_err += n;
      }

      // RMS

      total_rms += eij[0]*eij[0] + eij[1]*eij[1] + eij[2]*eij[2];
      dpos_rms += eij[0]*eij[0] + eij[1]*eij[1];
      dtheta_rms += eij[2]*eij[2];
      dnormal_plus_rms += dnormal_plus_v.back();
      my_n_e+=3;

      if(use_covars_){
        vcl_cerr << "Not coded\n";
        abort();
        // multiply this error by upper triangular Sij
        vnl_matrix<double>& Sij = factored_inv_covars_[k];
        eij[0] *= Sij(0,0);
        eij[0] += eij[1]*Sij(0,1);
        eij[1] *= Sij(1,1);
      }
    }
  }

  assert(my_n_e == e.size());
  total_rms = vcl_sqrt(total_rms/e.size());
  dtheta_rms = vcl_sqrt(dtheta_rms/(e.size()/3.0));
  dnormal_plus_rms = vcl_sqrt(dnormal_plus_rms/(e.size()/3.0));
  dpos_rms = vcl_sqrt(dpos_rms/(2.0*e.size()/3.0));

  unsigned dummy;
  vcl_cout << "     MY Total RMS: " << total_rms << vcl_endl
           << "         dpos RMS: " << dpos_rms << vcl_endl
           << "       dtheta RMS: " << dtheta_rms << " (max: " << vcl_sqrt(mw_util::max(dthetas,dummy))
           << ", min: " << vcl_sqrt(mw_util::min(dthetas,dummy))
           << ", avg: " << vcl_sqrt(mw_util::mean(dthetas)) << ")" << vcl_endl

           << " dnormal_plus RMS: " << dnormal_plus_rms << " (max: " << vcl_sqrt(mw_util::max(dnormal_plus_v,dummy))
           << ", min: " << vcl_sqrt(mw_util::min(dnormal_plus_v,dummy))
           << ", avg: " << vcl_sqrt(mw_util::mean(dnormal_plus_v)) << ")"
           << vcl_endl;

  if(use_weights_ && iteration_count_++ > 50 ){
    vcl_cerr << "Not coded\n";
    abort();
    vnl_vector<double> unweighted(e);
    for(unsigned int k=0; k<weights_.size(); ++k){
      e[2*k]   *= weights_[k];
      e[2*k+1] *= weights_[k];
    }
    // weighted average error
    double avg_error = e.rms();
    for(unsigned int k=0; k<weights_.size(); ++k){
      vnl_vector_ref<double> uw(2,unweighted.data_block()+2*k);
      double update = 2.0*avg_error/uw.rms();
      if(update < 1.0)
        weights_[k] = vcl_min(weights_[k], update);
      else
        weights_[k] = 1.0;
      //vcl_cout << weights_[k] << " ";  
      e[2*k]   = unweighted[2*k]   * weights_[k];
      e[2*k+1] = unweighted[2*k+1] * weights_[k];
    }
    vcl_cout << vcl_endl;
  }
}


//: Compute the sparse Jacobian in block form.
void
dvpgl_DG_bundle_adj_lsqr::jac_blocks(vnl_vector<double> const& a, vnl_vector<double> const& b,
                                  vcl_vector<vnl_matrix<double> >& A,
                                  vcl_vector<vnl_matrix<double> >& B)
{
  const double stepsize = 0.001;

  vcl_vector<vpgl_perspective_camera<double> > Pi_vec(number_of_a());
  vcl_vector<dbdif_camera> cam(number_of_a());
  for(unsigned int i=0; i<number_of_a(); ++i) {
    Pi_vec[i] = param_to_cam(i,a);
    cam[i].set_p(Pi_vec[i]);
  }

  typedef vnl_crs_index::sparse_vector::iterator sv_itr;
  for(unsigned int i=0; i<number_of_a(); ++i)
  {
    //: Construct the ith camera
    vnl_double_3x4 Pi = Pi_vec[i].get_matrix();

    vnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
    for(sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
    {
      unsigned int j = r_itr->second;
      unsigned int k = r_itr->first;
      // This is semi const incorrect - there is no vnl_vector_ref_const
      const vnl_vector_ref<double> bj(number_of_params_b(j),
                                      const_cast<double*>(b.data_block())+index_b(j));

      jac_Bij(Pi,bj,B[k]);  // compute Jacobian B_ij
      if(use_covars_)
        B[k] = factored_inv_covars_[k]*B[k];
    }

    // This is semi const incorrect - there is no vnl_vector_ref_const
    const vnl_vector_ref<double> ai(number_of_params_a(i),
                                    const_cast<double*>(a.data_block())+index_a(i));
    vnl_vector<double> tai = ai;
    for (unsigned int ii = 0; ii < 6; ++ii)
    {
      // calculate camera just to the right of ai[ii]
      double tplus = tai[ii] = ai[ii] + stepsize;
      vpgl_perspective_camera<double> Pi_plus = param_to_cam(i,tai.data_block());

      // calculate camera just to the left of ai[ii]
      double tminus = tai[ii] = ai[ii] - stepsize;
      vpgl_perspective_camera<double> Pi_minus = param_to_cam(i,tai.data_block());

      double h = 1.0 / (tplus - tminus);

      vnl_crs_index::sparse_vector row = residual_indices_.sparse_row(i);
      for(sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
      {
        unsigned int j = r_itr->second;
        unsigned int k = r_itr->first;

        // Construct the jth point
        const double* bj = b.data_block()+index_b(j);
        vgl_homg_point_3d<double> Xj(bj[0],bj[1],bj[2]);
        // Project jth point with the ith cameras
        vgl_homg_point_2d<double> xij_plus = Pi_plus(Xj);
        vgl_homg_point_2d<double> xij_minus = Pi_minus(Xj);

        vnl_matrix<double>& Aij = A[k];
        Aij(0,ii) = (xij_plus.x()/xij_plus.w() - xij_minus.x()/xij_minus.w()) * h * weights_[k];
        Aij(1,ii) = (xij_plus.y()/xij_plus.w() - xij_minus.y()/xij_minus.w()) * h * weights_[k];

        //: compute DG error by considering image points and camera_minus/camera_plus; 

        cam[i].set_p(Pi_plus);

//        unsigned i0 = (k / residual_indices_.num_cols())*residual_indices_.num_cols();
        unsigned i0 = k % residual_indices_.num_cols();
        
        //: Current implementation does not allow for incomplete corresps
//        assert(i0 == (unsigned)residual_indices_.sparse_row(i)[0].first);

        vcl_vector<dbdif_3rd_order_point_2d> pts(number_of_a());
        for (unsigned iv=0; iv < number_of_a(); ++iv) {
          pts[iv] = image_points_[i0 + iv*residual_indices_.num_cols()];
        }
        
        double dg_err_plus;
        {
          double dpos;
          double dtheta;
          double dnormal_plus;
          double dnormal_minus;
          double dtangential_plus;
          double dtangential_minus;
          double dk;
          double dkdot;
          unsigned n;

          dpos=dtheta=dk=dkdot=0;
          n =0;

          bool one_true = mw_algo_util::dg_reprojection_error(pts,cam,i,
              dpos,dtheta,
              dnormal_plus,
              dnormal_minus,
              dtangential_plus,
              dtangential_minus,
              dk,dkdot,n);
          if (!one_true)
            dg_err_plus = 0;
          else
            dg_err_plus = dtheta;
        }

        cam[i].set_p(Pi_minus);
        double dg_err_minus;
        {
          double dpos;
          double dtheta;
          double dnormal_plus; 
          double dnormal_minus;
          double dtangential_plus;
          double dtangential_minus;
          double dk;
          double dkdot;
          unsigned n;

          dpos=dtheta=dk=dkdot=0;
          n =0;

          bool one_true = mw_algo_util::dg_reprojection_error(pts,cam,i,
              dpos,dtheta,
              dnormal_plus,
              dnormal_minus,
              dtangential_plus,
              dtangential_minus,
              dk,dkdot,n);
          if (!one_true)
            dg_err_minus = 0;
          else
            dg_err_minus = dtheta;
        }
        cam[i].set_p(Pi_vec[i]);

        Aij(2,ii) = (dg_err_plus - dg_err_minus)* h * weights_[k];

        if(use_covars_){
          vcl_cerr << "Not coded\n";
          abort();
          // multiple this column of A by upper triangular Sij
          vnl_matrix<double>& Sij = factored_inv_covars_[k];
          Aij(0,ii) *= Sij(0,0);
          Aij(0,ii) += Aij(1,ii)*Sij(0,1);
          Aij(1,ii) *= Sij(1,1);
        }
      }

      // restore tai
      tai[ii] = ai[ii];
    }
  }
}


//: compute the Jacobian Bij
void
dvpgl_DG_bundle_adj_lsqr::jac_Bij(vnl_double_3x4 const& Pi, vnl_vector<double> const& bj,
                               vnl_matrix<double>& Bij)
{
  double denom = Pi(2,0)*bj[0] + Pi(2,1)*bj[1] + Pi(2,2)*bj[2] + Pi(2,3);
  denom *= denom;
  
  double txy = Pi(0,0)*Pi(2,1) - Pi(0,1)*Pi(2,0);
  double txz = Pi(0,0)*Pi(2,2) - Pi(0,2)*Pi(2,0);
  double tyz = Pi(0,1)*Pi(2,2) - Pi(0,2)*Pi(2,1);
  double tx  = Pi(0,0)*Pi(2,3) - Pi(0,3)*Pi(2,0);
  double ty  = Pi(0,1)*Pi(2,3) - Pi(0,3)*Pi(2,1);
  double tz  = Pi(0,2)*Pi(2,3) - Pi(0,3)*Pi(2,2);

  Bij(0,0) = (txy*bj[1] + txz*bj[2] + tx) / denom;
  Bij(0,1) = (txy*bj[0] + tyz*bj[2] + ty) / denom;
  Bij(0,2) = (txz*bj[0] + tyz*bj[1] + tz) / denom;

  txy = Pi(1,0)*Pi(2,1) - Pi(1,1)*Pi(2,0);
  txz = Pi(1,0)*Pi(2,2) - Pi(1,2)*Pi(2,0);
  tyz = Pi(1,1)*Pi(2,2) - Pi(1,2)*Pi(2,1);
  tx  = Pi(1,0)*Pi(2,3) - Pi(1,3)*Pi(2,0);
  ty  = Pi(1,1)*Pi(2,3) - Pi(1,3)*Pi(2,1);
  tz  = Pi(1,2)*Pi(2,3) - Pi(1,3)*Pi(2,2);

  Bij(1,0) = (txy*bj[1] + txz*bj[2] + tx) / denom;
  Bij(1,1) = (txy*bj[0] + tyz*bj[2] + ty) / denom;
  Bij(1,2) = (txz*bj[0] + tyz*bj[1] + tz) / denom;

  //XXX These are the values of the DG reprojection error by changing 3D points
  Bij(2,0) = 0;
  Bij(2,1) = 0;
  Bij(2,2) = 0;
}


//: Create the parameter vector \p a from a vector of cameras
vnl_vector<double> 
dvpgl_DG_bundle_adj_lsqr::create_param_vector(const vcl_vector<vpgl_perspective_camera<double> >& cameras)
{
  vnl_vector<double> a(6*cameras.size(),0.0);
  for(unsigned int i=0; i<cameras.size(); ++i){
    const vpgl_perspective_camera<double>& cam = cameras[i];
    const vgl_point_3d<double>& c = cam.get_camera_center();
    vnl_matrix<double> R(cam.get_rotation().as_matrix());

    // compute the Rodrigues vector from the rotation
    vnl_vector<double> w(3,0.0);
    w[0] = R(2,1)-R(1,2);
    w[1] = R(0,2)-R(2,0);
    w[2] = R(1,0)-R(0,1);
    w.normalize();
    w *= vcl_acos(vcl_sqrt( R(0,0)+R(1,1)+R(2,2)+1.0)/2.0)*2.0;
    
    double* ai = a.data_block() + i*6;
    ai[0]=w[0];   ai[1]=w[1];   ai[2]=w[2];
    ai[3]=c.x();  ai[4]=c.y();  ai[5]=c.z();
  }
  return a;
}
  

//: Create the parameter vector \p b from a vector of 3D points
vnl_vector<double> 
dvpgl_DG_bundle_adj_lsqr::create_param_vector(const vcl_vector<vgl_point_3d<double> >& world_points)
{ 
  vnl_vector<double> b(3*world_points.size(),0.0);
  for(unsigned int j=0; j<world_points.size(); ++j){
    const vgl_point_3d<double>& point = world_points[j];
    double* bj = b.data_block() + j*3;
    bj[0]=point.x();  bj[1]=point.y();  bj[2]=point.z();
  }
  return b;
}
