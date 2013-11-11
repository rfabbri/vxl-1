#include "imgr_isq_header.h"
#include <vil/vil_stream_read.h>
#include <vcl_cstring.h>
#include <vcl_exception.h>

const unsigned isq_magic_len = 16;

const unsigned size_of_int = 4;

char const* isq_magic = "CTDATA-HEADER_V1";

//: untility to used to read little indea data out
vxl_int_32 imgr_isq_header::read_little_endian(vil_stream* vs)
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

imgr_isq_header::imgr_isq_header(vil_stream * fs)
{
  is_valid_ = false;

  if(fs->ok())
  {
    fs->seek(0);

    // read magic words
    if (fs->read(check_, isq_magic_len) == static_cast<int>(isq_magic_len))
    {
      if(vcl_strncmp( check_, isq_magic, isq_magic_len) == 0)
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

        dimy_um_ = read_little_endian(fs);

        dimz_um_ = read_little_endian(fs);

        slice_thickness_um_ = read_little_endian(fs);

        slice_increment_um_ = read_little_endian(fs);

        slice_1_pos_um_ = read_little_endian(fs);

        min_data_value_ = read_little_endian(fs);

        max_data_value_ = read_little_endian(fs);

        mu_scaling_ = read_little_endian(fs);

        nr_of_samples_ = read_little_endian(fs);

        nr_of_projections_ = read_little_endian(fs);

        scandist_um_ = read_little_endian(fs);

        scanner_type_ = read_little_endian(fs);
        
        sampletime_us_ = read_little_endian(fs);
    
        index_measurement_ = read_little_endian(fs);
    
        site_ = read_little_endian(fs);            /* Coded value */
    
        reference_line_um_ = read_little_endian(fs);
    
        recon_alg_ = read_little_endian(fs);       /* Coded value */

        fs->read(name_, 40);
    
        energy_ = read_little_endian(fs);        /*V */
    
        intensity_ = read_little_endian(fs);        /* uA */
    
        for(unsigned i = 0; i< 83; i++)
          fill_[i] = read_little_endian(fs);

        data_offset_ = read_little_endian(fs);

      }

    }

    if(!is_valid_){
      vcl_throw vcl_exception();
    }
  }

}


