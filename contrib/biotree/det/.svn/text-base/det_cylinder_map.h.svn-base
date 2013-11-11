#ifndef det_cylinder_map_h_
#define det_cylinder_map_h_
//: 
// \file      det_cylinder_map.h
// \brief     Cylinder 3d map class. Each entry in the map holds the 
//            strength, direction and radius of a cylinder. 
// \author    Kongbin Kang 
// \date      2005-09-09
// 
#include "det_map_entry.h"
#include <vbl/vbl_array_3d.h>
#include <xmvg/xmvg_filter_response.h>
#include <xmvg/xmvg_composite_filter_descriptor.h>

typedef struct det_map_entry cme_t;

class det_cylinder_map
{
  private:
    vbl_array_3d<cme_t> map_;

    //: size on 3 dimensions
    unsigned nx_, ny_, nz_;

  public:

    unsigned nx() const { return nx_; }

    unsigned ny() const { return ny_; }

    unsigned nz() const { return nz_; }

    //: default constructor
    det_cylinder_map() {}

    //: construct a default map with (nx, ny, nz) dimensions
    det_cylinder_map(unsigned nx, unsigned ny, unsigned nz);
    
    //: construct a map from a vbl_array_3d
    det_cylinder_map(vbl_array_3d<cme_t> const & map);

    //: resize the map
    void resize(int nx, int ny, int nz);

    //: construct a map from the edge detectors along x, y and z directions
    //: this map is used for the 3D maximum suppression along a line
    det_cylinder_map(unsigned nx, unsigned ny, unsigned nz, 
      vcl_vector<xmvg_filter_response<double> > const& responses_x, 
      vcl_vector<xmvg_filter_response<double> > const& responses_y, 
      vcl_vector<xmvg_filter_response<double> > const& responses_z);

    //: construct a map from the gradients along x, y and z directions
    //: this map is used for the 3D maximum suppression along a line
    det_cylinder_map(unsigned nx, unsigned ny, unsigned nz, 
                                       double ***gradient_x, 
                                       double ***gradient_y, 
                                       double ***gradient_z);

    //: return the direction on the (i, j, k) is a 0-based index
    det_map_entry const & operator()(unsigned i, unsigned j, unsigned k) const;

    //: data access
    det_map_entry& operator()(unsigned i, unsigned j, unsigned k);

    det_map_entry * const* operator[](unsigned i) 
    {
      return map_[i];
    }

    det_map_entry const * const * operator[](unsigned i) const 
    {
      return map_[i];
    }

    
    
};

//: read from stream file
vcl_istream& operator >> ( vcl_istream& stream, det_cylinder_map& map);

vcl_ostream& operator << ( vcl_ostream& stream, const det_cylinder_map& map);
#endif
