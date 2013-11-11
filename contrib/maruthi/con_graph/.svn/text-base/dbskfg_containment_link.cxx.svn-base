// This is brcv/shp/dbskfg/dbskfg_containment_link.cxx

//:
// \file

#include <con_graph/dbskfg_containment_link.h>
#include <con_graph/dbskfg_containment_node.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_clip.h>
#include <dbskfg/algo/dbskfg_transform_manager.h>
#include <vcl_sstream.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/algo/dbskfg_expand_local_context.h>
#include <dbskfg/algo/dbskfg_loop_transforms.h>

//: Constructor
dbskfg_containment_link::dbskfg_containment_link(
    dbskfg_containment_node_sptr source,
    dbskfg_containment_node_sptr target,
    dbskfg_transform_descriptor_sptr transform) 
 :dbgrl_edge<dbskfg_containment_node>(source,target),
  transform_(transform),
  grouped_transform_(0),
  cost_(transform->cost_),
  transform_type_(transform->transform_type_),
  new_contour_id_(0),
  link_size_(0.0),
  distance_(0.0)
{

}

//: Destructor
dbskfg_containment_link::~dbskfg_containment_link() 
{
    if ( transform_ )
    {
        transform_->destroy_transform();
        transform_=0;
    }

    if ( grouped_transform_)
    {
        grouped_transform_->destroy_transform();
        grouped_transform_=0;
    }
}

void dbskfg_containment_link::set_contours_affected(
    vcl_vector<unsigned int> contours)
{
    contours_affected_ = contours;

    if ( transform_type_ == dbskfg_transform_descriptor::GAP )
    {
        new_contour_id_ = contours[contours.size()-1];
        contours_affected_.pop_back();
    }

}

void dbskfg_containment_link::print(vcl_ostream& os)
{
    if ( transform_type_ == dbskfg_transform_descriptor::GAP )
    {
        vcl_cout<<"Tranform Type: GAP"<<vcl_endl;
        vcl_cout<<"New Contour Id: "<<new_contour_id_<<vcl_endl;
    }
    else
    {

        vcl_cout<<"Transform Type: LOOP"<<vcl_endl;
    }

    
    vcl_cout<< "Contours Affected: ";
    
    for ( unsigned int i=0; i < contours_affected_.size() ; ++i)
    {

        vcl_cout<<contours_affected_[i]<<" ";
    }
    vcl_cout<<vcl_endl;

}

void dbskfg_containment_link::determine_group_transform()
{

    if ( this->source()->in_degree())
    {
        grouped_transform_ = new dbskfg_transform_descriptor();

        dbskfg_transform_descriptor_sptr source_transform =
            (*this->source()->in_edges_begin())->grouped_transform_;

        dbskfg_transform_descriptor_sptr trans2(0);

        // Decompose transform
        if ( transform_->transform_type_ 
             == dbskfg_transform_descriptor::GAP)
        {
            vcl_pair<vcl_string,vcl_string> gap_strings =
                transform_->gap_string();        
        
            vcl_map<vcl_string,unsigned int>& old_gap_map =
                dbskfg_transform_manager::Instance().old_gap_map();

            // See if exists in gap map
            if ( old_gap_map.count(gap_strings.first))
            {
                trans2 = dbskfg_transform_manager::Instance()
                    .transform_at_id(old_gap_map[gap_strings.first]);
                grouped_transform_->group_transform(*trans2);
            }

        }
        else
        {
            if ( transform_->loop_endpoints_.size() )
            {
                vcl_stringstream sstream1;
                sstream1<<transform_->loop_endpoints_[0]->pt();
        
                vcl_stringstream sstream2;
                sstream2<<transform_->loop_endpoints_[1]->pt();

                vcl_pair<vcl_string,vcl_string> pair1 =
                    vcl_make_pair(sstream1.str(),sstream2.str());

                if ( dbskfg_transform_manager::Instance().loop_pair(
                         pair1))
                {
                    trans2 = dbskfg_transform_manager::Instance().
                        loop_by_endpoints(pair1);
                    grouped_transform_->group_transform(*trans2);

                }
                else
                { 

                    bool flag_exist=false;

                    // loop endpoints
                    for ( unsigned int k=0; k < transform_->
                              loop_endpoints_.size() ; ++k )
                    {
                        dbskfg_composite_node_sptr new_node = 
                            dbskfg_transform_manager::Instance()
                            .get_cgraph()->find_node(
                                transform_->loop_endpoints_[k]->id());

                        if ( new_node->get_composite_degree() == 2 )
                        {
                            flag_exist=true;
                            break;
                        }
        
                    }

                    
                    if ( flag_exist)
                    {
 
                        dbskfg_composite_graph_sptr local_cgraph
                            = dbskfg_transform_manager::Instance()
                            .get_cgraph();

                        dbskfg_loop_transforms loop_detector
                            (local_cgraph,
                             dbskfg_transform_manager::Instance().get_image());

                        loop_detector.detect_loops(transform_);
                        trans2=transform_;
                        grouped_transform_->group_transform(*trans2);

                    }
                    else
                    {
                        dbskfg_transform_manager::Instance().find_transform
                            (transform_->contour_links_to_remove_,
                         grouped_transform_);
                        trans2=grouped_transform_;
                    }
                }
            }
            else
            {
                vcl_vector<unsigned int> contour_ids=
                    transform_->contour_ids_affected();

                unsigned int tran_in = dbskfg_transform_manager::Instance().
                    loop_by_contour_id(contour_ids[0]);

                trans2 = dbskfg_transform_manager::Instance().transform_at_id
                    (tran_in);
                grouped_transform_->group_transform(*trans2);

                for (unsigned int k=1; k < contour_ids.size() ; ++k)
                {
                
                    tran_in = dbskfg_transform_manager::Instance().
                        loop_by_contour_id(contour_ids[k]);

                    grouped_transform_->group_transform(
                        *dbskfg_transform_manager::Instance().
                        transform_at_id(tran_in));


                }

            }
   
        }
      
        grouped_transform_->group_transform(*source_transform);
        grouped_transform_->transform_type_ =
            dbskfg_transform_descriptor::LOOP;
        
        if ( trans2 == 0 )
        {


            // Add to grouped transform new contours spatial objects
            for ( unsigned int t=0; 
                  t < transform_->new_contours_spatial_objects_.size(); ++t)
            {
                grouped_transform_->new_contours_spatial_objects_.push_back(
                    transform_->new_contours_spatial_objects_[t]);
            }

            dbskfg_expand_local_context expander(grouped_transform_);
            grouped_transform_->group_cnodes_affected(*transform_);

            dbskfg_composite_graph_sptr local_cgraph
                = dbskfg_transform_manager::Instance()
                .get_cgraph();

            vcl_vector<unsigned int> nodes_to_delete;
            for ( unsigned int v=0; v < grouped_transform_->
                      contours_nodes_affected_.size(); ++v)
            {
                if (! 
                    local_cgraph->find_node(
                        grouped_transform_->contours_nodes_affected_[v]->id())
                    )
                {

                    nodes_to_delete.push_back(v);
                }
                     

            }

            for ( unsigned int d=0; d < nodes_to_delete.size(); ++d)
            {
                grouped_transform_->contours_nodes_affected_.
                    erase(grouped_transform_->contours_nodes_affected_.begin()
                          +nodes_to_delete[d]);
            }
        }
        
        grouped_transform_->remove_extra_shocks();

        // We can destroy transform_ at this point
        // it will be destroyed by calling node
        transform_=0;
    }
    else
    {
        grouped_transform_ = transform_;
        transform_ = 0;
    }



}
