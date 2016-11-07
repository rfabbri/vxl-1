
// This is contrib/sadali/vidpro1_BB_visualize_process.h
#ifndef vidpro1_BB_visualize_process_h_
#define vidpro1_BB_visualize_process_h_
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
#include <vil/vil_image_view.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>


//: Description of this process
class vidpro1_BB_visualize_process : public bpro1_process 
{
public:

  //: Constructor
  vidpro1_BB_visualize_process();
  //: Destructor
  virtual ~vidpro1_BB_visualize_process();

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
 
    // void 
    // InitializeParams(const vnl_double_4x4 &UpdatedBBMatrix, double &d, double &aleph);
     double
     compute_err(const vil_image_view<float>  &left_image_view_filt, const vil_image_view<float>  &right_image_view_filt,
                                      const vnl_double_3x4 &LeftProjMat , const vnl_double_3x4 &RightProjMat ,
                                      vcl_vector<vnl_double_4> &WorldPoints);
    void
     computeWorldPoints(vcl_vector<vnl_double_4> &WP, double samplex, double sampley, const vnl_double_4x4 &BBMatrix , int plane_index);
#if 0
    void
    updateBBMat(const vnl_double_4x4 &BBMatrix,vnl_double_4x4 &UpdatedBBMatrix, const double t,const double alpha);
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
#endif
    double
        Compute_proj(vcl_vector<vsol_spatial_object_2d_sptr>  &left_image_view_filt,  vcl_vector<vsol_spatial_object_2d_sptr>  &right_image_view_filt,
                    const vnl_double_3x4 &LeftProjMat , const vnl_double_3x4 &RightProjMat , const vcl_vector<vnl_double_4> &WorldPoints);


    
    int row;
    int col;

};






#endif  //vidpro1_BB_visualize_process__h_
