/*************************************************************************
*    NAME: pradeep
*    FILE: Lie_contour_geodesic_process.cxx
*    DATE: 17 July 2007
*************************************************************************/
#include "Lie_contour_geodesic_process.h"
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
* Function Name: Lie_contour_geodesic_process::Lie_contour_geodesic_process
* Parameters: 
* Effects: 
*************************************************************************/

Lie_contour_geodesic_process::Lie_contour_geodesic_process()
    {
    if (!parameters()->add( "samples curve 1: " , "-num_samples_c1" , 100 )  ||
        !parameters()->add( "samples curve 2: " , "-num_samples_c2" , 100 )  ||
        !parameters()->add( "Contour file list <filename...>" , "-contour_file_list", bpro1_filepath("","*")) ||
        !parameters()->add("Mean contour file <filename...>" , "-mean_contour_file", bpro1_filepath("","*"))||
        !parameters()->add( "Output contour file <filename...>" , "-output_transformations_file", bpro1_filepath("","*")))
        {
        vcl_cerr << "ERROR: Adding parameters in Lie_contour_geodesic_process::Lie_contour_geodesic_process()" << vcl_endl;
        }

    }

//: Clone the process
bpro1_process*
Lie_contour_geodesic_process::clone() const
    {
    return new Lie_contour_geodesic_process(*this);
    }


double Lie_contour_geodesic_process::compute_lie_cost(vcl_vector<vsol_point_2d_sptr> curve1_samples,vcl_vector<vsol_point_2d_sptr> curve2_samples )
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
        lie_cost = lie_cost +  (scale_comp*scale_comp) + (angle_comp*angle_comp);
        }
    lie_cost = vcl_sqrt(lie_cost);

    return lie_cost;
    }

void Lie_contour_geodesic_process::angles_scales(vcl_vector<vsol_point_2d_sptr> curve1,vcl_vector<vsol_point_2d_sptr> curve2,
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
        }
    }

/*************************************************************************
* Function Name: Lie_contour_geodesic_process::execute
* Parameters:
* Returns: bool
* Effects:
*************************************************************************/
// curve1 in input_spatial_object_[0]
// curve2 in input_spatial_object_[1]
bool Lie_contour_geodesic_process::execute()
    {
    clear_output();

    bpro1_filepath input_file,output_file,mean_file;
    vcl_string file_path,inp1,out,mean_file_path;
    int num_samples_c1,num_samples_c2;

    parameters()->get_value( "-contour_file_list" , input_file );
    parameters()->get_value("-num_samples_c1",num_samples_c1);
    parameters()->get_value("-num_samples_c2",num_samples_c2);
    parameters()->get_value( "-mean_contour_file",mean_file);
    parameters()->get_value( "-output_transformations_file" , output_file );

    file_path = input_file.path;
    mean_file_path = mean_file.path;
    out = output_file.path;

    vcl_ifstream infp(file_path.c_str());
    vcl_ofstream outfp(out.c_str());

   

    // construct the first curve
    vcl_vector<vsol_point_2d_sptr> mean_samples;
    loadCON(mean_file_path, mean_samples);

    vcl_vector<vcl_vector<double> > angles_vec,scales_vec;
    vcl_vector<vsol_point_2d_sptr> curve_samples;

    while (1)
        {
        vcl_string inp2;
        infp >> inp2;

        if(inp2.size() == 0)
            break;

        curve_samples.clear();

        vcl_cout << inp1 << vcl_endl;
        vcl_cout << inp2 << vcl_endl;

        // construct the second curve
        vcl_vector<vsol_point_2d_sptr> points;
        loadCON(inp2, points);
        dbsol_interp_curve_2d curve;
        vnl_vector<double> samples;

        dbsol_curve_algs::interpolate_eno(&curve,points,samples);

        double s;

        for (unsigned int i = 0;i<num_samples_c2;i++)
            {
            s = (double(i)/double(num_samples_c2))*curve.length();

            vsol_point_2d_sptr sample = curve.point_at(s);
            curve_samples.push_back(sample);
            }

        vcl_vector<double> angles,scales;
        angles_scales(mean_samples,curve_samples,angles,scales);

        angles_vec.push_back(angles);
        scales_vec.push_back(scales);

        }

    vcl_vector<double>mean_scales,mean_angles;
    double angle,scale,sum_ang;

    
    for (unsigned int i = 0;i<angles_vec.size();i++)
        {
        for (unsigned int j = 0;j<angles_vec[i].size();j++)
            outfp << angles_vec[i][j] << vcl_endl;

        outfp << "end of shape angles: " << i << vcl_endl;
        }

    for (unsigned int i = 0;i<scales_vec.size();i++)
        {
        for (unsigned int j = 0;j<scales_vec[i].size();j++)
            outfp << scales_vec[i][j] << vcl_endl;

        outfp << "end of shape scales: " << i << vcl_endl;
        }


    vcl_vector< vsol_spatial_object_2d_sptr > contour;
    vsol_polyline_2d_sptr newpolyline = new vsol_polyline_2d (mean_samples);
    contour.push_back(newpolyline->cast_to_spatial_object());

    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    output_vsol->add_objects(contour);

    output_data_[0].push_back(output_vsol);
    return true;
    }

