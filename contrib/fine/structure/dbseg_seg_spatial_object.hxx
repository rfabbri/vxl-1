#ifndef dbseg_seg_spatial_object_txx_
#define dbseg_seg_spatial_object_txx_

#include "dbseg_seg_spatial_object.h"

template <class T>
dbseg_seg_spatial_object<T>::dbseg_seg_spatial_object(int ident, int dep, int nPlanes) {
    id=ident;
    depth=dep;
    outline = new vgl_polygon<double>();
    outline->new_sheet();
    highlighted = false;
    outlined = false;
    data_.resize(nPlanes);

}

template <class T>
dbseg_seg_spatial_object<T>::~dbseg_seg_spatial_object() {
    if (id!=0) {
        delete outline;
    }
}

template <class T>
void dbseg_seg_spatial_object<T>::add_point(double x, double y) {
    //dbseg_seg_spatial_object::outline->
    outline->push_back(x, y);
    if (contours.size() > 0) {
        contours.pop_front();
    }
    contours.push_front(outline);
}

template <class T>
const int dbseg_seg_spatial_object<T>::get_bottom() {
    return bottom;
}

/*const double dbseg_seg_spatial_object::get_area() {
    double a = vgl_area<double>(*outline)*2;
    list<vgl_polygon<double>*>::iterator i;
    for (i = contours.begin(); i != contours.end(); ++i) {
        a -= vgl_area<double>(**i);
    }
    return a;
}*/

template <class T>
const int dbseg_seg_spatial_object<T>::get_depth() {
    return depth;
}

template <class T>
const int dbseg_seg_spatial_object<T>::get_id() {
    return id;
}

template <class T>
const int dbseg_seg_spatial_object<T>::get_left() {
    return left;
}

template <class T>
list<vgl_polygon<double>*> dbseg_seg_spatial_object<T>::get_outline() {
    return contours;
}


template <class T>
void dbseg_seg_spatial_object<T>::add_inner_outline(vgl_polygon<double>* o, int ident) {
    contours.push_back(o);
    contained.insert(ident);

}

/*vsol_polygon_2d_sptr dbseg_seg_spatial_object::get_voutline() {
    std::vector<vsol_point_2d_sptr> points;
    
    for (unsigned int s = 0; s < outline->num_sheets(); ++s) {
        for (unsigned int p = 0; p < (*outline)[s].size(); ++p) {
            points.push_back(new vsol_point_2d((*outline)[s][p].x(), (*outline)[s][p].y()));
            
        }
    }
    vsol_polygon_2d_sptr poly = new vsol_polygon_2d(points);
    return poly;
}*/

template <class T>
const int dbseg_seg_spatial_object<T>::get_right() {
    return right;
}

template <class T>
const int dbseg_seg_spatial_object<T>::get_top() {
    return top;
}

template <class T>
void dbseg_seg_spatial_object<T>::set_bounding_box(int t, int b, int l, int r) {
    top=t;
    bottom=b;
    left=l;
    right=r;

    //area=outline.getarea();


}


template <class T>
void dbseg_seg_spatial_object<T>::set_depth(int dep) {
    depth=dep;
}

template <class T>
void dbseg_seg_spatial_object<T>::increase_depth() {
    depth++;
}

template <class T>
void dbseg_seg_spatial_object<T>::decrease_depth() {
    depth--;
}

template <class T>
void dbseg_seg_spatial_object<T>::clear_outline() {
    outline->~vgl_polygon();
    contours.clear();
    contained.clear();
    outline = new vgl_polygon<double>();
    outline->new_sheet();
}

/*template <class T>
void dbseg_seg_spatial_object<T>::set_mean_intensity(short r1, short g1, short b1) {
    r = r1;
    g = g1;
    b = b1;
}

template <class T>
const short dbseg_seg_spatial_object<T>::get_r() {
    return r;
}

template <class T>
const short dbseg_seg_spatial_object<T>::get_g() {
    return g;
}

template <class T>
const short dbseg_seg_spatial_object<T>::get_b() {
    return b;
}
*/

template <class T>
void dbseg_seg_spatial_object<T>::toggle_outlined() {
    outlined = !outlined;
}

template <class T>
void dbseg_seg_spatial_object<T>::toggle_highlighted() {
    highlighted = !highlighted;
}

template <class T>
bool dbseg_seg_spatial_object<T>::is_outlined() {
    return outlined;
}

template <class T>
bool dbseg_seg_spatial_object<T>::is_highlighted() {
    return highlighted;
}

template <class T>
set<int>& dbseg_seg_spatial_object<T>::get_contained() {
    return contained;
}

template <class T>
bool dbseg_seg_spatial_object<T>::surrounds(int o) {
    return contained.find(o) != contained.end();
    /*set<int>::iterator i;
    //bool temp = false;
    for (i = contained.begin(); i != contained.end(); ++i) {
        if (*i == o) {
            return true;
        }
    }
    return false;
    */

}

template <class T>
dbseg_seg_spatial_object<T>::dbseg_seg_spatial_object(int ident, int dep, int bottomL, int topL, int leftL, int rightL, vector<T> data, std::set<int> containedL, std::list<vgl_polygon<double>*> contoursL) :
highlighted(false),
outlined(false),
id(ident),
depth(dep),
bottom(bottomL),
top(topL),
left(leftL),
right(rightL),
data_(data),
contained(containedL),
contours(contoursL)

{
    outline = contours.front();
}




template <class T>
vcl_ostream& dbseg_seg_spatial_object<T>::save_text(vcl_ostream &os) {
   /*        depth bottom top left right r g b
            (n points in outer contour) (n pairs of x y)
            n (n IDs that are contained)
  {n lines} j (j pairs of x y)                        the inner contours
   */
    
    int r2 = static_cast<int>(data_[0]);
    int g2 = static_cast<int>(data_[1]);
    int b2 = static_cast<int>(data_[2]);
    os << depth << " " << bottom << " " << top << " " << left << " " << right << " " << r2 << " " << g2 << " " << b2 << vcl_endl;
    
    os << (*contours.front())[0].size();
    for (int j = 0; j < (*contours.front())[0].size(); j++) {
        os << " " << (*contours.front())[0][j].x() << " " << (*contours.front())[0][j].y();
    }
    os << vcl_endl;

    os << contained.size();
    set<int>::iterator i;
    for (i = contained.begin(); i != contained.end(); ++i) {
        os << " " << *i;
    }
    os << vcl_endl;

    list<vgl_polygon<double>*>::iterator k;
    k = contours.begin();
    k++;
    for (k = k; k != contours.end(); ++k) {
        os << (**k)[0].size();
        for (int j = 0; j < (**k)[0].size(); j++) {
            os << " " << (**k)[0][j].x() << " " << (**k)[0][j].y();
        }
        os << vcl_endl;
    }
    
   return os;
}

template <class T>
void dbseg_seg_spatial_object<T>::set_data(vector<T> data) {
    data_ = data;
}

template <class T>
vector<T>& dbseg_seg_spatial_object<T>::get_data() {
    return data_;
}


#define DBSEG_SEG_SPATIAL_OBJECT_INSTANTIATE(T) \
template class dbseg_seg_spatial_object<T>;\


#endif // dbseg_seg_spatial_object_txx_
