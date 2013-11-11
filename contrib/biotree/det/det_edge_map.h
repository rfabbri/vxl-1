#ifndef det_edge_map_h_
#define det_edge_map_h_
//: 
// \file      det_edge_map.h
// \brief     Edge 3d map class. Each entry in the map holds the 
//            strength, direction and sub-voxel location of the edge. 
// \author    H. Can Aras
// \date      2006-04-05
// 
#include "det_map_entry.h"
#include <vbl/vbl_array_3d.h>
#include <xmvg/xmvg_filter_response.h>

typedef struct det_map_entry eme_t;

class det_edge_map
{
  private:
    vbl_array_3d<eme_t> map_;

    //: size on 3 dimensions
    unsigned nx_, ny_, nz_;

  public:

    unsigned nx() const { return nx_; }

    unsigned ny() const { return ny_; }

    unsigned nz() const { return nz_; }

    //: default constructor
    det_edge_map() {}

    //: construct a default map with (nx, ny, nz) dimensions
    det_edge_map(unsigned nx, unsigned ny, unsigned nz);
    
    //: construct a map from a vbl_array_3d
    det_edge_map(vbl_array_3d<eme_t> const & map);

    //: resize the map
    void resize(int nx, int ny, int nz);

    //: construct a map from the edge detectors along x, y and z directions
    //: this map is used for the 3D maximum suppression along a line
    det_edge_map(unsigned nx, unsigned ny, unsigned nz, 
      vcl_vector<xmvg_filter_response<double> > const& responses_x, 
      vcl_vector<xmvg_filter_response<double> > const& responses_y, 
      vcl_vector<xmvg_filter_response<double> > const& responses_z,
      double sharpening_coefficient=1.0);

    //: construct a map from the gradients along x, y and z directions
    //: this map is used for the 3D maximum suppression along a line
    det_edge_map(unsigned nx, unsigned ny, unsigned nz, 
                                       double ***gradient_x, 
                                       double ***gradient_y, 
                                       double ***gradient_z);

    //: construct a map from the gradients along x, y and z directions
    //: this map is used for the 3D maximum suppression along a line
    det_edge_map(unsigned nx, unsigned ny, unsigned nz, 
                                       const double *gradient_x, 
                                       const double *gradient_y, 
                                       const double *gradient_z);

    //: construct a map from the gradients along x, y and z directions
    //: this map is used for the 3D maximum suppression along a line
    det_edge_map(unsigned nx, unsigned ny, unsigned nz, 
                                       const float *gradient_x, 
                                       const float *gradient_y, 
                                       const float *gradient_z);


    det_edge_map(const vbl_array_3d<double>& gradient_x, 
                               const vbl_array_3d<double>& gradient_y, 
                               const vbl_array_3d<double>& gradient_z);


    det_edge_map nonmaxium_suppression_for_edge_detection();

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
vcl_istream& operator >> ( vcl_istream& stream, det_edge_map& map);

vcl_ostream& operator << ( vcl_ostream& stream, const det_edge_map& map);
#endif
