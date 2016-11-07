// This is contrib/mleotta/vidreg/vidreg_reconstructor.h
#ifndef vidreg_reconstructor_h_
#define vidreg_reconstructor_h_

//:
// \file
// \brief An object to reconstruct tracks into 3D
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 8/18/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <vidreg/vidreg_salient_group_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_vector.h>
#include <vcl_deque.h>


//: An object to reconstruct tracks into 3D
class vidreg_reconstructor
{
  public:
    //: Constructor
    vidreg_reconstructor(const vidreg_salient_group_sptr& group);
    //: Destructor
    ~vidreg_reconstructor();

    void reconstruct();

  protected:
    void init_cameras();

    void init_points();

  private:
    //: The fixed internal camera parameters
    vpgl_calibration_matrix<double> K_;

    vcl_deque<vidreg_salient_group_sptr>  groups_;
    vcl_vector<vpgl_perspective_camera<double> > cameras_;
    vcl_vector<vgl_point_2d<double> > image_points_;
    vcl_vector<vgl_point_3d<double> > world_points_;
    vcl_vector<vcl_vector<bool> > mask_;

};


#endif // vidreg_reconstructor_h_
