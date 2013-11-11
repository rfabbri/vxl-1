//This is contrib/fine/gui/dbseg_seg_spatial_object.h
#ifndef dbseg_seg_spatial_object_h_
#define dbseg_seg_spatial_object_h_

//:
// \file
// \the class representing a segmented region
// \author Eli Fine
// \date 7/29/08
//
//
// \verbatim
//  Modifications
// \endverbatim

//#include <vsol/vsol_polygon_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>

#include <list>
#include <set>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vsol/vsol_polygon_2d_sptr.h>


#include <fstream>
#include <vcl_ostream.h>
//#include <vcl_vector.h>
using namespace std;

template <class T>
class dbseg_seg_spatial_object {

public:
    //constructor
    dbseg_seg_spatial_object(int ident, int dep, int numPlanes);
    //constructor used when loading
    dbseg_seg_spatial_object(int ident, int dep, int bottomL, int topL, int leftL, int rightL, vector<T>, set<int> containedL, list<vgl_polygon<double>*> contoursL);
    //destructor
    ~dbseg_seg_spatial_object();
    //returns list of all contours
    list<vgl_polygon<double>*> get_outline();
    //returns top
    const int get_top();
    //returns bottom
    const int get_bottom();
    //returns left
    const int get_left();
    //returns right
    const int get_right();
    //returns depth
    const int get_depth();
    //sets depth
    void set_depth(int dep);
    //adds a point to the outline
    void add_point(double x, double y);
    //returns id
    const int get_id();
    //sets bounding box coordinates (top, bottom, left, right)
    void set_bounding_box(int t, int b, int l, int r);
    //clears all stored outlines
    void clear_outline();
    //adds an inner contour
    void add_inner_outline(vgl_polygon<double>* o, int ident);
    // returns area
    //moved to seg_object so that inner contours can be handled      const double get_area();
    //returns mean intensities
    
    vector<T>& get_data();

    /*const short get_r();
    const short get_g();
    const short get_b();*/

    void set_data(vector<T> data);
    
    //void set_mean_intensity(short r, short g, short b);

    //related to graphics
    void toggle_outlined();
    void toggle_highlighted();
    bool is_outlined();
    bool is_highlighted();

    void increase_depth();
    void decrease_depth();

    //returns list of objects that are contained in this one
    set<int>& get_contained();

    //returns true if this object is already contained
    bool surrounds(int o);

    //returns the text to put into the save file
    vcl_ostream& save_text(vcl_ostream & = vcl_cerr);
    
protected:
    //unique identification number for this object
    int id;  
    // the area of this segment
    int area; 
    //the y coordinate of the top of the bounding box
    int top; 
    //the y coordinate of the bottom of the bounding box
    int bottom;
    //the x coordinate of the left side of the bounding box
    int left;
    //the x coordinate of the right side of the bounding box
    int right;
    //the number of links this object is away from the root node in the hierachical tree
    int depth;
    //the outline of the segment
    vgl_polygon<double>* outline;
    //list of all outlines (outside one and any inner ones)
    list<vgl_polygon<double>*> contours;
    //list of contained regions
    set<int> contained;
    //the mean intensities
    /*short r;
    short g;
    short b;*/

    vector<T> data_;


    //related to graphics display
    bool highlighted;
    bool outlined;
    
    //pointer to the segmentation manager
    //seg_object* manager;




};

#endif

