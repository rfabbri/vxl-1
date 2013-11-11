/*************************************************************************
*    NAME: pradeep
*    FILE: Lie_contour_generate_geodesic_shapes.cxx
*    DATE: 17 July 2007
*************************************************************************/
#include "Lie_contour_generate_geodesic_shapes.h"
#include <manifold_extraction/Lie_contour_utilities.h>
#include <manifold_extraction/Lie_spoke_utilities.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>

/*************************************************************************
* Function Name: Lie_contour_generate_geodesic_shapes::Lie_contour_generate_geodesic_shapes
* Parameters: 
* Effects: 
*************************************************************************/

Lie_contour_generate_geodesic_shapes::Lie_contour_generate_geodesic_shapes()
    {
    if (!parameters()->add( "samples curve 1: " , "-num_samples_c1" , 100 )  ||
        !parameters()->add( "samples curve 2: " , "-num_samples_c2" , 100 )  ||
        !parameters()->add( "Contour file list <filename...>" , "-contour_file_list", bpro1_filepath("","*")) ||
        !parameters()->add( "Output contour file <filename...>" , "-output_contour_file", bpro1_filepath("","*")))
        {
        vcl_cerr << "ERROR: Adding parameters in Lie_contour_generate_geodesic_shapes::Lie_contour_generate_geodesic_shapes()" << vcl_endl;
        }

    }

//: Clone the process
bpro1_process*
Lie_contour_generate_geodesic_shapes::clone() const
    {
    return new Lie_contour_generate_geodesic_shapes(*this);
    }



double Lie_contour_generate_geodesic_shapes::compute_lie_cost(vcl_vector<vsol_point_2d_sptr> curve1_samples,vcl_vector<vsol_point_2d_sptr> curve2_samples )
    {
    double lie_cost = 0,length_1,length_2,angle_1,angle_2,scale_comp,angle_comp;

    for (unsigned int i = 0;i<curve1_samples.size()-1;i++)
        {
        length_1 = curve1_samples[i]->distance(curve1_samples[i+1]);
        length_2 = curve2_samples[i]->distance(curve2_samples[i+1]);

        angle_1 = vcl_atan2(curve1_samples[i+1]->y()-curve1_samples[i]->y(),curve1_samples[i+1]->x()-curve1_samples[i]->x());
        angle_2 = vcl_atan2(curve2_samples[i+1]->y()-curve2_samples[i]->y(),curve2_samples[i+1]->x()-curve2_samples[i]->x());

        length_1 = (vcl_fabs(length_1) < 1e-10) ? 1:length_1;
        length_2 = (vcl_fabs(length_2) < 1e-10) ? 1:length_2;

        scale_comp = vcl_log(length_2/length_1);
        angle_comp = angle_2 - angle_1;
        lie_cost = lie_cost +  scale_comp*scale_comp + (angle_comp*angle_comp);
        }
    lie_cost = vcl_sqrt(lie_cost);

    return lie_cost;
    }

void Lie_contour_generate_geodesic_shapes::angles_scales(vcl_vector<vsol_point_2d_sptr> curve1,vcl_vector<vsol_point_2d_sptr> curve2,
                                             vcl_vector<double> &angles,vcl_vector<double> &scales)
    {
    double length_1,length_2,angle_1,angle_2,Lie_cost,min_lie_cost = 1e100;
    unsigned int min_lie_index;
    vcl_vector <double> Lie_cost_vec;

    vcl_vector<vsol_point_2d_sptr> curve2_augmented;

    for (unsigned int j = 0;j<2;j++)
        for (unsigned int i = 0;i<curve2.size();i++)
            curve2_augmented.push_back(curve2[i]);

    vcl_vector<vsol_point_2d_sptr> curve2_open_samples;

    for (unsigned int k = 0;k<curve2.size();k++)
        {
        curve2_open_samples.clear();

        for (unsigned int i = k;i < k + curve2.size();i++)
            {
            curve2_open_samples.push_back(curve2_augmented[i]);
            }

        Lie_cost = compute_lie_cost(curve1,curve2_open_samples );
        Lie_cost_vec.push_back(Lie_cost);
        }


    for (unsigned int i = 0;i < Lie_cost_vec.size();i++)
        {
        if (Lie_cost_vec[i] < min_lie_cost)
            {
            min_lie_cost = Lie_cost_vec[i];
            min_lie_index = i;
            }
        }

    curve2_open_samples.clear();

    for (unsigned int i = min_lie_index;i < min_lie_index + curve2.size();i++)
        {
        curve2_open_samples.push_back(curve2_augmented[i]);
        }

    for (unsigned int i = 0;i<curve1.size()-1;i++)
        {
        length_1 = curve1[i]->distance(curve1[i+1]);
        length_2 = curve2_open_samples[i]->distance(curve2_open_samples[i+1]);

        angle_1 = vcl_atan2(curve1[i+1]->y()-curve1[i]->y(),curve1[i+1]->x()-curve1[i]->x());
        angle_2 = vcl_atan2(curve2_open_samples[i+1]->y()-curve2_open_samples[i]->y(),curve2_open_samples[i+1]->x()-curve2_open_samples[i]->x());



        scales.push_back(length_2/length_1);
        angles.push_back( angle_2 - angle_1 );
       // vcl_cout << "angles: " << angle_2 - angle_1 << vcl_endl;
        }
    }

vcl_vector<vsol_point_2d_sptr> Lie_contour_generate_geodesic_shapes::transform_shape(vcl_vector<vsol_point_2d_sptr> curve,
                                             vcl_vector<double> angles,vcl_vector<double> scales)
    {
    vcl_vector<vsol_point_2d_sptr> curve1,new_shape;

    for (unsigned int i = 0;i<curve.size();i++)
        {
        vsol_point_2d_sptr pt = new vsol_point_2d(curve[i]->x(),curve[i]->y());
        curve1.push_back(pt);
        }

    double pivot_x,pivot_y, pt1_x, pt1_y, pt2_x, pt2_y,thet,tx,ty,x_val,y_val,sum_ang;

for (unsigned int i = 0;i < angles.size();i++)
        {
        sum_ang = 0;

        for (unsigned int k = 0;k<i;k++)
            sum_ang += angles[k];

        angles[i] = angles[i] - sum_ang;
        }

    for (unsigned int i = 1;i<curve1.size();i++)
        {
        if (i != 0)
            {
            pivot_x = curve1[i-1]->x();
            pivot_y = curve1[i-1]->y();
            }

        pt1_x = curve1[i]->x() - pivot_x;
        pt1_y = curve1[i]->y() - pivot_y;
        pt2_x = pt1_x*scales[i-1] + pivot_x;
        pt2_y = pt1_y*scales[i-1] + pivot_y;


        curve1[i]->set_x(pt2_x);
        curve1[i]->set_y(pt2_y);

        tx = pt1_x*(scales[i-1]-1);
        ty = pt1_y*(scales[i-1]-1);

        for (unsigned int j = i+1;j<curve1.size();j++)
            {
            curve1[j]->set_x(curve1[j]->x() + tx);
            curve1[j]->set_y(curve1[j]->y() + ty);
            }
        }

    new_shape.push_back(curve1[0]);

    for (unsigned int i = 1;i<curve1.size();i++)
        {
        thet = angles[i-1];

        if (i != 0)
            {
            pivot_x = curve1[i-1]->x();
            pivot_y = curve1[i-1]->y();
            }
        else
            {
            pivot_x = 0;
            pivot_y = 0;
            thet = 0;
            }

        pt1_x = curve1[i]->x() - pivot_x;
        pt1_y = curve1[i]->y() - pivot_y;

        pt2_x = pt1_x*vcl_cos(thet) - pt1_y*vcl_sin(thet) + pivot_x;
        pt2_y = pt1_x*vcl_sin(thet) + pt1_y*vcl_cos(thet) + pivot_y;

        vsol_point_2d_sptr pt = new vsol_point_2d(pt2_x,pt2_y);
        new_shape.push_back(pt);
        curve1[i]->set_x(pt2_x);
        curve1[i]->set_y(pt2_y);

        for (unsigned int j = i+1;j < curve1.size();j++)
            {
            x_val = curve1[j]->x() - pivot_x;
            y_val = curve1[j]->y() - pivot_y;

            curve1[j]->set_x(x_val*vcl_cos(thet) - y_val*vcl_sin(thet) + pivot_x);
            curve1[j]->set_y(x_val*vcl_sin(thet) + y_val*vcl_cos(thet) + pivot_y);
            }
        }

    return new_shape;
    }

void Lie_contour_generate_geodesic_shapes::generate_values_along_geodesic(vcl_vector<double> angles,vcl_vector<double> scales,vcl_vector<double> &sample_angles,
                                                                          vcl_vector<double> &sample_scales,double t)                                      
    {
    for (unsigned int i = 0;i<angles.size();i++)
        sample_angles.push_back(t*angles[i]);
   

    for (unsigned int i = 0;i<scales.size();i++)
        sample_scales.push_back(vcl_exp(t*vcl_log(scales[i])));

    }

vcl_vector<vsol_point_2d_sptr> Lie_contour_generate_geodesic_shapes::closed_articulated_structure(vcl_vector<vsol_point_2d_sptr> final_points)
                                                                                        
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

void Lie_contour_generate_geodesic_shapes::save_shape(vcl_vector<vsol_point_2d_sptr> new_shape,vcl_string new_shapes_path,unsigned int i)
    {
    char* num = new char[4];
    num[0] = num[1] = num[2] = num[3] = '0';
    sprintf (num, "%d", i);
    vcl_string num_str = num;

    vcl_string fileName = new_shapes_path + num_str + ".txt";

  vcl_ofstream outfp(fileName.c_str());

  assert(outfp != NULL);
  outfp << "CONTOUR" << vcl_endl;
  outfp << "OPEN" << vcl_endl;
  outfp << new_shape.size() << vcl_endl;


  for(int i=0; i<new_shape.size(); i++)
  {
    outfp <<new_shape[i]->x() << " " << new_shape[i]->y() << " " << vcl_endl;
  }
  outfp.close();

    }

void Lie_contour_generate_geodesic_shapes::save_shape_as_image(vcl_vector<vsol_point_2d_sptr> new_shape,vcl_string new_shapes_path,unsigned int i)
    {
    char* num = new char[4];
    num[0] = num[1] = num[2] = num[3] = '0';
    sprintf (num, "%d", i);
    vcl_string num_str = num;
    unsigned int rows = 200,cols = 200;
    double x,y;

    vcl_string fileName = new_shapes_path + num_str + ".jpg";

    vil_image_resource_sptr res = vil_new_image_resource(rows,cols,1,VIL_PIXEL_FORMAT_BYTE);
    
    vil_image_view<unsigned char> img = *(res->get_copy_view());
    img.fill((unsigned char)0);
  
    for (unsigned int i = 0;i<new_shape.size();i++)
        {
        x = new_shape[i]->x();
        y = new_shape[i]->y();

        img((unsigned int)x,(unsigned int)y) = 255;    
        }

  res->put_view(img);
  vil_save_image_resource(res,fileName.c_str());
    }
 

/*************************************************************************
* Function Name: Lie_contour_generate_geodesic_shapes::execute
* Parameters:
* Returns: bool
* Effects:
*************************************************************************/
// curve1 in input_spatial_object_[0]
// curve2 in input_spatial_object_[1]
bool Lie_contour_generate_geodesic_shapes::execute()
    {
    clear_output();

    bpro1_filepath input_file,output_file;
    vcl_string file_path,inp1,out,new_shapes_path = "C:\\vehicle_models_3d\\curves\\99-database\\expt\\geodesic_shapes\\fish";
    int num_samples_c1,num_samples_c2;

    parameters()->get_value( "-contour_file_list" , input_file );
    parameters()->get_value("-num_samples_c1",num_samples_c1);
    parameters()->get_value("-num_samples_c2",num_samples_c2);
    parameters()->get_value( "-output_contour_file" , output_file );

    file_path = input_file.path;
    out = output_file.path;

    vcl_ifstream infp(file_path.c_str());
    vcl_ofstream outfp(out.c_str());
    vcl_vector<vcl_vector<vsol_point_2d_sptr> > curve_points;

    while (1)
        {
        vcl_string inp1;
        infp >> inp1;

        if(inp1.size() == 0)
            break;

        vcl_vector<vsol_point_2d_sptr> points1;
        loadCON(inp1, points1);
        dbsol_interp_curve_2d curve1;
        vnl_vector<double> samples1;
        dbsol_curve_algs::interpolate_eno(&curve1,points1,samples1);
       
        double s;

         vcl_vector<vsol_point_2d_sptr> curve1_samples;

        for (unsigned int i = 0;i<num_samples_c1;i++)
            {
            s = (double(i)/double(num_samples_c1))*curve1.length();

            vsol_point_2d_sptr sample = curve1.point_at(s);
            curve1_samples.push_back(sample);
            }
        curve_points.push_back(curve1_samples);

        }

double cost,min_cost = 1e100,min_cost_index,t;
vcl_vector<double>cost_vec;
   
for (unsigned int i = 0;i<curve_points.size();i++)
    {
    cost = 0;
    for (unsigned int j = 0;j<curve_points.size();j++)
        cost += compute_lie_cost(curve_points[i],curve_points[j]);
    cost_vec.push_back(cost);
    }

for (unsigned int i = 0;i<cost_vec.size();i++)
    {
    if (cost_vec[i] < min_cost)
        {
        min_cost = cost_vec[i];
        min_cost_index = i;
        }
    }

vcl_vector< vsol_spatial_object_2d_sptr > new_contour;
unsigned int count;
double weight_1,weight_2;

count = 0;
for (unsigned i = 0;i< curve_points.size()-1;i++)
    {
vcl_vector<double> angles_1,scales_1,angles_2,scales_2;

 /*angles_scales(curve_points[min_cost_index],curve_points[i],angles_1,scales_1);
 angles_scales(curve_points[min_cost_index],curve_points[i+1],angles_2,scales_2);*/

 angles_scales(curve_points[min_cost_index],curve_points[i],angles_1,scales_1);
 angles_scales(curve_points[min_cost_index],curve_points[i+1],angles_2,scales_2);

 for (weight_1 = 0;weight_1 <= 1;weight_1 += 0.1)
     {
     weight_2 = 1 - weight_1;

     vcl_vector <double> angles,scales;

 for (unsigned int j = 0;j<angles_1.size();j++)
     {
     angles.push_back(angles_1[j]*weight_1 + angles_2[j]*weight_2);
     scales.push_back(vcl_pow(scales_1[j],weight_1)*vcl_pow(scales_2[j],weight_2));

     }

 for ( t = 0;t<=1;t = t + 0.1)
     {
     count++;
     vcl_vector<double> sample_angles,sample_scales;

 generate_values_along_geodesic(angles,scales,sample_angles,sample_scales,t);
 vcl_vector<vsol_point_2d_sptr> new_shape = transform_shape(curve_points[min_cost_index],sample_angles,sample_scales);
 vcl_vector<vsol_point_2d_sptr> closed_shape = closed_articulated_structure(new_shape);

 save_shape(closed_shape,new_shapes_path,count);
 save_shape_as_image(closed_shape,new_shapes_path,count);
                                             
    vsol_polyline_2d_sptr newpolyline = new vsol_polyline_2d (closed_shape);
    new_contour.push_back(newpolyline->cast_to_spatial_object());
     }
    }
    }

    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    output_vsol->add_objects(new_contour);

    output_data_[0].push_back(output_vsol);
    return true;
    }

