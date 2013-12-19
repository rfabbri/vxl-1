// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_gap_detector.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_ishock_gap_detector.h>
#include <dbsk2d/dbsk2d_ishock_graph.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>

//: constructor
//: compute the salency of this shock element (edge/node)
dbsk2d_ishock_gap_detector::dbsk2d_ishock_gap_detector(
    dbsk2d_ishock_graph_sptr intrinsic_shock_graph)
    :ishock_graph_(intrinsic_shock_graph),
     boundary_(intrinsic_shock_graph->boundary())
{

}

//: Detect all gap 1 for whole ishock graph
void dbsk2d_ishock_gap_detector::detect_gap1(
    vcl_vector<vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >& 
    gap_pairs)
{

    vcl_map< vcl_pair<int,int>,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> > gaps_visited;
        
    vcl_vector<dbsk2d_ishock_belm*> belm_list = boundary_->belm_list();
    for (unsigned int i=0;i < belm_list.size() ; ++i)
    {
        if ( belm_list[i]->is_a_point() )
        {
            dbsk2d_ishock_bpoint* bpoint = 
                dynamic_cast<dbsk2d_ishock_bpoint*>
                (belm_list[i]);
            
            if ( bpoint->is_an_end_point() && bpoint->is_a_GUIelm())
            {
                bool flag=false;
                gap_endpoint(bpoint,gaps_visited,flag);
            }

        }
    }


    vcl_map< vcl_pair<int,int>,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >::iterator it;
    it = gaps_visited.begin();
    for ( ; it != gaps_visited.end() ; ++it)
    {
        gap_pairs.push_back((*it).second);
    }


}

//: Detect all gap 1 for just particular endpoint
void dbsk2d_ishock_gap_detector::detect_gap1(dbsk2d_ishock_belm* belm,
vcl_vector<vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >& 
        gap_pairs)
{
    vcl_map<unsigned int,dbsk2d_ishock_bpoint*> endpoints;
    dbsk2d_ishock_bpoint* bp1 = (dbsk2d_ishock_bpoint*)belm;
    bnd_ishock_map_iter curS = belm->shock_map().begin();
    for ( ; curS != belm->shock_map().end() ; ++curS)
    {
        dbsk2d_ishock_elm* selm = curS->second;

        if ( selm->is_a_link())
        {
            dbsk2d_ishock_edge* iedge = (dbsk2d_ishock_edge*)(curS->second);
            dbsk2d_ishock_belm* lbe = iedge->lBElement();
            if ( lbe->id() == belm->id())
            {
                dbsk2d_ishock_belm* other_belm = iedge->rBElement();
                if ( other_belm->is_a_point())
                {
                    dbsk2d_ishock_bpoint* other_bpoint =
                        (dbsk2d_ishock_bpoint*)(other_belm);
                    if ( other_bpoint->is_an_end_point() && 
                         endpoints.count(other_bpoint->id())==0)
                    {
                        gap_pairs.push_back(vcl_make_pair(bp1,other_bpoint));
                        endpoints[other_bpoint->id()]=other_bpoint;

                    }
                }
            }
            else 
            {
                dbsk2d_ishock_belm* other_belm = iedge->lBElement();
                if ( other_belm->is_a_point())
                {
                    dbsk2d_ishock_bpoint* other_bpoint =
                        (dbsk2d_ishock_bpoint*)(other_belm);
                    if ( other_bpoint->is_an_end_point() &&
                         endpoints.count(other_bpoint->id()) ==0 )
                    {
                        gap_pairs.push_back(vcl_make_pair(bp1,other_bpoint));
                        endpoints[other_bpoint->id()]=other_bpoint;
                     
                    }
                }

            }
                

        }
    }


}

//: Detect all gap 1 for a bunch of regions
void dbsk2d_ishock_gap_detector::detect_gap1(
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >& region_outer_nodes,
    vcl_vector<
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >&
    gap_pairs)
{

    // Detect all gaps for a bunch of regions

    vcl_map< vcl_pair<int,int>,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> > gaps_visited;

    // Loop thru new regions and determine new transforms
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >::iterator it;

    for ( it = region_outer_nodes.begin() ; it != region_outer_nodes.end(); 
          ++it)
    {
        // Detect transforms
        vcl_vector<dbsk2d_ishock_node*> outer_shock_nodes = (*it).second;
        for ( unsigned int i=0; i < outer_shock_nodes.size() ; ++i)
        {
            ishock_edge_list adj_edges = outer_shock_nodes[i]->adj_edges();
            ishock_edge_list::iterator curS = adj_edges.begin();
            for ( ; curS != adj_edges.end() ; ++curS )
            {
                dbsk2d_ishock_edge* edge = *curS;
                if ( edge->lBElement()->is_a_point() )
                {
                    dbsk2d_ishock_bpoint* bpoint =
                        (dbsk2d_ishock_bpoint*) edge->lBElement();
                    if ( bpoint->is_an_end_point())
                    {
                        bool flag=false;
                        gap_endpoint(bpoint,gaps_visited,flag);
                    }
                }
                
                if(edge->rBElement()->is_a_point())
                {
                    dbsk2d_ishock_bpoint* bpoint =
                        (dbsk2d_ishock_bpoint*) edge->rBElement();
                    if ( bpoint->is_an_end_point())
                    {
                        bool flag=false;
                        gap_endpoint(bpoint,gaps_visited,flag);

                    }
                    
                }
            }
        }   
    }

    vcl_map< vcl_pair<int,int>,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >::iterator bit;
    bit = gaps_visited.begin();
    for ( ; bit != gaps_visited.end() ; ++bit)
    {
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> gap_pair
            = (*bit).second;
        int id1(0),id2(0);

        {
            dbsk2d_bnd_vertex* vertex=gap_pair.first->bnd_vertex();
            edge_list edges;
            vertex->edges(edges);
            
            edge_list::iterator it;
            if ( edges.size() == 2)
            {
                edges.erase(edges.begin());
            }
            it=edges.begin();
            
            const vcl_list< vtol_topology_object * > * 
                superiors  = (*it)->superiors_list();
            vcl_list<vtol_topology_object*>::const_iterator tit;
            tit=(*superiors).begin();
            
            id1 = (*tit)->get_id();
        }


        {
            dbsk2d_bnd_vertex* vertex=gap_pair.second->bnd_vertex();
            edge_list edges;
            vertex->edges(edges);
            
            edge_list::iterator it;
            if ( edges.size() == 2)
            {
                edges.erase(edges.begin());
            }
            it=edges.begin();
            
            const vcl_list< vtol_topology_object * > * 
                superiors  = (*it)->superiors_list();
            vcl_list<vtol_topology_object*>::const_iterator tit;
            tit=(*superiors).begin();
            
            id2 = (*tit)->get_id();
        }

        if ( id1 != id2 )
        {
            gap_pairs.push_back((*bit).second);
        }
    }
}

//: Detect all gap 1 for a bunch of regions
void dbsk2d_ishock_gap_detector::detect_all_gaps(
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >& region_outer_nodes,
    vcl_vector<
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >&
    gap_pairs,
    vcl_vector<
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> >&
    gap4_pairs)
{

    // Detect all gaps for a bunch of regions

    vcl_map< vcl_pair<int,int>,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> > gaps_visited;
    vcl_map< vcl_pair<int,int>,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> > gaps4_visited;

    // Loop thru new regions and determine new transforms
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >::iterator it;

    for ( it = region_outer_nodes.begin() ; it != region_outer_nodes.end(); 
          ++it)
    {
        // Detect transforms
        vcl_vector<dbsk2d_ishock_node*> outer_shock_nodes = (*it).second;
        for ( unsigned int i=0; i < outer_shock_nodes.size() ; ++i)
        {
            ishock_edge_list adj_edges = outer_shock_nodes[i]->adj_edges();
            ishock_edge_list::iterator curS = adj_edges.begin();
            for ( ; curS != adj_edges.end() ; ++curS )
            {
                dbsk2d_ishock_edge* edge = *curS;
                if ( edge->lBElement()->is_a_point() )
                {
                    dbsk2d_ishock_bpoint* bpoint =
                        (dbsk2d_ishock_bpoint*) edge->lBElement();
                    if ( bpoint->is_an_end_point())
                    {
                        bool flag=false;
                        gap_endpoint(bpoint,gaps_visited,flag);

                        if ( !flag )
                        {
                            vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*>
                                pair(0,0);
                            detect_gap4(bpoint,pair);
                            
                            if ( pair.first )
                            {
                                gaps4_visited[vcl_make_pair(pair.first->id(),
                                                            pair.second->id())]
                                    =pair;
                            }
                        }
                    }
                }
                
                if(edge->rBElement()->is_a_point())
                {
                    dbsk2d_ishock_bpoint* bpoint =
                        (dbsk2d_ishock_bpoint*) edge->rBElement();
                    if ( bpoint->is_an_end_point())
                    {
                        bool flag=false;
                        gap_endpoint(bpoint,gaps_visited,flag);

                        if ( !flag )
                        {
                            vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*>
                                pair(0,0);
                            detect_gap4(bpoint,pair);
                            
                            if ( pair.first )
                            {
                                gaps4_visited[vcl_make_pair(pair.first->id(),
                                                            pair.second->id())]
                                    =pair;
                            }
                        }
                    }
                    
                }
            }
        }   
    }

    vcl_map< vcl_pair<int,int>,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >::iterator bit;
    bit = gaps_visited.begin();
    for ( ; bit != gaps_visited.end() ; ++bit)
    {
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> gap_pair
            = (*bit).second;
        int id1(0),id2(0);

        {
            dbsk2d_bnd_vertex* vertex=gap_pair.first->bnd_vertex();
            edge_list edges;
            vertex->edges(edges);
            
            edge_list::iterator it;
            if ( edges.size() == 2)
            {
                edges.erase(edges.begin());
            }
            it=edges.begin();
            
            const vcl_list< vtol_topology_object * > * 
                superiors  = (*it)->superiors_list();
            vcl_list<vtol_topology_object*>::const_iterator tit;
            tit=(*superiors).begin();
            
            id1 = (*tit)->get_id();
        }


        {
            dbsk2d_bnd_vertex* vertex=gap_pair.second->bnd_vertex();
            edge_list edges;
            vertex->edges(edges);
            
            edge_list::iterator it;
            if ( edges.size() == 2)
            {
                edges.erase(edges.begin());
            }
            it=edges.begin();
            
            const vcl_list< vtol_topology_object * > * 
                superiors  = (*it)->superiors_list();
            vcl_list<vtol_topology_object*>::const_iterator tit;
            tit=(*superiors).begin();
            
            id2 = (*tit)->get_id();
        }

        if ( id1 != id2 )
        {
            gap_pairs.push_back((*bit).second);
        }
    }


    vcl_map< vcl_pair<int,int>,
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> >::iterator sit;
    sit = gaps4_visited.begin();
    for ( ; sit != gaps4_visited.end() ; ++sit)
    {
        gap4_pairs.push_back((*sit).second);
    }
}

//: Detect all gap 1 for whole ishock graph
void dbsk2d_ishock_gap_detector::detect_gap4(
    vcl_vector<vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> >& 
    gap_pairs)
{
    
    vcl_vector<dbsk2d_ishock_belm*> belm_list = boundary_->belm_list();
    for (unsigned int i=0;i < belm_list.size() ; ++i)
    {
        if ( belm_list[i]->is_a_point() )
        {
            dbsk2d_ishock_bpoint* bpoint = 
                dynamic_cast<dbsk2d_ishock_bpoint*>
                (belm_list[i]);
            
            if ( bpoint->is_an_end_point() && bpoint->is_a_GUIelm())
            {
                vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*>
                    pair(0,0);
                detect_gap4(bpoint,pair);

                if ( pair.first )
                {
                    gap_pairs.push_back(pair);
                }
            }

        }
    }




}


//: Detect all gap 1 
void dbsk2d_ishock_gap_detector::detect_gap4(
    dbsk2d_ishock_belm* belm,
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*>& gap4_pair)
{

    belm_list interacting_elms;
    belm->get_interacting_belements(interacting_elms);

    dbsk2d_ishock_bpoint* bp1 = (dbsk2d_ishock_bpoint*)belm;

    vcl_map<double,dbsk2d_ishock_bline*> element_maps;

    bnd_ishock_map_iter curS = belm->shock_map().begin();
    for ( ; curS != belm->shock_map().end() ; ++curS)
    {
        dbsk2d_ishock_elm* selm = curS->second;

        if ( selm->is_a_link())
        {
            dbsk2d_ishock_edge* iedge = (dbsk2d_ishock_edge*)(curS->second);
            dbsk2d_ishock_belm* lbe = iedge->lBElement();
            if ( lbe->id() == belm->id())
            {
                dbsk2d_ishock_belm* other_belm = iedge->rBElement();
                if ( other_belm->is_a_line())
                {
                    if ( iedge->pSNode())
                    {
                        if ( iedge->pSNode()->is_a_source())
                        {
                            dbsk2d_ishock_bline* other_bline =
                                (dbsk2d_ishock_bline*)(other_belm);
                            element_maps[
                                vcl_fabs(vnl_math::pi_over_2-curS->first.s_eta)]
                                =other_bline;
                        }
                    }
                }
                else if(other_belm->is_a_point())
                {
                    bool flag=false;
                    if ( iedge->pSNode())
                    {
                        if ( iedge->pSNode()->is_a_source())
                        {
                            dbsk2d_ishock_bpoint* other_bpoint =
                                (dbsk2d_ishock_bpoint*)(other_belm);
                            belm_list LinkedBElmList=
                                other_bpoint->LinkedBElmList;
                            belm_list::iterator it;
                            for ( it = LinkedBElmList.begin() ; 
                                  it != LinkedBElmList.end() ; 
                                  ++it)
                            {
                                belm_list::iterator bit;
                                for ( bit = interacting_elms.begin();
                                      bit != interacting_elms.end();
                                      ++bit)
                                {
                                    if ( (*it)->id() == (*bit)->id())
                                    {
                                        dbsk2d_ishock_bline* 
                                            other_bline=(dbsk2d_ishock_bline*)
                                            (*it);
                                        element_maps[
                                            vcl_fabs
                                            (vnl_math::pi_over_2-curS
                                             ->first.s_eta)]
                                            =other_bline;
                                        flag=true;
                                        break;
                                    }
                                }
                                if ( flag )
                                {
                                    break;
                                }

                            }
                        }
                    } 
                }
            }
            else 
            {
                dbsk2d_ishock_belm* other_belm = iedge->lBElement();
                if ( other_belm->is_a_line())
                {
                    if ( iedge->pSNode())
                    {
                        if ( iedge->pSNode()->is_a_source())
                        {
                            dbsk2d_ishock_bline* other_bline =
                                (dbsk2d_ishock_bline*)(other_belm);
                            element_maps[
                                vcl_fabs(vnl_math::pi_over_2-curS->first.s_eta)]
                                =other_bline;
                        }
                    }

                }
                else if(other_belm->is_a_point())
                {
                    bool flag=false;
                    if ( iedge->pSNode())
                    {
                        if ( iedge->pSNode()->is_a_source())
                        {
                            dbsk2d_ishock_bpoint* other_bpoint =
                                (dbsk2d_ishock_bpoint*)(other_belm);
                            belm_list LinkedBElmList=
                                other_bpoint->LinkedBElmList;
                            belm_list::iterator it;
                            for ( it = LinkedBElmList.begin() ; 
                                  it != LinkedBElmList.end() ; 
                                  ++it)
                            {
                                belm_list::iterator bit;
                                for ( bit = interacting_elms.begin();
                                      bit != interacting_elms.end();
                                      ++bit)
                                {
                                    if ( (*it)->id() == (*bit)->id())
                                    {
                                        dbsk2d_ishock_bline* other_bline
                                            =(dbsk2d_ishock_bline*)
                                            (*it);
                                        element_maps[
                                            vcl_fabs
                                            (vnl_math::pi_over_2-curS
                                             ->first.s_eta)]
                                            =other_bline;
                                        flag=true;
                                        break;
                                    }
                                }
                                if ( flag )
                                {
                                    break;
                                }

                            }
                        }
                    } 
                }
            }
                

        }
    }

    if ( element_maps.size() )
    {
        gap4_pair.first=bp1;
        gap4_pair.second=(*element_maps.begin()).second;
        
    }




}


//: remove boundary element
void dbsk2d_ishock_gap_detector::gap_endpoint(
    dbsk2d_ishock_bpoint* bp,
    vcl_map<vcl_pair<int,int>,
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >& gaps_visited,
    bool& flag)
{
    bnd_ishock_map_iter curS = bp->shock_map().begin();
    for ( ; curS != bp->shock_map().end() ; ++curS)
    {
        dbsk2d_ishock_elm* selm = curS->second;

        if ( selm->is_a_link())
        {
            dbsk2d_ishock_edge* iedge = (dbsk2d_ishock_edge*)(curS->second);
            dbsk2d_ishock_belm* lbe = iedge->lBElement();
            if ( lbe->id() == bp->id())
            {
                dbsk2d_ishock_belm* other_bp = iedge->rBElement();
                if ( other_bp->is_a_point())
                {
                    dbsk2d_ishock_bpoint* other_bpoint =
                        (dbsk2d_ishock_bpoint*)(other_bp);
                    if ( other_bpoint->is_an_end_point())
                    {
                        flag=true;
                        vcl_pair<int, int> pair1(
                            bp->id(),
                            other_bpoint->id());
                        vcl_pair<int, int> pair2(
                            other_bpoint->id(),
                            bp->id());
                        if ( gaps_visited.count(pair1)==0 
                             && 
                             gaps_visited.count(pair2)==0
                            )
                        {
                            gaps_visited[pair1]=vcl_make_pair(bp,other_bpoint);

                        }
                    }
                }
            }
            else 
            {
                dbsk2d_ishock_belm* other_bp = iedge->lBElement();
                if ( other_bp->is_a_point())
                {
                    dbsk2d_ishock_bpoint* other_bpoint =
                        (dbsk2d_ishock_bpoint*)(other_bp);
                    if ( other_bpoint->is_an_end_point())
                    {
                        flag=true;
                        vcl_pair<unsigned int, unsigned int> pair1(
                            bp->id(),
                            other_bpoint->id());
                        vcl_pair<unsigned int, unsigned int> pair2(
                            other_bpoint->id(),
                            bp->id());
                        if ( gaps_visited.count(pair1)==0 
                             && 
                             gaps_visited.count(pair2)==0
                            )
                        {
                            gaps_visited[pair1]=vcl_make_pair(bp,other_bpoint);

                        }
                    }
                     
                }
            }

        }
                

    }
    
}
