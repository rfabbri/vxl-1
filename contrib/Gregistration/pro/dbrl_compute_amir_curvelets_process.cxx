
#include "dbrl_compute_amir_curvelets_process.h"
#include <bpro1/bpro1_parameters.h>
#include <brip/brip_vil_float_ops.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <pro/dbrl_id_point_2d_storage_sptr.h>
#include <pro/dbrl_id_point_2d_storage.h>
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
#include <georegister/dbrl_feature_point_tangent_curvature.h>
//: Constructor
dbrl_compute_amir_curvelets_process::dbrl_compute_amir_curvelets_process(void): bpro1_process()
    {
        if( !parameters()->add( "Grid equals image size" , "-use_image_size" , true ) ||
            !parameters()->add( "    Grid #cols" , "-ncols" , 300 ) ||
            !parameters()->add( "    Grid #rows" , "-nrows" , 300 ) ||
            //grouping parameters
            !parameters()->add( "Position uncertainty" , "-dx" , 0.3 ) ||
            !parameters()->add( "Orientation uncertainty(Deg)" , "-dt" , 30.0 ) ||
            !parameters()->add( "Radius of Neighborhood" , "-nrad" , 3 )||
            !parameters()->add( "Maximum # of edgels to group" , "-max_size_to_group", (unsigned) 7 ) ||
            !parameters()->add( "compute curvature for curvelets" , "-iscurvature" , bool(false) ) )
            {
                
            }
    }

//: Destructor
dbrl_compute_amir_curvelets_process::~dbrl_compute_amir_curvelets_process()
    {

    }


//: Return the name of this process
vcl_string
dbrl_compute_amir_curvelets_process::name()
    {
    return "Computing Amir Curvelets";
    }


//: Return the number of input frame for this process
int
dbrl_compute_amir_curvelets_process::input_frames()
    {
    return 1;
    }


//: Return the number of output frames for this process
int
dbrl_compute_amir_curvelets_process::output_frames()
    {
    return 1;

    }


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_compute_amir_curvelets_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
        to_return.push_back( "image" );

    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_compute_amir_curvelets_process::get_output_type()
    {  
        vcl_vector<vcl_string > to_return;
        to_return.push_back( "sel" );
        to_return.push_back( "vsol2D" );
        //to_return.push_back( "sel" );
        //to_return.push_back( "sel" );
        //to_return.push_back( "sel" );
        //to_return.push_back( "sel" );
        ////to_return.push_back( "sel" );
        return to_return;
    }


//: Execute the process
bool
dbrl_compute_amir_curvelets_process::execute()
    {
        if ( input_data_.size() != 1 ){
            vcl_cout << "In dbrl_compute_amir_curvelets_process::execute() - "
                << "not exactly two input images \n";
            return false;
        }
        // get image from the storage class
        vidpro1_vsol2D_storage_sptr input_vsol;
        input_vsol.vertical_cast(input_data_[0][0]);
        vcl_vector<vcl_string> groups=input_vsol->groups();
        vcl_vector< vsol_line_2d_sptr > alllines;
        for (unsigned c=0;c<groups.size();c++)
        {
            vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->data_named(groups[c]);
            vcl_vector< vsol_line_2d_sptr > lines;
            for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) {
                if( vsol_list[b]->cast_to_curve()){
                    //LINE
                    if( vsol_list[b]->cast_to_curve()->cast_to_line() ){
                        vsol_line_2d_sptr eline = vsol_list[b]->cast_to_curve()->cast_to_line();
                        lines.push_back(eline);
                    }
                }
            }
           //compute_curvelets_from_edges(lines);
           alllines.insert(alllines.end(),lines.begin(),lines.end());
        }
        compute_curvelets_from_edges(alllines);
        return true;  
}
//: Clone the process
bpro1_process*
dbrl_compute_amir_curvelets_process::clone() const
    {
    return new dbrl_compute_amir_curvelets_process(*this);
    }

bool
dbrl_compute_amir_curvelets_process::finish()
    {
    return true;
    }


bool dbrl_compute_amir_curvelets_process::compute_curvelets_from_edges(vcl_vector<vsol_line_2d_sptr> lines)
{
    static int nrad;
    static double dx;
    static double dt;
    static int ncols;
    static int nrows;
    bool use_image_size;
    bool output_vsol;
    int min_size_to_keep;

    unsigned max_size_to_group;
    parameters()->get_value( "-use_image_size", use_image_size);
    if (!use_image_size) {
        parameters()->get_value( "-nrows", nrows);
        parameters()->get_value( "-ncols", ncols);
    } 
    else {
        vcl_cout << "Using image size for the grid\n";
        // get image from the storage class
        vidpro1_image_storage_sptr frame_image;
        frame_image.vertical_cast(get_input(0)[1]);
        if (!frame_image) {
            vcl_cout << "Error: no image in input storages\n";
            return  false;
        }
        vil_image_resource_sptr image_sptr = frame_image->get_image();
        ncols = image_sptr->ni();
        nrows = image_sptr->nj();
        vcl_cout << "Nrows: " << nrows;
        vcl_cout << "  Ncols: " << ncols << vcl_endl;
    }

    parameters()->get_value( "-nrad", nrad);
    parameters()->get_value( "-dx", dx);
    parameters()->get_value( "-dt", dt);
    parameters()->get_value( "-output_vsol", output_vsol );
    parameters()->get_value( "-min_size_to_keep", min_size_to_keep );
    parameters()->get_value( "-max_size_to_group", max_size_to_group );


    vcl_vector<dbdet_edgel* > all_edgels;

    //convert from vsol2D to edgels
    for (unsigned int b = 0 ; b < lines.size() ; b++ ) {
        if( lines[b]->cast_to_curve()){
            //LINE
            if( lines[b]->cast_to_curve()->cast_to_line() ){
                vsol_line_2d_sptr eline = lines[b]->cast_to_curve()->cast_to_line();
                vgl_point_2d<double> spt(eline->p0()->x(), eline->p0()->y());
                vgl_point_2d<double> ept(eline->p1()->x(), eline->p1()->y());
                vgl_point_2d<double> pt(eline->middle()->x(), eline->middle()->y());
                double tan = dbdet_vPointPoint(spt, ept);
                all_edgels.push_back(new dbdet_edgel(pt, tan));
            }
        }
    }
    dbdet_edgemap_sptr edgemap=new dbdet_edgemap(ncols,nrows,all_edgels);
    //different types of linkers depending on the curve model
    typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
      typedef dbdet_sel<dbdet_CC_curve_model> dbdet_sel_CC;

    dbdet_sel_sptr edge_linker= new dbdet_sel_CC(edgemap, nrad, dt*vnl_math::pi/180, dx);
    //dbdet_sel_ES( , nrad, dt*vnl_math::pi/180, dx);

    //build the edgel neighborhood first
    edge_linker->build_curvelets_greedy(max_size_to_group);
    //perform local edgel grouping
    dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

    vcl_vector<dbdet_edgel*> edgels=edge_linker->get_edgels();
    vcl_vector<dbrl_feature_point_tangent_curvature *> edges;
    for(unsigned i=0;i<edgels.size();i++)
    {
        dbrl_feature_point_tangent_curvature * pt;
        if(edgels[i]->curvelets.size()==1)
        {
            curvelet_list_iter cv_it = edgels[i]->curvelets.begin();
            for ( ; cv_it!=edgels[i]->curvelets.end(); cv_it++)
            {
                dbdet_curvelet* cvlet = (*cv_it);
                if(cvlet->edgel_chain.size()>2)
                {
                    if(dbdet_ES_curve_model * esmodel
                        =dynamic_cast<dbdet_ES_curve_model *>((edgels[i]->curvelets).front()->curve_model))
                    {
                        pt=new dbrl_feature_point_tangent_curvature(esmodel->pt.x(),esmodel->pt.y(),esmodel->theta);
                        pt->set_k(esmodel->k);
                        pt->compute_arc(1.0);
                    }
                }
            }
        }
        else
        {
             pt=new dbrl_feature_point_tangent_curvature(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent);
        }
        edges.push_back(pt);
    }

    vidpro1_vsol2D_storage_sptr arcout=new vidpro1_vsol2D_storage();
    for(unsigned i=0;i<edges.size();i++)
    {
        if(dbrl_feature_point_tangent_curvature * pt
            =dynamic_cast<dbrl_feature_point_tangent_curvature *>(edges[i]))
        {
            if(pt->k_flag())
            {   
                //dbgl_circ_arc carc=pt->get_arc();
                //dbsol_circ_arc_2d_sptr dbsolarc=new dbsol_circ_arc_2d(carc);
            }
        }
    }
    output_sel->set_sel(edge_linker);
    output_data_[0].push_back(output_sel);
    output_data_[0].push_back(arcout);
    return true;
}
