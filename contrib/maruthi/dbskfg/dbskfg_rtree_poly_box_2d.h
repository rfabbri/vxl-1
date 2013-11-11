// This is core/vgl/algo/vgl_rtree_c.h
#ifndef dbskfg_rtree_poly_box_2d_h_
#define dbskfg_rtree_poly_box_2d_h_


#include <vgl/algo/vgl_rtree_c.h>

template <class T>
class dbskfg_rtree_poly_box_2d
{
    // only static methods
    dbskfg_rtree_poly_box_2d();
    ~dbskfg_rtree_poly_box_2d();
 
public:

    typedef vcl_vector< vgl_point_2d<T> > v_type ;
    typedef vgl_bbox_2d<T> b_type;
    typedef T t_type;

    // Operations------
    static void  init  (vgl_bbox_2d<T>& b, 
                        vcl_vector<vgl_point_2d<T> > const& v)
    { b = vgl_bbox_2d<T>();  
        
        for (unsigned int s = 0; s < v.size(); ++s)
        { 
                b.add(v[s]);
        }
    }

    static void  update(vgl_bbox_2d<T>& b, 
                        vcl_vector<vgl_point_2d<T> > const& v)
    {
        for (unsigned int s = 0; s < v.size(); ++s)
        { 
            b.add(v[s]);
        }
    }
 
    static void  update(vgl_bbox_2d<T>& b0, vgl_bbox_2d<T> const &b1)
    { b0.add(b1.min_point());  b0.add(b1.max_point()); }
 
    static bool meet(vgl_bbox_2d<T> const& b0, 
                     vcl_vector<vgl_point_2d<T> > const& v) 
    {  
        for (unsigned int s = 0; s < v.size(); ++s)
        { 
            return b0.contains(v[s]);
        }

    }
 
    static bool  meet(vgl_bbox_2d<T> const& b0, vgl_bbox_2d<T> const& b1) 
    {
        bool resultf =(b0.contains(b1.min_point()) 
                       || b0.contains(b1.max_point()));
        bool resultr =(b1.contains(b0.min_point()) 
                       || b1.contains(b0.max_point()));
        return resultf||resultr;
    }
 
    static float volume(vgl_bbox_2d<T> const& b)
    { return static_cast<float>(b.area()); }
 
    // point meets for a polygon, used by generic rtree probe
    static bool meets(vcl_vector< vgl_point_2d<T> > const& v, 
                      vgl_polygon<T> poly)
    {    for (unsigned int s = 0; s < v.size(); ++s)
        { 
            return poly.contains(v[s]);
        }
    }

    // box meets for a polygon, used by generic rtree probe
    static bool meets(vgl_bbox_2d<T> const& b, vgl_polygon<T> poly)
    { return vgl_intersection<T>(b, poly); }

};
 

#endif // dbskfg_rtree_poly_box_2d_h_
