// This is brcv/shp/dbsk2d/algo/dbsk2d_transform_manager.cxx

//:
// \file
#include <dbsk2d/dbsk2d_transform_manager.h>
#include <vil3d/vil3d_load.h>
#include <vil/vil_load.h>
#include <vil/vil_bilin_interp.h>
#include <vil3d/vil3d_tricub_interp.h>
#include <vcl_fstream.h>

#include <vil/vil_plane.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vcl_cmath.h>
#include <bsol/bsol_algs.h>

#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/algo/vgl_convex_hull_2d.h>

#include <dbsk2d/dbsk2d_bnd_contour.h>

#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vsol/vsol_polyline_2d.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>

dbsk2d_transform_manager::dbsk2d_transform_manager()
    :image_(0),
     threshold_(0),
     dist_volume_(0),
     gPb_image_(0),
     out_folder_(),
     out_prefix_(),
     logistic_beta0_(1.0),
     logistic_beta1_(0.0),
     id_(0),
     normalization_(1.0)

{


}

dbsk2d_transform_manager::~dbsk2d_transform_manager()
{
    destroy_singleton();
}

void dbsk2d_transform_manager::destroy_singleton()
{
   
    image_=0;
}

void dbsk2d_transform_manager::read_in_training_data(vcl_string filename)
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

void dbsk2d_transform_manager::read_in_gpb_data(vcl_string filename)
{

    vcl_ifstream file (filename.c_str(), 
                       vcl_ios::in|vcl_ios::binary|vcl_ios::ate);
    double* memblock(0);
    double max_gPb_value=0.0;
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

        vcl_cout<<"Reading in a "<<ni<<" by "<<nj<<" gPb values"
                <<vcl_endl;

        gPb_image_.set_size(ni,nj);
        unsigned int index=2;

        for (unsigned j=0;j<nj;++j)
        {
            for (unsigned i=0;i<ni;++i)
            {
                double value = memblock[index];
                gPb_image_(i,j)=value;
                if ( value > max_gPb_value )
                {
                    max_gPb_value=value;
                }
                index++;
            }
        }
        

        delete[] memblock;
        memblock=0;
    }

    normalization_=max_gPb_value;
}

double dbsk2d_transform_manager::contour_gpb_value(
    vcl_vector<dbsk2d_ishock_belm*>& belms)
{
    vcl_map<int, vgl_point_2d<double> > output_points;
  
    double perimeter=0.0;
    vcl_vector<dbsk2d_ishock_belm*>::iterator lit;  
    for (lit = belms.begin() ; lit != belms.end() ; ++lit)
    {
        dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)(*lit);
        double distance=vgl_distance(bline->s_pt()->pt(),
                                     bline->e_pt()->pt());

        output_points[bline->s_pt()->id()]=bline->s_pt()->pt();
        output_points[bline->e_pt()->id()]=bline->e_pt()->pt();

        perimeter=perimeter+distance;
    }     


    double summation=0.0;
    vcl_map<int,vgl_point_2d<double> >::iterator it;
    for ( it = output_points.begin() ; it != output_points.end(); ++it)
    {
        double y=(*it).second.x();
        double x=(*it).second.y();

        double gPb = vil_bilin_interp_safe_extend(gPb_image_,
                                                  x,
                                                  y);

        summation = summation + gPb;
    }
    


    return summation/perimeter;
}


void dbsk2d_transform_manager::grid_points(
    vcl_vector<dbsk2d_ishock_edge*>& region,
    vcl_vector<dbsk2d_ishock_belm*>& belms,
    vcl_vector<vgl_point_2d<double> >& foreground_grid,
    vcl_vector<vgl_point_2d<double> >& background_grid)
{
 
    // Make a dummy coarse shock graph
    dbsk2d_shock_graph_sptr coarse_graph;
    dbsk2d_sample_ishock sampler(coarse_graph);
    sampler.set_sample_resolution(0.5);
    double step_size=1.0;
    
    vcl_set<int> foreground_shocks;

    // Deal with foreground first
    {
        for ( unsigned int i=0; i < region.size() ; ++i)
        {
            dbsk2d_ishock_edge* cur_iedge=region[i];
            
            foreground_shocks.insert(cur_iedge->id());

            // Create a dummy xshock edge
            dbsk2d_shock_node_sptr parent_node = new dbsk2d_shock_node();
            dbsk2d_shock_node_sptr child_node  = new dbsk2d_shock_node();
            dbsk2d_xshock_edge cur_edge(1,parent_node,child_node);
       
            switch (cur_iedge->type())
            {
            case dbsk2d_ishock_elm::POINTPOINT:
                sampler.sample_ishock_edge((dbsk2d_ishock_pointpoint*)
                                           cur_iedge, 
                                           &cur_edge);
                break;
            case dbsk2d_ishock_elm::POINTLINE:
                sampler.sample_ishock_edge((dbsk2d_ishock_pointline*)
                                           cur_iedge, 
                                           &cur_edge);
                break;
            case dbsk2d_ishock_elm::LINELINE:
                sampler.sample_ishock_edge((dbsk2d_ishock_lineline*)
                                           cur_iedge, 
                                           &cur_edge);
                break;
            default:
                break;
            }

            for ( unsigned int s=0; s < cur_edge.num_samples() ; ++s)
            {
                dbsk2d_xshock_sample_sptr sample=cur_edge.sample(s);

                double R1=sample->radius;
                vgl_point_2d<double> pt =sample->pt;
                double theta=sample->theta;
                double phi=0.0;
                double r=step_size;
            

                if (sample->speed != 0 && sample->speed < 99990)
                {
                    phi=vcl_acos(-1.0/sample->speed);
                }
                else
                {
                    phi=vnl_math::pi/2;
                }

                double vec1=theta+phi;
                double vec2=theta-phi;

                while ( r < R1)
                {

                    vgl_point_2d<double> plus_pt=_translatePoint(pt,vec1,r);
                    vgl_point_2d<double> minus_pt=_translatePoint(pt,vec2,r);
                


                    foreground_grid.push_back(plus_pt);
                    foreground_grid.push_back(minus_pt);



                    r+=step_size;
                }
            
                foreground_grid.push_back(pt);

            }
        }

    }

    // Deal with Background second
    {
        // get outer shocks
        vcl_map<int,dbsk2d_ishock_edge*> outer_shocks;

        for ( unsigned int b=0; b < belms.size() ; ++b)
        {
            dbsk2d_ishock_bline* bline=(dbsk2d_ishock_bline*)belms[b];
            dbsk2d_ishock_bline* twinline=bline->twinLine();
            dbsk2d_ishock_bpoint* s_pt=twinline->s_pt();
            dbsk2d_ishock_bpoint* e_pt=twinline->e_pt();
        
            // Twinline first
            {
                bnd_ishock_map shocks=twinline->shock_map();
                bnd_ishock_map_iter curS = shocks.begin();
                for (; curS!=shocks.end(); ++curS)
                {
                    if ( !foreground_shocks.count(curS->second->id()))
                    {
                        outer_shocks[curS->second->id()]=curS->second;
                    } 
                }
            }

            // s_pt second
            {
                bnd_ishock_map shocks=s_pt->shock_map();
                bnd_ishock_map_iter curS = shocks.begin();
                for (; curS!=shocks.end(); ++curS)
                {
                    if ( !foreground_shocks.count(curS->second->id()))
                    {
                        outer_shocks[curS->second->id()]=curS->second;
                    } 
                }
            }

            // e_pt second
            {
                bnd_ishock_map shocks=e_pt->shock_map();
                bnd_ishock_map_iter curS = shocks.begin();
                for (; curS!=shocks.end(); ++curS)
                {
                    if ( !foreground_shocks.count(curS->second->id()))
                    {
                        outer_shocks[curS->second->id()]=curS->second;
                    } 
                }
            }

        }


        vcl_map<int,dbsk2d_ishock_edge*>::iterator it;
        for ( it = outer_shocks.begin() ; it != outer_shocks.end() ; ++it)
        {
            dbsk2d_ishock_edge* cur_iedge=(*it).second;
 
            // Create a dummy xshock edge
            dbsk2d_shock_node_sptr parent_node = new dbsk2d_shock_node();
            dbsk2d_shock_node_sptr child_node  = new dbsk2d_shock_node();
            dbsk2d_xshock_edge cur_edge(1,parent_node,child_node);
       
            switch (cur_iedge->type())
            {
            case dbsk2d_ishock_elm::POINTPOINT:
                sampler.sample_ishock_edge((dbsk2d_ishock_pointpoint*)
                                           cur_iedge, 
                                           &cur_edge);
                break;
            case dbsk2d_ishock_elm::POINTLINE:
                sampler.sample_ishock_edge((dbsk2d_ishock_pointline*)
                                           cur_iedge, 
                                           &cur_edge);
                break;
            case dbsk2d_ishock_elm::LINELINE:
                sampler.sample_ishock_edge((dbsk2d_ishock_lineline*)
                                           cur_iedge, 
                                           &cur_edge);
                break;
            default:
                break;
            }

            for ( unsigned int s=0; s < cur_edge.num_samples() ; ++s)
            {
                dbsk2d_xshock_sample_sptr sample=cur_edge.sample(s);

                double R1=sample->radius;
                vgl_point_2d<double> pt =sample->pt;
                double theta=sample->theta;
                double phi=0.0;
                double r=step_size;
            

                if (sample->speed != 0 && sample->speed < 99990)
                {
                    phi=vcl_acos(-1.0/sample->speed);
                }
                else
                {
                    phi=vnl_math::pi/2;
                }

                double vec1=theta+phi;
                double vec2=theta-phi;

                while ( r < R1)
                {

                    vgl_point_2d<double> plus_pt=_translatePoint(pt,vec1,r);
                    vgl_point_2d<double> minus_pt=_translatePoint(pt,vec2,r);
                



                    if ( plus_pt.x() >= 0 && 
                         plus_pt.y() >= 0 &&
                         plus_pt.x() < image_->ni() && 
                         plus_pt.y() < image_->nj())
                    {
                        background_grid.push_back(plus_pt);
                        
                    }

                    if ( minus_pt.x() >= 0 && 
                         minus_pt.y() >= 0 &&
                         minus_pt.x() < image_->ni() && 
                         minus_pt.y() < image_->nj())
                    {
                        background_grid.push_back(minus_pt);
                        
                    }



                    r+=step_size;
                }
            
                if ( pt.x() >= 0 && 
                     pt.y() >= 0 &&
                     pt.x() < image_->ni() && 
                     pt.y() < image_->nj())
                {
                    background_grid.push_back(pt);
                }
            }
        }
    }


}

double dbsk2d_transform_manager::transform_probability(
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

double dbsk2d_transform_manager::transform_probability(
    vcl_vector<vgl_point_2d<double> >& curve)
{

    if ( gPb_image_ == 0 || curve.size() == 0 )
    {
        return 0.0;
    }

    double summation=0.0;
    for ( unsigned int c=0; c < curve.size() ; ++c)
    {
        double y=curve[c].x();
        double x=curve[c].y();

        double gPb = vil_bilin_interp_safe_extend(gPb_image_,
                                                  x,
                                                  y);

        summation = summation + gPb;
    }
    

    double average_gPb = summation/curve.size();
    if ( average_gPb > normalization_ )
    {
        average_gPb = normalization_;
    }
    return average_gPb/normalization_;

}

double dbsk2d_transform_manager::transform_probability(
    vsol_polyline_2d_sptr& curve)
{

    if ( gPb_image_ == 0 || curve->size() == 0 )
    {
        return 0.0;
    }

    double summation=0.0;
    for ( unsigned int c=0; c < curve->size() ; ++c)
    {
        double y=curve->vertex(c)->get_p().x();
        double x=curve->vertex(c)->get_p().y();

        double gPb = vil_bilin_interp_safe(gPb_image_,
                                           x,
                                           y);

        summation = summation + gPb;
    }
    

    double average_gPb = summation/curve->size();

    return average_gPb/normalization_;

}

void dbsk2d_transform_manager::start_binary_file(vcl_string binary_file_output)
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

void dbsk2d_transform_manager::start_region_file(vcl_string binary_file_output)
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

void dbsk2d_transform_manager::write_output_polygon(vgl_polygon<double>& poly)
{


    vcl_ofstream output_binary_file;
    output_binary_file.open(output_binary_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);
    
    double num_sheets=poly.num_sheets();
    output_binary_file.write(reinterpret_cast<char *>(&num_sheets),
                              sizeof(double));

    for (unsigned int s = 0; s < poly.num_sheets(); ++s)
    { 

        double num_vertices= poly[s].size();
        output_binary_file.write(reinterpret_cast<char *>(&num_vertices),
                                 sizeof(double));

        for (unsigned int p = 0; p < poly[s].size(); ++p)
        {
            double xcoord=poly[s][p].x();
            double ycoord=poly[s][p].y();

            output_binary_file.write(reinterpret_cast<char *>(&xcoord),
                                     sizeof(double));
            output_binary_file.write(reinterpret_cast<char *>(&ycoord),
                                     sizeof(double));

        }
    }

    output_binary_file.close();


}

void dbsk2d_transform_manager::write_output_region_stats(
    vcl_vector<double>& region_stats)
{


    vcl_ofstream output_binary_file;
    output_binary_file.open(output_region_stats_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);
    
    double depth         = region_stats[0];
    double path_prob     = region_stats[1];
    double gap_prob      = region_stats[2];
    double contour_ratio = region_stats[3];
    double area          = region_stats[4];
    double convex_area   = region_stats[5];
    double convexity     = region_stats[6];
    
    output_binary_file.write(reinterpret_cast<char *>(&depth),
                             sizeof(double));
    
    output_binary_file.write(reinterpret_cast<char *>(&path_prob),
                             sizeof(double));
    
    output_binary_file.write(reinterpret_cast<char *>(&gap_prob),
                             sizeof(double));
    
    output_binary_file.write(reinterpret_cast<char *>(&contour_ratio),
                             sizeof(double));
    
    output_binary_file.write(reinterpret_cast<char *>(&area),
                             sizeof(double));
    
    output_binary_file.write(reinterpret_cast<char *>(&convex_area),
                             sizeof(double));
    
    output_binary_file.write(reinterpret_cast<char *>(&convexity),
                             sizeof(double));
    
    
    output_binary_file.close();


}


void dbsk2d_transform_manager::write_output_region(vgl_polygon<double>& poly)
{


    vcl_ofstream output_region_file;
    output_region_file.open(output_region_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);
    
    double num_contours= (poly[0].size()-1)*4.0+(poly[0].size()-1);
    output_region_file.write(reinterpret_cast<char *>(&num_contours),
                              sizeof(double));
    for (unsigned int p = 0; p < (poly[0].size()-1); ++p)
    {
        double x1_coord = poly[0][p].x();
        double y1_coord = poly[0][p].y();

        double x2_coord = poly[0][p+1].x();
        double y2_coord = poly[0][p+1].y();

        output_region_file.write(reinterpret_cast<char *>(&x1_coord),
                                  sizeof(double));
        output_region_file.write(reinterpret_cast<char *>(&y1_coord),
                                  sizeof(double));

        output_region_file.write(reinterpret_cast<char *>(&x2_coord),
                                  sizeof(double));
        output_region_file.write(reinterpret_cast<char *>(&y2_coord),
                                  sizeof(double));

        double contour_id = 1.0;
        output_region_file.write(reinterpret_cast<char *>(&contour_id),
                                 sizeof(double));
        
    
    }

    output_region_file.close();


}

void dbsk2d_transform_manager::get_extra_belms(
    vcl_vector<dbsk2d_ishock_belm*>& region_belms,
    vcl_set<int>& key,
    vcl_map<int,dbsk2d_ishock_bline*>& output_lines)
{
    vcl_vector<dbsk2d_ishock_belm*>::iterator lit;  
    for (lit = region_belms.begin() ; lit != region_belms.end() ; ++lit)
    {
        dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)(*lit);
        output_lines[bline->id()]=bline;
    }     

    for (lit = region_belms.begin() ; lit != region_belms.end() ; ++lit)
    {
    
        dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)(*lit);

        {
            dbsk2d_ishock_bpoint* s_pt=bline->s_pt();

            dbsk2d_ishock_bline* left_element_spt =
                (dbsk2d_ishock_bline*)(s_pt->getElmToTheRightOf(bline));
            dbsk2d_ishock_bline* right_element_spt =
                (dbsk2d_ishock_bline*)(s_pt->getElmToTheLeftOf(bline));

            if ( !output_lines.count(left_element_spt->twinLine()->id()))
            { 
                output_lines[left_element_spt->id()]=left_element_spt;
            }
           
            if ( !output_lines.count(right_element_spt->twinLine()->id()))
            { 
                output_lines[right_element_spt->id()]=right_element_spt;
            }
            
        }

        
        {
            dbsk2d_ishock_bpoint* e_pt=bline->e_pt();
           
            dbsk2d_ishock_bline* left_element_ept =
                (dbsk2d_ishock_bline*)(e_pt->getElmToTheRightOf(bline));
            dbsk2d_ishock_bline* right_element_ept =
                (dbsk2d_ishock_bline*)(e_pt->getElmToTheLeftOf(bline));

            if ( !output_lines.count(left_element_ept->twinLine()->id()))
            { 
                output_lines[left_element_ept->id()]=left_element_ept;
            }
           
            if ( !output_lines.count(right_element_ept->twinLine()->id()))
            { 
                output_lines[right_element_ept->id()]=right_element_ept;
            }
        }
   
    }

    vcl_map<int, dbsk2d_ishock_bline*>::iterator oit;
    for (oit = output_lines.begin() ; oit != output_lines.end() ; ++oit)
    {
        key.insert((*oit).first);
    }
}

void dbsk2d_transform_manager::write_output_region(
    vcl_vector<dbsk2d_ishock_belm*>& region_belms)
{


    vcl_ofstream output_region_file;
    output_region_file.open(output_region_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);
    
    vcl_map<int, dbsk2d_ishock_bline*> output_lines;
  
    vcl_vector<dbsk2d_ishock_belm*>::iterator lit;  
    for (lit = region_belms.begin() ; lit != region_belms.end() ; ++lit)
    {
        dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)(*lit);
        output_lines[bline->id()]=bline;
    }     

    // for (lit = region_belms.begin() ; lit != region_belms.end() ; ++lit)
    // {
    
    //     dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)(*lit);

    //     {
    //         dbsk2d_ishock_bpoint* s_pt=bline->s_pt();
    //         belm_list linkedbelms=s_pt->LinkedBElmList;
            
    //         belm_list::iterator bit;
    //         for ( bit = linkedbelms.begin() ; bit != linkedbelms.end() ; ++bit)
    //         {
    //             dbsk2d_ishock_bline* bline_extra = (dbsk2d_ishock_bline*)(*bit);
    //             if ( !output_lines.count(bline_extra->twinLine()->id()))
    //             { 
    //                 output_lines[bline_extra->id()]=bline_extra;
    //             }
                
    //         }
    //     }

        
    //     {
    //         dbsk2d_ishock_bpoint* e_pt=bline->e_pt();
    //         belm_list linkedbelms=e_pt->LinkedBElmList;
            
    //         belm_list::iterator bit;
    //         for ( bit = linkedbelms.begin() ; bit != linkedbelms.end() ; ++bit)
    //         {
    //             dbsk2d_ishock_bline* bline_extra = (dbsk2d_ishock_bline*)(*bit);
    //             if ( !output_lines.count(bline_extra->twinLine()->id()))
    //             { 
    //                 output_lines[bline_extra->id()]=bline_extra;
    //             }
    //         }
    //     }
   
    // }

        
 
    double num_contours= output_lines.size()*4.0+output_lines.size();
    output_region_file.write(reinterpret_cast<char *>(&num_contours),
                              sizeof(double));
  
    vcl_map<int, dbsk2d_ishock_bline*>::iterator oit;
    for (oit = output_lines.begin() ; oit != output_lines.end() ; ++oit)
    {

        dbsk2d_ishock_bline* bline = (dbsk2d_ishock_bline*)((*oit).second);
                
        dbsk2d_ishock_bpoint* source = bline->s_pt();
        dbsk2d_ishock_bpoint* target = bline->e_pt();

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

        double contour_id = (*oit).second->get_contour_id();
        output_region_file.write(reinterpret_cast<char *>(&contour_id),
                                 sizeof(double));

    }
    output_region_file.close();

}


void dbsk2d_transform_manager::write_output_region(
    vcl_vector<dbsk2d_bnd_contour_sptr>& contours,
    vcl_vector<vgl_point_2d<double> >& gap_filler)
{


    vgl_fit_lines_2d<double> fitter;
    fitter.set_min_fit_length(2);
    fitter.set_rms_error_tol(0.05f);
    fitter.add_curve(gap_filler);
    fitter.fit();

    vcl_vector<vgl_line_segment_2d<double> > segs;
    segs= fitter.get_line_segs();
    
    vcl_ofstream output_region_file;
    output_region_file.open(output_region_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);
    
    vcl_map<int,dbsk2d_bnd_edge_sptr> output_edges;
    double contour_id_orig=contours[0]->get_id();

    for ( unsigned int c=0; c < contours.size() ; ++c)
    {
        dbsk2d_bnd_contour_sptr curve = contours[c];
        for ( unsigned int v=0; v < curve->num_edges() ; ++v)
        {
            dbsk2d_bnd_edge_sptr edge=curve->bnd_edge(v);
            output_edges[edge->get_id()]=edge;
        }
    }
        
 
    double num_contours= output_edges.size()*4.0+output_edges.size()+
        segs.size()*4.0 +segs.size();
    output_region_file.write(reinterpret_cast<char *>(&num_contours),
                              sizeof(double));
  
    vcl_map<int, dbsk2d_bnd_edge_sptr>::iterator oit;
    for (oit = output_edges.begin() ; oit != output_edges.end() ; ++oit)
    {

        dbsk2d_bnd_edge_sptr bedge = (*oit).second;
                
        dbsk2d_ishock_bpoint* source = bedge->bnd_v1()->bpoint();
        dbsk2d_ishock_bpoint* target = bedge->bnd_v2()->bpoint();

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

        double contour_id = (*oit).second->get_id();
        output_region_file.write(reinterpret_cast<char *>(&contour_id),
                                 sizeof(double));

    }

    for ( unsigned int ls=0; ls < segs.size() ; ++ls)
    {

        vgl_line_segment_2d<double> segment=segs[ls];

        double x1_coord = segment.point1().x();
        double y1_coord = segment.point1().y();

        double x2_coord = segment.point2().x();
        double y2_coord = segment.point2().y();

        output_region_file.write(reinterpret_cast<char *>(&x1_coord),
                                  sizeof(double));
        output_region_file.write(reinterpret_cast<char *>(&y1_coord),
                                  sizeof(double));

        output_region_file.write(reinterpret_cast<char *>(&x2_coord),
                                  sizeof(double));
        output_region_file.write(reinterpret_cast<char *>(&y2_coord),
                                  sizeof(double));

        output_region_file.write(reinterpret_cast<char *>(&contour_id_orig),
                                 sizeof(double));

    }
    output_region_file.close();

}

void dbsk2d_transform_manager::write_output_polygon(
    vcl_vector<dbsk2d_bnd_contour_sptr>& contours,
    vcl_vector<vgl_point_2d<double> >& gap_filler)
{


   
    vcl_vector<vgl_point_2d<double> > hull_points;

    for (unsigned int k=0; k < gap_filler.size() ; ++k)
    {

        hull_points.push_back(gap_filler[k]);
    }

    for ( unsigned int c=0; c < contours.size() ; ++c)
    {
        dbsk2d_bnd_contour_sptr curve = contours[c];
        for ( unsigned int v=0; v < curve->num_edges()+1 ; ++v)
        {
            dbsk2d_bnd_vertex_sptr vertex=curve->bnd_vertex(v);
            hull_points.push_back(vertex->point());
        }
    }

    vgl_convex_hull_2d<double> convex_hull(hull_points);
    vgl_polygon<double> poly=convex_hull.hull();

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


void dbsk2d_transform_manager::save_image_poly(
    vgl_polygon<double>& vgl_poly,
    vcl_string filename)
{
    
    vil_image_resource_sptr img_r = vil_plane(image_, 0);
    vil_image_resource_sptr img_g = vil_plane(image_, 1);
    vil_image_resource_sptr img_b = vil_plane(image_, 2);

    vsol_polygon_2d_sptr vsol_poly = bsol_algs::poly_from_vgl(vgl_poly);
    
    vsol_poly->compute_bounding_box();
    vsol_box_2d_sptr bbox = vsol_poly->get_bounding_box();
    double minx = bbox->get_min_x()-5 < 0 ? 0 : bbox->get_min_x()-5;
    double miny = bbox->get_min_y()-5 < 0 ? 0 : bbox->get_min_y()-5;

    vil_image_view<vil_rgb<vxl_byte> > 
        temp((int)vcl_ceil(bbox->width() + 10), 
             (int)vcl_ceil(bbox->height() + 10), 1); 
    vil_rgb<vxl_byte> bg_col(255, 255, 255);
    temp.fill(bg_col);

    vil_image_view<vxl_byte> img_rv = img_r->get_view();
    vil_image_view<vxl_byte> img_gv = img_g->get_view();
    vil_image_view<vxl_byte> img_bv = img_b->get_view();

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
            if (x >= int(img_r->ni()) || y >= int(img_r->nj()))
            { 
                continue;
            }
            int xx = (int)vcl_floor(x - minx + 0.5); 
            int yy = (int)vcl_floor(y - miny + 0.5);
            if (xx < 0 || yy < 0)
            {
                continue;
            }
            if (double(xx) > bbox->width() || double(yy) > bbox->height())
            {
                continue;
            }
            temp(xx,yy) = 
                vil_rgb<vxl_byte>(img_rv(x,y), img_gv(x,y), img_bv(x,y));
        }
    }

    vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);
    vil_save_image_resource(out_img, 
                            filename.c_str()); 

}
