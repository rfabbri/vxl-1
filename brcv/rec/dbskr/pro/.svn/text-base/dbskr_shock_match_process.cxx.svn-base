// This is dbskr/pro/dbskr_shock_match_process.cxx

//:
// \file

#include <dbskr/pro/dbskr_shock_match_process.h>

#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_localize_match.h>
#include <dbskr/dbskr_sm_cor.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_ctime.h>
#include <vul/vul_timer.h>

#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/dbskr_tree_edit_coarse.h>
#include <dbskr/dbskr_tree_edit_combined.h>
#include <dbsk2d/algo/dbsk2d_hor_flip_shock_graph.h>

dbskr_shock_match_process::dbskr_shock_match_process() : bpro1_process()
{
  if ( !parameters()->add( "Load shock graph 1 from esf file?" , "-load1" , (bool) false ) ||
       !parameters()->add( "Input esf file 1 <filename...>" , 
                           "-esf1" ,
                           bpro1_filepath("Z:\\projects\\kimia\\categorization\\99-database\\esf\\f16.esf","*.esf") ) ||
       !parameters()->add( "Load shock graph 2 from esf file?" , "-load2" , (bool) false ) ||
       !parameters()->add( "Input esf file 2 <filename...>" , 
                           "-esf2" ,
                           bpro1_filepath("Z:\\projects\\kimia\\categorization\\99-database\\esf\\dude8.esf","*.esf") ) ||
       !parameters()->add( "Prune Trees?" , "-prune_tree" , (bool) false ) ||
       !parameters()->add( "Prune Threshold?" , "-prune_thres" , (double) 5.0f ) ||
       !parameters()->add( "normalize: using reconst bnd length (else total splice cost)" , "-norm_reconst", (bool) true ) ||

       // some of the matching parameters are not carried over via the shgm file 
       // so set scurve sample ds, scurve matching R, elastic splice cost etc (6 parameters below) as was set to generate this shgm file
       // otherwise the displayed correspondence will be ok but the costs shown will be different from the ones used to generate this shgm file
       !parameters()->add( "Match file exists? (if exists carry over matching parameters below!)" , "-fileexists" , (bool) false ) ||
       !parameters()->add( "Write out shock matching results to shgm file?","-write_out",(bool) false) ||
       !parameters()->add( "Input/Output shgm file <filename...>" , 
                           "-shgm" ,
                           bpro1_filepath("Z:\\projects\\kimia\\categorization\\99-database\\shgm\\tmp-f16-dude8.shgm","*.shgm") ) ||
       !parameters()->add("Sampling ds to reconstruct the scurve", "-scurve_sample_ds", (float) 5.0f ) ||
       !parameters()->add("Interpolation ds to get densely interpolated versions of the scurves: meaningful if localized_edit option is ON","-scurve_interpolate_ds" , (float) 1.0f ) ||
       !parameters()->add("Improves elastic matching cost of scurves using the densely interpolated version", "-localized_edit", (bool) false ) ||
       !parameters()->add("use the elastic splice cost computation","-elastic_splice_cost", (bool) false) ||
       !parameters()->add("curve matching R that weighs bending over stretching","-curve_matching_R", (double) 6.0f) ||
       !parameters()->add("use circular completions at leaf branches during shock matching","-circular_ends", (bool) true) ||
       !parameters()->add("use combined edit cost during shock matching","-combined_edit", (bool) false) ||
       !parameters()->add("run coarse edit distance algorithm","-coarse_edit", (bool) false) ||
       !parameters()->add("use cost approximation for speed purposes","-use_approx", (bool) true) ||
       !parameters()->add("perform mirror matching","-mirror_match",(bool) false)

       )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Clone the process
bpro1_process*
dbskr_shock_match_process::clone() const
{
  return new dbskr_shock_match_process(*this);
}

vcl_vector< vcl_string > dbskr_shock_match_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  to_return.push_back( "shock" );
  return to_return;
}

vcl_vector< vcl_string > dbskr_shock_match_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("shock_match");
  return to_return;
}

bool dbskr_shock_match_process::execute()
{

    bool load1=false, load2=false;
    parameters()->get_value( "-load1" , load1);
    parameters()->get_value( "-load2" , load2);
    bpro1_filepath input_path;
    parameters()->get_value( "-esf1" , input_path);
    vcl_string esf_file1 = input_path.path;
    parameters()->get_value( "-esf2" , input_path);
    vcl_string esf_file2 = input_path.path;  
    parameters()->get_value( "-shgm" , input_path);
    vcl_string shgm_file = input_path.path;

    bool perform_mirror_matching;
    parameters()->get_value( "-mirror_match", perform_mirror_matching);

    dbsk2d_xshock_graph_fileio loader;

    dbsk2d_shock_storage_sptr shock1, shock2;
    dbsk2d_shock_graph_sptr sg1, sg2, sg2_flip;

    if (load1) {
        sg1 = loader.load_xshock_graph(esf_file1);
    } else {
        // get input storage class
        shock1.vertical_cast(input_data_[0][0]);
        sg1 = shock1->get_shock_graph();
    }

    if (load2) {
        sg2 = loader.load_xshock_graph(esf_file2);
    } else {
        shock2.vertical_cast(input_data_[0][1]);
        sg2 = shock2->get_shock_graph();
    }
  
    if (!sg1 || !sg2)
    {
        vcl_cerr << "Problems in getting shock graphs!\n";
        return false;
    }

    bool exists=false;
    parameters()->get_value( "-fileexists" , exists);
  
    bool prune_tree=false;
    parameters()->get_value( "-prune_tree" , prune_tree);
    double prune_thres=0;
    parameters()->get_value( "-prune_thres" , prune_thres);

    bool norm_reconst;
    parameters()->get_value( "-norm_reconst", norm_reconst);

    bool write_out(false);
    parameters()->get_value( "-write_out",write_out);

    //get the subsampling parameter
    bool elastic_splice_cost    = false; 
    float scurve_sample_ds      = 0.0f;
    float scurve_interpolate_ds = 1.0f;
    bool localized_edit         = false;
    double scurve_matching_R    = 6.0f;
    bool circular_ends          = false;
    bool combined_edit          = false;
    bool coarse_edit            = false;
    bool use_approx             = true;

    parameters()->get_value("-elastic_splice_cost"  , elastic_splice_cost); 
    parameters()->get_value("-scurve_sample_ds"     , scurve_sample_ds);
    parameters()->get_value("-scurve_interpolate_ds", scurve_interpolate_ds); 
    parameters()->get_value("-localized_edit"       , localized_edit ); 
    parameters()->get_value("-curve_matching_R"     , scurve_matching_R ); 
    parameters()->get_value("-circular_ends"        , circular_ends); 
    parameters()->get_value("-combined_edit"        , combined_edit); 
    parameters()->get_value("-coarse_edit"          , coarse_edit); 
    parameters()->get_value("-use_approx"           , use_approx);
  
    //: prepare the trees also
    dbskr_tree_sptr tree1 = new dbskr_tree(scurve_sample_ds, 
                                           scurve_interpolate_ds, 
                                           scurve_matching_R);

    dbskr_tree_sptr tree2 = new dbskr_tree(scurve_sample_ds, 
                                           scurve_interpolate_ds, 
                                           scurve_matching_R);

    if (prune_tree)
    {
        tree1->acquire_and_prune(sg1, prune_thres, 
                                 elastic_splice_cost, 
                                 circular_ends, combined_edit);
    }
    else
    {
        tree1->acquire(sg1, elastic_splice_cost, circular_ends, combined_edit);
    }

    if (prune_tree)
    {
        tree2->acquire_and_prune(sg2, prune_thres, 
                                 elastic_splice_cost, 
                                 circular_ends, combined_edit);
    }
    else
    {
        tree2->acquire(sg2, elastic_splice_cost, circular_ends, combined_edit);
    }

    // create the output storage class
    dbskr_shock_match_storage_sptr output_match = 
        dbskr_shock_match_storage_new();
  
    if (exists)  // no need for matching, the parameters for reconstruction of 
        // scurves and matching of scurves should be 
        // set as was used during the generation of this shgm file 
        // (these parameter values should be carried over
        // via the shgm file itself but unfortunately this is not 
        // the case in the current implementation)
    {  
        dbskr_sm_cor_sptr sm_cor = new dbskr_sm_cor(tree1, tree2);
    
        // should be called before read_and_construct_from_shgm()
        sm_cor->edit_params_.curve_matching_R_ = scurve_matching_R;  
    
        sm_cor->edit_params_.combined_edit_ = combined_edit;
        sm_cor->edit_params_.localized_edit_ = localized_edit;
        sm_cor->edit_params_.circular_ends_ = circular_ends;
        sm_cor->edit_params_.coarse_edit_ = coarse_edit;
        sm_cor->edit_params_.elastic_splice_cost_ = elastic_splice_cost;
        sm_cor->edit_params_.scurve_interpolate_ds_ = scurve_interpolate_ds;
        sm_cor->edit_params_.scurve_sample_ds_ = scurve_sample_ds;
    
        sm_cor->read_and_construct_from_shgm(shgm_file);
        output_match->set_sm_cor(sm_cor);
    } 
    else  // do the matching
    { 
        vcl_cout << "matching shock graphs...\n";
        vul_timer t;
        t.mark();

        dbskr_sm_cor_sptr sm_cor; float val, norm_val = 0; float val_flip, norm_val_flip = 0;

        if (!coarse_edit && !combined_edit) {
            vcl_cout << "matching shock graphs using Sebastian's "
                     << "original edit distance"<<vcl_endl;

            //instantiate the edit distance algorithms
            dbskr_tree_edit edit(tree1, tree2, circular_ends, localized_edit);

            edit.save_path(true);
            edit.set_curvematching_R(scurve_matching_R);
            edit.set_use_approx(use_approx);

            if (!edit.edit()) {
                vcl_cerr << "Problems in editing trees"<<vcl_endl;
                return false;
            }

            val = edit.final_cost();
                    
            if ( !norm_reconst )
                    
            {
                norm_val = val/(tree1->total_splice_cost()+
                                tree2->total_splice_cost());
            }
            else
            {
                norm_val = val/(tree1->total_reconstructed_boundary_length()+
                                tree2->total_reconstructed_boundary_length());
            }

            //If user wants to perform mirror matching
            if(perform_mirror_matching)
            {
                dbskr_tree_sptr tree2_flip = new dbskr_tree(scurve_sample_ds,
                                                            scurve_interpolate_ds,
                                                            scurve_matching_R);

                if(!dbsk2d_hor_flip_shock_graph(sg2))
                {
                    vcl_cout << "Flip operation is not successful" << vcl_endl;
                    vcl_cout << "The results are not reliable" << vcl_endl;
                }

                if (prune_tree)
                {
                    tree2_flip->acquire_and_prune(sg2, prune_thres, 
                                                  elastic_splice_cost, 
                                                  circular_ends, combined_edit);
                }
                else
                {
                    tree2_flip->acquire(sg2, elastic_splice_cost, circular_ends, combined_edit);
                }

                dbskr_tree_edit edit_flip(tree1, tree2_flip, circular_ends, localized_edit);

                edit_flip.save_path(true);
                edit_flip.set_curvematching_R(scurve_matching_R);
                edit_flip.set_use_approx(use_approx);


                if (!edit_flip.edit()) {
                    vcl_cerr << "Problems in editing trees"<<vcl_endl;
                    return false;
                }

                val_flip = edit_flip.final_cost();
                    
                if ( !norm_reconst )
                {
                    norm_val_flip = val_flip/(tree1->total_splice_cost()+
                                              tree2_flip->total_splice_cost());
                }
                else
                {
                    norm_val_flip = val_flip/(tree1->total_reconstructed_boundary_length()+
                                              tree2_flip->total_reconstructed_boundary_length());
                }

                if(val_flip < val)
                {
                    val = val_flip;
                    norm_val = norm_val_flip;
               
                    sm_cor = edit_flip.get_correspondence(); 
                    sm_cor->set_final_cost(val);
                    sm_cor->set_final_norm_cost(norm_val);

                    if ( !norm_reconst )
                    {
                        vcl_cout << "final cost: " << val << " final norm cost: " 
                                 << norm_val << "( tree1 tot splice: " 
                                 << tree1->total_splice_cost() << ", tree2: " 
                                 << tree2_flip->total_splice_cost() << ")" << vcl_endl;
                    }
                    else
                    {

                        vcl_cout << "final cost: " << val << " final norm cost: " 
                                 << norm_val << "( tree1 tot length: " 
                                 << tree1->total_reconstructed_boundary_length() 
                                 << ", tree2: " 
                                 << tree2_flip->total_reconstructed_boundary_length() << ")" 
                                 << vcl_endl;

                    }

                }
                else
                {
                    dbsk2d_hor_flip_shock_graph(sg2);
                    sm_cor = edit.get_correspondence();
                    sm_cor->set_final_cost(val);
                    sm_cor->set_final_norm_cost(norm_val);

                    if ( !norm_reconst )
                    {
                        vcl_cout << "final cost: " << val << " final norm cost: " 
                                 << norm_val << "( tree1 tot splice: " 
                                 << tree1->total_splice_cost() << ", tree2: " 
                                 << tree2->total_splice_cost() << ")" << vcl_endl;
                    }
                    else
                    {
                        vcl_cout << "final cost: " << val << " final norm cost: " 
                                 << norm_val << "( tree1 tot length: " 
                                 << tree1->total_reconstructed_boundary_length() 
                                 << ", tree2: " 
                                 << tree2->total_reconstructed_boundary_length() << ")" 
                                 << vcl_endl;

                    }

                }
            }
            else
            {
                sm_cor = edit.get_correspondence();
                sm_cor->set_final_cost(val);
                sm_cor->set_final_norm_cost(norm_val);

                if ( !norm_reconst )
                {
                    vcl_cout << "final cost: " << val << " final norm cost: " 
                             << norm_val << "( tree1 tot splice: " 
                             << tree1->total_splice_cost() << ", tree2: " 
                             << tree2->total_splice_cost() << ")" << vcl_endl;
                }
                else
                {
                    vcl_cout << "final cost: " << val << " final norm cost: " 
                             << norm_val << "( tree1 tot length: " 
                             << tree1->total_reconstructed_boundary_length() 
                             << ", tree2: " 
                             << tree2->total_reconstructed_boundary_length() << ")" 
                             << vcl_endl;

                }
            }

            sm_cor->clear_map_list();

            // fills in the cost vectors for debugging
            // sm_cor->recover_dart_ids_and_scurves(); 

            if ( write_out )
            {
                edit.write_shgm(shgm_file);
            }

        } else if (combined_edit) {

            vcl_cout << "matching shock graphs using combined edit"<<vcl_endl;

            //instantiate the edit distance algorithm
            dbskr_tree_edit_combined edit(tree1, tree2, circular_ends);

            edit.save_path(true);
            edit.set_curvematching_R(scurve_matching_R);
            if (!edit.edit()) {
                vcl_cerr << "Problems in editing trees"<<vcl_endl;
                return false;
            }
            val = edit.final_cost();
            if ( !norm_reconst )
            {
                norm_val = val/(tree1->total_splice_cost()
                                +tree2->total_splice_cost());
            }
            else
            {
                norm_val = val/(tree1->total_reconstructed_boundary_length()
                                +tree2->total_reconstructed_boundary_length());
            }
            // sets parameters in sm_cor as in edit
            sm_cor = edit.get_correspondence();  

            sm_cor->set_final_cost(val);
            sm_cor->set_final_norm_cost(norm_val);
            vcl_cout << "final cost: " << val 
                     << " final norm cost: " << norm_val << vcl_endl;
            sm_cor->clear_map_list();
            // fills in the cost vectors for debugging
            // sm_cor->recover_dart_ids_and_scurves(); 

            if ( write_out )
            {
                edit.write_shgm(shgm_file);
            }
        } else { // coarse_edit
            vcl_cout << "matching shock graphs using coarse edit\n";
            dbskr_tree_edit_coarse edit(tree1, tree2, circular_ends);
            //edit.set_circular_ends(circular_ends);  
            // if false no circular completions at the end scurves
            edit.save_path(true);
            edit.set_curvematching_R(scurve_matching_R);
            if (!edit.edit()) {
                vcl_cerr << "Problems in editing trees\n";
                return false;
            }
            val = edit.final_cost();
            if ( !norm_reconst )
            {
                norm_val = val/(tree1->total_splice_cost()+
                                tree2->total_splice_cost());
            }
            else
            {
                norm_val = val/(tree1->total_reconstructed_boundary_length()+
                                tree2->total_reconstructed_boundary_length());
            }
            sm_cor = edit.get_correspondence_just_map();
            sm_cor->set_final_cost(val);
            sm_cor->set_final_norm_cost(norm_val);
            sm_cor->clear_map_list();
            sm_cor->set_tree1(tree1);
            sm_cor->set_tree2(tree2);
        }

        vcl_cout << " cost: " << val << " norm cost: " << norm_val << " time: "
                 << (t.real()/1000.0f) << " secs.\n";
    
        output_match->set_sm_cor(sm_cor);
    }

    output_data_.clear();
    output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_match));

    return true;
}
