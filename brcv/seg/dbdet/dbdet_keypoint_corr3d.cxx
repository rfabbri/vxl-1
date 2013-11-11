// This is brcv/seg/dbdet/dbdet_keypoint_corr3d.cxx
//:
// \file

#include "dbdet_keypoint_corr3d.h"
#include "dbdet_keypoint.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_map_io.h>


//: Return the keypoint that corresponds in the give view
// returns a NULL keypoint if there is no correspondence
dbdet_keypoint_sptr
dbdet_keypoint_corr3d::in_view(int view_index) const
{
  vcl_map<int, dbdet_keypoint_sptr>::const_iterator fitr = view_map_.find(view_index);
  if(fitr == view_map_.end())
    return dbdet_keypoint_sptr(NULL);
  
  return fitr->second;
}


//: Add a correspondence to the given keypoint in the given view
//  if a corresponding point already exist in that view it is overwritten
void
dbdet_keypoint_corr3d::add_correspondence(const dbdet_keypoint_sptr& kp, int view)
{
  view_map_.insert(vcl_pair<int, dbdet_keypoint_sptr>(view, kp));
}


//: Remove the correspondence in the given view if it exists
// return true if successful
bool 
dbdet_keypoint_corr3d::remove_correspondence(int view)
{
  return (0 < view_map_.erase(view));
}


//: Merge correspondence from another keypoint with this one
// only succeed if all existing correspondence agree
// returns true if successful 
bool 
dbdet_keypoint_corr3d::merge(const dbdet_keypoint_corr3d& other)
{
  vcl_map<int, dbdet_keypoint_sptr> merged_map(view_map_);
  for( vcl_map<int, dbdet_keypoint_sptr>::const_iterator itr = other.view_map_.begin();
       itr != other.view_map_.end();  ++itr )
  {
    vcl_pair<vcl_map<int, dbdet_keypoint_sptr>::iterator,bool> v = merged_map.insert(*itr);
    if(!v.second){
      if(v.first->second != itr->second)
        return false;
    }
  }
  view_map_ = merged_map;
  this->set((x()+other.x())/2.0, (y()+other.y())/2.0, (z()+other.z())/2.0);
  return true;
}


//: Binary save self to stream.
void
dbdet_keypoint_corr3d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->x());
  vsl_b_write(os, this->y());
  vsl_b_write(os, this->z());
  vsl_b_write(os, view_map_);
}


//: Binary load self from stream.
void
dbdet_keypoint_corr3d::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    double p[3];
    vsl_b_read(is, p[0]);
    vsl_b_read(is, p[1]);
    vsl_b_read(is, p[2]);
    this->set(p);
    vsl_b_read(is, view_map_);
    break;

  default:
    vcl_cerr << "I/O ERROR: dbdet_keypoint_corr3d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return IO version number;
short
dbdet_keypoint_corr3d::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
dbdet_keypoint_corr3d*
dbdet_keypoint_corr3d::clone() const
{
  return new dbdet_keypoint_corr3d(*this);
}


//: Print an ascii summary to the stream
void
dbdet_keypoint_corr3d::print_summary(vcl_ostream &os) const
{
  os << "dbdet_keypoint_corr3d("<<x()<<", "<<y()<<", "<<z()<<")";
}



//==============================================
//: Stream summary output for base class pointer
void vsl_print_summary(vcl_ostream& os,const dbdet_keypoint_corr3d* k)
{
  if (k)
    k->print_summary(os);
  else
    os << "NULL dbdet_keypoint_corr3d\n";
}

//: Read a dbdet_keypoint_corr3d point from the stream
void vsl_b_read(vsl_b_istream &is, dbdet_keypoint_corr3d* &k)
{
  delete k;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    k = new dbdet_keypoint_corr3d();
    k->b_read(is);
  }
  else
    k = 0;
}

//: Write a dbdet_keypoint_corr3d point to the stream
void vsl_b_write(vsl_b_ostream &os, const dbdet_keypoint_corr3d* k)
{
  if (k==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    k->b_write(os);
  }
}
