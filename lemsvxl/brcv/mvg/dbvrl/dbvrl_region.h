// This is algo/dbvrl/dbvrl_region.h
#ifndef dbvrl_region_h_
#define dbvrl_region_h_
//:
// \file
// \brief A class to represent region of an image to register.
// \author Matt Leotta
// \date 4/13/04
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_2d.h>
#include <vimt/vimt_transform_2d.h>
#include <dbvrl/dbvrl_world_roi_sptr.h>

//: A quadrilateral region of an image
class dbvrl_region : public vbl_ref_count
{
public:
  //: Default Constructor
  dbvrl_region();
  //: Constructor
  dbvrl_region( int ni, int nj );  

  //: Destructor
  ~dbvrl_region(){}

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Set the size of the world region
  void set_world_size(int ni, int nj); 
  //: Return the widthin world coords
  int ni() const;
  //: Return the height in world coords
  int nj() const;

  //: Returns a vector of four region corners in image coords 
  vcl_vector<vgl_point_2d<double> > corners() const;

  //: Return the transformation from world to image coords
  vimt_transform_2d xform() const;

protected:

  //: The size of the image in world coords
  int ni_, nj_;

};


//: Binary save dbvrl_region* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbvrl_region* r);

//: Binary load dbvrl_region* from stream.
void vsl_b_read(vsl_b_istream &is, dbvrl_region* &r);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const dbvrl_region* r);


#endif // dbvrl_region_h_

