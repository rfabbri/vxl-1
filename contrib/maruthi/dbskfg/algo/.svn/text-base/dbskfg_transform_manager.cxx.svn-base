// This is brcv/shp/dbskfg/algo/dbskfg_transform_manager.cxx

//:
// \file
#include <dbskfg/algo/dbskfg_transform_manager.h>
#include <dbskfg/algo/dbskfg_transform_descriptor.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_composite_node.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <dbskfg/dbskfg_contour_link.h>


#include <vil3d/vil3d_load.h>
#include <vil/vil_load.h>
#include <vil3d/vil3d_tricub_interp.h>
#include <vcl_algorithm.h>
#include <vcl_fstream.h>
#include <vcl_set.h>
#include <dbskfg/dbskfg_utilities.h>
#include <dbxml/dbxml_algos.h>

dbskfg_transform_manager::dbskfg_transform_manager()
    :image_(0),
     cgraph_(0),
     rag_graph_(0),
     threshold_(0),
     ess_(0),
     alpha_(0),
     dist_volume_(0),
     out_folder_(),
     out_prefix_(),
     logistic_beta0_(1.0),
     logistic_beta1_(0.0)

{


}

dbskfg_transform_manager::~dbskfg_transform_manager()
{
    destroy_singleton();
}

void dbskfg_transform_manager::destroy_singleton()
{
   
    // Assign id to all transforms
    for ( unsigned int t=0; t < transforms_.size() ; ++t)
    {
        transforms_[t]=0;
    }
    transforms_.clear();

    loop_map_.clear();
    loop_contour_map_.clear();
    gap_map_.clear();
    old_gap_map_.clear();

    image_=0;
    cgraph_=0;
    rag_graph_=0;

}

void dbskfg_transform_manager::update_transforms_conflicts()
{

    // Assign id to all transforms
    for ( unsigned int t=0; t < transforms_.size() ; ++t)
    {
        transforms_[t]->trim_transform();

        transforms_[t]->id_ = t;
       
        if ( transforms_[t]->transform_type_ == dbskfg_transform_descriptor::
             LOOP)
        {
            for ( unsigned int d=0; d < transforms_[t]->
                      contour_links_to_remove_.size(); ++d)

            {
                dbskfg_composite_link_sptr link =
                    transforms_[t]->
                    contour_links_to_remove_[d];
                    
                loop_segment_map_[link->id()]=t;
            }

            if ( transforms_[t]->loop_endpoints_.size() )
            {
                vcl_stringstream sstream1;
                sstream1<<transforms_[t]->loop_endpoints_[0]->pt();
        
                vcl_stringstream sstream2;
                sstream2<<transforms_[t]->loop_endpoints_[1]->pt();

                if ( 
                    transforms_[t]->loop_endpoints_[0]
                    ->get_composite_degree() == 1 )
                {
                    loop_map_[sstream1.str()]=t;
                }

                if ( 
                    transforms_[t]->loop_endpoints_[1]
                    ->get_composite_degree() == 1 )
                {
                    loop_map_[sstream2.str()]=t;
                }

                vcl_pair<vcl_string,vcl_string> pair1 =
                    vcl_make_pair(sstream1.str(),sstream2.str());
                vcl_pair<vcl_string,vcl_string> pair2 =
                    vcl_make_pair(sstream2.str(),sstream1.str());

                loop_end_map_[pair1]=t;
                loop_end_map_[pair2]=t;
            }
            
            vcl_vector<unsigned int> contour_ids=
                transforms_[t]->contour_ids_affected();

            for (unsigned int k=0; k < contour_ids.size() ; ++k)
            {
                
                loop_contour_map_[contour_ids[k]]= t;

            }
        
        }
        else
        {

            vcl_pair<vcl_string,vcl_string> gap_strings =
                transforms_[t]->gap_string();
            old_gap_map_[gap_strings.first]=t;
            old_gap_map_[gap_strings.second]=t;

            transforms_[t]->all_gaps_.push_back(transforms_[t]->gap_);
        }
    }



}

void dbskfg_transform_manager::read_in_training_data(vcl_string filename)
{

    vcl_ifstream file (filename.c_str(), 
                       vcl_ios::in|vcl_ios::binary|vcl_ios::ate);
    double* memblock(0);
    if (file.is_open())
    {
        vcl_ifstream::pos_type size = file.tellg();
        memblock = new double[size/sizeof(double)];
        file.seekg (0, vcl_ios::beg);
        file.read ((char *) memblock, size);
        file.close();

        // Read in dimensions
        unsigned int ni=memblock[0];
        unsigned int nj=memblock[1];
        unsigned int nk=memblock[2];

        vcl_cout<<"Reading in a "<<ni<<" by "<<nj<<" by "<< nk 
                <<" volume of distances"
                <<vcl_endl;

        dist_volume_.set_size(ni,nj,nk,1);
        unsigned int index=3;

        for (unsigned k=0;k<nk;++k)
        {
            for (unsigned j=0;j<nj;++j)
            {
                for (unsigned i=0;i<ni;++i)
                {
                    double value = memblock[index];
                    dist_volume_(i,j,k,0)=value;
                    index++;
                }
            }
        }

        delete[] memblock;
        memblock=0;
    }

    
}

double dbskfg_transform_manager::transform_probability(
    double gamma_norm, double k0_norm,double length)
{

    if ( dist_volume_ == 0 )
    {
        return 0.0;
    }

    // Convert
    double gamma_converted  = (gamma_norm+15.0)*2.0;
    double k0_converted     = (k0_norm+15.0)*2.0;
    double length_converted = length*2.0;

    double distance = vil3d_trilin_interp_safe(k0_converted,
                                               gamma_converted,
                                               length_converted,
                                               dist_volume_.origin_ptr(),
                                               dist_volume_.ni(),
                                               dist_volume_.nj(),
                                               dist_volume_.nk(),
                                               dist_volume_.istep(),
                                               dist_volume_.jstep(),
                                               dist_volume_.kstep());
 
    double prob = 1.0-(1.0/(1.0+vcl_exp(distance*logistic_beta0_+
                                    logistic_beta1_)));
    
    return prob;
}

void dbskfg_transform_manager::find_transform(
    vcl_vector<dbskfg_composite_link_sptr>&
    contour_links_to_remove,
    dbskfg_transform_descriptor_sptr& grouped_transform)
{

    // Local set
    vcl_set<unsigned int> local_set;

    for ( unsigned int d=0; d < contour_links_to_remove.size(); ++d)

    {
        dbskfg_composite_link_sptr link =
            contour_links_to_remove[d];
 
        local_set.insert(loop_segment_map_[link->id()]);
    }
      

    vcl_set<unsigned int>::iterator it;
    for ( it = local_set.begin() ; it != local_set.end() ; ++it)
    {

        grouped_transform->group_transform(
            *transforms_[*it]);
    }


}

void dbskfg_transform_manager::start_binary_file(vcl_string binary_file_output)
{
    output_binary_file_ = binary_file_output;

    vcl_ofstream output_binary_file;
    output_binary_file.open(output_binary_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);

    double size_x = image_->ni();
    double size_y = image_->nj();

    output_binary_file.write(reinterpret_cast<char *>(&size_x),
                              sizeof(double));
    output_binary_file.write(reinterpret_cast<char *>(&size_y),
                              sizeof(double));

    output_binary_file.close();


}

void dbskfg_transform_manager::start_region_file(vcl_string binary_file_output)
{
    output_region_file_ = binary_file_output;

    vcl_ofstream output_region_file;
    output_region_file.open(output_region_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);

    double size_x = image_->ni();
    double size_y = image_->nj();

    output_region_file.write(reinterpret_cast<char *>(&size_x),
                             sizeof(double));
    output_region_file.write(reinterpret_cast<char *>(&size_y),
                             sizeof(double));

    output_region_file.close();


}



void dbskfg_transform_manager::start_xml_file(vcl_string xml_file_output)
{
 
    xml_file_output_ = xml_file_output;
    bxml_data_sptr root     = new bxml_element("medial_fragments");
    bxml_element*  root_elm = dbxml_algos::
        cast_to_element(root,"medial_fragments");
    xml_file_.set_root_element(root_elm);


}


void dbskfg_transform_manager::write_xml_file()
{

   bxml_write(xml_file_output_,xml_file_);
}

void dbskfg_transform_manager::write_output_polygon(vgl_polygon<double>& poly)
{


    vcl_ofstream output_binary_file;
    output_binary_file.open(output_binary_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);
    
    double num_vertices= poly[0].size();
    output_binary_file.write(reinterpret_cast<char *>(&num_vertices),
                              sizeof(double));

    for (unsigned int p = 0; p < poly[0].size(); ++p)
    {
        double xcoord = poly[0][p].x();
        double ycoord = poly[0][p].y();

        output_binary_file.write(reinterpret_cast<char *>(&xcoord),
                                  sizeof(double));
        output_binary_file.write(reinterpret_cast<char *>(&ycoord),
                                  sizeof(double));
        
    
    }

    output_binary_file.close();


}

void dbskfg_transform_manager::write_output_region(
    dbskfg_rag_node_sptr rag_node)
{
    vcl_cout<<"Writing output region"<<vcl_endl;
    vcl_map<unsigned int,dbskfg_composite_link_sptr> all_links;
    
    vcl_map<unsigned int,dbskfg_shock_link*> shock_links=
        rag_node->get_shock_links();
    vcl_map<unsigned int, dbskfg_shock_node*> wavefront =
        rag_node->get_wavefront();

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it = shock_links.begin() ; it != shock_links.end() ; ++it)
    {
        vcl_vector<dbskfg_composite_link_sptr> left_links=
            (*it).second->left_contour_links();
        vcl_vector<dbskfg_composite_link_sptr> right_links=
            (*it).second->right_contour_links();

        dbskfg_composite_node_sptr source=(*it).second->source();
        dbskfg_composite_node_sptr target=(*it).second->target();

        for ( unsigned int left=0; left < left_links.size() ; ++left)
        {
            all_links[left_links[left]->id()]=left_links[left];

            dbskfg_composite_link_sptr link=left_links[left];
                
            // Process source first
            {
                dbskfg_composite_node_sptr csource=link->source();

                // In first
                dbskfg_composite_node::edge_iterator eit= csource
                    ->in_edges_begin();
                for (  ; eit != csource->in_edges_end() ; ++eit )
                {
                    if ( (*eit)->link_type() ==
                         dbskfg_composite_link::CONTOUR_LINK )
                    {
                        all_links[(*eit)->id()]=(*eit);
                            
                    }
                }

                // Out Second
                eit= csource->out_edges_begin();
                for (  ; eit != csource->out_edges_end() ; ++eit )
                {
                    if ( (*eit)->link_type() ==
                         dbskfg_composite_link::CONTOUR_LINK )
                    {
                        all_links[(*eit)->id()]=(*eit);

                    }
                }

            }
               
            // Process target first
            {
                dbskfg_composite_node_sptr ctarget=link->target();

                // In first
                dbskfg_composite_node::edge_iterator eit= ctarget
                    ->in_edges_begin();
                for (  ; eit != ctarget->in_edges_end() ; ++eit )
                {
                    if ( (*eit)->link_type() ==
                         dbskfg_composite_link::CONTOUR_LINK )
                    {
                        all_links[(*eit)->id()]=(*eit);
                            
                    }
                }

                // Out Second
                eit= ctarget->out_edges_begin();
                for (  ; eit != ctarget->out_edges_end() ; ++eit )
                {
                    if ( (*eit)->link_type() ==
                         dbskfg_composite_link::CONTOUR_LINK )
                    {
                        all_links[(*eit)->id()]=(*eit);

                    }
                }

            } 
        }

        for ( unsigned int right=0; right < right_links.size() ; ++right)
        {
            all_links[right_links[right]->id()]=right_links[right];

            dbskfg_composite_link_sptr link=right_links[right];
                
            // Process source first
            {
                dbskfg_composite_node_sptr csource=link->source();

                // In first
                dbskfg_composite_node::edge_iterator eit= csource
                    ->in_edges_begin();
                for (  ; eit != csource->in_edges_end() ; ++eit )
                {
                    if ( (*eit)->link_type() ==
                         dbskfg_composite_link::CONTOUR_LINK )
                    {
                        all_links[(*eit)->id()]=(*eit);
                            
                    }
                }

                // Out Second
                eit= csource->out_edges_begin();
                for (  ; eit != csource->out_edges_end() ; ++eit )
                {
                    if ( (*eit)->link_type() ==
                         dbskfg_composite_link::CONTOUR_LINK )
                    {
                        all_links[(*eit)->id()]=(*eit);

                    }
                }

            }
               
            // Process target first
            {
                dbskfg_composite_node_sptr ctarget=link->target();

                // In first
                dbskfg_composite_node::edge_iterator eit= ctarget
                    ->in_edges_begin();
                for (  ; eit != ctarget->in_edges_end() ; ++eit )
                {
                    if ( (*eit)->link_type() ==
                         dbskfg_composite_link::CONTOUR_LINK )
                    {
                        all_links[(*eit)->id()]=(*eit);
                            
                    }
                }

                // Out Second
                eit= ctarget->out_edges_begin();
                for (  ; eit != ctarget->out_edges_end() ; ++eit )
                {
                    if ( (*eit)->link_type() ==
                         dbskfg_composite_link::CONTOUR_LINK )
                    {
                        all_links[(*eit)->id()]=(*eit);

                    }
                }

            } 
        }

    }
    
    vcl_ofstream output_region_file;
    output_region_file.open(output_region_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);
    

    double num_contours= all_links.size()*4.0+all_links.size();
    output_region_file.write(reinterpret_cast<char *>(&num_contours),
                              sizeof(double));
    vcl_map<unsigned int,dbskfg_composite_link_sptr>::iterator lit;
    for (lit = all_links.begin() ; lit != all_links.end() ; ++lit)
    {

        dbskfg_composite_node_sptr source=(*lit).second->source();
        dbskfg_composite_node_sptr target=(*lit).second->target();
        
        
        double x1_coord = source->pt().x();
        double y1_coord = source->pt().y();

        double x2_coord = target->pt().x();
        double y2_coord = target->pt().y();

        output_region_file.write(reinterpret_cast<char *>(&x1_coord),
                                  sizeof(double));
        output_region_file.write(reinterpret_cast<char *>(&y1_coord),
                                  sizeof(double));

        output_region_file.write(reinterpret_cast<char *>(&x2_coord),
                                  sizeof(double));
        output_region_file.write(reinterpret_cast<char *>(&y2_coord),
                                  sizeof(double));

        dbskfg_contour_link* clink = dynamic_cast<dbskfg_contour_link*>
            (&(*(*lit).second));
        double contour_id = clink->contour_id();
        output_region_file.write(reinterpret_cast<char *>(&contour_id),
                                 sizeof(double));

    }
    output_region_file.close();
}

void dbskfg_transform_manager::write_binary_transforms(
    vcl_string binary_file_output)
{
   

    vcl_ofstream output_binary_file;
    output_binary_file.open(binary_file_output.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);

    double numb_transforms = transforms_.size();

    output_binary_file.write(reinterpret_cast<char *>(&numb_transforms),
                              sizeof(double));

    for ( unsigned int k=0; k < transforms_.size() ; ++k)
    {
        dbskfg_transform_descriptor_sptr transform =
            transforms_[k];

        double gamma  = transform->gamma_;
        double k0     = transform->k0_;
        double length = transform->length_;
        double d      = transform->d_;
        double theta1 = transform->theta1_;
        double theta2 = transform->theta2_;
        double shock_link_found = transform->shock_link_found_;

        // Get euler spiral points
        vcl_vector<vgl_point_2d<double> > points;
        dbskfg_utilities::ess_points(transform,points);

        double npoints = points.size();

        output_binary_file.write(reinterpret_cast<char *>(&gamma),
                                 sizeof(double));
        output_binary_file.write(reinterpret_cast<char *>(&k0),
                              sizeof(double));
        output_binary_file.write(reinterpret_cast<char *>(&length),
                                 sizeof(double));
        output_binary_file.write(reinterpret_cast<char *>(&d),
                              sizeof(double));
        output_binary_file.write(reinterpret_cast<char *>(&theta1),
                                 sizeof(double));
        output_binary_file.write(reinterpret_cast<char *>(&theta2),
                              sizeof(double));
        output_binary_file.write(reinterpret_cast<char *>(&shock_link_found),
                              sizeof(double));
        output_binary_file.write(reinterpret_cast<char *>(&npoints),
                                 sizeof(double));
 
        for (  unsigned int p=0; p < points.size(); ++p)
        {

            double xcoord = points[p].x();
            double ycoord = points[p].y();

            output_binary_file.write(reinterpret_cast<char *>(&xcoord),
                                     sizeof(double));
            output_binary_file.write(reinterpret_cast<char *>(&ycoord),
                                  sizeof(double));
            
        }

    }

    output_binary_file.close();


}
