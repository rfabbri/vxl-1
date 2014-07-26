// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_loop_transform.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_ishock_loop_transform.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_graph.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <dbsk2d/dbsk2d_transform_manager.h>
#include <dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h>
// vsol headers
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

//: constructor
//: compute the salency of this shock element (edge/node)
dbsk2d_ishock_loop_transform::dbsk2d_ishock_loop_transform(
    dbsk2d_ishock_graph_sptr intrinsic_shock_graph,
    dbsk2d_ishock_bpoint* contour_point)
    :dbsk2d_ishock_transform(intrinsic_shock_graph,
                             dbsk2d_ishock_transform::LOOP),
     contour_point_(contour_point),
     contour_pair_(contour_point,0),
     valid_transform_(true)
{
    detect_contour();
}

//: Add in execute transform
bool dbsk2d_ishock_loop_transform::execute_transform()
{
    outer_wavefront_.clear();
    shocks_removed_.clear();
    vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it=
        removal_bnd_elements_.begin();

    bool flag=true;
    if ( removal_bnd_elements_.size() ==0 || 
         (*it).second->is_a_GUIelm())
    {
        // vcl_cout<<"Removing contour"<<vcl_endl;
        flag=remove_contour();
    }
    else
    {
        //vcl_cout<<"Reinsert contour"<<vcl_endl;
        flag=reinsert_contour();
    }

    return flag;

}

double dbsk2d_ishock_loop_transform::likelihood()
{

    vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
    vcl_map<int,dbsk2d_ishock_bpoint*> curve_map;
    for ( it = removal_bnd_elements_.begin(); 
          it != removal_bnd_elements_.end(); ++it)
    {
        dbsk2d_ishock_belm* belm = (*it).second;
    
        if ( belm->is_a_line())
        {
            dbsk2d_ishock_bline* bline=dynamic_cast<dbsk2d_ishock_bline*>
                (belm);
                curve_map[bline->s_pt()->id()]=bline->s_pt();
                curve_map[bline->e_pt()->id()]=bline->e_pt();
        }
        else
        {
            dbsk2d_ishock_bpoint* bpoint=
                dynamic_cast<dbsk2d_ishock_bpoint*>(belm);
            curve_map[bpoint->id()]=bpoint;
        } 
    }

    vcl_vector<vgl_point_2d<double> > curve;
    vcl_map<int,dbsk2d_ishock_bpoint*>::iterator mit;
    for ( mit = curve_map.begin() ; mit != curve_map.end() ; ++mit)
    {
        curve.push_back((*mit).second->pt());
    }

    return 
        1.0-dbsk2d_transform_manager::Instance().transform_probability(curve);
}

//: remove boundary element
void dbsk2d_ishock_loop_transform::detect_contour()
{
    ordered_contour_.clear();

    // Determine all parts involved in this contour
    interacting_bnd_elements_[contour_point_->id()]=contour_point_;
    if ( !contour_point_->is_an_end_point())
    {
        higher_degree_nodes_[contour_point_->id()]=contour_point_;
    }
    else
    {
        removal_bnd_elements_[contour_point_->id()]=contour_point_;
    }
    ordered_contour_.push_back(contour_point_);

    // Since this a degree three put something back on right away
    dbsk2d_ishock_belm* first_belm = *(contour_point_->LinkedBElmList.begin());
    removal_bnd_elements_[first_belm->id()]=first_belm;
    dbsk2d_ishock_bline* bline = dynamic_cast<dbsk2d_ishock_bline*>(first_belm);
    removal_bnd_elements_[bline->twinLine()->id()]=bline->twinLine();

    
    vcl_vector<dbsk2d_ishock_bpoint*> stack;
    if ( bline->s_pt()->id()==contour_point_->id())
    {
        stack.push_back(bline->e_pt());
        ordered_contour_.push_back(stack.back());
        removal_bnd_elements_[stack.back()->id()]=stack.back();

    }
    else
    {
        stack.push_back(bline->s_pt());
        ordered_contour_.push_back(stack.back());
        removal_bnd_elements_[stack.back()->id()]=stack.back();
    }

    if ( stack.back()->is_an_end_point() == 1 )
    {
        contour_pair_.second=stack.back();
        stack.pop_back();
    }
    else if ( stack.back()->nLinkedElms()>= 6 )
    {

        interacting_bnd_elements_[stack.back()->id()]=stack.back();
        removal_bnd_elements_.erase(stack.back()->id());
        higher_degree_nodes_[stack.back()->id()]=stack.back();
        contour_pair_.second=stack.back();
        stack.pop_back();
    }

    while ( stack.size())
    {
      // Pop of stack
      dbsk2d_ishock_bpoint* node = stack.back();
      stack.pop_back();

      belm_list::iterator curB = node->LinkedBElmList.begin();
      for(; curB!=node->LinkedBElmList.end(); ++curB) 
      {
          removal_bnd_elements_[(*curB)->id()]=*curB;

          if ( (*curB)->is_a_line())
          {
              dbsk2d_ishock_bline* bline= dynamic_cast<dbsk2d_ishock_bline*>
                  (*curB);

              if ( stack.size()==0)
              {
                  if ( removal_bnd_elements_.count(bline->s_pt()->id())==0 
                       && higher_degree_nodes_.count(bline->s_pt()->id())==0)
                  {
                      removal_bnd_elements_[bline->s_pt()->id()]=bline->s_pt();
                      stack.push_back(bline->s_pt());
                      ordered_contour_.push_back(stack.back());
                  }
                  else if ( removal_bnd_elements_.count(bline->e_pt()->id())==0
                            && 
                            higher_degree_nodes_.count(bline->e_pt()->id())==0)
                  {
                      removal_bnd_elements_[bline->e_pt()->id()]=bline->e_pt();
                      stack.push_back(bline->e_pt());
                      ordered_contour_.push_back(stack.back());
                  }
              }
              
          }
          
      }

      if ( stack.size() == 0 )
      {
          vcl_cout<<"We have reached a loop"<<vcl_endl;
          break;
      }

      if ( stack.back()->is_an_end_point() == 1 )
      {
          contour_pair_.second=stack.back();
          stack.pop_back();
      }
      else if ( stack.back()->nLinkedElms()>= 6 )
      {
          
          interacting_bnd_elements_[stack.back()->id()]=stack.back();
          removal_bnd_elements_.erase(stack.back()->id());
          higher_degree_nodes_[stack.back()->id()]=stack.back();
          contour_pair_.second=stack.back();
          stack.pop_back();
      }
    }

    vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
    for ( it = removal_bnd_elements_.begin(); 
          it != removal_bnd_elements_.end(); ++it)
    {
        dbsk2d_ishock_belm* belm = (*it).second;

        if ( higher_degree_nodes_.size())
        {
            if ( belm->is_a_line())
            {
                dbsk2d_ishock_bline* bline=dynamic_cast<dbsk2d_ishock_bline*>
                    (belm);

                if ( higher_degree_nodes_.count(bline->s_pt()->id())) 
                {
                    dbsk2d_ishock_bpoint* bpoint = dynamic_cast
                         <dbsk2d_ishock_bpoint*>
                         (higher_degree_nodes_[bline->s_pt()->id()]);
                   

                    if ( contact_shock_pairs_.count(bpoint->id())==0 )
                    {
                        dbsk2d_ishock_bline* tline=bline->twinLine();
                        
                        dbsk2d_ishock_belm* left_belm=bpoint
                            ->getElmToTheLeftOf(bline);
                        if ( left_belm->id() == tline->id())
                        {
                            dbsk2d_ishock_belm* pair1=
                                bpoint->getElmToTheLeftOf(tline);
                            dbsk2d_ishock_belm* pair2=
                                bpoint->getElmToTheRightOf(bline);
                            contact_shock_pairs_[bpoint->id()]=
                                vcl_make_pair(pair1,pair2);
                        }
                        else
                        {
                            dbsk2d_ishock_belm* pair1=
                                bpoint->getElmToTheRightOf(tline);
                            dbsk2d_ishock_belm* pair2=
                                bpoint->getElmToTheLeftOf(bline);
                            contact_shock_pairs_[bpoint->id()]=
                                vcl_make_pair(pair1,pair2);

                        }
                        
                    }


                }

                if ( higher_degree_nodes_.count(bline->e_pt()->id()))
                {
                    dbsk2d_ishock_bpoint* bpoint = dynamic_cast
                        <dbsk2d_ishock_bpoint*>
                        (higher_degree_nodes_[bline->e_pt()->id()]);
     
                    if ( contact_shock_pairs_.count(bpoint->id())==0 )
                    {
                        dbsk2d_ishock_bline* tline=bline->twinLine();
                        
                        dbsk2d_ishock_belm* left_belm=bpoint
                            ->getElmToTheLeftOf(bline);
                        if ( left_belm->id() == tline->id())
                        {
                            dbsk2d_ishock_belm* pair1=
                                bpoint->getElmToTheLeftOf(tline);
                            dbsk2d_ishock_belm* pair2=
                                bpoint->getElmToTheRightOf(bline);
                            contact_shock_pairs_[bpoint->id()]=
                                vcl_make_pair(pair1,pair2);
                        }
                        else
                        {
                            dbsk2d_ishock_belm* pair1=
                                bpoint->getElmToTheRightOf(tline);
                            dbsk2d_ishock_belm* pair2=
                                bpoint->getElmToTheLeftOf(bline);
                            contact_shock_pairs_[bpoint->id()]=
                                vcl_make_pair(pair1,pair2);

                        }
                        
                    }



                }
            }
        }

    }
   
    for ( it = removal_bnd_elements_.begin(); 
          it != removal_bnd_elements_.end(); ++it)
    {
        dbsk2d_ishock_belm* belm = (*it).second;
        if ( belm->is_a_line())
        {
            dbsk2d_ishock_bline* bline=(dbsk2d_ishock_bline*)belm;
            dbsk2d_bnd_edge* edge=bline->bnd_edge();
            const vcl_list< vtol_topology_object * > * 
                superiors  = edge->superiors_list();
            vcl_list<vtol_topology_object*>::const_iterator tit;
            for ( tit=(*superiors).begin(); tit!= (*superiors).end(); ++tit)
            {
                if ( (*tit)->get_id() < 0 )
                {
                    valid_transform_=false;
                    break;
                }
            }
            
        }

        if ( !valid_transform_ )
        {
            break;
        }
    }
}

//: remove boundary element
bool dbsk2d_ishock_loop_transform::remove_contour()
{

    vcl_map<unsigned int,bool> local_visibility_map;
    vcl_map<unsigned int, dbsk2d_ishock_belm*>::iterator kit;
    for ( kit = higher_degree_nodes_.begin() ; 
          kit != higher_degree_nodes_.end(); ++kit )
    {
        dbsk2d_ishock_bpoint* bpoint= (dbsk2d_ishock_bpoint*)((*kit).second);
        local_visibility_map[(*kit).first]=bpoint->is_visible();
    }

    vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
    for ( it = removal_bnd_elements_.begin(); 
          it != removal_bnd_elements_.end(); ++it)
    {
        dbsk2d_ishock_belm* belm = (*it).second;
        delete_belm_shocks(belm);

        if ( higher_degree_nodes_.size())
        {
            if ( belm->is_a_line())
            {
                dbsk2d_ishock_bline* bline=dynamic_cast<dbsk2d_ishock_bline*>
                    (belm);

                if ( higher_degree_nodes_.count(bline->s_pt()->id())) 
                {
                    dbsk2d_ishock_bpoint* bpoint = dynamic_cast
                         <dbsk2d_ishock_bpoint*>
                         (higher_degree_nodes_[bline->s_pt()->id()]);

                    bpoint->disconnectFrom(bline);

                }

                if ( higher_degree_nodes_.count(bline->e_pt()->id()))
                {
                    dbsk2d_ishock_bpoint* bpoint = dynamic_cast
                        <dbsk2d_ishock_bpoint*>
                        (higher_degree_nodes_[bline->e_pt()->id()]);
                    
                    bpoint->disconnectFrom(bline);

                }
            }
        }
        belm->set_GUIelm(false);
        boundary_->set_belms_off(belm->id());
    }
  
    for ( it = higher_degree_nodes_.begin(); it != higher_degree_nodes_.end();
          ++it)
    {

        dbsk2d_ishock_belm* belm = (*it).second;
        vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_belm*> pair
            = contact_shock_pairs_[belm->id()];
        vcl_list<dbsk2d_ishock_belm*> interacting_belm_list;
        belm->get_interacting_belements(interacting_belm_list);

        if ( interacting_belm_list.size() == 0 )
        {
            // Look at first pair
            dbsk2d_ishock_belm* bl1 = pair.first;
            dbsk2d_ishock_belm* bl2 = pair.first;

            bnd_ishock_map_iter curS = bl1->shock_map().begin();
            for ( ; curS != bl1->shock_map().end() ; ++curS) 
            {
                  dbsk2d_ishock_elm* selm = curS->second;
                  dbsk2d_ishock_edge* cur_edge = (dbsk2d_ishock_edge*)selm; 
                  if ( removal_bnd_elements_.count(cur_edge->lBElement()->id())
                       ||
                       removal_bnd_elements_.count(cur_edge->rBElement()->id())
                      )
                  {
                      delete_shock_and_update(cur_edge);
                  }

            }

            curS = bl2->shock_map().begin();
            for ( ; curS != bl2->shock_map().end() ; ++curS) 
            {
                  dbsk2d_ishock_elm* selm = curS->second;
                  dbsk2d_ishock_edge* cur_edge = (dbsk2d_ishock_edge*)selm; 
                  if ( removal_bnd_elements_.count(cur_edge->lBElement()->id())
                       ||
                       removal_bnd_elements_.count(cur_edge->rBElement()->id())
                      )
                  {
                      delete_shock_and_update(cur_edge);
                  }

            }


            interacting_belm_list.push_back(bl1);
            interacting_belm_list.push_back(bl2);
            
        }
        bool flag=false;
        vcl_list<dbsk2d_ishock_belm*>::iterator bit;
        for ( bit = interacting_belm_list.begin() ; bit 
                  != interacting_belm_list.end() ; ++bit)
        {
            if ( (*bit)->id() == pair.first->id() ||
                 (*bit)->id() == pair.second->id() )
            {
                flag=true;
                break;
            }

        }
        
        if ( flag)
        {
            delete_belm_shocks(belm);
        }
    }

    if ( minimal_interacting_elements_.size() == 0 )
    {
        vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator sit;
        for ( sit=interacting_bnd_elements_.begin(); sit !=
                  interacting_bnd_elements_.end() ; ++sit)
        {
            if ( (*sit).second->is_a_line())
            {
                
                minimal_interacting_elements_.insert(
                    (*sit).second->get_contour_id());
            }
            min_local_context_[(*sit).first]=(*sit).second;
        }
    }

    delete_shock_vertices();

    
    vcl_map<unsigned int,vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_belm*> >
        ::iterator cit;
    for ( cit = contact_shock_pairs_.begin(); cit != contact_shock_pairs_.end();
          ++cit)
    {
        vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_belm*> pair
            = (*cit).second;
        unsigned int id=(*cit).first;
        dbsk2d_ishock_bline* bl1(0);
        dbsk2d_ishock_bline* bl2(0);
        dbsk2d_ishock_belm* belm1=const_cast<dbsk2d_ishock_belm*>(pair.first);
        dbsk2d_ishock_belm* belm2=const_cast<dbsk2d_ishock_belm*>(pair.second);

        if ( belm1->is_a_line() )
        {
            bl1=(dbsk2d_ishock_bline*)(belm1);
        }
        
        if ( belm2->is_a_line() )
        {
            bl2=(dbsk2d_ishock_bline*)(belm2);
        }

        dbsk2d_ishock_bpoint* bpoint = (bl1->s_pt()->id() ==id )
            ? bl1->s_pt() : bl1->e_pt();
        form_contact_shocks(belm1,belm2,bpoint);
    }

    // 4 Kick of shock
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
                vcl_cerr<<"Error: Reinsert Contour"<<vcl_endl;
        
                // 2. Reactivate contour

                vcl_vector<dbsk2d_ishock_belm*> contact_shock_set;
                for ( it = removal_bnd_elements_.begin(); 
                      it != removal_bnd_elements_.end() ; ++it)
                {
                    (*it).second->set_GUIelm(true);
                    boundary_->set_belms_on((*it).second->id());
                    
                    if ( (*it).second->is_a_point())
                    {
                        dbsk2d_ishock_bpoint* bpoint = 
                            (dbsk2d_ishock_bpoint*)((*it).second);

                        bpoint->set_max_eta(2.0*vnl_math::pi);
                        bpoint->set_vref(-1);

                    }

                    if ( (*it).second->is_a_line())
                    {
                        dbsk2d_ishock_bline* bline = 
                            (dbsk2d_ishock_bline*)((*it).second);
                        dbsk2d_ishock_belm* bpoint=0;
                        if ( higher_degree_nodes_.count(bline->s_pt()->id()))
                        {
                            bpoint=higher_degree_nodes_[bline->s_pt()->id()];
                        }
                        else if ( higher_degree_nodes_.count(
                                      bline->e_pt()->id()))
                        {
                            bpoint=higher_degree_nodes_[bline->e_pt()->id()];
                        }

                        if ( bpoint)
                        {
                            dbsk2d_ishock_bpoint* degree_three=
                                (dbsk2d_ishock_bpoint*)bpoint;
                            degree_three->connectTo((*it).second);
                            bool flag=local_visibility_map
                                [degree_three->id()];
                            degree_three->set_visibility(flag);
                            degree_three->set_max_eta(2.0*vnl_math::pi);
                            degree_three->set_vref(-1);
                            

                        }
                    }

                }
                
                return false;
            }

            vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
            for ( it = deleted_bnd_elements.begin();
                  it != deleted_bnd_elements.end();
                  ++it)
            {
                interacting_bnd_elements_[(*it).first]=(*it).second;
            }

            for ( unsigned int i=0; i < invalid_shocks.size() ; ++i)
            {
                dbsk2d_ishock_edge* edge=invalid_shocks[i];
                dbsk2d_ishock_belm* left_belm=edge->lBElement();
                dbsk2d_ishock_belm* right_belm=edge->rBElement();
                interacting_bnd_elements_[left_belm->id()]=left_belm;
                interacting_bnd_elements_[right_belm->id()]=right_belm;
                edge->reset_shock();
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



//: remove formed shocks
bool dbsk2d_ishock_loop_transform::reinsert_contour()
{

    // 1) Delete all shocks formed by interacting bnds
    vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
    for ( it = interacting_bnd_elements_.begin(); 
          it != interacting_bnd_elements_.end() ; ++it)
    {

        if ( !min_local_context_.count((*it).first) )
        {
            continue;
        }

        bnd_ishock_map shock_map=(*it).second->shock_map();
        bnd_ishock_map_iter curS = shock_map.begin();
        for (; curS!=shock_map.end(); ++curS)
        {
            dbsk2d_ishock_belm* lbe=(*curS).second->lBElement();
            dbsk2d_ishock_belm* rbe=(*curS).second->rBElement();
            
            dbsk2d_ishock_bline* left_bl=(lbe->is_a_line())
                ?(dbsk2d_ishock_bline*) lbe: 0;
            dbsk2d_ishock_bline* right_bl=(rbe->is_a_line())
                ?(dbsk2d_ishock_bline*) rbe: 0;

            if ( interacting_bnd_elements_.count(lbe->id()) ||
                 interacting_bnd_elements_.count(rbe->id()))
            {
                if (!((*curS).second->is_a_contact()))
                {
                    if ( left_bl && right_bl )
                    {
                        unsigned int left_start_id=left_bl->s_pt()->id();
                        unsigned int left_end_id  =left_bl->e_pt()->id();
                    
                        unsigned int right_start_id=right_bl->s_pt()->id();
                        unsigned int right_end_id  =right_bl->e_pt()->id();
                    
                        if ( left_start_id==right_start_id 
                             ||
                             left_start_id==right_end_id
                             ||
                             left_end_id==right_start_id
                             ||
                             left_end_id==right_end_id)
                        {
                            continue;
                        }
                    }
                    delete_shock_and_update((*curS).second);
                }
            }
            
        }
    }

    // 3 See if any contact shocks need to be deleted
    vcl_map<unsigned int,vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_belm*> >
        ::iterator cit;
    for ( cit = contact_shock_pairs_.begin(); cit != contact_shock_pairs_.end();
          ++cit)
    {
        vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_belm*> pair
            = (*cit).second;

        dbsk2d_ishock_belm* lbe=pair.first;
        dbsk2d_ishock_belm* rbe=pair.second;
        
        bnd_ishock_map shock_map=lbe->shock_map();
        bnd_ishock_map_iter curS = shock_map.begin();
        for (; curS!=shock_map.end(); ++curS)
        {
            
            dbsk2d_ishock_belm* elm_left=(*curS).second->lBElement();
            dbsk2d_ishock_belm* elm_right=(*curS).second->rBElement();

            if ( elm_left->id() == rbe->id() ||
                 elm_right->id() == rbe->id() )
            {
                delete_shock_and_update((*curS).second);
            }

            if ( higher_degree_nodes_.count(elm_left->id()) ||
                 higher_degree_nodes_.count(elm_right->id()))
            {
                delete_shock_and_update((*curS).second);
            }

        }


        shock_map=rbe->shock_map();
        curS = shock_map.begin();
        for (; curS!=shock_map.end(); ++curS)
        {
            
            dbsk2d_ishock_belm* elm_left=(*curS).second->lBElement();
            dbsk2d_ishock_belm* elm_right=(*curS).second->rBElement();

            if ( higher_degree_nodes_.count(elm_left->id()) ||
                 higher_degree_nodes_.count(elm_right->id()))
            {
                delete_shock_and_update((*curS).second);
            }

        }

    }
    
    // 2. Delete all vertices
    delete_shock_vertices();

    // 2. Reactivate contour
    vcl_vector<vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_bpoint*> > 
        junction_contacts;
    vcl_vector<dbsk2d_ishock_belm*> contact_shock_set;
    for ( it = removal_bnd_elements_.begin(); 
          it != removal_bnd_elements_.end() ; ++it)
    {
        (*it).second->set_GUIelm(true);
        boundary_->set_belms_on((*it).second->id());

        if ( (*it).second->is_a_line())
        {
            contact_shock_set.push_back((*it).second);
        }
        interacting_bnd_elements_[(*it).first]=(*it).second;

        if ( (*it).second->is_a_line())
        {
            dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)((*it).second);
            dbsk2d_ishock_belm* bpoint=0;
            if ( higher_degree_nodes_.count(bline->s_pt()->id()))
            {
                bpoint=higher_degree_nodes_[bline->s_pt()->id()];
            }
            else if ( higher_degree_nodes_.count(bline->e_pt()->id()))
            {
                bpoint=higher_degree_nodes_[bline->e_pt()->id()];
            }

            if ( bpoint)
            {
                dbsk2d_ishock_bpoint* degree_three=
                    (dbsk2d_ishock_bpoint*)bpoint;
                degree_three->connectTo((*it).second);
                vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_bpoint*>
                    pair(bline,degree_three);
                junction_contacts.push_back(pair);

            }
        }
    }
    
    // 3. Recreate contact shocks from by inserting shock
    if ( contact_shock_set.size())
    {
        ishock_detector_.initialize_contacts_and_A3s(contact_shock_set);
    }

    vcl_vector<vcl_pair<dbsk2d_ishock_belm*,dbsk2d_ishock_bpoint*> > 
        ::iterator bit;
    for ( bit=junction_contacts.begin(); bit != junction_contacts.end();
          ++bit)
    {
        dbsk2d_ishock_bline* bl1=(dbsk2d_ishock_bline*)(*bit).first;
        dbsk2d_ishock_bpoint* bp1=(*bit).second;
        
        dbsk2d_ishock_belm* opposite_line= 
            (bp1->getElmToTheLeftOf(bl1)->id() == bl1->twinLine()->id())
            ?bp1->getElmToTheRightOf(bl1):bp1->getElmToTheLeftOf(bl1);

        //traverse the bnd_ishock_map and record all the elements 
        bnd_ishock_map_iter curS = bl1->shock_map().begin();
        
        bool flag=true;
        for (; curS!=bl1->shock_map().end(); ++curS)
        {

            dbsk2d_ishock_belm* elm_left=(*curS).second->lBElement();
            dbsk2d_ishock_belm* elm_right=(*curS).second->rBElement();
            
            if ( elm_left->id()  == opposite_line->id() ||
                 elm_right->id() == opposite_line->id() ||
                 elm_left->id() == bp1->id() ||
                 elm_right->id() == bp1->id() )
            {
                flag=false;
                break;

            }
        }

        if ( flag )
        {
            form_contact_shocks(bl1,opposite_line,bp1);
            if ( bp1->shock_map().size() == 0)
            {
                bp1->set_visibility(false);
                bp1->set_max_eta(2*vnl_math::pi);
                bp1->set_vref(-1);

            }
            else
            {
                bp1->set_visibility(true);
                interacting_bnd_elements_[bp1->id()]=bp1;
            }
        }
    }

    // 4. Kick of shock
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
                vcl_cerr<<"Error: recomputing loop reinsert"<<vcl_endl;
                
                vcl_vector<dbsk2d_ishock_belm*> contact_shock_set;
                for ( it = removal_bnd_elements_.begin(); 
                      it != removal_bnd_elements_.end() ; ++it)
                {
                    (*it).second->set_GUIelm(true);
                    boundary_->set_belms_on((*it).second->id());
                    
                    if ( (*it).second->is_a_point())
                    {
                        dbsk2d_ishock_bpoint* bpoint = 
                            (dbsk2d_ishock_bpoint*)((*it).second);

                        bpoint->set_max_eta(2.0*vnl_math::pi);
                        bpoint->set_vref(-1);

                    }

                }
                
                vcl_map<unsigned int, dbsk2d_ishock_belm*>::iterator kit;
                for ( kit = higher_degree_nodes_.begin() ; 
                      kit != higher_degree_nodes_.end(); ++kit )
                {
                    dbsk2d_ishock_bpoint* bpoint= (dbsk2d_ishock_bpoint*)
                        ((*kit).second);
                   
                    bpoint->set_max_eta(2.0*vnl_math::pi);
                    bpoint->set_vref(-1);

                }

                return false;
            }
            vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
            for ( it = deleted_bnd_elements.begin();
                  it != deleted_bnd_elements.end();
                  ++it)
            {
                interacting_bnd_elements_[(*it).first]=(*it).second;
            }

            for ( unsigned int i=0; i < invalid_shocks.size() ; ++i)
            {
                dbsk2d_ishock_edge* edge=invalid_shocks[i];
                dbsk2d_ishock_belm* left_belm=edge->lBElement();
                dbsk2d_ishock_belm* right_belm=edge->rBElement();
                interacting_bnd_elements_[left_belm->id()]=left_belm;
                interacting_bnd_elements_[right_belm->id()]=right_belm;
                edge->reset_shock();
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
