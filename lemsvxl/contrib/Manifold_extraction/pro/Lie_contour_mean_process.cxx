/*************************************************************************
*    NAME: pradeep
*    FILE: Lie_contour_mean_process.cxx
*    DATE: 4 July 2007
*************************************************************************/
#include "Lie_contour_mean_process.h"
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

/*************************************************************************
* Function Name: Lie_contour_mean_process::Lie_contour_mean_process
* Parameters: 
* Effects: 
*************************************************************************/

Lie_contour_mean_process::Lie_contour_mean_process()
    {
    if (!parameters()->add( "samples curve 1: " , "-num_samples_c1" , 100 )  ||
        !parameters()->add( "samples curve 2: " , "-num_samples_c2" , 100 )  ||
        !parameters()->add( "Contour file list <filename...>" , "-contour_file_list", bpro1_filepath("","*")) ||
        !parameters()->add( "Output contour file <filename...>" , "-output_contour_file", bpro1_filepath("","*")))
        {
        vcl_cerr << "ERROR: Adding parameters in Lie_contour_mean_process::Lie_contour_mean_process()" << vcl_endl;
        }

    }

//: Clone the process
bpro1_process*
Lie_contour_mean_process::clone() const
    {
    return new Lie_contour_mean_process(*this);
    }



double Lie_contour_mean_process::compute_lie_cost(vcl_vector<vsol_point_2d_sptr> curve1_samples,vcl_vector<vsol_point_2d_sptr> curve2_samples )
    {
    double lie_cost = 0,length_1,length_2,angle_1,angle_2,scale_comp,angle_comp;

    for (unsigned int i = 0;i<curve1_samples.size()-1;i++)
        {
        length_1 = curve1_samples[i]->distance(curve1_samples[i+1]);
        length_2 = curve2_samples[i]->distance(curve2_samples[i+1]);

        angle_1 = vcl_atan2(curve1_samples[i+1]->y()-curve1_samples[i]->y(),curve1_samples[i+1]->x()-curve1_samples[i]->x());
        angle_2 = vcl_atan2(curve2_samples[i+1]->y()-curve2_samples[i]->y(),curve2_samples[i+1]->x()-curve2_samples[i]->x());

        scale_comp = vcl_log(length_2/length_1);
        angle_comp = angle_2 - angle_1;
        lie_cost = lie_cost + (scale_comp*scale_comp) + (angle_comp*angle_comp);
        }
    lie_cost = vcl_sqrt(lie_cost);

    return lie_cost;
    }

void Lie_contour_mean_process::angles_scales(vcl_vector<vsol_point_2d_sptr> curve1,vcl_vector<vsol_point_2d_sptr> curve2,
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



        scales.push_back(vcl_log(length_2/length_1));
        angles.push_back( angle_2 - angle_1 );
        vcl_cout << "angles: " << angle_2 - angle_1 << vcl_endl;
        }
    }

/*************************************************************************
* Function Name: Lie_contour_mean_process::execute
* Parameters:
* Returns: bool
* Effects:
*************************************************************************/
// curve1 in input_spatial_object_[0]
// curve2 in input_spatial_object_[1]
bool Lie_contour_mean_process::execute()
    {
    clear_output();

    bpro1_filepath input_file,output_file;
    vcl_string file_path,inp1,out;
    int num_samples_c1,num_samples_c2;

    parameters()->get_value( "-contour_file_list" , input_file );
    parameters()->get_value("-num_samples_c1",num_samples_c1);
    parameters()->get_value("-num_samples_c2",num_samples_c2);
    parameters()->get_value( "-output_contour_file" , output_file );

    file_path = input_file.path;
    out = output_file.path;

    vcl_ifstream infp(file_path.c_str());
    vcl_ofstream outfp(out.c_str());

    infp >> inp1;

    // construct the first curve
    vcl_vector<vsol_point_2d_sptr> points1,mean_points;
    loadCON(inp1, points1);
    dbsol_interp_curve_2d curve1;
    vnl_vector<double> samples1;

    vcl_vector<vcl_vector<double> > angles_vec,scales_vec;
    vcl_vector<vsol_point_2d_sptr> curve1_samples,curve2_samples;

    while (1)
        {
        vcl_string inp2;
        infp >> inp2;

        if(inp2.size() == 0)
            break;

        curve1_samples.clear();
        curve2_samples.clear();

        vcl_cout << inp1 << vcl_endl;
        vcl_cout << inp2 << vcl_endl;

        // construct the second curve
        vcl_vector<vsol_point_2d_sptr> points2;
        loadCON(inp2, points2);
        dbsol_interp_curve_2d curve2;
        vnl_vector<double> samples2;


        dbsol_curve_algs::interpolate_eno(&curve1,points1,samples1);
        dbsol_curve_algs::interpolate_eno(&curve2,points2,samples2);

        double s;

        outfp << "curve 1: " << vcl_endl;

        for (unsigned int i = 0;i<num_samples_c1;i++)
            {
            s = (double(i)/double(num_samples_c1))*curve1.length();

            vsol_point_2d_sptr sample = curve1.point_at(s);
            outfp << sample->x() << "  " << sample->y() << vcl_endl;
            curve1_samples.push_back(sample);
            }

        outfp << "curve 2: " << vcl_endl;

        for (unsigned int i = 0;i<num_samples_c2;i++)
            {
            s = (double(i)/double(num_samples_c2))*curve2.length();

            vsol_point_2d_sptr sample = curve2.point_at(s);
            outfp << sample->x() << "  " << sample->y() << vcl_endl;
            curve2_samples.push_back(sample);
            }

        vcl_vector<double> angles,scales;
        angles_scales(curve1_samples,curve2_samples,angles,scales);
        //  angles_scales(points1,points2,angles,scales);

        angles_vec.push_back(angles);
        scales_vec.push_back(scales);

        }

    /* curve1_samples.clear();

    for (unsigned int i = 0;i<points1.size();i++)
    {
    curve1_samples.push_back(points1[i]);
    }*/

    vcl_vector<double>mean_scales,mean_angles;
    double angle,scale,sum_ang;

  /*  for (unsigned int j = 0;j<angles_vec.size();j++)
        {
        vcl_vector <double> angles_struct = angles_vec[j];

        for (unsigned int i = 0;i < angles_struct.size();i++)
            {
            sum_ang = 0;

            for (unsigned int k = 0;k<i;k++)
                sum_ang += angles_struct[k];

            angles_struct[i] = angles_struct[i] - sum_ang;
            }
        angles_vec[j] = angles_struct;
        }*/

    for (unsigned int j = 0;j<angles_vec[0].size();j++)
        {
        angle = 0;
        for (unsigned int i = 0;i<angles_vec.size();i++)
            angle = angle + angles_vec[i][j];

        angle = angle/angles_vec.size();
        mean_angles.push_back(angle);
        }

    for (unsigned int j = 0;j<scales_vec[0].size();j++)
        {
        scale = 0;
        for (unsigned int i = 0;i<scales_vec.size();i++)
            scale = scale + scales_vec[i][j];

        scale = scale/scales_vec.size();
        scale = vcl_exp(scale);
        mean_scales.push_back(scale);
        }

    outfp << "mean scale values: " << vcl_endl;

    for (unsigned int i = 0;i<mean_scales.size();i++)
        {
        outfp << /*" scale: " << scales_vec[0][i] << "mean scale: " <<*/ mean_scales[i] << vcl_endl;
        }

    outfp << "mean angle values: " << vcl_endl;

    for (unsigned int i = 0;i<mean_angles.size();i++)
        {
        outfp << /*" angle: " << angles_vec[0][i] << "mean angle: " <<*/ mean_angles[i] << vcl_endl;
        }

    for (unsigned int i = 0;i < mean_angles.size();i++)
        {
        sum_ang = 0;

        for (unsigned int k = 0;k<i;k++)
            sum_ang += mean_angles[k];

        mean_angles[i] = mean_angles[i] - sum_ang;

        vcl_cout << "mean angles: " << mean_angles[i] << vcl_endl;
        }

    double pivot_x,pivot_y, pt1_x, pt1_y, pt2_x, pt2_y,thet,tx,ty,x_val,y_val;

    for (unsigned int i = 1;i<curve1_samples.size();i++)
        {
        if (i != 0)
            {
            pivot_x = curve1_samples[i-1]->x();
            pivot_y = curve1_samples[i-1]->y();
            }

        pt1_x = curve1_samples[i]->x() - pivot_x;
        pt1_y = curve1_samples[i]->y() - pivot_y;
        pt2_x = pt1_x*mean_scales[i-1] + pivot_x;
        pt2_y = pt1_y*mean_scales[i-1] + pivot_y;


        curve1_samples[i]->set_x(pt2_x);
        curve1_samples[i]->set_y(pt2_y);

        tx = pt1_x*(mean_scales[i-1]-1);
        ty = pt1_y*(mean_scales[i-1]-1);

        for (unsigned int j = i+1;j<curve1_samples.size();j++)
            {
            curve1_samples[j]->set_x(curve1_samples[j]->x() + tx);
            curve1_samples[j]->set_y(curve1_samples[j]->y() + ty);
            }
        }

    mean_points.push_back(curve1_samples[0]);

    for (unsigned int i = 1;i<curve1_samples.size();i++)
        {
        thet = mean_angles[i-1];

        if (i != 0)
            {
            pivot_x = curve1_samples[i-1]->x();
            pivot_y = curve1_samples[i-1]->y();
            }
        else
            {
            pivot_x = 0;
            pivot_y = 0;
            thet = 0;
            }

        pt1_x = curve1_samples[i]->x() - pivot_x;
        pt1_y = curve1_samples[i]->y() - pivot_y;

        pt2_x = pt1_x*vcl_cos(thet) - pt1_y*vcl_sin(thet) + pivot_x;
        pt2_y = pt1_x*vcl_sin(thet) + pt1_y*vcl_cos(thet) + pivot_y;

        vsol_point_2d_sptr pt = new vsol_point_2d(pt2_x,pt2_y);
        mean_points.push_back(pt);
        curve1_samples[i]->set_x(pt2_x);
        curve1_samples[i]->set_y(pt2_y);

        for (unsigned int j = i+1;j < curve1_samples.size();j++)
            {
            x_val = curve1_samples[j]->x() - pivot_x;
            y_val = curve1_samples[j]->y() - pivot_y;

            curve1_samples[j]->set_x(x_val*vcl_cos(thet) - y_val*vcl_sin(thet) + pivot_x);
            curve1_samples[j]->set_y(x_val*vcl_sin(thet) + y_val*vcl_cos(thet) + pivot_y);
            }
        }

    outfp << "mean samples: " << vcl_endl;

    for (unsigned int i = 0;i<mean_points.size();i++)
        {
        outfp << mean_points[i]->x() << " " << mean_points[i]->y() << vcl_endl;
        }

    vcl_vector<vsol_point_2d_sptr> closed_form = closed_articulated_structure(mean_points);

    vcl_vector< vsol_spatial_object_2d_sptr > mean_contour;
    //vsol_polyline_2d_sptr newpolyline = new vsol_polyline_2d (mean_points);
    vsol_polyline_2d_sptr newpolyline = new vsol_polyline_2d (closed_form);
    mean_contour.push_back(newpolyline->cast_to_spatial_object());

    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    output_vsol->add_objects(mean_contour);

    output_data_[0].push_back(output_vsol);
    return true;
    }

