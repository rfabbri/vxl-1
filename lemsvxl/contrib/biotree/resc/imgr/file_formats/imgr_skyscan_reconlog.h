#ifndef IMGR_SKYSCAN_RECONLOG_H_
#define IMGR_SKYSCAN_RECONLOG_H_

//: 
// \file  imgr_skyscan_reconlog.h
// \brief   header information of the scansky recontruction log file 
// NOTE: that is Brown copyrighted material which contains Scanso 
// proprietary information. Please don't disseminate any information 
// from this file.
// \author    Kongbin Kang
// \date        2006-02-15
// 

#include <vcl_string.h>
#include <imgr/imgr_reconstructed_images_resource.h>
#include <xscan/xscan_scan.h>
//
// This structure is filled when reading the header information from
// a log file. Each member relates to a field in the header part
// of the file. 

class imgr_skyscan_reconlog: public imgr_reconstructed_images_resource
{
  public:
    int start_slice_;
    int end_slice_;
    int slice_step_;
    double voxel_size_;

    virtual vgl_point_3d<double> fbpc_to_bsc(vgl_point_3d<double> const& pt);

    virtual vgl_point_3d<double> bsc_to_fbpc(vgl_point_3d<double> const& pt);

  public:

    imgr_skyscan_reconlog() ;
    ~imgr_skyscan_reconlog() { is_valid_ = false; }

    imgr_skyscan_reconlog(char const*  fname);

     imgr_skyscan_reconlog(char const*  fname,xscan_scan & scan);

     double get_v0() {return v0_;}

    double get_camera_pixel_size_u() {return camera_pixel_size_u_;}

     double get_camera_pixel_size_v() {return camera_pixel_size_v_;}

    int get_size_x(){return size_x_ ;}

    int get_size_y(){return size_y_ ;}

    double get_source_sensor_dist() {return src_snsr_dist_;}

    double get_source_rot_center_dist() {return src_rot_dist_;}
     

    bool is_valid() { return is_valid_; }

  private:
    //: wehther a valid head is read
    bool is_valid_; 

    double v0_; // v component of principle point coordinates

    double camera_pixel_size_u_;

    double camera_pixel_size_v_;

    int size_x_;

    int size_y_;

    int sv_;

    double src_snsr_dist_;

    double src_rot_dist_;


};
#endif
