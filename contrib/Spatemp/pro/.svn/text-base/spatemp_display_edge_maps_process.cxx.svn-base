
#include "spatemp_display_edge_maps_process.h"
#include <bpro1/bpro1_parameters.h>
#include <brip/brip_vil_float_ops.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vnl/vnl_math.h>
#include <vul/vul_sprintf.h>
#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/algo/dbdet_sel.h>
//#include <dbdet/dbdet_curve_model.h>
#include <dbsol/dbsol_circ_arc_2d_sptr.h>
#include <dbsol/dbsol_circ_arc_2d.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
//: Constructor
spatemp_display_edge_maps_process::spatemp_display_edge_maps_process(void): bpro1_process()
    {
        if( !parameters()->add( "# of frames" , "-nframes" , 2 ) )
            {
                
            }
    }

//: Destructor
spatemp_display_edge_maps_process::~spatemp_display_edge_maps_process()
    {

    }


//: Return the name of this process
vcl_string
spatemp_display_edge_maps_process::name()
    {
    return "Display Edge Maps";
    }


//: Return the number of input frame for this process
int
spatemp_display_edge_maps_process::input_frames()
    {
        int nframes=1;
        parameters()->get_value("-nframes",nframes);
        return nframes;

    }
void
spatemp_display_edge_maps_process::set_forward_input_frames()
    {
       

    }

//: Return the number of output frames for this process
int
spatemp_display_edge_maps_process::output_frames()
    {
    return 1;

    }


//: Provide a vector of required input types
vcl_vector< vcl_string > spatemp_display_edge_maps_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "edge_map" );
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > spatemp_display_edge_maps_process::get_output_type()
    {  
        vcl_vector<vcl_string > to_return;
        //to_return.push_back( "vsol2D" );
        return to_return;
    }


//: Execute the process
bool
spatemp_display_edge_maps_process::execute()
    {
        int nframes=1;
        parameters()->get_value("-nframes",nframes);


        for (int i =0;i<2*nframes+1;i++){
            // get vsol from the storage class
            dbdet_edgemap_storage_sptr input_edgemap;
            input_edgemap.vertical_cast(input_data_[i][0]);
            //vcl_cout<<"\n #of Edges in prev frame is "<<input_edgemap->get_edgemap()->num_edgels;
        }
        return true;  
}
//: Clone the process
bpro1_process*
spatemp_display_edge_maps_process::clone() const
    {
    return new spatemp_display_edge_maps_process(*this);
    }

bool
spatemp_display_edge_maps_process::finish()
    {
    return true;
    }


