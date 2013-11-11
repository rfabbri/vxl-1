/*************************************************************************
 *    FILE: shape_articulation_process.cxx
 *    DATE: Mon 11 June 2007
 *************************************************************************/


#include "shape_articulation_process.h"

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

shape_articulation_process::shape_articulation_process()
{  
            
      if(!parameters()->add( "Joint 1 angle " , "-angle_1" , 0.0 )  ||
         !parameters()->add( "Joint 2 angle " , "-angle_2" , 0.0 )  ||
         !parameters()->add( "Joint 3 angle " , "-angle_3" , 0.0 )  ||
         !parameters()->add( "Joint 4 angle " , "-angle_4" , 0.0 )  ||
         !parameters()->add( "Joint 5 angle " , "-angle_5" , 0.0 )  ||
         !parameters()->add( "Joint 6 angle " , "-angle_6" , 0.0 )  ||
         !parameters()->add( "Joint 7 angle " , "-angle_7" , 0.0 )  ||
         !parameters()->add( "Joint 8 angle " , "-angle_8" , 0.0 )  ||
         !parameters()->add( "Joint 9 angle " , "-angle_9" , 0.0 )  ||
         !parameters()->add( "Joint 10 angle " , "-angle_10" , 0.0 )||
         !parameters()->add( "Joint 2 scale " , "-scale_2" , 1.0 )  ||
         !parameters()->add( "Joint 3 scale " , "-scale_3" , 1.0 )  ||
         !parameters()->add( "Joint 4 scale " , "-scale_4" , 1.0 )  ||
         !parameters()->add( "Joint 5 scale " , "-scale_5" , 1.0 )  ||
         !parameters()->add( "Joint 6 scale " , "-scale_6" , 1.0 )  ||
         !parameters()->add( "Joint 7 scale " , "-scale_7" , 1.0 )  ||
         !parameters()->add( "Joint 8 scale " , "-scale_8" , 1.0 )  ||
         !parameters()->add( "Joint 9 scale " , "-scale_9" , 1.0 )  ||
         !parameters()->add( "Joint 10 scale " , "-scale_10" , 1.0 )  ||
         !parameters()->add( "Lie Costs file <filename...>" , "-filename", bpro1_filepath("","*")))
         {
    vcl_cerr << "ERROR: Adding parameters in shape_articulation_process::shape_articulation_process()" << vcl_endl;
}
}

//: Clone the process
bpro1_process*
shape_articulation_process::clone() const
{
  return new shape_articulation_process(*this);
}

vcl_vector<vsol_point_2d_sptr> shape_articulation_process::closed_articulated_structure(vcl_vector<vsol_point_2d_sptr> final_points)
                                                                                        
    {
    vcl_vector<vsol_point_2d_sptr> closed_form;

    for (unsigned int i = 0;i<final_points.size();i++)
        {
        vsol_point_2d_sptr pt= new vsol_point_2d(final_points[i]->x(),final_points[i]->y());
        closed_form.push_back(pt);
        }

    vcl_vector<double> angle_2_vec,length_2_vec;
    unsigned int num_points = final_points.size(),start_point;
    double initial_curve_length = 0,final_curve_length = 0,common_scale,scale,angle_2,diff_angle,gap_length,gap_x_comp,gap_y_comp;
    double length_2,pivot_x,pivot_y,scale_x,scale_y,thet;
    double pt1_x,pt1_y,pt2_x,pt2_y,ref_angle,tx,ty,tot_length,tot_weighted_length,length_to_add,length_add_x_comp,length_add_y_comp;
    double x_comp_r1_lb,x_comp_r1_ub,x_comp_r2_lb,x_comp_r2_ub,y_comp_lb,y_comp_ub;

  
    ref_angle = vcl_atan2((closed_form[num_points-1]->y() - closed_form[0]->y()),(closed_form[num_points-1]->x() - closed_form[0]->x()));

    if ((ref_angle > 0 )&& (ref_angle < vnl_math::pi/2))
        {
        x_comp_r1_lb = vnl_math::pi/2;
        x_comp_r1_ub = vnl_math::pi;

        x_comp_r2_lb = -1*vnl_math::pi;
        x_comp_r2_ub = -1*vnl_math::pi/2;

        y_comp_lb = -vnl_math::pi;
        y_comp_ub = 0;
        }

    if ((ref_angle > vnl_math::pi/2 )&& (ref_angle < vnl_math::pi))
        {
        x_comp_r1_lb = 0;
        x_comp_r1_ub = vnl_math::pi/2;

        x_comp_r2_lb = -1*vnl_math::pi/2;
        x_comp_r2_ub = 0;

        y_comp_lb = -vnl_math::pi;
        y_comp_ub = 0;
        }

     if ((ref_angle > -1*vnl_math::pi )&& (ref_angle < -1*vnl_math::pi/2))
        {
        x_comp_r1_lb = 0;
        x_comp_r1_ub = vnl_math::pi/2;

        x_comp_r2_lb = -1*vnl_math::pi/2;
        x_comp_r2_ub = 0;

        y_comp_lb = 0;
        y_comp_ub = vnl_math::pi;
        }

    
     if ((ref_angle > -1*vnl_math::pi/2 )&& (ref_angle < 0))
        {
        x_comp_r1_lb = vnl_math::pi/2;
        x_comp_r1_ub = vnl_math::pi;

        x_comp_r2_lb = -1*vnl_math::pi;
        x_comp_r2_ub = -1*vnl_math::pi/2;

        y_comp_lb = 0;
        y_comp_ub = vnl_math::pi;
        }


    unsigned int count = 0,num_x = 0,num_y = 0;

    for (unsigned int i = 0;i<num_points-1;i++)
        {
        angle_2 = vcl_atan2(closed_form[i+1]->y()-closed_form[i]->y(),closed_form[i+1]->x()-closed_form[i]->x());
        length_2 = closed_form[i]->distance(closed_form[i+1]);
        angle_2_vec.push_back(angle_2);
        length_2_vec.push_back(length_2);

        final_curve_length += length_2;

        if ((((angle_2 <= x_comp_r1_ub) && 
            (angle_2 >= x_comp_r1_lb)) || ((angle_2 <= x_comp_r2_ub)&&(angle_2 >= x_comp_r2_lb))))
            {
            if (((angle_2 >= y_comp_lb) && (angle_2 <= y_comp_ub))||(vcl_fabs(vcl_fabs(angle_2) - vnl_math::pi) < 1e-10))
                {
                if (vcl_fabs(vcl_cos(angle_2)) > 1e-10)
                    num_x++;

                if (vcl_fabs(vcl_sin(angle_2)) > 1e-10)
                    num_y++;
                }
            }
        }

    gap_length = closed_form[num_points-1]->distance(closed_form[0]);
    gap_x_comp = gap_length*vcl_cos(ref_angle);
    gap_y_comp = gap_length*vcl_sin(ref_angle);

    length_add_x_comp = vcl_fabs(gap_x_comp)/num_x;
    length_add_y_comp = vcl_fabs(gap_y_comp)/num_y;



    for (unsigned int i = 1;i<num_points;i++)
        {
        angle_2 = angle_2_vec[i-1];
        scale_x = 1;
        scale_y = 1;
       
        if ((((angle_2 <= x_comp_r1_ub) && 
            (angle_2 >= x_comp_r1_lb)) || ((angle_2 <= x_comp_r2_ub)&&(angle_2 >= x_comp_r2_lb))))
            {
            if (((angle_2 >= y_comp_lb) && (angle_2 <= y_comp_ub))||(vcl_fabs(vcl_fabs(angle_2) - vnl_math::pi) < 1e-10))
                {
                if (vcl_fabs(vcl_cos(angle_2)) > 1e-10)
                    scale_x = 1 + length_add_x_comp/(length_2_vec[i-1]*vcl_fabs(vcl_cos(angle_2)));

                if (vcl_fabs(vcl_sin(angle_2)) > 1e-10)
                    scale_y = 1 + length_add_y_comp/(length_2_vec[i-1]*vcl_fabs(vcl_sin(angle_2)));
                }
            }

     // scale = vcl_sqrt(scale_x*scale_x*vcl_cos(angle_2)*vcl_cos(angle_2) + scale_y*scale_y*vcl_sin(angle_2)*vcl_sin(angle_2));

      pivot_x = closed_form[i-1]->x();
      pivot_y = closed_form[i-1]->y();

       pt1_x = closed_form[i]->x() - pivot_x;
       pt1_y = closed_form[i]->y() - pivot_y;
       pt2_x = pt1_x*scale_x + pivot_x;
       pt2_y = pt1_y*scale_y + pivot_y;

       tx = pt1_x*(scale_x-1);
       ty = pt1_y*(scale_y-1);

       for (unsigned int j = i+1;j<closed_form.size();j++)
           {
           closed_form[j]->set_x(closed_form[j]->x() + tx);
           closed_form[j]->set_y(closed_form[j]->y() + ty);
           }

       closed_form[i]->set_x(pt2_x);
       closed_form[i]->set_y(pt2_y);
        }

    return closed_form;
    }

/*************************************************************************
 * Function Name: shape_articulation_process::execute
 * Parameters:
 * Returns: bool
 * Effects:
 *************************************************************************/
//: curve1 in input_spatial_object_[0], build curve from polyline (digital curve)
//: curve2 in input_spatial_object_[1]
bool shape_articulation_process::execute()
{
    bpro1_filepath file_path;
    vcl_vector<double> thet_joint;
    vcl_vector<double> scale;

    unsigned int i;

    for (i = 0;i<10;i++)
        scale.push_back(1);

    double thet_joint_1,thet_joint_2,thet_joint_3,thet_joint_4,thet_joint_5;
    double thet_joint_6,thet_joint_7,thet_joint_8,thet_joint_9,thet_joint_10;

    parameters()->get_value( "-angle_1" ,thet_joint_1);
    parameters()->get_value( "-angle_2" ,thet_joint_2);
    parameters()->get_value( "-angle_3" ,thet_joint_3);
    parameters()->get_value( "-angle_4" ,thet_joint_4);
    parameters()->get_value( "-angle_5" ,thet_joint_5);
    parameters()->get_value( "-angle_6" ,thet_joint_6);
    parameters()->get_value( "-angle_7" ,thet_joint_7);
    parameters()->get_value( "-angle_8" ,thet_joint_8);
    parameters()->get_value( "-angle_9" ,thet_joint_9);
    parameters()->get_value( "-angle_10" ,thet_joint_10);

     i = 1;

    parameters()->get_value( "-scale_2" ,scale[i++]);
    parameters()->get_value( "-scale_3" ,scale[i++]);
    parameters()->get_value( "-scale_4" ,scale[i++]);
    parameters()->get_value( "-scale_5" ,scale[i++]);
    parameters()->get_value( "-scale_6" ,scale[i++]);
    parameters()->get_value( "-scale_7" ,scale[i++]);
    parameters()->get_value( "-scale_8" ,scale[i++]);
    parameters()->get_value( "-scale_9" ,scale[i++]);
    parameters()->get_value( "-scale_10" ,scale[i++]);

    parameters()->get_value( "-filename" ,file_path);

    thet_joint.push_back(thet_joint_1);
    thet_joint.push_back(thet_joint_2);
    thet_joint.push_back(thet_joint_3);
    thet_joint.push_back(thet_joint_4);
    thet_joint.push_back(thet_joint_5);
    thet_joint.push_back(thet_joint_6);
    thet_joint.push_back(thet_joint_7);
    thet_joint.push_back(thet_joint_8);
    thet_joint.push_back(thet_joint_9);
    thet_joint.push_back(thet_joint_10);

  clear_output();

  //----------------------------------
  // get input vsol (two polygons)
  //----------------------------------
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);

  // The contour needs to be a polygon
  vsol_polyline_2d_sptr poly1;
  {
    const vcl_vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol1->all_data();
    poly1 = vsol_list[0]->cast_to_curve()->cast_to_polyline();
 
  }

  vcl_vector<vsol_point_2d_sptr> vertex_set,vertex_set_1;

  for (unsigned int i = 0;i<poly1->size();i++)
      {
vsol_point_2d_sptr pt = poly1->vertex(i);
vsol_point_2d_sptr pt_1 = new vsol_point_2d();
pt_1->set_x(poly1->vertex(i)->x());
pt_1->set_y(poly1->vertex(i)->y());
vertex_set.push_back(pt);
vertex_set_1.push_back(pt_1);
      }

    vsol_polyline_2d output_polyline;
     vcl_vector<vsol_point_2d_sptr> points;
  double pt1_x,pt1_y,pt2_x,pt2_y,thet,joint_ang,x_val,y_val,tx,ty,pivot_x = 0,pivot_y = 0;
 
  for (unsigned int i = 0;i<vertex_set_1.size();i++)
      {
      if (i != 0)
          {
      pivot_x = vertex_set_1[i-1]->x();
      pivot_y = vertex_set_1[i-1]->y();
          }

       pt1_x = vertex_set_1[i]->x() - pivot_x;
       pt1_y = vertex_set_1[i]->y() - pivot_y;
       pt2_x = pt1_x*scale[i] + pivot_x;
       pt2_y = pt1_y*scale[i] + pivot_y;

       tx = pt1_x*(scale[i]-1);
       ty = pt1_y*(scale[i]-1);

       for (unsigned int j = i+1;j<vertex_set_1.size();j++)
           {
           vertex_set_1[j]->set_x(vertex_set_1[j]->x() + tx);
           vertex_set_1[j]->set_y(vertex_set_1[j]->y() + ty);
           }

       vertex_set_1[i]->set_x(pt2_x);
       vertex_set_1[i]->set_y(pt2_y);
      }

  for (unsigned int i = 0;i<vertex_set_1.size();i++)
      {
      vcl_cout << "vertex set: " << vertex_set_1[i]->x() << " " << vertex_set_1[i]->y() << vcl_endl;
      }

        for (unsigned int i = 0;i<vertex_set_1.size();i++)
      {
     thet = thet_joint[i]*(vnl_math::pi)/180;

     if (i != 0)
         {
     pivot_x = points[i-1]->x();
     pivot_y = points[i-1]->y();
         }
     else
         {
         pivot_x = 0;
         pivot_y = 0;
         }
    
     pt1_x = vertex_set_1[i]->x() - pivot_x;
     pt1_y = vertex_set_1[i]->y() - pivot_y;

      pt2_x = pt1_x*vcl_cos(thet) - pt1_y*vcl_sin(thet);
      pt2_y = pt1_x*vcl_sin(thet) + pt1_y*vcl_cos(thet);

      pt2_x = pt2_x + pivot_x;
      pt2_y = pt2_y + pivot_y;

      vsol_point_2d_sptr pt = new vsol_point_2d(pt2_x,pt2_y);
      points.push_back(pt);

        for (unsigned int j = i+1;j < vertex_set_1.size();j++)
         {
         x_val = vertex_set_1[j]->x() - pivot_x;
         y_val = vertex_set_1[j]->y() - pivot_y;

        vertex_set_1[j]->set_x(x_val*vcl_cos(thet) - y_val*vcl_sin(thet) + pivot_x);
        vertex_set_1[j]->set_y(x_val*vcl_sin(thet) + y_val*vcl_cos(thet) + pivot_y);
         }
      }

  for (unsigned int i =0;i<points.size();i++)
      {
      vcl_cout << "points: " << points[i]->x() << " " << points[i]->y() << vcl_endl;
      }
 
  vcl_string save_fname = file_path.path;
  double length_1,length_2,angle_1,angle_2;
  vcl_vector<double> scale_comp,angle_comp;
  vcl_ofstream ofst(save_fname.c_str());

  /* for (unsigned int i = 0;i<vertex_set.size();i++)
      {
      ofst << "vertex set:" << vertex_set[i]->x() << " " << vertex_set[i]->y() << vcl_endl;
      }

  for (unsigned int i = 0;i<points.size();i++)
      {
      ofst << " " << points[i]->x() << " " << points[i]->y() << vcl_endl;
      }*/

  for (unsigned int i = 0;i<vertex_set.size()-1;i++)
      {
      length_1 = vertex_set[i]->distance(vertex_set[i+1]);
      length_2 = points[i]->distance(points[i+1]);

      angle_1 = vcl_atan2(vertex_set[i+1]->y()-vertex_set[i]->y(),vertex_set[i+1]->x()-vertex_set[i]->x());
      angle_2 = vcl_atan2(points[i+1]->y()-points[i]->y(),points[i+1]->x()-points[i]->x());

      // ofst << " " << i << " scale: " << vcl_log(length_2/length_1) << " angle: " << angle_2 - angle_1 <<  vcl_endl;
      // ofst << " " << vcl_log(length_2/length_1) << " " << angle_2 - angle_1 <<  vcl_endl;

      scale_comp.push_back(vcl_log(length_2/length_1));
      angle_comp.push_back( angle_2 - angle_1 );
      }

  if (!poly1) {
    vcl_cout << "polyline one is not valid.\n";
    return false;
  }

 /*double alpha,beta,min_alpha,min_beta,cost_temp,min_cost = 1e100,count = 0;
   vcl_vector<double> cost;
  for  (alpha = 0;alpha <= 1;alpha = alpha + 0.01)
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
   vcl_cout << " dist: " << count++ << " " << cost_temp << vcl_endl;
      }*/
//ofst << "dist " << min_cost << vcl_endl;
//ofst << "alpha: " << min_alpha << vcl_endl;
//ofst << "beta: " << min_beta << vcl_endl;

vcl_vector<vsol_point_2d_sptr> closed_points = closed_articulated_structure(points);
ofst << "transformed articulated structure: " << vcl_endl;

for (unsigned int i = 0;i<points.size();i++)
ofst << points[i]->x() << " " << points[i]->y() << vcl_endl;

ofst << "closed articulated structure: " << vcl_endl;

for (unsigned int i = 0;i<closed_points.size();i++)
ofst << closed_points[i]->x() << " " << closed_points[i]->y() << vcl_endl;

  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour
  vcl_vector< vsol_spatial_object_2d_sptr > articulated_structure;
  vsol_polyline_2d_sptr newpolyline = new vsol_polyline_2d (closed_points);
 // vsol_polyline_2d_sptr polyline2 = new vsol_polyline_2d (points);
  articulated_structure.push_back(newpolyline->cast_to_spatial_object());
 // articulated_structure.push_back(polyline2->cast_to_spatial_object());

 vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
 output_vsol->add_objects(articulated_structure);

   output_data_[0].push_back(output_vsol);

  return true;
}


