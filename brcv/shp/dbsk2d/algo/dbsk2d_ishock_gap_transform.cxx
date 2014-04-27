// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_gap_transform.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_ishock_gap_transform.h>
#include <dbsk2d/dbsk2d_transform_manager.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <vgl/vgl_line_2d.h>
#include <dbgl/algo/dbgl_eulerspiral.h>
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vgl/vgl_lineseg_test.h>
#include <vgl/vgl_distance.h>
// vnl random
#include <vnl/vnl_random.h>

//: constructor
//: compute the salency of this shock element (edge/node)
dbsk2d_ishock_gap_transform::dbsk2d_ishock_gap_transform(
    dbsk2d_ishock_graph_sptr intrinsic_shock_graph,
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*>& pair,
    int euler_spiral_id)
    :dbsk2d_ishock_transform(intrinsic_shock_graph,
                             dbsk2d_ishock_transform::GAP),
     gap_endpoint_(pair),
     likelihood_(0.0),
     euler_spiral_id_(euler_spiral_id)
{
    determine_likelihood();
}

//: remove boundary element
bool dbsk2d_ishock_gap_transform::execute_transform()
{
    
    this->clear();

    dbsk2d_ishock_bpoint* bp1 = gap_endpoint_.first;
    dbsk2d_ishock_bpoint* bp2 = gap_endpoint_.second;

    // First Try
    if ( local_belm_list_.size() )
    {
        // Not Hidden need hide everything
        if ( local_belm_list_[1]->is_a_GUIelm())
        {

            // We are undoing this gap
            // 1 Remove euler spiral from endpoints
            bp1->disconnectFrom(local_belm_list_[1]);
            bp1->disconnectFrom(local_belm_list_[2]);
    
            if ( local_belm_list_.size() > 4 )
            {
                bp2->disconnectFrom(
                    local_belm_list_[local_belm_list_.size()-2]);
                bp2->disconnectFrom(
                    local_belm_list_[local_belm_list_.size()-3]);
            }
            else
            {
                 bp2->disconnectFrom(local_belm_list_[1]);
                 bp2->disconnectFrom(local_belm_list_[2]);
                 
            }
         
            delete_belm_shocks(bp1);
            delete_belm_shocks(bp2);

            // 2 Remove all shocks from inserted Euler Spiral and hide curve
            for ( unsigned int i=1; i < local_belm_list_.size()-1; ++i)
            {
                delete_belm_shocks(local_belm_list_[i]);
                local_belm_list_[i]->set_GUIelm(false);

                dbsk2d_ishock_bpoint* startpt=local_belm_list_[i]->s_pt();
                dbsk2d_ishock_bpoint* endpt=local_belm_list_[i]->e_pt();

                if ( startpt->is_a_GUIelm() && 
                     startpt->id() != bp1->id() &&
                     startpt->id() != bp2->id() )
                {
                    delete_belm_shocks(startpt);
                    startpt->set_GUIelm(false);
                }

                if ( endpt->is_a_GUIelm() &&
                     endpt->id() != bp1->id() &&
                     endpt->id() != bp2->id() )
                {
                    delete_belm_shocks(endpt);
                    endpt->set_GUIelm(false);
                }
             
   
            }
            
            // Delete vertices
            delete_shock_vertices();

            // 4. Reinitialize contact shocks for endpoints

            belm_list belm_list=bp1->LinkedBElmList;    
            dbsk2d_ishock_belm* left = *(belm_list.begin());
            dbsk2d_ishock_belm* right = *(++belm_list.begin());
            
            bp1->set_visibility(true);
            form_contact_shocks(left,right,bp1);

            belm_list=bp2->LinkedBElmList;    
            left = *(belm_list.begin());
            right = *(++belm_list.begin());
            
            bp2->set_visibility(true);
            form_contact_shocks(left,right,bp2);
            
            interacting_bnd_elements_[bp1->id()]=bp1;
            interacting_bnd_elements_[bp2->id()]=bp2;
            
            bp1->set_max_eta(vnl_math::pi);
            bp2->set_max_eta(vnl_math::pi);

            
           
        }
        else // Hidden need to reactivate everything
        {
            delete_belm_shocks(bp1);
            delete_belm_shocks(bp2);
            delete_shock_vertices();

            // This transform was hidden lets reactive
            // 1 Reconnect euler spiral to  endpoints
            bp1->connectTo(local_belm_list_[1]);
            bp1->connectTo(local_belm_list_[2]);

            bp2->connectTo(local_belm_list_[local_belm_list_.size()-2]);
            bp2->connectTo(local_belm_list_[local_belm_list_.size()-3]);
         
            // 2 Reactivate all components of euler spiral
            for ( unsigned int i=1; i < local_belm_list_.size()-1; ++i)
            {
                local_belm_list_[i]->set_GUIelm(true);

                dbsk2d_ishock_bpoint* startpt=local_belm_list_[i]->s_pt();
                dbsk2d_ishock_bpoint* endpt=local_belm_list_[i]->e_pt();
                
                startpt->set_GUIelm(true);
                endpt->set_GUIelm(true);

            }
            
            // 3 Recreate contact shocks from by inserting boundary
            ishock_detector_.initialize_contacts_and_A3s(local_belm_list_);

            // 4. Reattach to interacinting bnd elements
            for ( unsigned int i=0; i < local_belm_list_.size() ; ++i)
            {
                interacting_bnd_elements_[local_belm_list_[i]->id()]=
                    local_belm_list_[i];
            }

        }
        
    }
    else
    {

        delete_belm_shocks(bp1);
        delete_belm_shocks(bp2);
        delete_shock_vertices();
        add_euler_spiral(bp1,
                         bp2);
       
    }

    local_shock_compute();
    bool shock_computation_valid = ishock_graph_->valid_shock_graph();
    ishock_graph_->update_shocks();

    if ( shock_computation_valid == false )
    {
        unsigned int iteration=1;
        while ( true )
        {
            // Grab all elements of active shocks
            vcl_vector<dbsk2d_ishock_edge*> invalid_shocks;
            ishock_graph_->invalid_shocks(invalid_shocks);
            
            // Grab elements of delete shocks
            vcl_map<unsigned int,dbsk2d_ishock_belm*> deleted_bnd_elements
                = ishock_detector_.get_deleted_bnd_elements();

            if ( invalid_shocks.size() == 0 )
            {
                break;
            }
            dbsk2d_ishock_belm::throw_exception=false;
            ++iteration;

            if ( iteration == 5 )
            {
                vcl_cerr<<"Error: Disconnecting Euler Spiral"<<vcl_endl;
                // Not Hidden need hide everything
                if ( local_belm_list_[1]->is_a_GUIelm())
                {

                    // We are undoing this gap
                    // 1 Remove euler spiral from endpoints
                    bp1->disconnectFrom(local_belm_list_[1]);
                    bp1->disconnectFrom(local_belm_list_[2]);
    
                    if ( local_belm_list_.size() > 4 )
                    {
                        bp2->disconnectFrom(
                            local_belm_list_[local_belm_list_.size()-2]);
                        bp2->disconnectFrom(
                            local_belm_list_[local_belm_list_.size()-3]);
                    }
                    else
                    {
                        bp2->disconnectFrom(local_belm_list_[1]);
                        bp2->disconnectFrom(local_belm_list_[2]);
                 
                    }
         
                    // Hide curve
                    for ( unsigned int i=1; i < local_belm_list_.size()-1; ++i)
                    {
                    
                        local_belm_list_[i]->set_GUIelm(false);

                        dbsk2d_ishock_bpoint* startpt=
                            local_belm_list_[i]->s_pt();
                        dbsk2d_ishock_bpoint* endpt=
                            local_belm_list_[i]->e_pt();

                        if ( startpt->is_a_GUIelm() && 
                             startpt->id() != bp1->id() &&
                             startpt->id() != bp2->id() )
                        {                            
                            startpt->set_GUIelm(false);
                        }

                        if ( endpt->is_a_GUIelm() &&
                             endpt->id() != bp1->id() &&
                             endpt->id() != bp2->id() )
                        {
                            endpt->set_GUIelm(false);
                        }
             
   
                    }
            
                   
                }
               
                bp1->set_visibility(true);
                bp2->set_visibility(true);            
                bp1->set_max_eta(2.0*vnl_math::pi);
                bp2->set_max_eta(2.0*vnl_math::pi);
                bp1->set_vref(-1);
                bp2->set_vref(-1);

                return false;
            }

            vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
            for ( it = deleted_bnd_elements.begin();
                  it != deleted_bnd_elements.end();
                  ++it)
            {
                interacting_bnd_elements_[(*it).first]=(*it).second;
            }

            if ( deleted_bnd_elements.size() > 0)
            {
                for ( unsigned int i=0; i < invalid_shocks.size() ; ++i)
                {
                    dbsk2d_ishock_edge* edge=invalid_shocks[i];
                    dbsk2d_ishock_belm* left_belm=edge->lBElement();
                    dbsk2d_ishock_belm* right_belm=edge->rBElement();
                    interacting_bnd_elements_[left_belm->id()]=left_belm;
                    interacting_bnd_elements_[right_belm->id()]=right_belm;
                   
                }
            }
            else if( iteration > 2 )
            {
                outer_wavefront_.clear();
                for ( unsigned int i=0; i < invalid_shocks.size() ; ++i)
                {
                    dbsk2d_ishock_edge* edge=invalid_shocks[i];

                    if ( edge->endTime() == ISHOCK_DIST_HUGE )
                    {
                        if ( !edge->is_a_contact())
                        {
                            delete_shock_and_update(invalid_shocks[i]);
                        }
                        else
                        {
                            edge->reset_shock();
                        }
                    }
                }
                delete_shock_vertices();
            }

            ishock_detector_.clear_deleted_elements();
            local_shock_compute();
            ishock_graph_->update_shocks();
        }
    }

    dbsk2d_ishock_belm::throw_exception=true;
    shock_computation_valid = ishock_graph_->valid_shock_graph();
    return shock_computation_valid;

}


//: Add euler spiral to boundary and add all boundary elements
void dbsk2d_ishock_gap_transform::add_euler_spiral(dbsk2d_ishock_bpoint* bp1,
                                                   dbsk2d_ishock_bpoint* bp2)
{
    bp1->set_max_eta(2*vnl_math::pi);
    bp1->set_vref(-1);
   
    bp2->set_max_eta(2*vnl_math::pi);
    bp2->set_vref(-1);

    // For each endpoint grap its corresponiding elements
    dbsk2d_ishock_belm* bl1=(*bp1->LinkedBElmList.begin());
    dbsk2d_ishock_belm* bl2=(*bp2->LinkedBElmList.begin());
    local_belm_list_.push_back(bl1);

    // 4) Now fit lines to euler spiral
    int min_fit_length = 2;
    vgl_fit_lines_2d<double> fitter;
    fitter.set_min_fit_length(min_fit_length);
    fitter.set_rms_error_tol(0.05f);
    fitter.add_curve(init_samples_);

    vcl_vector<vgl_line_segment_2d<double> > segs;
    if ( init_samples_.size() > 2 )
    {
        fitter.fit();
        segs= fitter.get_line_segs();
    }
    else
    {
        segs=fitter.get_line_segs();
        
    }

    // 5) Check if Euler Spiral intersects any bnd elements
    bool valid=true;
    // See if segs intersects any bnd elements
    for ( unsigned int i=0; i < segs.size() ; ++i)
    {
        vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator bit;
        for ( bit = interacting_bnd_elements_.begin(); 
              bit != interacting_bnd_elements_.end(); ++bit)
        {
            dbsk2d_ishock_belm* belm=(*bit).second;
            if ( belm->is_a_line())
            {
                dbsk2d_ishock_bline* bline=(dbsk2d_ishock_bline*)belm;
                if ( (bline->s_pt()->id() != bp1->id() &&
                      bline->s_pt()->id() != bp2->id())
                     &&
                     (bline->e_pt()->id() != bp1->id() &&
                      bline->e_pt()->id() != bp2->id()))
                {
                    vgl_line_segment_2d<double> contour(bline->s_pt()->pt(),
                                                        bline->e_pt()->pt());
                    if ( vgl_lineseg_test_lineseg(segs[i],contour))
                    {
                        valid=false;
                        break;
                    }
                    
                }
            }

            if ( !valid)
            {
                break;
            }

        }
    }

    if ( valid == false )
    {
        vcl_cout<<
        "Curve intersects with bnd elements insert a straight line"<<vcl_endl;
    }

    // convert the pts into bnd_vertex and put into a list
    vcl_vector<dbsk2d_bnd_vertex_sptr > bv_list;

    // Add in first two points of line segment
    bv_list.push_back(bp1->bnd_vertex());
    if ( valid && segs.size() > 1 )
    {
        bv_list.push_back(dbsk2d_bnd_utils::new_vertex(
                              segs[0].point2(), 
                              boundary_));

        for (unsigned int i = 1; i<segs.size()-1; i++) 
        {
            bv_list.push_back(dbsk2d_bnd_utils::new_vertex(
                                  segs[i].point2(), 
                                  boundary_));        
        }
    }
    bv_list.push_back(bp2->bnd_vertex());

    // now link all these vertices into a chain and save as a contour
    vcl_vector<dbsk2d_bnd_edge_sptr > bnd_edges;
    for (unsigned int i=0; i<bv_list.size()-1; ++i)
    {
       
        bnd_edges.push_back(dbsk2d_bnd_utils::new_line_between(
                                bv_list[i], 
                                bv_list[i+1], 
                                boundary_));
   
    }

    vcl_vector<signed char > directions(bnd_edges.size(), 1);

    dbsk2d_bnd_contour_sptr euler_spiral = new dbsk2d_bnd_contour(
        bnd_edges, 
        directions, 
        euler_spiral_id_);
    boundary_->update_belm_list(euler_spiral,local_belm_list_);
    local_belm_list_.push_back(bl2);

    // Populate interacting bnd elements  
    ishock_detector_.initialize_contacts_and_A3s(local_belm_list_);

    // Add to interacting bnd elements
    for ( unsigned int i=0; i < local_belm_list_.size() ; ++i)
    {
        interacting_bnd_elements_[local_belm_list_[i]->id()]=
            local_belm_list_[i];
       
    }

    if ( minimal_interacting_elements_.size() == 0 )
    {
        minimal_interacting_elements_.insert(euler_spiral->get_id());
            
    }
    
    euler_spiral = 0 ;
}

void dbsk2d_ishock_gap_transform::determine_likelihood()
{

    dbsk2d_ishock_bpoint* bp1 = gap_endpoint_.first;
    dbsk2d_ishock_bpoint* bp2 = gap_endpoint_.second;

    // Compute Euler Spiral
    // 1) Determine Tangent Pairs
    vcl_pair<double,double> tangent_pair=this->get_tangent_pairs(bp1,
                                                                 bp2);
    // 2) Compute Euler Spiral
    dbgl_eulerspiral es(
        bp1->pt(),
        tangent_pair.first,
        bp2->pt(),
        tangent_pair.second);

    es.compute_spiral(init_samples_, 0.1);

    vnl_random mz_random;
    mz_random.reseed((unsigned long)time(NULL));
    float noise_radius=0.002f;
    
    for ( unsigned int i=1; i < init_samples_.size()-1 ; ++i)
    {
        vgl_point_2d<double> point=init_samples_[i];
        double x=point.x();
        double y=point.y();
        double rand_x = mz_random.drand32(1.0);
        x += 2.0*noise_radius*(rand_x-0.5);
        double rand_y = mz_random.drand32(1.0);
        y += 2.0*noise_radius*(rand_y-0.5);
        init_samples_[i].set(x,y);


    }
 
    double length_of_gap=vgl_distance(bp1->pt(),
                                      bp2->pt());

    // likelihood_ = 
    //     dbsk2d_transform_manager::Instance().transform_probability(
    //         es.gamma()* length_of_gap* length_of_gap,
    //         es.k0()*length_of_gap,
    //         es.length());
 
    likelihood_ = 
        dbsk2d_transform_manager::Instance().transform_probability(
            init_samples_);

}

//: Add euler spiral to boundary and add all boundary elements
vcl_pair<double,double> dbsk2d_ishock_gap_transform::get_tangent_pairs(
    dbsk2d_ishock_bpoint* bp1,
    dbsk2d_ishock_bpoint* bp2)
{
    double ltan(0.0);
    double rtan(0.0);

    dbsk2d_ishock_belm* bl1=(*bp1->LinkedBElmList.begin());
    dbsk2d_ishock_belm* bl2=(*bp2->LinkedBElmList.begin());

    // remove the polarization using a reference direction
    if ( bp1->id() == bl1->e_pt()->id())
    {
        vgl_line_2d<double> ref_line(bl1->e_pt()->pt(),
                                     bl1->s_pt()->pt());
        ltan=ref_line.slope_radians();
    }
    else
    {
        vgl_line_2d<double> ref_line(bl1->s_pt()->pt(),
                                     bl1->e_pt()->pt());
        ltan=ref_line.slope_radians();

    }

    // remove the polarization using a reference direction
    if ( bp2->id() == bl2->e_pt()->id())
    {
        vgl_line_2d<double> ref_line(bl2->s_pt()->pt(),
                                     bl2->e_pt()->pt());
        rtan=ref_line.slope_radians();
    }
    else
    {
        vgl_line_2d<double> ref_line(bl2->e_pt()->pt(),
                                     bl2->s_pt()->pt());
        rtan=ref_line.slope_radians();

    }

    return vcl_make_pair(ltan,rtan);

}
