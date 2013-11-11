//This is contrib/fine/gui/dbseg_seg_object.h
#ifndef dbseg_seg_object_hh_
#define dbseg_seg_object_hh_

//:
// \file
// \the managing class for segmentation
// \author Eli Fine
// \date 7/29/08
//
//
//To Do:
//If a region surrounds others that are continuous, it will not join their outlines
//    this is a purely visual issue and has no effect on area calculations
//    if the inner regions are manually joined then the correct inner outline will be displayed
//  Solution - write a finish_trace() method into the dbseg_seg_spatial_object class that compares all possible combinations of the inner regions to see
//            if they are continuous and then uses the vgl_polygon union  method to join them


// \verbatim
//  Modifications
// \endverbatim
#include <vul/vul_timer.h>

#include <vgl/vgl_clip.h>

#include <vil/vil_image_view.h>
#include <vil/vil_rgb.h>
#include <set>
#include <vil/vil_image_view_base.h>
#include <vil/io/vil_io_image_view.h>
#include <vgl/io/vgl_io_polygon.h>
#include <vsl/vsl_vector_io.h>

#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_vector.h>

#include <vsl/vsl_binary_io.h>

//for tracing
#include <dbdet/tracer/dbdet_contour_tracer.h>

//for random numbers for color pattern
/*#include <cstdlib>
#include <ctime>*/


//#include "seg_spatial_object.h" already in dbseg_seg_tree
#include "dbseg_seg_tree.h"
#include "dbseg_seg_neighborhood.h"
#include <structure/dbseg_seg_object_base.h>


template <class T> 
class dbseg_seg_object : public dbseg_seg_object_base {

public:
    enum TraceType {
        INIT,
        ADD,
        REMOVE,
        CREATE,
        EDIT,
        RETRACE
    };

    enum SpinePart {
        C1,
        D1,
        C2,
        D2,
        C3,
        D3,
        C4,
        D4,
        C5,
        SPINE
    };


    dbseg_seg_object();
    dbseg_seg_object(vil_image_view<int> labeled, vil_image_view<T> original);

    //for loading
    dbseg_seg_object(vil_image_view<int> labeled, 
            vil_image_view<T> original, 
            list<int> validL, 
            vector<dbseg_seg_spatial_object<T>*> objL, 
            vector<int> parentL, vector<list<int>> childL, 
            vector<list<int>> depthL,
            vector<list<int>> neighborL, vector<int> spineP, vector<double> disk_cornersXL, vector<double> disk_cornersYL);

    //old version
    dbseg_seg_object(vil_image_view<int> labeled, 
            vil_image_view<T> original, 
            list<int> validL, 
            vector<dbseg_seg_spatial_object<T>*> objL, 
            vector<int> parentL, vector<list<int>> childL, 
            vector<list<int>> depthL,
            vector<list<int>> neighborL);



    ~dbseg_seg_object();
    //returns a pointer to the list of objects (which is found in the dbseg_seg_tree)
    vector<dbseg_seg_spatial_object<T>*>& get_object_list();
    //returns pointer to parent list (found in dbseg_seg_tree)
    vector<int>& get_parent_list();
    //returns children list (from dbseg_seg_tree)
    vector<list<int>>& get_children_list();
    //returns labeled image
    vil_image_view<int>& get_labeled_image();
    //returns segment tree
    dbseg_seg_tree<T>* get_tree();
    //returns the id of the object that contains this coordinate at the specified depth level
    int get_pixel_ID(int x, int y, int l);
    //returns the list of neighbors for object o at the same depth level (from neighborhood)
    list<int> get_neighbors(int o);
    //returns the list of neighbors for object o at a specified depth level (from neighborhood)
    list<int> get_neighbors(int o, int d);
    //returns the neighborhood object of this manager
    dbseg_seg_neighborhood* get_neighborhood();
    //returns the list of neighbors (from neighborhood)
    vector<list<int>>& get_neighbor_list();
    //creates the bounding box for the object and sets mean intensity
    void create_bounding_box(int o);
    //traces the outline of the segment and poplulates the neighborhood (if it is a base object)
    void trace(int o, TraceType type);
    //returns the image
    vil_image_view<T>& get_image();
    //adds child and updates tree and neighborhood, returns false if c was already child of p
    bool add_child(int c, int p);
    //removes child and updates tree and neighborhood, returns false if c was not a child of p or if it was the last child
    bool remove_child(int c, int p);
    //returns a list of all of the pixels in a region
    list<vsol_point_2d_sptr> get_pixels(int o);
    //returns the outline of a region
    //vsol_polygon_2d_sptr get_voutline(int o);
    list<vgl_polygon<double>*> get_outline(int o);
    //returns an image to display at depth and whether or not an object is being edited
    vil_image_view<vxl_byte> get_display(int dep, int editing = 0);
    //returns a list of all of the outlines at a certain depth for display 
    //list<vsol_polygon_2d_sptr> get_voutlines(int dep);
    list<list<vgl_polygon<double>*>> get_outlines(int dep, list<int> outL, bool all=false);


    //returns the deepest level of any object
    int get_max_depth() const;

    //gets the list of objects at a specified depth
    list<int> get_depth_list(int dep);

    //returns a list of all currently existing object IDs
    list<int>& get_valid_IDs();

    //creates a region that contains the list of children regions
    bool create_region(list<int>);

    double get_area(int o);

    //tests if a list of objects are connected
    int is_continuous(list<int> l, int d = 0);

    bool edit_children(list<int> l, int p);

    bool add_to_region(list<int> l, int o);

    bool remove_from_region(list<int> l, int o);

    //related to graphics display
    void toggle_highlighted(int o);
    void toggle_outlined(int o);
    bool is_highlighted(int o);
    bool is_outlined(int o);


    //returns the text to put into the save file
    vcl_ostream& save_text(vcl_ostream & = vcl_cerr);

    //clears all neighborhood relationships with this object and resets them
    void reset_neighbors(int o, list<int> l);


    int get_discontinuous();

    int get_child_count(int o);

    list<int> get_outlined_regions();


     // ==== Binary IO methods ======

      //: Binary save self to stream.
      void b_write(vsl_b_ostream &os) const;

      //: Binary load self from stream.
      void b_read(vsl_b_istream &is);

      //: Return IO version number;
      short version() const;

      //: Print an ascii summary to the stream
      void print_summary(vcl_ostream &os) const;

      //: Return a platform independent string identifying the class
      virtual vcl_string is_a() const {
          return vcl_string("dbseg_seg_object"+vcl_string(typeid(T).name())); }

      //: Return true if the argument matches the string identifying the class or any parent class
      virtual bool is_class(vcl_string const& cls) const { return cls==is_a(); }



      //spine and measurements
      void set_spine_part(SpinePart p, int o);
      int get_spine_part(SpinePart p);
      int get_spine_part(int p);

      void set_disk_corner(int p, bool bottom, double x, double y);
      void get_disk_corners(int p, double& x1, double& y1, double& x2, double& y2);

      void set_disk_pro(int p, double x, double y);
      void get_disk_pro(int p, double& x, double& y);


      vgl_polygon<double> get_disk_pro_poly(int p);
      void set_disk_pro_poly(int p, vgl_polygon<double> poly);
      //SpinePart num_to_enum(int i);

protected:
    //inital scan of image to create segment objects
    void initial_scan();

    //recursive check for continuous regions
    bool continuousR(int curr, int goal, set<int> possible, set<int>&, int d = 0);

    //grows the outline from children when creating a new region
    vgl_polygon<double>& growOutline(vgl_polygon<double>&, set<int>&, set<int>&, int curr);

    //the image labeled by segmented regions as outputted by the algorithm
    vil_image_view<int> labeled_image;
    //the segment tree hierarchy
    dbseg_seg_tree<T>* tree;
    //the ID of the last object that was created
    int idCount;
    //the actual image being segmented
    vil_image_view<T> image;
    //the adjacency neighborhood
    dbseg_seg_neighborhood* neighborhood;

    //updates the list of outlined regions
    void update_outlined(int o);

    //color values for each base region (randomized at initialization
    /*vector<int> r;
    vector<int> g;
    vector<int> b;
    */
    int discontinuous;

    list<int> outlinedRegions;

    //error checking
    int totalPixels;


    //for spine segmentation
    vector<int> spine_parts;

    vector<double> disk_cornersX;
    vector<double> disk_cornersY;

    vector<vgl_polygon<double>> disk_pro_polys;
    vector<double> disk_proX;
    vector<double> disk_proY;


};


//: Binary save dbsta_gaussian_sphere* to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const dbseg_seg_object<T>* p);

//: Binary load dbsta_gaussian_sphere* from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, dbseg_seg_object<T>* &p);



#endif

