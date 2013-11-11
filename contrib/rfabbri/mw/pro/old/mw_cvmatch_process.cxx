#include "mw_cvmatch_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_geno.h>

#include <mw/algo/mw_cvmatch.h>
#include <mw/algo/mw_cvmatch_sptr.h>
#include <dbdif/dbdif_rig.h>

#include <dbcvr/dbcvr_clsd_interp_cvmatch.h>
#include <dbcvr/dbcvr_clsd_interp_cvmatch_sptr.h>
#include <dbcvr/dbcvr_interp_cvmatch_sptr.h>

#include <dbcvr/dbcvr_clsd_cvmatch_even.h>
#include <dbcvr/dbcvr_clsd_cvmatch_even_sptr.h>


mw_cvmatch_process::mw_cvmatch_process()
{  
  if (//!parameters()->add( "With cost: sqrt((ds1)^2+(ds2)^2-(ds1*ds2*cos|dtheta1-dtheta2|))?" , "-normstretch" , false )  ||
      !parameters()->add( "Use my delta" , "-deltagiven" , false )  ||
      !parameters()->add( "delta:" , "-delta" , (float)10 )  ||
      !parameters()->add( "Give higher to see less number of alignment lines after matching:" , "-NN" , 1 )  ||
      !parameters()->add( "Template size (1, 3, 5 or 11):" , "-template_size" , 1 )  ||
      !parameters()->add( "R:" , "-r1" , 10.0f ) ||
      !parameters()->add( "Max epipolar error:" , "-epipolar_error_threshold" , 10) 
      ) 
  {
    vcl_cerr << "ERROR: Adding parameters in mw_cvmatch_process::mw_cvmatch_process()" << vcl_endl;
  }
}


//: Clone the process
bpro1_process*
mw_cvmatch_process::clone() const
{
  return new mw_cvmatch_process(*this);
}

bool mw_cvmatch_process::execute()
{
  vcl_ofstream cdebug;
  cdebug.open("debug");

  bool delta_given=false;
  parameters()->get_value( "-deltagiven" , delta_given );
  float deltag=0;
  parameters()->get_value( "-delta" , deltag );
  vcl_cout << "delta given is: " << deltag << vcl_endl;\
  int template_size=0;
  parameters()->get_value( "-template_size" , template_size );
  int NN=0;
  parameters()->get_value( "-NN" , NN );

  double epipolar_error_threshold=0;
  parameters()->get_value( "-epipolar_error_threshold" , epipolar_error_threshold);


  clear_output();

  

  // get input storage classes 
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);

  dvpgl_camera_storage_sptr input_cam1;
  input_cam1.vertical_cast(input_data_[0][1]);

  vidpro1_vsol2D_storage_sptr input_vsol2;
  input_vsol2.vertical_cast(input_data_[1][0]);

  dvpgl_camera_storage_sptr input_cam2;
  input_cam2.vertical_cast(input_data_[1][1]);



  vcl_vector<vsol_point_2d_sptr> inp1;
  vcl_vector<vsol_point_2d_sptr> inp2;
  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol1->all_data();
  bool closed1 = false;
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
          inp1.push_back(pt);
    } } }

    if( vsol_list[b]->cast_to_region())
    {
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        closed1 = true;
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
          //curve1->add_vertex(pt->x(), pt->y());
          inp1.push_back(pt);
  } } } }

  vcl_cout << "Original curve1 size: " << inp1.size() << vcl_endl;
  if (closed1)
    vcl_cout << "Original curve1 is a polygon, i.e. a closed curve\n";
  else
    vcl_cout << "Original curve1 is a polyline, i.e. an open curve\n";

  vsol_list = input_vsol2->all_data();
  bool closed2 = false;
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
          inp2.push_back(pt);
    } } }

    if( vsol_list[b]->cast_to_region())
    {
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        closed2 = true;
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
          //curve1->add_vertex(pt->x(), pt->y());
          inp2.push_back(pt);
  } } } }

  vcl_cout << "Original curve2 size: " << inp2.size() << vcl_endl;
  if (closed2)
    vcl_cout << "Original curve2 is a polygon, i.e. a closed curve\n";
  else
    vcl_cout << "Original curve2 is a polyline, i.e. an open curve\n";

  //dbsol_interp_curve_2d_sptr curve1 = new dbsol_interp_curve_2d();
  //dbsol_interp_curve_2d_sptr curve2 = new dbsol_interp_curve_2d();

  cdebug << "Camera1: \n" << input_cam1->get_camera();
  cdebug << "Camera2: \n" << input_cam2->get_camera();
  if (!input_cam1->get_camera()->cast_to_perspective_camera() || !input_cam2->get_camera()->cast_to_perspective_camera()) {
    vcl_cerr << "Error: input cams are not perspective\n";
  }

  dbdif_rig rig(
      *(input_cam1->get_camera()->cast_to_perspective_camera()), 
      *(input_cam2->get_camera()->cast_to_perspective_camera()));

  dbsol_interp_curve_2d_sptr curve1;
  dbsol_interp_curve_2d_sptr curve2;

  // GENO
  curve1 = new dbsol_geno_curve_2d();
  curve2 = new dbsol_geno_curve_2d();
  dbsol_geno::interpolate((dbsol_geno_curve_2d *)curve1.ptr(),inp1, true);
  dbsol_geno::interpolate((dbsol_geno_curve_2d *)curve2.ptr(),inp2, true);

  for (unsigned i=0; i< inp1.size(); ++i)
    cdebug << *inp1[i];

  float r1=0;
  parameters()->get_value( "-r1" , r1 );
 
  //: generate this object just for visualization
  dbcvr_cvmatch* curveMatch = new dbcvr_cvmatch();   // template size is 3 in this case
  clock_t time1, time2;
  
  int n1 = inp1.size();
  int n2 = inp2.size();
      
  double len1 = curve1->length();
  double len2 = curve2->length();
  vcl_cout << "length 1: " << len1 << vcl_endl; vcl_cout << "length 2: " << len2 << vcl_endl;
      
  double min_alignment_length = vcl_sqrt(vcl_pow(len1, 2)+vcl_pow(len2, 2));
  double max_alignment_length = len1+len2;
  vcl_cout << "min alignment curve length: " << min_alignment_length << vcl_endl;
  vcl_cout << "max alignment curve length: " << max_alignment_length << vcl_endl;
      
  double delta = max_alignment_length/(n1+n2);  // for now
  //double delta = min_alignment_length/(n1>n2?n1:n2);  // for now
  //double delta = (len2/n2<len1/n1?min_alignment_length/n2:min_alignment_length/n1);  // for now
  //double delta = (n2<n1?min_alignment_length/n2:min_alignment_length/n1);
  //int max_n = (n1>n2?n1:n2);
  //delta = min_alignment_length/max_n;  // for now
      
      /*vcl_cout << "min_alignment/max(n1, n2): " << delta << vcl_endl;
      delta = min_alignment_length/((float)max_n/2.0);
      vcl_cout << "min_alignment_length/((float)max_n/2.0: " << delta << vcl_endl;
      delta = min_alignment_length/((float)(n1+n2)/2.0);
      vcl_cout << "min_alignment_length/((float)(n1+n2)/2.0: " << delta << vcl_endl;
      */
      
  double delta_s1 = len1/inp1.size();
  double delta_s2 = len2/inp2.size();
  if (delta_given) {
    delta = deltag;
  } else {
    
    delta = (delta_s1 < delta_s2)? delta_s1:delta_s2;
  }

  //delta = max_alignment_length/(n1+n2); 
  vcl_cout << "delta: " << delta << vcl_endl;

  
  {
    vcl_cout << "In closed MULTIVIEW interpolated curve matching!!\n";
    
    int n1 = (int)(len1/delta+0.5);   // round to nearest integer
    int n2 = (int)(len2/delta+0.5);

    vcl_cout << "n1: " << n1 << " n2: " << n2 << vcl_endl;
    mw_cvmatch_sptr clsdp = new mw_cvmatch (
        curve1, curve2, n1, n2, (double)r1, template_size, epipolar_error_threshold);

    clsdp->set_rig(&rig);

    time1 = clock();
    clsdp->Match();
    time2 = clock();

    n1 = clsdp->n1();
    n2 = clsdp->n2();
    curve1 = clsdp->curve1();  // they might have been exchanged due to length
    curve2 = clsdp->curve2();
    len1 = curve1->length();
    len2 = curve2->length();
    delta_s1 = len1/n1;
    delta_s2 = len2/n2;
    
    /* Uncomment block for closed cvmatch
    double minCost=1E10;
    int minIndex;
    for (int count=0;count<n1;count++){
      if (minCost>clsdp->finalCost(count)){
        minCost=clsdp->finalCost(count);
        minIndex=count;
      }
    }
    */
    int minIndex = 0;
    double minCost=clsdp->finalCost(0);

    vcl_vector<vsol_point_2d_sptr> out_curve1;
    vcl_vector<vsol_point_2d_sptr> out_curve2;

    vcl_vector< vcl_pair <int,int> > map = clsdp->finalMap(minIndex);
    vcl_cout << "Map size: " << map.size() << vcl_endl;
    vcl_vector< vcl_pair <int,int> > map2;
    for (unsigned k = 0; k<map.size(); k++) {
      int i = map[k].first;
      int j = map[k].second;
      vcl_cout << "maps: " << i << " to " << j << vcl_endl;
      vcl_cout << "len1: " << vcl_fmod(i*delta_s1, len1) << " to " << vcl_fmod(j*delta_s2, len2) << vcl_endl;
      out_curve1.push_back(curve1->point_at(vcl_fmod(i*delta_s1, len1)));
      out_curve2.push_back(curve2->point_at(vcl_fmod(j*delta_s2, len2)));
      
    }

    vcl_vector<mw_vector_3d> C3d;
    rig.reconstruct_3d_curve(&C3d, out_curve1, out_curve2);
    // output to file
    vcl_ofstream prec;
    prec.open("reconstr.dat");
    for (unsigned i=0; i< C3d.size(); ++i) {
      prec << C3d[i][0] << " " << C3d[i][1] << " " << C3d[i][2] << vcl_endl;
    }
    prec.close();


    // Now we just create a plain curveMatch so we can use the same stuff done in
    // Brown Eyes for visualizing

    bsol_intrinsic_curve_2d_sptr c1 = new bsol_intrinsic_curve_2d(out_curve1);
    bsol_intrinsic_curve_2d_sptr c2 = new bsol_intrinsic_curve_2d(out_curve2);

    curveMatch->setCurve1 (c1);
    curveMatch->setCurve2 (c2);

    //vcl_cout<< vcl_endl<<"curve_2d matching cost: "<< clsdp->finalCost() <<vcl_endl;
    vcl_cout<< "cost: " << minCost << " time: "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;

    assert(c1->size() == c2->size());

    map.clear();
    for (int i = 0; i<c1->size(); i+=NN) {
      vcl_pair<int, int> p(i,i);
      map.push_back(p);
    }
    
    curveMatch->setFinalMap(map);
  }

  
  // create the output storage class
  dbcvr_curvematch_storage_sptr output_curvematch = dbcvr_curvematch_storage_new();
  output_data_[0].push_back (output_curvematch);
  output_curvematch->set_curvematch (curveMatch);

  cdebug.close();
  return true;
}

