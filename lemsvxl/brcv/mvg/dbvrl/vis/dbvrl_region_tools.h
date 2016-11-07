// This is brcv/mvg/dbvrl/vis/dbvrl_region_tools.h
#ifndef dbvrl_region_tools_h_
#define dbvrl_region_tools_h_
//:
// \file
// \brief Tools for assisting with registration
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 4/19/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include "dbvrl_region_tableau_sptr.h"
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <dbvrl/pro/dbvrl_region_storage_sptr.h>
#include <vimt/vimt_transform_2d.h>

#include <vimt/vimt_image_2d_of.h>

#include <dbvrl/dbvrl_region_sptr.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>



//: A tool for transforming dbvrl regions
class dbvrl_region_transform_tool : public bvis1_tool
{
public:
  enum states {TRANSLATE, SCALE, ROTATE, SKEW, PROJECT};
  dbvrl_region_transform_tool();
  virtual ~dbvrl_region_transform_tool();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

private:
  //: update the current transform
  void update_xform();

protected:

  dbvrl_region_tableau_sptr tableau_;
  dbvrl_region_storage_sptr xform_storage_;

  states state_;
  double init_x_, init_y_;
  float  last_x_, last_y_;

  bool active_;
  int active_corner_;
  vimt_transform_2d curr_xform_;
  dbvrl_region_sptr curr_region_;

  vcl_vector< vgl_point_2d<double> > vertices_;
  vcl_vector< vgl_point_2d<double> > new_corners_;
  vgl_point_2d<double> min_bound_;
  vgl_point_2d<double> max_bound_;
  dbvrl_region_tableau_sptr active_tableau_;
   
};





//: A tool for super resolution
class dbvrl_region_super_res_tool : public bvis1_tool
{
public:
  dbvrl_region_super_res_tool();
  virtual ~dbvrl_region_super_res_tool();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

protected:

  //: Registered images
  vcl_vector<vimt_image_2d_of<vxl_byte> > images_; 

  //: The region (defines the size of the resolved image)
  dbvrl_region_sptr region_;

};

#endif // dbvrl_region_tools_h_
