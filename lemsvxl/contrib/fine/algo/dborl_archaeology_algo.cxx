//:
// \file
// \brief 
// \author Eli Fine (eli_fine@brown.edu)
// \date 09/26/08
//
//

#include "dborl_archaeology_algo.h"
#include "dborl_archaeology_algo_params.h"
#include <dborl/algo/dborl_utilities.h>
#include <dborl/dborl_evaluation.h>
#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <dbsol/dbsol_file_io.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>

#include <dbsol/algo/dbsol_curve_algs.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dborl/algo/dborl_index_parser.h>

//: this method is run on each processor
bool dborl_archaeology_algo::parse_index(vcl_string index_file)
{
  dborl_index_parser parser;
  parser.clear();

  vcl_FILE *xmlFile = vcl_fopen(index_file.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << "dborl_shock_retrieval::parse_index() -- " << index_file << "-- error on opening" << vcl_endl;
    return false;
  }

  if (!parser.parseFile(xmlFile)) {
     vcl_cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << vcl_endl;
     return 0;
   }

  fclose(xmlFile);
  ind_ = parser.get_index();
  
  if (!ind_)
    return false;

  vcl_cout << "parsed the index file with name: " << ind_->name_ << vcl_endl;

  root_ = ind_->root_->cast_to_index_node();
  if (root_->names().size() != root_->paths().size()) {
    vcl_cout << "dborl_archaeology::parse_index() -- " << index_file << "-- number of names not equal number of paths!!" << vcl_endl;
    return false;
  }

  return true;
}

//: read the files etc
bool dborl_archaeology_algo::initialize()
{
  //: parse the index file 
  if (!parse_index(params_->db_index_()))
    return false;

  unsigned D = root_->names().size();
  vcl_cout << "db size: " << D << vcl_endl;

  if (root_->names().size() != root_->paths().size()) {
    vcl_cout << "dborl_archaeology_algo::initialize() -- names and paths size not equal in index: " << params_->db_index_() << vcl_endl;
    return false;
  }

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dborl_archaeology_algo::process()
{

    vcl_string destination;
    
    bpro1_filepath file;

    //bool altFill;
    float TQUAN;
    int scale;
    float mthresh;

    //load the image
    vil_image_view_base_sptr original_image = new vil_image_view_base(vil_load(file));

    //segment the image
    vil_image_view_base_sptr segmented_image = new vil_image_view_base(dbseg_jseg_process::static_execute(original_image, TQUAN, mthresh, scale));

    //create the segmentation structure
    dbseg_seg_object_base* segmentation_structure = new dbseg_seg_object_base(dbseg_seg_process::static_execute(segmented_image, original_image, true));

    //save the structure
    vul_file::make_directory_path(destination + "\\segmentation_structures");
    bpro1_filepath struct_destination = bpro1_filepath(destination + "\\segmentation_structures\\" + vul_file::strip_extension(vul_file::strip_directory(file)) + ".seg");
    dbseg_seg_save_process::static_execute(segmentation_struture, struct_destination);

    //extract the contour
    vul_file::make_directory_path(destination + "\\contours");
    bpro1_filepath contour_destination = bpro1_filepath(destination + "\\contours\\" + vul_file::strip_extension(vul_file::strip_directory(file)) + ".con");
    dbseg_seg_save_contour_process::static_execute(segmentation_structure, contour_destination);




  //: load each con file 
  unsigned D = root_->names().size();
  for (unsigned i = 0; i < D; i++) {
    vcl_string con_file =  root_->paths()[i] + "/" + root_->names()[i] + params_->input_extension_();
    if (!vul_file::exists(con_file)) {
      vcl_cout << "dborl_archaeology_algo::process() -- cannot find: " << con_file << vcl_endl;
      return false;
    }

     vcl_string output_filename = params_->cem_folder_to_create_() + "/" + root_->names()[i] + "/";
    //: write the output as a .cem fil
    if (!vul_file::is_directory(output_filename))
      vul_file::make_directory(output_filename);

    output_filename = output_filename + root_->names()[i] + ".cem";
    if (vul_file::exists(output_filename)) {
      vcl_cout << "\tfile: " << output_filename << " exists\n\n";
      continue;
    }
    vcl_cout << "extracting: " << root_->names()[i] << vcl_endl;

    vcl_vector<vsol_point_2d_sptr> points;
    bool is_closed;
    if (!dbsol_load_con_file(con_file.c_str(), points, is_closed)) { 
      vcl_cout << "dborl_archaeology_algo::process() -- cannot load: " << con_file << vcl_endl;
      return false;
    }

    vcl_vector<vsol_spatial_object_2d_sptr> vsol_list;
    vsol_polygon_2d_sptr poly = new vsol_polygon_2d(points);

    if (params_->delete_segments_()) {

      if (params_->add_segments_()) { // both delete and add
        
        vsol_list.clear();
        vcl_vector<vsol_polyline_2d_sptr> del_pieces;

        int counter = 0;
        while (!dbsol_curve_algs::segment_addition_and_deletion(poly, vsol_list, del_pieces, 
          params_->perc_add_(), params_->sigma_add_(), params_->divident_add_(), 
          params_->perc_del_(), params_->sigma_del_(), params_->divident_del_()) ||
          !vsol_list.size() || !del_pieces.size()) {
          vsol_list.clear();
          del_pieces.clear();
          counter++;
          if (counter > 100) {
            vcl_cout << "dborl_archaeology_algo::process() -- cannot randomize segment deletion! in: " << con_file << vcl_endl;
            return false;
          }
        }

      } else {  // just delete

        vcl_vector<vsol_polyline_2d_sptr> pieces;
        vcl_vector<vsol_polyline_2d_sptr> del_pieces;
        int counter = 0;
        while (!dbsol_curve_algs::segment_wise_deletion(poly, pieces, del_pieces, params_->perc_del_(), params_->sigma_del_(), params_->divident_del_()) ||
          !pieces.size() || !del_pieces.size()) {
          pieces.clear();
          del_pieces.clear();
          counter++;
          if (counter > 100) {
            vcl_cout << "dborl_archaeology_algo::process() -- cannot randomize segment deletion! in: " << con_file << vcl_endl;
            return false;
          }
        }
        
        vcl_cout << "there are " << pieces.size() << " remaining pieces and " << del_pieces.size() << " deleted pieces\n";

        
        for (unsigned ii = 0; ii < pieces.size(); ii++)
          vsol_list.push_back(pieces[ii]->cast_to_spatial_object());

      }
    } else {  
      if (params_->add_segments_()) {  // just add

        int counter = 0;
        while (!dbsol_curve_algs::segment_addition(poly, vsol_list, params_->perc_add_(), params_->sigma_add_(), params_->divident_add_()) ||
               !vsol_list.size()) 
        {
            vsol_list.clear();
            counter++;
            if (counter > 100) {
              vcl_cout << "dborl_archaeology_algo::process() -- cannot randomize segment addition! in: " << con_file << vcl_endl;
              return false;
            }
        }
        vsol_list.push_back(poly->cast_to_spatial_object());

      } else
        return true;
    }

   
    //output_filename = path + names_[i] + params_->output_file_postfix_ + ".cem";
    dbsol_save_cem(vsol_list, output_filename);
  }

  return true;
}

bool dborl_archaeology_algo::finalize()
{
  return true;
}









