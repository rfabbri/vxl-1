// This is brcv/shp/dbskfg/pro/dbskfg_form_containment_graph_process.cxx

//:
// \file

#include <con_graph/pro/dbskfg_form_containment_graph_process.h>
#include <con_graph/dbskfg_containment_graph_sptr.h>
#include <con_graph/dbskfg_containment_graph.h>

#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>

#include <dbskfg/algo/dbskfg_transform_manager.h>

//: Constructor
dbskfg_form_containment_graph_process::dbskfg_form_containment_graph_process()
{

    if ( !parameters()->add( "window size" , "-window_size" , (double) 20.0 )
         ||
         !parameters()->add( "xsamp" , "-x_sample" , (double) 4.0 )
         ||
         !parameters()->add( "ysamp" , "-y_smaple" , (double) 4.0 )
         ||
         !parameters()->add( "One bbox (size of contours)  " , 
                             "-all", bool(false)))
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_form_containment_graph_process::~dbskfg_form_containment_graph_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_form_containment_graph_process::clone() const
{
    return new dbskfg_form_containment_graph_process(*this);
}

vcl_string
dbskfg_form_containment_graph_process::name()
{
    return "Compute Containment Graph";
}

vcl_vector< vcl_string >
dbskfg_form_containment_graph_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    to_return.push_back( "image" );
    return to_return;
}

vcl_vector< vcl_string >
dbskfg_form_containment_graph_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    return to_return;
}

int dbskfg_form_containment_graph_process::input_frames()
{
    return 1;
}

int dbskfg_form_containment_graph_process::output_frames()
{
    return 1;
}

bool dbskfg_form_containment_graph_process::execute()
{

    bool status = true;
    
    // // 1) get input storage class
    // vidpro1_vsol2D_storage_sptr input_vsol;
    // input_vsol.vertical_cast(input_data_[0][0]);

    // //1) get input storage classes
    // vidpro1_image_storage_sptr frame_image;
    // frame_image.vertical_cast(input_data_[0][1]);

   
    // bool use_all_contours(false);
    // double window_size(20.0);
    // double xsample(4.0);
    // double ysample(4.0);
    // parameters()->get_value( "-window_size" , window_size );
    // parameters()->get_value( "-x_sample" , xsample );
    // parameters()->get_value( "-y_sample" , ysample );
    // parameters()->get_value( "-all", use_all_contours);

    // vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = 
    //     input_vsol->all_data();

    // if ( use_all_contours )
    // {

    //     dbskfg_containment_graph_sptr cgraph =
    //         new dbskfg_containment_graph(input_vsol,frame_image);
    //     cgraph->compute_root_node();
        

    //     dbskfg_transform_manager::Instance().destroy_singleton();

    //     return status;
    // }

    // // Create initial box 
    // vgl_box_2d<double> window(0,window_size,0,window_size);

    // // Keep centroids
    // vcl_vector<vgl_point_2d<double> > centroids;

    // vil_image_resource_sptr image = frame_image->get_image();

    // // Loop through and create a box
    // for ( unsigned int i=xsample; i < image->ni() ; i+=xsample )
    // {

    //     for ( unsigned int j=ysample; j < image->nj() ; j+=ysample )
    //     {
    //         centroids.push_back(vgl_point_2d<double>(i,j));
    //     }
    // }
   
    // for (unsigned int k=0; k < centroids.size() ; k++)
    // {
        
    //     vidpro1_vsol2D_storage_sptr box_contours = new vidpro1_vsol2D_storage();
    //     vgl_box_2d<double> box = window;
    //     box.set_centroid(centroids[k]);

    //     for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
    //     {
    //         if( vsol_list[b]->cast_to_curve()->cast_to_line() )
    //         {
    //             vsol_line_2d* line = vsol_list[b]->cast_to_curve()
    //                 ->cast_to_line();
    //             if ( box.contains(line->p0()->x(),line->p0()->y()) ||
    //                  box.contains(line->p1()->x(),line->p1()->y()))
    //             {
    //                 box_contours->add_object(line);
    //             }
    //         }
    //         else if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
    //         {
    //             vsol_polyline_2d* polyline = 
    //                 vsol_list[b]->cast_to_curve()->cast_to_polyline();

    //             for ( unsigned int p=0; p < polyline->size() ; ++p)
    //             {
    //                 if ( box.contains(polyline->vertex(p)->x(),
    //                                   polyline->vertex(p)->y()))
    //                 {
    //                     box_contours->add_object(polyline);
    //                     break;
    //                 }

    //             }
          
    //         }

    //     }

    //     if ( box_contours->all_data().size() >= 2 )
    //     {
    //         // Create a box and 
    //         // Create root node
    //         dbskfg_containment_graph_sptr cgraph =
    //             new dbskfg_containment_graph(box_contours,frame_image);
    //         cgraph->compute_root_node();
    //     }

    //     dbskfg_transform_manager::Instance().destroy_singleton();

    // }

    return status;
}

bool dbskfg_form_containment_graph_process::finish()
{
    return true;
}


