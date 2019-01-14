/*************************************************************************
 *    FILE: Lie_contour_match_process.cxx
 *    DATE: Mon 11 June 2007
 *************************************************************************/


#include "Lie_contour_match_process.h"

#include <ctime>
#include <algorithm>
#include <cstdio>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbgl/dbgl_eno_curve.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>

#include <manifold_extraction/Lie_cv_cor.h>
#include <manifold_extraction/Lie_cv_cor_sptr.h>
#include <manifold_extraction/Lie_contour_match.h>
#include <manifold_extraction/Lie_contour_match_sptr.h>

/*************************************************************************
 * Function Name: Lie_contour_match_process::Lie_contour_match_process
 *************************************************************************/

Lie_contour_match_process::Lie_contour_match_process()
{  
            
      if(!parameters()->add( "Do closed curve matching" , "-closedversion" , false )  ||
      !parameters()->add( "Template size: " , "-template_size" , 3 )) {
    std::cerr << "ERROR: Adding parameters in Lie_contour_match_process::Lie_contour_match_process()" << std::endl;
}
}

//: Clone the process
bpro1_process*
Lie_contour_match_process::clone() const
{
  return new Lie_contour_match_process(*this);
}

/*************************************************************************
 * Function Name: Lie_contour_match_process::execute
 * Parameters:
 * Returns: bool
 * Effects:
 *************************************************************************/
// curve1 in input_spatial_object_[0]
// curve2 in input_spatial_object_[1]
bool Lie_contour_match_process::execute()
{
  bool closed_version; parameters()->get_value( "-closedversion" , closed_version );
  int template_size=0;   parameters()->get_value( "-template_size" , template_size );
  float r1=0;            parameters()->get_value( "-r1" , r1 );
  

  clear_output();

  //----------------------------------
  // get input vsol (two polygons)
  //----------------------------------
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);

  vidpro1_vsol2D_storage_sptr input_vsol2;
  input_vsol2.vertical_cast(input_data_[0][1]);

  // The contour needs to be a polygon
  vsol_polygon_2d_sptr poly1;
  {
    const std::vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol1->all_data();
    poly1 = vsol_list[0]->cast_to_region()->cast_to_polygon();
  }

  //the second polygon
  vsol_polygon_2d_sptr poly2;
  {
    const std::vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol2->all_data();
    poly2 = vsol_list[0]->cast_to_region()->cast_to_polygon();
  }

  if (!poly1 || !poly2) {
    std::cout << "one of the polygons is not valid.\n";
    return false;
  }
/*
  // get input storage class
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);

  vidpro1_vsol2D_storage_sptr input_vsol2;
  input_vsol2.vertical_cast(input_data_[1][0]);
*/
  // no open-close curve notion in vsol2D
  /*bsol_intrinsic_curve_2d_sptr curve1 = new bsol_intrinsic_curve_2d();
  bsol_intrinsic_curve_2d_sptr curve2 = new bsol_intrinsic_curve_2d();*/

  std::vector<vsol_point_2d_sptr> pointset1;
  std::vector<vsol_point_2d_sptr> pointset2;

  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour
  {
    const std::vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol1->all_data();
    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
    {
      if( vsol_list[b]->cast_to_curve())
      {
        if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
        {
          for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
          {
            vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
            pointset1.push_back(pt);
      } } }
  
      if( vsol_list[b]->cast_to_region())
      {
        if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
        {
          for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
          {
            vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
            pointset1.push_back(pt);
          }
        }
      }
    }
  }

  {
    const std::vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol2->all_data();
    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
    {
      if( vsol_list[b]->cast_to_curve())
      {
        if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
        {
          for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
          {
            vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
            pointset2.push_back(pt);
      } } }
  
      if( vsol_list[b]->cast_to_region())
      {
        if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
        {
          for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
          {
            vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
            pointset2.push_back(pt);
          }
        }
      }
    }
  }

 /* std::cout << "pointset 1: " << std::endl;
  
  for(std::vector<vsol_point_2d_sptr>::iterator it = pointset1.begin();it != pointset1.end();it++)
      {
      (*it)->print(std::cout);
      }

   std::cout << "pointset 2: " << std::endl;

    for(std::vector<vsol_point_2d_sptr>::iterator it = pointset2.begin();it != pointset2.end();it++)
      {
      (*it)->print(std::cout);
      }*/

   dbsol_interp_curve_2d curve1;
   vnl_vector<double> samples1;
   dbsol_curve_algs::interpolate_eno(&curve1,pointset1,samples1);

   dbsol_interp_curve_2d curve2;
   vnl_vector<double> samples2;
   dbsol_curve_algs::interpolate_eno(&curve2,pointset2,samples2);

   std::cout << "printing curve 1: " << std::endl;
   curve1.print(std::cout);

   std::cout << "printing curve 2: " << std::endl;
   curve2.print(std::cout);

  Lie_contour_match* contourMatch = new Lie_contour_match();   // template size is 3 in this case
  
  /*contourMatch->setCurve1 (&curve1);
  contourMatch->setCurve2 (&curve2);*/
 
  dbsol_interp_curve_2d_sptr curve1_sptr = new dbsol_interp_curve_2d();
  curve1_sptr->make(curve1.intervals());

   
  dbsol_interp_curve_2d_sptr curve2_sptr = new dbsol_interp_curve_2d();
  curve2_sptr->make(curve2.intervals());

  contourMatch->setCurve1(curve1_sptr);
  contourMatch->setCurve2(curve2_sptr);

   Lie_contour_match_sptr matching = new Lie_contour_match(curve1_sptr,curve2_sptr,curve1.size(), curve2.size(), r1, template_size);
 
   matching->Match();

  std::vector<double> cost_vec = matching->finalCost();

        double min_cost = 1e20;
        int min_idx = -1; // infinity

        for (unsigned int i = 0;i<cost_vec.size();i++)
            {
            std::cout << "cost vector: " << i << " " << cost_vec[i] << std::endl;
            if (min_cost > cost_vec[i])
                {
                min_idx = i;
                min_cost = cost_vec[i];
                }
            }
        std::vector< std::vector<double> > DP_costs = matching->DPcost();

         FinalMapType fmap = matching->finalMap(min_idx);
         std::cout << "matching cost: " << cost_vec[min_idx] << std::endl;

         std::cout << "final map: " << std::endl;

         for (unsigned int i = 0;i<fmap.size();i++)
         std::cout << fmap[i].first << " " << fmap[i].second << std::endl;

         int idx_1,idx_2;

            for (unsigned int j = 0;j<fmap.size();j++)
                {
                idx_1 = fmap[j].first;
                idx_2 = fmap[j].second;
                std::cout << " " << idx_1 << " " << idx_2 << " " << DP_costs[idx_1][idx_2] << std::endl;
                }
            std::cout << "printing DP cost array: " << DP_costs[matching->n1() + min_idx - 1][matching->n2()-1] << std::endl;

         Lie_contour_match_storage_sptr output_curvematch = Lie_contour_match_storage_new();

         double test_cost = matching->computeLieIntervalCost(29,29,16,17);

         output_data_[0].push_back (output_curvematch);
        
         output_curvematch->set_curvematch (matching);
       
         // Lie_cv_cor_sptr cv_cor = new Lie_cv_cor(&curve1,&curve2,fmap);
          Lie_cv_cor_sptr cv_cor = new Lie_cv_cor(curve1_sptr,curve2_sptr,fmap);

          std::vector<vgl_point_2d<double> > ptset_1 = cv_cor->get_contour_pts1();
          
         
         output_curvematch->set_cv_cor(cv_cor);



  return true;
}

