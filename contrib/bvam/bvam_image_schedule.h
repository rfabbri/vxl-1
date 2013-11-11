#ifndef _bvam_image_schedule_h_
#define _bvam_image_schedule_h_

//:
// \file
// \brief
// \author Thomas Pollard
// \date 1/12/08

#include <vcl_string.h>
#include <vcl_vector.h>
#include "bvam_image_metadata.h"

class bvam_image_schedule {

public:

  bvam_image_schedule();
  ~bvam_image_schedule(){}

  // Loading commands.  Enter "NONE" in a field if it is not to be loaded.
  void load(
    vcl_string image_dir,
    vcl_string camera_file,
    vcl_string light_file,
    bool read_date );

  // Scheduling commands.
  void schedule_all();
  void schedule_subset(
    unsigned start_index,
    unsigned end_index,
    unsigned inc );
  void order_by_date();
  void shuffle();

  // Array access commands.
  const bvam_image_metadata& operator[]( unsigned i ){ return list_[schedule_[i]]; }
  const bvam_image_metadata& operator[] ( unsigned i ) const { return list_[schedule_[i]]; }
  void push_back( const bvam_image_metadata& new_img );
  unsigned size() const{ return schedule_.size(); }

protected:

  vcl_vector<bvam_image_metadata> list_;
  vcl_vector<unsigned> schedule_;

  // Get the date from a quickbird filename.
  double get_date( vcl_string file_name );

};





#endif // _bvam_image_schedule_h_
