#include "dbcvr_mutual_info_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/algo/dbsol_geno.h>

#include <vil/vil_convert.h>
//required because strk still uses vil1
#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_crop.h>
#include <vil/vil_new.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <brip/brip_vil1_float_ops.h>
#include <brip/brip_vil_float_ops.h>

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_intensity_face.h>
#include <btol/btol_face_algs.h>

#include <strk/strk_tracking_face_2d.h>
#include <strk/strk_tracking_face_2d_sptr.h>

#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/algo/vgl_fit_lines_2d.h>

#include <dbru/dbru_rcor.h>
#include <dbcvr/dbcvr_clsd_cvmatch_even_sptr.h>
#include <dbcvr/dbcvr_clsd_cvmatch_even.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>

#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_tree_edit.h>

#include <bpro1/bpro1_process_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>

#define MARGIN  (10)

dbcvr_mutual_info_process::dbcvr_mutual_info_process()
{  
  if (
      !parameters()->add( "Smoothing sigma" , "-sigma" , 1.0f )  ||
      !parameters()->add( "R: " , "-R" , 10.0f ) ||
      !parameters()->add( "Shock matching: ", "-shock", false) ||
      !parameters()->add( "Thomas curve matching: " , "-thomas" , true) ||
      !parameters()->add( "Even grid curve matching): " , "-even" , false) ||
      !parameters()->add( "If even grid matching, geno interpolation (otherwise linear): " , "-linear" , false) ||
      !parameters()->add( "increment: " , "-increment" , 20) ||
      !parameters()->add( "Camera image pixel range (in bits): " , "-imagebits" , 8) ||
      !parameters()->add( "Use line intersections for region correspondence? (if not shock matching):" , "-line" , true )  ||
      !parameters()->add( "Use distance transform region correspondence? (if not shock matching):" , "-disttrans" , false )  ||
      !parameters()->add( "lamda for dt: " , "-lambda" , 1.0f ) 
      ) {
    vcl_cerr << "ERROR: Adding parameters in dbcvr_mutual_info_process::dbcvr_mutual_info_process()" << vcl_endl;
  }

  total_info_ = 0;
  int_mutual_info_ = 0;
  grad_mutual_info_ = 0;
  color_mutual_info_ = 0;
}


//: Clone the process
bpro1_process*
dbcvr_mutual_info_process::clone() const
{
  return new dbcvr_mutual_info_process(*this);
}

bool dbcvr_mutual_info_process::execute()
{
  float sigma;
  parameters()->get_value( "-sigma" , sigma );
  float R;
  parameters()->get_value( "-R" , R );
  bool shock_matching;
  parameters()->get_value( "-shock" , shock_matching );
  bool line_intersections;
  parameters()->get_value( "-line" , line_intersections );
  bool distance_transform;
  parameters()->get_value( "-disttrans" , distance_transform );
  bool thomas_matching;
  parameters()->get_value( "-thomas" , thomas_matching );
  bool even_matching;
  parameters()->get_value( "-even" , even_matching );
  bool geno_interpolation;
  parameters()->get_value( "-linear" , geno_interpolation );
  float lambda;
  parameters()->get_value("-lambda", lambda);
  int increment;
  parameters()->get_value("-increment", increment);
  //vcl_cout << "increment value is: " << increment << vcl_endl;

  int image_bits;
  parameters()->get_value("-imagebits", image_bits);
  float max_value = float(vcl_pow(double(2.0), double(image_bits))-1);

  clear_output();

  //: get input vsol 
  vidpro1_vsol2D_storage_sptr input_vsol1;
  //input_vsol1.vertical_cast(input_data_[0][0]);
  input_vsol1.vertical_cast(input_data_[0][0]);

  vidpro1_vsol2D_storage_sptr input_vsol2;
  //input_vsol2.vertical_cast(input_data_[1][0]);
  input_vsol2.vertical_cast(input_data_[0][2]);

  

  vcl_vector<vsol_point_2d_sptr> inp1, inp2;

  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol1->all_data();
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = new vsol_point_2d(vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i)->get_p());
          inp1.push_back(pt);
    } } }

    if( vsol_list[b]->cast_to_region())
    {
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = new vsol_point_2d(vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i)->get_p());
          inp1.push_back(pt);
  } } } }

  vsol_list = input_vsol2->all_data();
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = new vsol_point_2d(vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i)->get_p());
          inp2.push_back(pt);
    } } }

    if( vsol_list[b]->cast_to_region())
    {
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = new vsol_point_2d(vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i)->get_p());
          inp2.push_back(pt);
  } } } }
  
  //: find the best cropped image size and location
  vsol_polygon_2d_sptr poly1 = new vsol_polygon_2d(inp1);
  vsol_polygon_2d_sptr poly2 = new vsol_polygon_2d(inp2);

  poly1->compute_bounding_box();
  poly2->compute_bounding_box();
  
  double topx1, topy1, lenx1, leny1, topx2, topy2, lenx2, leny2;
  topx1 = poly1->get_min_x();
  topy1 = poly1->get_min_y();
  lenx1 = (poly1->get_max_x() - topx1);
  leny1 = (poly1->get_max_y() - topy1);
  
  topx2 = poly2->get_min_x();
  topy2 = poly2->get_min_y();
  lenx2 = (poly2->get_max_x() - topx2);
  leny2 = (poly2->get_max_y() - topy2);

  dbsk2d_shock_graph_sptr sg0;
  dbsk2d_shock_graph_sptr sg1;

  if (shock_matching) {
  
  // SHOCK PROCESSING /////////////////////////////////////////////////////////////////////////////
  //: get xshock from .con files
  vidpro1_process_manager process_manager;
  bpro1_process_sptr get_shock = new dbsk2d_compute_ishock_process;
  process_manager.register_process(get_shock);
  vidpro1_repository_sptr rep = process_manager.get_repository_sptr();
  rep->initialize(1);
    
  //: register storage classes
  dbsk2d_shock_storage_sptr dummy1 = new dbsk2d_shock_storage();
  dummy1->register_binary_io(); 
  rep->register_type( dummy1 ); 

  vidpro1_vsol2D_storage_sptr dummy2 = new vidpro1_vsol2D_storage();
  dummy2->register_binary_io(); 
  rep->register_type( dummy2 ); 

  //: prepare the repository
  vcl_vector< vsol_spatial_object_2d_sptr > contour1, contour2;
  contour1.push_back(poly1->cast_to_spatial_object());
  contour2.push_back(poly2->cast_to_spatial_object());
  vidpro1_vsol2D_storage_sptr vsol1 = vidpro1_vsol2D_storage_new();
  vidpro1_vsol2D_storage_sptr vsol2 = vidpro1_vsol2D_storage_new();
  vsol1->add_objects(contour1, "poly1");
  vsol1->set_name("vsol2D1");
  if (!rep->store_data(vsol1)) {
    vcl_cout << "Problems in adding contour1 into repository as vsol2D\n";
    return 0;
  }
  vsol2->add_objects(contour2, "poly2");
  vsol2->set_name("vsol2D2");
  if (!rep->store_data(vsol2)) {
    vcl_cout << "Problems in adding contour2 into repository as vsol2D\n";
    return 0;
  }

  //: set the defaults
  get_shock->parameters()->set_value("-num_cols",(int)1);
  get_shock->parameters()->set_value("-num_rows",(int)1);
  get_shock->parameters()->set_value("-b_override", bool(false));
  get_shock->parameters()->set_value("-xmin" , (float)0.0 );
  get_shock->parameters()->set_value("-ymin" , (float)0.0 );
  get_shock->parameters()->set_value("-cell_width" , (float)1000.0 );
  get_shock->parameters()->set_value("-cell_height" , (float)1000.0 );
  get_shock->parameters()->set_value("-b_talkative" , bool(false) );
  get_shock->parameters()->set_value("-b_preprocess", bool(true) );
  get_shock->parameters()->set_value("-b_break_long_lines", bool(true) );
  get_shock->parameters()->set_value("-b_initonly", bool(false) );
  get_shock->parameters()->set_value("-b_prune" , bool(true) );
  get_shock->parameters()->set_value("-threshold" , (float)2.0 );
  //: TODO for AMIR: smoothing might be good but there is a bug currently
  get_shock->parameters()->set_value("-b_smooth", bool(false) );

  vcl_vector<vcl_string> input_names, output_names;
  input_names.push_back("image");
  input_names.push_back("vsol2D1");  get_shock->set_input_names(input_names);
  output_names.push_back("shock0");  get_shock->set_output_names(output_names);
  process_manager.run_process_on_current_frame(get_shock);

  input_names.clear();  
  input_names.push_back("image");
  input_names.push_back("vsol2D2"); get_shock->set_input_names(input_names);
  output_names.clear(); output_names.push_back("shock1"); get_shock->set_output_names(output_names);
  process_manager.run_process_on_current_frame(get_shock);
    
  bpro1_storage_sptr str; 
  dbsk2d_shock_storage_sptr shock_storage = new dbsk2d_shock_storage();
  str = rep->get_data(shock_storage->type(), 0, 0);
  shock_storage.vertical_cast(str);
  dbsk2d_shock_graph_sptr dummy = shock_storage->get_shock_graph();
  
  dbsk2d_sample_ishock ishock_sampler(dummy);
  ishock_sampler.sample((double)1.0);
  sg0 = ishock_sampler.extrinsic_shock_graph();

  str = rep->get_data(shock_storage->type(), 0, 1);
  shock_storage.vertical_cast(str);
  dummy = shock_storage->get_shock_graph();
  
  dbsk2d_sample_ishock ishock_sampler2(dummy);
  ishock_sampler2.sample((double)1.0);
  sg1 = ishock_sampler2.extrinsic_shock_graph();
  
  vcl_cout << " sg0 number of vertices: " << sg0->number_of_vertices() << " ";
  vcl_cout << " sg1 number of vertices: " << sg1->number_of_vertices() << vcl_endl;

  if (sg0->number_of_vertices() == 0) {
    vcl_cout << "First shock: ZERO number of vertices, skipping\n";
    total_info_ = 0;
    return 0;
  }

  if (sg1->number_of_vertices() == 0) {
    vcl_cout << "Second shock: ZERO number of vertices, skipping\n";
    total_info_ = 0;
    return 0;
  }

  }

  if (shock_matching && (!sg0 || !sg1))
  {
    vcl_cout << "Problems in getting shock graphs!\n";
    return 0;
  }

  //: get input images
  vidpro1_image_storage_sptr frame_image1;
  frame_image1.vertical_cast(input_data_[0][1]);
  vil_image_resource_sptr image1_sptr = frame_image1->get_image();

  vidpro1_image_storage_sptr frame_image2;
  frame_image2.vertical_cast(input_data_[0][3]);
  vil_image_resource_sptr image2_sptr = frame_image2->get_image();
  
  if (image1_sptr->nplanes() > 1) {
    vil_image_resource_sptr grey_image_sptr;
    grey_image_sptr = vil_new_image_resource_of_view( *vil_convert_to_grey_using_rgb_weighting ( image1_sptr->get_view() ) );
    if (grey_image_sptr->ni()==0)
    {
      vcl_cout<<"Failed to load image 1."<<vcl_endl;
      return false;
    }
    image1_sptr = grey_image_sptr;
  }
  if (image2_sptr->nplanes() > 1) {
    vil_image_resource_sptr grey_image_sptr = vil_new_image_resource_of_view( *vil_convert_to_grey_using_rgb_weighting ( image2_sptr->get_view() ) );
    if (grey_image_sptr->ni()==0)
    {
      vcl_cout<<"Failed to load image 2."<<vcl_endl;
      return false;
    }
    image2_sptr = grey_image_sptr;
  }
  
  int w = image1_sptr->ni(), h = image1_sptr->nj();
  topx1 = (topx1-MARGIN) <= 0 ? 0 : topx1 - MARGIN;
  topy1 = (topy1-MARGIN) <= 0 ? 0 : topy1 - MARGIN;
  lenx1 = (topx1+lenx1+2*MARGIN) > w ? (w-topx1-1) : (lenx1+2*MARGIN);
  leny1 = (topy1+leny1+2*MARGIN) > h ? (h-topy1-1) : (leny1+2*MARGIN);
  vil_image_resource_sptr crop_img1=vil_crop(image1_sptr,
                                             (unsigned int)vcl_floor(topx1+0.5),
                                             (unsigned int)vcl_floor(lenx1+0.5),
                                             (unsigned int)vcl_floor(topy1+0.5),
                                             (unsigned int)vcl_floor(leny1+0.5));
  
  
  w = image2_sptr->ni(); h = image2_sptr->nj();
  topx2 = (topx2-MARGIN) <= 0 ? 0 : topx2 - MARGIN;
  topy2 = (topy2-MARGIN) <= 0 ? 0 : topy2 - MARGIN;
  lenx2 = (topx2+lenx2+2*MARGIN) > w ? (w-topx2-1) : (lenx2+2*MARGIN);
  leny2 = (topy2+leny2+2*MARGIN) > h ? (h-topy2-1) : (leny2+2*MARGIN);
  vil_image_resource_sptr crop_img2 = vil_crop(image2_sptr, 
                                               (unsigned int)vcl_floor(topx2+0.5), 
                                               (unsigned int)vcl_floor(lenx2+0.5), 
                                               (unsigned int)vcl_floor(topy2+0.5), 
                                               (unsigned int)vcl_floor(leny2+0.5));

  
  vil_image_view<float> float_image1, float_image2;
  float_image1 =  brip_vil_float_ops::convert_to_float(*crop_img1);
  float_image2 =  brip_vil_float_ops::convert_to_float(*crop_img2);

  vil1_memory_image_of<float> image_0_;  //frame 0 intensity
  vil1_memory_image_of<float> image_1_;  //frame i intensity
  vil1_memory_image_of<float> Ix_0_;  //x derivative of image_0 intensity
  vil1_memory_image_of<float> Iy_0_;  //y derivative of image_0 intensity
  vil1_memory_image_of<float> Ix_1_;  //x derivative of image_i intensity
  vil1_memory_image_of<float> Iy_1_;  //y derivative of image_i intensity

  vil1_memory_image_of< float > img1 = vil1_from_vil_image_view( float_image1 );
  w = img1.width(), h = img1.height(); 
  
  //: create vtol_face_2d instances from vertices of vsol input
  vcl_vector<vtol_vertex_sptr> vert1, vert2;
  
  vgl_vector_2d<double> origin(-topx1, -topy1);
  for (unsigned int i = 0; i<inp1.size(); i++) {
    inp1[i]->add_vector(origin);
    vsol_point_2d_sptr pt = inp1[i];
    vtol_vertex_2d_sptr vv = new vtol_vertex_2d(*pt);
    vert1.push_back(vv->cast_to_vertex());
  }
  vtol_face_2d_sptr face1 = new vtol_face_2d(vert1);

  image_0_= brip_vil1_float_ops::gaussian(img1, sigma);

  Ix_0_.resize(w,h);
  Iy_0_.resize(w,h);
  brip_vil1_float_ops::gradient_3x3(image_0_, Ix_0_, Iy_0_);

  vil1_memory_image_of< float > img2 = vil1_from_vil_image_view( float_image2 );
  w = img2.width(), h = img2.height(); 
  
  vgl_vector_2d<double> origin2(-topx2, -topy2);
  for (unsigned int i = 0; i<inp2.size(); i++) {
    inp2[i]->add_vector(origin2);
    vsol_point_2d_sptr pt = inp2[i];
    vtol_vertex_2d_sptr vv = new vtol_vertex_2d(*pt);
    vert2.push_back(vv->cast_to_vertex());
  }
  vtol_face_2d_sptr face2 = new vtol_face_2d(vert2);
  
  image_1_= brip_vil1_float_ops::gaussian(img2, sigma);

  Ix_1_.resize(w,h);
  Iy_1_.resize(w,h);
  brip_vil1_float_ops::gradient_3x3(image_1_, Ix_1_, Iy_1_);

  //: find alignment between input contours using curve matching
  clock_t time1, time2;
  vcl_vector< vsol_point_2d_sptr > pts1, pts2;

  time1 = clock();

  if (even_matching && !shock_matching) {
  
    dbsol_interp_curve_2d_sptr curve1;
    dbsol_interp_curve_2d_sptr curve2;

    if (!geno_interpolation) {
      curve1 = new dbsol_interp_curve_2d();
      curve2 = new dbsol_interp_curve_2d();
      dbsol_curve_algs::interpolate_linear(curve1.ptr(), inp1, true);  // removed closed1, I want closed curves always
      dbsol_curve_algs::interpolate_linear(curve2.ptr(), inp2, true);  // removed closed2
    } else { // geno
      curve1 = new dbsol_geno_curve_2d();
      curve2 = new dbsol_geno_curve_2d();
      dbsol_geno::interpolate((dbsol_geno_curve_2d *)curve1.ptr(),inp1, true);
      dbsol_geno::interpolate((dbsol_geno_curve_2d *)curve2.ptr(),inp2, true);
    }   

    int n1 = inp1.size();
    int n2 = inp2.size();
      
    double length1 = curve1->length();
    double length2 = curve2->length();
    //vcl_cout << "length 1: " << length1 << vcl_endl; vcl_cout << "length 2: " << length2 << vcl_endl;

    dbcvr_clsd_cvmatch_even_sptr clsdp = new dbcvr_clsd_cvmatch_even (curve1, curve2, n1, n2, (double)R, 3);
    clsdp->Match();

    bool change = (n1 != clsdp->n1());
    if (change) { // curve matching have exchanged the curves, change them back 
      //vcl_cout << "changed them back!!\n";
      curve1 = clsdp->curve2();
      curve2 = clsdp->curve1();
    }

    double delta_s1 = length1/n1;
    double delta_s2 = length2/n2;
      
    double minCost=clsdp->finalCost(0);
    int minIndex = 0;
    for (int count=1;count<clsdp->n1();count++){
      if (minCost>clsdp->finalCost(count)){
        minCost=clsdp->finalCost(count);
        minIndex=count;
      }
    }
    vcl_vector< vcl_pair <int,int> > map = clsdp->finalMap(minIndex);
    //vcl_cout << "final cost of matching: " << minCost << vcl_endl;

    for (unsigned int k = 0; k<map.size(); k++) {
      int i;
      int j;
      if (change) {
        i = map[k].second;
        j = map[k].first;
      } else {
        i = map[k].first;
        j = map[k].second;
      }

      vsol_point_2d_sptr p = curve1->point_at(vcl_fmod(i*delta_s1, length1));
      pts1.push_back(p);
      vsol_point_2d_sptr p2 = curve2->point_at(vcl_fmod(j*delta_s2, length2));
      pts2.push_back(p2);
    }

  } 
  
  if (thomas_matching && !shock_matching) { // use Thomas curve matching code which aligns original samples of curves
  
    bsol_intrinsic_curve_2d_sptr curve1 = new bsol_intrinsic_curve_2d(inp1);
    bsol_intrinsic_curve_2d_sptr curve2 = new bsol_intrinsic_curve_2d(inp2);

    curve1->setOpen(false);
    curve2->setOpen(false);
    curve1->computeProperties();
    curve2->computeProperties();

    double length1=curve1->arcLength(curve1->size()-1);
    //vcl_cout << "length of first curve: " << length1 << vcl_endl;
    double length2=curve2->arcLength(curve2->size()-1);
    //vcl_cout << "length of second curve: " << length2 << vcl_endl;

    dbcvr_clsd_cvmatch_sptr d1 = new dbcvr_clsd_cvmatch(curve1,curve2,(double)R, 3);
    //d1->setStretchCostFlag(flag);
    d1->setStretchCostFlag(false);   // cost: |ds1-ds2| + R|d_theta1-d_theta2|

    time1 = clock();
    d1->Match();
  
    double minCost=d1->finalCost(0);
    int minIndex = 0;
    for (int count=1;count<curve1->size();count++){
      if (minCost>d1->finalCost(count)){
        minCost=d1->finalCost(count);
        minIndex=count;
      }
    }
  
    time2 = clock();

    double normCost=minCost/(length1+length2);
    vcl_printf("%9.6f %9.6f %9.6f\n",minCost,normCost,(length1+length2));
    //vcl_cout<< "matching time: "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
  
    for( unsigned int i = 0 ; i < d1->finalMap(minIndex).size() ; i++ ) {
      vsol_point_2d_sptr pt = curve2->vertex(d1->finalMap(minIndex)[i].second);
      pts2.push_back(pt);
    }

    for( unsigned int i = 0 ; i < d1->finalMap(minIndex).size() ; i++ ) {
      vsol_point_2d_sptr pt = curve1->vertex(d1->finalMap(minIndex)[i].first%curve1->size());
      pts1.push_back(pt);
    }

  }
  
  dbru_rcor rcor(face1, face2);

  if (distance_transform) {
    //vcl_cout << "Using distance transform to find region correspondences!\n";
    //: use interpolated curves for interpolation based on arclength
    dbsol_interp_curve_2d_sptr curve1 = new dbsol_interp_curve_2d();
    dbsol_interp_curve_2d_sptr curve2 = new dbsol_interp_curve_2d();
    dbsol_curve_algs::interpolate_linear(curve1.ptr(), inp1, true);  // removed closed1, I want closed curves always
    dbsol_curve_algs::interpolate_linear(curve2.ptr(), inp2, true);  // removed closed2   

    if (!rcor.find_correspondence_dt(curve1, curve2, pts1, pts2, lambda)) {
      vcl_cout << "Region correspondence based on distance transform could not be found!\n";
      return false;
    }

  }

  if (line_intersections) {
    //vcl_cout << "Using line intersections to find region correspondences!\n";
    if (!rcor.find_correspondence(pts1, pts2, increment) ) {
      vcl_cout << "Region correspondence based on line intersections could not be found!\n";
      return false;
    }
  }

  if (shock_matching) {

    //: prepare the trees also
    dbskr_tree_sptr tree0 = new dbskr_tree();
    dbskr_tree_sptr tree1 = new dbskr_tree();
    tree0->acquire(sg0);
    tree1->acquire(sg1);

    dbskr_tree_edit edit(tree0, tree1);
    vcl_vector<pathtable_key> path_map;
    vcl_vector<dbskr_scurve_sptr> curve_list1, curve_list2;
    vcl_vector< vcl_vector< vcl_pair<int, int> > > map_list;

    edit.save_path(true);
    if (!edit.edit()) {
      vcl_cout << "Problems in editing trees\n";
      return 0;
    }

    float val = edit.final_cost();
    vcl_cout << " cost: " << val << vcl_endl; 
    //edit.write_shgm(shgm_file);
    edit.get_correspondence(curve_list1, curve_list2, map_list);

    if (!rcor.find_correspondence_shock(curve_list1, curve_list2, map_list, topx1, topy1, topx2, topy2)) {
      vcl_cout << "Region correspondence based on shock matching could not be found!\n";
      return 0;
    }
  }

  vcl_vector <vcl_vector< vgl_point_2d<int> > > region1_map_output = rcor.get_map();
  int min_x = rcor.get_min_x();
  int min_y = rcor.get_min_y();

  // compute mutual info

  //: dummy hue and sat images
  vil1_memory_image_of<float> hue_0_;  //hue of image_0
  vil1_memory_image_of<float> sat_0_;  //saturation of image_0
  vil1_memory_image_of<float> hue_1_;  //hue of image 1
  vil1_memory_image_of<float> sat_1_;  //saturation of image_1

  // Parameters of mutual information tracking face
  float min_gradient = 0.0f;               //!< minimum gradient magnitude to be considered
  float parzen_sigma = 0.0f;               //!< smoothing for the histogram density
  unsigned int intensity_hist_bins = 16;   //!< number of intensity histogram bins
  unsigned int gradient_dir_hist_bins = 8; //!< number of grad dir histogram bins
  unsigned int color_hist_bins = 8;        //!< number of color histogram bins
  bool renyi_joint_entropy = false;        //!< use renyi entropy for joint distributions

  
  strk_tracking_face_2d_sptr tf =
    new strk_tracking_face_2d(face1, image_0_,
                              Ix_0_, Iy_0_, hue_0_, sat_0_,
                              min_gradient,
                              parzen_sigma,
                              intensity_hist_bins,
                              gradient_dir_hist_bins,
                              color_hist_bins,
                              max_value);

  if (renyi_joint_entropy)
    tf->set_renyi_joint_entropy();

  //: using region correspondence to compute mutual information
  if (!tf->compute_mutual_information(region1_map_output, min_x, min_y, image_1_, Ix_1_, Iy_1_, hue_1_, sat_1_)) {
      vcl_cout << "Problem in mutual information computation!\n";
    return false;
  }
  
  total_info_ = tf->total_info();
  int_mutual_info_ = tf->int_mutual_info();
  grad_mutual_info_ = tf->grad_mutual_info();
  color_mutual_info_ = tf->color_mutual_info();

  vcl_cout << "Total Inf = " << total_info_
             << " = IntInfo(" <<  int_mutual_info_
             << ") + GradInfo(" <<  grad_mutual_info_
             << ") + ColorInfo(" <<  color_mutual_info_
             << ')' << vcl_endl;

  w = img1.width()+img2.width()+img1.width();
  h = img1.height()>img2.height()?img1.height():img2.height();
  vil1_memory_image_of<float> image_out(w,h);

  for (int y = 0; y<h; y++ ) 
    for (int x = 0; x<w; x++)
      image_out[y][x] = 255;

  vgl_polygon<double> p;
  if (!btol_face_algs::vtol_to_vgl(face1->cast_to_face_2d(), p))
    return 0;
  vgl_polygon_scan_iterator<double> psi(p, true);

  //go through the pixels once
  for (psi.reset(); psi.next();)
    for (int x = psi.startx(); x<=psi.endx(); ++x)
    {
      int y = psi.scany();
      if (x<0 || x>=image_0_.width() || y<0 || y>= image_0_.height() )
        continue;

      image_out[y][x] = image_0_(x, y);
    }

  vgl_polygon<double> p2;
  if (!btol_face_algs::vtol_to_vgl(face2->cast_to_face_2d(), p2))
    return 0;
  vgl_polygon_scan_iterator<double> psi2(p2, true);

  //go through the pixels once
  for (psi2.reset(); psi2.next();)
    for (int x = psi2.startx(); x<=psi2.endx(); ++x)
    {
      int y = psi2.scany();
      if (x<0 || x>=image_1_.width() || y<0 || y>= image_1_.height() )
        continue;

      image_out[y][x+img1.width()] = image_1_(x, y);
    }

  //: prepare the output image
  for (unsigned int i = 0; i<region1_map_output.size(); i++) {
    for (unsigned int j = 0; j<region1_map_output[i].size(); j++) {
      
      vgl_point_2d<int> p = region1_map_output[i][j];
      if (p.x() < 0 || p.y() < 0 || p.x() >= image_1_.width() || p.y() >= image_1_.height()) continue;
      
      image_out(i+min_x+img1.width()+img2.width(), j+min_y) = image_1_(p.x(), p.y());
    }
  }

  vil1_memory_image_of<unsigned char> output_img = brip_vil1_float_ops::convert_to_byte(image_out, 0, max_value);
  vil_image_resource_sptr output_sptr = vil1_to_vil_image_resource(output_img);

  // create the output storage class
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(output_sptr);
  output_data_[0].push_back(output_storage);
  return true;
}

