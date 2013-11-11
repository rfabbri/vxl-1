// This is brcv/shp/dbsk2d/algo/dbsk2d_shock_transforms.cxx

//:
// \file

#include <vcl_algorithm.h>
#include <vcl_cmath.h>

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_ishock_graph.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_grouped_ishock_edge.h>
#include <dbsk2d/dbsk2d_shock_ishock_node.h>
#include <dbsk2d/algo/dbsk2d_shock_transforms.h>

#include <dbsk2d/dbsk2d_ishock_contact.h>             
#include <dbsk2d/dbsk2d_ishock_pointpoint.h>         
#include <dbsk2d/dbsk2d_ishock_pointline.h>          
#include <dbsk2d/dbsk2d_ishock_pointarc.h>           
#include <dbsk2d/dbsk2d_ishock_lineline.h>           
#include <dbsk2d/dbsk2d_ishock_linearc.h>            
#include <dbsk2d/dbsk2d_ishock_arcarc.h>             
#include <dbsk2d/dbsk2d_ishock_lineline_thirdorder.h>
#include <dbsk2d/dbsk2d_ishock_arcarc_thirdorder.h> 

#include <dbsol/algo/dbsol_curve_algs.h>
#include <vsol/vsol_polyline_2d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_bilin_interp.h>
#include <bil/algo/bil_color_conversions.h>

//#include <bsta/bsta_histogram.h>
#include <vnl/vnl_vector_fixed.h>

// the function that will operate on intrinsic shock graph and hide the degree two node
// and its immediate intrinsic edges
// compile_coarse_shock_graph() method should be run once after this to recreate the "entire" coarse shock graph
// this is the best way to make sure that the ordering of edges are correct after gap transforms
bool dbsk2d_shock_transforms::gap_transform_only_hide(dbsk2d_ishock_edge* iedge)
{
  if (!valid_gap_iedge(iedge))  // checks if the end points had become obsolete
    return false;

  dbsk2d_ishock_node* inode = iedge->pSNode();
  if (!inode)
    return false;

  //if (gap_type_ != DEGENERATEDEGTHREE_GAP)
    //inode->hide();

  // hide all children of this source node!!
  dbsk2d_ishock_edge* first_child = inode->cShock();  // first child
  if (first_child)
    first_child->hide();
  dbsk2d_ishock_edge* second_child = inode->cShock2();  // second child
  if (second_child)
    second_child->hide();

  if (gap_type_ == DEGENERATEDEGTHREE_GAP) {
    ishock_edge_list::iterator curS = inode->pShocks().begin();
    for(; curS!=inode->pShocks().end(); curS++) {
      dbsk2d_ishock_edge* parent = (*curS);
      vcl_vector<dbsk2d_ishock_edge*> to_be_hidden;
      to_be_hidden.push_back(parent);
      // hide all the nodes till a degree two source
      dbsk2d_ishock_node* current_node = parent->pSNode();
      if (!current_node)
        continue;

      while (current_node->is_a_junct()) { // does not excludes hidden edges in this check, good!!
        ishock_edge_list::iterator icurS = current_node->pShocks().begin();
        int non_contact = 0;
        for(; icurS!=current_node->pShocks().end(); icurS++) {
          dbsk2d_ishock_edge* ip = (*icurS);
          if (ip->is_a_contact())
            continue;
          non_contact++;
        }
        if (non_contact >= 2)
          break;

        current_node->hide();
        dbsk2d_ishock_edge* parent_prev = parent;
        parent = ishock_graph->cyclic_adj_succ(parent, current_node, true); // exclude hiddens
        if (!parent) {
          vcl_cout << "parent not found in DEGENERATEDEGTHREE GAP transform!\n"; break;
        }
        if (parent == parent_prev)  // if current_node is an end nodes
          break;

        to_be_hidden.push_back(parent);
        current_node = parent->pSNode();
        if (!current_node)
          break;
      }

      for (unsigned i = 0; i < to_be_hidden.size(); i++)
        to_be_hidden[i]->hide();
    }
  }
  
  // make the end points obsolete
  if (bpoint1_)
    obsolete_end_points_.insert(bpoint1_);
  if (bpoint2_)
    obsolete_end_points_.insert(bpoint2_);

  return true;
}

void dbsk2d_shock_transforms::recompile_coarse() {
  shock_graph->clear();
  compile_coarse_shock_graph();
}

bool dbsk2d_shock_transforms::gap_transform(dbsk2d_ishock_edge* iedge) {
  if (gap_transform_only_hide(iedge)) {
    //shock_graph->clear(); 
    //compile_coarse_shock_graph();
    return true;
  } else
    return false;
}

//: remove the edge and correct the shock graph depending on the type of remaining node
bool dbsk2d_shock_transforms::gap_transform(dbsk2d_shock_edge_sptr sedge) {
  dbsk2d_ishock_edge* grouped_edge = (dbsk2d_ishock_edge *)(sedge.ptr());
  if (grouped_edge) {
    dbsk2d_shock_grouped_ishock_edge* cur_edge = (dbsk2d_shock_grouped_ishock_edge*)grouped_edge;
    if (cur_edge) {
      coarse_shock_graph_source_ = sedge->source();
      dbsk2d_ishock_edge* iedge = *(cur_edge->edges().begin());
      return gap_transform(iedge);
    }
  }

  return false;
}

//: remove the edge and correct the shock graph depending on the type of remaining node
bool dbsk2d_shock_transforms::gap_transform(dbsk2d_shock_node_sptr snode) {
  dbsk2d_shock_ishock_node* sinode = (dbsk2d_shock_ishock_node*) snode.ptr();
  
  if (sinode)
    if (sinode->ishock_node()->cShock()) {
      coarse_shock_graph_source_ = snode;
      return gap_transform(sinode->ishock_node()->cShock());
    }

  return false;
}

bool dbsk2d_shock_transforms::valid_gap(dbsk2d_shock_edge_sptr sedge) {
  dbsk2d_ishock_edge* grouped_edge = (dbsk2d_ishock_edge *)sedge.ptr();
  if (grouped_edge) {
    dbsk2d_shock_grouped_ishock_edge* cur_edge = (dbsk2d_shock_grouped_ishock_edge*)grouped_edge;
    if (cur_edge) {
      coarse_shock_graph_source_ = sedge->source();
      dbsk2d_ishock_edge* iedge = *(cur_edge->edges().begin());
      return valid_gap_iedge(iedge);
    }
  }

  return false;
}
bool dbsk2d_shock_transforms::valid_gap(dbsk2d_shock_node_sptr snode) {
   dbsk2d_shock_ishock_node* sinode = (dbsk2d_shock_ishock_node*) snode.ptr();
  if (sinode)
    if (sinode->ishock_node()->cShock()) {
      coarse_shock_graph_source_ = snode;
      return valid_gap_iedge(sinode->ishock_node()->cShock());
    }
  return false;
}

int dbsk2d_shock_transforms::type_iedge(dbsk2d_ishock_edge *iedge) {
  bpoint1_ = 0;
  bpoint2_ = 0;

  dbsk2d_ishock_belm* lbe = iedge->lBElement();  ///< the left boundary element
  dbsk2d_ishock_belm* lbr = iedge->rBElement();

  bool first_point_found = false;
  if (lbe->is_a_point() && ((dbsk2d_ishock_bpoint*)lbe)->is_an_end_point()) {
    bpoint1_ = (dbsk2d_ishock_bpoint*)lbe;
    first_point_found = true;
  }
  bool second_point_found = false;
  if (lbr->is_a_point() && ((dbsk2d_ishock_bpoint*)lbr)->is_an_end_point()) {
    bpoint2_ = (dbsk2d_ishock_bpoint*)lbr;
    second_point_found = true;
  }

  dbsk2d_ishock_node* inode = iedge->pSNode();

  if (first_point_found) {
    if (second_point_found)
      if (inode->pShocks().size() > 1) {
        dbsk2d_ishock_edge* pe1 = *(inode->pShocks().begin());
        if (!pe1->is_a_contact() && !pe1->isHidden()) {
          dbsk2d_ishock_edge* pe2 = *(inode->pShocks().begin()++);
          if (!pe2->is_a_contact() && !pe2->isHidden())
            return DEGENERATEDEGTHREE_GAP;
        }
      }
      else
        return DEGENERATE_GAP;
    else
      if (inode->pShocks().size() == 0)
        return SEMIDEGENERATE1_GAP;
  } else {
    if (second_point_found)
      if (inode->pShocks().size() == 0)
        return SEMIDEGENERATE2_GAP;
  }
  return REGULAR_GAP;
}

bool dbsk2d_shock_transforms::valid_gap_iedge(dbsk2d_ishock_edge *iedge) {
  if (!iedge)
    return false;

  gap_type_ = type_iedge(iedge);
  
  if (gap_type_ == DEGENERATE_GAP || gap_type_ == DEGENERATEDEGTHREE_GAP) {
    valid_gap_node_radius_ = iedge->H()/2.0f;
    obsolete_iter i1, i2;
    i1 = obsolete_end_points_.find(bpoint1_);
    i2 = obsolete_end_points_.find(bpoint2_);
    if (i1 == obsolete_end_points_.end() || i2 == obsolete_end_points_.end()) // not closed before
      return true;
  } else if (gap_type_ == SEMIDEGENERATE1_GAP) {
    valid_gap_node_radius_ = iedge->H()/2.0f;
    obsolete_iter i = obsolete_end_points_.find(bpoint1_);
    if (i == obsolete_end_points_.end()) // not closed before
      return true;
  } else if (gap_type_ == SEMIDEGENERATE2_GAP) {
    valid_gap_node_radius_ = iedge->H()/2.0f;
    obsolete_iter i = obsolete_end_points_.find(bpoint1_);
    if (i == obsolete_end_points_.end()) // not closed before
      return true;
  } 

  return false;  // if REGULAR or the end points are OBSOLETE
}


//: For sorting pairs by their first element
inline bool
first_less( const vcl_pair<double,dbsk2d_ishock_edge*>& left,
            const vcl_pair<double,dbsk2d_ishock_edge*>& right )
{
  return left.first < right.first;
}

//: perform all possible gap transforms within threshold in a rank ordered fashion
void dbsk2d_shock_transforms::perform_all_gap_transforms(double thres_contour, double thres_app, 
                                                         double alpha_contour, double alpha_app, bool keep_eulerspirals)
{
  if (!image_set_) {
    vcl_cout << "IMAGE is not set!!\n";
  }

  vcl_vector<vcl_pair<double, dbsk2d_ishock_edge*> > gap_edges;
  
  // traverse all coarse shock edges
  for (dbsk2d_shock_graph::edge_iterator curE = shock_graph->edges_begin(); curE != shock_graph->edges_end(); curE++)
  {
    dbsk2d_ishock_edge* grouped_edge = (dbsk2d_ishock_edge *)((*curE).ptr());
    dbsk2d_shock_grouped_ishock_edge* cur_edge = (dbsk2d_shock_grouped_ishock_edge*)grouped_edge;

    dbsk2d_ishock_edge* iedge;  // find first nonzero iedge
    for (vcl_list<dbsk2d_ishock_edge*>::iterator curIE = cur_edge->edges().begin(); curIE != cur_edge->edges().end(); ++ curIE) {
      iedge = (*curIE);
      if (iedge == 0) continue;
      break;
    }
      
    if (valid_gap_iedge(iedge)) {
      double cont_cost, app_cost;     // contour cost may change for the gap edges that share the same source node
                                      // ozge TODO: appearance cost is symetric, there is some redundancy here!!
      coarse_shock_graph_source_ = (*curE)->source();
      if (coarse_shock_graph_source_->id() != iedge->pSNode()->id())
        continue;   // Problem!!
      if (close_the_gap(thres_contour, thres_app, alpha_contour, alpha_app, cont_cost, app_cost)) {
        gap_edges.push_back(vcl_pair<double, dbsk2d_ishock_edge*> (((alpha_contour*cont_cost)+(alpha_app*app_cost))/2.0f, iedge));
      }
      clear_all();
    }
    
  }
    
  //vcl_cout << "number of gap edges satisfying the thresholds: " << gap_edges.size() << " sorting... ";
  vcl_sort( gap_edges.begin(), gap_edges.end(), first_less );
  //vcl_cout << " DONE!\n";

  vcl_vector<vcl_pair<double, dbsk2d_ishock_edge*> >::iterator iter;
  for (iter = gap_edges.begin(); iter != gap_edges.end(); iter++) {
    if (gap_transform_only_hide(iter->second)) {
      //vcl_cout << "\nperformed on node with cost: " << iter->first << "\n";
      if (keep_eulerspirals) {
        gap_transform_contour_cost();
        if (es_)
          ess_.push_back(new dbgl_eulerspiral(*es_));
      }
    } //else
      //vcl_cout << "this transform had become OBSOLETE by a previous transform\n";
  }

  shock_graph->clear();
  compile_coarse_shock_graph();
  return;
}

void 
dbsk2d_shock_transforms::get_eulerspirals(vcl_vector< vsol_spatial_object_2d_sptr >& contours)
{
  for (unsigned i = 0; i < ess_.size(); i++) {
    vcl_vector<vgl_point_2d<double> > point_samples;
    vcl_vector<vsol_point_2d_sptr> ps;
    ess_[i]->compute_spiral(point_samples, 0.1);
    for (unsigned j = 0; j < point_samples.size(); j++)
      ps.push_back(new vsol_point_2d(point_samples[j]));
    vsol_polyline_2d_sptr poly = new vsol_polyline_2d(ps);
    contours.push_back(poly->cast_to_spatial_object());
  }
}

//: degree 2 node has immediate neighbor points, we use the radius and the nearest boundary at those points to extend the region beyond degree 2's immediate shock edges
double dbsk2d_shock_transforms::
create_till_boundary_interpolators(dbsk2d_shock_edge_sptr sedge, dbsk2d_shock_node_sptr tgtn, dbsol_interp_curve_2d_sptr& c)
{
  //dbsk2d_shock_node_sptr tgtn = (sedge.ptr())->opposite(srcn);
  dbsk2d_ishock_edge *grouped_edge = (dbsk2d_ishock_edge *)(sedge.ptr());
  dbsk2d_shock_grouped_ishock_edge* cur_edge = (dbsk2d_shock_grouped_ishock_edge*)grouped_edge;
  // get the last nonzero intrinsic edge on sedge
  dbsk2d_ishock_edge *iedge;  
  for (vcl_list<dbsk2d_ishock_edge*>::iterator curIE = cur_edge->edges().begin(); curIE != cur_edge->edges().end(); ++ curIE) {
    dbsk2d_ishock_edge* cur_iedge = (*curIE);
    if (cur_iedge == 0) continue;
    iedge = cur_iedge;
  }

  dbsk2d_ishock_edge* isedge = ishock_graph->cyclic_adj_succ(iedge, iedge->target(), false); // using hidden edges as well
  while (isedge && isedge->is_a_contact()) {
    isedge = ishock_graph->cyclic_adj_succ(isedge, iedge->target(), false); // using hidden edges as well
    if (isedge == iedge) {// not found!!!
      vcl_cout << "PROBLEM!!! An eligible edge successor is not found!!\n";
      return -1;
    }
  }

  if (!isedge) {
    vcl_cout << "PROBLEM!!! An eligible edge successor is not found!!\n";
    return -1;
  }
  
  vcl_vector<vgl_point_2d<double> > pts;
  pts.push_back(tgtn->pt());

  vgl_point_2d<double> pt;
  bool start = iedge->target() == isedge->source() ? true : false;
  
  //we need the shock edge type
  switch (isedge->type()){
    case dbsk2d_ishock_elm::POINTPOINT : {
      dbsk2d_ishock_pointpoint *ie = (dbsk2d_ishock_pointpoint *)isedge;
      start ? pt = ie->getLFootPt(ie->sTau()) : pt = ie->getRFootPt(ie->eTau());
      break; }
    case dbsk2d_ishock_elm::POINTLINE : {
      dbsk2d_ishock_pointline *ie = (dbsk2d_ishock_pointline *)isedge;
      start ? pt = ie->getLFootPt(ie->sTau()) : pt = ie->getRFootPt(ie->eTau());  
      break; }
    case dbsk2d_ishock_elm::LINELINE : {
      dbsk2d_ishock_lineline *ie = (dbsk2d_ishock_lineline *)isedge;
      start ? pt = ie->getLFootPt(ie->sTau()) : pt = ie->getRFootPt(ie->eTau());
      break; }
    case dbsk2d_ishock_elm::POINTARC : {
      dbsk2d_ishock_pointarc *ie = (dbsk2d_ishock_pointarc *)isedge;
      start ? pt = ie->getLFootPt(ie->sTau()) : pt = ie->getRFootPt(ie->eTau());
      break; }
    case dbsk2d_ishock_elm::LINEARC : {
      dbsk2d_ishock_linearc *ie = (dbsk2d_ishock_linearc *)isedge;
      start ? pt = ie->getLFootPt(ie->sTau()) : pt = ie->getRFootPt(ie->eTau());
      break; }
    case dbsk2d_ishock_elm::ARCARC : {
      dbsk2d_ishock_arcarc *ie = (dbsk2d_ishock_arcarc *)isedge;
      start ? pt = ie->getLFootPt(ie->sTau()) : pt = ie->getRFootPt(ie->eTau());
      break; }
    case dbsk2d_ishock_elm::LINELINE_THIRDORDER : {
      dbsk2d_ishock_lineline_thirdorder *ie = (dbsk2d_ishock_lineline_thirdorder *)isedge;
      start ? pt = ie->getLFootPt(ie->sTau()) : pt = ie->getRFootPt(ie->eTau());
      break; }
    case dbsk2d_ishock_elm::ARCARC_THIRDORDER : {
      dbsk2d_ishock_arcarc_thirdorder *ie = (dbsk2d_ishock_arcarc_thirdorder *)isedge;
      start ? pt = ie->getLFootPt(ie->sTau()) : pt = ie->getRFootPt(ie->eTau());
      break; }
    default: break;
  } 
  pts.push_back(pt);
 
  c = new dbsol_interp_curve_2d();
  dbsol_curve_algs::interpolate_linear(c.ptr(), pts, false);
  return c->length();
}

// merge edges till you hit a degree three node!! 
// If we're at a degerate degree two node, immediate neighbors are degree threes
// in semidegenerate case, we should continue all the way to the first degree three, merging all degree twos on the way if there are any 

double dbsk2d_shock_transforms::
create_shock_interpolators(dbsk2d_shock_edge_sptr sedge, dbsol_interp_curve_2d_sptr& c, dbsk2d_shock_edge_sptr& new_edge, dbsk2d_shock_node_sptr& new_node)
{
  vcl_vector<vgl_point_2d<double> > expts;

  dbsk2d_shock_grouped_ishock_edge* sedgeptr = (dbsk2d_shock_grouped_ishock_edge*)sedge.ptr();
  expts.insert(expts.begin(), sedgeptr->ex_pts().begin(), sedgeptr->ex_pts().end());

  new_edge = sedge;
  new_node = sedge->target();
  dbsk2d_shock_node_sptr target = sedge->target();
  if (!target)
    return -1;
  dbsk2d_shock_edge_sptr current = shock_graph->cyclic_adj_succ(sedge, target);
  while (target->degree() != 3) {
    dbsk2d_shock_grouped_ishock_edge* sptr = (dbsk2d_shock_grouped_ishock_edge*)current.ptr();
    
    if (target == current->source()) {
      expts.insert(expts.end(), sptr->ex_pts().begin(), sptr->ex_pts().end());
      target = current->target();
      new_node = target;
    } else {
      expts.insert(expts.end(), sptr->ex_pts().rbegin(), sptr->ex_pts().rend());
      target = current->source();
      new_node = target;
    }
    if (!target)
      break;
    new_edge = current;
    current = shock_graph->cyclic_adj_succ(current, target);
  }

  c = new dbsol_interp_curve_2d();
  dbsol_curve_algs::interpolate_linear(c.ptr(), expts, false);
  return c->length();
}




//: return the cost of gap transform if its valid, otherwise return -1
double dbsk2d_shock_transforms::gap_transform_contour_cost()
{
  if (bpoint1_ && bpoint2_) {

    vgl_point_2d<double> start, end;
    double start_angle = -1, end_angle = -1;

    start_angle = bpoint1_->tangent();
    start = bpoint1_->pt();
    end_angle = bpoint2_->tangent();
    end = bpoint2_->pt();

    // remove the polarization using a reference direction
    vgl_line_2d<double> ref_line(end, start);
    double ref_angle = angle0To2Pi(ref_line.slope_radians());
    double dirs, dire;
    if (_angle_vector_dot(ref_angle, start_angle)  >= 0)
      dirs = angle0To2Pi(start_angle);
    else
      dirs = angle0To2Pi(start_angle+vnl_math::pi);
    if (_angle_vector_dot(ref_angle, end_angle)  >= 0)
      dire = angle0To2Pi(end_angle);
    else
      dire = angle0To2Pi(end_angle+vnl_math::pi);
    es_ = new dbgl_eulerspiral(start, dirs, end, dire);

    double len = es_->length();
    //double len_power = len < 1.0f ? len : vcl_pow(es_->length(), double(3.0f));
    //double E = (vcl_pow(es_->gamma(), double(2.0f))*len_power + vcl_pow(es_->average_curvature(), double(2.0f))*len_power);
    //double E = (vcl_pow(es_->gamma(), double(2.0f)) + vcl_pow(es_->average_curvature(), double(4.0f)))*len;
    double E_gc = (vcl_pow(es_->curvature_at_length(len), double(5.0f))-vcl_pow(es_->curvature_at_length(0.0f), double(5.0f)))/(5*es_->gamma());
    E_gc = (E_gc + len*vcl_pow(es_->gamma(), double(2.0f)))*vcl_pow(len, double(3.0f));
    
    double E_L = curve_offset_ + vcl_pow(len/curve_length_gamma_, curve_power_);
    double E = 0;
    if (len < 1) { // less than 1 pixel
      E = E_L*curve_gamma_;  // I don't want scaling with curve_gamma_ if just using length
    } else {
      E = E_gc*E_L;
    }
    
#if 0
    vcl_cout << "ES L: " << len << " ";
    vcl_cout << "  gamma: " << es_->gamma() << "  K_0: " << es_->curvature_at_length(0.0f) << " K_f: " << es_->curvature_at_length(len) << "\n";
    vcl_cout << "E_gc: " << E_gc << " E_L: " << E_L << " E: " << E << " "; 
    vcl_cout << "[1-e^(-E/c_gamma)]: " << 1-vcl_exp(-E/curve_gamma_) << vcl_endl; 
#endif

    return 1.0f - vcl_exp(-E/curve_gamma_); 
  } else {
    // cost is only the radius of the node for now
    double len = valid_gap_node_radius_*2.0f;  // valid_gap_node_radius_ should have been set in valid_gap_node method 
    double E_L = curve_offset_ + vcl_pow(len/curve_length_gamma_, curve_power_);
#if 0
    vcl_cout << "2*radius: " << len << " E_L: " << E_L << " e^(-E_L): " << vcl_exp(-E_L) << " [1-e^(-E_L)]: " << 1-vcl_exp(-E_L) << vcl_endl; 
#endif

    if (bpoint1_) 
      es_ = new dbgl_eulerspiral(bpoint1_->pt(), angle0To2Pi(bpoint1_->tangent()+vnl_math::pi), 0, 0, len);
    else {
      if (bpoint2_)
        es_ = new dbgl_eulerspiral(bpoint2_->pt(), angle0To2Pi(bpoint2_->tangent()+vnl_math::pi), 0, 0, len);
      else
        es_ = 0;
    }
    

    return 1.0f - vcl_exp(-E_L); // no dividing by curve_gamma_ since curve_gamma is scaling E_gc not E_L 
  }
}


//: return the appearance cost of gap transform if the regions are created
double dbsk2d_shock_transforms::gap_transform_appearance_cost() {
    
  if (!coarse_shock_graph_source_) {
    vcl_cout << "PROBLEM!!!!\n coarse shock graph source for this gap transform was not set!!!\n";
    return -1;
  }

  dbsk2d_shock_edge_sptr sedge, othersedge, othersedge2;
  if (gap_type_ == DEGENERATEDEGTHREE_GAP) {
    sedge = *(coarse_shock_graph_source_->out_edges().begin());
    vcl_list<dbsk2d_shock_edge_sptr> in_edges = coarse_shock_graph_source_->in_edges();
    if (in_edges.size() < 2) {
      vcl_cout << "PROBLEM in DEGENERATE DEGREE THREE SOURCE\n";
      return -1;
    }
    vcl_list<dbsk2d_shock_edge_sptr>::iterator first_parent = (in_edges.begin());
    vcl_list<dbsk2d_shock_edge_sptr>::iterator second_parent = first_parent++;
    othersedge = shock_graph->cyclic_adj_succ(*first_parent, (*first_parent)->source());
    othersedge2 = shock_graph->cyclic_adj_succ(*second_parent, (*second_parent)->source());
  } else {
    sedge = shock_graph->first_adj_edge(coarse_shock_graph_source_);
    othersedge = shock_graph->cyclic_adj_succ(sedge, coarse_shock_graph_source_);
    othersedge2 = 0;
  }
  
  dbsol_interp_curve_2d_sptr shock_c1, shock_c2, shock_c3;
  // merge edges till you hit a degree three node!! 
  // If we're at a degerate degree two node, immediate neighbors are degree threes
  // in semidegenerate case, we should continue all the way to the first degree three, merging all degree twos on the way if there are any
  dbsk2d_shock_node_sptr new_node1, new_node2, new_node3;
  dbsk2d_shock_edge_sptr new_edge1, new_edge2, new_edge3;
  double len1 = create_shock_interpolators(sedge, shock_c1, new_edge1, new_node1);
  double len2 = create_shock_interpolators(othersedge, shock_c2, new_edge2, new_node2);
  double len5 = 100000000;

  if (len1 < 0 || len2 < 0 || !new_node1 || !new_node2)
    return -1;
  
  // extend this region
  dbsol_interp_curve_2d_sptr c1, c2, c3;
  double len3 = create_till_boundary_interpolators(new_edge1, new_node1, c1);
  double len4 = create_till_boundary_interpolators(new_edge2, new_node2, c2);
  double len6 = 100000000;
  
  if (othersedge2) {
    len5 = create_shock_interpolators(othersedge2, shock_c3, new_edge3, new_node3);
    if (len5 < 0)
      return -1;
    len6 = create_till_boundary_interpolators(new_edge3, new_node3, c3);
  }

  if (len3 < 0 || len4 < 0 || len6 < 0)
    return -1;

  double total_length = 0;
  if (len1 < len2) {
    if (len1 < len5) // len1 is the min
      total_length = len1 + len3*0.8f;
    else // len5 is the min
      total_length = len5 + len6*0.8f;
  } else {
    if (len2 < len5) // len2 is the min 
      total_length = len2 + len4*0.8f;
    else // len5 is the min
      total_length = len5 + len6*0.8f;
  }
    
  dbsol_curve_algs::sample_region_along_curve(*shock_c1, region_plus_points_, 0.3f, total_length, (float)coarse_shock_graph_source_->radius(), true);
  if (len1 < total_length)
    dbsol_curve_algs::sample_region_along_curve(*c1, region_plus_points_, 0.3f, total_length - len1, (float)coarse_shock_graph_source_->radius(), true);
  
  dbsol_curve_algs::sample_region_along_curve(*shock_c2, region_minus_points_, 0.3f, total_length, (float)coarse_shock_graph_source_->radius(), true);
  if (len2 < total_length)
    dbsol_curve_algs::sample_region_along_curve(*c2, region_minus_points_, 0.3f, total_length - len2, (float)coarse_shock_graph_source_->radius(), true);

  if (othersedge2) {
    dbsol_curve_algs::sample_region_along_curve(*shock_c3, region_minus_points_, 0.3f, total_length, (float)coarse_shock_graph_source_->radius(), true);
    if (len5 < total_length)
      dbsol_curve_algs::sample_region_along_curve(*c3, region_minus_points_, 0.3f, total_length - len5, (float)coarse_shock_graph_source_->radius(), true);
  }

  if (image_set_ && region_plus_points_.size() != 0 && region_minus_points_.size() != 0) {

    if (grey_image_) {
      double plus_mean = 0.0, minus_mean = 0.0;

      for(unsigned i = 0; i < region_plus_points_.size(); ++i)
      {
        double v = vil_bilin_interp_safe(I_, region_plus_points_[i]->x(), region_plus_points_[i]->y());
        plus_mean += v;
      }
      for(unsigned i = 0; i < region_minus_points_.size(); ++i)
      {
        double v = vil_bilin_interp_safe(I_, region_minus_points_[i]->x(), region_minus_points_[i]->y());
        minus_mean += v;
      }
      plus_mean /= region_plus_points_.size();
      minus_mean /= region_minus_points_.size();
  #if 0
      vcl_cout << "APP plus_mean: (" << plus_mean << ") minus mean: (" << minus_mean << ")\n"; 
  #endif
      return distance_intensity(plus_mean, minus_mean, color_gamma_);  // user should set the gamma accordingly if the image is grey!!
    } else {
      //create two histograms using bilinearly interpolated image values
      vnl_vector_fixed<double, 3> plus_mean((double)0.0f), minus_mean((double)0.0f);

      for(unsigned i = 0; i < region_plus_points_.size(); ++i)
      {
        vnl_vector_fixed<double, 3> v;
        v[0] = vil_bilin_interp_safe(L_, region_plus_points_[i]->x(), region_plus_points_[i]->y());
        v[1] = vil_bilin_interp_safe(A_, region_plus_points_[i]->x(), region_plus_points_[i]->y());
        v[2] = vil_bilin_interp_safe(B_, region_plus_points_[i]->x(), region_plus_points_[i]->y());
        plus_mean += v;
        
      }
      for(unsigned i = 0; i < region_minus_points_.size(); ++i)
      {
        vnl_vector_fixed<double, 3> v;
        v[0] = vil_bilin_interp_safe(L_, region_minus_points_[i]->x(), region_minus_points_[i]->y());
        v[1] = vil_bilin_interp_safe(A_, region_minus_points_[i]->x(), region_minus_points_[i]->y());
        v[2] = vil_bilin_interp_safe(B_, region_minus_points_[i]->x(), region_minus_points_[i]->y());
        minus_mean += v;
      }
      plus_mean /= region_plus_points_.size();
      minus_mean /= region_minus_points_.size();
  #if 0
      vcl_cout << "APP plus_mean: (" << plus_mean << ") minus mean: (" << minus_mean << ")\n"; 
  #endif
      return distance_LAB(plus_mean, minus_mean, color_gamma_);
    }
  }

  return -1;
}

void dbsk2d_shock_transforms::set_image(vil_image_resource_sptr image) { 
  image_ = image; 
  I_ = image_->get_view();
  if( I_.nplanes() != 3 ) {
     grey_image_ = true;
  } else {
    grey_image_ = false;
    convert_RGB_to_Lab(I_, L_, A_, B_);
  }
  image_set_ = true;
}

//: if both costs are available use both of them to decide
//  this old method is used in BMVC07 experiments, but it is redundant.
//  just two thresholds is enough, the function below is the new version
bool dbsk2d_shock_transforms::close_the_gap_old(double thres_contour_low, double thres_contour_high, 
                                            double thres_app_low, double thres_app_high, 
                                            double alpha_contour, double alpha_app, double& cont_cost, double& app_cost)
{

  app_cost = gap_transform_appearance_cost();
  cont_cost = gap_transform_contour_cost();
  if (app_cost > 0 && app_cost < thres_app_low && !(cont_cost > thres_contour_high))
  //if (app_cost > 0 && app_cost < thres_app_low)
    return true;
  if (cont_cost > 0 && cont_cost < thres_contour_low && !(app_cost > thres_app_high))
    return true;
  if (app_cost > thres_app_high)
    return false;
  if (cont_cost > thres_contour_high)
    return false;

  double thres_high = (alpha_contour*thres_contour_high + alpha_app*thres_app_high)/(alpha_contour + alpha_app);
  if (app_cost > 0) 
    if (cont_cost > 0) {
      if ((alpha_contour*cont_cost + alpha_app*app_cost)/(alpha_contour + alpha_app) > thres_high)
        return false;
      else 
        return true;
    }
  
  return false;    
}

//: if both costs are available use both of them to decide  
bool dbsk2d_shock_transforms::close_the_gap(double thres_contour, 
                                            double thres_app, 
                                            double alpha_contour, double alpha_app, double& cont_cost, double& app_cost)
{
  app_cost = gap_transform_appearance_cost();
  cont_cost = gap_transform_contour_cost();

  if (app_cost > 0 && cont_cost > 0) {
    double thres = alpha_contour*thres_contour + alpha_app*thres_app;
    if (alpha_contour*cont_cost + alpha_app*app_cost <= thres)
      return true;
  }
  
  return false;    
}





