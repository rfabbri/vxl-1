/*************************************************************************
 *    FILE: Lie_cost_geodesics_process.cxx
 *    DATE: Mon 11 June 2007
 *************************************************************************/


#include "Lie_cost_geodesics_process.h"

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbgl/dbgl_eno_curve.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <vidpro1/process/vidpro1_load_con_process.h>

Lie_cost_geodesics_process::Lie_cost_geodesics_process()
{  
       
if(!parameters()->add( "samples curve 1: " , "-num_samples_c1" , 100 )  ||
         !parameters()->add( "samples curve 2: " , "-num_samples_c2" , 100 )  ||
          !parameters()->add( "Lie Costs file <filename...>" , "-filename", bpro1_filepath("","*")))
         {
    vcl_cerr << "ERROR: Adding parameters in shape_articulation_process::shape_articulation_process()" << vcl_endl;
}
}

//: Clone the process
bpro1_process*
Lie_cost_geodesics_process::clone() const
{
  return new Lie_cost_geodesics_process(*this);
}

/*************************************************************************
 * Function Name: Lie_cost_geodesics_process::execute
 * Parameters:
 * Returns: bool
 * Effects:
 *************************************************************************/

double Lie_cost_geodesics_process::compute_lie_cost( vcl_vector<vsol_point_2d_sptr> curve1_samples, vcl_vector<vsol_point_2d_sptr> curve2_samples)
    {
 double length_1,length_2,angle_1,angle_2,lie_cost = 0;
  vcl_vector<double> scale_comp,angle_comp;

  for (unsigned int i = 0;i<curve1_samples.size()-1;i++)
      {
      length_1 = curve1_samples[i]->distance(curve1_samples[i+1]);
      length_2 = curve2_samples[i]->distance(curve2_samples[i+1]);

      angle_1 = vcl_atan2(curve1_samples[i+1]->y()-curve1_samples[i]->y(),curve1_samples[i+1]->x()-curve1_samples[i]->x());
      angle_2 = vcl_atan2(curve2_samples[i+1]->y()-curve2_samples[i]->y(),curve2_samples[i+1]->x()-curve2_samples[i]->x());

      scale_comp.push_back(vcl_log(length_2/length_1));
      angle_comp.push_back( angle_2 - angle_1 );

      lie_cost = lie_cost + (vcl_fabs(scale_comp[i]))*(vcl_fabs(scale_comp[i])) + 
           (vcl_fabs(angle_comp[i]))*(vcl_fabs(angle_comp[i]));
      }
  return lie_cost;
    }

double Lie_cost_geodesics_process::compute_kimia_cost( vcl_vector<vsol_point_2d_sptr> curve1_samples, vcl_vector<vsol_point_2d_sptr> curve2_samples,
                                                      vcl_vector<double> tangent_angle_c1,vcl_vector<double> tangent_angle_c2)
    {
    double length_1,length_2,kimia_cost = 0;

    for (unsigned int i = 0;i<curve1_samples.size()-1;i++)
        {
        length_1 = curve1_samples[i]->distance(curve1_samples[i+1]);
        length_2 = curve2_samples[i]->distance(curve2_samples[i+1]);

        kimia_cost = kimia_cost + ((length_1 - length_2)*(length_1 - length_2))/(length_1 + length_2) + 
            (vcl_fabs(tangent_angle_c1[i] - tangent_angle_c2[i]));
        }

   return kimia_cost;
    }

bool Lie_cost_geodesics_process::execute()
{
 bpro1_filepath file_path;
 int num_samples_c1,num_samples_c2;
 
    parameters()->get_value("-num_samples_c1",num_samples_c1);
    parameters()->get_value("-num_samples_c2",num_samples_c2);
    parameters()->get_value( "-filename" ,file_path);
    

  vcl_string save_fname = file_path.path;
  vcl_ofstream ofst(save_fname.c_str());

  clear_output();

  //----------------------------------
  // get input vsol (two polygons)
  //----------------------------------
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);

  vidpro1_vsol2D_storage_sptr input_vsol2;
  input_vsol2.vertical_cast(input_data_[0][1]);

  // The contour needs to be a polygon
  vsol_polyline_2d_sptr poly1;
  {
    const vcl_vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol1->all_data();
    poly1 = vsol_list[0]->cast_to_curve()->cast_to_polyline();
 
  }

   vsol_polyline_2d_sptr poly2;
  {
    const vcl_vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol2->all_data();
    poly2 = vsol_list[0]->cast_to_curve()->cast_to_polyline();
  }

    if (!poly1 || !poly2) {
    vcl_cout << "one of the polygons is not valid.\n";
    return false;
  }

  vcl_vector<vsol_point_2d_sptr> pointset1,pointset2,curve1_samples,curve2_samples;

  for (unsigned int i = 0;i<poly1->size();i++)
      {
vsol_point_2d_sptr pt = poly1->vertex(i);
pointset1.push_back(pt);
      }

  for (unsigned int i = 0;i<poly2->size();i++)
      {
vsol_point_2d_sptr pt = poly2->vertex(i);
pointset2.push_back(pt);
      }

   dbsol_interp_curve_2d curve1;
   vnl_vector<double> samples1;
   dbsol_curve_algs::interpolate_eno(&curve1,pointset1,samples1);

   dbsol_interp_curve_2d curve2;
   vnl_vector<double> samples2;
   dbsol_curve_algs::interpolate_eno(&curve2,pointset2,samples2);

   double s;
   vcl_vector<double> tangent_angle_c1,tangent_angle_c2;
   
   for (unsigned int i = 0;i<num_samples_c1;i++)
       {
    s = (double(i)/double(num_samples_c1))*curve1.length();

   vsol_point_2d_sptr sample = curve1.point_at(s);

   curve1_samples.push_back(sample);

   tangent_angle_c1.push_back(curve1.tangent_angle_at(s));

   // vcl_cout << "curve 1: " << " " << s << " " << sample->x() << " " << sample->y() << vcl_endl;
       }

   for (unsigned int i = 0;i<num_samples_c2;i++)
       {
    s = (double(i)/double(num_samples_c2))*curve2.length();

   vsol_point_2d_sptr sample = curve2.point_at(s);

   curve2_samples.push_back(sample);

    tangent_angle_c2.push_back(curve2.tangent_angle_at(s));

   // vcl_cout << "curve 2: " << " " << s << " " << sample->x() << " " << sample->y() << vcl_endl;
       }

   /*vcl_cout << "printing curve 1: " << vcl_endl;
   curve1.print(vcl_cout);

   vcl_cout << "printing curve 2: " << vcl_endl;
   curve2.print(vcl_cout);*/

    vsol_polyline_2d output_polyline;

 

   double alpha,beta,min_alpha,min_beta,lie_cost,kimia_cost,min_cost = 1e100;
   vcl_vector<double> cost;

  /*for  (alpha = 0;alpha <= 1;alpha = alpha + 0.01)
      {
   beta = 1 - alpha;
   cost_temp = 0;
   for (unsigned int i = 0;i<scale_comp.size();i++)
       {
       cost_temp = cost_temp + (alpha*vcl_fabs(scale_comp[i]))*(alpha*vcl_fabs(scale_comp[i])) + 
           (beta*vcl_fabs(angle_comp[i]))*(beta*vcl_fabs(angle_comp[i]));
       }
   cost.push_back(cost_temp);

   if (cost_temp < min_cost)
       {
       min_alpha = alpha;
       min_beta = beta;
       min_cost = cost_temp;
       }

      }
      
      ofst << "temporary cost " << cost_temp << vcl_endl;
ofst << "dist " << min_cost << vcl_endl;
ofst << "alpha: " << min_alpha << vcl_endl;
ofst << "beta: " << min_beta << vcl_endl;*/   

lie_cost = compute_lie_cost(curve1_samples, curve2_samples);
kimia_cost = compute_kimia_cost(curve1_samples, curve2_samples,tangent_angle_c1,tangent_angle_c2);

ofst << "Lie cost " << lie_cost << vcl_endl;
ofst << "alpha: " <<alpha << vcl_endl;
ofst << "beta: " << beta << vcl_endl;
ofst << "Kimia cost " << kimia_cost << vcl_endl;

  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour
  vcl_vector< vsol_spatial_object_2d_sptr > articulated_structure;
  vsol_polyline_2d_sptr newpolyline = new vsol_polyline_2d (curve2_samples);
  articulated_structure.push_back(newpolyline->cast_to_spatial_object());

 vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
 output_vsol->add_objects(articulated_structure);

   output_data_[0].push_back(output_vsol);

  return true;
}

