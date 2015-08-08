// This is brcv/shp/dbskfg/dbskr_align_shapes.cxx

//:
// \file

#include <shape_align/dbskr_align_shapes.h>

#include <dbsk2d/algo/dbsk2d_hor_flip_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbskr/dbskr_tree_edit.h>
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

    vcl_string line;
    while ( vcl_getline (esf_file,line) )
    {

        dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(line);

        //: prepare the trees 
        dbskr_tree_sptr tree = new dbskr_tree(sg);
        tree->acquire_tree_topology();

        //: prepare mirror tree
        dbsk2d_hor_flip_shock_graph(sg);
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

    }
    
    esf_file.close();



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
