//:
// \file
// \author Anil Usumezbas
// \date 05/04/2009
//
//        Algorithm to generate similarity matrix for shock matching experiments on VOX.
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
#include<time.h>


#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_threshold.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/dbsol_file_io.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>


#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/dbskr_tree_edit_coarse.h>
#include <dbskr/dbskr_tree_edit_combined.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_localize_match.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_generator.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vul/vul_file.h>

#include "dborl_create_shock_sim_params.h"
#include "dborl_create_shock_sim_params_sptr.h"

#include <dborl/dborl_index_sptr.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>

#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_image_description.h>

int main (int argc, char *argv[]) 
{
  // constructs with the default values
  dborl_create_shock_sim_params_sptr params = new dborl_create_shock_sim_params ("create_shock_sim");  
  if (!params->parse_command_line_args(argc, argv))
  {
    vcl_cout << "Cannot find the parameters file!" << vcl_endl;
    return 0;
  }

  // always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
    vcl_cout << "problems in writing params file to: " << params->print_params_file() << vcl_endl;

  if (params->exit_with_no_processing() || params->print_params_only())
  {
    vcl_cout << "Exit with no processing!" << vcl_endl;
    return 0;
  }

  // always call this method to actually parse the input parameter file whose name is extracted from the command line
  if (!params->parse_input_xml())
  {
    vcl_cout << "Failed to parse input xml file!" << vcl_endl;
    return 0;
  }

  //creating a dborl_index instance which will store the information coming from the index file
  dborl_index_sptr ind;

  vcl_string index_filename = params->index_filename_();

  //creating a parser for the index file
  dborl_index_parser parser;
  parser.clear();

  vcl_FILE *xmlFile = vcl_fopen(index_filename.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << "dborl_extract_contour::parse_index() -- " << index_filename << "-- error on opening" << vcl_endl;
    return 0;
  }

  if (!parser.parseFile(xmlFile)) {
     vcl_cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << vcl_endl;
     return 0;
   }

  fclose(xmlFile);
  ind = parser.get_index();
  
  if (!ind)
  {
    vcl_cout << "Index could not be parsed" << vcl_endl;
    return 0;
  }

  if (!(ind->get_type() == dborl_index_type::flat_image)) {
    vcl_cout << "Index is not of flat_image type! Exiting!\n";
    return 0;
  }

  vcl_string input_result;

  dborl_index_node_sptr root = ind->root_->cast_to_index_node();

  // create the output file (.xml)
  vcl_string output_file=params->input_dir_()+"/";

  if (!vul_file::exists(output_file)) 
    vul_file::make_directory(output_file);

  vcl_string nm_filename = output_file+"similarity_matrix.txt";
  vcl_string cm_filename = output_file+"cost_matrix.txt";
  vcl_string uo_filename = output_file+"unordered.txt";

  vcl_ofstream nameMatrix(nm_filename.c_str());

  if (!vul_file::exists(nm_filename))
  {
    vcl_cout << "Cannot create name matrix file" << "\n";
    return 0;
  }

  vcl_ofstream costMatrix(cm_filename.c_str());

  if (!vul_file::exists(cm_filename)) 
  {
    vcl_cout << "Cannot create cost matrix file" << "\n";
    return 0;
  }

  vcl_ofstream unOrdered(uo_filename.c_str());

  if (!vul_file::exists(uo_filename))
  {
    vcl_cout << "Cannot create unordered similarity matrix file" << "\n";
    return 0;
  }

  int number_of_shapes = root->paths().size();

//  unOrdered << "* ";

//  for(int i=0; i<number_of_shapes; i++)
//      unOrdered << root->names()[i] << " ";

//  unOrdered << vcl_endl;

  vcl_vector<vcl_vector<vcl_string> > name_array(number_of_shapes,vcl_vector<vcl_string>(number_of_shapes));
  vcl_vector<vcl_vector<double> > cost_array(number_of_shapes,vcl_vector<double>(number_of_shapes));


  for(int i=0; i<number_of_shapes; i++)
  {

    // load the input result file
    input_result = root->paths()[i] + "/" + root->names()[i] + ".mshgm";

    if (!vul_file::exists(input_result)) 
    {
      vcl_cout << "Cannot find shock match result (.mshgm) files: " << input_result << "\n";
      return 0;
    }

    vcl_ifstream mshgm;
    mshgm.open(input_result.c_str());

    vcl_string current_name;
    vcl_string current_cost_string;
    double current_cost;

    for(int j=0; j<number_of_shapes; j++)
    {
      getline(mshgm,current_name);
      getline(mshgm,current_cost_string);

      vcl_stringstream temp(current_cost_string);
      
      temp >> current_cost;

      name_array[i][j] = current_name;
      cost_array[i][j] = current_cost;
    }

//    unOrdered << root->names()[i] << " ";
  }

  int counter = 0;

  //Making the cost matrix symmetric
  for(int i=0; i<number_of_shapes; i++)
  {
      for(int j=i+1; j<number_of_shapes; j++)
      {
          if(cost_array[i][j]<cost_array[j][i])
          {
              cost_array[j][i]=cost_array[i][j];
              counter++;
          }
          else if(cost_array[i][j]>cost_array[j][i])
          {
              cost_array[i][j]=cost_array[j][i];
              counter++;
          }
      }
  }

  for(int i=0; i<number_of_shapes; i++)
  {   
      for(int j=0; j<number_of_shapes; j++)
        unOrdered << cost_array[i][j] << " ";

    unOrdered << vcl_endl;

  }

  for(int i=0; i<number_of_shapes; i++)
  {    

    vcl_map<double, vcl_string> similarity_row;

    for(int j=0; j<number_of_shapes; j++)
    {
        vcl_map<double, vcl_string>::iterator srit = similarity_row.find(cost_array[i][j]);
      while(srit!=similarity_row.end())
      {
          cost_array[i][j] = cost_array[i][j] - 0.000001;  //Anil:This is dangerous, far large datasets, this arbitrary number may
          srit = similarity_row.find(cost_array[i][j]);   //change the ordering. Think of a different solution for this
      }
      vcl_pair<double, vcl_string> current_pair(cost_array[i][j], name_array[i][j]);
      similarity_row.insert(current_pair);
    }

    nameMatrix << root->names()[i] << " ";

    for(vcl_map<double, vcl_string>::iterator mit = similarity_row.begin(); mit!=similarity_row.end(); ++mit)
    {
      nameMatrix << mit->second <<" ";
      costMatrix << mit->first  <<" ";
    }

    nameMatrix << vcl_endl;
    costMatrix << vcl_endl;

  }

  nameMatrix.close();
  costMatrix.close();
  unOrdered.close();

  vcl_cout << counter << vcl_endl;

  return 0;
}
