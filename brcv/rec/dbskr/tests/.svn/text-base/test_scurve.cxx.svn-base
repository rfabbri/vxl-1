#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vgl/vgl_distance.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>

#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_compute_scurve.h>
#include <dbskr/dbskr_dpmatch.h>

int read_curve_file2(vcl_ifstream& fs,
                    vcl_vector<double>& time, 
                    vcl_vector<double>& alpha, 
                    vcl_vector <int>& shock_dir, 
                    vcl_vector< vgl_point_2d<double> >& bdry_plus, 
                    vcl_vector< vgl_point_2d<double> >& bdry_minus,  
                    vcl_vector< vgl_point_2d<double> >& pt_array,   // points on shock branch  
                    vcl_vector<double>& bdry_plus_arclength, 
                    vcl_vector<double>& bdry_plus_angle, 
                    vcl_vector<double>& bdry_minus_arclength, 
                    vcl_vector<double>& bdry_minus_angle,
                    vcl_vector<double>& angle,
                    vcl_vector<double>& velocity,
                    vcl_vector<double>& curvature) 
{
  int size;
  fs >> size;
  for (int i = 0; i<size; i++) {
    
    //  fprintf(fp,"%7.5f %7.5f %7.5f %7.5f %8.6f %7.5f ",
    //    _ptArray[i].x(),_ptArray[i].y(),_time[i],_arcLength[i],
    //    (999.0<_velocity[i]?999.0:_velocity[i]),_curvature[i]);
    
    float x, y, dummy;
    fs >> x;
    fs >> y;  
    vgl_point_2d<double> p(x, y);   
    pt_array.push_back(p);

    fs >> dummy; time.push_back(dummy);
    fs >> dummy;  // arclength ignored
    fs >> dummy; velocity.push_back(dummy);
    fs >> dummy; curvature.push_back(dummy);
        
    // fprintf(fp,"%7.5f %7.5f %7.5f %7.5f %7.5f %7.5f %2d ",
    //    _alpha[i],_bdryPlusArray[i].x(),_bdryPlusArray[i].y(),
    //    _bdryMinusArray[i].x(),_bdryMinusArray[i].y(),_angle[i],_shockDir[i]);

    fs >> dummy; alpha.push_back(dummy);
    fs >> x;
    fs >> y;  
    vgl_point_2d<double> pp(x, y);   
    bdry_plus.push_back(pp);
    fs >> x;
    fs >> y;  
    vgl_point_2d<double> pm(x, y);   
    bdry_minus.push_back(pm);
    fs >> dummy; angle.push_back(dummy);
    int dir;
    fs >> dir; shock_dir.push_back(dir);
  
    //  fprintf(fp,"%7.5f %7.5f %7.5f %7.5f \n",
    //    _bdryPlusArcLength[i],_bdryMinusArcLength[i],
    //    _bdryPlusAngle[i],_bdryMinusAngle[i]);
    fs >> dummy; bdry_plus_arclength.push_back(dummy);
    fs >> dummy; bdry_minus_arclength.push_back(dummy);
    fs >> dummy; bdry_plus_angle.push_back(dummy);
    fs >> dummy; bdry_minus_angle.push_back(dummy);
  }

  return size;
}

//this is a very crude function to construct a shock path from ids
//This information will be compiled by a shock tree (in the real case)
void get_path_from_ids (dbsk2d_shock_graph_sptr shock_graph, 
                        dbsk2d_shock_node_sptr & start_node,
                        vcl_vector<dbsk2d_shock_edge_sptr> & path1, 
                        int start_node_id,
                        int edge_ids[], int num_of_edges)
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

bool compare_curves_pt_by_pt(dbskr_scurve_sptr c1, dbskr_scurve_sptr c2)
{
  bool equivalent = true;
  equivalent = (c1->num_points() == c2->num_points()); //same number of sample points?

  double error = 0;

  //go through them to compare them point by point
  for (int i=0; i < c1->num_points(); i++)
  {
    if (i >= c2->num_points())
      break;

    //vcl_cout << "Sh pt: (" << c1->sh_pt(i).x() << ", " << c1->sh_pt(i).y() << ")";
    //vcl_cout << "(" << c2->sh_pt(i).x() << ", " << c2->sh_pt(i).y() << ")" << vcl_endl;

    //vcl_cout << "B+ pt: (" << c1->bdry_plus_pt(i).x() << ", " << c1->bdry_plus_pt(i).y() << ")";
    //vcl_cout << "(" << c2->bdry_plus_pt(i).x() << ", " << c2->bdry_plus_pt(i).y() << ")" << vcl_endl;

    //vcl_cout << "B- pt: (" << c1->bdry_minus_pt(i).x() << ", " << c1->bdry_minus_pt(i).y() << ")";
    //vcl_cout << "(" << c2->bdry_minus_pt(i).x() << ", " << c2->bdry_minus_pt(i).y() << ")" << vcl_endl;

    //vcl_cout << "Radiu: " << c1->time(i) << ", " << c2->time(i) << vcl_endl;
    //vcl_cout << "alpha: " << c1->alpha(i) << ", " << c2->alpha(i) << vcl_endl;
    //vcl_cout << "S-dir: " << c1->shock_dir(i) << ", " << c2->shock_dir(i) << vcl_endl;

    //since the sampling is slightly different, true matching can only be done via curve matching!
    error += vgl_distance(c1->sh_pt(i), c2->sh_pt(i)) + 
             vgl_distance(c1->bdry_plus_pt(i), c2->bdry_plus_pt(i)) +
             vgl_distance(c1->bdry_minus_pt(i), c2->bdry_minus_pt(i));
  }

  if (error/c1->num_points() > 0.1) //on average points were within 0.1 of each other
    equivalent = false;

  return equivalent;
}

double compute_deform_cost(dbskr_scurve_sptr c1, dbskr_scurve_sptr c2)
{
  //compare curves by performing curve matching instead

  vcl_vector <double> lambda(3, 1.0);
  double R = 6.0;

  //call the shock curve matching code
  dbskr_dpmatch d(c1, c2, R, lambda, 3);
  d.Match();

  double cost = d.finalCost();
  
  return cost;
}

bool compare_curves(dbskr_scurve_sptr c1, dbskr_scurve_sptr c2)
{
  double cost = compute_deform_cost(c1, c2);
  
  return cost < 0.01; //is it a close match despite the sampling differences?
}

MAIN_ARGS(test_scurve)
{

  vcl_string dir_base;

  if ( argc >= 2 ) {
    dir_base = argv[1];
#ifdef VCL_WIN32
    dir_base += "\\";
#else
    dir_base += "/";
#endif
  }

  testlib_test_start("testing scurve class ");

  //1) load the esf files
  vcl_string esf_file1 = "data/brk001.esf";
  vcl_string esf_file2 = "data/pln006.esf";

  //the esf file I/O class
  dbsk2d_xshock_graph_fileio file_io; 
  dbsk2d_shock_graph_sptr xsg1 = file_io.load_xshock_graph(dir_base+esf_file1);
  //dbsk2d_shock_graph_sptr xsg1 = file_io.load_xshock_graph(dir_base+esf_file2);

  TEST("load esf file1", (xsg1==0), false);

  //2) construct a scurve from the shock graph for a given path
  //
  // path1 = brk001-0-1 := path(1203(N), <164, 165, 113, 114, 107, 106, 96, 97, 73, 72, 66, 67, 38, 39>
  // path1 = brk001-0-1-2 := path(1203(N), <164, 165, 113, 114, 107, 106, 96, 97, 73, 72, 66, 67, 38, 39, 979>
  int edge_ids1[] = {164, 165, 113, 114, 107, 106, 96, 97, 73, 72, 66, 67, 38, 39};
  int edge_ids2[] = {164, 165, 113, 114, 107, 106, 96, 97, 73, 72, 66, 67, 38, 39, 979};

  dbsk2d_shock_node_sptr start_node1, start_node2;
  vcl_vector<dbsk2d_shock_edge_sptr> path1, path2;
  dbskr_scurve_sptr sk_path_curve1, sk_path_curve2;

  if (xsg1){
    //path1
    //a)construct the simulated path from these ids
    get_path_from_ids (xsg1, start_node1, path1, 1203, edge_ids1, 14);

    //b)get the shock curve for this path
    sk_path_curve1 = dbskr_compute_scurve(start_node1, path1, true);

    //path2
    //a)construct the simulated path from these ids
    get_path_from_ids (xsg1, start_node2, path2, 1203, edge_ids2, 15);

    //b)get the shock curve for this path
    sk_path_curve2 = dbskr_compute_scurve(start_node2, path2, true);
  }
  TEST("compute path1 from esf file1", sk_path_curve1->num_points()>0, true);
  TEST("compute path2 from esf file1", sk_path_curve2->num_points()>0, true);

  //3) read a corresponding manually generated scurve from the file
  vcl_string curve_file1 = "data/brk001-pln006-0-1-vs-0-2-5-curves.txt";
  vcl_string curve_file2 = "data/brk001-pln006-0-1-2-vs-0-2-5-curves.txt";

  vcl_vector<double> time, alpha;
  vcl_vector <int> shock_dir;
  int num_points;
  vcl_vector< vgl_point_2d<double> > bdry_plus, bdry_minus, pt_array;
  vcl_vector<double> bdry_plus_arclength, bdry_plus_angle, bdry_minus_arclength, bdry_minus_angle;
  vcl_vector<double> angle, velocity, curvature;

  //open file1
  vcl_ifstream cf1((dir_base+curve_file1).c_str());
  TEST("file1 open", cf1.is_open(), true);
  //read curve1
  num_points = read_curve_file2(cf1, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
                               
  TEST("file1 read", int(bdry_plus.size()), num_points); 
  //construct curve 
  dbskr_scurve_sptr c1 = new dbskr_scurve(num_points, pt_array, time, angle, alpha);
  //close file1
  cf1.close();

  //clear these vectors first
  time.clear();
  alpha.clear();
  shock_dir.clear();
  bdry_plus.clear(); 
  bdry_minus.clear(); 
  pt_array.clear();

  //open file2
  vcl_ifstream cf2((dir_base+curve_file2).c_str());
  TEST("file2 open ", cf2.is_open(), true);

  //read curve2
  num_points = read_curve_file2(cf2, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
                               
  TEST("file2 read", int(bdry_plus.size()), num_points); 
  //construct curve 
  dbskr_scurve_sptr c2 = new dbskr_scurve(num_points, pt_array, time, angle, alpha, false);
  //close file2
  cf2.close();

  //4) compare the two curves point by point (this is unlikely to succeed)

  //write out the generated curve on to a file
  //c1->writeData("curve1-man.txt");
  //sk_path_curve1->writeData("curve1-gen.txt");

  TEST("Comparing curves 1 (pt-by-pt)", compare_curves_pt_by_pt(sk_path_curve1, c1), true);
  TEST("Comparing curves 2 (pt-by-pt)", compare_curves_pt_by_pt(sk_path_curve2, c2), true);

  TEST("Comparing curves 1 (by scvmatch)", compare_curves(sk_path_curve1, c1), true);
  TEST("Comparing curves 2 (by scvmatch)", compare_curves(sk_path_curve2, c2), true);
/*
  -------------------------------------------------------------------------
   comparing splice cost to shock matching cost
  
    A .                       
       \                      
        \                     
         \                    
        C .------------------. D
         /                    \
        /                      \
       /                        \
    B .                          . E
  
   Due to symmetry, the cost of splicing edge AC should be the same as 
   deforming AC to DD (using rect2.esf)
  
  -------------------------------------------------------------------------
*/
  //1) load the esf files
  vcl_string esf_file3 = "data/rect2.esf";
  dbsk2d_shock_graph_sptr xsg2 = file_io.load_xshock_graph(dir_base+esf_file3);

  //2) construct a scurve from the shock graph for a given path
  int edge_ids3[] = {22};
  int edge_ids4[] = {24};

  dbsk2d_shock_node_sptr start_node3, start_node3p, start_node4;
  vcl_vector<dbsk2d_shock_edge_sptr> path3, path3p, path4;
  dbskr_scurve_sptr sk_path_curve3, sk_path_curve3p, sk_path_curve4;

  if (xsg2){
    //path3
    //a)construct the simulated path from these ids
    get_path_from_ids (xsg2, start_node3, path3, 18, edge_ids3, 1);
    get_path_from_ids (xsg2, start_node3p, path3p, 18, edge_ids3, 1);

    //b)get the shock curve for this path
    sk_path_curve3 = dbskr_compute_scurve(start_node3, path3, true);
    sk_path_curve3p = dbskr_compute_scurve(start_node3p, path3p, true);

    //path4
    //a)construct the simulated path from these ids
    get_path_from_ids (xsg2, start_node4, path4, 20, edge_ids4, 1);

    //b)get the shock curve for this path
    sk_path_curve4 = dbskr_compute_scurve(start_node4, path4, true);
  }

  double splice_cost = 0; //sk_path_curve3->splice_cost(true, true, true, true);
  // commented out to fix compile error.  Should be fixed by somebody who understands it. -dec
  double def_cost1 = compute_deform_cost(sk_path_curve3, sk_path_curve4);
  double def_cost2 = compute_deform_cost(sk_path_curve3p, sk_path_curve4);

  TEST_NEAR("Comparing splice vs deform to splice cost", splice_cost/2, def_cost1, 0.1);
  TEST_NEAR("Comparing splice vs deform to splice cost", splice_cost/2, def_cost2, 0.1);

  //-------------------------------------------------------------------------
  // Computing match cost between a rect and a quad path
  //-------------------------------------------------------------------------

  //1) load the esf files
  vcl_string esf_file4 = "data/rect.esf";
  vcl_string esf_file5 = "data/quad.esf";

  dbsk2d_shock_graph_sptr xsg3 = file_io.load_xshock_graph(dir_base+esf_file4);
  dbsk2d_shock_graph_sptr xsg4 = file_io.load_xshock_graph(dir_base+esf_file5);

  //2) construct a scurve from the shock graph for a given path
  int edge_ids5[] = {21, 27, 28, 22};
  int edge_ids6[] = {22, 26, 21};

  dbsk2d_shock_node_sptr start_node5, start_node6;
  vcl_vector<dbsk2d_shock_edge_sptr> path5, path6;
  dbskr_scurve_sptr sk_path_curve5, sk_path_curve6;

  if (xsg3 && xsg4){
    //path3
    //a)construct the simulated path from these ids
    get_path_from_ids (xsg3, start_node5, path5, 17, edge_ids5, 4);

    //b)get the shock curve for this path
    sk_path_curve5 = dbskr_compute_scurve(start_node5, path5, true);

    //path4
    //a)construct the simulated path from these ids
    get_path_from_ids (xsg4, start_node6, path6, 18, edge_ids6, 3);

    //b)get the shock curve for this path
    sk_path_curve6 = dbskr_compute_scurve(start_node6, path6, true);
  }

  //open file3  
  vcl_string curve_file3 = "data/rect-quad-17-25-26-18-vs-18-27-25-17-curves.txt";
  vcl_ifstream cf3((dir_base+curve_file3).c_str());
  TEST("file3 open ", cf3.is_open(), true);

  //clear these vectors first
  time.clear();
  alpha.clear();
  shock_dir.clear();
  bdry_plus.clear(); 
  bdry_minus.clear(); 
  pt_array.clear();

  //read curve3
  num_points = read_curve_file2(cf3, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
                               
  //construct curve 3
  dbskr_scurve_sptr c3 = new dbskr_scurve(num_points, pt_array, time, angle, alpha, false);

  //clear these vectors first
  time.clear();
  alpha.clear();
  shock_dir.clear();
  bdry_plus.clear(); 
  bdry_minus.clear(); 
  pt_array.clear();

  //read curve4
  num_points = read_curve_file2(cf3, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
  //construct curve 4
  dbskr_scurve_sptr c4 = new dbskr_scurve(num_points, pt_array, time, angle, alpha, false);

  //close file3
  cf3.close();
  
  TEST("Comparing curves (rect) (pt-by-pt)", compare_curves_pt_by_pt(sk_path_curve5, c3), true);
  TEST("Comparing curves (quad) (pt-by-pt)", compare_curves_pt_by_pt(sk_path_curve6, c4), true);

  TEST("Comparing curves by cost (rect)", compare_curves(sk_path_curve5, c3), true);
  TEST("Comparing curves by cost (quad)", compare_curves(sk_path_curve6, c4), true);

  //write out the files for pt-wise comparison
  //c3->writeData("curve3-Thomas.txt");
  //sk_path_curve5->writeData("curve3-new.txt");
  //c4->writeData("curve4-Thomas.txt");
  //sk_path_curve6->writeData("curve4-new.txt");

  return testlib_test_summary();
}
