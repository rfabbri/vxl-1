// This is brcv/shp/dbsk2d/dbsk2d_boundary.cxx

//:
// \file

#include "dbsk2d_boundary.h"

#include <vcl_algorithm.h>

#include <vgl/vgl_line_2d.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbsol/dbsol_interp_curve_2d.h>

#include <dbgl/algo/dbgl_circ_arc.h>

#include <vtol/vtol_list_functions.h>

#include <dbsk2d/dbsk2d_defines.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_bnd_vertex.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>

#include <dbgl/algo/dbgl_closest_point.h>

// -----------------------------------------------------------
//: Constructor
dbsk2d_boundary::
dbsk2d_boundary ():
  _nextAvailableID(0),
  ishock_graph_(0),
  shock_graph_(0),
  cell_h_(-1),
  cell_w_(-1),
  xmin_(0),
  ymin_(0)
{  
  // make sure contour list and vertex list are empty
  this->preproc_contours_.clear();
  this->scratch_contours_.clear();
  this->belm_list_.clear();
}


// -----------------------------------------------------------
//: Destructor
dbsk2d_boundary::
~dbsk2d_boundary ()
{
}


// -----------------------------------------------------------
//: Return list of all contours, including preprocessed and scratch
void dbsk2d_boundary::
all_contours(bnd_contour_list& all_contours) const
{
  bnd_contour_list scratch = this->scratch_contours_;
  all_contours = this->preproc_contours_;
  all_contours.splice(all_contours.end(), scratch);
  return;
}



// -----------------------------------------------------------------------
//: compute bounding box
void dbsk2d_boundary::
compute_bounding_box() const
{
  // we need to clear the bounds of the box to correctly reflect edge bounds
  this->empty_bounding_box();

  bnd_contour_list all_contours;
  this->all_contours(all_contours);
  for (bnd_contour_list::iterator cit =
    all_contours.begin(); cit != all_contours.end(); ++cit)
  {
    if (!(*cit)->get_bounding_box())
    {
      vcl_cerr << "In dbsk2d_boundary::compute_bounding_box() -"
               << " contour has null bounding box\n";
      continue;
    }
    this->add_to_bounding_box((*cit)->get_bounding_box());
  }
}



//******************************************************
// Cell related functions
//******************************************************

//------------------------------------------------------------------------
//: Set parameters of cell partionting,
void dbsk2d_boundary::
set_partition_params(double xmin, double ymin,
                     int num_rows, int num_cols, 
                     double cell_height, double cell_width)
{
  this->xmin_ = xmin;
  this->ymin_ = ymin;
  this->cell_h_= cell_height;
  this->cell_w_= cell_width;
  this->cells_.resize(num_rows, num_cols);
  for (int i=0; i<num_rows; ++i)
  {
    for (int j=0; j<num_cols; ++j)
    {
      double cell_xmin = xmin + j*cell_width;
      double cell_xmax = cell_xmin + cell_width;
      double cell_ymin = ymin + i*cell_height;
      double cell_ymax = cell_ymin + cell_height;
      this->cells_(i,j) = 
        new dbsk2d_bnd_cell(dbsk2d_bnd_cell_index(i, j), 
        vgl_box_2d<double >(cell_xmin, cell_xmax, cell_ymin, cell_ymax));
    }
  }
}




//------------------------------------------------------------------------
//: Partition the boundary, i.e. putting boundary elements (edges) into cells
// `epsilon' is for fuzzy comparison
// Require: run set_partition_params(...) before this command
void dbsk2d_boundary::
partition_into_cells(bool tight, bool talkative, double tol)
{
  // only partition when partitioning parameters have been set
  if (this->num_cells()==0 || this->cell_w()<=0 || this->cell_h()<=0)
  {
    vcl_cerr << "Error: Partitioning parameters has not been set.\n";
    return;
  }
  // go through list of edges and put them in appropriate box(es).
  bnd_edge_list all_edges;
  dbsk2d_bnd_utils::extract_edge_list(this, all_edges);
  
  if (talkative)
    vcl_cout << "\nPatitioning boundary...\nNumber of edges= " << all_edges.size() << vcl_endl;

  // scan through all edges and put them in appropriate cells
  for (bnd_edge_list::iterator eit = all_edges.begin(); eit != all_edges.end();
    ++eit)
  {
    dbsk2d_bnd_edge_sptr e = *eit;

    // special treatment for line to get tight partitioning
    if (tight && (!e->is_a_point()) && (e->left_bcurve()->is_a_line()))
    {
      this->insert_line_into_cells_tight(e, tol);
      continue;
    }
    else
    {
      // for points and arc, use generic algorithm
      // generic algo DOES give tight partitioning for points
      this->insert_edge_into_cells_using_bounding_box(*eit, tol);
    }
  }
}



//------------------------------------------------------------------------
//: Insert an edge into cells using bounding box intersection
void dbsk2d_boundary::
insert_edge_into_cells_using_bounding_box(const dbsk2d_bnd_edge_sptr& e, 
                                          double tol)
{  
  vsol_box_2d_sptr box = e->get_bounding_box();
  vgl_box_2d<double > bbox;
  bbox.add(vgl_point_2d<double>(box->get_min_x(), box->get_min_y()));
  bbox.add(vgl_point_2d<double>(box->get_max_x(), box->get_max_y()));
  
  // list of cells this edge belongs to
  vcl_vector<dbsk2d_bnd_cell_sptr > cells = 
    this->compute_cell_membership_of_bbox(bbox, tol);

  for (unsigned i =0; i < cells.size(); ++i)
  {
    cells[i]->add_bnd_edge(e);
  }
}




// -----------------------------------------------------------------------
//: Insert an edge into cells it INTERSECTS with (fuzzily).
void dbsk2d_boundary::
insert_line_into_cells_tight(const dbsk2d_bnd_edge_sptr& e, double tol)
{
  dbsk2d_assert(e->left_bcurve()->is_a_line());


  // DEBUG //////////////
  int id = e->left_bcurve()->id();
  /////////////////////////////////////

  vgl_point_2d<double > p1 = e->point1();
  vgl_point_2d<double > p2 = e->point2();

  // range check
  vgl_box_2d<double > box = this->cell_region();
  if (!(box.contains(p1) && box.contains(p2)))
  {
    vcl_cerr << "In dbsk2d_boundary::insert_line_into_cells_tight().\n" << 
        "Error: line outside partitioned area\n";
    return;
  }

  // construct a rectangle whose sides are `tol' away from the linesegment

  //  line1_p1                                line1_p2
  //    o--------------------------------------o
  //     p1 o------------------------------o p2
  //    o--------------------------------------o
  //  line2_p1                                line2_p2

  vgl_line_segment_2d<double > lineseg(p1, p2);
  vgl_vector_2d<double > n = lineseg.normal();
  vgl_vector_2d<double > t = lineseg.direction();

  vgl_point_2d<double > line1_p1 = p1 + tol*n - tol * t;
  vgl_point_2d<double > line1_p2 = p2 + tol*n + tol * t;

  vgl_point_2d<double > line2_p1 = p1 - tol*n - tol * t;
  vgl_point_2d<double > line2_p2 = p2 - tol*n + tol * t;

  // compute cells that intersect with the constructed rectangle
  // assumming tol << cell_h, cell_w, we only need to compute intersection 
  // the two long linesegments with the cells
  vcl_list<dbsk2d_bnd_cell_sptr > cells_line1;
  vcl_list<dbsk2d_bnd_cell_sptr > cells_line2;

  this->compute_cell_membership_of_line(line1_p1, line1_p2, cells_line1);
  this->compute_cell_membership_of_line(line2_p1, line2_p2, cells_line2);

  // combine the two cell lists. This is also the list of cells the original
  // linesegment intersects with
  cells_line1.splice(cells_line1.end(), cells_line2);
  cells_line1.sort();
  cells_line1.unique();

  // put the edge into the computed cells
  for (vcl_list<dbsk2d_bnd_cell_sptr >::iterator cit = 
    cells_line1.begin(); cit != cells_line1.end(); ++cit)
  {
    (*cit)->add_bnd_edge(e);
  }
}


//: Compute list of cells that a point belongs to (fuzzily)
vcl_vector<dbsk2d_bnd_cell_sptr > dbsk2d_boundary::
compute_cell_membership_of_point(const vgl_point_2d<double >& pt, double tol) const
{
  vcl_vector<dbsk2d_bnd_cell_sptr > cell_list;
  cell_list.clear();

  // compute range of boxes this edge belongs to.
  int min_col = static_cast<int>(vcl_floor(
    (pt.x()-tol-this->xmin()) / this->cell_w()));
  int max_col = static_cast<int>(vcl_floor(
    (pt.x()+tol-this->xmin()) / this->cell_w()));
  int min_row = static_cast<int>(vcl_floor(
    (pt.y()-tol-this->ymin()) / this->cell_h()));
  int max_row = static_cast<int>(vcl_floor(
    (pt.y()+tol-this->ymin()) / this->cell_h()));

  for (int i=min_row; i<=max_row; ++i)
  {
    for (int j=min_col; j<=max_col; ++j)
    {
      cell_list.push_back(this->cell(i, j));
    }
  }
  return cell_list;
}



// -----------------------------------------------------------------------
//: Compute list of cells that a line segment intersect with (no fuzzy)
void dbsk2d_boundary::
compute_cell_membership_of_line(const vgl_point_2d<double >& p1, 
                                const vgl_point_2d<double >& p2, 
                                vcl_list<dbsk2d_bnd_cell_sptr >& ret_cells)
                                const
{
  ret_cells.clear();
  
  // form a list of intersection points
  vcl_list<double > intersections;
  this->intersect_line_with_cell_grids(p1, p2, intersections);
  intersections.push_back(1);

  // Now compute the midpoints of the disjoint line segments
  vcl_vector<vgl_point_2d<double > > midpoints;

  vgl_point_2d<double > prev_pt = p1;
  for (vcl_list<double >::iterator it=intersections.begin(); 
    it != intersections.end(); ++it)
  {
    vgl_point_2d<double > cur_pt = midpoint<double >(p1, p2, *it);
    midpoints.push_back(centre<double >(prev_pt, cur_pt));
    prev_pt = cur_pt;
  }

  // Compute cells that contain the midpoints. They are also the cells that
  // the linesegment intersects with
  for (unsigned int i=0; i<midpoints.size(); ++i)
  {
    int col = (int)vcl_floor((midpoints[i].x()-this->xmin()) / this->cell_w());
    int row = (int)vcl_floor((midpoints[i].y()-this->ymin()) / this->cell_h());
    // ignore if `col' and `row' are out of range
    if (col<0 || col >=this->num_cols() || row <0 || row >= this->num_rows())
      continue;

    ret_cells.push_back(this->cell(row, col));
  }
}


//: COmpute list of cells that a bounding box intersect with (no fuzzy)
vcl_vector<dbsk2d_bnd_cell_sptr > dbsk2d_boundary::
compute_cell_membership_of_bbox(const vgl_box_2d<double >& bbox, double tol) const
{
  vcl_vector<dbsk2d_bnd_cell_sptr > ret_cells;

  // display an error when edge is outside partitioned area
  bool inside_partition_area = (bbox.min_x() >= this->xmin() &&
    bbox.max_x() <= (this->xmin()+this->width()) &&
    bbox.min_y() >= this->ymin() &&
    bbox.max_y() <= (this->ymin()+this->height()) );
  if (!inside_partition_area)
  {
    vcl_cerr << "In dbsk2d_boundary::compute_cell_membership_of_bbox().\n" 
      << "Error: vsol_box_2d outside partitioned area\n";
    return ret_cells;
  }

  vgl_box_2d<double > box(bbox);
  box.set_min_x(box.min_x() - tol);
  box.set_max_x(box.max_x() + tol);
  box.set_min_y(box.min_y() - tol);
  box.set_max_y(box.max_y() + tol);

  // compute range of boxes this edge belongs to.
  int min_col = static_cast<int>(vcl_floor(
    (box.min_x()-this->xmin()) / this->cell_w()));
  int max_col = static_cast<int>(vcl_floor(
    (box.max_x()-this->xmin()) / this->cell_w()));
  int min_row = static_cast<int>(vcl_floor(
    (box.min_y()-this->ymin()) / this->cell_h()));
  int max_row = static_cast<int>(vcl_floor(
    (box.max_y()-this->ymin()) / this->cell_h()));

  // just in case the edge is outside partitioned area
  min_col = vnl_math_max(0,min_col);
  max_col = vnl_math_min(max_col, this->num_cols()-1);
  min_row = vnl_math_max(0, min_row);
  max_row = vnl_math_min(max_row, this->num_rows()-1);

  for (int i=min_row; i<=max_row; ++i)
  {
    for (int j=min_col; j<=max_col; ++j)
    {
      ret_cells.push_back(this->cell(i, j));
    }
  }

  return ret_cells;
}

// -----------------------------------------------------------------------
//: intersect a line with `this' boundary's grid lines
// Return a sorted list of parameters of intersections points
void dbsk2d_boundary::
intersect_line_with_cell_grids( const vgl_point_2d<double >& p1,
                               const vgl_point_2d<double >& p2, 
 vcl_list<double >& intersections) const
{
  // clear old stuffs
  intersections.clear();

  // compute range of rows and columns this line cuts through
  vgl_box_2d<double > box(p1, p2);

  int start_col = (int)(vcl_ceil((box.min_x()-this->xmin())/this->cell_w()));
  int end_col = (int)(vcl_ceil((box.max_x()-this->xmin())/this->cell_w()))-1;

  int start_row = (int)(vcl_ceil((box.min_y()-this->ymin())/this->cell_h()));
  int end_row = (int)(vcl_ceil((box.max_y()-this->ymin())/this->cell_h()))-1;


  for (int m=start_col; m<= end_col; ++m)
  {
    // That we are here means the linesegment is not a vertical line
    intersections.push_back(vnl_math_abs(
      this->xmin()+m*this->cell_w()-p1.x()) / box.width());
  }

  for (int m=start_row; m<= end_row; ++m)
  {
    // That we are here means the linesegment is not a horizontal line
    intersections.push_back(vnl_math_abs(
      this->ymin()+m*this->cell_h()-p1.y()) / box.height());
  }
  intersections.sort();
  return;
}



// ----------------------------------------------------------------------------
//: intersect a circular arc with `this' boundary's grid lines
// Return a sorted list of parameters of intersections points
void dbsk2d_boundary::
intersect_arc_with_cell_grids(const vgl_point_2d<double >& arc_p1, 
  const vgl_point_2d<double >& arc_p2, double arc_k,
  vcl_list<double >& intersections) const
{
  dbgl_circ_arc arc(arc_p1, arc_p2, arc_k);
  double h = arc.height();

  // compute an over-estimated bounding box of this arc
  vgl_box_2d<double > bbox;
  bbox.add(arc.start());
  bbox.add(arc.end());

  // compute the cells this bbox belongs to, which are potential containers of the arc
  vcl_vector<dbsk2d_bnd_cell_sptr > cells = 
    this->compute_cell_membership_of_bbox(bbox, h);

  // for each cell, check whether the arc ACTUALLY intersects it.
  // This is not the most effecient way to do it but it is an easy and safe way to do it
  for (unsigned i =0; i < cells.size(); ++i)
  {
    dbsk2d_bnd_cell_sptr cell = cells[i];
    // intersect each side of the cell with the arc
    vgl_box_2d<double > box = cell->box();

    // the four corners of the box
    vgl_point_2d<double > p1(box.min_x(), box.min_y());
    vgl_point_2d<double > p2(box.max_x(), box.min_y());
    vgl_point_2d<double > p3(box.max_x(), box.max_y());
    vgl_point_2d<double > p4(box.min_x(), box.max_y());

    vcl_vector<vgl_line_segment_2d<double > >line_list;
    line_list.push_back(vgl_line_segment_2d<double >(p1, p2));
    line_list.push_back(vgl_line_segment_2d<double >(p2, p3));
    line_list.push_back(vgl_line_segment_2d<double >(p3, p4));
    line_list.push_back(vgl_line_segment_2d<double >(p4, p1));

    for (unsigned i =0; i < line_list.size(); ++i)
    {
      vcl_vector<double > line_ratios;
      vcl_vector<double > arc_ratios;
      double d = dbgl_closest_point::lineseg_to_circular_arc(line_list[i], arc,
        line_ratios, arc_ratios);

      if (d == 0) // INTERSECTION !!!!
      {
        for (unsigned j =0; j < arc_ratios.size(); ++j)
        {
          intersections.push_back(arc_ratios[j]);
        }
      }
    }
  }
  intersections.sort();
  intersections.unique();
  return;
}





//------------------------------------------------------------------------
//: Break long lines into shorter segments such that each segment does not
// extend beyond one cell (fuzzily)
// Required: edges have been put in the cells (use partition_into_cells())
void dbsk2d_boundary::
break_long_line_edges(double tol)
{
  // collect all the edges of `this' boundary
  bnd_edge_list all_edges;
  dbsk2d_bnd_utils::extract_edge_list(this, all_edges);
  for (bnd_edge_list::iterator eit = all_edges.begin(); 
    eit != all_edges.end(); ++eit)
  {
    dbsk2d_bnd_edge_sptr e = *eit;

    // we only deal with lines for now
    if (e->is_a_point()) continue;

    if (!e->left_bcurve()->is_a_line()) continue;

    // no work to do with edges that belong to ONLY one cell
    if (e->cells().size() ==1) continue;

    // DEBUG //////////////////////////////////
    int id_left = e->left_bcurve()->id();
    ////////////////////////////////////////////

    // find intersection points with grid lines
    vcl_list<double > intersections;

    this->intersect_line_with_cell_grids(e->point1(), e->point2(),
      intersections);
    double len = (e->len());

    

    // remove intersection points that are too close to each other
    double prev_ratio = 0;
    vcl_list<double >::iterator it = intersections.begin();
    while (it != intersections.end())
    {
      double cur_ratio = *it;
      // check length of line segment
      if ((cur_ratio-prev_ratio)*len <= tol)
      {
        it = intersections.erase(it);
        continue;
      }
      else
      {
        prev_ratio = cur_ratio;
        ++it;
      }
    }

    // the last intersection point
    if ((1-prev_ratio)*len <= tol && !intersections.empty())
    {
      intersections.pop_back();
    }



    // replace `e' by a vector of new edges

    // vector of line edges that will be used to replace `e'
    vcl_vector<dbsk2d_bnd_edge_sptr > new_edges;
      
    // list of vertices along `e'    
    bnd_vertex_vector vertices_along_edge;
    for (vcl_list<double >::iterator it = intersections.begin();
      it != intersections.end(); ++it)
    {
      vgl_point_2d<double > p1 = e->point1();
      vgl_point_2d<double > p2 = e->point2();
      vertices_along_edge.push_back(dbsk2d_bnd_utils::new_vertex(
        midpoint<double >(p1, p2, *it), this));
    }



    // list of bpoints (including duplicated ones)
    vcl_vector<dbsk2d_ishock_bpoint* > bpoint_list;

    // if the bbox spans over two cells, that means the point is close to a boundary
    if (this->compute_cell_membership_of_point(e->point1(), tol).size() > 1)
    {
      dbsk2d_bnd_vertex_sptr v = e->bnd_v1();

      // clone the bpoint inside vertex
      dbsk2d_ishock_bpoint* old_bp = v->bpoint();
      
      dbsk2d_ishock_bpoint* new_bp1 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp1->set_bnd_vertex(v.ptr());

      dbsk2d_ishock_bpoint* new_bp2 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp2->set_bnd_vertex(v.ptr());

      // use one bpoint for the vertex and one for the line on this side
      v->set_bpoint(new_bp1);
      bpoint_list.push_back(new_bp2);
    }
    else
    {
      bpoint_list.push_back(e->bnd_v1()->bpoint());
    }

    // form 2 duplicates of each bpoint inside bnd_vertices 
    // at line intersections with grid lines
    for (bnd_vertex_vector::iterator vit = vertices_along_edge.begin();
      vit != vertices_along_edge.end(); ++vit)
    {
      // clone the bpoint inside vertex
      dbsk2d_ishock_bpoint* old_bp = (*vit)->bpoint();
      
      dbsk2d_ishock_bpoint* new_bp1 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp1->set_bnd_vertex((*vit).ptr());

      dbsk2d_ishock_bpoint* new_bp2 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp2->set_bnd_vertex((*vit).ptr());

      // add to the list
      bpoint_list.push_back(new_bp1);
      bpoint_list.push_back(new_bp2);
    }

    // Determine whether new end-bpoint need to be duplicated 

    // if the bbox spans over two cells, that means the point is close to a boundary
    if (this->compute_cell_membership_of_point(e->point2()).size() > 1)
    {
      dbsk2d_bnd_vertex_sptr v = e->bnd_v2();

      // clone the bpoint inside vertex
      dbsk2d_ishock_bpoint* old_bp = v->bpoint();
      
      dbsk2d_ishock_bpoint* new_bp1 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp1->set_bnd_vertex(v.ptr());

      dbsk2d_ishock_bpoint* new_bp2 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp2->set_bnd_vertex(v.ptr());

      // use one bpoint for the vertex and one for the line on this side
      v->set_bpoint(new_bp1);
      bpoint_list.push_back(new_bp2);
    }
    else
    {
      bpoint_list.push_back(e->bnd_v2()->bpoint());
    }
    
    vertices_along_edge.push_back(e->bnd_v2());

    // direction of new edges
    vcl_vector<signed char > directions;
    
    // form a vector of arc edges equivalent to the old edge
    dbsk2d_bnd_vertex_sptr bnd_v1 = e->bnd_v1();
    for (unsigned int i=0; i < vertices_along_edge.size(); ++i)
    {
      dbsk2d_bnd_vertex_sptr bnd_v2 = vertices_along_edge[i];

      // get hold of the two bpoints of the line
      dbsk2d_ishock_bpoint* bp1 = bpoint_list[2*i];
      dbsk2d_ishock_bpoint* bp2 = bpoint_list[2*i+1];

      // create left and right blines
      dbsk2d_ishock_bline* left = new dbsk2d_ishock_bline(bp1, bp2, 
        this->nextAvailableID(), true);
      dbsk2d_ishock_bline* right = new dbsk2d_ishock_bline(bp2, bp1,
        this->nextAvailableID(), false);
      left->set_twinLine(right);

      // form a new line edge
     
      new_edges.push_back(new dbsk2d_bnd_edge(bnd_v1, bnd_v2, left, right, 
        this->nextAvailableID()));

      directions.push_back(1);
      bnd_v1 = bnd_v2;
    }

    // At intersection with the grid lines, the bpoint is duplicated so that
    // shock computation is easier (for cell scheme)

    // replace `e' with the new vector of edges
    // containers for new_edges with reverse order and reverse directions
    // in case they are needed.
    vcl_vector<dbsk2d_bnd_edge_sptr > reverse_new_edges;
    vcl_vector<signed char > reverse_directions;

    // replace `cur_edge' with new edges, topologically
    while (e->numsup()>0)
    {
      vtol_topology_object* t = e->superiors_list()->front();
      dbsk2d_bnd_contour_sptr contour = static_cast<dbsk2d_bnd_contour* >(t);
      signed char dir = contour->direction(*e);
      // if direction of `cur_edge' in `contour' is -1 then we
      // need to reverse the order of new edges and sign of new directions
      if (dir == 1)
      {
        contour->replace_edges(new_edges, directions, e);  
      }
      else
      {
        // only reverse copy from `new_edges' to `reverse_new_edges' once
        if (reverse_new_edges.size() == 0)
        {
          reverse_new_edges.reserve(new_edges.size());
          vcl_reverse_copy(new_edges.begin(), new_edges.end(), 
            reverse_new_edges.begin());
          reverse_directions.assign(directions.size(), -1);
        }

        // replace the edge with new_edges whose orders are reversed
        contour->replace_edges(reverse_new_edges, reverse_directions, e);
      }
    }
    // unlink `e' from its end vertices
    e->unlink();

    // We shall use midpoints to determine which cell a linesegment belongs to 

    // replace `cur_edge' with new edges, geographically
    e->unlink_from_cells();

    for (unsigned int i=0; i<new_edges.size(); ++i)
    {
      dbsk2d_bnd_edge_sptr new_e = new_edges[i];
      vgl_point_2d<double > p = centre<double >(new_e->point1(), new_e->point2());
      
      // compute range of boxes this edge belongs to.
      int min_col = static_cast<int>(vcl_floor(
        (p.x()-tol-this->xmin()) / this->cell_w()));
      int max_col = static_cast<int>(vcl_floor(
        (p.x()+tol-this->xmin()) / this->cell_w()));
      int min_row = static_cast<int>(vcl_floor(
        (p.y()-tol-this->ymin()) / this->cell_h()));
      int max_row = static_cast<int>(vcl_floor(
        (p.y()+tol-this->ymin()) / this->cell_h()));

      for (int i=min_row; i<=max_row; ++i)
      {
        for (int j=min_col; j<=max_col; ++j)
        {
          this->cell(i, j)->add_bnd_edge(new_e);
        }
      }
    }

    
  }
  
  return;
}









//: Break long arcs into shorter segments such that each segment does not
// extend beyond one cell (fuzzily)
// Required: edges have been put in the cells (use partition_into_cells())
void dbsk2d_boundary::
break_long_arc_edges(double tol)
{
  // collect all the edges of `this' boundary
  bnd_edge_list all_edges;
  dbsk2d_bnd_utils::extract_edge_list(this, all_edges);
  for (bnd_edge_list::iterator eit = all_edges.begin(); 
    eit != all_edges.end(); ++eit)
  {
    dbsk2d_bnd_edge_sptr e = *eit;

    // we only deal with lines for now
    if (e->is_a_point()) continue;

    if (!e->left_bcurve()->is_an_arc()) continue;

    // no work to do with edges that belong to ONLY one cell
    if (e->cells().size() ==1) continue;

    // DEBUG //////////////////////////////////
    int id_left = e->left_bcurve()->id();
    ////////////////////////////////////////////

    // find intersection points with grid lines
    
    dbsk2d_ishock_barc* left_barc = static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
    dbsk2d_ishock_barc* right_barc = static_cast<dbsk2d_ishock_barc* >(e->right_bcurve());

    dbgl_circ_arc arc(left_barc->start(), left_barc->end(), left_barc->curvature());


    // intersection locations
    vcl_list<double > intersections;
    this->intersect_arc_with_cell_grids(arc.start(), arc.end(), arc.k(), intersections);

    // length of the segments
    double len = (e->len());

    // remove intersection points that are too close to each other
    double prev_ratio = 0;
    vcl_list<double >::iterator it = intersections.begin();
    while (it != intersections.end())
    {
      double cur_ratio = *it;
      // check length of line segment
      if ((cur_ratio-prev_ratio)*len <= tol)
      {
        it = intersections.erase(it);
        continue;
      }
      else
      {
        prev_ratio = cur_ratio;
        ++it;
      }
    }

    // the last intersection point
    if ((1-prev_ratio)*len <= tol && !intersections.empty())
    {
      intersections.pop_back();
    }

    // replace `e' by a vector of new edges
    
    // vector of line edges that will be used to replace `e'
    vcl_vector<dbsk2d_bnd_edge_sptr > new_edges;
      
    // list of vertices along `e'    
    bnd_vertex_vector vertices_along_edge;
    for (vcl_list<double >::iterator it = intersections.begin();
      it != intersections.end(); ++it)
    {
      vertices_along_edge.push_back(dbsk2d_bnd_utils::new_vertex(
        arc.point_at(*it), this));
    }

    // list of bpoints (including duplicated ones)
    vcl_vector<dbsk2d_ishock_bpoint* > bpoint_list;

    // if the bbox spans over two cells, that means the point is close to a boundary
    if (this->compute_cell_membership_of_point(e->point1(), tol).size() > 1)
    {
      dbsk2d_bnd_vertex_sptr v = e->bnd_v1();

      // clone the bpoint inside vertex
      dbsk2d_ishock_bpoint* old_bp = v->bpoint();
      
      dbsk2d_ishock_bpoint* new_bp1 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp1->set_bnd_vertex(v.ptr());

      dbsk2d_ishock_bpoint* new_bp2 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp2->set_bnd_vertex(v.ptr());

      // use one bpoint for the vertex and one for the line on this side
      v->set_bpoint(new_bp1);
      bpoint_list.push_back(new_bp2);
    }
    else
    {
      bpoint_list.push_back(e->bnd_v1()->bpoint());
    }

    // form 2 duplicates of each bpoint inside bnd_vertices 
    // at line intersections with grid lines
    for (bnd_vertex_vector::iterator vit = vertices_along_edge.begin();
      vit != vertices_along_edge.end(); ++vit)
    {
      dbsk2d_bnd_vertex_sptr v = (*vit);
      // clone the bpoint inside vertex
      dbsk2d_ishock_bpoint* old_bp = v->bpoint();
      
      dbsk2d_ishock_bpoint* new_bp1 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp1->set_bnd_vertex(v.ptr());

      dbsk2d_ishock_bpoint* new_bp2 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp2->set_bnd_vertex(v.ptr());

      // add to the list
      bpoint_list.push_back(new_bp1);
      bpoint_list.push_back(new_bp2);
    }

    // Determine whether new end-bpoint need to be duplicated 

    // if the bbox spans over two cells, that means the point is close to a boundary
    if (this->compute_cell_membership_of_point(e->point2(), tol).size() > 1)
    {
      dbsk2d_bnd_vertex_sptr v = e->bnd_v2();

      // clone the bpoint inside vertex
      dbsk2d_ishock_bpoint* old_bp = v->bpoint();
      
      dbsk2d_ishock_bpoint* new_bp1 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp1->set_bnd_vertex(v.ptr());

      dbsk2d_ishock_bpoint* new_bp2 = 
        new dbsk2d_ishock_bpoint(old_bp, this->nextAvailableID());
      new_bp2->set_bnd_vertex(v.ptr());

      // use one bpoint for the vertex and one for the line on this side
      v->set_bpoint(new_bp1);
      bpoint_list.push_back(new_bp2);
    }
    else
    {
      bpoint_list.push_back(e->bnd_v2()->bpoint());
    }
    vertices_along_edge.push_back(e->bnd_v2());

    // direction of new edges
    vcl_vector<signed char > directions;
    
    // form a vector of line edges equivalent to the old edge
    dbsk2d_bnd_vertex_sptr bnd_v1 = e->bnd_v1();
    for (unsigned int i=0; i < vertices_along_edge.size(); ++i)
    {
      dbsk2d_bnd_vertex_sptr bnd_v2 = vertices_along_edge[i];

      // get hold of the two bpoints of the line
      dbsk2d_ishock_bpoint* bp1 = bpoint_list[2*i];
      dbsk2d_ishock_bpoint* bp2 = bpoint_list[2*i+1];

      // create left and right blines
      dbsk2d_ishock_barc* left = new dbsk2d_ishock_barc(bp1, bp2, 
        this->nextAvailableID(), true, left_barc->center(), 
        left_barc->R(), left_barc->nud());
      dbsk2d_ishock_barc* right = new dbsk2d_ishock_barc(bp2, bp1,
        this->nextAvailableID(), false, right_barc->center(), 
        right_barc->R(), right_barc->nud());
      left->set_twinArc(right);

      // form a new line edge
     
      new_edges.push_back(new dbsk2d_bnd_edge(bnd_v1, bnd_v2, left, right, 
        this->nextAvailableID()));

      directions.push_back(1);
      bnd_v1 = bnd_v2;
    }

    // At intersection with the grid lines, the bpoint is duplicated so that
    // shock computation is easier (for cell scheme)

    // replace `e' with the new vector of edges
    // containers for new_edges with reverse order and reverse directions
    // in case they are needed.
    vcl_vector<dbsk2d_bnd_edge_sptr > reverse_new_edges;
    vcl_vector<signed char > reverse_directions;

    // replace `cur_edge' with new edges, topologically
    while (e->numsup()>0)
    {
      vtol_topology_object* t = e->superiors_list()->front();
      dbsk2d_bnd_contour_sptr contour = static_cast<dbsk2d_bnd_contour* >(t);
      signed char dir = contour->direction(*e);
      // if direction of `cur_edge' in `contour' is -1 then we
      // need to reverse the order of new edges and sign of new directions
      if (dir == 1)
      {
        contour->replace_edges(new_edges, directions, e);  
      }
      else
      {
        // only reverse copy from `new_edges' to `reverse_new_edges' once
        if (reverse_new_edges.size() == 0)
        {
          reverse_new_edges.reserve(new_edges.size());
          vcl_reverse_copy(new_edges.begin(), new_edges.end(), 
            reverse_new_edges.begin());
          reverse_directions.assign(directions.size(), -1);
        }

        // replace the edge with new_edges whose orders are reversed
        contour->replace_edges(reverse_new_edges, reverse_directions, e);
      }
    }
    // unlink `e' from its end vertices
    e->unlink();

    // We shall use midpoints to determine which cell a linesegment belongs to 

    // replace `cur_edge' with new edges, geographically
    e->unlink_from_cells();

    for (unsigned int i=0; i<new_edges.size(); ++i)
    {
      dbsk2d_bnd_edge_sptr new_e = new_edges[i];
      vgl_point_2d<double > p = centre<double >(new_e->point1(), new_e->point2());
      vcl_vector<dbsk2d_bnd_cell_sptr > cells = 
        this->compute_cell_membership_of_point(p, tol);
      for (unsigned i =0; i < cells.size(); ++i)
      {
        cells[i]->add_bnd_edge(new_e);
      }
    }
  }
  
  return;
}












//------------------------------------------------------------------------
//: Remove all edges from the cells
void dbsk2d_boundary::
clear_cells()
{
  for (int m=0; m < this->num_rows(); ++m)
  {
    for (int n=0; n<this->num_cols(); ++n)
    {
      this->cell(m, n)->clear_all();
    }
  }
}






// -----------------------------------------------------------------------
//: Update the list of dbsk2d_ishock_belm
// This list is created by collecting data from
// preproc_contours and scratch_contours
void dbsk2d_boundary::update_belm_list() const
{
  this->belm_list_.clear();
  
  // Collect the vertices and edges in the all contours

  //merge preproc_contour_list and scratch_contour_list into one vector
  bnd_contour_list all_contours;
  this->all_contours(all_contours);

  // now collect the vertices and edges in all the contours and 

  //bnd_vertex_list all_vertices;

  dbsk2d_bnd_utils::extract_edge_list(all_contours, this->all_edges_);
  dbsk2d_bnd_utils::extract_belm_list(this->all_edges_, this->belm_list_);

  return;
  
}


// -----------------------------------------------------------------------
//: Update the list of dbsk2d_ishock_belm
// This list is created by collecting data from
// preproc_contours and scratch_contours
void dbsk2d_boundary::update_belm_list(
    dbsk2d_bnd_contour_sptr& new_contour,
    vcl_vector<dbsk2d_ishock_belm*>& ret_belms) 
{

    // Add new contour to pre-processes list
    this->add_a_preproc_contour(new_contour);
  
    // Do not add start and end point
    dbsk2d_bnd_vertex_sptr startpt = new_contour->bnd_vertex(0);
    dbsk2d_bnd_vertex_sptr endpt = new_contour->bnd_vertex(
        new_contour->num_edges());

    vcl_map<unsigned int,vcl_string> local_count;
    local_count[startpt->get_id()]="temp";
    local_count[endpt->get_id()]="temp";

    vcl_vector<dbsk2d_ishock_belm*> local_gap;

    //Now convert to edges
    for ( unsigned int c=0; c < new_contour->num_edges() ; ++c)
    {
        dbsk2d_bnd_edge_sptr edge=new_contour->bnd_edge(c);
        this->all_edges_.push_back(new_contour->bnd_edge(c));

        this->belm_list_.push_back(edge->left_bcurve());
        ret_belms.push_back(edge->left_bcurve());
        local_gap.push_back(edge->left_bcurve());

        this->belm_list_.push_back(edge->right_bcurve());
        ret_belms.push_back(edge->right_bcurve());
        local_gap.push_back(edge->right_bcurve());

        if ( local_count.count(edge->bnd_v1()->get_id()) == 0 )
        {
            this->belm_list_.push_back(edge->left_bcurve()->s_pt());
            local_count[edge->bnd_v1()->get_id()]="temp";
        }

        if ( local_count.count(edge->bnd_v2()->get_id()) == 0 )
        {
            this->belm_list_.push_back(edge->left_bcurve()->e_pt());
            local_count[edge->bnd_v2()->get_id()]="temp";
        }
    }

    gaps_.push_back(local_gap);
    return;
  
}




// -----------------------------------------------------------------------
//: Add a stand-alone point, return smart ptr to the new dbsk2d_bnd_contour
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_a_point(const vgl_point_2d<double >& pt, bool preproc_needed)
{
    
  dbsk2d_bnd_edge_sptr edge = 
    dbsk2d_bnd_utils::new_stand_alone_point(pt, this);

  // construct a contour from degenerate edge
  vcl_vector<dbsk2d_bnd_edge_sptr > bnd_edges(1, edge);
  dbsk2d_bnd_contour_sptr to_return = 
    new dbsk2d_bnd_contour(bnd_edges, this->nextAvailableID());

  this->add_a_bnd_contour(to_return, preproc_needed);
  return to_return;
}

// -----------------------------------------------------------------------
//: Add a stand-alone point, return smart ptr to the new dbsk2d_bnd_contour
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_a_point(const vsol_point_2d_sptr & point,
            bool preproc_needed)
{
  return this->add_a_point(point->get_p(), preproc_needed);
}



// -----------------------------------------------------------------------
//: Add a line segment, return smart ptr to the new the dbsk2d_bnd_contour
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_a_line(const vgl_point_2d<double >& p1, const vgl_point_2d<double >& p2, 
           bool preproc_needed)
{  
  // put end points of `line' into a list
  vcl_vector<vgl_point_2d<double > > pts;
  pts.push_back(p1);
  pts.push_back(p2);
    
  return this->add_connected_lines(pts, false, preproc_needed);
}

// -----------------------------------------------------------------------
//: Add a line segment, return smart ptr to the new the dbsk2d_bnd_contour
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_a_line(const vsol_line_2d_sptr & line, bool preproc_needed)
{
  // check line's existence
  if (!line) return 0;
  
  return this->add_a_line(line->p0()->get_p(), line->p1()->get_p());
}




// -----------------------------------------------------------------------
//: Add a polyline, return smart ptr to the new dbsk2d_bnd_contour
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_a_polyline(const vsol_polyline_2d_sptr & polyline, bool preproc_needed)
{
  // check polyline's existence
  if (!polyline) return 0;

  // Do nothing if the polyline has only 1 vertex
  if (polyline->size() < 2) return 0;

  // collect vertices of the polyline
  vcl_vector<vgl_point_2d<double > > pts;
  pts.reserve(polyline->size());

  for (unsigned int i = 0; i < polyline->size(); i++)
    pts.push_back(polyline->vertex(i)->get_p());
    
  return this->add_connected_lines(pts, false, preproc_needed);
}




// -----------------------------------------------------------------------
//: Add a polygon, return smart ptr to the new dbsk2d_bnd_contour
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_a_polygon(const vsol_polygon_2d_sptr& polygon, 
              bool preproc_needed)
{
  // check polygon's existence
  if (!polygon) return 0;

  // a polygon must have at least 3 vertices
  if (polygon->size() < 3) return 0;

  // collect the polygon's vertices
  vcl_vector< vgl_point_2d<double > > pts;
  pts.reserve(polygon->size());
  for (unsigned int i = 0; i < polygon->size(); ++i)
    pts.push_back(polygon->vertex(i)->get_p());

  return this->add_connected_lines(pts, true, preproc_needed);
}




// -----------------------------------------------------------------------
//: Add a LEMS curve, return smart ptr to dbsk2d_bnd_contour object
// For now, just perform a linear interpolation on the LEMS curve and
// add in as a polyline
//\TODO: use more advanced interpolation algorithm to interpolate as a polyarcs
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_an_interp_curve_2d( const dbsol_interp_curve_2d_sptr& curve, 
                             bool preproc_needed)
{
  // check existence
  if (!curve) return 0;

  // collect sampled points and convert to a polyline
  vcl_vector<vgl_point_2d<double > > pts;
  for (unsigned int i = 0; i < curve->size(); i ++)
  {
    pts.push_back(curve->interval(i)->point_at(0));
  }
  pts.push_back(curve->p1()->get_p());
  return this->add_connected_lines(pts, false, preproc_needed);
}






//-------------------------------------------------------------------------
//: Add a set of connected lines to the boundary. 
// This is a common function for add_a_polyline and add_a_polygon
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_connected_lines(const vcl_vector<vgl_point_2d<double > > &vertices, 
                    bool closed, bool preproc_needed )
{
  // form a new contour and add to `this' boundary
  dbsk2d_bnd_contour_sptr to_return = 
    dbsk2d_bnd_utils::new_polyline_contour(vertices, closed, this);

  this->add_a_bnd_contour(to_return, preproc_needed);
  return to_return;
}


//-------------------------------------------------------------------------
//: Add an arc to the boundary. Return smart poiter to the new contour
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_an_arc( vgl_point_2d<double > arc_p1, vgl_point_2d<double > arc_p2, 
                  double arc_k, bool preproc_needed)
{
  vcl_vector<vgl_point_2d<double > > pts;
  pts.push_back(arc_p1);
  pts.push_back(arc_p2);
  vcl_vector<double > curvatures(1, arc_k);
  return this->add_connected_arcs(pts, curvatures, false, preproc_needed);
}




//-------------------------------------------------------------------------
//: Add a set of connected arcs to the boundary
dbsk2d_bnd_contour_sptr dbsk2d_boundary::
add_connected_arcs( const vcl_vector<vgl_point_2d<double > > &vertices,
                   const vcl_vector<double >& curvatures,
                   bool closed, bool preproc_needed)
{
  if (vertices.size() < 2) return 0;

  // require
  dbsk2d_assert(curvatures.size() >= (vertices.size()-1));
  if (closed) dbsk2d_assert(curvatures.size() >= (vertices.size()));

  // for a list topological vertices
  // convert the pts into bnd_vertex and put into a list
  vcl_vector<dbsk2d_bnd_vertex_sptr > bv_list;
  bv_list.reserve(vertices.size());
  for (unsigned int i = 0; i < vertices.size(); ++i)
  {
    bv_list.push_back(dbsk2d_bnd_utils::new_vertex(vertices[i], this));
  }

  // add starting point as endpoint if the contour is closed
  if (closed) bv_list.push_back(bv_list[0]);

  // now link all these vertices into a chain and save as a contour
  vcl_vector<dbsk2d_bnd_edge_sptr > bnd_edges;
  for (unsigned int i=0; i<bv_list.size()-1; ++i)
  {
    dbgl_circ_arc arc(bv_list[i]->point(), bv_list[i+1]->point(), curvatures[i]);
    
    //if (vnl_math_abs(curvatures[i]) < MIN_ARC_CURVATURE)
    if (arc.height()<B_EPSILON)
    {
      bnd_edges.push_back(dbsk2d_bnd_utils::new_line_between(bv_list[i], 
        bv_list[i+1], this));
    }
    else
    {
      bnd_edges.push_back(dbsk2d_bnd_utils::new_arc_between(bv_list[i], 
        bv_list[i+1], curvatures[i], this));
    }
  }
  vcl_vector<signed char > directions(bnd_edges.size(), 1);
  
  // for a new contour and add to `this' boundary
  dbsk2d_bnd_contour_sptr to_return = 
    new dbsk2d_bnd_contour(bnd_edges, directions, this->nextAvailableID());

  this->add_a_bnd_contour(to_return, preproc_needed);
  return to_return;
}




// -----------------------------------------------------------------------
//: Remove empty contours
void dbsk2d_boundary::
remove_empty_contours()
{

  // preprocessed contours
  bnd_contour_list::iterator cit = this->preproc_contours_.begin();
  while (cit != this->preproc_contours_.end())
  {
    if ((*cit)->numinf()==0)
    {
      (*cit)->unlink();
      cit = this->preproc_contours_.erase(cit);
    }
    else
    {
      ++cit;
    }
  }


  // scratch contours
  cit = this->scratch_contours_.begin();
  while (cit != this->scratch_contours_.end())
  {
    if ((*cit)->numinf()==0)
    {
      (*cit)->unlink();
      cit = this->scratch_contours_.erase(cit);
    }
    else
    {
      ++cit;
    }
  }

  this->touch();
}



// -----------------------------------------------------------------------
//: Add a boundary contour to the boundary.
void dbsk2d_boundary::
add_a_bnd_contour(const dbsk2d_bnd_contour_sptr& new_bnd_contour, 
                  bool preproc_needed)
{
  if (preproc_needed)
  {
    this->add_a_scratch_contour(new_bnd_contour);
  }
  else
  {
    this->add_a_preproc_contour(new_bnd_contour);
  }
  this->touch();
  return;
}




// -----------------------------------------------------------------------
//: print a quick summary
// Need rewrite.
void dbsk2d_boundary::
print(vcl_ostream &os) const
{
  bnd_contour_list all_contours;
  this->all_contours(all_contours);
  os << "<dbsk2d_boundary  " << all_contours.size()
    << "  " << (void const *) this << ">\n";
}
  

//: describe
// Need rewrite.
void dbsk2d_boundary::
describe(vcl_ostream &os, int blanking) const
{
  for (int j=0; j<blanking; ++j) os << ' ';
  this->print(os);
  bnd_contour_list all;
  this->all_contours(all);
  for (bnd_contour_list::iterator cit = all.begin(); cit != all.end(); ++cit)
  {
    (*cit)->describe(os, blanking);
  }
}


// -----------------------------------------------------------------------
//: Print the belm list
void dbsk2d_boundary::
print_belm_list(vcl_ostream& os )
{
  for (dbsk2d_boundary::belm_iterator belm_it = this->belm_begin();
    belm_it != this->belm_end(); ++belm_it)
  {
    dbsk2d_ishock_belm* belm = *belm_it;
    if (belm->is_a_point())
    {
      os << "BPOINT id = " << belm->id() << 
        " \n   pt = " << belm->start() << vcl_endl;
    }
    else if (belm->is_a_line())
    {
      os << "BLINE id = " << belm->id() << 
        "\n   start id = " << belm->s_pt()->id() << 
        "   end id = " << belm->e_pt()->id() << vcl_endl;
    }
    else if (belm->is_an_arc())
    {
      os << "BARC  id = " << belm->id() << 
        "\n   start id = " << belm->s_pt()->id() << 
        "   end id = " << belm->e_pt()->id() << vcl_endl;
    }
  }
  return;
}


//: print a summary of partition parameters
void dbsk2d_boundary::
print_partition_summary(vcl_ostream& os) const
{
  os << "Partition params: \n" <<
    "(xmin, ymin)= (" << this->xmin() << "," << this->ymin() << ")\n" <<
    "num_rows= " << this->num_rows() << "   num_cols= " << this->num_cols() <<
    "\ncell_height= " << this->cell_h() << "  cell_width= " << this->cell_w() << 
    "\n(xmax, ymax)=(" << this->xmin()+this->width() << "," <<
    this->ymin()+this->height() << ") \n";
}











//dbsk2d_ishock_bpoint* dbsk2d_boundary::
//chopBElement(dbsk2d_ishock_belm* belm, dbsk2d_ishock_bpoint* StartPt, vgl_point_2d<double> EndPt, bool chopCompletely)
//{
//  //: \todo too much of this function needed to be copied to handle shocks. Reiterate
//
//  dbsk2d_ishock_bpoint *start_pt=NULL, *end_pt=NULL;
//
//  switch (belm->type()){
//    case BPOINT:
//    {
//      return (dbsk2d_ishock_bpoint*)belm;
//      break;
//    }
//    case BLINE:
//    {
//      dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*) belm;
//
//      //is it linked to other elements?
//      if (StartPt->is_a_junction_point())
//        start_pt = StartPt;
//      else
//        start_pt = add_a_point(StartPt->pt().x(), StartPt->pt().y());
//
//      //delete the original line
//      delGUIElement(bline);
//
//      if (chopCompletely){
//        end_pt = start_pt;
//      }
//      else {
//        //add the chopped piece
//        end_pt = add_a_point(EndPt.x(), EndPt.y());
//        add_a_line_segment_between(start_pt, end_pt);
//      }
//      break;
//    }
//    case BARC:
//    {
//      dbsk2d_ishock_barc* barc = (dbsk2d_ishock_barc*) belm;
//      bool bchopEnd = (StartPt==barc->s_pt());
//
//      //is it linked to other elements?
//      if (StartPt->is_a_junction_point())
//        start_pt = StartPt;
//      else
//        start_pt = add_a_point(StartPt->pt().x(), StartPt->pt().y());
//
//      //save the other parameters of the arc
//      vgl_point_2d<double> cen = barc->center();
//      double R = barc->R();
//      ARC_NUD nud = barc->nud();
//
//      //delete the original line
//      delGUIElement(barc);
//
//      if (chopCompletely){
//        end_pt = start_pt;
//      }
//      else {
//        //add the chopped piece
//        end_pt = add_a_point(EndPt.x(), EndPt.y());
//
//        if (bchopEnd)
//          add_an_arc_segment_between(start_pt, end_pt, cen, R, nud, ARC_NUS_SMALL);
//        else
//          add_an_arc_segment_between(end_pt, start_pt, cen, R, nud, ARC_NUS_SMALL);
//      }
//      break;
//    }
//    default: break;
//  }
//
//  //it is wise to remove all the contact shocks that this pair of points
//  //cause as well. This give the shock propagation a fresh problem
//  //and removes the chance that any zero length shocks form
//  ishock_elm_list ContactsToDel;
//  bnd_ishock_map_iter curS = start_pt->shock_map().begin();
//  for (; curS!=start_pt->shock_map().end(); ++curS) {
//    if (curS->second->label() == dbsk2d_ishock_elm::CONTACT)
//      ContactsToDel.push_back(curS->second);
//  }
//
//  //finally delete them
//  for (ishock_elm_list_iter curC = ContactsToDel.begin(); curC!=ContactsToDel.end(); ++curC)
//  {
//    _shock->remove_edge((dbsk2d_ishock_contact*)*curC);
//  }
//
//  return end_pt;
//}
//
//
//void dbsk2d_boundary::
//moveGUIElement (dbsk2d_ishock_belm* belm, double x, double y)
//{
//}
//
//
////Set Range of Influence of Shocks caused by the dbsk2d_boundary element...
//belm_list dbsk2d_boundary::
//getAllNeighboringBElementsFromAGUIElement(dbsk2d_ishock_belm* GUIElm)
//{
//  belm_list AllNeighboringBElms;
//  belm_list neighboringBElms;
//
//  AllNeighboringBElms.clear();
//
//  switch (GUIElm->type()) {
//    case BPOINT:
//    {
//      dbsk2d_ishock_bpoint* bpoint = (dbsk2d_ishock_bpoint*)GUIElm;
//
//      neighboringBElms = bpoint->getAllNeighboringBElements();
//      AllNeighboringBElms.merge(neighboringBElms);
//      neighboringBElms.clear();
//      break;
//    }
//    case BLINE:
//    {
//      dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)GUIElm;
//
//      neighboringBElms = bline->getAllNeighboringBElements();
//      AllNeighboringBElms.merge(neighboringBElms);
//      neighboringBElms.clear();
//
//      if (bline->twinLine()){
//        belm_list neighboringBElms = bline->twinLine()->getAllNeighboringBElements();
//        AllNeighboringBElms.merge(neighboringBElms);
//        neighboringBElms.clear();
//      }
//
//      neighboringBElms = bline->s_pt()->getAllNeighboringBElements();
//      AllNeighboringBElms.merge(neighboringBElms);
//      neighboringBElms.clear();
//
//      neighboringBElms = bline->e_pt()->getAllNeighboringBElements();
//      AllNeighboringBElms.merge(neighboringBElms);
//      neighboringBElms.clear();
//
//      break;
//    }
//    case BARC:
//    {
//      dbsk2d_ishock_barc* barc = (dbsk2d_ishock_barc*)GUIElm;
//
//      neighboringBElms = barc->getAllNeighboringBElements();
//      AllNeighboringBElms.merge(neighboringBElms);
//      neighboringBElms.clear();
//
//      if (barc->twinArc()){
//        neighboringBElms = barc->twinArc()->getAllNeighboringBElements();
//        AllNeighboringBElms.merge(neighboringBElms);
//        neighboringBElms.clear();
//      }
//
//      neighboringBElms = barc->s_pt()->getAllNeighboringBElements();
//      AllNeighboringBElms.merge(neighboringBElms);
//      neighboringBElms.clear();
//
//      neighboringBElms = barc->e_pt()->getAllNeighboringBElements();
//      AllNeighboringBElms.merge(neighboringBElms);
//      neighboringBElms.clear();
//      break;
//    }
//    default: break;
//   }
//
//  return AllNeighboringBElms;
//}
//
//
//void dbsk2d_boundary::
//setRangeOfInfluence (dbsk2d_ishock_belm* elm)
//{
///*
//   SLink* slist = elm->SElementList;
//
//   //go through the list and vcl_set bShow to GUI_RANGE_OF_INFLUENCE...
//   while (slist != 0) {
//      slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
//      //if (elm->type()!=BPOINT)
//      //   slist->se->setRangeOfInfluenceForChildShock (slist->se);
//      slist = slist->next;
//   }
//
//   //TWIN GUY...
//   switch (elm->type()) {
//      case BPOINT:
//           break;
//      case BLINE:
//
//           //twinLine
//           slist = ((dbsk2d_ishock_bline*)elm)->twinLine->SElementList;
//           while (slist != 0) {
//              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
//           //   slist->se->setRangeOfInfluenceForChildShock (slist->se);
//              slist = slist->next;
//           }
//           //endvgl_point_2d<double> 1
//           slist = ((dbsk2d_ishock_bline*)elm)->e_pt()[0]->SElementList;
//           while (slist != 0) {
//              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
//              //slist->se->setRangeOfInfluenceForChildShock (slist->se);
//              slist = slist->next;
//           }
//           //endvgl_point_2d<double> 2
//           slist = ((dbsk2d_ishock_bline*)elm)->e_pt()[1]->SElementList;
//           while (slist != 0) {
//              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
//              //slist->se->setRangeOfInfluenceForChildShock (slist->se);
//              slist = slist->next;
//           }
//           break;
//      case BARC:
//           //twinArc
//           slist = ((dbsk2d_ishock_barc*)elm)->twinArc->SElementList;
//           while (slist != 0) {
//              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
//           //   slist->se->setRangeOfInfluenceForChildShock (slist->se);
//              slist = slist->next;
//           }
//           //endvgl_point_2d<double> 1
//           slist = ((dbsk2d_ishock_barc*)elm)->e_pt()[0]->SElementList;
//           while (slist != 0) {
//              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
//              //slist->se->setRangeOfInfluenceForChildShock (slist->se);
//              slist = slist->next;
//           }
//           //endvgl_point_2d<double> 2
//           slist = ((dbsk2d_ishock_barc*)elm)->e_pt()[1]->SElementList;
//           while (slist != 0) {
//              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
//              //slist->se->setRangeOfInfluenceForChildShock (slist->se);
//              slist = slist->next;
//           }
//           break;
//      case BCIRCLE:
//           //twinCircle
//           slist = ((BCircle*)elm)->twinCircle->SElementList;
//           while (slist != 0) {
//              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
//           //   slist->se->setRangeOfInfluenceForChildShock (slist->se);
//              slist = slist->next;
//           }
//           break;
//   }*/
//}
//
//
//
////**************************************************************//
////             DEBUG FUNCTIONS
////**************************************************************//
//
//
//void dbsk2d_boundary::
//MessageOutBoundarySummaries (int wndid)
//{
//   int nTotalBElms=0;
//  int nTotalGUIElms=0;
//  int nTotalNonGUIElms=0;
//  int nBP=0, nBL=0, nBA=0;
//  int nGUIBP=0, nGUIBL=0, nGUIBA=0;
//  int nNonGUIBP=0, nNonGUIBL=0, nNonGUIBA=0;
//
//  belm_map_iter curB = BElmList.begin();
//  for (; curB!=BElmList.end(); curB++){
//    dbsk2d_ishock_belm* current = (curB->second);
//
//    //BoundaryLimitHack
//    if (_BoundaryLimit == BIG_RECTANGLE || _BoundaryLimit == BIG_CIRCLE)
//      if (current->id() <=8) continue;
//
//    if (!current->is_a_GUIelm())  nTotalNonGUIElms++;
//    else              nTotalGUIElms++;
//
//    switch (current->type()) {
//    case BPOINT:
//      if (!current->is_a_GUIelm()) nNonGUIBP++;
//      else               nGUIBP++;
//    break;
//    case BLINE:
//      if (!current->is_a_GUIelm()) nNonGUIBL++;
//      else               nGUIBL++;
//    break;
//    case BARC:
//      if (!current->is_a_GUIelm()) nNonGUIBA++;
//      else               nGUIBA++;
//    break;
//    default: break;
//    }
//  }
//
//  nTotalBElms  = nBElement();
//  nBP = nGUIBP + nNonGUIBP;
//  nBL = nGUIBL + nNonGUIBL;
//  nBA = nGUIBA + nNonGUIBA;
//
//  nTotalNonGUIElms = nNonGUIBP + nNonGUIBL + nNonGUIBA;
//  //dbsk2d_assert (nTotalBElms == nBP + nBL + nBA + 8 );
//  //dbsk2d_assert (nTotalBElms == nBP + nBL + nBA);
//
//   vcl_cout<<"dbsk2d_boundary Summaries" <<vcl_endl;
//   vcl_cout<<"# of Total dbsk2d_boundary Elements: "<< nTotalBElms <<vcl_endl;
//   vcl_cout<<"# of Total GUI BElements: "<< nTotalGUIElms
//     <<", total Non-GUI: "<< nTotalNonGUIElms <<vcl_endl;
//   vcl_cout<<"# of BPOINTs: " << nBP << " (GUI: " << nGUIBP
//     << ", Non-GUI: " << nNonGUIBP << ")" <<vcl_endl;
//   vcl_cout<<"# of BLines: "  << nBL << " (GUI: " << nGUIBL
//     << ", Non-GUI: " << nNonGUIBL << ")" <<vcl_endl;
//   vcl_cout<<"# of BArcs: "   << nBA << " (GUI: " << nGUIBA
//     << ", Non-GUI: " << nNonGUIBA << ")" <<vcl_endl;
//
//}
//
//void dbsk2d_boundary::
//DebugPrintBoundaryList()
//{
//  vcl_cout <<vcl_endl;
//  vcl_cout << " ==== BOUNDARY LIST ====" <<vcl_endl;
//  vcl_cout<< "BoundaryList: " <<vcl_endl;
//  belm_map_iter elmPtr = BElmList.begin();
//  for (; elmPtr != BElmList.end(); elmPtr++) {
//    dbsk2d_ishock_belm* current = (elmPtr->second);
//
//    switch (current->type()) {
//    case BPOINT:  vcl_cout<< "dbsk2d_ishock_bpoint"; break;
//    case BLINE:    vcl_cout<< "dbsk2d_ishock_bline"; break;
//    case BARC:    vcl_cout<< "dbsk2d_ishock_barc"; break;
//    default: break;
//    }
//    vcl_cout<< ", Bid: "<< current->id()<<vcl_endl;
//  }
//
//  vcl_cout <<" ========================" <<vcl_endl;
//
//}
//
//void dbsk2d_boundary::
//DebugPrintBElementInfoFromID(int id)
//{
//  belm_map_iter elmPtr = BElmList.find(id);
//
//  if (elmPtr != BElmList.end()){
//    dbsk2d_ishock_belm* current = elmPtr->second;
//
//#ifndef _VISUALIZER_CMDLINE_
//    //display info
//    current->getInfo(vcl_cout);
//#endif
//  }
//  else
//    vcl_cout <<"INVALID BOUNDARY ID: "<<id<<vcl_endl;
//}
//
//void dbsk2d_boundary::
//DebugPrintTaintedBoundaryList()
//{
//  vcl_cout <<vcl_endl;
//  vcl_cout << " ==== TAINTED BOUNDARY LIST ====" <<vcl_endl;
//  vcl_cout<< "BoundaryList: " <<vcl_endl;
//  belm_map_iter elmPtr = taintedBElmList.begin();
//  for (; elmPtr != taintedBElmList.end(); elmPtr++) {
//    dbsk2d_ishock_belm* current = (elmPtr->second);
//
//    switch (current->type()) {
//    case BPOINT:  vcl_cout<< "dbsk2d_ishock_bpoint"; break;
//    case BLINE:    vcl_cout<< "dbsk2d_ishock_bline"; break;
//    case BARC:    vcl_cout<< "dbsk2d_ishock_barc"; break;
//    default: break;
//    }
//    vcl_cout<< ", Bid: "<< current->id()<<vcl_endl;
//  }
//
//  vcl_cout <<" ========================" <<vcl_endl;
//}
//
//
//void dbsk2d_boundary::
//UpdateBoundary()
//{
//  //recompute the extrinsic locus of all the elements that were changed
//  //the update list is maintained so that minimal recomputation of the
//  //actual extrinsic locus is needed during computation
//  //
//  //: \todo Debate: Maintenance of this list might be too much time consuming
//
//  belm_map_iter curB = update_list.begin();
//  for (; curB!=update_list.end(); curB++) {
//    dbsk2d_ishock_belm* curBElm = (dbsk2d_ishock_belm*)(curB->second);
//    curBElm->compute_extrinsic_locus();
//  }
//
//  //clear list after it is drawn
//  update_list.clear();
//}












////************************************************************//
////    DELETE OPERATIONS
////************************************************************//
//
////delete all shocks caused by this boundary element
//void dbsk2d_boundary::
//delBElementShockList (dbsk2d_ishock_belm* belm)
//{
//  dbsk2d_assert(belm);
//
//  //go through the ShockList and delete all related shocks
//  while ( belm->shock_map().size()>0){
//    bnd_ishock_map_iter curS = belm->shock_map().begin();
//    dbsk2d_ishock_elm* selm = curS->second;
//    dbsk2d_assert(selm->id()>0);
//    _shock->remove_edge((dbsk2d_ishock_edge*)selm);
//  }
//}
//
//
//void dbsk2d_boundary::
//delBElement (dbsk2d_ishock_belm* belm)
//{
//  if ( GetBoundaryLimit() == BIG_RECTANGLE || 
//       GetBoundaryLimit() == BIG_CIRCLE) {
//    //BoundaryLimitHack
//    //do not allow deleting the first eight elements
//    //these represent the shock domain boundaries
//    if (belm->id()<=8)
//      return;
//  }
//
//  dbsk2d_assert(belm);
//  //delete all the shocks caused by this boundary element
//  delBElementShockList(belm);
//  //Then remove this element from the boundary list
//  BElmList.erase(belm->id());
//
//  //We also need to remove it from the taintedBElmList if it exists there
//  taintedBElmList.erase(belm->id());
//
//  //also remove it from the update list
//  update_list.erase(belm->id());
//
//  //and delete the element object
//  delete belm; belm=NULL;
//}
//
//
//void dbsk2d_boundary::
//delBElementList ()
//{
//  belm_map_iter curB;
//
//  if (GetBoundaryLimit() == NO_LIMIT) {
//    curB = BElmList.begin();
//    for (; curB!=BElmList.end(); curB++) {
//      dbsk2d_ishock_belm* curBElm = (dbsk2d_ishock_belm*)(curB->second);
//      delBElementShockList(curBElm);
//      delete curBElm;
//    }
//    BElmList.clear();
//  }
//  else if ( GetBoundaryLimit() == BIG_RECTANGLE || 
//            GetBoundaryLimit() == BIG_CIRCLE) {
//    curB = BElmList.begin();
//    for (; curB!=BElmList.end(); curB++) {
//      dbsk2d_ishock_belm* curBElm = (dbsk2d_ishock_belm*)(curB->second);
//      if (curBElm->id()>8) {
//        delBElementShockList(curBElm);
//        delete curBElm;
//      }
//    }
//
//    //remove the entries of the deleted objects
//    //doing it this way increases the speed of deletion
//    curB = BElmList.find(8); curB++;
//    BElmList.erase(curB, BElmList.end());
//  }
//
//  //tainted list is invalid so clear it too
//  update_list.clear();
//  taintedBElmList.clear();
//}
//
//
//
////return 1: deletion is successful
////return 0: deletion fails
//
////: \todo We should also make this function call local shock
//// detection function to patch up the shock structure
//// following the deletion of the elements. On second thoughts, maybe I decided that
//// this was not a safe thing to do. Check it out. }
//bool dbsk2d_boundary::
//delGUIElement (dbsk2d_ishock_belm* elm)
//{
//  if (_BoundaryLimit == BIG_RECTANGLE || _BoundaryLimit == BIG_CIRCLE) {
//    //BoundaryLimitHack
//    //this hack is duplicated in delBelement for safety
//    if (elm->id()<=8)
//      return false;
//  }
//
//   switch (elm->type()) {
//    case BPOINT:
//    {
//      if (((dbsk2d_ishock_bpoint*)elm)->is_a_free_point())
//        delBElement (elm);
//         break;
//    }
//    case BLINE:
//    {
//      dbsk2d_ishock_bline* bline;
//      if (elm->is_a_GUIelm())
//        bline = (dbsk2d_ishock_bline*)elm;
//      else
//        bline = ((dbsk2d_ishock_bline*)elm)->twinLine();
//
//         if (!bline->s_pt()->is_a_junction_point())
//            delBElement (bline->s_pt());
//      else {
//        //just remove link from the s_pt()
//        bline->s_pt()->disconnectFrom(bline);
//        bline->s_pt()->disconnectFrom(bline->twinLine());
//      }
//
//      if (!bline->e_pt()->is_a_junction_point())
//            delBElement (bline->e_pt());
//      else{
//        //just remove link from the e_pt()
//        bline->e_pt()->disconnectFrom(bline);
//        bline->e_pt()->disconnectFrom(bline->twinLine());
//      }
//
//      //having removed the links, now delete the lines
//      delBElement (bline->twinLine());
//      delBElement (bline);
//      break;
//    }
//    case BARC:
//    {
//      dbsk2d_ishock_barc* barc;
//      if (elm->is_a_GUIelm())
//        barc = (dbsk2d_ishock_barc*)elm;
//      else
//        barc = ((dbsk2d_ishock_barc*)elm)->twinArc();
//
//      if (!barc->s_pt()->is_a_junction_point())
//            delBElement (barc->s_pt());
//      else {
//        //just remove link from the point
//        barc->s_pt()->disconnectFrom(barc);
//        barc->s_pt()->disconnectFrom(barc->twinArc());
//      }
//
//      if (!barc->e_pt()->is_a_junction_point())
//         delBElement (barc->e_pt());
//      else {
//        //just remove link from the point
//        barc->e_pt()->disconnectFrom(barc);
//        barc->e_pt()->disconnectFrom(barc->twinArc());
//      }
//
//      //having removed the links, now delete the arcs
//      delBElement (barc->twinArc());
//      delBElement (barc);
//      break;
//    }
//    default: break;
//   }
//
//   return true;
//}
//
//
////************************************************************//
////    ADD OPERATIONS
////************************************************************//
//
//void dbsk2d_boundary::
//addBElement (dbsk2d_ishock_belm* elm)
//{
//  BElmList.insert(id_belm_pair(elm->id(), elm));
//
//  //also add it to the plagued list
//  taintedBElmList.insert(id_belm_pair(elm->id(), elm));
//}
//
//
//#endif 








//dbsk2d_ishock_belm* dbsk2d_boundary::
//add_a_line_segment_between (dbsk2d_ishock_bpoint* spt, dbsk2d_ishock_bpoint* ept)
//{
//  //: \todo Too much of this function has been duplicated here
//  // just so that some contact shocks can be cleaned. Need to find
//  // a better way to do that
//
//  //if the two boundary points are close, delete one point
//  if (_BisEqPoint(spt->pt(), ept->pt())){
//    mergeDuplicatepoints(spt, ept);
//    delBElement(ept);
//    return NULL;
//  }
//
//  bool delLBElm=false;
//  bool delRBElm=false;
//
//  dbsk2d_ishock_bpoint* fin_spt = spt;
//  dbsk2d_ishock_bpoint* fin_ept = ept;
//
//  double dtheta;
//  double u = _vPointPoint (spt->pt(), ept->pt());
//  //double lLength = _distPointPoint(spt->pt(), ept->pt());
//
//  dbsk2d_ishock_bline *lLine=NULL, *rLine=NULL;
//
//  double wL = ISHOCK_DIST_HUGE;
//  double wR = ISHOCK_DIST_HUGE;
//
//  //check for colinearity here
//  if (spt->is_an_end_point()){
//    if (spt->LinkedBElmList.front()->is_a_line()){
//      lLine = (dbsk2d_ishock_bline*) spt->LinkedBElmList.front();
//
//      double l, k;
//      //if spt is the starting point of this line
//      if (lLine->s_pt() == spt){
//        dtheta = vcl_fabs(u- angle0To2Pi(lLine->u()+vnl_math::pi));
//        l = _distPointPoint(spt->pt(),lLine->e_pt()->pt());
//        k = 1/getArcRadiusFromThreePoints(lLine->e_pt()->pt(), spt->pt(), ept->pt());
//      }
//      else {
//        dtheta = vcl_fabs(u- lLine->u());
//        l = _distPointPoint(spt->pt(),lLine->s_pt()->pt());
//        k = 1/getArcRadiusFromThreePoints(lLine->s_pt()->pt(), spt->pt(), ept->pt());
//      }
//
//      //Trying to fit the lines as arc in a box smaller than the accuracy of the input
//      wL = l*l*k/8;
//    }
//  }
//
//  //also check for colinearity on the other side
//  if (ept->is_an_end_point()){
//    if (ept->LinkedBElmList.front()->is_a_line()){
//      rLine = (dbsk2d_ishock_bline*)ept->LinkedBElmList.front();
//
//      double l, k;
//      //if ept is the starting point of this line
//      if (rLine->s_pt() == ept){
//        dtheta = vcl_fabs(u- rLine->u());
//        l = _distPointPoint(ept->pt(),rLine->e_pt()->pt());
//        k = 1/getArcRadiusFromThreePoints(rLine->e_pt()->pt(), ept->pt(), spt->pt());
//      }
//      else {
//        dtheta = vcl_fabs(u- angle0To2Pi(rLine->u()+vnl_math::pi));
//        l = _distPointPoint(ept->pt(),rLine->s_pt()->pt());
//        k = 1/getArcRadiusFromThreePoints(rLine->s_pt()->pt(), ept->pt(), spt->pt());
//      }
//
//      //Trying to fit the lines as arc in a box smaller than the accuracy of the input
//      wR = l*l*k/8;
//    }
//  }
//
//  //use the smaller scale one for now
//  if (wL < W_THRESHOLD && wL <= wR){ //boundary estimation accuracy
//    //merge them
//    if (lLine->s_pt() == spt){
//        fin_spt = lLine->e_pt();
//    }
//    else {
//        fin_spt = lLine->s_pt();
//    }
//
//    //delete the connected line
//    //delGUIElement(lLine);
//    //only mark for deletion
//    delLBElm = true;
//  }
//
//  if (wR < W_THRESHOLD && wR<wL){ //boundary estimation accuracy
//    //merge them
//    if (rLine->s_pt() == ept){
//        fin_ept = rLine->e_pt();
//    }
//    else {
//        fin_ept = rLine->s_pt();
//    }
//
//    //delete the connected line
//    //delGUIElement(rLine);
//    //only mark for deletion
//    delRBElm = true;
//  }
//
//  //Re-activate existing dbsk2d_ishock_contact to removes the chance of zero length shocks
//  if (!fin_spt->is_a_free_point()){
//    bnd_ishock_map_iter curS = fin_spt->shock_map().begin();
//    for (; curS!=fin_spt->shock_map().end(); ++curS) {
//      if (curS->second->label() == dbsk2d_ishock_elm::CONTACT) {
//        dbsk2d_ishock_contact* contact = (dbsk2d_ishock_contact*)curS->second;
//        _shock->remove_vertex (contact->cSNode());
//      }
//    }
//  }
//
//  if (!fin_ept->is_a_free_point()){
//    //check for colinearity here
//    bnd_ishock_map_iter curS = fin_ept->shock_map().begin();
//    for (; curS!=fin_ept->shock_map().end(); ++curS) {
//      if (curS->second->label() == dbsk2d_ishock_elm::CONTACT) {
//        dbsk2d_ishock_contact* contact = (dbsk2d_ishock_contact*)curS->second;
//        _shock->remove_vertex (contact->cSNode());
//      }
//    }
//  }
//
//  //now put in the new line
//
//  dbsk2d_ishock_bline* newelm = new dbsk2d_ishock_bline (fin_spt, fin_ept, nextAvailableID(), true); //GUI one
//  dbsk2d_ishock_bline* twinelm = new dbsk2d_ishock_bline (fin_ept, fin_spt, nextAvailableID(), false); //Non-GUI
//
//  //link to one another (best to do this here)
//  newelm->set_twinLine(twinelm);
//  twinelm->set_twinLine(newelm);
//
//  addBElement (newelm);
//  addBElement (twinelm);
//
//  //only the GUIElement need to be displayed
//  update_list.insert(id_belm_pair(newelm->id(), newelm));
//
//  //but the points might need to be updated
//  if (fin_spt->is_a_GUIelm())
//    update_list.insert(id_belm_pair(fin_spt->id(), fin_spt));
//  if (fin_ept->is_a_GUIelm())
//    update_list.insert(id_belm_pair(fin_ept->id(), fin_ept));
//
//  //the points are no longer gui elements
//  fin_spt->set_GUIelm (false);
//  fin_ept->set_GUIelm (false);
//
//  // delete the old lines now if necessary
//  if (delRBElm)    delGUIElement(rLine);
//  if (delLBElm)    delGUIElement(lLine);
//
//   return (dbsk2d_ishock_belm*)newelm;
//}
//
//
//dbsk2d_ishock_belm* dbsk2d_boundary::
//add_an_arc_segment(double sx, double sy, double ex, double ey,
//                    double cx, double cy,
//                    double r, ARC_NUD nud, ARC_NUS nus)
//{
//  //if the arc is zero length, just add a point
//  if (BisEq(sx, ex) && BisEq(sy, ey)) {
//    dbsk2d_assert (0);
//    vcl_cout << "arc is too small! " <<vcl_endl;
//    return NULL;
//    //return add_a_point(sx, sy);
//  }
//
//  if (r==0)
//    return NULL;
//
//  dbsk2d_ishock_bpoint* spt = new dbsk2d_ishock_bpoint (sx, sy, nextAvailableID());
//  dbsk2d_ishock_bpoint* ept = new dbsk2d_ishock_bpoint (ex, ey, nextAvailableID());
//  addBElement (spt);
//   addBElement (ept);
//
//  dbsk2d_ishock_belm* newelm = add_an_arc_segment_between (spt, ept, vgl_point_2d<double>(cx,cy), r, nud, nus);
//
//   return newelm;
//}
//
//
//dbsk2d_ishock_belm* dbsk2d_boundary::
//add_an_arc_segment_between (dbsk2d_ishock_bpoint* spt, dbsk2d_ishock_bpoint* ept, vgl_point_2d<double> center,
//                          double r, ARC_NUD nud, ARC_NUS nus)
//{
//  //if the two boundary points are close, delete one point
//  if (_BisEqPoint(spt->pt(), ept->pt())){
//    mergeDuplicatepoints(spt, ept);
//    delBElement(ept);
//    return NULL;
//  }
//
//  if (r> MAX_RADIUS){
//    return add_a_line_segment_between(spt, ept);
//  }
//
//  if (nud==ARC_NUD_CW) { //this is to always make the GUI arc the CCW one
//    dbsk2d_ishock_bpoint* temp = ept;
//    ept = spt;
//    spt = temp;
//    nud = ARC_NUD_CCW;
//  }
//
//  //Determine if this arc is better suited to be a line
//  double v1 = _vPointPoint(center, spt->pt());
//  double v2 = _vPointPoint(center, ept->pt());
//  double theta = CCW(v1, v2);//because should always be counterclockwise
//
//  //Trying to fit the arc in a box smaller than the accuracy of the input
//  theta = theta/2;
//  //double l = 2*r*vcl_sin(theta);
//  //double w = l*l/r/8;
//
//  //if (w < W_THRESHOLD){ //boundary estimation accuracy
//  //  return add_a_line_segment_between(spt, ept);
//  //}
//
//  //030321 If the arc is a big arc, we break it into two.
//  //This will solve a lot of annoying cases later on.
//  if (nus==ARC_NUS_LARGE) { //1)Big arc case:
//    VECTOR_TYPE svector = _vPointPoint (center, spt->pt());
//    VECTOR_TYPE evector = _vPointPoint (center, ept->pt());
//    double angle;
//    if (nud==ARC_NUD_CCW)
//      angle = CCW (svector, evector);
//    else
//      angle = CCW (evector, svector);
//    VECTOR_TYPE mvector;
//    if (nud==ARC_NUD_CCW)
//      mvector = svector + angle/2;
//    else
//      mvector = evector + angle/2;
//    vgl_point_2d<double> mpoint = _translatePoint (center, mvector, r);
//
//    //add the midpoint
//    dbsk2d_ishock_bpoint* mpt = new dbsk2d_ishock_bpoint (mpoint.x(), mpoint.y(), nextAvailableID());
//    addBElement (mpt);
//
//    add_an_arc_segment_between (mpt, ept, center, r, nud, ARC_NUS_SMALL);
//    return add_an_arc_segment_between (spt, mpt, center, r, nud, ARC_NUS_SMALL);
//  }
//  else { //2)Small arc case:
//    dbsk2d_ishock_barc* newelm = new dbsk2d_ishock_barc (spt, ept, nextAvailableID(), true, center, r, nud);
//    dbsk2d_ishock_barc* twinelm = new dbsk2d_ishock_barc (ept, spt, nextAvailableID(), false, center, r, (ARC_NUD)(-(int)nud));
//
//    //link to one another (best to do this here)
//    newelm->set_twinArc(twinelm);
//    twinelm->set_twinArc(newelm);
//
//    addBElement (newelm);
//    addBElement (twinelm);
//
//    //only the GUIElement need to be displayed
//    update_list.insert(id_belm_pair(newelm->id(), newelm));
//
//    //but the points might need to be updated
//    if (spt->is_a_GUIelm())
//      update_list.insert(id_belm_pair(spt->id(), spt));
//    if (ept->is_a_GUIelm())
//      update_list.insert(id_belm_pair(ept->id(), ept));
//
//    //the points are no longer gui elements
//    spt->set_GUIelm (false);
//    ept->set_GUIelm (false);
//
//    return newelm;
//  }
//}
//
//
//dbsk2d_ishock_belm* dbsk2d_boundary::
//add_an_arc_segment_between (vgl_point_2d<double> spt, dbsk2d_ishock_bpoint* bept, vgl_point_2d<double> center,
//                       double r, ARC_NUD nud, ARC_NUS nus)
//{
//  //1)if the arc is zero length, return
//  if (_BisEqPoint(spt, bept->pt())) {
//    return NULL;
//  }
//
//  dbsk2d_ishock_bpoint* bspt = new dbsk2d_ishock_bpoint (spt.x(), spt.y(), nextAvailableID());
//  addBElement (bspt);
//
//  return add_an_arc_segment_between (bspt, bept, center, r, nud, nus);
//}
//
//
//
//dbsk2d_ishock_belm* dbsk2d_boundary::
//add_an_arc_segment_between (dbsk2d_ishock_bpoint* bspt, vgl_point_2d<double> ept, vgl_point_2d<double> center,
//                       double r, ARC_NUD nud, ARC_NUS nus)
//{
//  //1)if the line is zero length, return
//  if (_BisEqPoint(bspt->pt(), ept)) {
//    return NULL;
//  }
//
//  dbsk2d_ishock_bpoint* bept = new dbsk2d_ishock_bpoint (ept.x(), ept.y(), nextAvailableID());
//  addBElement (bept);
//
//  return add_an_arc_segment_between (bspt, bept, center, r, nud, nus);
//}
//
//
//void dbsk2d_boundary::
//add_biarc_segments_between (dbsk2d_ishock_bpoint* arc_start_pt, double SAngle,
//                        dbsk2d_ishock_bpoint* arc_end_pt, double EAngle)
//{
//  ////Compute the biarcs:
//  ////now that we have the points and the appropriate tangents
//  ////we can call the biarc completion code
//  //BiArcShock new_biArc(arc_start_pt->pt(), SAngle, arc_end_pt->pt(), EAngle);
//  //new_biArc.compute_biarc_params();
//  //new_biArc.compute_other_stuff();
//
//  //vgl_point_2d<double> mid = new_biArc.bi_arc_params.end1;
//  //double R1 = vcl_fabs(new_biArc.bi_arc_params.radius1);
//  //double R2 = vcl_fabs(new_biArc.bi_arc_params.radius2);
//  //int dir1 = new_biArc.bi_arc_params.dir1;
//  //int dir2 = new_biArc.bi_arc_params.dir2;
//  //double l1 = new_biArc.bi_arc_params.Length1;
//  //double l2 = new_biArc.bi_arc_params.Length2;
//
//  //dbsk2d_ishock_bpoint* mid_pt = (dbsk2d_ishock_bpoint*) add_a_point(mid.x(), mid.y());
//  //vgl_point_2d<double> cen1 = getCenterOfArc (arc_start_pt->pt().x(), arc_start_pt->pt().y(), mid_pt->pt().x(), mid_pt->pt().y(), R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
//  //vgl_point_2d<double> cen2 = getCenterOfArc (mid_pt->pt().x(), mid_pt->pt().y(), arc_end_pt->pt().x(), arc_end_pt->pt().y(), R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
//  //add_an_arc_segment_between (arc_start_pt, mid_pt, cen1, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
//  //add_an_arc_segment_between (mid_pt, arc_end_pt, cen2, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
//}

