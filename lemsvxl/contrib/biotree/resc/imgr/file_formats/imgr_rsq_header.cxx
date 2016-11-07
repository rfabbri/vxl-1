#include "imgr_rsq_header.h"
#include <vil/vil_stream_read.h>
#include <vcl_cstring.h>

const unsigned rsq_magic_len = 16;

const unsigned size_of_int = 4;

char const* rsq_magic = "CTDATA-HEADER_V1";

//: untility to used to read little indea data out
vxl_int_32 read_little_endian(vil_stream* vs)
{
  vxl_byte bytes[4];
  if(vs->read(bytes,sizeof bytes) != sizeof bytes) 
    return 0;

#if VXL_LITTLE_ENDIAN
  return ((int)bytes[0]) | ((int)bytes[1])<<8 | ((int)bytes[2])<<16 | ((int)bytes[3])<<24;
#else
  return ((int)bytes[3]) | ((int)bytes[2])<<8 | ((int)bytes[1])<<16 | ((int)bytes[0])<<24;
#endif
}

imgr_rsq_header::imgr_rsq_header(vil_stream * fs)
{
  is_valid_ = false;

  if(fs != 0){
    if(fs->ok())
    {
      fs->seek(0);

      // read magic words
      if (fs->read(check_, rsq_magic_len) == rsq_magic_len)
      {
        if(vcl_strncmp( check_, rsq_magic, rsq_magic_len) == 0)
        {
          is_valid_ = true;

          data_type_ = read_little_endian(fs);

          nr_of_bytes_ = read_little_endian(fs);

          nr_of_blocks_ = read_little_endian(fs);

          patient_index_ = read_little_endian(fs);

          scanner_id_ = read_little_endian(fs);

          creation_date_[0] = read_little_endian(fs);

          creation_date_[1] = read_little_endian(fs);

          dimx_p_ = read_little_endian(fs);

          dimy_p_ = read_little_endian(fs); 

          dimz_p_ = read_little_endian(fs);

          dimx_um_ = read_little_endian(fs);

          dimy_mdeg_ = read_little_endian(fs);

          dimz_um_ = read_little_endian(fs);

          slice_thickness_um_ = read_little_endian(fs);

          slice_increment_um_ = read_little_endian(fs);

          slice_1_pos_um_ = read_little_endian(fs);

          scanner_type_ = read_little_endian(fs);

          min_data_value_ = read_little_endian(fs);

          max_data_value_ = read_little_endian(fs);

          u_.area_.init(fs);

          for(unsigned i=0; i<36; i++)
            fill_[i] = read_little_endian(fs);

          reco_flags_ = read_little_endian(fs);

          reference_line_ = read_little_endian(fs);

          site_ = read_little_endian(fs);

          def_reco_angle_mdeg_ = read_little_endian(fs);

          def_reco_size_ = read_little_endian(fs);

          dummy1_ = read_little_endian(fs);

          dummy2_ = read_little_endian(fs);

          dummy3_ = read_little_endian(fs);

          dummy4_ = read_little_endian(fs);

          hext_offset_ = read_little_endian(fs);  

          data_offset_ = read_little_endian(fs);

        }

      }
    }
  }

}

void raw_area_type::init(vil_stream *fs)
{
  nr_of_det_x_ = read_little_endian(fs) ;

  nr_of_det_y_ = read_little_endian(fs);

  detector_length_x_um_ = read_little_endian(fs);

  detector_length_y_um_ = read_little_endian(fs);

  detector_center_x_p_ = read_little_endian(fs);

  detector_center_y_p_ = read_little_endian(fs);

  detector_distance_um_ = read_little_endian(fs);

  rotation_center_distance_um_ = read_little_endian(fs);

  detector_angle_x_mdeg_ = read_little_endian(fs);

  detector_angle_y_mdeg_ = read_little_endian(fs);

  detector_angle_z_mdeg_ = read_little_endian(fs);

  slice_angle_increment_mdeg_ = read_little_endian(fs);

  i0_pixel_l_x_ = read_little_endian(fs);

  i0_pixel_r_x_ = read_little_endian(fs);

  i0_pixel_u_y_ = read_little_endian(fs);

  i0_pixel_d_y_ = read_little_endian(fs);

  dark_pixel_l_x_ = read_little_endian(fs);

  dark_pixel_r_x_ = read_little_endian(fs);

  dark_pixel_u_y_ = read_little_endian(fs);

  dark_pixel_d_y_ = read_little_endian(fs);

  data_pixel_l_x_ = read_little_endian(fs);

  data_pixel_r_x_ = read_little_endian(fs);

  data_pixel_u_y_ = read_little_endian(fs);

  data_pixel_d_y_ = read_little_endian(fs);

  i0_index_ = read_little_endian(fs);

  dark_index_ = read_little_endian(fs);

  data_index_ = read_little_endian(fs);

  index_measurement_ = read_little_endian(fs);

  integration_time_us_ = read_little_endian(fs);

  fs->read(name_, 40);      

  energy_ = read_little_endian(fs);     /*V */

  intensity_ = read_little_endian(fs);    /* uA */ 

  scanning_mode_ = read_little_endian(fs);    

  no_frames_per_stack_ = read_little_endian(fs);  /* no if I,D,P/stack */

  no_I0_per_stack_ = read_little_endian(fs);  /* no if I0/stack */

  no_dark_per_stack_ = read_little_endian(fs);  /* no if dark/stack */

  no_pro_per_stack_ = read_little_endian(fs); /* no if pro/stack */

  z_pos_1stframe_center_um_ = read_little_endian(fs); /* in um */

  z_inc_per_frame_nm_ = read_little_endian(fs);   /* in nm */

  z_inc_per_stack_um_ = read_little_endian(fs);   /* in um */

  vt_ = read_little_endian(fs);     /* tots/offs detector */

  ht_ = read_little_endian(fs);

  vo_ = read_little_endian(fs);

  ho_ = read_little_endian(fs);

  flip_code_ = read_little_endian(fs);    /* detector flip code */

  vbinu_ = read_little_endian(fs);      /* vbin = u/d    */

  vbind_ = read_little_endian(fs);      /* e.g. MR = 2/1 */

  hbinu_ = read_little_endian(fs);      /* hbin = u/d    */

  hbind_ = read_little_endian(fs);
}


