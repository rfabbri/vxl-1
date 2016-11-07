//:
// \file
// \brief 
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 07/15/09
//
//

#include "vox_eval_shock_patches.h"
#include "vox_eval_shock_patches_params.h"
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dborl/algo/dborl_xio_image_description.h>
#include <vul/vul_file.h>
#include <dbxml/dbxml_algos.h>
#include <dbxml/dbxml_xio.h>
#include <vsol/vsol_box_2d.h>
#include <dborl/dborl_image_description.h>

//: Default ctor
vox_eval_shock_patches::vox_eval_shock_patches
(vox_eval_shock_patches_params_sptr params):
    params_(params),
    model_id_(),
    ind_(),
    output_file_()
{

    // Nothing else to do her
}

//: Default dtor
vox_eval_shock_patches::~vox_eval_shock_patches()
{
   
}

//: this method actually performs the algorithm
bool vox_eval_shock_patches::process()
{
    
    bool status(false);
    
    // Kick of the steps of processing

    // Reading of index file is also performed in initialize process
    status = initialize();
    if ( status )
    {
        // Move on to matching
        status = perform_eval();
        if ( status )
        {
            // Finally write out results
            status = write_out();
        }
    }
     
    return status;
}

//: this method actually initializes arguments for other helper methods
bool vox_eval_shock_patches::initialize()
{
    //creating a parser for the index file
    dborl_index_parser parser;
    parser.clear();
    ind_ = dborl_index_parser::parse(params_->query_index_filename_(),parser);

    if ( !ind_ )
    {
        vcl_cerr<<"Paring index file failed!!"<<vcl_endl;
        return false;
    }
    vcl_cout<<vcl_endl;

    // Grab the model shock
    vcl_string model_gt_file = params_->model_object_dir_() + "/" +
        params_->model_object_name_()+".xml";

    // Read in model gt file
    if ( !x_read(model_gt_file,model_id_) )
    {
        vcl_cerr<<"Error parsing model xml groundtruth file"<<vcl_endl;
        return false;
    }

    // Determine where to write eval results to
    // either output folder or input object dir
    if (params_->save_to_object_folder_())
    { 
        output_file_ = params_->output_shock_patch_eval_folder_() + "/";
    }
    else 
    {
        output_file_ = params_->query_object_dir_() + "/";
    }
    
    if (!vul_file::exists(output_file_)) 
    {
        vul_file::make_directory(output_file_);
        
    }
  
    output_file_ += params_->query_object_name_()+
        params_->output_shock_patch_eval_extension_();
    
    // --------------- Read in a Detection Result ----------------------------
    // To determine number of thresholds lets look at one of the detect_results
    // file
    dborl_index_node_sptr root    = ind_->root_->cast_to_index_node();
    vcl_string query_bbox_detect_file = root->paths()[0] + "/" 
        + root->names()[0] + 
        params_->input_bbox_detect_extension_();

    if ( !vul_file::exists(query_bbox_detect_file))
    {
        vcl_cerr<<"Cannot find xml detect results, "<<
            query_bbox_detect_file<<vcl_endl;
        return false;

    }
    bxml_document doc_in = bxml_read(query_bbox_detect_file);
    bxml_data_sptr root_xml = doc_in.root_element();

    // Before we call this function move to immediate parent of node
    // we are interested in
    bxml_element* head=dbxml_algos::find_by_name(root_xml,"query");
    dbxml_algos::find_all_elems_by_name(head,"threshold",thresholds_);

    // --------------- Read in all Groundtruth files ---------------------------
    // Now lets loop over all query images and count positive and negative
    // images

    // Declare some variables before we start
    unsigned int number_of_queries = root->paths().size();
    unsigned int i(0);
    vcl_string query_gt_file;
    dborl_image_description_sptr query_id;

    // Find number of negative instances and postive instances
    unsigned int positive_cnt(0);
    unsigned int negative_cnt(0);
    for ( ; i < number_of_queries ; ++i)
    {
        
        // load the model shock
        query_gt_file = root->paths()[i] + "/" + root->names()[i] + ".xml";
        
        if ( !x_read(query_gt_file,query_id) )
        {
            vcl_cerr<<"Error parsing query xml groundtruth file"<<vcl_endl;
            return false;
        }

        // Push back query_id
        query_ids_.push_back(query_id);

        // FIX this later to handle multiple classes in ground truth
        if ( query_ids_[i]->category_exists(model_id_->get_first_category()))
        {
            positive_cnt++;

        }
        else
        {

            negative_cnt++;
        }
        
    }

    // Finally fill in each dborl_eval stats with the positive and negative cnt
    unsigned int t(0);
    unsigned int num_thresholds = thresholds_.size();
    for ( ; t < num_thresholds; ++t)
    {

        stats_at_threshold_.push_back(dborl_exp_stat());
        stats_at_threshold_[t].positive_cnt_ = positive_cnt;
        stats_at_threshold_[t].negative_cnt_ = negative_cnt;
    }

    return true;
}

//: This function is performs the actual evaluation of
// the bbox results 
bool vox_eval_shock_patches::perform_eval()
{
    // Loop over index file and compare query shock against all 
    // shapes within index file
    dborl_index_node_sptr root    = ind_->root_->cast_to_index_node();
    unsigned int number_of_queries = root->paths().size();

    // Loop over all thresholds and then loop over all detection results
    unsigned int thres(0);
    unsigned int num_thres(thresholds_.size());

    for( ; thres < num_thres ; ++thres)
    {
        bxml_element* data_elm = static_cast<bxml_element*>
            ((thresholds_[thres]).ptr());

        // Threshold hold to find in each detect_results file
        vcl_string att_threshold = data_elm->attribute("value");

        vcl_cout<<"Processing Point "<< thres <<" at threshold of "
                <<att_threshold<<vcl_endl;

        // Holds the detection resutls for query
        vcl_string query_bbox_detect_file;
  
        // Loop over children of index file
        unsigned int i(0);
        for ( ; i < number_of_queries ; ++i)
        {

            // Now read in the detection results 
            query_bbox_detect_file = root->paths()[i] + "/" 
                + root->names()[i] + 
                params_->input_bbox_detect_extension_();

            if ( !vul_file::exists(query_bbox_detect_file ))
            {
                vcl_cerr<<"Cannot file xml detect results, "<<
                    query_bbox_detect_file<<vcl_endl;
                return false;

            }

            // Create a query element to find the bounding box at that
            // threshold
            bxml_document  doc_in   = bxml_read(query_bbox_detect_file);
            bxml_data_sptr root_xml = doc_in.root_element();
            bxml_element   query_elm("threshold");
            query_elm.set_attribute("value",att_threshold);
            bxml_data_sptr threshold_data = bxml_find(root_xml,query_elm);
            
            // Now that we have found out the right threshold grab the bbox
            bxml_element*  threshold_elm = dbxml_algos::
                cast_to_element(threshold_data,"threshold");
            vsol_box_2d_sptr bbox=new vsol_box_2d();
            bounding_box(threshold_elm,bbox);

            int area = static_cast<int>(bbox->area());
            
            if ( area == 0 )
            {
                // Delete the memory by unref
                bbox->unref();

                //Set bbox to null pointer
                bbox=0;

            }

            dborl_exp_stat instance_stat;
            vsol_box_2d_sptr gt_box = dborl_evaluation_evaluate_detection
                (instance_stat, 
                 model_id_->get_first_category(), 
                 query_ids_[i], 
                 bbox, 
                 params_->bbox_overlap_ratio_());
            
            // Now update vector element with instance_stat
            stats_at_threshold_[thres].increment_TP_by(instance_stat.TP_);
            stats_at_threshold_[thres].increment_FP_by(instance_stat.FP_);
            stats_at_threshold_[thres].increment_TN_by(instance_stat.TN_);
            stats_at_threshold_[thres].increment_FN_by(instance_stat.FN_);
            
        }
        
        stats_at_threshold_[thres].print_only_stats();
        vcl_cout<<vcl_endl;
    }
    return true;
}

//: This method writes out the msghm files
bool vox_eval_shock_patches::write_out()
{
    //******************** Write Evaluation Results ****************************
    vcl_cout<<"************  Write Evaluation Results  *************"<<vcl_endl;
   
    // Xml Tree: eval_results->dataset->stats

    // Create root element
    bxml_document  doc;
    bxml_data_sptr root     = new bxml_element("eval_results");
    bxml_element*  root_elm = dbxml_algos::cast_to_element(root,"eval_results");
    doc.set_root_element(root); 
    root_elm->append_text("\n   ");

    // Create query element
    bxml_data_sptr dataset     = new bxml_element("dataset");
    bxml_element*  dataset_elm = 
        dbxml_algos::cast_to_element(dataset,"dataset");
    root_elm->append_data(dataset);
    root_elm->append_text("\n");
    dataset_elm->set_attribute("name",params_->query_object_name_());
    dataset_elm->set_attribute("num_positive",
                               stats_at_threshold_[0].positive_cnt_);
    dataset_elm->set_attribute("num_negative",
                               stats_at_threshold_[0].negative_cnt_);
    dataset_elm->append_text("\n      ");
    
    // Loop over vector of bounding boxes
    unsigned int i(0);
    for ( ; i < stats_at_threshold_.size() ; ++i)
    {

        bxml_data_sptr stats     = new bxml_element("stats");
        bxml_element*  stats_elm = dbxml_algos::
            cast_to_element(stats,"stats");

        if ( i > 0 )
        {
            dataset_elm->append_text("   ");
        }
        dataset_elm->append_data(stats);
        dataset_elm->append_text("\n   ");
        
        // Append coordinates
        bxml_data_sptr tp = new bxml_element("TP");
        bxml_data_sptr fp = new bxml_element("FP");
        bxml_data_sptr tn = new bxml_element("TN");
        bxml_data_sptr fn = new bxml_element("FN");
        
        bxml_element* tp_elm=dbxml_algos::cast_to_element(tp,"TP");
        bxml_element* fp_elm=dbxml_algos::cast_to_element(fp,"FP");
        bxml_element* tn_elm=dbxml_algos::cast_to_element(tn,"TN");
        bxml_element* fn_elm=dbxml_algos::cast_to_element(fn,"FN");
       
        vcl_stringstream tpvalue,fpvalue,tnvalue,fnvalue;
        
        tpvalue<<stats_at_threshold_[i].TP_;
        fpvalue<<stats_at_threshold_[i].FP_;
        tnvalue<<stats_at_threshold_[i].TN_;
        fnvalue<<stats_at_threshold_[i].FN_;

        stats_elm->append_text("\n         ");
        stats_elm->append_data(tp);
        tp_elm->append_text(tpvalue.str());
 
        stats_elm->append_text("\n         ");
        stats_elm->append_data(fp);
        fp_elm->append_text(fpvalue.str());
 
        stats_elm->append_text("\n         ");
        stats_elm->append_data(tn);
        tn_elm->append_text(tnvalue.str());

        stats_elm->append_text("\n         ");
        stats_elm->append_data(fn);
        fn_elm->append_text(fnvalue.str());

        stats_elm->append_text("\n      ");

    }

    bxml_write(output_file_,doc);

    return true;
}

void vox_eval_shock_patches::bounding_box(bxml_element* threshold,
                                          vsol_box_2d_sptr box)
{

    bxml_element *xmin_elm = dbxml_algos::find_by_name(threshold,"xmin");
    bxml_element *ymin_elm = dbxml_algos::find_by_name(threshold,"ymin");
    bxml_element *xmax_elm = dbxml_algos::find_by_name(threshold,"xmax");
    bxml_element *ymax_elm = dbxml_algos::find_by_name(threshold,"ymax");

    double xmin,xmax,ymin,ymax;
    xml_parse(xmin_elm,xmin);
    xml_parse(ymin_elm,ymin);
    xml_parse(xmax_elm,xmax);
    xml_parse(ymax_elm,ymax);

    // Grow bounding box
    box->add_point(xmin,ymin);
    box->add_point(xmax,ymax);

}
