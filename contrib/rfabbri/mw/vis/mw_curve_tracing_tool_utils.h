// This is mw_curve_tracing_tool_utils.h
#ifndef mw_curve_tracing_tool_utils_h
#define mw_curve_tracing_tool_utils_h
//:
//\file
//\brief Utility components of mw_curve_tracing_tool
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 03/16/2006 01:07:28 PM EST
//

#include "mw_curve_tracing_tool.h"

inline unsigned
find_nearest_pt(const vsol_point_2d_sptr &pt, const vsol_polyline_2d_sptr &crv, unsigned &mindist);

inline vsol_polyline_2d_sptr 
delete_last_vertex(const vsol_polyline_2d_sptr &c);

inline vsol_polyline_2d_sptr 
reverse_order(const vsol_polyline_2d_sptr &c);

inline vsol_polyline_2d_sptr 
get_subcurve(const vsol_polyline_2d_sptr &c, unsigned ini, unsigned end);

//----------------------------------------------------------------------

inline vsol_polyline_2d_sptr 
delete_last_vertex(const vsol_polyline_2d_sptr &c)
{
  vsol_polyline_2d *c_short = new vsol_polyline_2d();

  for (unsigned i=0; i<c->size()-1; ++i) {
    c_short->add_vertex(c->vertex(i));
  }
  return c_short;
}

inline vsol_polyline_2d_sptr 
reverse_order(const vsol_polyline_2d_sptr &c)
{
  vsol_polyline_2d *c_rev = new vsol_polyline_2d();

  for (unsigned i=c->size()-1; i!=(unsigned)-1; --i) {
    c_rev->add_vertex(c->vertex(i));
  }
  return c_rev;
}


//: returns index of nearest point to pt in crv and also the minimum distance
//(mindist param)
inline unsigned
find_nearest_pt(const vsol_point_2d_sptr &pt, const vsol_polyline_2d_sptr &crv, unsigned &mindist)
{
  unsigned min_idx,d;
  long int dx,dy;


  dx = static_cast<long>(crv->vertex(0)->x() - pt->x());
  dy = static_cast<long>(crv->vertex(0)->y() - pt->y());
  mindist = static_cast<unsigned>(dx*dx + dy*dy);
  min_idx = 0;

  for (unsigned i=1; i<crv->size(); ++i) {
    dx = static_cast<long>(crv->vertex(i)->x() - pt->x());
    dy = static_cast<long>(crv->vertex(i)->y() - pt->y());
    d = static_cast<unsigned>(dx*dx + dy*dy);
    if (mindist > d) {
      mindist = d;
      min_idx = i;
    }
  }

  return min_idx;
}

//: return sub-polyline from c(ini) to c(end); ini can be greater or smaller
// than end; no problem.
inline vsol_polyline_2d_sptr 
get_subcurve(const vsol_polyline_2d_sptr &c, unsigned ini, unsigned end)
{
  vsol_polyline_2d *c_short = new vsol_polyline_2d();

  if (ini > end)
    for (unsigned i=ini; i >= end && i != (unsigned)-1; --i) {
      c_short->add_vertex(c->vertex(i));
    }
  else
    for (unsigned i=ini; i<=end; ++i) {
      c_short->add_vertex(c->vertex(i));
    }


  return c_short;
}

#endif // mw_curve_tracing_tool_utils_h

