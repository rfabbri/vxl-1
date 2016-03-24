// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_gap4_transform.cxx

//:
// \file

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap4_transform.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <dbsk2d/dbsk2d_transform_manager.h>
#include <vgl/vgl_closest_point.h>

//: constructor
//: compute the salency of this shock element (edge/node)
dbsk2d_ishock_gap4_transform::dbsk2d_ishock_gap4_transform(
    dbsk2d_ishock_graph_sptr intrinsic_shock_graph,
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*>& pair,
    dbsk2d_ishock_bpoint* anchor_pt,
    int euler_spiral_id)
    :dbsk2d_ishock_transform(intrinsic_shock_graph,
                             dbsk2d_ishock_transform::GAP4),
     gap_line_pair_(pair),
     anchor_pt_(anchor_pt),
     euler_spiral_id_(euler_spiral_id)
{
    dbsk2d_ishock_bpoint* bp1 = gap_line_pair_.first;
    dbsk2d_ishock_bline*  bl1 = gap_line_pair_.second;

    double d1=vgl_distance(bp1->pt(),bl1->s_pt()->pt());
    double d2=vgl_distance(bp1->pt(),bl1->e_pt()->pt());
    
    // convert the pts into bnd_vertex and put into a list
    vcl_vector<vgl_point_2d<double> > bv_list;

    anchor_pt_visible_=anchor_pt_->is_visible();
    
    
}

//: compute likelihood
double dbsk2d_ishock_gap4_transform::likelihood()
{
 
    dbsk2d_ishock_bpoint* bp1=gap_line_pair_.first;

    vgl_line_segment_2d<double> line_segment(bp1->pt(),
                                             anchor_pt_->pt());

    vcl_vector<vgl_point_2d<double> > bv_list;
    for ( unsigned int i=0; i <= 10 ; ++i)
    {
        double sample = i/10;
        bv_list.push_back(line_segment.point_t(sample));
    }

    double likelihood = 
        dbsk2d_transform_manager::Instance().transform_probability(
            bv_list);

    return likelihood;
}

//: remove boundary element
bool dbsk2d_ishock_gap4_transform::execute_transform()
{
    
    this->clear();

    dbsk2d_ishock_bpoint* bp1 = gap_line_pair_.first;
    dbsk2d_ishock_bline*  bl2 = gap_line_pair_.second;

    // First Try
    if ( local_belm_list_.size() )
    {

        dbsk2d_ishock_belm* left_line=local_belm_list_[0];
        dbsk2d_ishock_belm* right_line=local_belm_list_[1];

        dbsk2d_ishock_bpoint* bp2=(left_line->s_pt()->id()==bp1->id())
            ?left_line->e_pt():left_line->s_pt();

        // We are undoing this gap
        // 1 Remove line from endpoints
        bp1->disconnectFrom(left_line);
        bp1->disconnectFrom(right_line);

        bp2->disconnectFrom(left_line);
        bp2->disconnectFrom(right_line);
        
        // 2 Totally delete line
        
        // 2 Delete shocks from lines
        delete_belm_shocks(left_line);
        delete_belm_shocks(right_line);
        delete_belm_shocks(bp1);

        if ( interacting_bnd_elements_.count(bp2->id()))
        {
            delete_belm_shocks(bp2);
        }

        left_line->set_GUIelm(false);
        right_line->set_GUIelm(false);
        
        // Delete vertices
        delete_shock_vertices();
        
        // 3. Reinitialize contact shocks for endpoints and point
        
        belm_list belm_list=bp1->LinkedBElmList;    
        dbsk2d_ishock_belm* left = *(belm_list.begin());
        dbsk2d_ishock_belm* right = *(++belm_list.begin());
        
        bp1->set_visibility(true);
        form_contact_shocks(left,right,bp1);
        
        bp2->set_visibility(anchor_pt_visible_);
        form_contact_shocks(contour_pair_.first,
                            contour_pair_.second,bp2);
        bp1->set_max_eta(vnl_math::pi);
            
        interacting_bnd_elements_[bp1->id()]=bp1;
        interacting_bnd_elements_[bp2->id()]=bp2;
        
        // local_belm_list_.clear();
        // boundary_->remove_a_bnd_contour(contour_);
        
        // contour_=0;
    }
    else
    {

        delete_belm_shocks(bp1);
        delete_shock_vertices();
        add_connecting_line(bp1,
                            bl2);
        delete_shock_vertices();
    }

    local_shock_compute();
    bool shock_computation_valid = ishock_graph_->valid_shock_graph(true);
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
                dbsk2d_ishock_belm* left_line=local_belm_list_[0];
                dbsk2d_ishock_belm* right_line=local_belm_list_[1];

                dbsk2d_ishock_bpoint* bp2=(left_line->s_pt()->id()==bp1->id())
                    ?left_line->e_pt():left_line->s_pt();

                // We are undoing this gap
                // 1 Remove line from endpoints
                bp1->disconnectFrom(left_line);
                bp1->disconnectFrom(right_line);

                bp2->disconnectFrom(left_line);
                bp2->disconnectFrom(right_line);

                left_line->set_GUIelm(false);
                right_line->set_GUIelm(false);

                bp1->set_visibility(true);
                bp2->set_visibility(anchor_pt_visible_);            
                bp1->set_max_eta(2.0*vnl_math::pi);
                bp2->set_max_eta(2.0*vnl_math::pi);
                bp1->set_vref(-1);
                bp2->set_vref(-1);
            
                local_belm_list_.clear();
                boundary_->remove_a_bnd_contour(contour_);
        
                contour_=0;

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

    if ( anchor_pt_->shock_map().size()==0)
    {
        anchor_pt_->set_max_eta(2*vnl_math::pi);
        anchor_pt_->set_vref(-1);
        anchor_pt_->set_visibility(false);
    }

    dbsk2d_ishock_belm::throw_exception=true;
    shock_computation_valid = ishock_graph_->valid_shock_graph(true);
    return shock_computation_valid;

}


//: Add euler spiral to boundary and add all boundary elements
void dbsk2d_ishock_gap4_transform::add_connecting_line(
    dbsk2d_ishock_bpoint* bp1,
    dbsk2d_ishock_bline* bl1)
{
    // Determine eta , of where endpoint and line meet

    vgl_line_segment_2d<double> line(bl1->s_pt()->pt(),
                                     bl1->e_pt()->pt());
    
    vgl_point_2d<double> closest_pt = vgl_closest_point(line,bp1->pt());

    double d1=vgl_distance(bp1->pt(),bl1->s_pt()->pt());
    double d2=vgl_distance(bp1->pt(),bl1->e_pt()->pt());

    dbsk2d_ishock_bpoint* s_pt=bl1->s_pt();
    dbsk2d_ishock_bpoint* e_pt=bl1->e_pt();

    dbsk2d_ishock_bline* s_pt_line=(
        s_pt->getElmToTheLeftOf(bl1)->id() == bl1->twinLine()->id())?
        (dbsk2d_ishock_bline*)s_pt->getElmToTheRightOf(bl1):
        (dbsk2d_ishock_bline*)s_pt->getElmToTheLeftOf(bl1);

    dbsk2d_ishock_bline* e_pt_line=(
        e_pt->getElmToTheLeftOf(bl1)->id() == bl1->twinLine()->id())?
        (dbsk2d_ishock_bline*)e_pt->getElmToTheRightOf(bl1):
        (dbsk2d_ishock_bline*)e_pt->getElmToTheLeftOf(bl1);

    // convert the pts into bnd_vertex and put into a list
    vcl_vector<dbsk2d_bnd_vertex_sptr > bv_list;

    dbsk2d_ishock_bpoint* bp2(0);

    double min_eta,max_eta;

    // Add in first two points of line segment
    bv_list.push_back(bp1->bnd_vertex());   
    if ( bl1->s_pt() == anchor_pt_ )
    {
        bp2=bl1->s_pt();
        bv_list.push_back(bl1->s_pt()->bnd_vertex());

        min_eta=bl1->min_eta();
        max_eta=vgl_distance(closest_pt,bl1->s_pt()->pt());
    }
    else
    {
        bp2=bl1->e_pt();
        bv_list.push_back(bl1->e_pt()->bnd_vertex());

        min_eta=vgl_distance(closest_pt,bl1->s_pt()->pt());
        max_eta=bl1->max_eta();
    }

    dbsk2d_ishock_bline* bl2(0);

    if ( bp2->getElmToTheLeftOf(bl1)->id() == bl1->twinLine()->id())
    {
        bl2=(dbsk2d_ishock_bline*)bp2->getElmToTheRightOf(bl1);
    }
    else
    {
        bl2=(dbsk2d_ishock_bline*)bp2->getElmToTheLeftOf(bl1);
    }

    contour_pair_.first= bl1;
    contour_pair_.second=bl2;

    if ( interacting_bnd_elements_.count(bl2->id())==0)
    {
        dbsk2d_ishock_belm::throw_exception = false;
        // belm_list belm_list=bp1->LinkedBElmList;    
        // dbsk2d_ishock_belm* left = *(belm_list.begin());
        // dbsk2d_ishock_belm* right = *(++belm_list.begin());

        // form_contact_shocks(left,right,bp1);

        // return;
    }

    vcl_vector<dbsk2d_ishock_edge*> shocks_to_delete;

    bnd_ishock_map_iter curS = bl1->shock_map().begin();
    for ( ; curS != bl1->shock_map().end() ; ++curS)
    {
        dbsk2d_ishock_elm* selm = curS->second;
        
        if ( selm->is_a_link() ) 
        {
            if ( curS->first.s_eta >= min_eta && curS->first.s_eta <= max_eta)
            {
                dbsk2d_ishock_edge* iedge = (dbsk2d_ishock_edge*)(curS->second);
                if ( !iedge->is_a_contact())
                {
                    shocks_to_delete.push_back(iedge);
                }
            }
        }
    }

    while ( shocks_to_delete.size())
    {
        dbsk2d_ishock_edge* cur_edge=shocks_to_delete.back();
        shocks_to_delete.pop_back();
        delete_shock_and_update(cur_edge);

    }


    curS = bl1->shock_map().begin();
    for ( ; curS != bl1->shock_map().end() ; ++curS)
    {
        dbsk2d_ishock_elm* selm = curS->second;
        

        if ( selm->is_a_link())
        {
            dbsk2d_ishock_edge* iedge = (dbsk2d_ishock_edge*)(curS->second);
            if ( iedge->lBElement()->id() == bl2->id())
            {
                delete_shock(iedge);
                break;
            }
            else if (iedge->rBElement()->id() == bl2->id())
            {
                delete_shock(iedge);
                break;
            }
        }

    }

    bool flag=false;
    vcl_vector<dbsk2d_ishock_edge*> edges_to_delete;
    curS = bp2->shock_map().begin();
    for ( ; curS != bp2->shock_map().end() ; ++curS)
    {
        dbsk2d_ishock_elm* selm = curS->second;

        if ( selm->is_a_link())
        {
            dbsk2d_ishock_edge* iedge = (dbsk2d_ishock_edge*)(curS->second);
            if ( iedge->lBElement()->id() == bl2->id() ||
                 iedge->lBElement()->id() == bl1->id() )
            {
                flag=true;
                edges_to_delete.push_back(iedge);

            }
            else if (iedge->rBElement()->id() == bl2->id() ||
                     iedge->rBElement()->id() == bl1->id() )
            {
                flag=true;
                edges_to_delete.push_back(iedge);
                
            }
        }
    }

    if ( flag )
    {
        while(edges_to_delete.size() > 0 )
        {
            dbsk2d_ishock_edge* cur_edge=edges_to_delete.back();
            edges_to_delete.pop_back();
            delete_shock_and_update(cur_edge);
        }
        
    }

    vcl_vector<dbsk2d_bnd_edge_sptr> bnd_edges;
    bnd_edges.push_back(dbsk2d_bnd_utils::new_line_between(
                            bv_list[0], 
                            bv_list[1], 
                            boundary_));

    vcl_vector<signed char > directions(bnd_edges.size(), 1);
    contour_ = new dbsk2d_bnd_contour(
        bnd_edges, 
        directions, 
        euler_spiral_id_);


    boundary_->update_belm_list(contour_,local_belm_list_);

    vcl_vector<vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_belm*> > 
        contact_shock_pairs;

    belm_list belm_list=bp1->LinkedBElmList;    
    dbsk2d_ishock_belm* left = *(belm_list.begin());
    dbsk2d_ishock_belm* right = *(++belm_list.begin());

    dbsk2d_ishock_belm* pair1=
        bp1->getElmToTheRightOf(left);
    dbsk2d_ishock_belm* pair2=
        bp1->getElmToTheLeftOf(right);

    form_contact_shocks(left,pair1,bp1);
    form_contact_shocks(pair2,right,bp1);

    dbsk2d_ishock_belm* left_line=local_belm_list_[0];
    dbsk2d_ishock_belm* right_line=local_belm_list_[1];

    if ( bp2->getElmToTheLeftOf(bl1)->id() == left_line->id() ||
         bp2->getElmToTheLeftOf(bl1)->id() == right_line->id())
    {
        try 
        {
            form_contact_shocks(bp2->getElmToTheLeftOf(bl1),bl1,bp2);
        }
        catch (dbsk2d_exception_topology_error &e)
        {
            return;
        }
    }
    else
    {
        try
        {
            form_contact_shocks(bl1,bp2->getElmToTheRightOf(bl1),bp2);
        }
        catch (dbsk2d_exception_topology_error &e)
        {
            return;
        }
    }
    
    if ( bp2->getElmToTheLeftOf(bl2)->id() == left_line->id() ||
         bp2->getElmToTheLeftOf(bl2)->id() == right_line->id())
    {
        try
        {
            form_contact_shocks(bp2->getElmToTheLeftOf(bl2),bl2,bp2);
        }
        catch (dbsk2d_exception_topology_error &e)
        {
            return;
        }
    }
    else
    {
        try
        {
            form_contact_shocks(bl2,bp2->getElmToTheRightOf(bl2),bp2);
        }
        catch (dbsk2d_exception_topology_error &e)
        {
            return;
        }
    }

    
    interacting_bnd_elements_.erase(bp1->id());
    interacting_bnd_elements_.erase(bp2->id());
 
    if ( bp2->shock_map().size()==0)
    {
        bp2->set_max_eta(2*vnl_math::pi);
        bp2->set_vref(-1);
        bp2->set_visibility(false);
    }
 

    vcl_vector<dbsk2d_ishock_edge*> pt_shocks_to_delete;

    bnd_ishock_map_iter it = bp2->shock_map().begin();
    for ( ; it != bp2->shock_map().end() ; ++it)
    {
        dbsk2d_ishock_elm* selm = it->second;
        
        if ( selm->is_a_link() )
        {
            if ( it->first.s_eta > bp2->max_eta() )
            {
                dbsk2d_ishock_edge* iedge = (dbsk2d_ishock_edge*)(it->second);
                pt_shocks_to_delete.push_back(iedge);
            }
        }

    }

    while ( pt_shocks_to_delete.size())
    {
        dbsk2d_ishock_edge* cur_edge=pt_shocks_to_delete.back();
        pt_shocks_to_delete.pop_back();
        delete_shock_and_update(cur_edge);

    }


}

bool dbsk2d_ishock_gap4_transform::valid_transform()
{
    belm_list belmList;
    gap_line_pair_.first->get_interacting_belements(belmList);

    bool flag=false;
    belm_list::iterator it;
    for ( it = belmList.begin() ; it != belmList.end() ; ++it)
    {
        if ( (*it)->id() == gap_line_pair_.second->id())
        {
            flag=true;
            break;
        }
    }
        
    if ( ! flag )
    {
        return false;
    }

    if ( gap_line_pair_.second->get_contour_id() < 0 )
    {
        return false;
    }

    vil_image_resource_sptr img=dbsk2d_transform_manager::Instance().
        get_image();

    vgl_point_2d<double> pt1=gap_line_pair_.first->pt();
    vgl_point_2d<double> pt2=anchor_pt_->pt();

    bool valid=true;
    if ( pt1.x() < 0 || pt1.y() < 0 ||
         pt1.x() >= img->ni() || pt1.y() >= img->nj() || 
         pt2.x() < 0 || pt2.y() < 0 ||
         pt2.x() >= img->ni() || pt2.y() >= img->nj())
        
    {
        valid=false;
        
    }
    
    
    return valid;

}
