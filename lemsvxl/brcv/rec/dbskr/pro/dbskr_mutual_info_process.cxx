#include "dbskr_mutual_info_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>


#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
//#include <vsol/vsol_curve_2d.h>
//#include <vsol/vsol_point_2d.h>
//#include <vsol/vsol_line_2d.h>


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

#include <dbskr/pro/dbskr_shock_match_process.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_localize_match.h>

#define MARGIN  (10)

dbskr_mutual_info_process::dbskr_mutual_info_process()
{  
  if (!parameters()->add( "Load shock graph 1 from esf file?" , "-load1" , (bool) true ) ||
       !parameters()->add( "Input esf file 1 <filename...>" , 
                           "-esf1" ,
                           bpro1_filepath("","*.esf") 
                           ) ||
       !parameters()->add( "Load shock graph 2 from esf file?" , "-load2" , (bool) true ) ||
       !parameters()->add( "Input esf file 2 <filename...>" , 
                           "-esf2" ,
                           bpro1_filepath("","*.esf") 
                           ) ||
       !parameters()->add( "Match file exists?" , "-fileexists" , (bool) true ) ||
       !parameters()->add( "Input/Output shgm file <filename...>" , 
                           "-shgm" ,
                           bpro1_filepath("","*.shgm") 
                           ) ||
      !parameters()->add( "Smoothing sigma" , "-sigma" , 1.0f )  ||
      //!parameters()->add( "R: " , "-R" , 10.0f ) ||
      //!parameters()->add( "Thomas matching (otherwise even grid matching): " , "-even" , false) ||
      //!parameters()->add( "If even grid matching, geno interpolation (otherwise linear): " , "-linear" , false) ||
      //!parameters()->add( "increment: " , "-increment" , 20) ||
      !parameters()->add( "Camera image pixel range (in bits): " , "-imagebits" , 8) 
      //!parameters()->add( "Use line intersections for region correspondence?:" , "-disttrans" , false )  ||
      //!parameters()->add( "lamda for dt: " , "-lambda" , 1.0f ) 
      ) {
    vcl_cerr << "ERROR: Adding parameters in dbskr_mutual_info_process::dbskr_mutual_info_process()" << vcl_endl;
  }

  total_info_ = 0;
  int_mutual_info_ = 0;
  grad_mutual_info_ = 0;
  color_mutual_info_ = 0;
}


//: Clone the process
bpro1_process*
dbskr_mutual_info_process::clone() const
{
  return new dbskr_mutual_info_process(*this);
}

bool dbskr_mutual_info_process::execute()
{
  
  float sigma;
  parameters()->get_value( "-sigma" , sigma );
  /*float R;
  parameters()->get_value( "-R" , R );
  bool line_intersections;
  parameters()->get_value( "-disttrans" , line_intersections );
  bool thomas_matching;
  parameters()->get_value( "-even" , thomas_matching );
  bool geno_interpolation;
  parameters()->get_value( "-linear" , geno_interpolation );
  float lambda;
  parameters()->get_value("-lambda", lambda);
  int increment;
  parameters()->get_value("-increment", increment);
  //vcl_cout << "increment value is: " << increment << vcl_endl;
*/

  int image_bits;
  parameters()->get_value("-imagebits", image_bits);
  float max_value = float(vcl_pow(double(2.0), double(image_bits))-1);

  clear_output();
  clock_t time1, time2;

  bool load1, load2;
  parameters()->get_value( "-load1" , load1);
  parameters()->get_value( "-load2" , load2);
  bpro1_filepath input_path;
  parameters()->get_value( "-esf1" , input_path);
  vcl_string esf_file1 = input_path.path;
  parameters()->get_value( "-esf2" , input_path);
  vcl_string esf_file2 = input_path.path;  
  parameters()->get_value( "-shgm" , input_path);
  vcl_string shgm_file = input_path.path;

  dbsk2d_xshock_graph_fileio loader;

  dbsk2d_shock_storage_sptr shock1, shock2;
  dbsk2d_shock_graph_sptr sg1, sg2;

  time1 = clock();

  if (load1) {
    sg1 = loader.load_xshock_graph(esf_file1);
  } else {
    // get input storage class
    shock1.vertical_cast(input_data_[0][1]);
    sg1 = shock1->get_shock_graph();
  }

  if (load2) {
    sg2 = loader.load_xshock_graph(esf_file2);
  } else {
    shock2.vertical_cast(input_data_[0][4]);
    sg2 = shock2->get_shock_graph();
  }
  
  if (!sg1 || !sg2)
  {
    vcl_cout << "Problems in getting shock graphs!\n";
    return false;
  }

  //: prepare the trees also
  dbskr_tree_sptr tree1 = new dbskr_tree();
  dbskr_tree_sptr tree2 = new dbskr_tree();
  tree1->acquire(sg1);
  tree2->acquire(sg2);

  dbskr_tree_edit edit(tree1, tree2);

  bool exists;
  parameters()->get_value( "-fileexists" , exists);

  vcl_vector<pathtable_key> path_map;
  vcl_vector<dbskr_scurve_sptr> curve_list1, curve_list2;
  vcl_vector< vcl_vector< vcl_pair<int, int> > > map_list;
  if (exists) { // no need for matching
    path_map = read_shgm(tree1, tree2, shgm_file);
    get_correspondence(tree1, tree2, path_map, curve_list1, curve_list2, map_list);
  } else {  // do the matching
    vcl_cout << "matching shock graphs...\n";
    clock_t time1, time2;
    time1 = clock();
    
    edit.save_path(true);
    if (!edit.edit()) {
      vcl_cout << "Problems in editing trees\n";
      return false;
    }
    time2 = clock();
    float val = edit.final_cost();
    vcl_cout << " cost: " << val << " time: "<< ((double)(time2-time1))/CLOCKS_PER_SEC << "\n";
    edit.write_shgm(shgm_file);
    path_map = read_shgm(tree1, tree2, shgm_file);
    edit.get_correspondence(curve_list1, curve_list2, map_list);
  }

  //: get input vsol 
  vidpro1_vsol2D_storage_sptr input_vsol1;
  //input_vsol1.vertical_cast(input_data_[0][0]);
  input_vsol1.vertical_cast(input_data_[0][0]);

  vidpro1_vsol2D_storage_sptr input_vsol2;
  //input_vsol2.vertical_cast(input_data_[1][0]);
  input_vsol2.vertical_cast(input_data_[0][3]);

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

  //: get input images
  vidpro1_image_storage_sptr frame_image1;
  frame_image1.vertical_cast(input_data_[0][2]);
  vil_image_resource_sptr image1_sptr = frame_image1->get_image();

  vidpro1_image_storage_sptr frame_image2;
  frame_image2.vertical_cast(input_data_[0][5]);
  vil_image_resource_sptr image2_sptr = frame_image2->get_image();
  
  vil_image_resource_sptr grey_image_sptr;
  grey_image_sptr = vil_new_image_resource_of_view( *vil_convert_to_grey_using_rgb_weighting ( image1_sptr->get_view() ) );
  if (grey_image_sptr->ni()==0)
  {
    vcl_cout<<"Failed to load image 1."<<vcl_endl;
    return false;
  }
  image1_sptr = grey_image_sptr;
  grey_image_sptr = vil_new_image_resource_of_view( *vil_convert_to_grey_using_rgb_weighting ( image2_sptr->get_view() ) );
  if (grey_image_sptr->ni()==0)
  {
    vcl_cout<<"Failed to load image 2."<<vcl_endl;
    return false;
  }
  image2_sptr = grey_image_sptr;

  if (image1_sptr->nplanes() > 1 || image2_sptr->nplanes() > 1) {
    vcl_cout << "Failed to load images!\n";
    return false;
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

  dbru_rcor rcor(face1, face2);
  if (!rcor.find_correspondence_shock(curve_list1, curve_list2, map_list, topx1, topy1, topx2, topy2)) {
      vcl_cout << "Region correspondence based on shock matching could not be found!\n";
      return false;
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

  time2 = clock();

  vcl_cout << "Total Inf = " << total_info_
             << " = IntInfo(" <<  int_mutual_info_
             << ") + GradInfo(" <<  grad_mutual_info_
             << ") + ColorInfo(" <<  color_mutual_info_
             << ") total time: " << ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds " << vcl_endl;

  
  for (unsigned int i = 0; i<region1_map_output.size(); i++) {
    for (unsigned int j = 0; j<region1_map_output[i].size(); j++) {
         
      image_0_(i+min_x, j+min_y) = 255;
    }
  }

  //: prepare the output image
  for (unsigned int i = 0; i<region1_map_output.size(); i++) {
    for (unsigned int j = 0; j<region1_map_output[i].size(); j++) {
      
      vgl_point_2d<int> p = region1_map_output[i][j];
      if (p.x() < 0 || p.y() < 0 || p.x() >= image_1_.width() || p.y() >= image_1_.height()) continue;
      
      image_0_(i+min_x, j+min_y) = image_1_(p.x(), p.y());
    }
  }

  

  vil1_memory_image_of<unsigned char> output_img = brip_vil1_float_ops::convert_to_byte(image_0_, 0, max_value);
  vil_image_resource_sptr output_sptr = vil1_to_vil_image_resource(output_img);

  // create the output storage class
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(output_sptr);
  output_data_[0].push_back(output_storage);
  return true;
}


vcl_vector<pathtable_key> 
dbskr_mutual_info_process::read_shgm(dbskr_tree_sptr tree1, dbskr_tree_sptr tree2, vcl_string fname) 
{
  
  vcl_vector<pathtable_key> out;

  vcl_ifstream tf(fname.c_str());
  
  if (!tf) {
    vcl_cout << "Unable to open file " << fname << " for write " << vcl_endl;
    return out;
  }

  char buffer[1000];
  tf.getline(buffer, 1000);  // version
  tf.getline(buffer, 1000);  // name 1
  vcl_string line = buffer;
  tf.getline(buffer, 1000);  // name 2
  line = buffer;

  float cost;
  tf >> cost;
  vcl_cout << "matching cost: " << cost << "\n";
  
  vcl_map<vcl_string, vcl_string> paths;
  vcl_string line2;

  while (!tf.eof()) {
    tf.getline(buffer, 1000);
    line = buffer;
    if (line.length() < 2) // this is new line character or eof
      continue;
    // read the second line
    tf.getline(buffer, 1000);
    line2 = buffer;
    if (line2.length() < 2) {  // something is wrong with this file
      vcl_cout << "T1 and T2 corresponding paths are not on consecutive lines in this file\n!!";
      return out;  // return as it is
    }

    char * pch;
    char tmp[1000];
    vcl_sprintf(tmp, "%s", line.c_str());
    //pch = strtok (line.c_str()," ");
    pch = strtok(tmp," ");
    int first = atoi(pch);
    int id = 0;
    while (pch != NULL)
    {
      id = atoi(pch);
      pch = strtok (NULL, " ");
    }

    pathtable_key key;
    key.first.first = first;
    key.first.second = id;

    vcl_sprintf(tmp, "%s", line2.c_str());
    pch = strtok(tmp," ");
    first = atoi(pch);
    
    while (pch != NULL)
    {
      id = atoi(pch);
      pch = strtok (NULL, " ");
    }
    key.second.first = first;
    key.second.second = id;

    out.push_back(key);
  }

  tf.close();

  //: currently out has keys with node ids on the tree, we need to turn them into dart ids on the tree
  //  using vcl_vector<int>& get_dart_path_from_nodes(int node1, int node2) method of tree

  for (unsigned int i = 0; i<out.size(); i++) {
    vcl_vector<int> dart_path1 = tree1->get_dart_path_from_nodes(out[i].first.first, out[i].first.second);
    vcl_vector<int> dart_path2 = tree2->get_dart_path_from_nodes(out[i].second.first, out[i].second.second);
    
    out[i].first.first = dart_path1[0];
    out[i].first.second = dart_path1[dart_path1.size()-1];

    out[i].second.first = dart_path2[0];
    out[i].second.second = dart_path2[dart_path2.size()-1];
  }

  return out;
}

void
dbskr_mutual_info_process::get_correspondence(dbskr_tree_sptr tree1, dbskr_tree_sptr tree2,
                                              vcl_vector<pathtable_key>& path_map, 
                                              vcl_vector<dbskr_scurve_sptr>& curve_list1, 
                                              vcl_vector<dbskr_scurve_sptr>& curve_list2, 
                                              vcl_vector<vcl_vector<vcl_pair<int, int> > >& map_list)
{
  curve_list1.clear();
  curve_list2.clear();
  map_list.clear();

  for (int i = path_map.size()-1; i>=0; i--) {
    pathtable_key key = path_map[i];
    dbskr_sc_pair_sptr curve_pair1 = tree1->get_curve_pair(key.first.first, key.first.second);
    dbskr_sc_pair_sptr curve_pair2 = tree2->get_curve_pair(key.second.first, key.second.second);

    dbskr_scurve_sptr sc1 = curve_pair1->coarse;
    dbskr_scurve_sptr sc2 = curve_pair2->coarse;

    //curve_list1.push_back(curve_pair1->dense);
    //curve_list2.push_back(curve_pair2->dense);
    curve_list1.push_back(sc1);
    curve_list2.push_back(sc2);
   
    dbskr_dpmatch d(sc1, sc2);
    d.Match();
    vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());
    dbskr_localize_match lmatch(sc1, sc2, 
                                curve_pair1->dense, curve_pair2->dense, 
                                curve_pair1->c_d_map, curve_pair2->c_d_map, 
                                fmap, 1.0f);
    lmatch.match();
    //map_list.push_back(lmatch.finalMap());
    map_list.push_back(fmap);
  }
}
