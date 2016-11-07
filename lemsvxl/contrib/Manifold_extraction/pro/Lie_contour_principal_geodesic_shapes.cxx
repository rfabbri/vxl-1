/*************************************************************************
*    NAME: pradeep
*    FILE: Lie_contour_principal_geodesic_shapes.cxx
*    DATE: 17 July 2007
*************************************************************************/
#include "Lie_contour_principal_geodesic_shapes.h"
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
* Function Name: Lie_contour_principal_geodesic_shapes::Lie_contour_principal_geodesic_shapes
* Parameters: 
* Effects: 
*************************************************************************/

Lie_contour_principal_geodesic_shapes::Lie_contour_principal_geodesic_shapes()
    {
    if (!parameters()->add( "Input mean file <filename...>" , "-mean_file", bpro1_filepath("","*")) ||
        !parameters()->add( "Geodesics file <filename...>" , "-geodesics_file", bpro1_filepath("","*"))||
        !parameters()->add( "Geodesics storage <filename...>" , "-storage_geodesic_shapes", bpro1_filepath("","*")))
        {
        vcl_cerr << "ERROR: Adding parameters in Lie_contour_principal_geodesic_shapes::Lie_contour_principal_geodesic_shapes()" << vcl_endl;
        }

    }

//: Clone the process
bpro1_process*
Lie_contour_principal_geodesic_shapes::clone() const
    {
    return new Lie_contour_principal_geodesic_shapes(*this);
    }

/*************************************************************************
* Function Name: Lie_contour_principal_geodesic_shapes::execute
* Parameters:
* Returns: bool
* Effects:
*************************************************************************/
// curve1 in input_spatial_object_[0]
// curve2 in input_spatial_object_[1]
bool Lie_contour_principal_geodesic_shapes::execute()
    {
    clear_output();

    bpro1_filepath input_mean_file,geodesics_file,bpro1_storage_geodesics;
    vcl_string mean_path,geodesics_info,storage_path;
    //vcl_string storage_path = "C:\\vehicle_models_3d\\curves\\99-database\\expt\\geodesic_shapes\\fish_princp_geodesics";

    int num_samples_c1,num_samples_c2;

    parameters()->get_value( "-mean_file" , input_mean_file );
    parameters()->get_value( "-geodesics_file" , geodesics_file );
    parameters()->get_value( "-storage_geodesic_shapes" ,bpro1_storage_geodesics);

    mean_path = input_mean_file.path;
    geodesics_info = geodesics_file.path;
    storage_path = bpro1_storage_geodesics.path;

    // construct the first curve
    vcl_vector<vsol_point_2d_sptr> mean_points;
    loadCON(mean_path,mean_points);
    double t;
    unsigned int count = 0;

     vcl_vector<double> angles,scales;
    read_geodesic_info(geodesics_info,angles,scales);
   
    for (t = -100;t<=100;t += 1)
        {
        vcl_vector<double> sample_angles,sample_scales;

   generate_values_along_geodesic(angles,scales,sample_angles,sample_scales,t);

   vcl_vector<vsol_point_2d_sptr> new_shape = transform_shape(mean_points,sample_angles,sample_scales);
   vcl_vector<vsol_point_2d_sptr> closed_shape = closed_articulated_structure(new_shape);
   save_shape_as_image(closed_shape,storage_path,count);
   count++;
        }
     return true;
    }

