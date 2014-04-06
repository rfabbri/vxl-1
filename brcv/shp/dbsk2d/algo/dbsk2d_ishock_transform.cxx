// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_transform.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_ishock_transform.h>
#include <dbsk2d/dbsk2d_file_io.h>
// vsol headers
#include <vsol/vsol_line_2d.h>
// vgl headers
#include <vgl/vgl_distance.h>
// vul headers
#include <vul/vul_psfile.h>
// vgl polygon
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
// vnl random
#include <vnl/vnl_random.h>

//: constructor
//: base class for shock transforms
dbsk2d_ishock_transform::dbsk2d_ishock_transform(
    dbsk2d_ishock_graph_sptr intrinsic_shock_graph,
    TransformType transform_type)
    :transform_type_(transform_type),
     ishock_graph_(intrinsic_shock_graph),
     boundary_(intrinsic_shock_graph->boundary()),
     ishock_detector_(boundary_)
{
    ishock_detector_.set_bnd_cell(boundary_->cell(0,0));
    if ( transform_type_ == GAP )
    {
        ishock_detector_.set_local_shock(true);
    }
}

//: delete all shocks of a boundary element
void dbsk2d_ishock_transform::delete_belm_shocks(
    dbsk2d_ishock_belm* belm)
{

    while ( belm->shock_map().size() > 0 )
    {
        bnd_ishock_map_iter curS = belm->shock_map().begin();
        dbsk2d_ishock_elm* selm = curS->second;
        dbsk2d_ishock_edge* cur_edge = (dbsk2d_ishock_edge*)selm; 
        delete_shock_and_update(cur_edge);       
    }



}

//: delete an ishock edge
void dbsk2d_ishock_transform::delete_shock_and_update(
    dbsk2d_ishock_edge* cur_edge)
{

    //First, update all the connectivity information 
    if (cur_edge->lNeighbor())
    {
        cur_edge->lNeighbor()->clear_rNeighbor();        
    }

    if (cur_edge->rNeighbor())
    {
        cur_edge->rNeighbor()->clear_lNeighbor();
    }

    if (cur_edge->pSNode())
    {
        if (cur_edge->pSNode()->cShock() == cur_edge)
        {
            cur_edge->pSNode()->clear_cShock();
        }
        else
        {
            cur_edge->pSNode()->clear_cShock2();
        }
    
        outer_wavefront_[cur_edge->pSNode()->id()]=
            cur_edge->pSNode();
    }

    if ( cur_edge->cSNode())
    {
        cur_edge->cSNode()->remove_pShock(cur_edge);                
        outer_wavefront_[cur_edge->cSNode()->id()]=
            cur_edge->cSNode();
    }
 
    if ( removal_bnd_elements_.count(cur_edge->lBElement()->id())==0 )
    {

        interacting_bnd_elements_
            [cur_edge->lBElement()->id()]=cur_edge->lBElement();

    }

    if ( removal_bnd_elements_.count(cur_edge->rBElement()->id())==0)
    {
        interacting_bnd_elements_[
            cur_edge->rBElement()->id()]=cur_edge->rBElement();

    }

    shocks_removed_[cur_edge->id()]="temp";
    ishock_detector_.clear_active_shock(cur_edge);
    ishock_graph_->remove_edge(cur_edge);
            
    
}


//: delete an ishock edge
void dbsk2d_ishock_transform::delete_shock(dbsk2d_ishock_edge* cur_edge)
{

    //First, update all the connectivity information 
    if (cur_edge->lNeighbor())
    {
        cur_edge->lNeighbor()->clear_rNeighbor();        
    }

    if (cur_edge->rNeighbor())
    {
        cur_edge->rNeighbor()->clear_lNeighbor();
    }

    if (cur_edge->pSNode())
    {
        if (cur_edge->pSNode()->cShock() == cur_edge)
        {
            cur_edge->pSNode()->clear_cShock();
        }
        else
        {
            cur_edge->pSNode()->clear_cShock2();
        }
    
        outer_wavefront_[cur_edge->pSNode()->id()]=
            cur_edge->pSNode();
    }

    if ( cur_edge->cSNode())
    {
        cur_edge->cSNode()->remove_pShock(cur_edge);                
      
    }

    ishock_detector_.clear_active_shock(cur_edge);
    ishock_graph_->remove_edge(cur_edge);
    if ( cur_edge->pSNode())
    {
        ishock_graph_->remove_vertex(cur_edge->pSNode());
    }
}

//: local_shock_compute
void dbsk2d_ishock_transform::local_shock_compute()
{
    ishock_detector_.compile_the_active_selm_list();

    vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator bit;
    for ( bit = interacting_bnd_elements_.begin(); 
          bit != interacting_bnd_elements_.end(); ++bit)
    {

        vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator dit=bit;
        ++dit;

        for ( ; dit != interacting_bnd_elements_.end(); ++dit)
        {
       
            ishock_detector_.init_cand_src_between((*bit).second,
                                                  (*dit).second);

        }
            

    }

    ishock_detector_.propagate_shocks();

}

//: Recompute full shock graph 
void dbsk2d_ishock_transform::recompute_full_shock_graph()
{
    
    bool add_noise=false;
    bool valid_shock_computation=false;

    do
    {
        ishock_detector_.clear();
        this->clear();

        vcl_vector<dbsk2d_ishock_belm*> belm_list=boundary_->belm_list();

        vnl_random mz_random;
        mz_random.reseed((unsigned long)time(NULL));
        float noise_radius=0.002f;

        vcl_vector<dbsk2d_ishock_belm*>::iterator bit;
        for ( bit = belm_list.begin(); bit != belm_list.end(); ++bit)
        {
            if ( (*bit)->is_a_point())
            {

                dbsk2d_ishock_bpoint* bpoint=(dbsk2d_ishock_bpoint*)(*bit);

                bpoint->set_max_eta(2*vnl_math::pi);
                bpoint->set_vref(-1);
            
                if ( add_noise )
                {
                    vgl_point_2d<double> point=bpoint->pt();
                    double x=point.x();
                    double y=point.y();
                    double rand_x = mz_random.drand32(1.0);
                    x += 2.0*noise_radius*(rand_x-0.5);
                    double rand_y = mz_random.drand32(1.0);
                    y += 2.0*noise_radius*(rand_y-0.5);
                    bpoint->set_pt(x,y);
                }
            }
        }

        ishock_detector_.initialize_contacts_and_A3s_recompute();

        vcl_vector< vcl_vector<dbsk2d_ishock_belm*> > gaps=
            boundary_->gaps();

        for ( unsigned int i=0; i < gaps.size() ; ++i)
        {
            vcl_vector< dbsk2d_ishock_belm*>  euler_spiral = gaps[i];
            if (euler_spiral[0]->is_a_GUIelm())
            {
                ishock_detector_.initialize_contacts_and_A3s(euler_spiral);
            }
        }  

        for ( bit = belm_list.begin(); bit != belm_list.end(); ++bit)
        {
            if ( (*bit)->is_a_GUIelm() )
            {
                vcl_vector<dbsk2d_ishock_belm*>::iterator dit=bit;
                ++dit;

                for ( ; dit != belm_list.end(); ++dit)
                {
                    if ( (*dit)->is_a_GUIelm() )
                    {
                        ishock_detector_.init_cand_src_between(*bit,*dit);
                    }
                
                }
            
            }
        }

        ishock_detector_.propagate_shocks();
        ishock_graph_->update_shocks();
        valid_shock_computation=ishock_graph_->valid_shock_graph();

        if ( !valid_shock_computation )
        {
            add_noise=true;
            vcl_cout<<"Rerun with Noise"<<vcl_endl;
        }
        else
        {
            vcl_cout<<"Valid shock graph: "<<valid_shock_computation<<vcl_endl;
        }

    }while( valid_shock_computation == false);

}

//: Delete shock vertices
void dbsk2d_ishock_transform::delete_shock_vertices()
{

    vcl_map<unsigned int,dbsk2d_ishock_node*>::iterator it;
    while ( outer_wavefront_.size() > 0 )
    {
        it = outer_wavefront_.begin();
        ishock_edge_list pshocks = (*it).second->pShocks();
        dbsk2d_ishock_edge* cshock = (*it).second->cShock();
        dbsk2d_ishock_edge* cshock2 = (*it).second->cShock2();

        //remove this edge from the nodes' parent list
        ishock_edge_list::iterator curS = pshocks.begin();
        for(; curS!=pshocks.end(); ++curS)
        {
            dbsk2d_ishock_edge* shock = (*curS);
              
            shock->reset_shock();
            shock->setId(ishock_graph_->nextAvailableID());

            if ( shock->lShock())
            {
                if ( shocks_removed_.count(shock->lShock()->id()))
                {
                    shock->set_lShock(NULL);
                }
            }

            if ( shock->rShock())
            {
                if ( shocks_removed_.count(shock->rShock()->id()))
                {
                    shock->set_rShock(NULL);
                }
            }

            if ( interacting_bnd_elements_.count(shock->lBElement()->id()))
            {
                interacting_bnd_elements_[shock->rBElement()->id()]=
                    shock->rBElement();
            }
           
            if ( interacting_bnd_elements_.count(shock->rBElement()->id()))
            {
                interacting_bnd_elements_[shock->lBElement()->id()]=
                    shock->lBElement();
            }     
            
        }

        if ( cshock )
        {
            cshock->set_pSNode(NULL);
            if ( cshock->cSNode() )
            {
                outer_wavefront_[cshock->cSNode()->id()]=cshock->cSNode();
            }
            delete_shock_and_update(cshock);

        }

        if ( cshock2 )
        {
            cshock2->set_pSNode(NULL);
            if ( cshock2->cSNode() )
            {
                outer_wavefront_[cshock2->cSNode()->id()]=cshock2->cSNode();
            }
            delete_shock_and_update(cshock2);

        }

        ishock_graph_->remove_vertex((*it).second);
        outer_wavefront_.erase(it);
     
    }
}

void dbsk2d_ishock_transform::write_boundary(vcl_string filename)
{

    // Grap all boundary elements
    vcl_vector<dbsk2d_ishock_belm* > belm_list = boundary_->belm_list();
    
    vcl_map<unsigned int,vcl_string> lines_visited;
    vcl_vector<vsol_spatial_object_2d_sptr> line_objects;
    for ( unsigned int i=0; i < belm_list.size() ; ++i)
    {
        if ( belm_list[i]->is_a_line() )
        {
            dbsk2d_ishock_bline* line_element = 
                dynamic_cast<dbsk2d_ishock_bline*>(belm_list[i]);
            unsigned int line_id      = line_element->id();
            unsigned int twin_line_id = line_element->twinLine()->id();

            if ( lines_visited.count(line_id)==0 &&
                 lines_visited.count(twin_line_id)==0 && 
                 line_element->is_a_GUIelm())
            {
                // Add in contours for front 
                vsol_spatial_object_2d_sptr obj=
                    new vsol_line_2d(line_element->s_pt()->pt(),
                                     line_element->e_pt()->pt());
                line_objects.push_back(obj);
                lines_visited[line_id]="temp";
                lines_visited[twin_line_id]="temp";
            }

        }
    }

    dbsk2d_file_io::save_bnd_v3_0(filename,line_objects);
}

void dbsk2d_ishock_transform::write_state(
    vcl_string filename,
    vcl_vector<vgl_polygon<double> >& polys)
{

    // create a ps file object
    vul_psfile psfile(filename.c_str(), false);

    psfile.set_line_width(1.0f);
    psfile.set_fg_color(1.0f,0.0f,0.0f);
    psfile.set_bg_color(1.0f,1.0f,1.0f);

    vnl_random rand_object;
    for ( unsigned int i=0; i < polys.size() ; ++i)
    {

        vgl_polygon<double> vgl_poly=polys[i];
     
        psfile.set_fg_color(rand_object.drand64(0.0,1.0),
                            rand_object.drand64(0.0,1.0),
                            rand_object.drand64(0.0,1.0));

        // do not include boundary
        vgl_polygon_scan_iterator<double> psi(vgl_poly, false);  
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x) 
            {

                if (x < 0 || y < 0)
                {
                    continue;
                }
                psfile.point(x,y,1.0f);
            }
        }

    }

    // Grap all boundary elements
    vcl_vector<dbsk2d_ishock_belm* > belm_list = boundary_->belm_list();
    
    vcl_map<unsigned int,vcl_string> lines_visited;
    vcl_vector<vsol_spatial_object_2d_sptr> line_objects;
    for ( unsigned int i=0; i < belm_list.size() ; ++i)
    {
        if ( belm_list[i]->is_a_line() )
        {
            dbsk2d_ishock_bline* line_element = 
                dynamic_cast<dbsk2d_ishock_bline*>(belm_list[i]);
            int line_id      = line_element->id();
            int twin_line_id = line_element->twinLine()->id();

            if ( lines_visited.count(line_id)==0 &&
                 lines_visited.count(twin_line_id)==0 && 
                 line_element->is_a_GUIelm())
            {
                if ( line_element->get_contour_id() < 0 )
                {
                    psfile.set_fg_color(0.0f,0.0f,1.0f);
                }
                else
                {
                    psfile.set_fg_color(1.0f,0.0f,0.0f);
                }

                psfile.line(
                    line_element->s_pt()->pt().x(),
                    line_element->s_pt()->pt().y(),
                    line_element->e_pt()->pt().x(),
                    line_element->e_pt()->pt().y());
                
            }

        }
    }

    psfile.set_fg_color(0.0f,1.0f,0.0f);

    //draw the edges first
    for ( dbsk2d_ishock_graph::edge_iterator curE = 
              ishock_graph_->all_edges().begin();
          curE != ishock_graph_->all_edges().end();
          curE++ ) 
    {
        dbsk2d_ishock_edge* selm = (*curE);
        vcl_vector<vgl_point_2d<double> > ex_pts= selm->ex_pts();

        if ( selm->is_a_contact() )
        {
            continue;
        }

        for ( unsigned int i=0; i < (ex_pts.size()-1) ; ++i)
        {
            vgl_point_2d<double> s_pt=ex_pts[i];
            vgl_point_2d<double> e_pt=ex_pts[i+1];
            
            psfile.line(
                s_pt.x(),
                s_pt.y(),
                e_pt.x(),
                e_pt.y());
            
        }
    }
}


// Determine if contact shocks needed
void dbsk2d_ishock_transform::form_contact_shocks(
    dbsk2d_ishock_belm* belm1,
    dbsk2d_ishock_belm* belm2,
    dbsk2d_ishock_bpoint* bp1)
{   
 
    dbsk2d_ishock_bline* bl1(0);
    dbsk2d_ishock_bline* bl2(0);
           
    if ( bp1->id() == belm1->s_pt()->id())
    {
        bl1=(dbsk2d_ishock_bline*)(belm2);
        bl2=(dbsk2d_ishock_bline*)(belm1);
    }
    
    if ( bp1->id() == belm1->e_pt()->id())
    {
        bl1=(dbsk2d_ishock_bline*)(belm1);
        bl2=(dbsk2d_ishock_bline*)(belm2);

    }

    //get the tangents of the elements at this junction
    double left_tangent, right_tangent;
    left_tangent = bl1->u();
    right_tangent = bl2->u();

    //decide whether a contact or A3 needs to be created at this junction
    double theta = CCW(left_tangent, right_tangent);
    if (_isEqAngle(theta, 0, CONTACT_EPSILON))
    {
        bl1->e_pt()->set_max_eta(2*vnl_math::pi);
        bl1->e_pt()->set_vref(-1);

        dbsk2d_ishock_contact* shock = ishock_detector_.
            form_a_contact_shock(bl1,bl2);

        if ( interacting_bnd_elements_.count(shock->lBElement()->id()))
        {
            interacting_bnd_elements_[shock->rBElement()->id()]=
                shock->rBElement();
        }
           
        if ( interacting_bnd_elements_.count(shock->rBElement()->id()))
        {
            interacting_bnd_elements_[shock->lBElement()->id()]=
                shock->lBElement();
        }   
    }
    else if (AisL(theta,vnl_math::pi))
    {
        dbsk2d_ishock_node* node = ishock_detector_.form_a_corner_a3(bl1,bl2);
        dbsk2d_ishock_edge* shock = node->cShock();

        if ( interacting_bnd_elements_.count(shock->lBElement()->id()))
        {
            interacting_bnd_elements_[shock->rBElement()->id()]=
                shock->rBElement();
        }
           
        if ( interacting_bnd_elements_.count(shock->rBElement()->id()))
        {
            interacting_bnd_elements_[shock->lBElement()->id()]=
                shock->lBElement();
        }  

    }
    else
    {
        bl1->e_pt()->set_max_eta(2*vnl_math::pi);
        bl1->e_pt()->set_vref(-1);

        //form contact shocks for both curves
        dbsk2d_ishock_contact* rcontact = ishock_detector_.
            form_a_contact_shock(
            bl1, bl1->e_pt());
        dbsk2d_ishock_contact* lcontact = ishock_detector_.
            form_a_contact_shock(
            bl2->s_pt(), bl2);

        if (lcontact){
            //Need to actively set the max_eta (wavefront limit) 
            //for the visible portion of the endpoint
            bl2->s_pt()->set_max_eta(lcontact->LsEta());
        }

        if (lcontact)
        {
            if ( interacting_bnd_elements_.count(lcontact->lBElement()->id()))
            {
                interacting_bnd_elements_[lcontact->rBElement()->id()]=
                    lcontact->rBElement();
            }
           
            if ( interacting_bnd_elements_.count(lcontact->rBElement()->id()))
            {
                interacting_bnd_elements_[lcontact->lBElement()->id()]=
                    lcontact->lBElement();
            }
        }

        if ( rcontact )
        {
            if ( interacting_bnd_elements_.count(rcontact->lBElement()->id()))
            {
                interacting_bnd_elements_[rcontact->rBElement()->id()]=
                    rcontact->rBElement();
            }
           
            if ( interacting_bnd_elements_.count(rcontact->rBElement()->id()))
            {
                interacting_bnd_elements_[rcontact->lBElement()->id()]=
                    rcontact->lBElement();
            }
        }
        bl1->e_pt()->set_visibility(true);
    }
}

double dbsk2d_ishock_transform::distance_from_ess(
    vcl_vector<dbsk2d_ishock_belm*>& belm_list,
    vgl_point_2d<double> test_point)
{

    vcl_vector<double> distances;

    // Grab distance to poly line
    // We have to also account for distances to the individual points
    for ( unsigned int v=0; v < belm_list.size()  ; ++v )
    {
        vgl_point_2d<double> p0=belm_list[v]->start();
        vgl_point_2d<double> p1=belm_list[v]->end();
        
        vgl_line_segment_2d<double> line_seg =
            vgl_line_segment_2d<double>(p0,
                                        p1);

        distances.push_back(vgl_distance(line_seg,test_point));
    }

    return *vcl_min_element(distances.begin(),distances.end());


}
