#ifndef IMGR_ISQ_HEADER_H_
#define IMGR_ISQ_HEADER_H_

//: 
// \file  imgr_isq_header.h
// \brief   header information of the scanco isq file. 
//NOTE: that is Brown Copyrighted material which contains Scanso 
// proprietary information. Please don't disseminate any information 
// from this file.
// \author    Kongbin Kang
// \date        2005-04-27
// 

#include <vil/vil_stream.h>
#include <vxl_config.h>

struct imgr_isq_header {
  public:
    /*---------------------------------------------*/
    char  check_[16];
    vxl_int_32   data_type_;
    vxl_int_32   nr_of_bytes_;        /* either one of them     */
    vxl_int_32   nr_of_blocks_;        /* or both, but min. of 1 */
    vxl_int_32   patient_index_;              /* 1 block = 512 bytes    */
    vxl_int_32   scanner_id_;
    vxl_int_32   creation_date_[2];
    /*---------------------------------------------*/
    vxl_int_32   dimx_p_;
    vxl_int_32   dimy_p_;
    vxl_int_32   dimz_p_;
    vxl_int_32   dimx_um_;
    vxl_int_32   dimy_um_;
    vxl_int_32   dimz_um_;
    vxl_int_32   slice_thickness_um_;
    vxl_int_32   slice_increment_um_;
    vxl_int_32   slice_1_pos_um_;
    vxl_int_32   min_data_value_;
    vxl_int_32   max_data_value_;
    vxl_int_32   mu_scaling_;     /* p(x,y,z)/mu_scaling = value [1/cm] */
    vxl_int_32   nr_of_samples_;
    vxl_int_32   nr_of_projections_;
    vxl_int_32   scandist_um_;
    vxl_int_32   scanner_type_;
    vxl_int_32   sampletime_us_;
    vxl_int_32   index_measurement_;
    vxl_int_32   site_;            /* Coded value */
    vxl_int_32   reference_line_um_;
    vxl_int_32   recon_alg_;       /* Coded value */
    char         name_[40];
    vxl_int_32   energy_;        /*V */
    vxl_int_32   intensity_;        /* uA */
    vxl_int_32   fill_[83];
    /*---------------------------------------------*/
    vxl_int_32   data_offset_;    /* in 512-byte-blocks */

  public:
    imgr_isq_header() { is_valid_ = false; }

    ~imgr_isq_header() { is_valid_ = false; }

    imgr_isq_header(vil_stream * vs);

    bool is_valid() { return is_valid_;}

  private:

    bool is_valid_ ;

    vxl_int_32 read_little_endian(vil_stream* vs);
};
#endif
