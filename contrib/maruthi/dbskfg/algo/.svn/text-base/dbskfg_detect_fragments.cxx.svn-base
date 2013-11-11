// This is brcv/shp/dbskfg/algo/dbskfg_detect_fragments.cxx

//:
// \file
#include <dbskfg/algo/dbskfg_detect_fragments.h>
#include <dbxml/dbxml_algos.h>


dbskfg_detect_fragments::dbskfg_detect_fragments
( 
    vcl_string input_match_file
)
{
    load_match_file(input_match_file);
}

dbskfg_detect_fragments::~dbskfg_detect_fragments()
{
}

void dbskfg_detect_fragments::detect(double threshold,
                                     double N_query_frags,
                                     double K_model_frags,
                                     vsol_box_2d_sptr& bounding_box)
{

    vcl_map<unsigned int,vcl_vector<vcl_pair<double,unsigned int> > >::
        iterator it;

    // For each row hols the number below threshold
    vcl_map<unsigned int,vcl_vector<unsigned int> > rows_that_match;
    for ( it = sim_matrix_.begin() ; it != sim_matrix_.end() ; ++it)
    {
        vcl_vector<vcl_pair<double,unsigned int> > row = (*it).second;
        
        for ( unsigned int i=0 ; i < row.size() ; ++i)
        {
            double cost = row[i].first;
            if ( cost < threshold )
            {
                rows_that_match[(*it).first].push_back(row[i].second);

            }
        }
        
        if ( rows_that_match[(*it).first].size() < N_query_frags )
        {
            rows_that_match.erase((*it).first); 
        }
        

    }
    
    bool flag=rows_that_match.size() >= K_model_frags ? true : false;
   
    if ( flag)
    {
        bounding_box = new vsol_box_2d();
        vcl_map<unsigned int,vcl_vector<unsigned int> >::iterator rit;
        for ( rit = rows_that_match.begin() ; rit != rows_that_match.end() ; 
              ++rit)
        {
            vcl_vector<unsigned int> matches=(*rit).second;
            vcl_cout<<"Model id "
                    <<(*rit).first
                    <<" has "
                    <<matches.size()
                    <<" matches that meet detection criteria "
                    <<vcl_endl;

            for ( unsigned int i=0; i < matches.size() ; ++i)
            {

                bounding_box->add_point(bbox_[matches[i]].get_min_x(),
                                        bbox_[matches[i]].get_min_y());
                bounding_box->add_point(bbox_[matches[i]].get_max_x(),
                                        bbox_[matches[i]].get_max_y());

            }
        }
    }

}

void dbskfg_detect_fragments::load_match_file(vcl_string input_sim_matrix)
{
    // read document
    bxml_document doc_in = bxml_read(input_sim_matrix);
    bxml_data_sptr root_xml = doc_in.root_element();

    load_bbox(root_xml);
    load_matrix(root_xml);

}

void dbskfg_detect_fragments::load_bbox(const bxml_data_sptr& root_xml)
{
    vcl_vector<bxml_data_sptr> bounding_boxes; // contour_points

    // Lets find bounding boxs
    bxml_element* head=dbxml_algos::find_by_name(root_xml,"bbox");
    dbxml_algos::find_all_elems_by_name(head,"bbox_fragment",bounding_boxes);

    //Loop over bounding boxes
    for ( unsigned int b=0; b < bounding_boxes.size() ; ++b)
    {
        bxml_element* data_elm = static_cast<bxml_element*>
            ((bounding_boxes[b]).ptr());
        
        // Grab all attributes
        unsigned int id;
        double xmin,ymin,xmax,ymax;
        
        data_elm->get_attribute("id",id);
        data_elm->get_attribute("xmin",xmin);
        data_elm->get_attribute("ymin",ymin);
        data_elm->get_attribute("xmax",xmax);
        data_elm->get_attribute("ymax",ymax);
        
        vsol_box_2d query_bbox;
        query_bbox.add_point(xmin,ymin);
        query_bbox.add_point(xmax,ymax);

        bbox_[id]=query_bbox;

    }

    
}

void dbskfg_detect_fragments::load_matrix(const bxml_data_sptr& root_xml)
{

    vcl_vector<bxml_data_sptr> one_to_many; // contour_points
    
    // Lets find bounding boxs
    bxml_element* head=dbxml_algos::find_by_name(root_xml,"costs");
    dbxml_algos::find_all_elems_by_name(head,"one_to_many",one_to_many);

    //Loop over rows of matrix
    for ( unsigned int rows=0; rows < one_to_many.size() ; ++rows)
    {
        // Grab id
        bxml_element* data_elm = static_cast<bxml_element*>
            ((one_to_many[rows]).ptr());
        
        // Grab all attributes
        unsigned int model_id;
        data_elm->get_attribute("model_id",model_id);

        vcl_vector<bxml_data_sptr> query_matches; // contour_points

        dbxml_algos::find_all_elems_by_name(one_to_many[rows],
                                            "query_match",query_matches);

        for ( unsigned int q=0; q < query_matches.size() ; ++q)
        {
            // Grab cost id 
            bxml_element* query_elm = static_cast<bxml_element*>
                ((query_matches[q]).ptr());
     
            // Grab all attributes
            unsigned int id;
            double cost;
        
            query_elm->get_attribute("query_id",id);
            query_elm->get_attribute("cost",cost);
            
            sim_matrix_[model_id].push_back(vcl_make_pair(cost,id));

        }
        


    }
}
