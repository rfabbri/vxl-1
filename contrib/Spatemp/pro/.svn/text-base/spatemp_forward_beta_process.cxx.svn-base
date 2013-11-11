
#include "spatemp_forward_beta_process.h"
#include <bpro1/bpro1_parameters.h>
#include <brip/brip_vil_float_ops.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/sel/dbdet_curvelet_map.h>

#include <Spatemp/algo/dbdet_temporal_map.h>
#include <Spatemp/algo/dbdet_temporal_map_sptr.h>


#include <Spatemp/pro/dbdet_temporal_map_storage.h>
#include <Spatemp/pro/dbdet_temporal_map_storage_sptr.h>

//#include <dbdet/dbdet_curve_model.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
//: Constructor
spatemp_forward_beta_process::spatemp_forward_beta_process(void): bpro1_process()
    {
        if( !parameters()->add( "# of frames" , "-nframes" , 1 )||
            !parameters()->add( "Expected Motion" , "-expmotion" , (unsigned int)3))
            {
                vcl_cout<<"\n Could not read the parameters";                 
            }
    }

//: Destructor
spatemp_forward_beta_process::~spatemp_forward_beta_process()
    {

    }


//: Return the name of this process
vcl_string
spatemp_forward_beta_process::name()
    {
    return "Forward Beta Map";
    }


//: Return the number of input frame for this process
int
spatemp_forward_beta_process::input_frames()
    {
        int nframes=1;
        parameters()->get_value("-nframes",nframes);
        return nframes;

    }
void
spatemp_forward_beta_process::set_forward_input_frames()
    {
       

    }

//: Return the number of output frames for this process
int
spatemp_forward_beta_process::output_frames()
    {
    return 1;
    }


//: Provide a vector of required input types
vcl_vector< vcl_string > 
spatemp_forward_beta_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "sel" );
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > 
spatemp_forward_beta_process::get_output_type()
    {  
        vcl_vector<vcl_string > to_return;
        to_return.push_back( "temporalmap" );
        return to_return;
    }


//: Execute the process
bool
spatemp_forward_beta_process::execute()
    {
        int nframes=1;
        parameters()->get_value("-nframes",nframes);

        dbdet_temporal_map_params params;
        unsigned int tneighbor;
        unsigned int niter;
        float derr;
        unsigned model_type=0;

        parameters()->get_value("-expmotion",tneighbor);
        params.tneighbor=tneighbor;
        dbdet_temporal_map_storage_sptr tmap_storage=dbdet_temporal_map_storage_new();
        vcl_map<int,dbdet_curvelet_map*> neighbor_cmap;
        vcl_map<int,dbdet_edgemap_sptr> neighbor_emap;
        dbdet_temporal_map_sptr tmap;
        for (int i =0;i<2*nframes+1;i++){
            // get vsol from the storage class
            dbdet_sel_storage_sptr input_sel;
            input_sel.vertical_cast(input_data_[i][0]);

            if(i==nframes)
            {
                tmap=new dbdet_temporal_map(input_sel->CM(),params);
                tmap->elg_=input_sel->ELG();
            }
            else
            {
                neighbor_cmap[-i+nframes]=&input_sel->CM();
                neighbor_emap[-i+nframes]=input_sel->EM();
            }
        }
        tmap->neighbor_cmap_=neighbor_cmap;
        tmap->neighbor_emap_=neighbor_emap;

        //: compute forward flow
        tmap->compute_forward_beta();
        //: compute backward flow
        tmap->compute_backward_beta();
        //: compute beta_t
        tmap->compute_temporal_derivatives();
        //: compute beta_s
        tmap->compute_spatial_derivatives();
        //: form bundles per edge
        tmap->compute_spatial_temporal_bundles();


        
        tmap->extract_one_chains_from_the_link_graph();
        //: 
        tmap->extract_regions();

        //
        tmap->compute_distribution_per_region_new();
        tmap->compute_hough_transform();
        //tmap->compute_initial_probability();

        tmap_storage->set_temporalmap(tmap);


        output_data_[0].push_back(tmap_storage);
        return true;  
}
//: Clone the process
bpro1_process*
spatemp_forward_beta_process::clone() const
{
    return new spatemp_forward_beta_process(*this);
}

bool
spatemp_forward_beta_process::finish()
{
    return true;
}


