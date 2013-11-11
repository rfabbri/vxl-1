// This is brcv/shp/dbsk2d/pro/dbsk2d_compute_ishock_process.cxx

//:
// \file

#include "dbsk2d_compute_ishock_process.h"

#include "dbsk2d_shock_storage.h"
#include "dbsk2d_shock_storage_sptr.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>

#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>

#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>
#include <dbsk2d/algo/dbsk2d_ishock_detector.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>

#include <vul/vul_timer.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vnl/vnl_random.h>
#include <vcl_ctime.h>

#include <vgl/algo/vgl_fit_lines_2d.h>

dbsk2d_compute_ishock_process::
dbsk2d_compute_ishock_process()
{
  if (
      !parameters()->add( "# of cols" , "-num_cols" , (int)1 ) ||
      !parameters()->add( "# of rows" , "-num_rows" , (int)1 ) ||
      
      /*
      !parameters()->add( 
      "Override automatically computed partition params with" , 
      "-b_override", bool(false) ) ||
      !parameters()->add( "Xmin" , "-xmin" , (float)0.0 ) ||
      !parameters()->add( "Ymin" , "-ymin" , (float)0.0 ) ||
      !parameters()->add( "Cell Width" , "-cell_width" , (float)1000.0 ) ||
      !parameters()->add( "Cell Height" , "-cell_height" , (float)1000.0 ) ||
      */
      !parameters()->add( "Talkative during computation" , "-b_talkative", bool(true) ) ||
      !parameters()->add( "Pre-process boundary" , "-b_preprocess", bool(true) ) ||
      !parameters()->add( "Break long lines in boundary" , "-b_break_long_lines", bool(true) ) ||
      !parameters()->add( "smooth Contour" , "-b_smooth", bool(false) ) ||
      //: OZGE added:
      //  if the boundary has many collinearities, this result in many degeneracies
      //  which are not handled by the current shock computation algorithm
      //  add noise in an attempt to remove these collinearities
      !parameters()->add( "add noise to Contour" , "-b_noise", bool(false) ) ||
      !parameters()->add( "random noise radius" , "-noise_radius" , (float)0.002f ) ||
      !parameters()->add( "fit lines to Contour" , "-fit_lines", bool(true) ) ||
      !parameters()->add( "line fitting rms" , "-rms" , (float)0.05f ) ||
      !parameters()->add( "fit circular arcs to Contour" , "-fit_arcs", bool(false) ) ||
      !parameters()->add( "arc fitting rms" , "-arc_rms" , (float)0.05f ) ||
      !parameters()->add( "Initialize Only" , "-b_initonly", bool(false) ) ||
      !parameters()->add( "Prune Shocks" , "-b_prune" , bool(false) ) || 
      !parameters()->add( "Prune Threshold" , "-threshold" , (float)1.0 )  ||
      !parameters()->add( "Use Existing Ids" , "-exist_ids" , bool(false) ) ||
      !parameters()->add( "Grow Bounding Box" , "-b_growbbox" , bool(false) ) ||
      !parameters()->add( "Min x: " , "-bbox_minx" , (double)0.0 ) ||
      !parameters()->add( "Min y: " , "-bbox_miny" , (double)0.0 ) ||
      !parameters()->add( "Max x: " , "-bbox_maxx" , (double)0.0 ) ||
      !parameters()->add( "Max y: " , "-bbox_maxy" , (double)0.0 ) )

  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

dbsk2d_compute_ishock_process::
~dbsk2d_compute_ishock_process()
{
}


//: Clone the process
bpro1_process* dbsk2d_compute_ishock_process::
clone() const
{
  return new dbsk2d_compute_ishock_process(*this);
}


vcl_string dbsk2d_compute_ishock_process::
name()
{
  return "Compute Shocks";
}

vcl_vector< vcl_string > dbsk2d_compute_ishock_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "vsol2D" );
  return to_return;
}

vcl_vector< vcl_string > dbsk2d_compute_ishock_process::
get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

int dbsk2d_compute_ishock_process::
input_frames()
{
  return 1;
}

int dbsk2d_compute_ishock_process::
output_frames()
{
  return 1;
}


bool dbsk2d_compute_ishock_process::
execute()
{
  //1) instantiate a boundary class depending on the algorithm to be used
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();

  //2) get input storage classes
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  vidpro1_vsol2D_storage_sptr input_vsol;
  input_vsol.vertical_cast(input_data_[0][1]);


  //3) VSOL -> dbsk2d_boundary
  //   parse through all the vsol classes and add to the boundary
  //vcl_cout << "\nPutting VSOL objects into boundary ... ";
  bool bsmooth_contour=false;
  parameters()->get_value( "-b_smooth" , bsmooth_contour );

  //: OZGE added:
  //  if the boundary has many collinearities, this result in many degeneracies
  //  which are not handled by the current shock computation algorithm
  //  add noise in an attempt to remove these collinearities
  bool bnoise_contour=false;
  parameters()->get_value( "-b_noise" , bnoise_contour );
  float noise_radius=false;
  parameters()->get_value( "-noise_radius" , noise_radius );

  bool bfit_lines=false;
  parameters()->get_value( "-fit_lines" , bfit_lines );
  float rms=false;
  parameters()->get_value( "-rms" , rms );

  // Fit circular arcs to the contours
  bool bfit_arcs=false;
  parameters()->get_value( "-fit_arcs" , bfit_arcs );
  float arc_rms=false;
  parameters()->get_value( "-arc_rms" , arc_rms );

  bool flag=false;
  parameters()->get_value( "-exist_ids",flag);

  // timer to measure time for each step
  vul_timer timer1;

  // start timer
  timer1.mark();

  vcl_vector<int> ids_sos;

  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
  {
      ids_sos.push_back(vsol_list[b]->get_id());

    //POINT
    if( vsol_list[b]->cast_to_point() ) {
      boundary->add_a_point(vsol_list[b]->cast_to_point()); 
    }
    else if( vsol_list[b]->cast_to_curve())
    {
      //LINE
      if( vsol_list[b]->cast_to_curve()->cast_to_line() )
      {
        boundary->add_a_line(vsol_list[b]->cast_to_curve()->cast_to_line());
      }
      //POLYLINE
      else if( vsol_list[b]->cast_to_curve()->cast_to_polyline() ) {
        if (bnoise_contour)
          boundary->add_a_polyline(add_noise_to_contour(
            vsol_list[b]->cast_to_curve()->cast_to_polyline(), noise_radius));
        else if (bfit_lines) 
          boundary->add_a_polyline(fit_lines_to_contour(
            vsol_list[b]->cast_to_curve()->cast_to_polyline(), rms));
        else
          boundary->add_a_polyline(vsol_list[b]->cast_to_curve()->cast_to_polyline());
      }
      // CIRCULAR ARC
      else if (vsol_list[b]->cast_to_curve()->cast_to_conic())
      {
        vsol_conic_2d_sptr conic = vsol_list[b]->cast_to_curve()->cast_to_conic();
        if (conic->is_real_circle())
        {
          vgl_point_2d<double > arc_p1 = conic->p0()->get_p();
          vgl_point_2d<double > arc_p2 = conic->p1()->get_p();
          vgl_point_2d<double > arc_center = conic->centre();
          double arc_k = 1/(arc_center-arc_p1).length();
          // determine sign of curvature
          if (cross_product<double >(arc_p1-arc_center, arc_p2-arc_center) <0)
          {
            arc_k = -arc_k;
          }

          boundary->add_an_arc(arc_p1, arc_p2, arc_k);
        }
        
      }
    }
    else if( vsol_list[b]->cast_to_region())
    {
      //POLYGON
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() ){

        if (bsmooth_contour)
          boundary->add_a_polygon(smooth_closed_contour(
            vsol_list[b]->cast_to_region()->cast_to_polygon()));
        else if (bnoise_contour)
          boundary->add_a_polygon(add_noise_to_contour(
            vsol_list[b]->cast_to_region()->cast_to_polygon(), noise_radius));
        else if (bfit_lines) 
          boundary->add_a_polygon(fit_lines_to_contour(
            vsol_list[b]->cast_to_region()->cast_to_polygon(), rms));
        else
          boundary->add_a_polygon(vsol_list[b]->cast_to_region()->cast_to_polygon());
      }
    }
  }

  //unused double vsol2boundary = (float)(timer1.real())/1000;
  //vcl_cout << "done. Time taken= " << vsol2boundary << "seconds\n";
  
  
  //4) Partition the boundary elements into cells
  //vcl_cout << "Partitioning boundary into cells...";

  float xmin=0, ymin=0, cell_width=1000.0f, cell_height=1000.0f;
  int num_rows=1, num_cols=1;

  // retrieve parameter values
  parameters()->get_value( "-num_rows" , num_rows );
  parameters()->get_value( "-num_cols" , num_cols );

  //bool override_default = true;
  //parameters()->get_value( "-b_override" , override_default );

  bool talkative = false;
  parameters()->get_value( "-b_talkative" , talkative );

  bool grow_bbox = false;
  parameters()->get_value( "-b_growbbox", grow_bbox);

  double minx,miny,maxx,maxy;
  parameters()->get_value( "-bbox_minx", minx);
  parameters()->get_value( "-bbox_miny", miny);
  parameters()->get_value( "-bbox_maxx", maxx);
  parameters()->get_value( "-bbox_maxy", maxy);

  /*if (override_default)
  {
    parameters()->get_value( "-xmin" , xmin );
    parameters()->get_value( "-ymin" , ymin );
    parameters()->get_value( "-cell_width" , cell_width );
    parameters()->get_value( "-cell_height" , cell_height );
  }
  else
  {
    */
  vsol_box_2d_sptr box = boundary->get_bounding_box();
  if ( grow_bbox )
  {
      box->add_point(minx,miny);
      box->add_point(maxx,maxy);
  }

  cell_width = (float)( box->width()/(num_cols-0.5) );
  cell_height = (float)( box->height()/(num_rows-0.5) );
  xmin = (float)(box->get_min_x() - cell_width/4);
  ymin = (float)(box->get_min_y() - cell_height/4);
  //}

  // set partitioning parameters
  timer1.mark();
  boundary->set_partition_params(xmin, ymin, num_rows, num_cols, cell_height, cell_width);
  //boundary->print_partition_summary();

  // Partition the boundary
  boundary->partition_into_cells(true, talkative, dbsk2d_bnd_preprocess::distance_tol);

  //unused double partition_time = (float)(timer1.real())/1000;

  //vcl_cout << "done. Time taken= " << partition_time << "seconds\n";
  

  //5) Preprocess the boundary to build topology
  bool preprocess_boundary=false;
  parameters()->get_value( "-b_preprocess" , preprocess_boundary );

  double preprocess_time = -1;
  //unused double verify_time = -1;
    

  dbsk2d_bnd_preprocess bnd_preprocessor;
  if (preprocess_boundary){

    //vcl_cout << "Preprocessing boundary ... ";
  
    timer1.mark();
    bnd_preprocessor.preprocess(boundary, talkative);

    preprocess_time = (float)(timer1.real())/1000;
    vcl_cout << "done. Time taken= " << preprocess_time << "seconds\n";
  



  // validate boundary preprocessing 
    vcl_cout << "Verifying boundary preprocessing .. " ;
    timer1.mark();
    bool need_preprocess = bnd_preprocessor.need_preprocessing(boundary);

    //verify_time = (float)(timer1.real())/1000;
    //vcl_cout << "done. Time taken= " << verify_time << "seconds\n";

    vcl_string result = (need_preprocess) ? "Yes" : "No";
    vcl_cout <<"Boundary needs preprocessing " << result << vcl_endl;

    
    if (need_preprocess) 
      return false;
  }


  //6) Break long lines that extend beyond one cell into small segments
  bool break_long_lines=false;
  double time_to_break_long_lines = -1;
  parameters()->get_value( "-b_break_long_lines" , break_long_lines );

  if (break_long_lines)
  {
    timer1.mark();
    boundary->break_long_line_edges(dbsk2d_bnd_preprocess::distance_tol);
    time_to_break_long_lines = (float)(timer1.real())/1000;
    //vcl_cout << "done. Time taken= " << time_to_break_long_lines
    //  << "seconds\n";
    boundary->break_long_arc_edges(dbsk2d_bnd_preprocess::distance_tol);
  }

  //after everything is converted, compile the belm_list
  boundary->update_belm_list();
  //vcl_cout << "Number of edges = " << boundary->all_edges().size() << vcl_endl;
  //vcl_cout << "Number of belms = " << boundary->num_belms() << vcl_endl;
  //boundary->print_belm_list(vcl_cout);


  // Redo ids if flag is up
  if ( flag)
  {

      vcl_list<dbsk2d_bnd_edge_sptr > edges = boundary->all_edges();
      vcl_map<int,vcl_vector<vtol_topology_object*> > superior_map;

      vcl_list<dbsk2d_bnd_edge_sptr>::iterator it;
      for ( it = edges.begin() ; it != edges.end() ; ++it )
      {
          const vcl_list<vtol_topology_object*>* contour_list= 
              (*it)->superiors_list();

          vcl_list<vtol_topology_object*>::const_iterator it2;
          for ( it2 = contour_list->begin() ; it2 != contour_list->end();
                ++it2 )
          {
              superior_map[(*it2)->get_id()].push_back(*it2);
          }
   
      }

      // vcl_cout<<"Map length, equivalent to numb contours: "
      //         <<superior_map.size()<<" cons "<<ids_sos.size()
      //         <<vcl_endl;

      vcl_map<int,vcl_vector<vtol_topology_object*> >::iterator mit;

      for ( mit = superior_map.begin() ; mit != superior_map.end() ; ++mit)
      {

          vcl_vector<vtol_topology_object*> segs = (*mit).second;

          //   vcl_cout<<"Length of superiors: "<<segs.size()<<vcl_endl;
          for ( unsigned int i=0 ; i <segs.size() ; ++i)
          {
              segs[i]->set_id(ids_sos.front());
              

          }
          ids_sos.erase(ids_sos.begin());
      }
      
  }

/*
  vcl_cout << "summary: \n";
  vcl_cout << "\nPut VSOL objects into boundary ......... " << vsol2boundary << " seconds.\n";
  vcl_cout << "Partition boundary into cells........... " << partition_time << " seconds.\n";
  vcl_cout << "Preprocess boundary ....................  "<< preprocess_time << " seconds.\n";
  vcl_cout << "Break long lines .......................  " << 
    time_to_break_long_lines << " seconds.\n";

  vcl_cout << "Verify boundary preprocessing ........... ";

  if (verify_time < 0) 
  {
    vcl_cout << "(not run)";
  }
  else
  {
    vcl_cout << verify_time ;
  }
  vcl_cout << "seconds\n";
*/

  //7) Instantiate the shock detector according to the selected algorithm
  dbsk2d_ishock_detector* sh_det = new dbsk2d_ishock_detector(boundary);
  //dbsk2d_ishock_detector* sh_det=NULL;
  //
  //if (ishockinit == "DEFAULT_INIT" || ishockinit == "DYNVAL_INIT") {
  //  sh_det = new dbsk2d_ishock_detector(boundary);
  //}
  //else if (ishockinit == "BUCKETING_INIT") {
  //  sh_det = new dbsk2d_ishock_detector_bkt(dbsk2d_ishock_boundary_bkt_sptr((dbsk2d_ishock_boundary_bkt*)boundary.ptr()));
  //}

  //8) Detect Shocks from this boundary
  bool init_only=false;
  parameters()->get_value( "-b_initonly" , init_only );

  bool shock_computation_valid=false;
  if (init_only){
    sh_det->initialize_shocks();//shock_init_type
  }
  else {
    sh_det->detect_shocks();//shock_init_type
    //validate the shock computation
    shock_computation_valid = sh_det->validate_shocks();
  }

  //update the extrinsic points so that they are displayed properly
  sh_det->ishock_graph()->update_shocks();
  
  //output a summary information of the shocks
  #ifdef DEBUG_SHOCK_VERBOSE
  sh_det->ishock_graph()->print_summary(vcl_cout);
  #endif

  //9) Prune the shock graph
  float prune_threshold=false;
  bool prune_shocks=false;
  parameters()->get_value( "-b_prune" , prune_shocks );
  parameters()->get_value( "-threshold" , prune_threshold );

  //instantiate an empty coarse shock graph at this time
  //this will be properly defined once the shock is pruned
  dbsk2d_shock_graph_sptr coarse_ishock = new dbsk2d_shock_graph();

  if (prune_shocks && shock_computation_valid){
    //prune this shock graph and output a coarse shock graph 
    //corresponding to the remaining shock edges
    dbsk2d_prune_ishock ishock_pruner(sh_det->ishock_graph(), coarse_ishock);
    ishock_pruner.prune(prune_threshold);
    ishock_pruner.compile_coarse_shock_graph();
  }

  //10) create the output shock storage class
  dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
  if (frame_image){
    vil_image_resource_sptr image_sptr = frame_image->get_image();
    output_shock->set_image(image_sptr);
  }
  output_shock->set_boundary(boundary);
  output_shock->set_ishock_graph(sh_det->ishock_graph());
  output_shock->set_shock_graph(coarse_ishock);

  //11) output the shock storage class
  clear_output();
  output_data_[0].push_back(output_shock);

  delete sh_det;

  return shock_computation_valid;
}

bool
dbsk2d_compute_ishock_process::finish()
{
  return true;
}

vsol_polygon_2d_sptr dbsk2d_compute_ishock_process::
smooth_closed_contour(vsol_polygon_2d_sptr polygon)
{
  vcl_vector<vgl_point_2d<float> > c;
  for (unsigned i=0; i<polygon->size(); i++)
    c.push_back(vgl_point_2d<float>((float)polygon->vertex(i)->x(), 
                                    (float)polygon->vertex(i)->y()));

  double psi=1.0;
  double nsteps = 10;
  int n = c.size();
  
  vcl_vector<vgl_point_2d<float> > cs;
  cs.resize(n);

  // fix endpts 
  cs[0] = c[0];
  cs[n-1] = c[n-1];

  for (int si=0; si<nsteps; si++){
    for (int i=1; i<n-1; i++){
      vgl_vector_2d<float> v1 = c[i-1] - c[i];
      vgl_vector_2d<float> v2 = c[i+1] - c[i];

      double nv1 = vcl_sqrt(v1.x()*v1.x() + v1.y()*v1.y());
      double nv2 = vcl_sqrt(v2.x()*v2.x() + v2.y()*v2.y());

      vgl_vector_2d<float> v1n = v1/nv1;
      vgl_vector_2d<float> v2n = v2/nv2;

      vgl_vector_2d<float> nrm;

      if (nv1 < nv2)
        nrm = (v1 + nv1*v2n)/4;
      else
        nrm = (v2 + nv2*v1n)/4;

      cs[i] = c[i] + psi*nrm;
    }
  }

  vcl_vector<vsol_point_2d_sptr > smoothed_pts;
  for (int i=0; i<n; i++)
    smoothed_pts.push_back(new vsol_point_2d(cs[i].x(), cs[i].y()));

  return new vsol_polygon_2d(smoothed_pts);
}

vsol_polygon_2d_sptr dbsk2d_compute_ishock_process::
add_noise_to_contour(vsol_polygon_2d_sptr polygon, double noise_radius)
{
  vcl_vector<vsol_point_2d_sptr > noisy_pts;
  vnl_random mz_random;
  mz_random.reseed((unsigned long)time(NULL));
  for (unsigned i=0; i<polygon->size(); i++) {
    double x = polygon->vertex(i)->x();
    double y = polygon->vertex(i)->y();
    double rand_x = mz_random.drand32(1.0);
    x += 2.0*noise_radius*(rand_x-0.5);
    double rand_y = mz_random.drand32(1.0);
    y += 2.0*noise_radius*(rand_y-0.5);
    noisy_pts.push_back(new vsol_point_2d(x, y));
  }
  
  return new vsol_polygon_2d(noisy_pts);
}

vsol_polyline_2d_sptr dbsk2d_compute_ishock_process::
add_noise_to_contour(vsol_polyline_2d_sptr poly, double noise_radius)
{
  vcl_vector<vsol_point_2d_sptr > noisy_pts;
  vnl_random mz_random;
  mz_random.reseed((unsigned long)time(NULL));
  for (unsigned i=0; i<poly->size(); i++) {
    double x = poly->vertex(i)->x();
    double y = poly->vertex(i)->y();
    double rand_x = mz_random.drand32(1.0);
    x += 2.0*noise_radius*(rand_x-0.5);
    double rand_y = mz_random.drand32(1.0);
    y += 2.0*noise_radius*(rand_y-0.5);
    noisy_pts.push_back(new vsol_point_2d(x, y));
  }
  
  return new vsol_polyline_2d(noisy_pts);
}
vsol_polyline_2d_sptr dbsk2d_compute_ishock_process::
fit_lines_to_contour(vsol_polyline_2d_sptr poly, double rms) {
 if (rms > 0) {  // vgl_fit
    int min_fit_length = 2;
    vgl_fit_lines_2d<double> fitter; 
    fitter.set_min_fit_length(min_fit_length);
    fitter.set_rms_error_tol(rms);
    vcl_cout << "original polyline size: " << poly->size() << " ";
    for (unsigned int i = 0; i<poly->size(); i++) {
      vgl_point_2d<double> p = poly->vertex(i)->get_p();
      fitter.add_point(p);
    }
    fitter.fit();
    vcl_vector<vgl_line_segment_2d<double> >& segs = fitter.get_line_segs();

    vcl_vector<vsol_point_2d_sptr > new_pts;
    new_pts.push_back(new vsol_point_2d(segs[0].point1().x(),segs[0].point1().y()));
    new_pts.push_back(new vsol_point_2d(segs[0].point2().x(),segs[0].point2().y()));
    for (unsigned int i = 1; i<segs.size(); i++) {
      new_pts.push_back(new vsol_point_2d(segs[i].point2().x(),segs[i].point2().y()));
    }
    vcl_cout << "fitted polyline size: " << new_pts.size() << vcl_endl;
    return new vsol_polyline_2d(new_pts);
 } else
   return 0;
}
vsol_polygon_2d_sptr dbsk2d_compute_ishock_process::
fit_lines_to_contour(vsol_polygon_2d_sptr poly, double rms) {
 if (rms > 0) {  // vgl_fit
    int min_fit_length = 2;
    vgl_fit_lines_2d<double> fitter; 
    fitter.set_min_fit_length(min_fit_length);
    fitter.set_rms_error_tol(rms);
    vcl_cout << "original polygon size: " << poly->size() << " ";
    for (unsigned int i = 0; i<poly->size(); i++) {
      vgl_point_2d<double> p = poly->vertex(i)->get_p();
      fitter.add_point(p);
    }
    fitter.fit();
    vcl_vector<vgl_line_segment_2d<double> >& segs = fitter.get_line_segs();

    vcl_vector<vsol_point_2d_sptr > new_pts;
    new_pts.push_back(new vsol_point_2d(segs[0].point1().x(),segs[0].point1().y()));
    new_pts.push_back(new vsol_point_2d(segs[0].point2().x(),segs[0].point2().y()));
    for (unsigned int i = 1; i<segs.size(); i++) {
      new_pts.push_back(new vsol_point_2d(segs[i].point2().x(),segs[i].point2().y()));
    }
    vcl_cout << "fitted polygon size: " << new_pts.size() << vcl_endl;
    return new vsol_polygon_2d(new_pts);
 } else
   return 0;
}




