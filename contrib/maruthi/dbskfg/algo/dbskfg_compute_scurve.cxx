// This is brcv/shp/dbskfg/dbskfg_compute_scurve.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/algo/dbskfg_compute_scurve.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_shock_link_boundary.h>

// vgl headers
#include <vgl/vgl_point_2d.h>

// dbsk2d headers
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_lineline.h>
#include <dbsk2d/dbsk2d_defines.h>
#include <dbsk2d/dbsk2d_ishock_pointpoint.h>
#include <dbsk2d/dbsk2d_ishock_pointline.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>

// dbskr headers
#include <dbskr/dbskr_scurve.h>

//: Constructor
dbskfg_compute_scurve::dbskfg_compute_scurve(vsol_box_2d_sptr bbox):bbox_(bbox)
{

}

//: Destructor
dbskfg_compute_scurve::~dbskfg_compute_scurve() 
{
    bbox_=0;
}

// compute shock curve
dbskr_scurve_sptr dbskfg_compute_scurve::compute_curve(
    dbskfg_composite_node_sptr start_node,
    vcl_vector<dbskfg_composite_link_sptr> path,
    bool leaf_edge, 
    bool binterpolate, bool bsub_sample,
    double interpolate_ds, double subsample_ds,
    double scale_ratio,
    bool mirror)
{
    vcl_vector< vgl_point_2d<double> > sh_pt;
    vcl_vector<double> time, theta, phi;

    dbskfg_composite_node_sptr cur_start_node = start_node;
    //traverse through the path, interpolating where necessary
    vcl_vector<dbskfg_composite_link_sptr>::iterator e_it;

    for (e_it = path.begin(); e_it != path.end(); e_it++)
    {
        // Create an xshock edge
        dbsk2d_shock_node_sptr parent_node = new dbsk2d_shock_node();
        dbsk2d_shock_node_sptr child_node  = new dbsk2d_shock_node();
        dbsk2d_xshock_edge cur_edge(1,parent_node,child_node);
        sample_shock_link(*e_it,cur_edge,mirror); 
   
        if (cur_start_node == (*e_it)->source())
        {
            //this edge is to be reconstructed the right way

            //go through the edge samples and append it to the shock curve
            for (int i=0; i< cur_edge.num_samples(); i++)
            {
                dbsk2d_xshock_sample_sptr sample = cur_edge.sample(i);
                vgl_point_2d<double> old_shock_pt=sample->pt;
                vgl_point_2d<double> new_shock_pt(old_shock_pt.x()*scale_ratio,
                                                  old_shock_pt.y()*scale_ratio);

                sh_pt.push_back(new_shock_pt);
                time.push_back(sample->radius*scale_ratio); 
                //100000 signals infinity
                if (sample->speed != 0 && sample->speed < 99990)
                {
                    phi.push_back(vcl_acos(-1.0/sample->speed));
                }
                else
                {
                    phi.push_back(vnl_math::pi/2);
                }

                //direction of flow is same as actual
                theta.push_back(sample->theta);
            }                

        }
        else
        {
            //go through the edge samples in the reverse order and append 
            // it to the shock curve
            for (int i=cur_edge.num_samples()-1; i>=0 ; i--)
            {
                dbsk2d_xshock_sample_sptr sample = cur_edge.sample(i);
                vgl_point_2d<double> old_shock_pt=sample->pt;
                vgl_point_2d<double> new_shock_pt(old_shock_pt.x()*scale_ratio,
                                                  old_shock_pt.y()*scale_ratio);
                
                sh_pt.push_back(new_shock_pt);
                time.push_back(sample->radius*scale_ratio); 
                //100000 signals infinity
                if (sample->speed != 0 && sample->speed < 99990)
                { 
                    phi.push_back(vnl_math::pi - vcl_acos(-1.0/sample->speed));
                }
                else
                {
                    phi.push_back(vnl_math::pi/2);
                }

                //direction of flow is reversed
                theta.push_back(angle0To2Pi(sample->theta+vnl_math::pi));
            }



        }

        //update the cur_start_node to the opposite node
        cur_start_node = (*e_it)->opposite(cur_start_node);
   
    }

    //close off the end with a circular arc if it is an A3
    unsigned num_points = sh_pt.size();

    if (leaf_edge)
    {
        double dphi = 0-phi[num_points-1]; //phi at the end is 0
        double apprxds = vcl_abs(dphi)*time[num_points-1];
    
        //num of extra samples
        int num = int(apprxds/interpolate_ds); 
    
        //add the interpolated samples
        for(int j=1; j<=num; j++)
        {
            double ratio = (double)j/(double)num;

            //at an A3, point remains stationary
            sh_pt.push_back(sh_pt[num_points-1]); 
            time.push_back(time[num_points-1]);
            theta.push_back(theta[num_points-1]);
            //phi needs to be interpolated
            phi.push_back(phi[num_points-1]+ratio*dphi);  
        }
    }
    
    dbskr_scurve_sptr shock_curve = new dbskr_scurve(sh_pt.size(),
                                                     sh_pt, time, theta, phi,
                                                     binterpolate, 
                                                     interpolate_ds,
                                                     bsub_sample, subsample_ds,
                                                     leaf_edge);

    return shock_curve;
}

void dbskfg_compute_scurve::sample_shock_link(dbskfg_composite_link_sptr link,
                                              dbsk2d_xshock_edge& xshock_edge,
                                              bool mirror)
{
    dbskfg_shock_link* shock_link = dynamic_cast<dbskfg_shock_link*>
        (&(*link));

    dbsk2d_ishock_edge* cur_iedge = shock_link->ishock_edge();

   
    // Make a dummy coarse shock graph
    dbsk2d_shock_graph_sptr coarse_graph;
    dbsk2d_sample_ishock sampler(coarse_graph);
    sampler.set_sample_resolution(1.0);

    if ( shock_link->shock_compute_type() == dbskfg_utilities::LL )
    {
   
        sampler.sample_ishock_edge((dbsk2d_ishock_lineline*)cur_iedge,
                                   &xshock_edge);
    }
    else if( shock_link->shock_compute_type() == dbskfg_utilities::LLRP )
    {

        sampler.sample_ishock_edge((dbsk2d_ishock_pointline*)cur_iedge,
                                   &xshock_edge);
 
    }
    else if( shock_link->shock_compute_type() == dbskfg_utilities::RLLP )
    {

        sampler.sample_ishock_edge((dbsk2d_ishock_pointline*)cur_iedge,
                                   &xshock_edge);
 
    }
    else if ( shock_link->shock_compute_type() == dbskfg_utilities::PP )
    {
        sampler.sample_ishock_edge((dbsk2d_ishock_pointpoint*)cur_iedge,
                                   &xshock_edge);
 
    }


    if ( mirror )
    {
        mirror_shock(xshock_edge);
    }
}

void dbskfg_compute_scurve::mirror_shock(dbsk2d_xshock_edge& xshock_edge)
{

    double width = bbox_->width();
    xshock_edge.clear_ex_pts();

    for(unsigned int i=0; i<xshock_edge.num_samples(); ++i)
    {
        dbsk2d_xshock_sample_sptr cur_sample = xshock_edge.sample(i);
        dbsk2d_xshock_sample_sptr new_sample = cur_sample;

        vgl_point_2d<double> cur_point = cur_sample->pt;
        vgl_point_2d<double> new_point(width-cur_point.x(),cur_point.y());
        new_sample->pt = new_point;
            
        vgl_point_2d<double> cur_right_bnd_pt = cur_sample->right_bnd_pt;
        vgl_point_2d<double> new_left_bnd_pt(width-cur_right_bnd_pt.x(),
                                             cur_right_bnd_pt.y());
        vgl_point_2d<double> cur_left_bnd_pt = cur_sample->left_bnd_pt;
        vgl_point_2d<double> new_right_bnd_pt(width-cur_left_bnd_pt.x(),
                                              cur_left_bnd_pt.y());
        new_sample->left_bnd_pt = new_left_bnd_pt;
        new_sample->right_bnd_pt = new_right_bnd_pt;

        double new_theta = vnl_math::pi - cur_sample->theta;
        new_sample->theta = new_theta;

        double new_left_bnd_tangent = vnl_math::pi - 
            cur_sample->right_bnd_tangent;
        double new_right_bnd_tangent = vnl_math::pi - 
            cur_sample->left_bnd_tangent;
        new_sample->left_bnd_tangent = new_left_bnd_tangent;
        new_sample->right_bnd_tangent = new_right_bnd_tangent;

        xshock_edge.set_sample(i,new_sample);
    }

    xshock_edge.compute_extrinsic_locus();

    


}
