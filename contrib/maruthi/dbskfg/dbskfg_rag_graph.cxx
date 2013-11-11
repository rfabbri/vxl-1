// This is file shp/dbskfg/dbskfg_rag_graph.cxx

//:
// \file

#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <vgl/vgl_area.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <vcl_algorithm.h>

// ----------------------------------------------------------------------------
//: Constructor
dbskfg_rag_graph::
dbskfg_rag_graph(): 
    dbgrl_graph<dbskfg_rag_node, dbskfg_rag_link>(),
    next_available_id_(0)
{

}



unsigned int dbskfg_rag_graph::next_available_id()
{
    next_available_id_++;
    return next_available_id_;


}
dbskfg_rag_node_sptr dbskfg_rag_graph::rag_node(unsigned int rag_id)
{
    dbskfg_rag_node_sptr node(0);
    
    // push back node 
    for (dbskfg_rag_graph::vertex_iterator vit = 
             (*this).vertices_begin(); 
         vit != (*this).vertices_end(); ++vit)
    {
       
        if ( (*vit)->id() == rag_id )
        {
            node=*vit;
            break;
        }
    }

    if ( node == 0 && region_tree_.count(rag_id) )
    {        
        // This was a deleted node, it could have broken apart or 
        // have been merged pick the largest
        // Find the correspoinding id in the rag graph
        vcl_map<unsigned int,vcl_string> regions=region_tree_[rag_id];
        vcl_map<unsigned int,vcl_string>::iterator nit;

        nit = regions.begin();
        while(region_tree_.count((*nit).first))
        {

            nit = region_tree_[(*nit).first].begin();

        }

        
        for (dbskfg_rag_graph::vertex_iterator vit = 
                 (*this).vertices_begin(); 
             vit != (*this).vertices_end(); ++vit)
        {
            if ( (*vit)->id() == (*nit).first )
            {
                node = (*vit);
            }
        }
  
    }

    return node;
}


dbskfg_rag_node_sptr dbskfg_rag_graph::rag_node(vcl_set<vcl_string>& 
                                                wavefront)
{

    dbskfg_rag_node_sptr node(0);
    
    // push back node 
    for (dbskfg_rag_graph::vertex_iterator vit = 
             (*this).vertices_begin(); 
         vit != (*this).vertices_end(); ++vit)
    {
       
        vcl_set<vcl_string> regions_to_compare;
        (*vit)->wavefront_string(regions_to_compare);

        vcl_vector<vcl_string> intersection(100);
        vcl_vector<vcl_string>::iterator start_iterator;
        start_iterator=intersection.begin();
        vcl_vector<vcl_string>::iterator out_iterator;
        
        out_iterator=vcl_set_intersection(wavefront.begin(),
                                          wavefront.end(),
                                          regions_to_compare.begin(),
                                          regions_to_compare.end(),
                                          start_iterator);
        int int_size = int(out_iterator-start_iterator);
        
        if ( int_size == wavefront.size() )
        {
            node=*vit;
            break;
        }
    }


    return node;
}


dbskfg_rag_node_sptr dbskfg_rag_graph::rag_node(unsigned int rag_id,
                                                vcl_set<unsigned int>& 
                                                rag_con_ids)
{
    
    dbskfg_rag_node_sptr node(0);

    // push back node 
    for (dbskfg_rag_graph::vertex_iterator vit = 
             (*this).vertices_begin(); 
         vit != (*this).vertices_end(); ++vit)
    {
       
        if ( (*vit)->id() == rag_id )
        {
            node=*vit;
            break;
        }
    }
    
    if ( node == 0 && region_tree_.count(rag_id) )
    {        
        // This was a deleted node, it could have broken apart or 
        // have been merged pick the largest
        // Find the correspoinding id in the rag graph
        vcl_map<unsigned int,vcl_string> regions=region_tree_[rag_id];
        vcl_map<unsigned int,vcl_string>::iterator nit;

        nit = regions.begin();
        while(region_tree_.count((*nit).first))
        {

            nit = region_tree_[(*nit).first].begin();

        }

        
        for (dbskfg_rag_graph::vertex_iterator vit = 
                 (*this).vertices_begin(); 
             vit != (*this).vertices_end(); ++vit)
        {
            if ( (*vit)->id() == (*nit).first )
            {
                node = (*vit);
            }
        }
  
    }
    else
    {   
        for (dbskfg_rag_graph::vertex_iterator vit = 
                 (*this).vertices_begin(); 
             vit != (*this).vertices_end(); ++vit)
        {
            vcl_set<unsigned int> test_set;
            (*vit)->rag_contour_ids(test_set);

            if ( vcl_includes(test_set.begin(),
                              test_set.end(),
                              rag_con_ids.begin(),
                              rag_con_ids.end()))
            {
                node = (*vit);
                break;
            }
        }
     
    }

    return node;
}

void dbskfg_rag_graph::
rag_node(vcl_set<unsigned int> rag_con_ids,
         dbskfg_transform_descriptor_sptr transform,
         vcl_vector<dbskfg_rag_node_sptr>& rag_nodes)
{
    
    // See if this region is deleted
    vcl_vector<dbskfg_composite_node_sptr>::iterator nit;

    for ( nit = transform->contours_to_remove_.begin(); 
          nit != transform->contours_to_remove_.end() ; 
          ++nit)
    {
        dbskfg_contour_node* node = dynamic_cast<dbskfg_contour_node*>
            (&(*(*nit)));
     
        if ( node->get_composite_degree() < 3 )
        {
            vcl_set<unsigned int>::iterator fit;
            fit = rag_con_ids.find(node->contour_id());
        
            if ( fit != rag_con_ids.end() )
            {
                rag_con_ids.erase(fit);
            }
        }
   
                  
    }


    vcl_vector<unsigned int> intersection(100,0);
    int offset=0;

    // Now see if we can find a combination out of 
    if ( rag_con_ids.size() >= 2 )
    {
        vcl_set<unsigned int>::iterator it;
        for ( it = rag_con_ids.begin() ; it != rag_con_ids.end() ;
              ++it)
        {
            if ( !contour_to_region_.count((*it)) )
            {
                continue;
            }

            vcl_set<unsigned int> regions_of_contour =
                contour_to_region_[(*it)];
            vcl_set<unsigned int>::iterator hit;
            hit=it;
            hit++;

            for ( ; hit != rag_con_ids.end(); ++hit)
            {
                if ( !contour_to_region_.count((*hit)) )
                {
                    continue;
                }

                vcl_set<unsigned int> regions_to_compare =
                    contour_to_region_[(*hit)];

                vcl_vector<unsigned int>::iterator start_iterator;
                start_iterator=intersection.begin()+offset;
                vcl_vector<unsigned int>::iterator out_iterator;

                out_iterator=vcl_set_intersection(regions_of_contour.begin(),
                                                  regions_of_contour.end(),
                                                  regions_to_compare.begin(),
                                                  regions_to_compare.end(),
                                                  start_iterator);
                int int_size = int(out_iterator-start_iterator);
                offset =offset+int_size;                
                
            }
            
        }

    }
    else
    {


        vcl_set<unsigned int>::iterator it;
        for ( it = rag_con_ids.begin() ; it != rag_con_ids.end() ;
              ++it)
        {

            if ( !contour_to_region_.count((*it)) )
            {
                continue;
            }

            vcl_set<unsigned int> regions_of_contour =
                contour_to_region_[(*it)];
            vcl_vector<dbskfg_composite_link_sptr>::iterator hit;
          
            for ( hit=transform->contours_affected_.begin(); hit != 
                      transform->contours_affected_.end(); ++hit)
            {


                dbskfg_contour_link* clink = dynamic_cast<dbskfg_contour_link*>
                    (&(*(*hit)));

                unsigned int cid = clink->contour_id();

                if ( cid == (*it))
                {
                    continue;
                }


                if ( !contour_to_region_.count(cid))
                {
                    continue;
                }

                vcl_set<unsigned int> regions_to_compare =
                    contour_to_region_[cid];
  
                vcl_vector<unsigned int>::iterator start_iterator;
                start_iterator=intersection.begin()+offset;
                vcl_vector<unsigned int>::iterator out_iterator;

                out_iterator=vcl_set_intersection(regions_of_contour.begin(),
                                                  regions_of_contour.end(),
                                                  regions_to_compare.begin(),
                                                  regions_to_compare.end(),
                                                  start_iterator);
                

                int int_size = int(out_iterator-start_iterator);
                offset =offset+int_size;
            
            }
            
        }

    }

    
    // Remove duplicates
    vcl_set<unsigned int> unique_frags;
    for ( unsigned int i=0; i < intersection.size() ; ++i)
    {
        if ( intersection[i]>0)
        {
            unique_frags.insert(intersection[i]);
        }
    }

    vcl_set<unsigned int>::iterator rit;
    for ( rit = unique_frags.begin() ; rit != unique_frags.end(); ++rit)
    {
        for (dbskfg_rag_graph::vertex_iterator vit = 
                 (*this).vertices_begin(); 
             vit != (*this).vertices_end(); ++vit)
        {
       
            if ( (*vit)->id() == *rit )
            {
                rag_nodes.push_back(*vit);
                break;
            }

        }

    }

}

dbskfg_rag_node_sptr 
dbskfg_rag_graph::find_region(
    const vcl_map<unsigned int, vgl_point_2d<double> >& wavefront)
{
    // Test if shock ray lies in vertex
    for (dbskfg_rag_graph::vertex_iterator vit = 
             this->vertices_begin(); 
         vit != this->vertices_end(); ++vit)
    {
        
        if ( (*vit)->wavefront_exists(wavefront) && wavefront.size() ==
             (*vit)->get_wavefront().size())
        {
            return (*vit);
        }

    }

    return 0;

   
}
//: return number of regular fragments()
unsigned int dbskfg_rag_graph::numb_regular_fragments()
{
    unsigned int number=0;
   // Test if shock ray lies in vertex
    for (dbskfg_rag_graph::vertex_iterator vit = 
             this->vertices_begin(); 
         vit != this->vertices_end(); ++vit)
    {
        if (!(*vit)->endpoint_spawned_node())
        {
            ++number;
        }
    }

    return number;

}

void dbskfg_rag_graph::create_copy(const dbskfg_composite_graph_sptr& cgraph,
                                   dbskfg_rag_graph& that)
{

    // Reset current id
    this->set_current_id(that.current_id());

    // Test if shock ray lies in vertex
    for (dbskfg_rag_graph::vertex_iterator vit = 
             that.vertices_begin(); 
         vit != that.vertices_end(); ++vit)
    {
        vcl_map<unsigned int,dbskfg_shock_link*> that_shock_links =
            (*vit)->get_shock_links();

        dbskfg_rag_node_sptr this_rag_node = new dbskfg_rag_node(
            (*vit)->id());
        
        vcl_map<unsigned int,dbskfg_shock_link*>::iterator that_it;
        for ( that_it = that_shock_links.begin(); 
              that_it != that_shock_links.end(); ++that_it)
        {
            dbskfg_shock_link* that_shock = (*that_it).second;
            
            dbskfg_composite_link_sptr find_shock = 
                cgraph->find_link(that_shock->id());
            
            dbskfg_shock_link* this_shock = dynamic_cast<dbskfg_shock_link*>
                (&(*find_shock));
            
            this_rag_node->add_shock_link(this_shock);
            this_shock->set_rag_node(&(*this_rag_node));
           
            
        }

        this->add_vertex(this_rag_node);

    }

}
