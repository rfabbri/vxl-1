#ifndef IMGR_RSQ_HEADER_H_
#define IMGR_RSQ_HEADER_H_

//: 
// \file  imgr_file_header.h
// \brief   header information of the scanco rsq file 
// NOTE: that is Brown copyrighted material which contains Scanso 
// proprietary information. Please don't disseminate any information 
// from this file.
// \author    Kongbin Kang
// \date        2005-04-15
// 

#include <vil/vil_stream.h>
#include <vxl_config.h>

struct raw_area_type 
{
    int nr_of_det_x_;
    int nr_of_det_y_;
    int detector_length_x_um_;
    int detector_length_y_um_;
    int detector_center_x_p_;
    int detector_center_y_p_;
    int detector_distance_um_;
    int rotation_center_distance_um_;
    int detector_angle_x_mdeg_;
    int detector_angle_y_mdeg_;
    int detector_angle_z_mdeg_;
    int slice_angle_increment_mdeg_;
    int i0_pixel_l_x_;
    int i0_pixel_r_x_;
    int i0_pixel_u_y_;
    int i0_pixel_d_y_;
    int dark_pixel_l_x_;
    int dark_pixel_r_x_;
    int dark_pixel_u_y_;
    int dark_pixel_d_y_;
    int data_pixel_l_x_;
    int data_pixel_r_x_;
    int data_pixel_u_y_;
    int data_pixel_d_y_;
    int i0_index_;
    int dark_index_;
    int data_index_;
    int index_measurement_;
    int integration_time_us_;
    char name_[40];     
    int energy_;      /*V */
    int intensity_;   /* uA */ 
    int scanning_mode_;   
    int no_frames_per_stack_; /* no if I,D,P/stack */
    int no_I0_per_stack_; /* no if I0/stack */
    int no_dark_per_stack_; /* no if dark/stack */
    int no_pro_per_stack_;  /* no if pro/stack */
    int z_pos_1stframe_center_um_; /* in um */
    int z_inc_per_frame_nm_;  /* in nm */
    int z_inc_per_stack_um_;  /* in um */
    int vt_;      /* tots/offs detector */
    int ht_;
    int vo_;
    int ho_;
    int flip_code_;   /* detector flip code */
    int vbinu_;     /* vbin = u/d    */
    int vbind_;     /* e.g. MR = 2/1 */
    int hbinu_;     /* hbin = u/d    */
    int hbind_;

  public:
   void init(vil_stream*);
};      /* Raw area detector, total 58 int */

struct raw_line_type {
    int nr_of_detectors;
    int detector_length_um;
    int detector_center_p;
    int detector_distance_um;
    int detector_angle_mdeg;
    int rotation_center_distance_um;
    int slice_angle_increment_mdeg;
    int i0_pixel_l;
    int i0_pixel_r;
    int dark_pixel_l;
    int dark_pixel_r;
    int data_pixel_l;
    int data_pixel_r;
    int i0_record_nr;
    int dark_record_nr;
    int first_data_record_nr;
    int index_measurement;
    char name[40];      
    int integration_time_us;/* total 28 int */
    int energy;     /*V */
    int intensity;    /* uA */ 
    int detector_angle_x_mdeg;
    int detector_angle_y_mdeg;
    int detector_angle_z_mdeg;
    int dummy[23];      /* total 58 int */
} ;     /* Raw fan-beam linear */

struct raw_t_r_type{
    int nr_of_samples;
    int nr_of_projections;
    int nr_of_scans;
    int scandist_um;
    int sampletime_us;
    int measurement_index;
    int site;       /* Coded value  */
    int z_pos_1_um;
    int reference_line_um;
    char patient_name[40];
    int energy;     /*V */
    int intensity;    /* uA */ 
    int dummy[35];      /* total 58 int */
};        /* Raw translation-rotation */


//: RSQ information read from the header
//
// This structure is filled when reading the header information from
// a rsq file. Each member relates to a field in the header part
// of the file. Note: 4-byte-int is assumed here

class imgr_rsq_header
{
  public:

    //: the following struct arranged corresponding the way it 
    // stored in scanco rsq file

    char    check_[16];

    vxl_int_32  data_type_;

    //: either one of them     
    vxl_int_32  nr_of_bytes_;
    //: or both, but min. of 1 
    vxl_int_32  nr_of_blocks_;              
    //: 1 block = 512 bytes
    vxl_int_32  patient_index_;              

    vxl_int_32  scanner_id_;

    vxl_int_32  creation_date_[2];

    //
    vxl_int_32  dimx_p_;

    vxl_int_32  dimy_p_;

    vxl_int_32  dimz_p_;

    vxl_int_32  dimx_um_;

    vxl_int_32  dimy_mdeg_;

    vxl_int_32  dimz_um_;

    vxl_int_32  slice_thickness_um_;

    vxl_int_32  slice_increment_um_;

    vxl_int_32  slice_1_pos_um_;

    vxl_int_32  scanner_type_;

    vxl_int_32  min_data_value_;

    vxl_int_32  max_data_value_;

    union
    {
      raw_area_type area_;
      raw_line_type line_;
      raw_t_r_type tr_;
    } u_;

    vxl_int_32  fill_[36];

    vxl_int_32  reco_flags_;

    vxl_int_32 reference_line_;

    // Coded value
    vxl_int_32     site_;            

    //: Default Reco Angle offset
    vxl_int_32    def_reco_angle_mdeg_; 

    // default reonstruction size
    vxl_int_32     def_reco_size_;       

    vxl_int_32  dummy1_;

    vxl_int_32  dummy2_;

    vxl_int_32  dummy3_;

    vxl_int_32  dummy4_;

    //: ext header offset in 512-byte-blocks
    vxl_int_32  hext_offset_;          

    //: offset in 512-byte-blocks
    vxl_int_32      data_offset_;       


  public:

    imgr_rsq_header() ;
    ~imgr_rsq_header() { is_valid_ = false; }

    imgr_rsq_header(vil_stream *fs);

    bool is_valid() { return is_valid_; }

  private:
    //: wehther a valid head is read
    bool is_valid_; 

};
#endif
