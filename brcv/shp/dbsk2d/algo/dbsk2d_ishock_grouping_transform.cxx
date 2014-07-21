// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_grouping_transform.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_ishock_grouping_transform.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_convex.h>
#include <vcl_algorithm.h>
#include <dbsk2d/algo/dbsk2d_ishock_transform.h>
#include <dbsk2d/dbsk2d_transform_manager.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>

//: constructor
//: base class for shock grouping_transforms
dbsk2d_ishock_grouping_transform::dbsk2d_ishock_grouping_transform(
    dbsk2d_ishock_graph_sptr intrinsic_shock_graph)
    :ishock_graph_(intrinsic_shock_graph),
     boundary_(intrinsic_shock_graph->boundary())
{
}


void dbsk2d_ishock_grouping_transform::grow_regions()
{

    unsigned int index=0;

    //draw the edges first
    for ( dbsk2d_ishock_graph::edge_iterator curE = 
              ishock_graph_->all_edges().begin();
          curE != ishock_graph_->all_edges().end();
          curE++ ) 
    {
        dbsk2d_ishock_edge* selm = (*curE);
        
        if (selm->is_a_contact() )
        {
            continue;

        }

        if ( shock_from_endpoint(selm))
        {
            continue;
        }

        if ( visited_edges_.count(selm->id()) == 0 )
        {
            visited_edges_[selm->id()]="temp";
            region_nodes_[index].push_back(selm);
            if ( selm->pSNode()->degree() > 1 )
            {
                expand_wavefront(selm->pSNode(),index);
            }
            if ( selm->cSNode())
            {
                expand_wavefront(selm->cSNode(),index);
            }
            index++;
        }

    }

    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >::iterator it;
    for ( it = region_nodes_.begin() ; it != region_nodes_.end() ; ++it)
    {

        vcl_sort((*it).second.begin(),(*it).second.end(),
                 dbsk2d_ishock_grouping_transform::sort_edges);
    }

    vcl_cout<<"Number of Regions: "<<region_nodes_.size()<<vcl_endl;
    // vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >::iterator it;
    // for ( it = region_nodes_.begin() ; it != region_nodes_.end() ; ++it)
    // {
    //     vcl_vector<dbsk2d_ishock_edge*> edges=(*it).second;
    //     vcl_cout<<"Region id: "<<(*it).first<<" with edges: ";
    //     for ( unsigned int i =0 ;i < edges.size()  ; ++i)
    //     {
    //         vcl_cout<<edges[i]->id()<<" ";
    //     }
    //     vcl_cout<<vcl_endl;

    // double con_ratio=contour_ratio(26);
    // vcl_cout<<"Contour Ratio: "<<con_ratio<<vcl_endl;

    // vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >::iterator nit;
    // for ( nit = outer_shock_nodes_.begin() ; 
    //       nit != outer_shock_nodes_.end() ; ++nit)
    // {
    //     vcl_vector<dbsk2d_ishock_node*> nodes=(*nit).second;
    //     vcl_cout<<"Region id: "<<(*nit).first<<" with outer nodes: ";
    //     for ( unsigned int i =0 ;i < nodes.size()  ; ++i)
    //     {
    //         vcl_cout<<nodes[i]->id()<<" ";
    //     }
    //     vcl_cout<<vcl_endl;
    // }
}

void dbsk2d_ishock_grouping_transform::grow_transformed_regions(int id)
{

    unsigned int index=0;

    vcl_list<dbsk2d_ishock_edge*>::reverse_iterator curE;

    //draw the edges first
    for ( curE = ishock_graph_->all_edges().rbegin();
          curE != ishock_graph_->all_edges().rend();
          curE++ ) 
    {
        dbsk2d_ishock_edge* selm = (*curE);
        
        if (selm->is_a_contact() )
        {
            continue;

        }

        if ( shock_from_endpoint(selm))
        {
            continue;
        }

        if ( selm->id() <= id )
        {
            break;
        }

        if ( visited_edges_.count(selm->id()) == 0 )
        {
            visited_edges_[selm->id()]="temp";
            region_nodes_[index].push_back(selm);
            if ( selm->pSNode()->degree() > 1 )
            {
                expand_wavefront(selm->pSNode(),index);
            }
            if ( selm->cSNode())
            {
                expand_wavefront(selm->cSNode(),index);
            }
            index++;
        }

    }

    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >::iterator it;
    for ( it = region_nodes_.begin() ; it != region_nodes_.end() ; ++it)
    {

        vcl_sort((*it).second.begin(),(*it).second.end(),
                 dbsk2d_ishock_grouping_transform::sort_edges);
    }

 
   
}

void dbsk2d_ishock_grouping_transform::extract_polygon(
    dbsk2d_ishock_edge* edge,vgl_polygon<double>& poly)
{

    dbsk2d_ishock_node* source_node=edge->pSNode();
    dbsk2d_ishock_node* target_node=edge->cSNode();

    // create polygon
    vgl_polygon<double> temp_poly(1);
         
    //Line/Line
    if ( edge->lBElement()->is_a_line() && 
         edge->rBElement()->is_a_line())
    {

        temp_poly.push_back(source_node->origin());
        temp_poly.push_back(edge->getLFootPt(edge->sTau()));
        temp_poly.push_back(edge->getLFootPt(edge->eTau()));
        temp_poly.push_back(target_node->origin());
        temp_poly.push_back(edge->getRFootPt(edge->eTau()));
        temp_poly.push_back(edge->getRFootPt(edge->sTau()));

    }
    //Left line/Right Point
    else if ( edge->lBElement()->is_a_line() && 
              edge->rBElement()->is_a_point())
    {

        dbsk2d_ishock_bpoint* right_bpoint= 
            (dbsk2d_ishock_bpoint*) edge->rBElement();

        temp_poly.push_back(source_node->origin());
        temp_poly.push_back(edge->getLFootPt(edge->sTau()));
        temp_poly.push_back(edge->getLFootPt(edge->eTau()));
        temp_poly.push_back(target_node->origin());
        temp_poly.push_back(right_bpoint->pt());


    }
    //Right Line/Left Point
    else if ( edge->lBElement()->is_a_point() && 
              edge->rBElement()->is_a_line())
    {       
      
        dbsk2d_ishock_bpoint* left_bpoint = 
            (dbsk2d_ishock_bpoint*) edge->lBElement();

        temp_poly.push_back(source_node->origin());
        temp_poly.push_back(edge->getRFootPt(edge->sTau()));
        temp_poly.push_back(edge->getRFootPt(edge->eTau()));
        temp_poly.push_back(target_node->origin());
        temp_poly.push_back(left_bpoint->pt());

    }
    else 
    {

                           
        dbsk2d_ishock_bpoint* left_bpoint = 
            (dbsk2d_ishock_bpoint*) edge->lBElement();
        dbsk2d_ishock_bpoint* right_bpoint = 
            (dbsk2d_ishock_bpoint*) edge->rBElement();

        temp_poly.push_back(source_node->origin());
        temp_poly.push_back(left_bpoint->pt());
        temp_poly.push_back(target_node->origin());
        temp_poly.push_back(right_bpoint->pt());   

    }

    poly=temp_poly;

    double G = vcl_pow(2.0,-25.0);

    for (unsigned int s = 0; s < poly.num_sheets(); ++s)
    {
        for (unsigned int p = 0; p < poly[s].size(); ++p)
        { 
            poly[s][p].x()=
                (vcl_floor((poly[s][p].x()/G)+0.5))*G;
            poly[s][p].y()=
                (vcl_floor((poly[s][p].y()/G)+0.5))*G;
        }
    }
}

void dbsk2d_ishock_grouping_transform::get_region_stats(
    unsigned int index, vgl_polygon<double>& poly,
    vcl_vector<double>& region_stats)
{


    unsigned int f_index=0;
    vgl_polygon<double> start_poly(poly[f_index]);
    double area=vgl_area(start_poly);

    for (unsigned int s = 1; s < poly.num_sheets(); ++s)
    { 
        
        vgl_polygon<double> tempy(poly[s]);
        double area_temp = vgl_area(tempy);
        if ( area_temp > area )
        {
             area = area_temp;
             f_index=s;

        }
        
    }

    vgl_box_2d<double> bbox;

    vcl_vector<vgl_point_2d<double>  > points;

    vgl_polygon<double> final_poly(poly[f_index]);

    for ( unsigned int k=0; k < final_poly.num_sheets() ; ++k)
    {
        for (unsigned int p = 0; p < final_poly[k].size(); ++p)
        {
            points.push_back(final_poly[k][p]);
            bbox.add(final_poly[k][p]);
        }
        
    }
    
    // Compute area poly stats
    vgl_polygon<double> hull=vgl_convex_hull(points);
    double convex_area=vgl_area(hull);
        
    
    // Area of final poly
    area=vgl_area(final_poly);
    
    // Convexity
    double ratio=area/convex_area;
    
    // Distance to image center
    vgl_point_2d<double> centroid=vgl_centroid(final_poly);
    vil_image_resource_sptr img=dbsk2d_transform_manager::Instance().
        get_image();
    vgl_point_2d<double> image_center(img->ni()/2,img->nj()/2);
    double distance_to_center=vgl_distance(centroid,image_center);
        
   
    // Compute total length of this polygon
    double length = 0.0;

    // Take first sheet
    vgl_point_2d<double> p0(final_poly[0][0].x(),final_poly[0][0].y());
    for (unsigned int p = 1; p < final_poly[0].size(); ++p) 
    {
        vgl_point_2d<double> c0(final_poly[0][p].x(),final_poly[0][p].y());
        length += vgl_distance(p0,c0);
        p0=c0;
    }


    double bb_width=bbox.width();
    double bb_height=bbox.height();

    double con_ratio=this->contour_ratio(index);
    double gt_ratio(0.0),gap_ratio(0.0);
    this->real_contour_ratio(index,gt_ratio,gap_ratio);


    vcl_vector<dbsk2d_ishock_edge*> edges=region_nodes_[index];

    
    dbsk2d_shock_graph_sptr shock_graph=new dbsk2d_shock_graph();
    dbsk2d_prune_ishock pruner(ishock_graph_,shock_graph);

    unsigned int total_edges=0;
    double total_splice_cost=0.0;

    for ( unsigned int s=0; s < edges.size() ; ++s)
    {
        double sc=pruner.splice_cost(edges[s]);
        total_splice_cost+=sc;

        if ( sc >= 1.0 )
        {
            total_edges+=1;
        }
        
    }

    region_stats.push_back(area);           //Area of polygon
    region_stats.push_back(convex_area);    //Convex area of polygon
    region_stats.push_back(ratio);          //Ratio of area/convex area
    region_stats.push_back(length);         //Perimiter of region

    region_stats.push_back(bb_width);           // Bounding box width
    region_stats.push_back(bb_height);          // Bounding box height
    region_stats.push_back(distance_to_center); // Distance to center of image 

    region_stats.push_back(con_ratio);      // ratio=(contour+gap)/length
    region_stats.push_back(gt_ratio);       // ratio=(contour)/length
    region_stats.push_back(gap_ratio);      // ratio=(gap)/length 

    region_stats.push_back(total_splice_cost);  // total splice cost
    region_stats.push_back(total_edges);        // edges above threshold 
}

double dbsk2d_ishock_grouping_transform::convex_area(vgl_polygon<double>& 
                                                     poly)
{
    unsigned int f_index=0;
    vgl_polygon<double> start_poly(poly[f_index]);
    double area=vgl_area(start_poly);

    for (unsigned int s = 1; s < poly.num_sheets(); ++s)
    { 
        
        vgl_polygon<double> tempy(poly[s]);
        double area_temp = vgl_area(tempy);
        if ( area_temp > area )
        {
             area = area_temp;
             f_index=s;

        }
        
    }
    
    vcl_vector<vgl_point_2d<double>  > points;

    vgl_polygon<double> final_poly(poly[f_index]);

    for ( unsigned int k=0; k < final_poly.num_sheets() ; ++k)
    {
        for (unsigned int p = 0; p < final_poly[k].size(); ++p)
        {
            points.push_back(final_poly[k][p]);
            
        }
        
    }
    
    vgl_polygon<double> hull=vgl_convex_hull(points);
    
    return vgl_area(hull);
}

double dbsk2d_ishock_grouping_transform::convex_area(unsigned int index)
{
    
    vcl_vector<dbsk2d_ishock_belm*> frag_belms = region_belms_[index];


    vcl_vector<vgl_point_2d<double> > points;
  
    vcl_vector<dbsk2d_ishock_belm*>::iterator lit;  
    for (lit = frag_belms.begin() ; lit != frag_belms.end() ; ++lit)
    {
        dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)(*lit);
        points.push_back(bline->s_pt()->pt());

    }     

    vgl_polygon<double> poly=vgl_convex_hull(points);
    
    return vgl_area(poly);
}

double dbsk2d_ishock_grouping_transform::contour_ratio(
    unsigned int index,vgl_polygon<double>& polygon)
{

    double virtual_boundary_length=0;
    vcl_vector<dbsk2d_ishock_node*> outer_nodes = outer_shock_nodes_[index];
    
    unsigned int i=0; 
    for ( ; i < outer_nodes.size() ; ++i)
    {
        virtual_boundary_length+=2*outer_nodes[i]->startTime();

    }

    // Compute total length of this polygon
    double length = 0.0;

    // Take first sheet
    vgl_point_2d<double> p0(polygon[0][0].x(),polygon[0][0].y());
    for (unsigned int p = 1; p < polygon[0].size(); ++p) 
    {
        vgl_point_2d<double> c0(polygon[0][p].x(),polygon[0][p].y());
        length += vgl_distance(p0,c0);
        p0=c0;
    }

    // Add in last distance
    vgl_point_2d<double> last_point(polygon[0][polygon[0].size()-1].x(),
                                    polygon[0][polygon[0].size()-1].y());
    vgl_point_2d<double> first_point(polygon[0][0].x(),
                                     polygon[0][0].y());
    length += vgl_distance(first_point,last_point);

    return 1.0-(virtual_boundary_length/length);

}

double dbsk2d_ishock_grouping_transform::contour_ratio(
    unsigned int index)
{

    double virtual_boundary_length=0;
    vcl_vector<dbsk2d_ishock_node*> outer_nodes = outer_shock_nodes_[index];
    
    unsigned int i=0; 
    for ( ; i < outer_nodes.size() ; ++i)
    {
        virtual_boundary_length+=2*outer_nodes[i]->startTime();

    }

    // Compute total length of this polygon
    double total_length = 0.0;

    double real_distance=0.0;

    vcl_vector<dbsk2d_ishock_edge*> shock_edges = region_nodes_[index];

    for ( unsigned int s=0; s < shock_edges.size() ; ++s)
    {  
        dbsk2d_ishock_edge* edge=shock_edges[s];

        //Line/Line
        if ( edge->lBElement()->is_a_line() && 
             edge->rBElement()->is_a_line())
        {
            
            real_distance += vgl_distance(edge->getLFootPt(edge->sTau()),
                                          edge->getLFootPt(edge->eTau()));
            real_distance += vgl_distance(edge->getRFootPt(edge->eTau()),
                                          edge->getRFootPt(edge->sTau()));

        }
        //Left line/Right Point
        else if ( edge->lBElement()->is_a_line() && 
                  edge->rBElement()->is_a_point())
        {
            real_distance += vgl_distance(edge->getLFootPt(edge->sTau()),
                                          edge->getLFootPt(edge->eTau()));     

        }
        //Right Line/Left Point
        else if ( edge->lBElement()->is_a_point() && 
                  edge->rBElement()->is_a_line())
        {      
            real_distance += vgl_distance(edge->getRFootPt(edge->sTau()),
                                          edge->getRFootPt(edge->eTau()));

        }
    }
    total_length=real_distance+virtual_boundary_length;
    double contour_ratio = 1.0-(virtual_boundary_length/total_length);
    return contour_ratio;
}



void dbsk2d_ishock_grouping_transform::real_contour_ratio(
    unsigned int index,
    double& gt_con_ratio,
    double& gap_ratio)
{

    double virtual_boundary_length=0;
    vcl_vector<dbsk2d_ishock_node*> outer_nodes = outer_shock_nodes_[index];
    
    unsigned int i=0; 
    for ( ; i < outer_nodes.size() ; ++i)
    {
        virtual_boundary_length+=2*outer_nodes[i]->startTime();

    }

    // Compute total length of this polygon
    double total_length = 0.0;

    double real_distance=0.0;
    double gap_distance=0.0;

    vcl_vector<dbsk2d_ishock_edge*> shock_edges = region_nodes_[index];

    for ( unsigned int s=0; s < shock_edges.size() ; ++s)
    {  
        dbsk2d_ishock_edge* edge=shock_edges[s];

        double temp_distance1(0.0);
        double temp_distance2(0.0);

        //Line/Line
        if ( edge->lBElement()->is_a_line() && 
             edge->rBElement()->is_a_line())
        {
            
            temp_distance1 = vgl_distance(edge->getLFootPt(edge->sTau()),
                                          edge->getLFootPt(edge->eTau()));
            temp_distance2 = vgl_distance(edge->getRFootPt(edge->eTau()),
                                          edge->getRFootPt(edge->sTau()));

            if ( edge->lBElement()->get_contour_id()<0)
            {
                gap_distance += temp_distance1;                
            }
            else
            {
                real_distance += temp_distance1;
            }

            if ( edge->rBElement()->get_contour_id()<0)
            {
                gap_distance += temp_distance2;                
            }
            else
            {
                real_distance += temp_distance2;
            }

        }
        //Left line/Right Point
        else if ( edge->lBElement()->is_a_line() && 
                  edge->rBElement()->is_a_point())
        {
            temp_distance1 = vgl_distance(edge->getLFootPt(edge->sTau()),
                                          edge->getLFootPt(edge->eTau()));
     
            if ( edge->lBElement()->get_contour_id()<0)
            {
                gap_distance += temp_distance1;    
            }
            else
            {
                real_distance += temp_distance1;
            }

        }
        //Right Line/Left Point
        else if ( edge->lBElement()->is_a_point() && 
                  edge->rBElement()->is_a_line())
        {      
            temp_distance1 = vgl_distance(edge->getRFootPt(edge->sTau()),
                                          edge->getRFootPt(edge->eTau()));

            if ( edge->rBElement()->get_contour_id()<0)
            {
                gap_distance += temp_distance1;    
            }
            else
            {
                real_distance += temp_distance1;
            }

        }
    }
    total_length=real_distance+gap_distance+virtual_boundary_length;
    gt_con_ratio = real_distance/total_length;
    gap_ratio    = gap_distance/total_length;
}

double dbsk2d_ishock_grouping_transform::real_contour_length(unsigned int index)
{
    double real_distance=0.0;

    vcl_vector<dbsk2d_ishock_edge*> shock_edges = region_nodes_[index];

    for ( unsigned int s=0; s < shock_edges.size() ; ++s)
    {  
        dbsk2d_ishock_edge* edge=shock_edges[s];

        //Line/Line
        if ( edge->lBElement()->is_a_line() && 
             edge->rBElement()->is_a_line())
        {
            
            real_distance += vgl_distance(edge->getLFootPt(edge->sTau()),
                                          edge->getLFootPt(edge->eTau()));
            real_distance += vgl_distance(edge->getRFootPt(edge->eTau()),
                                          edge->getRFootPt(edge->sTau()));

        }
        //Left line/Right Point
        else if ( edge->lBElement()->is_a_line() && 
                  edge->rBElement()->is_a_point())
        {
            real_distance += vgl_distance(edge->getLFootPt(edge->sTau()),
                                          edge->getLFootPt(edge->eTau()));     

        }
        //Right Line/Left Point
        else if ( edge->lBElement()->is_a_point() && 
                  edge->rBElement()->is_a_line())
        {      
            real_distance += vgl_distance(edge->getRFootPt(edge->sTau()),
                                          edge->getRFootPt(edge->eTau()));

        }
    }

    return real_distance;
}

bool dbsk2d_ishock_grouping_transform::region_within_image(
    unsigned int index)
{

    vil_image_resource_sptr img=dbsk2d_transform_manager::Instance().
        get_image();
   
    vcl_vector<dbsk2d_ishock_belm*> belms  = region_belms_[index];
    
    for ( unsigned int i=0; i < belms.size() ; ++i)
    {
        dbsk2d_ishock_belm* belm=belms[i];
        vgl_point_2d<double> pt1,pt2;
       
        dbsk2d_ishock_bline* bline=(dbsk2d_ishock_bline*)(belm);

        if ( bline->get_contour_id() < 0 )
        {
            continue;
        }

        if ( belm->is_a_line() )
        {
            dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)(belm);
            pt1=bline->s_pt()->pt();
            pt2=bline->e_pt()->pt();
        }
        else if ( belm->is_a_point())
        {
            dbsk2d_ishock_bpoint* bpoint = ( dbsk2d_ishock_bpoint*)(belm);
            pt1=bpoint->pt();
            pt2=bpoint->pt();
        }

        if ( pt1.x() < 0 || pt1.y() < 0 ||
             pt1.x() >= img->ni() || pt1.y() >= img->nj() || 
             pt2.x() < 0 || pt2.y() < 0 ||
             pt2.x() >= img->ni() || pt2.y() >= img->nj())
        
        {
            return false;
        
        }

    }

    return true;
}

void dbsk2d_ishock_grouping_transform::polygon_fragment(
    unsigned int index,vgl_polygon<double>& poly)
{
    //***************** Compute Polygon ************************
    //Loop over all shock links
    vcl_vector<dbsk2d_ishock_edge*> edges = region_nodes_[index];
    
    // Start polygon
    vgl_polygon<double> start_poly;
    extract_polygon(edges[0],start_poly);

    unsigned int s=1;
    for ( ; s < edges.size() ; ++s)
    {

        
        //Take temp
        vgl_polygon<double> temp;
        extract_polygon(edges[s],temp);

        //Keep a flag for status
        int value;

        //Take union of two polygons
        start_poly = vgl_clip(start_poly,             // p1
                              temp,                   // p2
                              vgl_clip_type_union,    // p1 U p2
                              &value);                // test if success

        assert(value==1);


    }
 
    // Keep largest area polygon
    double area=0;
    unsigned int f_index=0;

    for (unsigned int s = 0; s < start_poly.num_sheets(); ++s)
    { 
        poly.push_back(start_poly[s]);
        // vgl_polygon<double> tempy(start_poly[s]);
        // double area_temp = vgl_area(tempy);
        // if ( area_temp > area )
        // {
        //     area = area_temp;
        //     f_index=s;

        // }
        
    }
    
//     poly.push_back(start_poly[f_index]); 
    
}

void dbsk2d_ishock_grouping_transform::write_out_polygons(vcl_string filename)
{

    vcl_ofstream output_binary_file;
    output_binary_file.open(filename.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);
    
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >::iterator it;
    for ( it = region_nodes_.begin() ; it != region_nodes_.end() ; ++it)
    {
        vgl_polygon<double> poly;
        polygon_fragment((*it).first,poly);

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

    }

    

    output_binary_file.close();


}


void dbsk2d_ishock_grouping_transform::write_out_polygons(vcl_string filename,
    dbsk2d_ishock_transform& transform)
{

    vcl_ofstream output_binary_file;
    output_binary_file.open(filename.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);

    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >::iterator it;
    for ( it = region_nodes_.begin() ; it != region_nodes_.end() ; ++it)
    {
        if ( transform.region_affected(region_belms_contour_ids_[(*it).first]))
        {
       
            vgl_polygon<double> poly;
            polygon_fragment((*it).first,poly);
            
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
        }
    }

    output_binary_file.close();


}

void dbsk2d_ishock_grouping_transform::
expand_wavefront(dbsk2d_ishock_node* node,unsigned int map_key)
{

    vcl_vector<dbsk2d_ishock_edge*> edges;
    bool insert=false;
    node_expandable(node,edges,insert);
    if ( !edges.size() )
    {
        if ( insert )
        {
            outer_shock_nodes_[map_key].push_back(node);
        }
        return;
    }
    else
    {
        for ( unsigned int i=0; i < edges.size() ; ++i)
        {
            dbsk2d_ishock_edge* edge=edges[i];
            dbsk2d_ishock_node* opposite=(edge->pSNode()->id()==node->id())?
                edge->cSNode():edge->pSNode();

            if ( visited_edges_.count(edge->id())==0)
            {
                int lbe_id(0);
                int rbe_id(0);

                if ( edge->lBElement()->is_a_line())
                {
                    if ( !region_belms_ids_[map_key]
                         .count(edge->lBElement()->id()) )
                    {
                        region_belms_[map_key].push_back(edge->lBElement());

                    }

                    region_belms_ids_[map_key].insert(
                        edge->lBElement()->id());
                    region_belms_contour_ids_[map_key].insert(
                        edge->lBElement()->get_contour_id());
                }

                if ( edge->rBElement()->is_a_line())
                {
                    if ( !region_belms_ids_[map_key]
                         .count(edge->rBElement()->id()) )
                    {
                        region_belms_[map_key].push_back(edge->rBElement());

                    }

                    region_belms_ids_[map_key].insert(
                        edge->rBElement()->id());
                    region_belms_contour_ids_[map_key].insert(
                        edge->rBElement()->get_contour_id());
                }

                region_nodes_[map_key].push_back(edge);
                visited_edges_[edge->id()]="temp";
            }

            if (opposite->degree() > 1 )
            {
                expand_wavefront(opposite,map_key);
            }
        }
    }
}
