// This is contrib/sadali/vidpro1_3D_planar_curve_reconst_process.h
#ifndef vidpro1_3D_planar_curve_reconst_process_h_
#define vidpro1_3D_planar_curve_reconst_process_h_
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
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3x4.h>
#include <vcl_vector.h>
#include <vgl/algo/vgl_h_matrix_2d.h>


//: Description of this process
class vidpro1_3D_planar_curve_reconst_process : public bpro1_process 
{
public:

    //: Constructor
    vidpro1_3D_planar_curve_reconst_process();
    //: Destructor
    virtual ~vidpro1_3D_planar_curve_reconst_process();

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
    void set_BB_matrix( vnl_double_4x4 BB);
    void 
        initialize();

protected:
    
   void compute_plane_params(const vnl_double_4x4  &newBBmatrix, vnl_double_4 &Normal);
   
  bool initialized; 
   vnl_double_4x4 BBMatrix;
   vnl_double_3x4 CurProjMatL;
   vnl_double_3x4 CurProjMatR;
   vgl_h_matrix_2d<double> Hleft;
   vgl_h_matrix_2d<double> Hright;

};


#endif // vidpro1_3D_planar_curve_reconst_process_h_
