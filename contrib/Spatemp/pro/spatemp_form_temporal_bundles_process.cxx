
#include "spatemp_form_temporal_bundles_process.h"
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
//#include <dbdet/sel/dbdet_curvelet_map_sptr.h>

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
spatemp_form_temporal_bundles_process::spatemp_form_temporal_bundles_process(void): bpro1_process()
    {
        vcl_vector<vcl_string> model_choices;
        model_choices.push_back("constant_velocity");                   //0
        model_choices.push_back("normal_velocity");                     //1

        if( !parameters()->add( "# of frames" , "-nframes" , 2 )||
            !parameters()->add( "Expected Motion" , "-expmotion" , (unsigned int)5 )||
            !parameters()->add( "# of relaxation Iterations" , "-niter" , (unsigned int)5 )||
            !parameters()->add( "Distance " , "-derr" , (float)0.5 )||
            !parameters()->add( "Curve Model"   , "-temporal_model" , model_choices, 1) )
            {
                
            }
    }

//: Destructor
spatemp_form_temporal_bundles_process::~spatemp_form_temporal_bundles_process()
    {

    }


//: Return the name of this process
vcl_string
spatemp_form_temporal_bundles_process::name()
    {
    return "Form Temporal Bundles";
    }


//: Return the number of input frame for this process
int
spatemp_form_temporal_bundles_process::input_frames()
    {
        int nframes=1;
        parameters()->get_value("-nframes",nframes);
        return nframes;

    }
void
spatemp_form_temporal_bundles_process::set_forward_input_frames()
    {
       

    }

//: Return the number of output frames for this process
int
spatemp_form_temporal_bundles_process::output_frames()
    {
    return 1;

    }


//: Provide a vector of required input types
vcl_vector< vcl_string > spatemp_form_temporal_bundles_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "sel" );
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > spatemp_form_temporal_bundles_process::get_output_type()
    {  
        vcl_vector<vcl_string > to_return;
        to_return.push_back( "temporalmap" );
        return to_return;
    }


//: Execute the process
bool
spatemp_form_temporal_bundles_process::execute()
    {
        int nframes=1;
        parameters()->get_value("-nframes",nframes);

        dbdet_temporal_map_params params;
        unsigned int tneighbor;
        unsigned int niter;
        float derr;
        unsigned model_type=0;
        parameters()->get_value("-expmotion",tneighbor);
        parameters()->get_value("-niter",niter);
        parameters()->get_value("-derr",derr);
         parameters()->get_value("-temporal_model",model_type);
        params.tneighbor=tneighbor;
        params.no_of_relaxation_iterations=niter;
        params.derr=derr;
        if(model_type==0)
            params.type=params.constant_velocity;
        else
            params.type=params.normal_velocity;

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
                neighbor_cmap[i-nframes]=&input_sel->CM();
                neighbor_emap[i-nframes]=input_sel->EM();
            }
        }
        tmap->neighbor_cmap_=neighbor_cmap;
        tmap->neighbor_emap_=neighbor_emap;

        tmap->form_bundles();
        tmap->extract_one_chains_from_the_link_graph();
        tmap->extract_regions();
        //tmap->compute_RANSAC_V();
        //tmap->compute_motion_for_triangles();
        tmap->compute_hough_transform();
        tmap->compute_intial_motion();
        //tmap->update_delaunay_edge_weights();
        //tmap->dt_->threshold_delaunay_edges(0.01);
        //tmap->dt_->recompute_delaunay();
        //tmap->update_delaunay_edge_weights();


        //tmap->find_onesided_temporal_bundles();
        //tmap->pruning_neighbor_links();
        //tmap->optimize();
        tmap_storage->set_temporalmap(tmap);


        output_data_[0].push_back(tmap_storage);
        return true;  
}
//: Clone the process
bpro1_process*
spatemp_form_temporal_bundles_process::clone() const
    {
    return new spatemp_form_temporal_bundles_process(*this);
    }

bool
spatemp_form_temporal_bundles_process::finish()
    {
    return true;
    }


