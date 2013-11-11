// This is brcv/rec/dbskr/pro/dbskr_compute_sk_path_curve_process.cxx

//:
// \file

#include "dbskr_compute_sk_path_curve_process.h"
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_graph.h>

#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_compute_scurve.h>

#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>

#include <vgl/vgl_distance.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_tree.h>

dbskr_compute_sk_path_curve_process::dbskr_compute_sk_path_curve_process()
{
  if (!parameters()->add( "compute reconst boundary polygon?" , "-reconst_poly" , (bool) true ) ||
      !parameters()->add( "construct with circular completions?" , "-circular_ends" , (bool) true ) ||
      !parameters()->add( "Start node id" , "-sN_id" , (int) 1203 ) ||
      !parameters()->add( "Path edge list" , "-sh_path" , vcl_string("164 165 113 114 107 106 96 97 73 72 66 67 38 39") ) ||
      !parameters()->add( "Interpolate curve" , "-b_interpolate" , (bool) true ) ||
      !parameters()->add( "Interpolate ds" , "-interpolate_ds" , (double) 1.0 ) ||
      !parameters()->add( "Subsample curve" , "-b_subsample" , (bool) true ) ||
      !parameters()->add( "Subsample ds" , "-subsample_ds" , (double) 1.0 ) ||
      !parameters()->add( "Display pts only" , "-b_pts_only" , (bool) false ) ||
      //!parameters()->add( "Use Thomas' code" , "-b_Thomas" , (bool) false ) ||
      !parameters()->add( "Display (s,t) coordinates" , "-b_coords" , (bool) false) ||
      !parameters()->add( "Method 1/2" , "-b_method" , (bool) false) ||
      !parameters()->add( "N between samples" , "-sN" , (int) 2 ) ||
      !parameters()->add( "N along Radius" , "-sNR" , (int) 5 )) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

dbskr_compute_sk_path_curve_process::~dbskr_compute_sk_path_curve_process()
{
}


//: Clone the process
bpro1_process* dbskr_compute_sk_path_curve_process::clone() const
{
  return new dbskr_compute_sk_path_curve_process(*this);
}


vcl_string dbskr_compute_sk_path_curve_process::name()
{
  return "Compute Shock-path Curve";
}

vcl_vector< vcl_string > dbskr_compute_sk_path_curve_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

vcl_vector< vcl_string > dbskr_compute_sk_path_curve_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

int
dbskr_compute_sk_path_curve_process::input_frames()
{
  return 1;
}

int
dbskr_compute_sk_path_curve_process::output_frames()
{
  return 1;
}

bool
dbskr_compute_sk_path_curve_process::execute()
{
  // get input storage class
  dbsk2d_shock_storage_sptr shock;
  shock.vertical_cast(input_data_[0][0]);

  //get the shock graph
  dbsk2d_shock_graph_sptr sg = shock->get_shock_graph();

  //subsample curve?
    bool subsample=false, binterpolate=false, pts_only=false, bThomas_curve, b_coords=false;
    double interpolate_ds=0, subsample_ds=0;
    parameters()->get_value( "-b_interpolate" , binterpolate );
    parameters()->get_value( "-interpolate_ds" , interpolate_ds );
    parameters()->get_value( "-b_subsample" , subsample );
    parameters()->get_value( "-subsample_ds" , subsample_ds );
    parameters()->get_value( "-b_pts_only" , pts_only );
    parameters()->get_value( "-b_Thomas" , bThomas_curve );
    parameters()->get_value( "-b_coords" , b_coords );

  bool reconst_poly;
  parameters()->get_value( "-reconst_poly", reconst_poly);
  if (sg && reconst_poly) {

    bool circular_ends;
    parameters()->get_value( "-circular_ends", circular_ends);

    // create the output storage class
    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    vcl_vector< vsol_spatial_object_2d_sptr > contours;

    dbskr_tree_sptr tree = new dbskr_tree((float)subsample_ds, (float)interpolate_ds);
    tree->acquire(sg, false, circular_ends, false);
    vsol_polygon_2d_sptr poly = tree->compute_reconstructed_boundary_polygon(circular_ends);
    
    contours.push_back(poly->cast_to_spatial_object());

    output_vsol->add_objects(contours);
    output_data_.clear();
    output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_vsol));

  } else if (sg)
  {
    //get start node
    int start_node_id=0;
    parameters()->get_value( "-sN_id" , start_node_id );


    //get shock path
    vcl_string path_list;
    parameters()->get_value( "-sh_path" , path_list );

    //compute shock path curve
    dbsk2d_shock_node_sptr start_node;
    vcl_vector<dbsk2d_shock_edge_sptr> path1;

    //construct the simulated path from these ids
    get_path_from_ids (sg, start_node, path1, start_node_id, path_list);

    //get the shock curve for this path using both methods
    //dbskr_scurve_sptr sk_path_Thomas = dbskr_compute_scurve_Thomas(start_node, path1, binterpolate, subsample);
    dbskr_scurve_sptr sk_path_new = dbskr_compute_scurve(start_node, path1, false, binterpolate, subsample, interpolate_ds, subsample_ds);

    //compare the curves
    //compare_curves(sk_path_new, sk_path_Thomas);

    dbskr_scurve_sptr sk_path;
    //if (bThomas_curve)
    //  sk_path = sk_path_Thomas;
    //else
      sk_path = sk_path_new;
      
    // create the output storage class
    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    vcl_vector< vsol_spatial_object_2d_sptr > contours;

    // for debugging fragment_pt
    if (b_coords)
    {
      bool bmethod=false;
      int N=0, NR=0;    
      parameters()->get_value( "-sN" , N );
      parameters()->get_value( "-sNR" , NR );
      parameters()->get_value( "-b_method" , bmethod );
      
      
      //draw the oars first
      for (int i=0; i<sk_path->num_points()-1; i++){
        for (int ip=0; ip<=N; ip++){
          vsol_line_2d_sptr line1 = new vsol_line_2d();
          vsol_line_2d_sptr line2 = new vsol_line_2d();
          
          if (!bmethod){
            double r_ip = sk_path->interp_radius(i, i+1, N, ip);
            line1->set_p0(new vsol_point_2d(sk_path->fragment_pt(i, i+1, N, ip, 0)));
            line1->set_p1(new vsol_point_2d(sk_path->fragment_pt(i, i+1, N, ip, r_ip)));
            line2->set_p0(new vsol_point_2d(sk_path->fragment_pt(i, i+1, N, ip, 0)));
            line2->set_p1(new vsol_point_2d(sk_path->fragment_pt(i, i+1, N, ip, -r_ip)));
          }
          else {
            double index = double(i) + double(ip)/double(N);
            double r_ip = sk_path->interp_radius(index);
            line1->set_p0(new vsol_point_2d(sk_path->fragment_pt(index, 0)));
            line1->set_p1(new vsol_point_2d(sk_path->fragment_pt(index, r_ip)));
            line2->set_p0(new vsol_point_2d(sk_path->fragment_pt(index, 0)));
            line2->set_p1(new vsol_point_2d(sk_path->fragment_pt(index, -r_ip)));
          }
          contours.push_back(line1->cast_to_spatial_object());
          contours.push_back(line2->cast_to_spatial_object());
        }
      }

      //draw the parallel curves
      for (int r=0; r<=NR; r++){
        vcl_vector< vsol_point_2d_sptr > plus_pts, minus_pts;

        for (int i=0; i<sk_path->num_points()-1; i++){
          for (int ip=0; ip<=N; ip++){
            if (!bmethod){
              double r_ip = sk_path->interp_radius(i, i+1, N, ip);
              plus_pts.push_back(new vsol_point_2d(sk_path->fragment_pt(i, i+1, N, ip, r_ip*r/NR)));
              minus_pts.push_back(new vsol_point_2d(sk_path->fragment_pt(i, i+1, N, ip, -r_ip*r/NR)));
            }
            else {
              double index = double(i) + double(ip)/double(N);
              double r_ip = sk_path->interp_radius(index);
              plus_pts.push_back(new vsol_point_2d(sk_path->fragment_pt(index, r_ip*r/NR)));
              minus_pts.push_back(new vsol_point_2d(sk_path->fragment_pt(index, -r_ip*r/NR)));
            }
          }
        }

        vsol_polyline_2d_sptr BP_curve_p1_2 = new vsol_polyline_2d (plus_pts);
        contours.push_back(BP_curve_p1_2->cast_to_spatial_object());

        vsol_polyline_2d_sptr BP_curve_m1_2 = new vsol_polyline_2d (minus_pts);
        contours.push_back(BP_curve_m1_2->cast_to_spatial_object());
      }
      output_vsol->add_objects(contours);
    }
    else if (pts_only) 
    {
      //return 3 points sets
      vcl_vector< vsol_spatial_object_2d_sptr > sh_pts, bdry_plus_pts, bdry_minus_pts;
      for (int i=0; i<sk_path->num_points(); i++){
        sh_pts.push_back(new vsol_point_2d(sk_path->sh_pt(i)));
        bdry_plus_pts.push_back(new vsol_point_2d(sk_path->bdry_plus_pt(i)));
        bdry_minus_pts.push_back(new vsol_point_2d(sk_path->bdry_minus_pt(i)));
      }

      output_vsol->add_objects(sh_pts);
      output_vsol->add_objects(bdry_plus_pts);
      output_vsol->add_objects(bdry_minus_pts);

      // for debugging fragment_pt
      vcl_vector< vsol_spatial_object_2d_sptr > plus_pts, minus_pts;
      for (int i=0; i<sk_path->num_points(); i++){
        plus_pts.push_back(new vsol_point_2d(sk_path->fragment_pt(i, sk_path->time(i)/2)));
        minus_pts.push_back(new vsol_point_2d(sk_path->fragment_pt(i, -sk_path->time(i)/2)));
      }
      output_vsol->add_objects(plus_pts);
      output_vsol->add_objects(minus_pts);
    }
    else {
      //return as 3 vsol_polyline_2d 
      vcl_vector< vsol_point_2d_sptr > sh_pts, bdry_plus_pts, bdry_minus_pts;
      for (int i=0; i<sk_path->num_points(); i++){
        sh_pts.push_back(new vsol_point_2d(sk_path->sh_pt(i)));
        bdry_plus_pts.push_back(new vsol_point_2d(sk_path->bdry_plus_pt(i)));
        bdry_minus_pts.push_back(new vsol_point_2d(sk_path->bdry_minus_pt(i)));
      }

      vsol_polyline_2d_sptr sh_curve = new vsol_polyline_2d (sh_pts);
      contours.push_back(sh_curve->cast_to_spatial_object());
      
      vsol_polyline_2d_sptr BP_curve = new vsol_polyline_2d (bdry_plus_pts);
      contours.push_back(BP_curve->cast_to_spatial_object());
      
      vsol_polyline_2d_sptr BM_curve = new vsol_polyline_2d (bdry_minus_pts);
      contours.push_back(BM_curve->cast_to_spatial_object());

      output_vsol->add_objects(contours);
    }
    
    output_data_.clear();
    output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_vsol));
  }

  return true;
}

vcl_vector<int> 
dbskr_compute_sk_path_curve_process::parseLineForNumbers(vcl_string s)
{
  vcl_vector <int> vec;

  if (s.size() > 0){

    vcl_string numerics("0123456789.-+e");
    vcl_string::size_type pos1=0,pos2=0;

    while((pos1=s.find_first_of(numerics,pos1)) != vcl_string::npos) {
      pos2=s.find_first_not_of(numerics,pos1);
      vec.push_back(atoi(s.substr(pos1,pos2-pos1).c_str()));
      pos1=pos2;
    }
  }
  return vec;
}

void dbskr_compute_sk_path_curve_process::get_path_from_ids (
                        dbsk2d_shock_graph_sptr shock_graph, 
                        dbsk2d_shock_node_sptr & start_node,
                        vcl_vector<dbsk2d_shock_edge_sptr> & path1, 
                        int start_node_id, vcl_string sh_path)
{
  //get the start node object
  for (dbsk2d_shock_graph::vertex_iterator v_it = shock_graph->vertices_begin(); 
       v_it != shock_graph->vertices_end(); v_it++)
  {
    if ((*v_it)->id() == start_node_id){
      start_node = (*v_it);
      break;
    }
  }

  //get the list of edge objects
  vcl_vector<int> edge_ids = parseLineForNumbers(sh_path);
  int num_of_edges = edge_ids.size();
  for (int i=0; i<num_of_edges; i++)
  {
    for (dbsk2d_shock_graph::edge_iterator e_it = shock_graph->edges_begin(); 
         e_it != shock_graph->edges_end(); e_it++)
    {
      if ((*e_it)->id() == edge_ids[i]){
        path1.push_back(*e_it);
        break;
      }
    }
  }
}

bool
dbskr_compute_sk_path_curve_process::finish()
{
  return true;
}

/*
// The following function was written to compare against Thomas' original 
// code to compute shock curves

#include "sdefc/ShockCurve.h"

dbskr_scurve_sptr 
dbskr_compute_sk_path_curve_process::
dbskr_compute_scurve_Thomas(dbsk2d_shock_node_sptr start_node,
                            vcl_vector<dbsk2d_shock_edge_sptr> path,
                            bool binterpolate, bool bsub_sample)
{
  vcl_vector< Point > sh_pts;
  vcl_vector<double> time, alpha, theta;
  vcl_vector <int> shock_dir;
  vcl_vector<double> velocity, acceleration, curvature;

  dbsk2d_shock_node_sptr cur_start_node = start_node;

  //traverse through the path, interpolating where necessary
  for (vcl_vector<dbsk2d_shock_edge_sptr>::iterator e_it = path.begin();
       e_it != path.end(); e_it++)
  {
    dbsk2d_xshock_edge* cur_edge = dynamic_cast<dbsk2d_xshock_edge*>(e_it->ptr());

    if (cur_start_node == cur_edge->source())
    {
      //this edge is to be reconstructed the right way

      //go through the edge samples and append it to the shock curve
      for (int i=0; i< cur_edge->num_samples(); i++)
      {
        dbsk2d_xshock_sample_sptr sample = cur_edge->sample(i);

        sh_pts.push_back(Point(sample->pt.x(), sample->pt.y()));
        time.push_back(sample->radius); 
        if (sample->speed != 0 && sample->speed < 99990) //100000 signals infinity
          alpha.push_back(vcl_asin(1/sample->speed));
        else
          alpha.push_back(0.0);

        theta.push_back(sample->theta);
        shock_dir.push_back(1); //direction of flow is same as actual
        velocity.push_back(sample->speed);
        acceleration.push_back(0.0);
        curvature.push_back(0.0);
      }
    }
    else 
    {
      //this edge is to be flipped

      //go through the edge samples in the reverse order and append it to the shock curve
      for (int i=cur_edge->num_samples()-1; i>=0 ; i--)
      {
        dbsk2d_xshock_sample_sptr sample = cur_edge->sample(i);

        sh_pts.push_back(Point(sample->pt.x(), sample->pt.y()));
        time.push_back(sample->radius); 
        if (sample->speed != 0 && sample->speed < 99990) //100000 signals infinity
          alpha.push_back(vcl_asin(1/sample->speed));
        else
          alpha.push_back(0.0);
        
        theta.push_back(sample->theta);
        shock_dir.push_back(-1); //direction of flow is reversed
        velocity.push_back(sample->speed);
        acceleration.push_back(0.0);
        curvature.push_back(0.0);
      }
    }

    //update the cur_start_node to the opposite node
    cur_start_node = cur_edge->opposite(cur_start_node);
  }

  int num_points = sh_pts.size();

  char startNode, endNode;

  if (start_node->degree()==1)//some sinks can be degree 1
    startNode = 'A';
  else
    startNode = 'J';

  if (cur_start_node->degree()==1)//some sinks can be degree 1
    endNode = 'A';
  else
    endNode = 'J';

  //First form the curve using Thomas' code
  // The curve computed by Thomas' code is then converted into a dbskr_scurve
  ShockCurve Thomas_scurve(sh_pts, time, velocity, theta, 
    curvature, acceleration, shock_dir, num_points, startNode, endNode, binterpolate, bsub_sample);
  
  vcl_vector< vgl_point_2d<double> > intp_sh_pts;
  vcl_vector< vgl_point_2d<double> > intp_bdry_plus, intp_bdry_minus;
  vcl_vector<double> intp_time, intp_alpha, intp_theta;
  vcl_vector <int> intp_shock_dir;

  int num_intp_points = Thomas_scurve.numPoints();

  for (int i=0; i<num_intp_points; i++)
  {
    intp_sh_pts.push_back(vgl_point_2d<double>(Thomas_scurve.point(i).x(), Thomas_scurve.point(i).y()));
    intp_bdry_plus.push_back(vgl_point_2d<double>(Thomas_scurve.boundaryPlusX(i), Thomas_scurve.boundaryPlusY(i)));
    intp_bdry_minus.push_back(vgl_point_2d<double>(Thomas_scurve.boundaryMinusX(i), Thomas_scurve.boundaryMinusY(i)));
    intp_time.push_back(Thomas_scurve.time(i));
    intp_theta.push_back(Thomas_scurve.angle(i));
    intp_alpha.push_back(Thomas_scurve.alpha(i)); 
    intp_shock_dir.push_back(Thomas_scurve.shockDir(i));
  }

  //construct the shock curve from the compiled information
  dbskr_scurve_sptr intp_shock_curve = new dbskr_scurve(num_intp_points,
                                                        intp_sh_pts,
                                                        intp_bdry_plus, intp_bdry_minus, 
                                                        intp_time, intp_theta, intp_alpha, intp_shock_dir,
                                                        false);

  return intp_shock_curve;
}

void dbskr_compute_sk_path_curve_process::
compare_curves(dbskr_scurve_sptr c1, dbskr_scurve_sptr c2)
{
  vcl_cout << "Comparing Curves point-by-point...\n";
  vcl_cout << "Curves have same # of points ? " << (c1->num_points() == c2->num_points()) << vcl_endl;

  double error = 0;

  //go through them to compare them point by point
  for (int i=0; i < c1->num_points(); i++)
  {
    if (i >= c2->num_points())
      break;

    if (vgl_distance(c1->sh_pt(i), c2->sh_pt(i))>0.01){
      vcl_cout << "Sh pt["<<i<<"]: (" << c1->sh_pt(i).x() << ", " << c1->sh_pt(i).y() << ")";
      vcl_cout << "(" << c2->sh_pt(i).x() << ", " << c2->sh_pt(i).y() << ")" << vcl_endl;
    }

    if (vgl_distance(c1->bdry_plus_pt(i), c2->bdry_plus_pt(i))>0.01){
      vcl_cout << "B+ pt["<<i<<"]: (" << c1->bdry_plus_pt(i).x() << ", " << c1->bdry_plus_pt(i).y() << ")";
      vcl_cout << "(" << c2->bdry_plus_pt(i).x() << ", " << c2->bdry_plus_pt(i).y() << ")" << vcl_endl;
    }
    
    if (vgl_distance(c1->bdry_minus_pt(i), c2->bdry_minus_pt(i))>0.01){
      vcl_cout << "B- pt["<<i<<"]: (" << c1->bdry_minus_pt(i).x() << ", " << c1->bdry_minus_pt(i).y() << ")";
      vcl_cout << "(" << c2->bdry_minus_pt(i).x() << ", " << c2->bdry_minus_pt(i).y() << ")" << vcl_endl;
    }

    if (vcl_fabs(c1->time(i)-c2->time(i))>0.01)
      vcl_cout << "Radius["<<i<<"]: " << c1->time(i) << ", " << c2->time(i) << vcl_endl;

    if (vcl_fabs(c1->theta(i)-c2->theta(i))>0.01)
      vcl_cout << "theta["<<i<<"]: " << c1->theta(i) << ", " << c2->theta(i) << vcl_endl;
    
    if (vcl_fabs(c1->alpha(i)-c2->alpha(i))>0.01)
      vcl_cout << "alpha["<<i<<"]: " << c1->alpha(i) << ", " << c2->alpha(i) << vcl_endl;
    
    if (c1->shock_dir(i)!=c2->shock_dir(i))
      vcl_cout << "S-dir["<<i<<"]: " << c1->shock_dir(i) << ", " << c2->shock_dir(i) << vcl_endl;
  }

}
*/

