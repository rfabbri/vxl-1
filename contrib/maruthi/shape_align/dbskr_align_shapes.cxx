// This is brcv/shp/dbskfg/dbskr_align_shapes.cxx

//:
// \file

#include <shape_align/dbskr_align_shapes.h>

#include <vul/vul_file.h>

#include <vnl/vnl_matrix.h>

#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_polygon_scan_iterator.h>

#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <dbsk2d/algo/dbsk2d_hor_flip_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_xshock_node.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>

#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/dbskr_scurve.h>

#include <vcl_sstream.h>
#include <vcl_set.h>

//: Constructor
dbskr_align_shapes::dbskr_align_shapes(
    vcl_string model_filename,
    vcl_string query_filename,
    bool elastic_splice_cost, 
    float scurve_sample_ds,     
    float scurve_interpolate_ds,
    bool localized_edit,
    double scurve_matching_R,
    bool circular_ends,
    bool combined_edit,
    bool use_approx,
    bool lambda_scaling,
    double lambda_area
):elastic_splice_cost_(elastic_splice_cost),
  scurve_sample_ds_(scurve_sample_ds),
  scurve_interpolate_ds_(scurve_interpolate_ds),
  localized_edit_(localized_edit),
  scurve_matching_R_(scurve_matching_R),
  circular_ends_(circular_ends),
  combined_edit_(combined_edit),
  use_approx_(use_approx),
  lambda_scaling_(lambda_scaling),
  lambda_area_(lambda_area),
  switched_(false),
  tree1_mirror_(false),
  tree2_mirror_(false),
  shape_matrix_file_(),
  dc_file_()
{
    vcl_cout<<"Loading Model ESF Files"<<vcl_endl;
    load_esf(model_filename,true);
    vcl_cout<<"Loading Query ESF Files"<<vcl_endl;
    load_esf(query_filename,false);

    vcl_string model_basename=vul_file::basename(model_filename);
    vcl_string query_basename=vul_file::basename(query_filename);

    model_basename=vul_file::strip_extension(model_basename);
    query_basename=vul_file::strip_extension(query_basename);
    
    shape_matrix_file_ = model_basename + "_vs_" + query_basename + 
        "_dist_matrix.txt";

    dc_file_ = model_basename + "_vs_" + query_basename + 
        "_shape_align.bin";
    
    vcl_cout<<"Writing out shape matrix to "<<shape_matrix_file_<<vcl_endl;
    vcl_cout<<"Writing out shape alignment to "<<dc_file_<<vcl_endl;

    set_up_dc_file();
    
}

//: Destructor
dbskr_align_shapes::~dbskr_align_shapes() 
{
    // for (unsigned int m=0; m < model_trees_.size() ; ++m)
    // {
    //     model_trees_[m].first=0;
    //     model_trees_[m].second=0;
    // }
}


//: Set up bin file
void dbskr_align_shapes::set_up_dc_file()
{

    vcl_ofstream output_binary_file;
    output_binary_file.open(dc_file_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::binary);
    
    double m = model_trees_.size();
    double q = query_trees_.size();
    
    // Write out number of model trees vs query trees
    // we are comparing
    output_binary_file.write(reinterpret_cast<char *>(&m),
                             sizeof(double));
    output_binary_file.write(reinterpret_cast<char *>(&q),
                             sizeof(double));

    for (unsigned int p=0; p < query_polygons_.size() ; ++p)
    {

        // get polygon
        vgl_polygon<double> poly=query_polygons_[p];

        // save off points
        vcl_vector<vcl_pair<int,int> > points;

        // do not include boundary
        vgl_polygon_scan_iterator<double> psi(poly, false);  
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x) 
            {
                vcl_pair<int,int> coords(x,y);
                points.push_back(coords);
            }
        }

        double size=points.size();

        output_binary_file.write(reinterpret_cast<char *>(&size),
                                 sizeof(double));

        // write out points
        for ( unsigned int v=0; v < points.size() ; ++v)
        {
            double x=points[v].first;
            double y=points[v].second;
            
            output_binary_file.write(reinterpret_cast<char *>(&x),
                                     sizeof(double));
            output_binary_file.write(reinterpret_cast<char *>(&y),
                                     sizeof(double));
            
        }

    }

    output_binary_file.close();
}

//: Match
void dbskr_align_shapes::match()
{
    // Match
    vcl_ofstream output_binary_file;
    output_binary_file.open(dc_file_.c_str(),
                            vcl_ios::out |
                            vcl_ios::app |
                            vcl_ios::binary);

    // Lets keep output in a matrix
    vnl_matrix<double> ed_matrix(model_trees_.size(),
                                 query_trees_.size(),
                                 0.0);

    vcl_cout<<"Matching "<<model_trees_.size()<<" vs "<<
            query_trees_.size()<<vcl_endl;

    unsigned int m=0;
    while (model_trees_.size())
    {
        
        // Compute all pairs of edit distance
        dbskr_tree_sptr model_tree=(*model_trees_.begin()).first;
        dbskr_tree_sptr model_mirror_tree=(*model_trees_.begin()).second;

        for ( unsigned int q=0; q < query_trees_.size() ; ++q)
        {
            vcl_cout<<"Matching "<<m<<" to "<<q<<vcl_endl;

            vcl_vector<dbskr_scurve_sptr> curve_list1;
            vcl_vector<dbskr_scurve_sptr> curve_list2;
            vcl_vector< vcl_vector < vcl_pair <int,int> > > map_list;

            dbskr_tree_sptr query_tree=query_trees_[q].first;
            dbskr_tree_sptr query_mirror_tree=query_trees_[q].second;

            double model_area=model_tree->get_area();
            double query_area=query_tree->get_area();

            double mean_area=(model_area + 
                              query_area)/2;
                        
            double model_scale_ratio=1.0;
            double query_scale_ratio=1.0;
            double test_R=scurve_matching_R_;

            double model_sample_ds=scurve_sample_ds_;
            double query_sample_ds=scurve_sample_ds_;
            
            if ( lambda_scaling_ )
            {
                query_tree->clear_scurve_cache();
                query_mirror_tree->clear_scurve_cache();

                test_R=scurve_matching_R_*
                    vcl_sqrt(mean_area/lambda_area_);
                model_scale_ratio = vcl_sqrt(mean_area/model_area);
                query_scale_ratio = vcl_sqrt(mean_area/query_area);

            
                model_sample_ds=scurve_sample_ds_*vcl_sqrt(model_area
                                                           /lambda_area_);
                query_sample_ds=scurve_sample_ds_*vcl_sqrt(query_area
                                                         /lambda_area_);
                
            }

            
            // Do orig tree

            model_tree->set_R(test_R);
            query_tree->set_R(test_R);

            model_tree->set_ds(model_sample_ds);
            query_tree->set_ds(query_sample_ds);

            model_tree->set_scale_ratio(model_scale_ratio);
            query_tree->set_scale_ratio(query_scale_ratio);

            model_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_,circular_ends_,combined_edit_);
            query_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_,circular_ends_,combined_edit_);

            // Do mirror tree
            
            model_mirror_tree->set_R(test_R);
            query_mirror_tree->set_R(test_R);

            model_mirror_tree->set_ds(model_sample_ds);
            query_mirror_tree->set_ds(query_sample_ds);

            model_mirror_tree->set_scale_ratio(model_scale_ratio);
            query_mirror_tree->set_scale_ratio(query_scale_ratio);

            model_mirror_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_,circular_ends_,combined_edit_);
            query_mirror_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_,circular_ends_,combined_edit_);

            vcl_set<double> dists;

            dists.insert(1.0e6);

            // No mirroring
            double c1=edit_distance(model_tree,query_tree,test_R,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    false,
                                    (*dists.begin()));
            dists.insert(c1);
            double c2=edit_distance(query_tree,model_tree,test_R,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    true,
                                    (*dists.begin()));
            dists.insert(c2);
            
            // Query mirror
            double c3=edit_distance(model_tree,query_mirror_tree,test_R,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    false,
                                    (*dists.begin()));
            dists.insert(c3);
            double c4=edit_distance(query_mirror_tree,model_tree,test_R,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    true,
                                    (*dists.begin()));
            dists.insert(c4);

            // Model mirror
            double c5=edit_distance(model_mirror_tree,query_tree,test_R,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    false,
                                    (*dists.begin()));
            dists.insert(c5);
            double c6=edit_distance(query_tree,model_mirror_tree,test_R,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    true,
                                    (*dists.begin()));
            dists.insert(c6);


            ed_matrix(m,q)=(*dists.begin());

            // Perform shape alignment
            vgl_polygon<double> poly=query_polygons_[q];


            if ( tree1_mirror_ )
            {
                if ( switched_ )
                {
                    shape_alignment(poly,
                                    model_tree,
                                    query_mirror_tree,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    output_binary_file);
                }
                else
                {
                    shape_alignment(poly,
                                    model_mirror_tree,
                                    query_tree,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    output_binary_file);
                
                }
            }
            else if ( tree2_mirror_ )
            {
                
                if ( switched_ )
                {
                    shape_alignment(poly,
                                    model_mirror_tree,
                                    query_tree,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    output_binary_file);
                }
                else
                {
                    shape_alignment(poly,
                                    model_tree,
                                    query_mirror_tree,
                                    curve_list1,
                                    curve_list2,
                                    map_list,
                                    output_binary_file);
                
                }
            
            }
            else
            {
                shape_alignment(poly,model_tree,query_tree,
                                curve_list1,
                                curve_list2,
                                map_list,
                                output_binary_file);
            }
            
            curve_list1.clear();
            curve_list2.clear();
            map_list.clear();
        }

        model_trees_.erase(model_trees_.begin());

        ++m;

    }

    // close dc file
    output_binary_file.close();

    // Write out shape matrix
    vcl_ofstream file(shape_matrix_file_.c_str());
    ed_matrix.print(file);
    file.close();
    

}

void dbskr_align_shapes::load_esf(vcl_string& filename,bool flag)
{




    vcl_ifstream esf_file(filename.c_str());

    if ( !esf_file.is_open() )
    {
        vcl_cerr<<"Error opening "<<filename<<vcl_endl;
        return;
    }

    dbsk2d_xshock_graph_fileio loader;

    unsigned int line_number=0;

    vcl_string line;
    while ( vcl_getline (esf_file,line) )
    {

        // Load in two of the same one for mirroring one for not
        dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(line);
        dbsk2d_shock_graph_sptr hor_sg = loader.load_xshock_graph(line);

        // compute bounding box
        dbsk2d_compute_bounding_box(sg);

        // Get the bounding box for the shock graph
        vsol_box_2d_sptr bbox = sg->get_bounding_box();

        // Get the width for the bounding box
        double width = bbox->width();

        // Compute area, and figure out new sample ds
        vgl_polygon<double> polygon(1);
        double area=compute_boundary(sg,polygon);
        double lambda_ds=scurve_sample_ds_;

        if ( lambda_scaling_ )
        {
            lambda_ds=scurve_sample_ds_*vcl_sqrt(area
                                                 /lambda_area_);
        }

        //: prepare the trees 
        dbskr_tree_sptr tree = new dbskr_tree(sg,false,0,
                                              lambda_ds,
                                              scurve_interpolate_ds_,
                                              scurve_matching_R_);
        tree->acquire_tree_topology();
        tree->set_area(area);

        //: prepare mirror tree
        dbsk2d_hor_flip_shock_graph(hor_sg);
        dbskr_tree_sptr tree_mirror = new dbskr_tree(hor_sg,true,width,
                                                     lambda_ds,
                                                     scurve_interpolate_ds_,
                                                     scurve_matching_R_);
        tree_mirror->acquire_tree_topology();
        tree_mirror->set_area(area);

        // store away tree
        vcl_pair<dbskr_tree_sptr,dbskr_tree_sptr> pair(tree,tree_mirror);

        if ( flag )
        {
            model_trees_.push_back(pair);
        }
        else
        {
            query_trees_.push_back(pair);
            query_polygons_.push_back(polygon);
        }

        ++line_number;

    }
    
    esf_file.close();
}

double dbskr_align_shapes::compute_boundary(dbsk2d_shock_graph_sptr& sg,
                                          vgl_polygon<double>& final_poly)
{


    double G = vcl_pow(2.0,-25.0);
    vcl_vector<vgl_polygon<double> > fragments;

    //1) Initialize labels and connected components
    dbsk2d_shock_graph::edge_iterator eit = sg->edges_begin();
    for (; eit != sg->edges_end(); eit++)
    {
        dbsk2d_xshock_edge* edge = dynamic_cast<dbsk2d_xshock_edge*>
            (eit->ptr());

        vcl_vector<vgl_point_2d<double> > pts;

        edge->get_fragment_boundary(pts);

        vgl_polygon<double> mvf(pts,1);

        for (unsigned int s = 0; s < mvf.num_sheets(); ++s)
        {
            for (unsigned int p = 0; p < mvf[s].size(); ++p)
            { 
                mvf[s][p].x()=
                    (vcl_floor((mvf[s][p].x()/G)+0.5))*G;
                mvf[s][p].y()=
                    (vcl_floor((mvf[s][p].y()/G)+0.5))*G;
            }
        }

        fragments.push_back(mvf);
    }

    dbsk2d_shock_graph::vertex_iterator vit = sg->vertices_begin();
    for (; vit != sg->vertices_end(); vit++)
    {
        dbsk2d_shock_node* vertex=(vit->ptr());

        vcl_vector<vgl_point_2d<double> > pts;

        if ( vertex->type() == dbsk2d_shock_node::A3 )
        {
            dbsk2d_shock_edge_sptr link = (*vertex->out_edges_begin());
            dbsk2d_xshock_edge* edge = dynamic_cast<dbsk2d_xshock_edge*>
                (link.ptr());
            
            dbsk2d_xshock_sample_sptr first_sample=edge->first_sample();

            double speed               = first_sample->speed;
            vgl_point_2d<double> sh_pt = first_sample->pt;
            double time                = first_sample->radius;
            double theta               = first_sample->theta;

            double phi(0);
            if (speed != 0 && speed < 99990) //100000 signals infinity
            {
                phi = vcl_acos(-1/speed);
            }
            else
            {
                phi = vnl_math::pi/2;
            }

            double cur_tan=phi+theta;
            double end_tan=theta-phi;

            if (end_tan < cur_tan)
            {
                end_tan += 2*vnl_math::pi;
            }

            pts.push_back(sh_pt);
            pts.push_back(first_sample->left_bnd_pt);

            cur_tan +=0.02;
            while (cur_tan <= end_tan)
            {
                pts.push_back(_translatePoint(sh_pt, cur_tan, time));
                cur_tan += 0.02; //increment the tangent angle (going CCW)
            }


            pts.push_back(first_sample->right_bnd_pt);
            pts.push_back(sh_pt);
        }
        else if ( vertex->type() == dbsk2d_shock_node::TERMINAL )
        {
            dbsk2d_shock_edge_sptr link = (*vertex->in_edges_begin());
            dbsk2d_xshock_edge* edge = dynamic_cast<dbsk2d_xshock_edge*>
                (link.ptr());
            
            dbsk2d_xshock_sample_sptr last_sample=edge->last_sample();

            double speed               = last_sample->speed;
            vgl_point_2d<double> sh_pt = last_sample->pt;
            double time                = last_sample->radius;
            double theta               = last_sample->theta;

            double phi(0);
            if (speed != 0 && speed < 99990) //100000 signals infinity
            {
                phi = vcl_acos(-1/speed);
            }
            else
            {
                phi = vnl_math::pi/2;
            }

            double end_tan=phi+theta;
            double cur_tan=theta-phi;

            if (end_tan < cur_tan)
            {
                end_tan += 2*vnl_math::pi;
            }

            pts.push_back(sh_pt);
            pts.push_back(last_sample->right_bnd_pt);

            cur_tan +=0.02;
            while (cur_tan <= end_tan)
            {
                pts.push_back(_translatePoint(sh_pt, cur_tan, time));
                cur_tan += 0.02; //increment the tangent angle (going CCW)
            }


            pts.push_back(last_sample->left_bnd_pt);
            pts.push_back(sh_pt);



        }
        else 
        {
            
            if ( vertex->degree() == 2 )
            {
                dbsk2d_xshock_edge* link1(0);
                dbsk2d_xshock_edge* link2(0);
                

                vgl_point_2d<double> sh_pt;
                double time(0);                

                double cur_tan(0),end_tan(0);

                if ( vertex->type() == dbsk2d_shock_node::SOURCE )
                {
                    dbsk2d_shock_node::edge_iterator eit= (
                        vertex->out_edges_begin());

                    link1 = 
                        dynamic_cast<dbsk2d_xshock_edge*>
                        ((*eit).ptr());
                    ++eit;
                    link2 = 
                        dynamic_cast<dbsk2d_xshock_edge*>
                        ((*eit).ptr());
                    
                    dbsk2d_xshock_sample_sptr link1_sample=link1
                        ->first_sample();
                    dbsk2d_xshock_sample_sptr link2_sample=link2
                        ->first_sample();
                    
                    if ( link1_sample->left_bnd_pt != 
                         link2_sample->right_bnd_pt )
                    {
                    }


                    if ( link1_sample->right_bnd_pt != 
                         link2_sample->left_bnd_pt )
                    {
                    }

                }
                else if ( vertex->type() == dbsk2d_shock_node::SINK )
                {
                    dbsk2d_shock_node::edge_iterator eit= (
                        vertex->in_edges_begin());

                    link1 = 
                        dynamic_cast<dbsk2d_xshock_edge*>
                        ((*eit).ptr());
                    ++eit;
                    link2 = 
                        dynamic_cast<dbsk2d_xshock_edge*>
                        ((*eit).ptr());

                    dbsk2d_xshock_sample_sptr link1_sample=link1
                        ->last_sample();
                    dbsk2d_xshock_sample_sptr link2_sample=link2
                        ->last_sample();
                    
                    vgl_point_2d<double> footpt1;
                    vgl_point_2d<double> footpt2;

                    double speed_link1=link1_sample->speed;
                    double theta_link1=link1_sample->theta;
                    double phi_link1(0);
                    vgl_point_2d<double> sh_pt = link1_sample->pt;
                    double time = link1_sample->radius;

                    if (speed_link1 != 0 && speed_link1 < 99990)
                    {
                        phi_link1 = vcl_acos(-1/speed_link1);
                    }
                    else
                    {
                        phi_link1 = vnl_math::pi/2;
                    }


                    double speed_link2=link2_sample->speed;
                    double theta_link2=link2_sample->theta;
                    double phi_link2(0);

                    if (speed_link2 != 0 && speed_link2 < 99990)
                    {
                        phi_link2 = vcl_acos(-1/speed_link2);
                    }
                    else
                    {
                        phi_link2 = vnl_math::pi/2;
                    }
                    

                    if ( link1_sample->left_bnd_pt != 
                         link2_sample->right_bnd_pt )
                    {
                        end_tan=theta_link1-phi_link1;
                        cur_tan=theta_link2+phi_link2;

                        footpt1=link2_sample->right_bnd_pt;
                        footpt2=link1_sample->left_bnd_pt;

                    }

                    if ( link1_sample->right_bnd_pt != 
                         link2_sample->left_bnd_pt )
                    {
                        cur_tan=theta_link1+phi_link1;
                        end_tan=theta_link2-phi_link2;

                        footpt1=link1_sample->right_bnd_pt;
                        footpt2=link2_sample->left_bnd_pt;

                    }

                    if (end_tan < cur_tan)
                    {
                        end_tan += 2*vnl_math::pi;
                    }

                    pts.push_back(sh_pt);
                    pts.push_back(footpt1);

                    cur_tan +=0.02;
                    while (cur_tan <= end_tan)
                    {
                        pts.push_back(_translatePoint(sh_pt, cur_tan, time));
                        cur_tan += 0.02;
                    }


                    pts.push_back(footpt2);
                    pts.push_back(sh_pt);

                }
                else
                {

                    link1 = 
                        dynamic_cast<dbsk2d_xshock_edge*>
                        ((*(vertex->in_edges_begin())).ptr());
                    link2 = 
                        dynamic_cast<dbsk2d_xshock_edge*>
                        ((*(vertex->out_edges_begin())).ptr());


                }

            }

        }

 
        if ( pts.size() )
        {
            vgl_polygon<double> mvf(pts,1);
            
            for (unsigned int s = 0; s < mvf.num_sheets(); ++s)
            {
                for (unsigned int p = 0; p < mvf[s].size(); ++p)
                { 
                    mvf[s][p].x()=
                        (vcl_floor((mvf[s][p].x()/G)+0.5))*G;
                    mvf[s][p].y()=
                        (vcl_floor((mvf[s][p].y()/G)+0.5))*G;
                }
            }

            fragments.push_back(mvf);
        }
    
    }

    // Start polygon
    vgl_polygon<double> start_poly=fragments[0];

    unsigned int s=1;
    for ( ; s < fragments.size() ; ++s)
    {

        
        //Take temp
        vgl_polygon<double> temp=fragments[s];

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
        vgl_polygon<double> tempy(start_poly[s]);
        double area_temp = vgl_area(tempy);
        if ( area_temp > area )
        {
            area = area_temp;
            f_index=s;

        }
        
    }
    
    final_poly.push_back(start_poly[f_index]); 

    return area;

}
//: Match
double dbskr_align_shapes::edit_distance(
    dbskr_tree_sptr& tree1,
    dbskr_tree_sptr& tree2,
    float test_curve_matching_R,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool switched,
    double prev_distance)
{
    //instantiate the edit distance algorithms
    dbskr_tree_edit edit(tree1, tree2,circular_ends_,localized_edit_);
    
    edit.save_path(true);
    edit.set_curvematching_R(test_curve_matching_R);
    edit.set_use_approx(use_approx_);
    
    if (!edit.edit()) 
    {
        vcl_cerr << "Problems in editing trees"<<vcl_endl;
        return false;
    }
    
    double val = edit.final_cost();
    
    double norm_val = val/(tree1->total_splice_cost()+
                           tree2->total_splice_cost());
    
    // vcl_cout << "final cost: " << val << " final norm cost: " 
    //          << norm_val << "( tree1 tot splice: " 
    //          << tree1->total_splice_cost() << ", tree2: " 
    //          << tree2->total_splice_cost() << ")" << vcl_endl;

    if ( norm_val < prev_distance)
    {
        // Get correspondece
        

        //: Get path key
        vcl_vector< pathtable_key > path_map;

        curve_list1.clear();
        curve_list2.clear();
        map_list.clear();
        edit.get_correspondence(curve_list1,
                                curve_list2,
                                map_list,
                                path_map);
        
        switched_=switched;

        tree1_mirror_=tree1->mirror();
        tree2_mirror_=tree2->mirror();
    }

    return norm_val;
}



// Get dense correspondence between two shapes
void dbskr_align_shapes::shape_alignment(
    vgl_polygon<double>& poly,
    dbskr_tree_sptr& model_tree,
    dbskr_tree_sptr& query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_ofstream& output_binary_file)
{

    // write out results to binary file

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> query_pt(x,y);

            vgl_point_2d<double> model_rt(0,0),query_rt(0,0);

            int curve_list_id(0);

            double query_width=query_tree->get_width();
            double model_width=model_tree->get_width();

            vgl_point_2d<double> mapping_pt=
                find_part_correspondences_qm(query_pt,
                                             curve_list1,
                                             curve_list2,
                                             map_list,
                                             switched_,
                                             query_width,
                                             model_tree
                                             ->get_scale_ratio(),
                                             query_tree
                                             ->get_scale_ratio());
            if ( mapping_pt.x() != -1 )
            {
                
                if ( model_tree->mirror() )
                {
                    mapping_pt.set((model_width-mapping_pt.x()),
                                   mapping_pt.y());
                    
                }
               
            }

            output_binary_file.write(reinterpret_cast<char *>(&mapping_pt.x()),
                                     sizeof(double));
            output_binary_file.write(reinterpret_cast<char *>(&mapping_pt.y()),
                                     sizeof(double));


        }
    }

}



vgl_point_2d<double> dbskr_align_shapes::
find_part_correspondences_qm(
    vgl_point_2d<double> query_pt,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool flag,
    double width,
    double model_scale_ratio,
    double query_scale_ratio)
{

    vgl_point_2d<double> mapping_pt;

    vgl_point_2d<double> ps1(query_pt);
    
    if ( width > 0 )
    {
        ps1.set((width-ps1.x())*query_scale_ratio,
                ps1.y()*query_scale_ratio);
    }
    else
    {
        ps1.set(ps1.x()*query_scale_ratio,ps1.y()*query_scale_ratio);
    }

    if ( !flag )
    {
        bool in_part=false;
        unsigned int c=0;
        for ( ; c < curve_list2.size() ; ++c)
        {
            vgl_polygon<double> poly(1);
            dbskr_scurve_sptr sc1=curve_list2[c];
            sc1->get_polygon(poly,width);

            if ( poly.contains(query_pt.x(),query_pt.y()))
            {
                in_part=true;
                break;
            }
            
        }
        
        if ( !in_part)
        {
            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        dbskr_scurve_sptr model_curve=curve_list2[c];
        dbskr_scurve_sptr query_curve=curve_list1[c];

        // Find point in model curve
        vgl_point_2d<double> int_pt;
        bool found=model_curve->intrinsinc_pt(ps1,int_pt);

        if ( !found )
        {

            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        vcl_vector<vcl_pair<int,int> > curve_map=map_list[c];

        double index=int_pt.x();

        double index_ratio=0.0;

        unsigned int v=0; 

        int model_int_diff(0);
        double int_diff(0.0);

        for ( ; v < curve_map.size()-1 ; ++v)
        {

            int lower=curve_map[v].second;
            int upper=curve_map[v+1].second;

            if ( upper < lower )
            {
                int temp=upper;
                upper=lower;
                lower=temp;
            }
            
            if ( index >= lower &&
                 index < upper )
            {
                model_int_diff=upper-lower;
                int_diff=index-lower;
                break;
                
            }

            
        }

        
        // Find mapping point
        int start_index=curve_map[v].first;
        int stop_index=curve_map[v+1].first;

        if ( stop_index < start_index)
        {
            int temp=stop_index;
            stop_index=start_index;
            start_index=temp;
        }

        int query_int_diff=stop_index-start_index;

        double s_map(0.0);

        if ( query_int_diff == 0 )
        {
            s_map=start_index;

        }
        else
        {
            double ratio=((double) query_int_diff)/((double) model_int_diff);
            s_map = start_index+int_diff*ratio;

        }

        double t_rad_model = model_curve->interp_radius(int_pt.x());
        double t_rad_query = query_curve->interp_radius(s_map);

        double t_map = int_pt.y()*(t_rad_query/t_rad_model);

        mapping_pt = query_curve->fragment_pt(s_map,
                                              t_map);

        
    }
    else
    {
        bool in_part=false;
        unsigned int c=0;
        for ( ; c < curve_list1.size() ; ++c)
        {
            vgl_polygon<double> poly(1);
            dbskr_scurve_sptr sc1=curve_list1[c];
            sc1->get_polygon(poly,width);

            if ( poly.contains(query_pt.x(),query_pt.y()))
            {
                in_part=true;
                break;
            }
            
        }

        if ( !in_part )
        {

            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        dbskr_scurve_sptr model_curve=curve_list1[c];
        dbskr_scurve_sptr query_curve=curve_list2[c];

        // Find point in model curve
        vgl_point_2d<double> int_pt;
        bool found = model_curve->intrinsinc_pt(ps1,int_pt);

        if ( !found )
        {
            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        vcl_vector<vcl_pair<int,int> > curve_map=map_list[c];

        double index=int_pt.x();

        double index_ratio=0.0;

        unsigned int v=0; 

        int model_int_diff(0);
        double int_diff(0.0);

        for ( ; v < curve_map.size()-1 ; ++v)
        {

            int lower=curve_map[v].first;
            int upper=curve_map[v+1].first;

            if ( upper < lower )
            {
                int temp=upper;
                upper=lower;
                lower=temp;
            }
            
            if ( index >= lower &&
                 index < upper )
            {
                model_int_diff=upper-lower;
                int_diff=index-lower;
                break;
                
            }

            
        }

        
        // Find mapping point
        int start_index=curve_map[v].second;
        int stop_index=curve_map[v+1].second;

        if ( stop_index < start_index)
        {
            int temp=stop_index;
            stop_index=start_index;
            start_index=temp;
        }

        int query_int_diff=stop_index-start_index;

        double s_map(0.0);

        if ( query_int_diff == 0 )
        {
            s_map=start_index;

        }
        else
        {
            double ratio=((double) query_int_diff)/((double) model_int_diff);
            s_map = start_index+int_diff*ratio;

        }

        double t_rad_model = model_curve->interp_radius(int_pt.x());
        double t_rad_query = query_curve->interp_radius(s_map);

        double t_map = int_pt.y()*(t_rad_query/t_rad_model);

        mapping_pt = query_curve->fragment_pt(s_map,
                                              t_map);


    }


    mapping_pt.set(mapping_pt.x()/model_scale_ratio,
                   mapping_pt.y()/model_scale_ratio);

    return mapping_pt;
}
