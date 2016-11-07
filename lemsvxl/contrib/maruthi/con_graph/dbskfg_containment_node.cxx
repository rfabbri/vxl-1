// This is brcv/shp/dbskfg/dbskfg_containment_node.cxx

//:
// \file

// con_graph headers
#include <con_graph/dbskfg_containment_node.h>
#include <con_graph/dbskfg_containment_node_sptr.h>
#include <con_graph/dbskfg_containment_link.h>
#include <con_graph/dbskfg_containment_graph.h>
// dbskfg headers
#include <dbskfg/algo/dbskfg_composite_graph_fileio.h>
#include <dbskfg/dbskfg_utilities.h>
#include <dbskfg/pro/dbskfg_extract_fragments_process.h>
#include <dbskfg/algo/dbskfg_transform_manager.h>
#include <dbskfg/algo/dbskfg_transformer.h>
#include <dbskfg/algo/dbskfg_detect_transforms.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_contour_link.h>
// vul headers
#include <vul/vul_psfile.h>
#include <vul/vul_file.h>
// breye1 headers
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
// vsol headers
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
// vcl headers
#include <vcl_sstream.h>
#include <vcl_algorithm.h>
#include <vcl_algorithm.h>
#include <vcl_iterator.h>
// vil headers
#include <vil/vil_image_resource.h>

//: Constructor
dbskfg_containment_node::dbskfg_containment_node(unsigned int id,
                                                 unsigned int rag_node_id,
                                                 unsigned int depth)
:dbgrl_vertex<dbskfg_containment_link>(),
 id_(id),
 rag_node_id_(rag_node_id),
 cgraph_(0),
 rag_graph_(0),
 depth_(depth),
 prob_fragment_(1.0),
 poly_()
{

}

//: Constructor
dbskfg_containment_node::dbskfg_containment_node(
    unsigned int id,
    dbskfg_composite_graph& prev_cgraph,
    dbskfg_rag_graph& prev_rag_graph,
    unsigned int depth)
:dbgrl_vertex<dbskfg_containment_link>(),
 id_(id),
 cgraph_(0),
 rag_graph_(0),
 depth_(depth),
 prob_fragment_(1.0)
{
     
    cgraph_  = new dbskfg_composite_graph(prev_cgraph);
  
    rag_graph_ = new dbskfg_rag_graph();

    rag_graph_->create_copy(cgraph_,prev_rag_graph);
}

//: Destructor
dbskfg_containment_node::~dbskfg_containment_node() 
{

    if ( cgraph_ )
    {
        cgraph_->clear();
        cgraph_=0;
    }

    if ( rag_graph_ )
    {
        rag_graph_->clear();
        rag_graph_=0;
    }

}

//: Prints information about the node
void dbskfg_containment_node::print(vcl_ostream& os)
{
    os<<"Node id: "<<id_<<vcl_endl;
    os<<"Rag  id: "<<rag_node_id_<<vcl_endl;

    os<<"Rag  con ids: ";
    vcl_set<unsigned int>::iterator bit;
    for ( bit = rag_con_ids_.begin(); bit != rag_con_ids_.end(); ++bit)
    {
        os<<*bit<<" ";
    }
    os<<vcl_endl;

    os<<"Rag Wavefront: ";
    vcl_set<vcl_string>::iterator sit;
    for ( sit = rag_node_wavefront_.begin(); 
          sit != rag_node_wavefront_.end(); ++sit)
    {
        os<<*sit<<" ";
    }
    os<<vcl_endl;
    
    os<<"Attribute vector: "<<vcl_endl;
    vcl_map<unsigned int,bool>::iterator it;
    for ( it = attr_.begin() ; it != attr_.end() ; ++it)
    {
        os<<"Contour ("<<(*it).first<<") : "<<(*it).second<<vcl_endl; 
    }
    vcl_cout<<"Numb trans applicable "<<this->out_degree()<<vcl_endl;
    vcl_cout<<vcl_endl;
}

//: Prints the output of the node as a cem
void dbskfg_containment_node::print_cem(
    vcl_map<unsigned int,vcl_vector<vsol_spatial_object_2d_sptr> >& 
    contours)                 
{

    // 2) Get vsol storage class 
    // Create vsol storage class with affected contour for transforms
    vidpro1_vsol2D_storage_sptr contour_storage = vidpro1_vsol2D_storage_new();
    
    vcl_vector<vsol_spatial_object_2d_sptr> all_contours;
    vcl_map<unsigned int,bool>::iterator ait;

    // Determine contours to populate 
    for ( ait = attr_.begin() ; ait != attr_.end() ; ++ait)
    {
        if ( (*ait).second )
        {
            vcl_vector<vsol_spatial_object_2d_sptr> cons = 
                contours[(*ait).first];

            // If we have more than one coutour is signifies a gap set
            // We need to create an aggregrate polyline
            if ( cons.size() > 1 )
            {
                // Firt element will be a line

                // Grab first line
                vsol_line_2d* first_line = cons[0]->
                    cast_to_curve()->cast_to_line();
                vsol_polyline_2d* poly_line = cons[1]
                    ->cast_to_curve()->cast_to_polyline();
                vsol_line_2d* second_line = cons[2]->
                    cast_to_curve()->cast_to_line();

                vcl_vector<vsol_point_2d_sptr> ps;
                ps.push_back(first_line->p0());

                for ( unsigned int p=0; p < poly_line->size() ; ++p)
                {
                    ps.push_back(poly_line->vertex(p));
                }
                
                ps.push_back(second_line->p1());
                vsol_spatial_object_2d_sptr poly = new vsol_polyline_2d(ps);
                all_contours.push_back(poly);

            }
            else
            {
                all_contours.push_back(cons[0]);
            }
          
        }
    }

    contour_storage->add_objects(all_contours);

    // Save off cem file

    /*********************** Shock Compute **********************************/
    // 3) Create shock pro process and assign inputs 
    vidpro1_save_cem_process save_cem;

    save_cem.clear_input();
    save_cem.clear_output();

    save_cem.add_input(contour_storage);

    vcl_stringstream fstream;
    fstream<<"Node"<<id_<<"_map.cem";

    bpro1_filepath file_pathf(fstream.str());

    // Set params
    save_cem.parameters()->set_value("-cemoutput",file_pathf);

    bool cem_flag = save_cem.execute();
    save_cem.finish();
}

//: Method updates attributes related to on/off swithces
void dbskfg_containment_node::update_attributes()
{
    
    // clear old attributes first
    attr_.clear();

    // Look at the incoming link and attributes
    dbskfg_containment_node_sptr source_node = 
        (*(this->
           in_edges_begin()))->source();

    // Grab old vertices
    vcl_map<unsigned int, bool> attr =source_node->get_attributes();

    // Lets look at incoming link
    dbskfg_containment_link_sptr incoming_link = *this->in_edges_begin();
    

    // Set old attributes to new attributes
    attr_ = attr;

    // If gap update map
    if ( incoming_link->transform_type() == dbskfg_transform_descriptor::GAP )
    {
        attr_[incoming_link->get_new_contour_id()]=true;
    }
    else
    {
        vcl_vector<unsigned int> contours = incoming_link->
            get_contours_affected();
        for ( unsigned int k=0; k < contours.size() ; ++k)
        {
            attr_[contours[k]]=false;

        }
    }

}

// Detect whether two nodes merge
bool dbskfg_containment_node::nodes_merge(
    dbskfg_containment_node_sptr compare_node,
    vcl_vector<unsigned int> contours_affected,
    dbskfg_transform_descriptor::TransformType transform)
{
    
    // First look at the node we are expanding and determing string
   
    // This is the node we are going to expand
    // We want to make a copy here not reference! due to seeing what node is
    vcl_map<unsigned int,bool> node_to_expand_attr = attr_; 

    // If gap update map
    if ( transform == dbskfg_transform_descriptor::GAP )
    {
        node_to_expand_attr[contours_affected.back()]=true;
    }
    else
    {
        for ( unsigned int k=0; k < contours_affected.size() ; ++k)
        {
            node_to_expand_attr[contours_affected[k]]=false;

        }
    }

    // Next compute string for comparision node
    vcl_map<unsigned int,bool> compare_node_attr = 
        compare_node->get_attributes();    
      
    // Lets determine maximum size of original size
    unsigned int string1_size  = (*compare_node_attr.rbegin()).first;
    unsigned int string2_size  = (*node_to_expand_attr.rbegin()).first;

    vcl_vector<bool> bit_set1(string1_size,false);
    vcl_vector<bool> bit_set2(string2_size,false);

    for  ( unsigned int i=0; i <= string1_size ; ++i) 
    {
        if ( compare_node_attr.count(i) && compare_node_attr[i] )
        {
            bit_set1[i]=true;
        }
    }
   
    for  ( unsigned int j=0 ; j <= string2_size ; ++j)
    {
        if ( node_to_expand_attr.count(j) && node_to_expand_attr[j] )
        {
            bit_set2[j]=true;
        }
    }

    return bit_set1 == bit_set2;

}

bool dbskfg_containment_node::node_empty()
{

    unsigned int count=0;
    vcl_map<unsigned int,bool>::iterator it;
    for ( it = attr_.begin() ; it != attr_.end() ; ++it)
    {
        if ( (*it).second )
        {
            count++;
        }
    }

    return (count <= 1);
}

void dbskfg_containment_node::expand_node_explicit(
    vcl_map<unsigned int,dbskfg_containment_node_sptr>& nodes,
    vcl_map<unsigned int, 
    vcl_vector<vsol_spatial_object_2d_sptr> >& contours,
    dbskfg_containment_graph& cgraph)
{

    vcl_cout<<vcl_endl;
    vcl_cout<<"********************************"<<vcl_endl;
    vcl_cout<<"Expanding Node: "<<id_<<vcl_endl;
    vcl_cout<<"********************************"<<vcl_endl;

    // Lets look at incoming link
    dbskfg_containment_link_sptr incoming_link = *this->in_edges_begin();

    // Set rag contour ids to whatever the previous link was
    this->rag_con_ids_ = incoming_link->source()->rag_con_ids_;

    // Graph old region id
    unsigned int old_region_id =
        incoming_link->source()->rag_node_id_;
 
    // Grab incoming link transform
    dbskfg_transform_descriptor_sptr incoming_transform
        = incoming_link->transform();
   
    // We have to reset ids
    incoming_transform->create_copy(cgraph_);
   
    // Apply the actual transform
    {
  
        dbskfg_transformer transformer(incoming_transform,
                                       cgraph_,
                                       rag_graph_);
    }

    // Grow the regions
    {
        dbskfg_region_growing_transforms region_growing(rag_graph_);
        region_growing.grow_regions(old_region_id);
    }

    // Now find region after transform
    dbskfg_rag_node_sptr region = rag_graph_->rag_node(old_region_id,
                                                       this->rag_con_ids_);

    // Set region of node
    this->rag_node_id_ = region->id();

    // Write out the region
    vcl_stringstream node_str,depth_str;
    node_str<<id_;
    depth_str<<depth_;

    // Create wavefront set
    {
         vcl_map<unsigned int, dbskfg_shock_node*> wavefront
             = region->get_wavefront();

         vcl_map<unsigned int, dbskfg_shock_node*>::iterator it;
         for ( it = wavefront.begin(); it != wavefront.end() ; ++it)
         {
             vgl_point_2d<double> point = (*it).second->pt();
             vcl_stringstream stream;
             stream<<point;
             rag_node_wavefront_.insert(stream.str());
         }
    }

    // Print region
    // Look at incoming set
    vcl_set<vcl_string> incoming_node_set = incoming_link->source()->
        rag_node_wavefront_;
    if( !vcl_includes(this->rag_node_wavefront_.begin(),
                      this->rag_node_wavefront_.end(),
                      incoming_node_set.begin(),
                      incoming_node_set.end()))
    {
        // Create node name
        vcl_string node_name = dbskfg_transform_manager::Instance().
            get_output_prefix()+"_cgraph_node_"+node_str.str()+"_"
            +depth_str.str();

        if ( region->is_rag_node_within_image(
                 dbskfg_transform_manager::Instance().get_image()->ni(),
                 dbskfg_transform_manager::Instance().get_image()->nj()))
        {
      
            // Print region
            dbskfg_composite_graph_fileio file;
            file.write_out(rag_graph_,
                           dbskfg_transform_manager::Instance().get_image(),
                           region->id(),
                           1.0,
                           node_name,
                           dbskfg_transform_manager::Instance().
                           get_output_frag_folder());
        }
    }

    // Now with this region find new transforms
    vcl_vector<dbskfg_transform_descriptor_sptr> trans;
    {
        // First lets detect transforms
        dbskfg_detect_transforms transforms(
            cgraph_,
            dbskfg_transform_manager::Instance().get_image());

        transforms.transform_affects_region(region,rag_con_ids_);
        trans = transforms.objects();
    }

    for ( unsigned int i=0; i < trans.size() ; ++i)
    {
        vcl_vector<unsigned int> contours_affected = 
            trans[i]->contour_ids_affected();
        
        if ( trans[i]->transform_type_ == dbskfg_transform_descriptor::LOOP)
        {
            bool flag=false;
            for ( unsigned int c=0; c < contours_affected.size() ; ++c)
            {
                if ( rag_con_ids_.count(contours_affected[c]) ||
                     contours_affected[c] == 0)
                {
                    flag=true;
                    break;
                }
            }

            if ( flag)
            {
                trans[i]->destroy_transform();
                continue;
            }
        }

        expand_node_helper(trans[i],
                           contours,
                           contours_affected);



        insert_new_node(nodes,cgraph,contours_affected,trans[i]);
        

    }
    cgraph_->destroy_cache();
  
    for (dbskfg_containment_node::edge_iterator it = this->in_edges_begin();
         it != this->in_edges_end(); ++it)
    {
        (*it)->destroy_transform();
    }

    cgraph_->clear();
    cgraph_=0;
    rag_graph_->clear();
    rag_graph_=0;
    trans.clear();
  
}


void dbskfg_containment_node::expand_node_implicit(
    vcl_map<unsigned int,dbskfg_containment_node_sptr>& nodes,
    vcl_map<unsigned int, 
    vcl_vector<vsol_spatial_object_2d_sptr> >& contours,
    dbskfg_containment_graph& cgraph)
{

    vcl_cout<<vcl_endl;
    vcl_cout<<"********************************"<<vcl_endl;
    vcl_cout<<"Expanding Node: "<<id_<<vcl_endl;
    vcl_cout<<"********************************"<<vcl_endl;

    // Lets look at incoming link
    dbskfg_containment_link_sptr incoming_link = *this->in_edges_begin();

    // Set rag contour ids to whatever previous link was
    this->rag_con_ids_ = incoming_link->source()->rag_con_ids_;

    // Grab incoming grouped transform
    dbskfg_transform_descriptor_sptr incoming_transform
        = incoming_link->grouped_transform();

    // Create local copies of everything
    dbskfg_composite_graph local_cgraph(*dbskfg_transform_manager::Instance().
                                        get_cgraph());
    dbskfg_composite_graph_sptr local_cgraph_sptr = &local_cgraph;

    // We have to reset ids
    incoming_transform->create_copy(local_cgraph_sptr);
     
    dbskfg_rag_graph local_rag_graph;
    local_rag_graph.create_copy(
        local_cgraph_sptr,
        *dbskfg_transform_manager::Instance().get_rag_graph());

    //  Create smart ptrs for both
    dbskfg_rag_graph_sptr local_rag_graph_sptr = &local_rag_graph;        
    
    bool region_invalid = false;
    // Apply the actual transform
    {
  
        dbskfg_transformer transformer(incoming_transform,
                                       local_cgraph_sptr,
                                       local_rag_graph_sptr);

        region_invalid = transformer.rag_deleted(this->rag_node_id_);
    }

    // Grow just this region
    dbskfg_rag_node_sptr region(0);
    if ( !region_invalid ) 
    {
        dbskfg_region_growing_transforms region_growing(local_rag_graph_sptr);
        region = region_growing.grow_region(this->rag_node_id_);
    }
    else
    {
        // Graph old region id
        unsigned int old_region_id =
            incoming_link->source()->rag_node_id_;

        dbskfg_region_growing_transforms region_growing(local_rag_graph_sptr);
        region_growing.grow_regions(old_region_id);
        region = local_rag_graph_sptr->rag_node(old_region_id,
                                                this->rag_con_ids_);
        this->rag_node_id_ = region->id();
    }
    local_rag_graph_sptr->destroy_map();

    // Write out the region
    vcl_stringstream node_str,depth_str;
    node_str<<id_;
    depth_str<<depth_;

    // Create wavefront set
    {
         vcl_map<unsigned int, dbskfg_shock_node*> wavefront
             = region->get_wavefront();

         vcl_map<unsigned int, dbskfg_shock_node*>::iterator it;
         for ( it = wavefront.begin(); it != wavefront.end() ; ++it)
         {
             vgl_point_2d<double> point = (*it).second->pt();
             vcl_stringstream stream;
             stream<<point;
             rag_node_wavefront_.insert(stream.str());
         }
    }

    // Print region
    // Look at incoming set
    vcl_set<vcl_string> incoming_node_set = incoming_link->source()->
        rag_node_wavefront_;
    if( !vcl_includes(this->rag_node_wavefront_.begin(),
                      this->rag_node_wavefront_.end(),
                      incoming_node_set.begin(),
                      incoming_node_set.end()))
    {
        // Create node name
        vcl_string node_name = dbskfg_transform_manager::Instance().
            get_output_prefix()+"_cgraph_node_"+node_str.str()+
            "_"+depth_str.str();

        if ( region->is_rag_node_within_image(
                 dbskfg_transform_manager::Instance().get_image()->ni(),
                 dbskfg_transform_manager::Instance().get_image()->nj()))
        {
            // Print region
            dbskfg_composite_graph_fileio file;
            file.write_out(local_rag_graph_sptr,
                           dbskfg_transform_manager::Instance().get_image(),
                           region->id(),
                           1.0,
                           node_name,
                           dbskfg_transform_manager::Instance().
                           get_output_frag_folder());
        }
    }

    // Now with this region find new transforms
    vcl_vector<dbskfg_transform_descriptor_sptr> trans;
    {
        // First lets detect transforms
        dbskfg_detect_transforms transforms(
            local_cgraph_sptr,
            dbskfg_transform_manager::Instance().get_image());

        transforms.transform_affects_region(region,rag_con_ids_);
        trans = transforms.objects();
    }

    dbskfg_composite_graph_sptr cgraph_local = 
        dbskfg_transform_manager::Instance().
        get_cgraph();

    // Source transform needs to be moved over to original graph
    incoming_transform->create_copy(cgraph_local);
    cgraph_local = 0 ;

    for ( unsigned int i=0; i < trans.size() ; ++i)
    {
        vcl_vector<unsigned int> contours_affected = 
            trans[i]->contour_ids_affected();
        
        if ( trans[i]->transform_type_ == dbskfg_transform_descriptor::LOOP)
        {
            bool flag=false;
            for ( unsigned int c=0; c < contours_affected.size() ; ++c)
            {
                
                if ( rag_con_ids_.count(contours_affected[c]) ||
                     contours_affected[c] == 0)
                {
                    flag=true;
                    break;
                }
            }

            if ( flag)
            {
                trans[i]->destroy_transform();
                trans[i]=0;
                continue;
            }
        }

        expand_node_helper(trans[i],
                           contours,
                           contours_affected);
        
        insert_new_node(nodes,cgraph,contours_affected,trans[i]);
      

    }
    incoming_transform = 0;
    region = 0;
  
    for (dbskfg_containment_node::edge_iterator it = this->in_edges_begin();
         it != this->in_edges_end(); ++it)
    {
        (*it)->destroy_transform();
    }
  
    trans.clear();
    local_rag_graph_sptr->ref();
    local_cgraph_sptr->ref();
    local_cgraph_sptr =0;
    local_rag_graph_sptr=0;
    local_cgraph.destroy_cache();
    local_cgraph.clear();
    local_rag_graph.clear();

}


void dbskfg_containment_node::expand_node_implicit_multi(
    vcl_map<unsigned int, 
    vcl_vector<vsol_spatial_object_2d_sptr> >& contours,
    dbskfg_containment_graph& cgraph)
{

    vcl_cout<<vcl_endl;
    vcl_cout<<"********************************"<<vcl_endl;
    vcl_cout<<"Expanding Node: "<<id_<<vcl_endl;
    vcl_cout<<"********************************"<<vcl_endl;

    // **************** Recreate State First/Find Region ******************

    // Lets look at incoming link
    dbskfg_containment_link_sptr incoming_link = *this->in_edges_begin();

    // Grab incoming grouped transform
    dbskfg_transform_descriptor_sptr incoming_transform
        = incoming_link->grouped_transform();

    // Create local copies of everything
    dbskfg_composite_graph local_cgraph(*dbskfg_transform_manager::Instance().
                                        get_cgraph());
    dbskfg_composite_graph_sptr local_cgraph_sptr = &local_cgraph;

    // We have to reset ids
    incoming_transform->create_copy(local_cgraph_sptr);

    dbskfg_rag_graph local_rag_graph;
    local_rag_graph.create_copy(
        local_cgraph_sptr,
        *dbskfg_transform_manager::Instance().get_rag_graph());

    //  Create smart ptrs for both
    dbskfg_rag_graph_sptr local_rag_graph_sptr = &local_rag_graph;        
  
    vcl_vector<unsigned int> region_ids;
    // Apply the actual transform
    {
  
        dbskfg_transformer transformer(incoming_transform,
                                       local_cgraph_sptr,
                                       local_rag_graph_sptr);

        bool status_flag = transformer.status();
        if ( status_flag == false )
        {

            incoming_transform = 0;

            for (dbskfg_containment_node::edge_iterator it 
                     = this->in_edges_begin();
                 it != this->in_edges_end(); ++it)
            {
                (*it)->destroy_transform();
            }
  
            local_rag_graph_sptr->ref();
            local_cgraph_sptr->ref();
            local_cgraph_sptr =0;
            local_rag_graph_sptr=0;
            local_cgraph.destroy_cache();
            local_cgraph.clear();
            local_rag_graph.clear();
            
            return;

        }

        region_ids = transformer.new_rag_nodes();
    }
    
    // Grow only new regions resulting from transform
    {
        dbskfg_region_growing_transforms region_growing(local_rag_graph_sptr);
        region_growing.grow_regions(region_ids);
    }

    // Find region based just on id
    dbskfg_rag_node_sptr root_node = local_rag_graph_sptr->rag_node(
        this->rag_node_wavefront_);
    region_ids.clear();
   
    // If root node cannot be found indicates an error
    if ( root_node == 0 )
    {
        incoming_transform = 0;

        for (dbskfg_containment_node::edge_iterator it 
                 = this->in_edges_begin();
             it != this->in_edges_end(); ++it)
        {
            (*it)->destroy_transform();
        }
  
        local_rag_graph_sptr->ref();
        local_cgraph_sptr->ref();
        local_cgraph_sptr =0;
        local_rag_graph_sptr=0;
        local_cgraph.destroy_cache();
        local_cgraph.clear();
        local_rag_graph.clear();
            
        return;

    }

    // First lets detect transforms
    dbskfg_detect_transforms transforms(
        local_cgraph_sptr,
        dbskfg_transform_manager::Instance().get_image());

    // Create empty set as we dont care, whether part of contour or not
    transforms.transform_affects_region(
        root_node,
        dbskfg_transform_manager::Instance().get_ess());
    
    // Grab transforms for this region
    vcl_vector<dbskfg_transform_descriptor_sptr> trans=
        transforms.objects();

    dbskfg_composite_graph_sptr cgraph_local = 
        dbskfg_transform_manager::Instance().
        get_cgraph();

    // Source transform needs to be moved over to original graph
    incoming_transform->create_copy(cgraph_local);
    cgraph_local = 0 ;    

    unsigned int closed_count=0;

    for ( unsigned int i=0; i < trans.size() ; ++i)
    {

        // Make sure transform is in local context
        if ( !root_node->transform_local_context(trans[i]))
        {
            continue;
        }
        
        bool flag=false;
        if ( trans[i]->transform_type_ == dbskfg_transform_descriptor::LOOP)
        {
            if ( trans[i]->t_type_transform_ )
            {
                for ( unsigned int k = 0; 
                      k < trans[i]->contour_links_to_remove_.size(); 
                      ++k)
                {

                    dbskfg_contour_link* cid = 
                        dynamic_cast<dbskfg_contour_link*>
                        (&(*trans[i]->contour_links_to_remove_[k]));

                    if ( cid->contour_id() == 0  )
                    {
                        flag=true;
                        break;
                    }
                }
            }
            else
            {
                for ( unsigned int k = 0; 
                      k < trans[i]->contours_to_remove_.size(); 
                      ++k)
                {

                    dbskfg_contour_node* cid = 
                        dynamic_cast<dbskfg_contour_node*>
                        (&(*trans[i]->contours_to_remove_[k]));

                    if ( cid->contour_id() == 0  )
                    {
                        flag=true;
                        break;
                    }
                }
            }

        }

        if ( flag )
        {
            continue;
        }

        // Create local copies of everything
        dbskfg_composite_graph local_trans_cgraph(local_cgraph);

        dbskfg_composite_graph_sptr local_trans_cgraph_sptr = 
            &local_trans_cgraph;

        // We have to reset ids
        trans[i]->create_copy(local_trans_cgraph_sptr);

        dbskfg_rag_graph local_trans_rag_graph;
        local_trans_rag_graph.create_copy(
            local_trans_cgraph_sptr,
            local_rag_graph);

        //  Create smart ptrs for both
        dbskfg_rag_graph_sptr local_trans_rag_graph_sptr = 
            &local_trans_rag_graph;

        // Grab new regions
        vcl_vector<unsigned int> new_region_ids;
        {
            
            dbskfg_transformer transformer(trans[i],
                                           local_trans_cgraph_sptr,
                                           local_trans_rag_graph_sptr);

            bool status_flag = transformer.status();
            if ( status_flag == false )
            {

                local_trans_rag_graph_sptr->destroy_map();
                local_trans_rag_graph_sptr->ref();
                local_trans_cgraph_sptr->ref();
                local_trans_cgraph_sptr =0;
                local_trans_rag_graph_sptr=0;
                local_trans_cgraph.destroy_cache();
                local_trans_cgraph.clear();
                local_trans_rag_graph.clear();


                continue;
            }
            new_region_ids = transformer.new_rag_nodes();

        }
   
        // Grow only new regions resulting from transform
        {
            dbskfg_region_growing_transforms region_growing(
                local_trans_rag_graph_sptr);
            region_growing.grow_regions(new_region_ids);
        }

        // Find region
        vcl_vector<dbskfg_rag_node_sptr> region_vector;
        {
   
            local_trans_rag_graph_sptr->rag_node(this->rag_con_ids_,
                                                 trans[i],
                                                 region_vector);
        
        }
 
        // Loop over and find transforms for each region
        for ( unsigned int k=0; k < region_vector.size(); ++k)
        {

            if (!region_vector[k]->is_rag_node_within_image(
                    dbskfg_transform_manager::Instance().get_image()
                    ->ni(),
                    dbskfg_transform_manager::Instance().get_image()
                    ->nj()))
            {
                // // Write out the region
                // vcl_stringstream node_str,depth_str;
                // node_str<<this->id_;
                // depth_str<<this->depth_;

                // vcl_stringstream streamer;
                // streamer<<closed_count;

                // // Create node name
                // vcl_string node_name = dbskfg_transform_manager::Instance().
                //     get_output_prefix()+"_cgraph_node_"+node_str.str()+
                //     "_closed_"+streamer.str()+"_"
                //     +depth_str.str();
                
                // // Print region
                // dbskfg_composite_graph_fileio file;
                // file.write_out(region_vector[k],
                //                dbskfg_transform_manager::Instance().get_image(),
                //                1.0,
                //                node_name,
                //                dbskfg_transform_manager::Instance().
                //                get_output_frag_folder());
                // closed_count++;
                continue;

            }

            // Grab contours affected
            vcl_vector<unsigned int> contours_affected = 
                trans[i]->contour_ids_affected();

            // Expand node helper
            expand_node_helper(trans[i],
                               contours,
                               contours_affected);        

            // Create temp node map
            insert_new_node(cgraph,contours_affected,trans[i],
                            region_vector[k]);


        }

        // Loop over regions and set to null
        for ( unsigned int k=0; k < region_vector.size(); ++k)
        {
            region_vector[k]=0;
        }
   
        local_trans_rag_graph_sptr->destroy_map();
        local_trans_rag_graph_sptr->ref();
        local_trans_cgraph_sptr->ref();
        local_trans_cgraph_sptr =0;
        local_trans_rag_graph_sptr=0;
        local_trans_cgraph.destroy_cache();
        local_trans_cgraph.clear();
        local_trans_rag_graph.clear();
    }


    incoming_transform = 0;

    for (dbskfg_containment_node::edge_iterator it = this->in_edges_begin();
         it != this->in_edges_end(); ++it)
    {
        (*it)->destroy_transform();
    }
  

    local_rag_graph_sptr->ref();
    local_cgraph_sptr->ref();
    local_cgraph_sptr =0;
    local_rag_graph_sptr=0;
    local_cgraph.destroy_cache();
    local_cgraph.clear();
    local_rag_graph.clear();

}


void dbskfg_containment_node::expand_root_node(
    vcl_map<unsigned int,vcl_vector<vsol_spatial_object_2d_sptr> >& 
        contours,
    dbskfg_containment_graph& cgraph,
    dbskfg_rag_node_sptr root_node)
{

    vcl_cout<<vcl_endl;
    vcl_cout<<"********************************"<<vcl_endl;
    vcl_cout<<"Expanding Node: "<<id_<<vcl_endl;
    vcl_cout<<"********************************"<<vcl_endl;
    
    // First lets detect transforms
    dbskfg_detect_transforms transforms(
        &(*dbskfg_transform_manager::Instance().
          get_cgraph()),
        dbskfg_transform_manager::Instance().get_image());

    // Create empty set as we dont care, whether part of contour or not
    transforms.transform_affects_region(
        root_node,
        dbskfg_transform_manager::Instance().get_ess());
    
    // Grab transforms for this region
    vcl_vector<dbskfg_transform_descriptor_sptr> trans=
        transforms.objects();

    unsigned int closed_count=0;

    for ( unsigned int i=0; i < trans.size() ; ++i)
    {

        // Make sure transform is in local context
        if ( !root_node->transform_local_context(trans[i]))
        {
            continue;
        }
       
        // Create local copies of everything
        dbskfg_composite_graph local_cgraph(
            *dbskfg_transform_manager::Instance().
            get_cgraph());

        dbskfg_composite_graph_sptr local_cgraph_sptr = &local_cgraph;

        // We have to reset ids
        trans[i]->create_copy(local_cgraph_sptr);

        dbskfg_rag_graph local_rag_graph;
        local_rag_graph.create_copy(
            local_cgraph_sptr,
             *dbskfg_transform_manager::Instance().get_rag_graph());

        //  Create smart ptrs for both
        dbskfg_rag_graph_sptr local_rag_graph_sptr = &local_rag_graph;        

        // Grab new regions
        vcl_vector<unsigned int> new_region_ids;
        {
            
            dbskfg_transformer transformer(trans[i],
                                           local_cgraph_sptr,
                                           local_rag_graph_sptr);

            new_region_ids = transformer.new_rag_nodes();

        }
   
        // Grow only new regions resulting from transform
        {
            dbskfg_region_growing_transforms region_growing(
                local_rag_graph_sptr);
            region_growing.grow_regions(new_region_ids);
        }

        // Find region
        vcl_vector<dbskfg_rag_node_sptr> region_vector;
        {
   
            local_rag_graph_sptr->rag_node(this->rag_con_ids_,
                                           trans[i],
                                           region_vector);
        
        }

        dbskfg_composite_graph_sptr cgraph2 =
            dbskfg_transform_manager::Instance().get_cgraph();
      
        // We have to reset ids
        trans[i]->create_copy(cgraph2);

        // Loop over and find transforms for each region
        for ( unsigned int k=0; k < region_vector.size(); ++k)
        {
            if (!region_vector[k]->is_rag_node_within_image(
                    dbskfg_transform_manager::Instance().get_image()
                    ->ni(),
                    dbskfg_transform_manager::Instance().get_image()
                    ->nj()))
            {

                // // Write out the region
                // vcl_stringstream node_str,depth_str;
                // node_str<<this->id_;
                // depth_str<<this->depth_;

                // vcl_stringstream streamer;
                // streamer<<closed_count;

                // // Create node name
                // vcl_string node_name = dbskfg_transform_manager::Instance().
                //     get_output_prefix()+"_cgraph_node_"+node_str.str()+
                //     "_closed_"+streamer.str()+"_"
                //     +depth_str.str();
                
                // // Print region
                // dbskfg_composite_graph_fileio file;
                // file.write_out(region_vector[k],
                //                dbskfg_transform_manager::Instance().get_image(),
                //                1.0,
                //                node_name,
                //                dbskfg_transform_manager::Instance().
                //                get_output_frag_folder());
                // closed_count++;
                continue;
            }

            // Grab contours affected
            vcl_vector<unsigned int> contours_affected = 
                trans[i]->contour_ids_affected();
            
            // Expand node helper
            expand_node_helper(trans[i],
                               contours,
                               contours_affected);        

            // Create temp node map
            insert_new_node(cgraph,contours_affected,trans[i],
                            region_vector[k]);


        }

        // Loop over regions and set to null
        for ( unsigned int k=0; k < region_vector.size(); ++k)
        {
            region_vector[k]=0;
        }
   
        local_rag_graph_sptr->destroy_map();
        local_rag_graph_sptr->ref();
        local_cgraph_sptr->ref();
        local_cgraph_sptr =0;
        local_rag_graph_sptr=0;
        local_cgraph.destroy_cache();
        local_cgraph.clear();
        local_rag_graph.clear();
    }



}

void dbskfg_containment_node::expand_node_helper(
    const dbskfg_transform_descriptor_sptr& transform,
    vcl_map<unsigned int, 
    vcl_vector<vsol_spatial_object_2d_sptr> >& contours,
    vcl_vector<unsigned int>& contours_affected)
{ 
    vcl_map<vcl_string,unsigned int>& gap_map = 
        dbskfg_transform_manager::Instance().gap_map();

    // Determine transform type
    dbskfg_transform_descriptor::TransformType ttype =
        transform->transform_type_;
        
    // Get next id
    unsigned int next_id = contours.size();

    if ( ttype == dbskfg_transform_descriptor::GAP )
    {
        vcl_vector<vsol_spatial_object_2d_sptr> 
            gap_contours = transform->new_contours_spatial_objects_;
            
        vcl_pair<vcl_string,vcl_string> gap_strings =
            transform->gap_string();

        if ( gap_map.count(gap_strings.first) == 0 && 
             gap_map.count(gap_strings.second) == 0 )
        {
            for ( unsigned int c=0; c < gap_contours.size() ; ++c)
            {
                contours[next_id].push_back(gap_contours[c]);

            }

            transform->gap_id_= next_id;
            gap_map[gap_strings.first]=next_id;
            gap_map[gap_strings.second]=next_id;
        }
        else
        {
            next_id = gap_map[gap_strings.first];
        }
        contours_affected.push_back(next_id);

    }

}
   
void dbskfg_containment_node::insert_new_node( 
    vcl_map<unsigned int,dbskfg_containment_node_sptr>& nodes,
    dbskfg_containment_graph& cgraph,
    const vcl_vector<unsigned int>& contours_affected,
    const dbskfg_transform_descriptor_sptr& transform)
{

    // Determine transform type
    dbskfg_transform_descriptor::TransformType ttype =
        transform->transform_type_;

    dbskfg_containment_node_sptr next_node(0);
    dbskfg_containment_link_sptr link(0);
    dbskfg_containment_node_sptr fold_node(0);
    bool flag(false);
    vcl_map<unsigned int,dbskfg_containment_node_sptr>::iterator nit;
    for ( nit = nodes.begin() ; nit != nodes.end() ; ++nit)
    {
       
        flag = this->nodes_merge((*nit).second,contours_affected,ttype);
        if ( flag == true )
        {   fold_node = (*nit).second;  
            break;
        }    
    }
  

    // Determine next node
    if ( flag == true )
    {
        next_node = fold_node;
    }
    else
    {
        if ( cgraph_ && rag_graph_)
        {
            next_node = new dbskfg_containment_node(cgraph.next_available_id(),
                                                    *cgraph_,
                                                    *rag_graph_,
                                                    depth_+1);
        }
        else
        {
            next_node = new dbskfg_containment_node(cgraph.next_available_id(),
                                                    this->rag_node_id_,
                                                    depth_+1);
        }

        cgraph.add_vertex(next_node);
    }

    nodes[next_node->id()]=next_node;

    link = new dbskfg_containment_link(this,
                                       next_node,
                                       transform);

    // If explicit do this
    if ( cgraph_ == 0  && rag_graph_ == 0  )
    {
        link->determine_group_transform();
    }

    link->set_contours_affected(contours_affected);
    cgraph.add_edge(link);
        
    this->add_outgoing_edge(link);
    next_node->add_incoming_edge(link);
    next_node->update_attributes();
    
    next_node->prob_fragment_ = link->cost()*this->prob_fragment_;
}

   
void dbskfg_containment_node::insert_new_node( 
    dbskfg_containment_graph& cgraph,
    const vcl_vector<unsigned int>& contours_affected,
    const dbskfg_transform_descriptor_sptr& transform,
    dbskfg_rag_node_sptr rag_node)
{

    // Determine transform type
    dbskfg_transform_descriptor::TransformType ttype =
        transform->transform_type_;

    // Create attr string

    // This is the node we are going to expand
    // We want to make a copy here not reference! due to seeing what node is
    vcl_map<unsigned int,bool> node_to_expand_attr = attr_; 

    // If gap update map
    if ( ttype == dbskfg_transform_descriptor::GAP )
    {
        node_to_expand_attr[contours_affected.back()]=true;
    }
    else
    {
        for ( unsigned int k=0; k < contours_affected.size() ; ++k)
        {
            node_to_expand_attr[contours_affected[k]]=false;

        }
    }

    vcl_set<vcl_string> wavefront;
    rag_node->wavefront_string(wavefront);

    dbskfg_containment_node_sptr fold_node = 
        cgraph.node_merge(node_to_expand_attr,wavefront);        
    dbskfg_containment_link_sptr link(0);
    dbskfg_containment_node_sptr next_node(0);

    vgl_polygon<double> node_poly;
        rag_node->fragment_boundary(node_poly);
    double distance = dbskfg_utilities::jacard_distance(node_poly,
                                                        this->poly_);

    if ( fold_node )
    {
        next_node = fold_node;
    }
    else
    {
        if ( cgraph_ && rag_graph_)
        {
            next_node = new dbskfg_containment_node(cgraph.next_available_id(),
                                                    *cgraph_,
                                                    *rag_graph_,
                                                    depth_+1);
        }
        else
        {
            next_node = new dbskfg_containment_node(cgraph.next_available_id(),
                                                    rag_node->id(),
                                                    depth_+1);
        }

        // Write out the region
        vcl_stringstream node_str,depth_str;
        node_str<<next_node->id_;
        depth_str<<next_node->depth_;

        // Create node name
        vcl_string node_name = dbskfg_transform_manager::Instance().
            get_output_prefix()+"_cgraph_node_"+node_str.str()+"_"
            +depth_str.str();
        
        if ( rag_node->contour_ratio() >= 0.4)
        {
            // Print region
            dbskfg_composite_graph_fileio file;
            file.write_out(rag_node,
                           dbskfg_transform_manager::Instance().get_image(),
                           1.0,
                           node_name,
                           dbskfg_transform_manager::Instance().
                           get_output_frag_folder());
            next_node->poly_ = node_poly;

        }
        else
        {
            next_node->poly_ = this->poly_;
        }

        cgraph.add_vertex(next_node);
    }


    link = new dbskfg_containment_link(this,
                                       next_node,
                                       transform);
    // If implicit do this
    link->determine_group_transform();
    link->set_distance(distance);
    link->set_contours_affected(contours_affected);
    cgraph.add_edge(link);
        
    this->add_outgoing_edge(link);
   
    vcl_set<unsigned int> rag_ids;
    rag_node->rag_contour_ids(rag_ids);
    
    next_node->add_incoming_edge(link);
    next_node->update_attributes();
    next_node->set_rag_con_ids(rag_ids);
    next_node->set_rag_wavefront(wavefront);
    next_node->prob_fragment_ = link->cost()*this->prob_fragment_;

    if ( fold_node == 0 )
    {
        cgraph.insert_node(next_node);

    }
}

void dbskfg_containment_node::print_rag_node(ExpandType expansion)
{
    if ( expansion == IMPLICIT )
    {
        // Lets look at incoming link
        dbskfg_containment_link_sptr incoming_link = *this->in_edges_begin();

        // Set rag contour ids to whatever previous link was
        this->rag_con_ids_ = incoming_link->source()->rag_con_ids_;

        // Grab incoming grouped transform
        dbskfg_transform_descriptor_sptr incoming_transform
            = incoming_link->grouped_transform();

        // Create local copies of everything
        dbskfg_composite_graph local_cgraph(
            *dbskfg_transform_manager::Instance().
            get_cgraph());
        dbskfg_composite_graph_sptr local_cgraph_sptr = &local_cgraph;

        // We have to reset ids
        incoming_transform->create_copy(local_cgraph_sptr);
     
        dbskfg_rag_graph local_rag_graph;
        local_rag_graph.create_copy(
            local_cgraph_sptr,
            *dbskfg_transform_manager::Instance().get_rag_graph());

        //  Create smart ptrs for both
        dbskfg_rag_graph_sptr local_rag_graph_sptr = &local_rag_graph;        
    
        bool region_invalid = false;
        // Apply the actual transform
        {
  
            dbskfg_transformer transformer(incoming_transform,
                                           local_cgraph_sptr,
                                           local_rag_graph_sptr);

            region_invalid = transformer.rag_deleted(this->rag_node_id_);
        }

        // Grow just this region
        dbskfg_rag_node_sptr region(0);
        if ( !region_invalid ) 
        {
            dbskfg_region_growing_transforms region_growing(
                local_rag_graph_sptr);
            region = region_growing.grow_region(this->rag_node_id_);
        }
        else
        {
            // Graph old region id
            unsigned int old_region_id =
                incoming_link->source()->rag_node_id_;

            dbskfg_region_growing_transforms region_growing(
                local_rag_graph_sptr);
            region_growing.grow_regions(old_region_id);
            region = local_rag_graph_sptr->rag_node(old_region_id,
                                                    this->rag_con_ids_);
            this->rag_node_id_ = region->id();
        }
        local_rag_graph_sptr->destroy_map();

        // Write out the region
        vcl_stringstream node_str,depth_str;
        node_str<<id_;
        depth_str<<depth_;

        // Create wavefront set
        {
            vcl_map<unsigned int, dbskfg_shock_node*> wavefront
                = region->get_wavefront();

            vcl_map<unsigned int, dbskfg_shock_node*>::iterator it;
            for ( it = wavefront.begin(); it != wavefront.end() ; ++it)
            {
                vgl_point_2d<double> point = (*it).second->pt();
                vcl_stringstream stream;
                stream<<point;
                rag_node_wavefront_.insert(stream.str());
            }
        }

        // Print region
        // Look at incoming set
        vcl_set<vcl_string> incoming_node_set = incoming_link->source()->
            rag_node_wavefront_;
        if( !vcl_includes(this->rag_node_wavefront_.begin(),
                          this->rag_node_wavefront_.end(),
                          incoming_node_set.begin(),
                          incoming_node_set.end()))
        {
            // Create node name
            vcl_string node_name = dbskfg_transform_manager::Instance().
                get_output_prefix()+"_cgraph_node_"+node_str.str()+
                "_"+depth_str.str();

            if ( region->is_rag_node_within_image(
                     dbskfg_transform_manager::Instance().get_image()->ni(),
                     dbskfg_transform_manager::Instance().get_image()->nj()))
            {
           
                // Print region
                dbskfg_composite_graph_fileio file;
                file.write_out(local_rag_graph_sptr,
                               dbskfg_transform_manager::Instance().get_image(),
                               region->id(),
                               1.0,
                               node_name,
                               dbskfg_transform_manager::Instance().
                               get_output_frag_folder());

            }
        }

        local_rag_graph_sptr->ref();
        local_cgraph_sptr->ref();
        local_cgraph_sptr =0;
        local_rag_graph_sptr=0;
        local_cgraph.destroy_cache();
        local_cgraph.clear();
        local_rag_graph.clear();
    }
    else
    {
        // Lets look at incoming link
        dbskfg_containment_link_sptr incoming_link = *this->in_edges_begin();

        // Set rag contour ids to whatever the previous link was
        this->rag_con_ids_ = incoming_link->source()->rag_con_ids_;

        // Graph old region id
        unsigned int old_region_id =
            incoming_link->source()->rag_node_id_;
 
        // Grab incoming link transform
        dbskfg_transform_descriptor_sptr incoming_transform
            = incoming_link->transform();
   
        // We have to reset ids
        incoming_transform->create_copy(cgraph_);
   
        // Apply the actual transform
        {
  
            dbskfg_transformer transformer(incoming_transform,
                                           cgraph_,
                                           rag_graph_);
        }

        // Grow the regions
        {
            dbskfg_region_growing_transforms region_growing(rag_graph_);
            region_growing.grow_regions(old_region_id);
        }

        // Now find region after transform
        dbskfg_rag_node_sptr region = rag_graph_->rag_node(old_region_id,
                                                           this->rag_con_ids_);

        // Set region of node
        this->rag_node_id_ = region->id();

        // Write out the region
        vcl_stringstream node_str,depth_str;
        node_str<<id_;
        depth_str<<depth_;

        // Create wavefront set
        {
            vcl_map<unsigned int, dbskfg_shock_node*> wavefront
                = region->get_wavefront();

            vcl_map<unsigned int, dbskfg_shock_node*>::iterator it;
            for ( it = wavefront.begin(); it != wavefront.end() ; ++it)
            {
                vgl_point_2d<double> point = (*it).second->pt();
                vcl_stringstream stream;
                stream<<point;
                rag_node_wavefront_.insert(stream.str());
            }
        }

        // Print region
        // Look at incoming set
        vcl_set<vcl_string> incoming_node_set = incoming_link->source()->
            rag_node_wavefront_;
        if( !vcl_includes(this->rag_node_wavefront_.begin(),
                          this->rag_node_wavefront_.end(),
                          incoming_node_set.begin(),
                          incoming_node_set.end()))
        {
            // Create node name
            vcl_string node_name = dbskfg_transform_manager::Instance().
                get_output_prefix()+"_cgraph_node_"+node_str.str()+"_"
                +depth_str.str();

            if ( region->is_rag_node_within_image(
                     dbskfg_transform_manager::Instance().get_image()->ni(),
                     dbskfg_transform_manager::Instance().get_image()->nj()))
            {
                // Print region
                dbskfg_composite_graph_fileio file;
                file.write_out(rag_graph_,
                               dbskfg_transform_manager::Instance().get_image(),
                               region->id(),
                               1.0,
                               node_name,
                               dbskfg_transform_manager::Instance().
                               get_output_frag_folder());
            }
        }

        cgraph_->clear();
        cgraph_=0;
        rag_graph_->clear();
        rag_graph_=0;


    }
        


}
