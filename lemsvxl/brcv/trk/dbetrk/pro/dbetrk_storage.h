// This is breye1/vidpro1/storage/dbetrk_storage.h
#ifndef dbetrk_storage_h_
#define dbetrk_storage_h_

//:
// \file
// \brief The vidpro1 storage class for edgetrk .
// \author Vishal Jain, (vj@lems.brown.edu)
// \date Sat Sept 23 2004
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbetrk/pro/dbetrk_storage_sptr.h>
#include <dbetrk/dbetrk_edge_sptr.h>
#include <vcl_vector.h>

//: Storage class for ctrk 
class dbetrk_storage : public bpro1_storage {
public:

  //: Constructor
  dbetrk_storage(){}
  //: Constructor
  dbetrk_storage( const vcl_vector<dbetrk_edge_sptr>& dbetrk_edges);
  //: Destructor
  virtual ~dbetrk_storage(){}

  //: Returns the type string "edgetrk"
  virtual vcl_string type() const { return "edgetrk"; }

  //: Set the ctrk storage vector
  void set_dbetrk_edges(const vcl_vector<dbetrk_edge_sptr>& dbetrk_edges);

  //: Retrieve the edgetrk storage vector
  void get_dbetrk_edges(vcl_vector<dbetrk_edge_sptr>& dbetrk_edges) const;

  virtual vcl_string is_a() const {return "dbetrk_storage";}

  virtual bpro1_storage* clone() const;

  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
 private:
  //: The vector of  edgetrk smart pointers
  vcl_vector< dbetrk_edge_sptr > dbetrk_edges_;  
};


//: Create a smart-pointer to a dbetrk_storage.
struct dbetrk_storage_new : public dbetrk_storage_sptr
{
  typedef dbetrk_storage_sptr base;

  //: Constructor - creates a default vidpro1_ctrk_storage_sptr.
  dbetrk_storage_new() : base(new dbetrk_storage()) { }

  //: Constructor - creates a vidpro1_ctrk_storage_sptr with ctrks.
  dbetrk_storage_new(const vcl_vector<dbetrk_edge_sptr>& dbetrk_edges)
   : base(new dbetrk_storage(dbetrk_edges )) { }
};


#endif //vidpro1_ctrk_storage_h_
