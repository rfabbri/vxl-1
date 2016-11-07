/*************************************************************************
*    NAME: pradeep
*    FILE: Lie_spoke_classify_shapes.cxx
*    DATE: 06 August 2007
*************************************************************************/
#include "Lie_spoke_classify_shapes.h"
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
* Function Name: Lie_spoke_classify_shapes::Lie_spoke_classify_shapes
* Parameters: 
* Effects: 
*************************************************************************/

Lie_spoke_classify_shapes::Lie_spoke_classify_shapes()
    {

    if (!parameters()->add( "Class 1 " , "-class_1", bpro1_filepath("","*")) ||
        !parameters()->add( "Class 2 " , "-class_2", bpro1_filepath("","*")) ||
        !parameters()->add( "Class 1 mean file <filename...>" , "-class_1_mean_file", bpro1_filepath("","*")) ||
        !parameters()->add( "Class 2 mean file <filename...>" , "-class_2_mean_file", bpro1_filepath("","*")) ||
        !parameters()->add( "new class file <filename...>" , "-new_class_file", bpro1_filepath("","*")) ||
        !parameters()->add( "Class 1 geodesic file <filename...>" , "-class_1_geodesic_file", bpro1_filepath("","*")) ||
        !parameters()->add( "Class 2 geodesic file <filename...>" , "-class_2_geodesic_file", bpro1_filepath("","*")) ||
        !parameters()->add( "new shapes cost file <filename...>" , "-costs_new_shapes_file", bpro1_filepath("","*")) ||
        !parameters()->add( "samples curve 1: " , "-num_samples_c1" , 100 )  ||
        !parameters()->add( "samples curve 2: " , "-num_samples_c2" , 100 ) ||
        !parameters()->add( "Use eno interpolation","-use-eno-interpolation",true))

        {
        vcl_cerr << "ERROR: Adding parameters in Lie_spoke_classify_shapes::Lie_spoke_classify_shapes()" << vcl_endl;
        }

    }

//: Clone the process
bpro1_process*
Lie_spoke_classify_shapes::clone() const
    {
    return new Lie_spoke_classify_shapes(*this);
    }

/*************************************************************************
* Function Name: Lie_spoke_classify_shapes::execute
* Parameters:
* Returns: bool
* Effects:
*************************************************************************/
// curve1 in input_spatial_object_[0]
// curve2 in input_spatial_object_[1]
bool Lie_spoke_classify_shapes::execute()
    {
    clear_output();

    bool use_eno;
    bpro1_filepath mean_file_1,mean_file_2,new_file,geodesic_file_1,geodesic_file_2,costs_new_file,class_1,class_2;
    vcl_string mean_path_1,mean_path_2,new_class_path,geodesic_path_1,geodesic_path_2,costs_new_shapes,class_1_name,class_2_name;

    int num_samples_c1,num_samples_c2;

    parameters()->get_value( "-class_1" , class_1 );
    parameters()->get_value( "-class_2" , class_2 );
    parameters()->get_value( "-class_1_mean_file" , mean_file_1 );
    parameters()->get_value( "-class_2_mean_file" , mean_file_2 );
    parameters()->get_value( "-new_class_file" , new_file );
    parameters()->get_value( "-class_1_geodesic_file" , geodesic_file_1 );
    parameters()->get_value( "-class_2_geodesic_file" , geodesic_file_2 );
    parameters()->get_value( "-costs_new_shapes_file" , costs_new_file);
    parameters()->get_value("-num_samples_c1",num_samples_c1);
    parameters()->get_value("-num_samples_c2",num_samples_c2);
    parameters()->get_value( "-use-eno-interpolation",use_eno);

    class_1_name = class_1.path;
    class_2_name = class_2.path;
    mean_path_1 = mean_file_1.path;
    mean_path_2 = mean_file_2.path;
    new_class_path = new_file.path;
    geodesic_path_1 = geodesic_file_1.path;
    geodesic_path_2 = geodesic_file_2.path;
    costs_new_shapes = costs_new_file.path;


    // construct the first curve
    vcl_vector<vsol_point_2d_sptr> mean_points_1,mean_points_2;

    loadCON(mean_path_1,mean_points_1);
    loadCON(mean_path_2,mean_points_2);

    vcl_ifstream infp(new_class_path.c_str());
    vcl_ofstream ofst(costs_new_shapes.c_str());
    vcl_vector<vcl_vector<vsol_point_2d_sptr> > curve_points;

     //get all the contour files residing in the input directory
    vcl_vector<vcl_string> file_names = get_all_files(new_class_path);

      for (unsigned int file_num = 0;file_num <file_names.size();file_num++)
        {
        vcl_string inp1 = file_names[file_num];

       /* vcl_string inp1;
        infp >>inp1;*/

        if(inp1.size() == 0)
            break;

        vcl_vector<vsol_point_2d_sptr> points1;
        loadCON(inp1, points1);
        dbsol_interp_curve_2d curve1;
        vnl_vector<double> samples1;
        dbsol_curve_algs::interpolate_eno(&curve1,points1,samples1);

        double s;

        vcl_vector<vsol_point_2d_sptr> curve1_samples;

          if (use_eno)
             {
        for (unsigned int i = 0;i<num_samples_c1;i++)
            {
            s = (double(i)*curve1.length()/double(num_samples_c1));

            vsol_point_2d_sptr sample = curve1.point_at(s);
            curve1_samples.push_back(sample);
            }
        //to close the contour since interpolation only gives the points in between
        curve1_samples.push_back(curve1_samples[0]);
             }
         else
             {
             for (unsigned int i = 0;i<points1.size();i++)
                 curve1_samples.push_back(points1[i]);

             curve1_samples.push_back(points1[0]);
             }

     /*   for (unsigned int i = 0;i<num_samples_c1;i++)
            {
            s = (double(i)/double(num_samples_c1))*curve1.length();

            vsol_point_2d_sptr sample = curve1.point_at(s);
            curve1_samples.push_back(sample);
            }
        curve1_samples.push_back(curve1_samples[0]);*/

        curve_points.push_back(curve1_samples);

        }
    double cost_1,cost_2;

    ofst << class_1_name << " mean costs: " << class_2_name << " mean costs: " << vcl_endl;

    for (unsigned int i = 0;i<curve_points.size();i++)
        {
        cost_1 = compute_lie_spoke_cost(curve_points[i],mean_points_1);
        cost_2 = compute_lie_spoke_cost(curve_points[i],mean_points_2);
        ofst << cost_1 << " " << cost_2 << vcl_endl;
        }


    char* num = new char[1];
    num[0] = '0';

    vcl_vector<vcl_vector<double> > instance_scales_1_vec,instance_angles_1_vec,instance_scales_2_vec,instance_angles_2_vec;

    for (unsigned int i=0;i<curve_points.size();i++)
        {
        vcl_vector<double> instance_angles_1,instance_scales_1,instance_angles_2,instance_scales_2;

        compute_spoke_scales_angles(mean_points_1,curve_points[i],instance_scales_1,instance_angles_1);
        compute_spoke_scales_angles(mean_points_2,curve_points[i],instance_scales_2,instance_angles_2);

        instance_scales_1_vec.push_back(instance_scales_1);
        instance_angles_1_vec.push_back(instance_angles_1);
        instance_scales_2_vec.push_back(instance_scales_2);
        instance_angles_2_vec.push_back(instance_angles_2);
        }

    for (unsigned int iter = 1;iter<=3;iter++)
        {
        vcl_vector<double> geodesic_angles_1,geodesic_scales_1,geodesic_angles_2,geodesic_scales_2;
        double proj_cost_1,proj_cost_2;
        int proj_coeff_1,proj_coeff_2;
        sprintf (num, "%d", iter);

        // geodesic_path_1.replace(imgfile.size()-num_str.size(), num_str.size(), num);
        geodesic_path_1.replace(geodesic_path_1.size() - 5,1,num);
        geodesic_path_2.replace(geodesic_path_2.size() - 5,1,num);

        read_spoke_geodesic_info(geodesic_path_1,geodesic_angles_1,geodesic_scales_1);
        read_spoke_geodesic_info(geodesic_path_2,geodesic_angles_2,geodesic_scales_2);

        ofst << class_1_name << " geodesic costs: " << class_2_name << " geodesic costs: " << vcl_endl;
        ofst << "geodesic angles " << " geodesic scales " << vcl_endl;

        //for (unsigned int i=0;i<geodesic_angles_1.size();i++)
        //    ofst << geodesic_angles_1[i] << " " << geodesic_scales_1[i] << vcl_endl;

        for (unsigned int i = 0;i<curve_points.size();i++)
            {
            vcl_vector<double> proj_angles_1,proj_scales_1,proj_angles_2,proj_scales_2;

            find_spoke_projection_on_geodesic(geodesic_angles_1,geodesic_scales_1,instance_angles_1_vec[i],instance_scales_1_vec[i],
                proj_angles_1,proj_scales_1,proj_cost_1,proj_coeff_1);

            find_spoke_projection_on_geodesic(geodesic_angles_2,geodesic_scales_2, instance_angles_2_vec[i],instance_scales_2_vec[i],
                proj_angles_2,proj_scales_2,proj_cost_2,proj_coeff_2);


            ofst << proj_cost_1 << " " << proj_cost_2 << " " << proj_coeff_1 << " " << proj_coeff_2 << vcl_endl;

            for (unsigned int j=0;j<proj_angles_1.size();j++)
                {
                instance_scales_1_vec[i][j] = instance_scales_1_vec[i][j]- proj_scales_1[j];
                instance_angles_1_vec[i][j] = instance_angles_1_vec[i][j]- proj_angles_1[j];
                instance_scales_2_vec[i][j] = instance_scales_2_vec[i][j]- proj_scales_2[j];
                instance_angles_2_vec[i][j] = instance_angles_2_vec[i][j]- proj_angles_2[j];
                }
            }
        }

    ofst.close();

    return true;
    }

