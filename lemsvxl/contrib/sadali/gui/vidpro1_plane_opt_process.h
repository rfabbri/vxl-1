// This is contrib/sadali/vidpro1_plane_opt_process.h
#ifndef vidpro1_plane_opt_process_h_
#define vidpro1_plane_opt_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Short description of the process
//
// \author
//   Your Name (sadali@lems.brown.edu)
//
// \verbatim
//  Modifications:
//   Name         Date        Changes made
// \endverbatim
//--------------------------------------------------------------------------------


#include <bpro1/bpro1_process.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vil/vil_image_view.h>
#include <vgl/algo/vgl_h_matrix_2d.h>


//: Description of this process
class vidpro1_plane_opt_process : public bpro1_process 
{
public:

  //: Constructor
  vidpro1_plane_opt_process();
  //: Destructor
  virtual ~vidpro1_plane_opt_process();

  virtual bpro1_process* clone() const;

  //: The name of this process
  vcl_string name();

  //: Returns the number of input frames to this process
  int input_frames();
  //: Return the number of output frames for this process
  int output_frames();

  //: Returns a vector of strings describing the input types to this process
  vcl_vector< vcl_string > get_input_type();
  //: Returns a vector of strings describing the output types of this process
  vcl_vector< vcl_string > get_output_type();

  //: Execute the process
  bool execute();
  bool finish();

protected:
 
     void 
     InitializeParams(const vnl_double_4x4 &UpdatedBBMatrix, double &d, double &aleph);
     double
     compute_err(const vil_image_view<float>  &left_image_view_filt, const vil_image_view<float>  &right_image_view_filt,
                                     
                                      vcl_vector<vnl_double_4> &WorldPoints, const vnl_double_4x4 &BBoxTransf,int &num_of_samples);
    void
     computeWorldPoints(vcl_vector<vnl_double_4> &WP, double samplex, double sampley, const vnl_double_4x4 &BBMatrix );
    void
    updateBBMat(const vnl_double_4x4 &BBMatrix,vnl_double_4x4 &UpdatedBBMatrix, const double t,const double alpha);

    // Transformation matrix functions
    vnl_double_4x4
        rot_mat(const vnl_double_4 &p, const vnl_double_4 &v,const double a);
    vnl_double_4x4
        trans_mat(const vnl_double_4 &trans);
    vnl_double_4x4
        rotX_mat(const double a);
    vnl_double_4x4
        rotY_mat(const double a);
    vnl_double_4x4
        rotZ_mat(const double a);
  void
       compute_plane_params(const vnl_double_4x4 &BBMat);
    double 
       compute_lengthBB(const vnl_double_4x4 &BBMat);
    void
        compute_plane_Normal(const vcl_vector<vnl_double_4> & corners, vnl_double_4 &Normal);

    //Process data objects
    vil_image_view<float> corrleft;
    vil_image_view<float>  corrright;
    vil_image_view<float> mapleft;
    vil_image_view<float>  mapright;
    vil_image_view<vxl_byte>  left_mask_view ;
    vil_image_view<vxl_byte>  right_mask_view ;

    vgl_h_matrix_2d<double> Hleft;
    vgl_h_matrix_2d<double> Hright;

    vgl_h_matrix_2d<double> Left2Right;
    int number_of_residuals;



    vnl_double_3x4 LeftProjMat;
    vnl_double_3x4 RightProjMat;
    vnl_double_4 Planeparam;
    vnl_double_4 trans_vec;
    vnl_double_3x3 K_;
    vnl_double_3 t_;
    bool rotation;
    bool translation;
    bool trans_from_KT;
    bool WP_point_samp;

};


#endif // vidpro1_plane_opt_process_h_
