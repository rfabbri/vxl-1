//---------------------------------------------------------------------
// This is brcv/rec/dbru/dbru_rcor.h
//:
// \file
// \brief Class that finds correspondence between the pixels of polygonal input regions,
//        based on given correspondence of samples on the contour or
//        the correspondence of shock graph branches
//
// \author
//  O.C. Ozcanli - December 03, 2004
//
// \verbatim
//  Modifications
//    O.C. Ozcanli - May 16, 2005
//    O.C. Ozcanli - Nov 23, 2005  made this class a mere container of the correspondence
//   
// \endverbatim
//
//-------------------------------------------------------------------------

// If a pixel is within the polygonal region, it will be assumed to be valid and
// a correspondence will be found. The users of this class should assure that
// these pixels are in valid regions of the images they are imposed upon.
// (i.e. they should put checks like x <= w && y <= height, etc. for pixel (x, y) of regions)

#ifndef _dbru_rcor_h
#define _dbru_rcor_h

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <assert.h>

#include <dbcvr/dbcvr_cv_cor_sptr.h>
#include <dbskr/dbskr_sm_cor_sptr.h>

//#include <vtol/vtol_face_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vil/vil_image_resource_sptr.h>

//#include <dbskr/dbskr_scurve_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>

//: This class stores correspondence information between two regions 
//  defined by polygons. 
class dbru_rcor : public vbl_ref_count 
{
  friend class dbru_rcor_generator;

protected:
  //Data
  dbinfo_observation_sptr obs1_, obs2_;

  vgl_polygon<double> p1_, p2_; ///> corresponding polygons

  vcl_vector<vcl_vector <int> > region1_, region2_;
  vbl_array_2d< vgl_point_2d<int> > region1_map_output_;
  vbl_array_2d< vgl_point_2d<float> > region1_map_output_float_;
  
  //: this vector holds the corresponding region point indices
  vcl_vector<vcl_pair<unsigned, unsigned> > correspondences_;

  bool save_histograms_;
  bool save_float_;

  // The following two attributes are only necessary for "line intersections" algorithm
  // The related generation methods in dbru_rcor_generator class will be utilizing these

  //: this vector holds the vote distribution in the second region, for each pixel in region1
  //  this vector will be cleared if save_histograms_ is false
  //  save_histograms_ is set to true only if histograms will be displayed via matching tool
  vbl_array_2d < vcl_vector< vcl_pair< vgl_point_2d<int>, int > > > region1_histograms_;
  vbl_array_2d < vcl_vector< vcl_pair< vgl_point_2d<float>, int > > > region1_histograms_float_;

  //: save the sillhouette contour/polygon correspondence if curve matching
  dbcvr_cv_cor_sptr sil_cor_;

  //: save the shock graph correspondence after shock matching if shock matching
  dbskr_sm_cor_sptr sm_cor_;

  //: bounding boxes for the two polygons
  int max1_x_, max1_y_, min1_x_, min1_y_, max2_x_, max2_y_, min2_x_, min2_y_;

  //: upper left coordinates of these regions in the actual image
  int upper_x_, upper_y_, upper2_x_, upper2_y_;

  bool halt_;

  int cnt1_;
  int cnt2_;
  int cnt3_;

public:
  //: Constructor 
  //  Compute region correspondece between two dbinfo_observation instances
  dbru_rcor(dbinfo_observation_sptr obs1, dbinfo_observation_sptr obs2, bool save_histograms = false, bool save_float = false);

  //: Destructor
  virtual ~dbru_rcor() {};

  //-------------------------------------------------------------------
  // Access data members
  //-------------------------------------------------------------------

  //: return polygon1
  vgl_polygon<double> p1() { return p1_; }
  
  //: return polygon2
  vgl_polygon<double> p2() { return p2_; }
  
  //: return correspondence map
  vbl_array_2d< vgl_point_2d<int> >& get_map() { return region1_map_output_; }
  vbl_array_2d< vgl_point_2d<float> >& get_map_float() { return region1_map_output_float_; }
  
  //: return correspondences between points by their ids
  vcl_vector <vcl_pair< unsigned, unsigned> >& get_correspondences() { return correspondences_; }

  //: return vote distribution histograms 
  vbl_array_2d< vcl_vector< vcl_pair< vgl_point_2d<int>, int > > >& get_region1_histograms() { return region1_histograms_; }

  int get_min_x() { return min1_x_; }
  int get_min_y() { return min1_y_; }
  int get_min2_x() { return min2_x_; }
  int get_min2_y() { return min2_y_; }

  int get_upper1_x() { return upper_x_; }
  int get_upper1_y() { return upper_y_; }
  int get_upper2_x() { return upper2_x_; }
  int get_upper2_y() { return upper2_y_; }

  //: set silhouette contour/polygon correspondence
  void set_sil_cor(dbcvr_cv_cor_sptr sil_cor) { sil_cor_ = sil_cor; }

  //: get the pointer to the silhouette contour/polygon correspondence
  dbcvr_cv_cor_sptr get_sil_cor(void) { return sil_cor_; }

  //: set shock graph correspondence after shock matching
  void set_sm_cor(dbskr_sm_cor_sptr sm_cor) { sm_cor_ = sm_cor; }

  //: get the pointer to the shock graph correspondence
  dbskr_sm_cor_sptr get_sm_cor(void) { return sm_cor_; }

  //: get the image that shows the pixels used in region1/observation1
  vil_image_resource_sptr get_used_pixels1();

  //: get the image that shows the pixels used in region2/observation2
  vil_image_resource_sptr get_used_pixels2();

  //: get the image that puts corresponding region2 pixel intensities on top of region1 pixels
  vil_image_resource_sptr get_appearance2_on_pixels1();

  //: clear current region correspondence to allow for matching using the same sillhoette correspondence
  void clear_region_correspondence();

  //: dbru_rcor_generator class methods call the following for "line intersections" algorithm only
  void initialize_region1_histograms(); 

  //: this method should be called if save float option is turned on 
  //  after an instance is constructed 
  void initialize_float_map();
  
  void clear_region1_histograms();
  void set_save_histograms(bool val) { save_histograms_ = val; }
  void set_save_float(bool val) { save_float_ = val; }
  //: write the histogram of current pixel 
  //bool write_histogram(int x, int y, vcl_string file_name);
};

#endif // _dbru_rcor_h
