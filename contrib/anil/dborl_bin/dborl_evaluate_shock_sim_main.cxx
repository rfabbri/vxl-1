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

#include "dborl_evaluate_shock_sim_params.h"
#include "dborl_evaluate_shock_sim_params_sptr.h"

#include <dborl/dborl_index_sptr.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>

#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_image_description.h>

void write_header(vcl_ofstream outfile)
{
  outfile << "<type name=\"evaluation_result\">";
  outfile << vcl_endl;
  outfile << "<algorithm name=\"dborl_shock_matcher\" />";
  outfile << vcl_endl;
  outfile << "<description />";
  outfile << vcl_endl;
  outfile << "<job id='000'>";
  outfile << vcl_endl;
}

void write_point(vcl_ofstream outfile, const int &TP, const int &TN, const int &FP, const int &FN, const int &pointNum, const bool &isFinal)
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

  outfile << vcl_endl;
  outfile << "</job>";
  outfile << vcl_endl;

  if(isFinal)
    outfile << "</type>" << vcl_endl;
  else
    outfile << "<job id='000'>" << vcl_endl;
}

vcl_string find_category(vcl_string obj_groundtruth)
{
  vcl_string category;

  //: find the current categorization, i.e. category of the nearest neighbor in this node of the index
  if (!vul_file::exists(obj_groundtruth)) {
    vcl_cout << "dborl_categorization_simple::finalize() -- cannot find ground truth description: " << obj_groundtruth << vcl_endl;
    return 0;
  }

  // assuming its an image ground truth description:
  dborl_image_desc_parser parser;
  dborl_image_description_sptr object_desc = dborl_image_description_parse(obj_groundtruth, parser);
  if (!object_desc->has_single_category()) {
    vcl_cout << "dborl_categorization_simple::finalize() - WARNING: image description: " << obj_groundtruth << " has more than one category! not a good model! using the first category\n";
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

    dborl_index_node_sptr root = ind->root_->cast_to_index_node();

    // create the output file (.xml)
    vcl_string output_file=params->output_dir_()+"/";

    if (!vul_file::exists(output_file)) 
        vul_file::make_directory(output_file);

    vcl_string eval_filename = output_file+"evaluation.xml";
    vcl_string bin_filename = output_file+"binary_matrix.txt";

    vcl_ofstream eval(eval_filename.c_str());
    vcl_ofstream bin(bin_filename.c_str());

    if (!vul_file::exists(eval_filename))
    {
        vcl_cout << "Cannot create evaluation.xml" << "\n";
        return 0;
    }

    int number_of_shapes = root->paths().size();

    vcl_map<vcl_string, int> category_map;
//   int max_value = 1;

    vcl_map<vcl_string, vcl_string> directory_map;

    for(int i=0; i<number_of_shapes; i++)
    {

        vcl_string category;

        //: find the current categorization, i.e. category of the nearest neighbor in this node of the index
        vcl_string groundtruth = root->paths()[i] + "/" + root->names()[i] + ".xml";

        //vcl_cout << i << ": " << groundtruth << vcl_endl;

        vcl_pair<vcl_string, vcl_string> current_pair(root->names()[i],root->paths()[i]);
        directory_map.insert(current_pair);

        category = find_category(groundtruth);
        vcl_map<vcl_string, int>::iterator mit = category_map.find(category);

        if(mit==category_map.end())
        {
            vcl_pair<vcl_string, int> assoc(category,1);
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

    vcl_string sm_filename = params->input_dir_()+"/similarity_matrix.txt";

    vcl_ifstream sm;
    sm.open(sm_filename.c_str());

    vcl_vector<vcl_vector<vcl_string> > similarity_matrix(number_of_shapes,vcl_vector<vcl_string>(number_of_shapes+1));
    vcl_vector<vcl_string> dummy_vector(number_of_shapes);

    for(int i=0; i<number_of_shapes; i++)
        for(int j=0; j<number_of_shapes+1; j++)
            sm >> similarity_matrix[i][j];


    vcl_vector<int> TP(number_of_shapes,0);
    vcl_vector<int> TN(number_of_shapes,0);
    vcl_vector<int> FP(number_of_shapes,0);
    vcl_vector<int> FN(number_of_shapes,0);

    eval << "Sequence: TP, FP, TN, FN" << vcl_endl;

    vcl_vector<vcl_vector<bool> > binary_matrix(number_of_shapes,vcl_vector<bool>(number_of_shapes+1,false));   

    for(int i=0; i<number_of_shapes; i++)
    {
        vcl_string current_shape = similarity_matrix[i][0];
        vcl_string current_category;

        vcl_string obj_groundtruth = directory_map[current_shape] + "/" + current_shape + ".xml";

        current_category = find_category(obj_groundtruth);

        for(int j=2; j<number_of_shapes+1; j++)
        {
            vcl_string query_shape = similarity_matrix[i][j];
            vcl_string query_category;

            vcl_string query_groundtruth = directory_map[query_shape] + "/" + query_shape + ".xml";

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

       
        eval << "Point" << k << ":" << vcl_endl;
        eval << TP[k] << " " << FP[k] << " " << TN[k] << " " << FN[k] << vcl_endl;

        k++;

    }

    eval.close();

    for(int i=0; i<number_of_shapes; i++)
    {
        for(int j=1; j<number_of_shapes+1; j++)
            bin << binary_matrix[i][j] << " ";

        bin << vcl_endl;
    }

    bin.close();



    return 0;
}
