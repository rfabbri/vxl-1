// This is shp/dbsksp/algo/dbsksp_shock_path.h
#ifndef dbsksp_shock_path_h_
#define dbsksp_shock_path_h_

//:
// \file
// \brief A class to represent an extrinsic shock branch
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 13, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>

//==============================================================================
// dbsksp_shock_path
//==============================================================================

//:A class to represent an extrinsic shock branch
class dbsksp_shock_path: public vbl_ref_count
{
public:
  //: Constructor
  dbsksp_shock_path(const vcl_vector<dbsksp_xshock_node_descriptor >& xdesc)
  {
    this->xdesc_ = xdesc;
    this->num_points_ = xdesc.size();
    this->compute_cache_data();
  }

  //: Destructor
  virtual ~dbsksp_shock_path(){};

  // Data access----------------------------------------------------------------

  //: Get a descriptor
  const dbsksp_xshock_node_descriptor* xdesc(unsigned index) const
  {
    return &(this->xdesc_[index]);
  }

  //: Get a descriptor
  dbsksp_xshock_node_descriptor* xdesc(unsigned index)
  {
    return &(this->xdesc_[index]);
  }


  //: Get list of all xsamples
  void get_all_xdesc(vcl_vector<dbsksp_xshock_node_descriptor >& list_xdesc)
  {
    list_xdesc = this->xdesc_;
  }

  //: Return number of points
  unsigned num_points() const
  {
    return this->num_points_;
  }
  
  //: Get list of points on the mid line (mid-points of boundary chords)
  void get_mid_line(vcl_vector<vgl_point_2d<double > >& mid_line) const
  {
    mid_line = this->mid_line_;
  }

  //: Get length along mid-line of the shock points
  void get_mid_line_lengths(vcl_vector<double >& mid_line_lengths) const
  {
    mid_line_lengths = this->mid_line_lengths_;
  }

  // Utilities------------------------------------------------------------------

  //: Compute cache data for fast access to the path's properties
  void compute_cache_data();


   
  // Member variables-----------------------------------------------------------

protected:

  // User-input

  //: List of node descriptors along the branch, including the end points
  vcl_vector<dbsksp_xshock_node_descriptor > xdesc_;

  //: Number of points in the branch (should match with the size of xdesc_)
  unsigned num_points_;

  // Cache data

  //: Mid-line (connecting mid-points of boundary chords)
  vcl_vector<vgl_point_2d<double > > mid_line_;

  //: Length along the mid-line
  vcl_vector<double > mid_line_lengths_;


};



#endif // shp/dbsksp/dbsksp_shock_path.h









