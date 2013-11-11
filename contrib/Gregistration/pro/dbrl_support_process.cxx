
#include "dbrl_support_process.h"
#include <bpro1/bpro1_parameters.h>
#include <brip/brip_vil_float_ops.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/algo/dbdet_sel.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_edgel.h>

//: Constructor
dbrl_support_process::dbrl_support_process(void): bpro1_process()
    {
            if( !parameters()->add( "Position uncertainty" , "-dx" , 0.3 ) ||
            !parameters()->add( "Orientation uncertainty(Deg)" , "-dt" , 30.0 ) ||
            !parameters()->add( "Radius of Neighborhood" , "-nrad" , 3 )||
            !parameters()->add( "Maximum # of edgels to group" , "-max_size_to_group", (unsigned) 60 ) ||
            !parameters()->add( "threshold on size to estimate the true edges","-thresh_size", (unsigned) 30 )
            )
          {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
    }

    }
//: Destructor
dbrl_support_process::~dbrl_support_process()
    {
    }
//: Return the name of this process
vcl_string
dbrl_support_process::name()
    {
    return "Estimating Support";
    }
//: Return the number of input frame for this process
int
dbrl_support_process::input_frames()
    {
    return 1;
    }
//: Return the number of output frames for this process
int
dbrl_support_process::output_frames()
    {
    return 1;

    }
//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_support_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    return to_return;
    }
//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_support_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    return to_return;
    }
//: Execute the process
bool
dbrl_support_process::execute()
    {
        if ( input_data_.size() != 1 ){
            vcl_cout << "In dbrl_support_process::execute() - "
                << "not exactly two input images \n";
            return false;
        }

            static int nrad;
    static double dx;
    static double dt;
    unsigned max_size_to_group;
    unsigned thresh_size;

    parameters()->get_value( "-nrad", nrad);
    parameters()->get_value( "-dx", dx);
    parameters()->get_value( "-dt", dt);
    parameters()->get_value( "-max_size_to_group", max_size_to_group );
    parameters()->get_value( "-thresh_size", thresh_size );

        vidpro1_vsol2D_storage_sptr frame_vsols;
        frame_vsols.vertical_cast(input_data_[0][0]);

        vcl_vector< vsol_spatial_object_2d_sptr > lines=frame_vsols->all_data();

        vcl_vector<dbdet_edgel* > all_edgels;

        double xmax=0.0;
        double ymax=0.0;

        for (unsigned k=0; k<lines.size(); k++)
        {
            if(lines[k]->cast_to_curve())
            {
                if(vsol_line_2d_sptr l=lines[k]->cast_to_curve()->cast_to_line())
                {
                    vgl_point_2d<double> start(l->p0()->x(),l->p0()->y());
                    vgl_point_2d<double> end(l->p1()->x(),l->p1()->y());
                    vgl_point_2d<double> pt(l->middle()->x(), l->middle()->y());

                    if(pt.x()>xmax)
                        xmax=pt.x();
                    if(pt.y()>ymax)
                        ymax=pt.y();

                    double tan = dbdet_vPointPoint(start, end);
                    all_edgels.push_back(new dbdet_edgel(pt, tan));
                }
            }
        }

    dbdet_edgemap_sptr edgemap=new dbdet_edgemap(vcl_ceil(xmax)+5,vcl_ceil(ymax)+5,all_edgels);
    //different types of linkers depending on the curve model
    typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
    typedef dbdet_sel<dbdet_CC_curve_model> dbdet_sel_CC;

    dbdet_sel_sptr edge_linker= new dbdet_sel_CC(edgemap, nrad, dt*vnl_math::pi/180, dx);
    edge_linker->build_curvelets_greedy(max_size_to_group);

    vcl_vector<dbdet_edgel*> edgels=edge_linker->get_edgels();
    vcl_vector<dbdet_edgel*> sedgels;
    for(unsigned i=0;i<edgels.size();i++)
    {
            curvelet_list_iter cv_it = edgels[i]->curvelets.begin();
            for ( ; cv_it!=edgels[i]->curvelets.end(); cv_it++)
            {
                if((*cv_it)->edgel_chain.size()>thresh_size)
                {
                    sedgels.push_back(edgels[i]);
                    break;
                }
            }
        
    }
    //vcl_vector<vsol_line_2d_sptr> supportedges;

    vcl_vector<vsol_spatial_object_2d_sptr> supportedges;
    for(unsigned j=0;j<sedgels.size();j++)
    {
            vsol_point_2d_sptr p=new vsol_point_2d(sedgels[j]->pt);
            vsol_point_2d_sptr ps=new vsol_point_2d(p->x()+0.2*vcl_cos(sedgels[j]->tangent),p->y()+0.2*vcl_sin(sedgels[j]->tangent));
            vsol_point_2d_sptr pe=new vsol_point_2d(p->x()-0.2*vcl_cos(sedgels[j]->tangent),p->y()-0.2*vcl_sin(sedgels[j]->tangent));
            vsol_line_2d_sptr l=new vsol_line_2d(ps,pe);
            supportedges.push_back(l->cast_to_spatial_object());
    }


    vidpro1_vsol2D_storage_sptr output_edges=new vidpro1_vsol2D_storage();

    output_edges->add_objects(supportedges);
    output_data_[0].push_back(output_edges);
    return true;  
    }
//: Clone the process
bpro1_process*
dbrl_support_process::clone() const
    {
    return new dbrl_support_process(*this);
    }

bool
dbrl_support_process::finish()
    {

    return true;
    }





