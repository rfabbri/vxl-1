#include <dbvis1/tool/dbvis1_edge_transform_tools.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_style.h>

//#include <bdgl/bdgl_curve_algs.h>
#include <ctrk/ctrk_DPMatch.h>
#include <ctrk/ctrk_curve_clustering.h>

dbvis1_edge_transform_tool::dbvis1_edge_transform_tool()
{
    gesture_register_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
    gesture_transform_ = vgui_event_condition(vgui_key('t'), true);
}

dbvis1_edge_transform_tool::~dbvis1_edge_transform_tool()
{
}

bool
dbvis1_edge_transform_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  bgui_vsol2D_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;
    
  if (this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau))){
    tableau_ = temp_tab;
    return true;
  }
  return false;
}

bool
dbvis1_edge_transform_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;
  //make sure its a vsol storage class
  if (storage->type() == "vsol2D"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

bgui_vsol2D_tableau_sptr
dbvis1_edge_transform_tool::tableau()
{
  return tableau_;
}

vidpro1_vsol2D_storage_sptr
dbvis1_edge_transform_tool::storage()
{
  vidpro1_vsol2D_storage_sptr vsol_storage;
  vsol_storage.vertical_cast(storage_);
  return vsol_storage;
}

double compute_mean(vcl_vector<double> t)
{
  double sum=0;
  for (unsigned int i=0; i<t.size(); ++i)
  {
    sum+=t[i];
  }
  sum=sum/t.size();
  return sum;
}
void transform_curve(vcl_vector<double> x, 
                     vcl_vector<double> y, 
                     vcl_vector<vsol_point_2d_sptr> &new_vertices, 
                     vnl_matrix <double> Tbar,
                     vnl_matrix <double> R,
                     double scale) {

    double x_cen=compute_mean(x);
    double y_cen=compute_mean(y);

    for (unsigned int i=0; i<x.size(); ++i)
    {
      double tempx=x[i]-x_cen;
      double tempy=y[i]-y_cen;

      double xt=scale*R(0,0)*tempx + scale*R(0,1)*tempy + Tbar(0,0)+x_cen;
      double yt=scale*R(1,0)*tempx + scale*R(1,1)*tempy + Tbar(1,0)+y_cen;

      vsol_point_2d_sptr temp = new vsol_point_2d(xt, yt);
      new_vertices.push_back(temp);
    }
}

bool
dbvis1_edge_transform_tool::handle( const vgui_event & e, 
                                   const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if ( gesture_register_(e) && curves_.size() < 2) {

    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_all();
    
    for (unsigned int i = 0; i<all_objects.size(); i++) {
      if (tableau_->is_selected(all_objects[i]->get_id())) {      
        vcl_cout << "found a selected curve, curves size: " << curves_.size() << "\n";
        if (((bgui_vsol_soview2D_polyline*)all_objects[i])->type_name_() == "bgui_vsol_soview2D_polyline") {
          curves_.push_back(((bgui_vsol_soview2D_polyline*)all_objects[i])->sptr());   

          tableau_->deselect(all_objects[i]->get_id());
          //tableau_->remove(all_objects[i]);
          //tableau_->add_vsol_polyline_2d(((bgui_vsol_soview2D_polyline*)all_objects[i])->sptr(),
            //                              vgui_style::new_style(0,1,0,2.0,2.0));
        }
      }
    }

    tableau_->post_redraw();
    return false;
  }

  if ( gesture_register_(e) && curves_.size() == 2) {

    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_all();
    
    for (unsigned int i = 0; i<all_objects.size(); i++) {
      if (tableau_->is_selected(all_objects[i]->get_id())) {      
        vcl_cout << "found a selected curve, curves size: " << curves_.size() << "\n";
        if (((bgui_vsol_soview2D_polyline*)all_objects[i])->type_name_() == "bgui_vsol_soview2D_polyline") {
          curves_.push_back(((bgui_vsol_soview2D_polyline*)all_objects[i])->sptr());   

        }
      }
    }

    tableau_->post_redraw();
    return false;
  }

  if ( gesture_transform_(e) && curves_.size() > 1 && curves_.size() < 4) {

    vsol_polyline_2d_sptr curve_2d1 = curves_[0];
    vsol_polyline_2d_sptr curve_2d2 = curves_[1];

    vcl_vector<vcl_pair<double,double> > points1;
    vcl_vector<vcl_pair<double,double> > points2;
    

    vcl_cout << "in curve 1 number of points: " << curve_2d1->size() << "\n";
    for (unsigned int j = 0; j<curve_2d1->size(); j++) {
      vcl_pair<double,double> newPt;
      newPt.first  = curve_2d1->vertex(j)->x();
      newPt.second = curve_2d1->vertex(j)->y();
      //vcl_cout << "x: " << newPt.first << " y: " << newPt.second << "\n";
      points1.push_back(newPt);
    }

    vcl_cout << "in curve 2 number of points: " << curve_2d2->size() << "\n";
    for (unsigned int j = 0; j<curve_2d2->size(); j++) {
      vcl_pair<double,double> newPt;
      newPt.first  = curve_2d2->vertex(j)->x();
      newPt.second = curve_2d2->vertex(j)->y();
      //vcl_cout << "x: " << newPt.first << " y: " << newPt.second << "\n";
      points2.push_back(newPt);
    }

    Curve c1; c1.readDataFromVector(points1);
    Curve c2; c2.readDataFromVector(points2);

    ctrk_DPMatch d1(c1,c2);
    d1.match();
    vcl_vector<int> tail1,tail2;
    d1.detect_tail(tail1,tail2);
    double b = d1.normfinalCost();
    vcl_cout << "cost of matching these curves: " << b <<"\n";
    double norm = 2.0;       //assuming norm is the largest possible value!!
    if (b>norm) b = norm;   // if curves matching cost is larger than 2, these nodes are not comparable.
    b = 1-b/norm;
    vcl_cout << "normalized cost: " << b <<"\n"; 

    double euc_dist           = d1.transformed_euclidean_distance();
    vcl_map<int,int> mapping  = d1.alignment;
    vnl_matrix <double> R     = d1.R;
    vnl_matrix <double> Tbar  = d1.Tbar;  // local one 
    double scale              = d1.scale;

    vcl_cout << "transformed euclidean distance between curves: " << euc_dist << "\n";
    vcl_cout << "R:\n";
    vcl_cout << R << "\n";
    vcl_cout << "Tbar:\n";
    vcl_cout << Tbar << "\n";
    vcl_cout << "scale: " << scale << "\n";

    
    vcl_vector<double> x;
    vcl_vector<double> y;
    vcl_vector<vsol_point_2d_sptr> new_vertices;

    for (unsigned int i=0; i<points1.size(); ++i)
    {
      x.push_back(points1[i].first);
      y.push_back(points1[i].second);
    }

    transform_curve(x, y, new_vertices, Tbar, R, scale);

    vcl_cout << "transformation computed..., size of new_vertices: " << new_vertices.size() << "\n";

    vsol_polyline_2d_sptr transformed_curve = new vsol_polyline_2d(new_vertices);
    tableau_->add_vsol_polyline_2d(transformed_curve, vgui_style::new_style(0,1,0,3.0,3.0));

    if (curves_.size() == 3) {
      curve_2d2 = curves_[2];
      points2.clear();
      vcl_cout << "in curve 3 number of points: " << curve_2d2->size() << "\n";
      for (unsigned int j = 0; j<curve_2d2->size(); j++) {
        vcl_pair<double,double> newPt;
        newPt.first  = curve_2d2->vertex(j)->x();
        newPt.second = curve_2d2->vertex(j)->y();
        //vcl_cout << "x: " << newPt.first << " y: " << newPt.second << "\n";
        points2.push_back(newPt);
      }

      x.clear(); y.clear();
      new_vertices.clear();
      for (unsigned int i=0; i<points2.size(); ++i)
      {
        x.push_back(points2[i].first);
        y.push_back(points2[i].second);
      }
      transform_curve(x, y, new_vertices, Tbar, R, scale);
      vsol_polyline_2d_sptr transformed_curve2 = new vsol_polyline_2d(new_vertices);
      tableau_->add_vsol_polyline_2d(transformed_curve2, vgui_style::new_style(0,1,0,3.0,3.0));
    }

    tableau_->post_redraw();
    return false;
  }  
  return false;

}

//: Return the name of this tool
vcl_string 
dbvis1_edge_transform_tool::name() const
{
  return "Edge Transform";
}


