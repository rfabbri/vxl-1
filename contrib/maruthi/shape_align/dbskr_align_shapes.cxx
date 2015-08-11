// This is brcv/shp/dbskfg/dbskr_align_shapes.cxx

//:
// \file

#include <shape_align/dbskr_align_shapes.h>

#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include <dbsk2d/algo/dbsk2d_hor_flip_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_xshock_node.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>

#include <dbskr/dbskr_tree_edit.h>

#include <vcl_sstream.h>

//: Constructor
dbskr_align_shapes::dbskr_align_shapes(
    vcl_string model_filename,
    vcl_string query_filename,
    double lambda_area):
    lambda_area_(lambda_area),
    switched_(false),
    tree1_mirror_(false),
    tree2_mirror_(false)
{
    vcl_cout<<"Loading Model ESF Files"<<vcl_endl;
    load_esf(model_filename,true);
    vcl_cout<<"Loading Query ESF Files"<<vcl_endl;
    load_esf(query_filename,false);
}

//: Destructor
dbskr_align_shapes::~dbskr_align_shapes() 
{
 
}


//: Match
void dbskr_align_shapes::match()
{
    // Match

    for ( unsigned int m=0; m < model_trees_.size() ; ++m)
    {
        for ( unsigned int q=0; q < query_trees_.size() ; ++q)
        {

            // Compute all pairs of edit distance
            dbskr_tree_sptr model_tree=model_trees_[m].first;
            dbskr_tree_sptr model_mirror_tree=model_trees_[m].second;

            dbskr_tree_sptr query_tree=query_trees_[q].first;
            dbskr_tree_sptr query_mirror_tree=query_trees_[q].second;
            
            double c1=edit_distance(model_tree,query_tree);
            double c2=edit_distance(query_tree,model_tree,true,c1);

            double c3=edit_distance(model_tree,query_mirror_tree,c2);
            double c4=edit_distance(query_mirror_tree,model_tree,true,c3);

            double c5=edit_distance(model_mirror_tree,query_tree,c4);
            double c6=edit_distance(query_tree,model_mirror_tree,true,c5);

        }

    }
 

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

        dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(line);

        vgl_polygon<double> model_poly(1);

        compute_boundary(sg,model_poly);
        {
            vcl_stringstream stream;
            stream<<"model_"<<line_number<<".txt";

            vcl_ofstream model_file(stream.str().c_str());
            for (unsigned int s = 0; s < model_poly.num_sheets(); ++s)
            {
                for (unsigned int p = 0; p < model_poly[s].size(); ++p)
                {
                    model_file<<model_poly[s][p].x()
                              <<","<<model_poly[s][p].y()<<vcl_endl;
                }
            }
            model_file.close();
        }



        //: prepare the trees 
        dbskr_tree_sptr tree = new dbskr_tree(sg);
        bool okay=tree->acquire_tree_topology();

        if ( !okay )
        {
            vcl_cerr<<"This is very bad"<<vcl_endl;
        }
        //: prepare mirror tree
        //dbsk2d_hor_flip_shock_graph(sg);
        dbskr_tree_sptr tree_mirror = new dbskr_tree(sg,true);
        tree_mirror->acquire_tree_topology();

        vcl_pair<dbskr_tree_sptr,dbskr_tree_sptr> pair(tree,tree_mirror);

        if ( flag )
        {
            model_trees_.push_back(pair);
        }
        else
        {
            query_trees_.push_back(pair);
        }

        ++line_number;

    }
    
    esf_file.close();
    // vcl_cout<<"Finished loading all model trees"<<vcl_endl;
    // for ( unsigned int k=0; k < model_trees_.size() ; ++k)
    // {
    //     vgl_polygon<double> model_poly(1);

    //     dbskr_tree_sptr mtree=model_trees_[k].first;
        
    //     vcl_stringstream stream;
    //     stream<<"model_"<<k<<".txt";

    //     mtree->compute_reconstructed_boundary_polygon(model_poly);


    // }
    exit(0);
}

void dbskr_align_shapes::compute_boundary(dbsk2d_shock_graph_sptr& sg,
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
            
            // if ( vertex->degree() == 2 )
            // {
            //     if ( vertex->out_degree() )
            //     {
            //         dbsk2d_shock_edge_sptr link = (*vertex->out_edges_begin());
            //         dbsk2d_xshock_edge* edge = dynamic_cast<dbsk2d_xshock_edge*>
            //             (link.ptr());


            // }

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

}
//: Match
double dbskr_align_shapes::edit_distance(dbskr_tree_sptr& tree1,
                                         dbskr_tree_sptr& tree2,
                                         bool switched,
                                         double prev_distance)
{


    //instantiate the edit distance algorithms
    dbskr_tree_edit edit(tree1, tree2, true,false);
    
    edit.save_path(true);
    //edit.set_curvematching_R(scurve_matching_R);
    //edit.set_use_approx(use_approx);
    
    if (!edit.edit()) 
    {
        vcl_cerr << "Problems in editing trees"<<vcl_endl;
        return false;
    }
    
    double val = edit.final_cost();
    
    double norm_val = val/(tree1->total_splice_cost()+
                           tree2->total_splice_cost());
    

    if ( norm_val < prev_distance)
    {
        // Get correspondece
        

        //: Get path key
        vcl_vector< pathtable_key > path_map;

        // First clear out what we have
        curve_list1_.clear();
        curve_list2_.clear();
        map_list_.clear();

        edit.get_correspondence(curve_list1_,
                                curve_list2_,
                                map_list_,
                                path_map);
        
        switched_=switched;

        tree1_mirror_=tree1->mirror();
        tree2_mirror_=tree2->mirror();
    }

    return norm_val;
}



// Get dense correspondence between two shapes
void dbskr_align_shapes::shape_alignment(
    dbskr_tree_sptr query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool query_mirror,
    bool switched)
{

    

}
