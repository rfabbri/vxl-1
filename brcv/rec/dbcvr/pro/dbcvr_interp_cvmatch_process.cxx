#include "dbcvr_interp_cvmatch_process.h"

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

#include <dbcvr/dbcvr_interp_cvmatch.h>
#include <dbcvr/dbcvr_interp_cvmatch_sptr.h>

#include <dbcvr/dbcvr_clsd_interp_cvmatch.h>
#include <dbcvr/dbcvr_clsd_interp_cvmatch_sptr.h>

#include <dbcvr/dbcvr_clsd_cvmatch_even.h>
#include <dbcvr/dbcvr_clsd_cvmatch_even_sptr.h>

#include <dbcvr/dbcvr_cvmatch_even.h>
#include <dbcvr/dbcvr_cvmatch_even_sptr.h>
#include <dbcvr/dbcvr_cv_cor.h>

dbcvr_interp_cvmatch_process::dbcvr_interp_cvmatch_process()
{  
  if (//!parameters()->add( "With cost: sqrt((ds1)^2+(ds2)^2-(ds1*ds2*cos|dtheta1-dtheta2|))?" , "-normstretch" , false )  ||
      !parameters()->add( "Use my delta" , "-deltagiven" , false )  ||
      !parameters()->add( "delta:" , "-delta" , (float)10 )  ||
      !parameters()->add( "Type of interpolation: (1: bilinear, 2: bicubic, 3: geno, 4: eno)" , "-type" , 1 )  ||
      //!parameters()->add( "Back: ", "-back" , 5 )  ||
      !parameters()->add( "Closed curve matching? oblique grid" , "-closedversion" , true )  ||
      !parameters()->add( "Closed curve matching? even grid" , "-closedversioneven" , false )  ||
      !parameters()->add( "if open matching with even grid?" , "-openeven" , false )  ||
      !parameters()->add( "Give higher to see less number of alignment lines after matching:" , "-NN" , 1 )  ||
      !parameters()->add( "Template size (1, 3, 5 or 11):" , "-template_size" , 1 )  ||
      !parameters()->add( "R:" , "-r1" , 10.0f ) ) {
    vcl_cerr << "ERROR: Adding parameters in dbcvr_interp_cvmatch_process::dbcvr_interp_cvmatch_process()" << vcl_endl;
  }
}


//: Clone the process
bpro1_process*
dbcvr_interp_cvmatch_process::clone() const
{
  return new dbcvr_interp_cvmatch_process(*this);
}

bool dbcvr_interp_cvmatch_process::execute()
{
  bool open_even=false;
  parameters()->get_value( "-openeven" , open_even );
  bool closed_version_even=false;
  parameters()->get_value( "-closedversioneven" , closed_version_even );
  bool closed_version_normal=false;
  parameters()->get_value( "-closedversion" , closed_version_normal );
  bool delta_given=false;
  parameters()->get_value( "-deltagiven" , delta_given );
  float deltag=0;
  parameters()->get_value( "-delta" , deltag );
  vcl_cout << "delta given is: " << deltag << vcl_endl; 
  int interpolation_type=0;
  parameters()->get_value( "-type" , interpolation_type );
  int template_size=0;
  parameters()->get_value( "-template_size" , template_size );

  int NN=0;
  parameters()->get_value( "-NN" , NN );
  clear_output();

  // get input storage class
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);
  vcl_vector<vsol_point_2d_sptr> inp1;

  vidpro1_vsol2D_storage_sptr input_vsol2;
  input_vsol2.vertical_cast(input_data_[0][1]);
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

  dbsol_interp_curve_2d_sptr curve1;
  dbsol_interp_curve_2d_sptr curve2;

  bool closed = closed_version_normal || closed_version_even;

  if (interpolation_type == 1) 
  {  // linear
    curve1 = new dbsol_interp_curve_2d();
    curve2 = new dbsol_interp_curve_2d();
    dbsol_curve_algs::interpolate_linear(curve1.ptr(), inp1, closed);  // removed closed1, I want closed curves always
    dbsol_curve_algs::interpolate_linear(curve2.ptr(), inp2, closed);  // removed closed2
  } 
  else if (interpolation_type == 2)
  {  // cubic
    vcl_cout << "CUBIC INTERPOLATION NOT IMPLEMENTED" << vcl_endl;
    return false;
  } 
  else if (interpolation_type == 3)
  {  // geno
    curve1 = new dbsol_geno_curve_2d();
    curve2 = new dbsol_geno_curve_2d();
    dbsol_geno::interpolate((dbsol_geno_curve_2d *)curve1.ptr(),inp1, closed);
    dbsol_geno::interpolate((dbsol_geno_curve_2d *)curve2.ptr(),inp2, closed);
  } 
  else if (interpolation_type == 4)
  {
    // eno
    curve1 = new dbsol_interp_curve_2d();
    curve2 = new dbsol_interp_curve_2d();
    vnl_vector<double> sample_pts;
    dbsol_curve_algs::interpolate_eno(curve1.ptr(), inp1, sample_pts);
    dbsol_curve_algs::interpolate_eno(curve2.ptr(), inp2, sample_pts);
  }
  else 
  {  //unknown
    vcl_cout << "This type of interpolation is unknown\n";
    return false;
  }
  
  float r1=0;
  parameters()->get_value( "-r1" , r1 );
 
  //: generate this object just for visualization
  dbcvr_cvmatch* curveMatch = new dbcvr_cvmatch();   // template size is 3 in this case
  clock_t time1, time2;
  
  //: generate these smart pointers to be used in curveMatch for visualization
  bsol_intrinsic_curve_2d_sptr c1;
  bsol_intrinsic_curve_2d_sptr c2;

  int n1 = inp1.size();
  int n2 = inp2.size();
      
  double len1 = curve1->length();
  double len2 = curve2->length();
  vcl_cout << "length 1: " << len1 << vcl_endl; vcl_cout << "length 2: " << len2 << vcl_endl;
      
  double min_alignment_length = vcl_sqrt(vcl_pow(len1, 2)+vcl_pow(len2, 2));
  double max_alignment_length = len1+len2;
  vcl_cout << "min alignment curve length: " << min_alignment_length << vcl_endl;
  vcl_cout << "max alignment curve length: " << max_alignment_length << vcl_endl;
      
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  double delta = max_alignment_length/(n1+n2);  // for now
#endif
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
      
  
  //delta = max_alignment_length/(n1+n2); 
  
  dbcvr_cv_cor_sptr sil_cor;
  if (closed_version_normal) {    // closed curve matching with oblique grid
      vcl_cout << "In closed curve matching with oblique grid!!\n";
      
      // I want k starting points on shorter curve
      double L1 = (len1<len2)?len1:len2;
      double L2 = (len1<len2)?len2:len1;

      int k = static_cast<int>(vcl_ceil( L1/ ((L1+L2)/(n1+n2)) )); 

      double delta_eta = (L1*L2)/( vcl_sqrt(vcl_pow(L1,2) + vcl_pow(L2,2)) * k );

      double delta = vcl_pow(L1*L1 + L2*L2, 0.5)/(n1>n2?n1:n2);  // for now

      dbcvr_clsd_interp_cvmatch_sptr clsdp = new dbcvr_clsd_interp_cvmatch(curve1, 
                                                                           curve2, 
                                                                           (double)r1, 
                                                                           delta, 
                                                                           delta_eta);
      time1 = clock();
      clsdp->Match();
      time2 = clock();

      c1 = new bsol_intrinsic_curve_2d(clsdp->output_curve1());
      c2 = new bsol_intrinsic_curve_2d(clsdp->output_curve2());

      curveMatch->setCurve1 (c1);
      curveMatch->setCurve2 (c2);

      //vcl_cout<< vcl_endl<<"curve_2d matching cost: "<< clsdp->finalCost(0) <<vcl_endl;
      vcl_cout<< "matching time: "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;

      assert(c1->size() == c2->size());

      vcl_vector< vcl_pair<int,int> > map;
      for (int i = 0; i<c1->size(); i++) {
        vcl_pair<int, int> p(i,i);
        map.push_back(p);
      }
      
      curveMatch->setFinalMap(map);

      sil_cor = new dbcvr_cv_cor(c1, c2, map, c1->size());

  } else if (closed_version_even) {
      vcl_cout << "In closed curve matching with normal grid!!\n";
      
      double delta_s1 = len1/inp1.size();
      double delta_s2 = len2/inp2.size();
      
      double delta = (delta_s1 < delta_s2)? delta_s1:delta_s2;
      int n1 = (int)(len1/delta+0.5);   // round to nearest integer
      int n2 = (int)(len2/delta+0.5);

      dbcvr_clsd_cvmatch_even_sptr clsdp = new dbcvr_clsd_cvmatch_even (curve1, 
          //                                                              curve2, n1, n2,
                                                                        curve2, inp1.size(), inp2.size(),
                                                                        (double)r1, 
                                                                        template_size);
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
      
      double minCost=1E10;
      int minIndex=0;
      for (int count=0;count<n1;count++){
        if (minCost>clsdp->finalCost(count)){
          minCost=clsdp->finalCost(count);
          minIndex=count;
        }
      }

      vcl_vector<vsol_point_2d_sptr> out_curve1;
      vcl_vector<vsol_point_2d_sptr> out_curve2;

      vcl_vector< vcl_pair <int,int> > map = clsdp->finalMap(minIndex);
      vcl_vector< vcl_pair <int,int> > map2;
      for (unsigned k = 0; k<map.size(); k++) {
        int i = map[k].first;
        int j = map[k].second;
        //vcl_cout << "maps: " << i << " to " << j << vcl_endl;
        //vcl_cout << "len1: " << vcl_fmod(i*delta_s1, len1) << " to " << vcl_fmod(j*delta_s2, len2) << vcl_endl;
        out_curve1.push_back(curve1->point_at(vcl_fmod(i*delta_s1, len1)));
        out_curve2.push_back(curve2->point_at(vcl_fmod(j*delta_s2, len2)));
        
      }
        

      c1 = new bsol_intrinsic_curve_2d(out_curve1);
      c2 = new bsol_intrinsic_curve_2d(out_curve2);

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

      sil_cor = new dbcvr_cv_cor(c1, c2, map, c1->size());

  } else if (open_even) {
     vcl_cout << "In open curve matching using evenly sampled normal grid!\n";

     double delta_s1 = len1/(inp1.size()-1);
      double delta_s2 = len2/(inp2.size()-1);
      
      double delta = (delta_s1 < delta_s2)? delta_s1:delta_s2;
      int n1 = (int)(len1/delta+0.5);   // round to nearest integer
      int n2 = (int)(len2/delta+0.5);

      dbcvr_cvmatch_even_sptr cvmatch = new dbcvr_cvmatch_even(curve1, curve2, n1, n2, (double)r1, template_size);
        time1 = clock();
        cvmatch->Match();  
        time2 = clock();

      double minCost = cvmatch->finalCost();
      
      vcl_vector<vsol_point_2d_sptr> out_curve1;
      vcl_vector<vsol_point_2d_sptr> out_curve2;

      vcl_vector< vcl_pair <int,int> > map = *(cvmatch->finalMap());

      delta_s1 = len1/(n1-1);  // there are n1-1 intervals (first and last points are also included in n1)
      delta_s2 = len2/(n2-1);
      for (unsigned k = 0; k<map.size(); k++) {
        int i = map[k].first;
        int j = map[k].second;
        out_curve1.push_back(curve1->point_at(i*delta_s1));
        out_curve2.push_back(curve2->point_at(j*delta_s2));
      }
        
      c1 = new bsol_intrinsic_curve_2d(out_curve1);
      c2 = new bsol_intrinsic_curve_2d(out_curve2);

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

      sil_cor = new dbcvr_cv_cor(c1, c2, map, c1->size());
  
  } else {

    /**************************************************************
     THIS PART DOES OPEN CURVE MATCHING WITH INTERP CURVE CLASS SPECIFICALLY WRITTEN 
     TO DO OPEN CURVE MATCHING
     **************************************************************/
      vcl_cout << "In open curve matching with OBLIQUE grid!\n";
     
      double L1 = (len1<len2)?len1:len2;
      double L2 = (len1<len2)?len2:len1;

      int k = static_cast<int>(vcl_ceil( L1/ ((L1+L2)/(n1+n2)) )); 

      double delta_eta = (L1*L2)/( vcl_sqrt(vcl_pow(L1,2) + vcl_pow(L2,2)) * k );

      double delta = vcl_pow(L1*L1 + L2*L2, 0.5)/(n1>n2?n1:n2);  // for now
      if (delta_given) delta = deltag;
      vcl_cout << "delta_ksi: " << delta << vcl_endl;
      vcl_cout << "delta_eta: " << delta_eta << vcl_endl;

      //open_dpmatch_2d* newdp = new open_dpmatch_2d(curve1, curve2, (double)r1, delta, interpolation_order, back_step_size);  
      dbcvr_interp_cvmatch_sptr newdp = new dbcvr_interp_cvmatch(curve1, curve2, (double)r1, delta, delta_eta);
      //newdp->setCostFlag(flag);
      //newdp->setCostFlag(false);   // cost: |ds1-ds2| + R|d_theta1-d_theta2|

      time1 = clock();
      newdp->Match();
      time2 = clock();
  
      c1 = new bsol_intrinsic_curve_2d(newdp->output_curve1());
      c2 = new bsol_intrinsic_curve_2d(newdp->output_curve2());

      curveMatch->setCurve1 (c1);
      curveMatch->setCurve2 (c2);

      vcl_cout<< vcl_endl<<"curve_2d matching cost: "<< newdp->finalCost() <<vcl_endl;
      vcl_cout<< "matching time: "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;

      assert(c1->size() == c2->size());

      vcl_vector< vcl_pair<int,int> > map;
      for (int i = 0; i<c1->size(); i++) {
        vcl_pair<int, int> p(i,i);
        map.push_back(p);
      }
      
      curveMatch->setFinalMap(map);
      
      sil_cor = new dbcvr_cv_cor(c1, c2, map, c1->size());
    }
  
  // create the output storage class
  dbcvr_curvematch_storage_sptr output_curvematch = dbcvr_curvematch_storage_new();
  output_data_[0].push_back (output_curvematch);
  output_curvematch->set_curvematch (curveMatch);
  output_curvematch->set_cv_cor (sil_cor);

  return true;
}

