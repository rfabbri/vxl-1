//:
// \file
// \author Anil Usumezbas
// \date 26/04/2009
//
//        Algorithm to process the similarity matrix for shock matching experiments and 
//        generate the ROC/PR plot information on VOX.
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
#include <time.h>


#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vector>
#include <string>
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

#include <iostream>
#include <fstream>
#include <cstdio>
#include <vul/vul_file.h>

#include "dborl_evaluate_shock_sim_params.h"
#include "dborl_evaluate_shock_sim_params_sptr.h"

#include <dborl/dborl_index_sptr.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>

#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_image_description.h>

void write_header(std::ofstream outfile)
{
  outfile << "<type name=\"evaluation_result\">";
  outfile << std::endl;
  outfile << "<algorithm name=\"dborl_shock_matcher\" />";
  outfile << std::endl;
  outfile << "<description />";
  outfile << std::endl;
  outfile << "<job id='000'>";
  outfile << std::endl;
}

void write_point(std::ofstream outfile, const int &TP, const int &TN, const int &FP, const int &FN, const int &pointNum, const bool &isFinal)
{
  outfile << "<statistics name=";
  outfile << pointNum;
  outfile << "\" TP=";
  outfile << TP;
  outfile << "\" FP=";
  outfile << FP;
  outfile << "\" TN=";
  outfile << TN;
  outfile << "\" FN=";
  outfile << FN;
  outfile << "\" />";

  outfile << std::endl;
  outfile << "</job>";
  outfile << std::endl;

  if(isFinal)
    outfile << "</type>" << std::endl;
  else
    outfile << "<job id='000'>" << std::endl;
}

std::string find_category(std::string obj_groundtruth)
{
  std::string category;

  //: find the current categorization, i.e. category of the nearest neighbor in this node of the index
  if (!vul_file::exists(obj_groundtruth)) {
    std::cout << "dborl_categorization_simple::finalize() -- cannot find ground truth description: " << obj_groundtruth << std::endl;
    return 0;
  }

  // assuming its an image ground truth description:
  dborl_image_desc_parser parser;
  dborl_image_description_sptr object_desc = dborl_image_description_parse(obj_groundtruth, parser);
  if (!object_desc->has_single_category()) {
    std::cout << "dborl_categorization_simple::finalize() - WARNING: image description: " << obj_groundtruth << " has more than one category! not a good model! using the first category\n";
  }
  category = object_desc->get_first_category();
  return category; 
}

int main (int argc, char *argv[]) 
{
    // constructs with the default values
    dborl_evaluate_shock_sim_params_sptr params = new dborl_evaluate_shock_sim_params ("evaluate_shock_sim");  
    if (!params->parse_command_line_args(argc, argv))
    {
        std::cout << "Cannot find the parameters file!" << std::endl;
        return 0;
    }

    // always print the params file if an executable to work with ORL web interface
    if (!params->print_params_xml(params->print_params_file()))
        std::cout << "problems in writing params file to: " << params->print_params_file() << std::endl;

    if (params->exit_with_no_processing() || params->print_params_only())
    {
        std::cout << "Exit with no processing!" << std::endl;
        return 0;
    }

    // always call this method to actually parse the input parameter file whose name is extracted from the command line
    if (!params->parse_input_xml())
    {
        std::cout << "Failed to parse input xml file!" << std::endl;
        return 0;
    }

    //creating a dborl_index instance which will store the information coming from the index file
    dborl_index_sptr ind;

    std::string index_filename = params->index_filename_();

    //creating a parser for the index file
    dborl_index_parser parser;
    parser.clear();

    std::FILE *xmlFile = std::fopen(index_filename.c_str(), "r");
    if (xmlFile == NULL){
        std::cout << "dborl_extract_contour::parse_index() -- " << index_filename << "-- error on opening" << std::endl;
        return 0;
    }

    if (!parser.parseFile(xmlFile)) {
        std::cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
            parser.XML_GetCurrentLineNumber() << std::endl;
        return 0;
    }

    fclose(xmlFile);
    ind = parser.get_index();
  
    if (!ind)
    {
        std::cout << "Index could not be parsed" << std::endl;
        return 0;
    }

    if (!(ind->get_type() == dborl_index_type::flat_image)) {
        std::cout << "Index is not of flat_image type! Exiting!\n";
        return 0;
    }

    dborl_index_node_sptr root = ind->root_->cast_to_index_node();

    // create the output file (.xml)
    std::string output_file=params->output_dir_()+"/";

    if (!vul_file::exists(output_file)) 
        vul_file::make_directory(output_file);

    std::string eval_filename = output_file+"evaluation.xml";
    std::string bin_filename = output_file+"binary_matrix.txt";

    std::ofstream eval(eval_filename.c_str());
    std::ofstream bin(bin_filename.c_str());

    if (!vul_file::exists(eval_filename))
    {
        std::cout << "Cannot create evaluation.xml" << "\n";
        return 0;
    }

    int number_of_shapes = root->paths().size();

    std::map<std::string, int> category_map;
//   int max_value = 1;

    std::map<std::string, std::string> directory_map;

    for(int i=0; i<number_of_shapes; i++)
    {

        std::string category;

        //: find the current categorization, i.e. category of the nearest neighbor in this node of the index
        std::string groundtruth = root->paths()[i] + "/" + root->names()[i] + ".xml";

        //std::cout << i << ": " << groundtruth << std::endl;

        std::pair<std::string, std::string> current_pair(root->names()[i],root->paths()[i]);
        directory_map.insert(current_pair);

        category = find_category(groundtruth);
        std::map<std::string, int>::iterator mit = category_map.find(category);

        if(mit==category_map.end())
        {
            std::pair<std::string, int> assoc(category,1);
            category_map.insert(assoc);
        }
        else
        {
            int prev_value = category_map[category];
            category_map[category] = prev_value + 1;
//            if(category_map[category]>max_value)
//                max_value = category_map[category];
        }

    }

    std::string sm_filename = params->input_dir_()+"/similarity_matrix.txt";

    std::ifstream sm;
    sm.open(sm_filename.c_str());

    std::vector<std::vector<std::string> > similarity_matrix(number_of_shapes,std::vector<std::string>(number_of_shapes+1));
    std::vector<std::string> dummy_vector(number_of_shapes);

    for(int i=0; i<number_of_shapes; i++)
        for(int j=0; j<number_of_shapes+1; j++)
            sm >> similarity_matrix[i][j];


    std::vector<int> TP(number_of_shapes,0);
    std::vector<int> TN(number_of_shapes,0);
    std::vector<int> FP(number_of_shapes,0);
    std::vector<int> FN(number_of_shapes,0);

    eval << "Sequence: TP, FP, TN, FN" << std::endl;

    std::vector<std::vector<bool> > binary_matrix(number_of_shapes,std::vector<bool>(number_of_shapes+1,false));   

    for(int i=0; i<number_of_shapes; i++)
    {
        std::string current_shape = similarity_matrix[i][0];
        std::string current_category;

        std::string obj_groundtruth = directory_map[current_shape] + "/" + current_shape + ".xml";

        current_category = find_category(obj_groundtruth);

        for(int j=2; j<number_of_shapes+1; j++)
        {
            std::string query_shape = similarity_matrix[i][j];
            std::string query_category;

            std::string query_groundtruth = directory_map[query_shape] + "/" + query_shape + ".xml";

            query_category = find_category(query_groundtruth);

            if(query_category==current_category)
                binary_matrix[i][j] = true;

        }
    }

            

    int k=1;

    //Initialization of totalFN to something nonzero
    int totalFN=1;

    while(totalFN != 0)
    {
        totalFN = 0;

        for(int i=0; i<number_of_shapes; i++)
        {
            for(int j=2; j<number_of_shapes+1; j++)
            {
                if(binary_matrix[i][j]==true)
                {
                    if(j<=k+1)
                        TP[k] = TP[k]+1;
                    else
                    {
                        FN[k] = FN[k]+1;
                        totalFN++;
                    }
                }
                else
                    if(j<=k+1)
                        FP[k] = FP[k]+1;
                    else
                        TN[k] = TN[k]+1;
            }
 
        }

       
        eval << "Point" << k << ":" << std::endl;
        eval << TP[k] << " " << FP[k] << " " << TN[k] << " " << FN[k] << std::endl;

        k++;

    }

    eval.close();

    for(int i=0; i<number_of_shapes; i++)
    {
        for(int j=1; j<number_of_shapes+1; j++)
            bin << binary_matrix[i][j] << " ";

        bin << std::endl;
    }

    bin.close();



    return 0;
}
