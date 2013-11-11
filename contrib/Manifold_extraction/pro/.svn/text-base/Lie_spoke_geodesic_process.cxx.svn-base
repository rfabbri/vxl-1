/*************************************************************************
*    NAME: pradeep
*    FILE: Lie_spoke_geodesic_process.cxx
*    DATE: 06 August 2007
*************************************************************************/
#include "Lie_spoke_geodesic_process.h"
#include <manifold_extraction/Lie_spoke_utilities.h>
#include <manifold_extraction/Lie_contour_utilities.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>

/*************************************************************************
* Function Name: Lie_spoke_geodesic_process::Lie_spoke_geodesic_process
* Parameters: 
* Effects: 
*************************************************************************/

Lie_spoke_geodesic_process::Lie_spoke_geodesic_process()
    {
    if (!parameters()->add( "samples curve 1: " , "-num_samples_c1" , 100 )  ||
        !parameters()->add( "samples curve 2: " , "-num_samples_c2" , 100 )  ||
        !parameters()->add( "Contour file list <filename...>" , "-contour_file_list", bpro1_filepath("","*")) ||
        !parameters()->add("Mean contour file <filename...>" , "-mean_contour_file", bpro1_filepath("","*"))||
        !parameters()->add( "Output contour file <filename...>" , "-output_transformations_file", bpro1_filepath("","*"))||
        !parameters()->add( "Use eno interpolation","-use-eno-interpolation",true))
        {
        vcl_cerr << "ERROR: Adding parameters in Lie_spoke_geodesic_process::Lie_spoke_geodesic_process()" << vcl_endl;
        }

    }


//: Clone the process
bpro1_process*
Lie_spoke_geodesic_process::clone() const
    {
    return new Lie_spoke_geodesic_process(*this);
    }

/*************************************************************************
* Function Name: Lie_spoke_geodesic_process::execute
* Parameters:
* Returns: bool
* Effects:
*************************************************************************/
// curve1 in input_spatial_object_[0]
// curve2 in input_spatial_object_[1]
bool Lie_spoke_geodesic_process::execute()
    {
    clear_output();

    bpro1_filepath input_file,output_file,mean_file;
    vcl_string file_path,inp1,out,mean_file_path;
    bool use_eno;
    int num_samples_c1,num_samples_c2;

    parameters()->get_value( "-contour_file_list" , input_file );
    parameters()->get_value("-num_samples_c1",num_samples_c1);
    parameters()->get_value("-num_samples_c2",num_samples_c2);
    parameters()->get_value( "-mean_contour_file",mean_file);
    parameters()->get_value( "-output_transformations_file" , output_file );
    parameters()->get_value( "-use-eno-interpolation",use_eno);

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

     //get all the contour files residing in the input directory
    vcl_vector<vcl_string> file_names = get_all_files(file_path);

      for (unsigned int file_num = 0;file_num <file_names.size();file_num++)
        {
        vcl_string inp2 = file_names[file_num];
        // infp >> inp2;

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

        
         if (use_eno)
             {
        for (unsigned int i = 0;i<num_samples_c1;i++)
            {
            s = (double(i)*curve.length()/double(num_samples_c1));

            vsol_point_2d_sptr sample = curve.point_at(s);
            curve_samples.push_back(sample);
            }
        //to close the contour since interpolation only gives the points in between
        curve_samples.push_back(curve_samples[0]);
             }
         else
             {
             for (unsigned int i = 0;i<points.size();i++)
                 curve_samples.push_back(points[i]);

             curve_samples.push_back(points[0]);
             }
        

     /*   for (unsigned int i = 0;i<num_samples_c2;i++)
            {
            s = (double(i)/double(num_samples_c2))*curve.length();

            vsol_point_2d_sptr sample = curve.point_at(s);
            curve_samples.push_back(sample);
            }

        curve_samples.push_back(curve_samples[0]);
        */

        vcl_vector<double> angles,scales;
        compute_spoke_scales_angles(mean_samples,curve_samples,scales,angles);

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

