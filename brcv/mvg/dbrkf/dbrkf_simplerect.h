#ifndef _dbrkf_simplerect_h_
#define _dbrkf_simplerect_h_

#include <vcl_iostream.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_2x2.h>

//:
// \file
// \brief This class is used to rectify pair of stereo images using atwo step procedure.
// The "first step" maps the epipole for both the images to infinity separatley and 
// then in the "second step" a 1D homography is computed to map the parallel( horizontal) epipolar lines to the
// epipolar lines in the second image.
//
// \author Vishal Jain, (vj@lems.brown.edu)
// \date 08/31/04
//
// \verbatim
//  Modifications
//     2007-JUN-21  Eduardo Almeida   Fixed bugs in map_epipoles_infinity()
// \endverbatim


class dbrkf_simplerect
{
public:
    //: default constructor
    dbrkf_simplerect();
    //: initialize the set of corresponding points 
    dbrkf_simplerect(vcl_vector<vsol_point_2d_sptr > pointsl,
           vcl_vector<vsol_point_2d_sptr > pointsr,
           vil_image_view<vxl_byte> imagel,
           vil_image_view<vxl_byte> imager);
    ~dbrkf_simplerect();

    //: step1 is to send the epipole of each image to infinity along the horizontal direction 
    bool rect_step1();
    //: step2 is to align the horizontal epipolar lines from the two images.
    bool rect_step2();

    //: reverses the rectification steps so as to have a mapping of the original points to the rectified images.
    vgl_point_2d<double> undo_steps(vgl_point_2d<double> p,vcl_string tag);

    vil_image_view<vxl_byte> *imgl_step1;
    vil_image_view<vxl_byte> *imgr_step1;

    vil_image_view<vxl_byte> *imgl_step2;
    vil_image_view<vxl_byte> *imgr_step2;


private:

    bool compute_fundamental_matrix(vcl_vector<vsol_point_2d_sptr > pointsl,
                                    vcl_vector<vsol_point_2d_sptr > pointsr);

    bool map_epipoles_infinity(vnl_double_3x3 &H0,vgl_homg_point_2d<double> ep,double ci, double cj);

    void resample(vnl_double_3x3 &Hfl,vnl_double_3x3 &Hfr,vil_image_view<vxl_byte> *imgfl,vil_image_view<vxl_byte> *imgfr,
                  vnl_double_3x3 Hl,vnl_double_3x3 Hr,vil_image_view<vxl_byte> imgl,vil_image_view<vxl_byte> imgr);

    void resample(vnl_double_2x2 &Hfl,vnl_double_2x2 &Hfr,vil_image_view<vxl_byte> *imgfl,vil_image_view<vxl_byte> *imgfr,
                  vnl_double_2x2 Hl,vnl_double_2x2 Hr,vil_image_view<vxl_byte> *imgl,vil_image_view<vxl_byte> *imgr);


    vil_image_view<vxl_byte> imgl;
    vil_image_view<vxl_byte> imgr;


    vcl_vector<vsol_point_2d_sptr > pointsl_;
    vcl_vector<vsol_point_2d_sptr > pointsr_;

    vgl_homg_point_2d<double> el;
    vgl_homg_point_2d<double> er;

    vnl_double_3x3 Fm;

    vnl_double_3x3 Hl_step1;
    vnl_double_3x3 Hr_step1;

    vnl_double_2x2 Hl_step2;
    vnl_double_2x2 Hr_step2;

};
#endif
