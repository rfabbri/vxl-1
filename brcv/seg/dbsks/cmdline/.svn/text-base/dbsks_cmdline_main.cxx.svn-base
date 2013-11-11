// This is dbsks/cmdline/dbsks_cmdline_main.cxx

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 10, 2008


#include "dbsks_cmdline_utils.h"
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vcl_cstdlib.h>

//: Command line programs for the sks class
int main(int argc, char *argv[]) 
{
  // Arguments
  vul_arg_info_list arg_list;
  
  // ps-to-jpeg
  vul_arg<bool > ps_to_jpeg(arg_list,"-ps-to-jpeg", "Convert PS to PNG for a folder", false);
  
  // create-shapematch-summary-xml
  vul_arg<bool > create_shapematch_summary_xml(arg_list, "-create-shapematch-summary-xml", 
    "Create an .XML file summarizing the shapematch results in a folder", false);
  
  // extract geometry a fragment from a list of shock graphs
  vul_arg<bool > extract_xfrag_geom(arg_list, "-extract-xfrag-geom",
    "Extract geometric information of a fragment from a list extrinsic shock graph", false);
  
  // extract postive histogram of gradient (HOG) of a fragment (identified by a shock graph and an id)
  vul_arg<bool > extract_positive_xfrag_bhog(arg_list, "-extract-positive-xfrag-bhog",
    "Extract positive examples of boundary histogram of gradient (BHOG) of a fragment in an extrinsic shock graph", false);

  // extract negative examples of the HOG of a fragment
  vul_arg<bool > extract_negative_xfrag_bhog(arg_list, "-extract-negative-xfrag-bhog",
    "Extract negative examples of boundary histogram of gradient (BHOG) of a fragment in an extrinsic shock graph", false);

  // Use a shape fragment model and apply it to an image to detect the positive fragment
  vul_arg<bool > detect_xfrag_using_bhog_model(arg_list, "-detect-xfrag-using-bhog-model",
    "Detect positve xfrags in an image using the fragment's BHOG model", false);

  // Extract CCM cost from image pyramid----------------------------------------
  vul_arg<bool > collect_xgraph_ccm_using_edgemap_pyramid(arg_list, "-collect-xgraph-ccm-using-edgemap-pyramid",
    "Compute training data for ccm cost of xgraph using edgemap pyramid", false);
 
  
  // Use a shape fragment model and apply it to an image to detect the positive fragment
  vul_arg<bool > extend_giraffe_legs(arg_list, "-extend-giraffe-legs",
    "Extend the front and rear legs of the giraffe models", false);

  // Re-compute bounding boxes of detections that were wrongly computed and saved
  vul_arg<bool > recompute_detection_bbox(arg_list, "-recompute-detection-bbox",
    "Re-compute bounding boxes of detections that were wrongly computed and saved", false);

  

  //============================================================================
  // Supporting arguments for the functions above
  vul_arg<vcl_string > shapematch_extension(arg_list, "-shapematch-extension", 
    "Extension of shapematch output file (to concatenate with image name)", "_shapematch_out.txt");
  vul_arg<vcl_string> input_folder(arg_list,"-input-folder","the input folder","");
  
  vul_arg<vcl_string> output_folder(arg_list,"-output-folder","the output folder","");
  vul_arg<vcl_string> output_file(arg_list,"-output-file","the output file","");

  vul_arg<vcl_string> xshock_folder(arg_list,"-xshock-folder","the folder containing extrinsic shock","");
  vul_arg<vcl_string > xshock_list_file(arg_list, "-xshock-list-file",
    "file containing list of extrinsic shock XML files", "");
  
  vul_arg<vcl_string> image_folder(arg_list,"-image-folder","the folder containing images","");
  vul_arg<vcl_string > image_list_file(arg_list, "-image-list-file", "file containing list of image file names", "");
  
  vul_arg<unsigned > edge_id(arg_list, "-edge-id", "ID of the edge of interest", 0);
  vul_arg<unsigned > root_vid(arg_list, "-root-vid", "ID of the root node", 0);


  vul_arg <vcl_string > libsvm_xfrag_bhog_model_file(arg_list, "-libsvm-xfrag-bhog-model-file",
    "Model for boundary histogram of orientation gradient (BHOG) of a xfragment in LIBSVM format", "");

  vul_arg <vcl_string > xfrag_geom_file(arg_list, "-xfrag-geom-file", 
    "A file containing the geometry information of training extrinsic fragments", "");

  vul_arg<vcl_string > xshock_extension(arg_list, "-xshock-extension", "extension of the xshock files", ".xml");
  vul_arg<vcl_string > image_extension(arg_list, "-image-extension", "extension of the image files", ".png");
  vul_arg<vcl_string > edgemap_folder(arg_list, "-edgemap-folder", "path to the edgemap folder", ""); 
  vul_arg<vcl_string > edgemap_extension(arg_list, "-edgemap-extension", "extension of the edgemap files", ".edges.tif");
  vul_arg<vcl_string > edgeorient_folder(arg_list, "-edgeorient-folder", "path to edge orientation folder", ""); 
  vul_arg<vcl_string > edgeorient_extension(arg_list, "-edgeorient-extension", "extension of the edgemap files", ".orient.txt");
  vul_arg<vcl_string > cemv_folder(arg_list, "-cemv-folder", "path to contour-edge-map folder (cem files)", ""); 
  vul_arg<vcl_string > cemv_extension(arg_list, "-cemv-extension", "extension of contour-edge-map files", ".cemv");


  // print help
  vul_arg<bool > print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool > print_help(arg_list,"-help", "print usage info and exit",false);

  // parse input arguments
  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  //============================================================================
  
  // convert PS (with bounding box) images to jpeg images
  if (ps_to_jpeg())
  {
    if (print_help())
    {
      vcl_cout << "Usage with -ps-to-jpeg option:\n"
      << input_folder.option() << " " << input_folder.help() << "\n"
      << output_folder.option() << " " << output_folder.help() << "\n";
      return EXIT_SUCCESS;
    }
    else
    {
      vcl_cout << "input folder = " << input_folder() << "\n"
        << "output_folder = " << output_folder() << "\n";
      dbsks_batch_ps_to_jpeg(input_folder(), output_folder());
      return EXIT_SUCCESS;
    }
  }

 
  //< create_shape_match_summary >------------------------------------------------
  if (create_shapematch_summary_xml())
  {
    if (print_help())
    {
      vcl_cout << "Usage with -create-shapematch-summary-xml option:\n"
        << input_folder.option() << " " << input_folder.help() << "\n"
        << image_list_file.option() << " " << image_list_file.help() << "\n"
        << shapematch_extension.option() << " " << shapematch_extension.help() << "\n"
        << output_file.option() << " " << output_file.help() << "\n";
      return EXIT_SUCCESS;
    }
    else
    {
      vcl_cout << "input folder = " << input_folder() << "\n"
        << "image list file = " << image_list_file() << "\n"
        << "shapematch_extension = " << shapematch_extension() << "\n"
        << "output file = " << output_file() << "\n";
      return dbsks_create_shapematch_summary_xml(output_file(), input_folder(),
        image_list_file(), shapematch_extension()) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
  }
  //</>-------------------------------------------------------------------------

  //< extract geometry of a fragment in a list of extrinsic shock graphs >-------
  if (extract_xfrag_geom())
  {
    if (print_help())
    {
      vcl_cout << "Usage with -extract-xfrag-geom option:\n"
        << xshock_folder.option() << " " << xshock_folder.help() << "\n"
        << xshock_list_file.option() << " " << xshock_list_file.help() << "\n"
        << edge_id.option() << " " << edge_id.help() << "\n"
        << output_file.option() << " " << output_file.help() << "\n";
      return EXIT_SUCCESS;
    }
    else
    {
      if ( xshock_folder() == "" || xshock_list_file() == "" || edge_id() == 0 || output_file() == "")
      {
        vcl_cout << "ERROR: invalid input arguments. Use -help-extract-xfrag-geom to see necessary arguments.\n";
        return EXIT_FAILURE;
      }

      //bool success = dbsks_extract_xfrag_properties(xshock_folder(), image_folder(), xshock_list_file(), edge_id(), output_file());
      bool success = dbsks_extract_xfrag_geom(xshock_folder(), xshock_list_file(), edge_id(), output_file());

      if (success)
      {
        vcl_cout << "Geometry of fragment extracted sucessfully.\n";
      }
    }
  }
  //</>-----------------------------------------------------------------------
  
  // extract positive xfrag BHOG from a list of shock graphs and images
  // example
  // argv[0] -extract-negative-xfrag-bhog -xshock-folder V:\projects\kimia\shockshape\symseg\results\ETHZ-database\xshock-graph\giraffes -xshock-list-file V:\projects\kimia\shockshape\symseg\results\ETHZ-database\xshock-graph\list_giraffes_xml.txt -edge-id 8 -image-folder V:\projects\kimia\shockshape\symseg\results\ETHZ-database\reorganized-data\all_originals -image-list-file V:\projects\kimia\shockshape\symseg\results\ETHZ-database\training\list_all_images_except_giraffes.txt -output-file V:\projects\kimia\shockshape\symseg\results\ETHZ-database\training\giraffes-eid_8-negative.txt
  if (extract_negative_xfrag_bhog())
  {
    if (print_help())
    {
      
      vcl_cout << "This option first extract a list of corresponding fragments from an edge-id and a list of shock graphs.\n"
        << "Then for each fragment, the program generates various rigidly-transformed version of the fragment and extract"
        << " the boundary histogram of gradient (BHOG) in all negative images (identified by an image folder and an image list" 
        << "Usage with -extract-negative-xfrag-bhog option:\n"
        << xshock_folder.option() << " " << xshock_folder.help() << "\n"
        << xshock_list_file.option() << " " << xshock_list_file.help() << "\n"
        << edge_id.option() << " " << edge_id.help() << "\n"
        << image_folder.option() << ": " << image_folder.help() << "\n"
        << image_list_file.option() << ": " << image_list_file.help() << "\n"
        << output_file.option() << " " << output_file.help() << "\n";
      return EXIT_SUCCESS;
          
    }
    
    if ( xshock_folder() == "" || xshock_list_file() == "" || edge_id() == 0 || 
      image_folder() == "" || image_list_file()== "" || output_file() == "")
    {
      vcl_cout << "ERROR: invalid input arguments. Use -help-extract-negative-xfrag-bhog to see necessary arguments.\n";
      return EXIT_FAILURE;
    }

    bool success = dbsks_extract_negative_xfrag_bhog(xshock_folder(),
      xshock_list_file(), edge_id(), image_folder(), image_list_file(), output_file());

    if (success)
    {
      vcl_cout << "Extracted boundary HOG of negative images sucessfully.\n";
      return EXIT_SUCCESS;
    }
    else
    {
      vcl_cout << "ERROR: operation " << extract_negative_xfrag_bhog.option() << " failed.\n";
      return 1;
    }  
  }
  //</>-------------------------------------------------------------------------
  
  

  // extract positive fragment BHOG in a list of shock graphs
  // example
  // argv[0] -extract-positive-xfrag-bhog -xshock-folder V:\projects\kimia\shockshape\symseg\results\ETHZ-database\xshock-graph\giraffes -xshock-list-file V:\projects\kimia\shockshape\symseg\results\ETHZ-database\xshock-graph\list_giraffes_xml.txt -edge-id 8 -image-folder V:\projects\kimia\shockshape\symseg\results\ETHZ-database\training\giraffes -output-file D:\vision\data\symseg\ETHZShapeClasses\training\giraffes-eid_8-positive.txt

  //< extract_positve_xfrag_bhog >----------------------------------------------
  if (extract_positive_xfrag_bhog())
  {
    if (print_help())
    {
      vcl_cout << "Usage with -extract-positive-xfrag-bhog option:\n"
        << image_folder.option() << " " << image_folder.help() << "\n"
        << xshock_folder.option() << " " << xshock_folder.help() << "\n"
        << xshock_list_file.option() << " " << xshock_list_file.help() << "\n"
        << edge_id.option() << " " << edge_id.help() << "\n"
        << output_file.option() << " " << output_file.help() << "\n";
      return EXIT_SUCCESS;
    }
    else
    {
      if ( image_folder() == "" || xshock_folder() == "" || xshock_list_file() == "" 
        || edge_id() == 0 || output_file() == "")
      {
        vcl_cout << "ERROR: invalid input arguments. Use -help-extract-positive-xfrag-bhog to see necessary arguments.\n";
        return -1;
      }
      //bool success = dbsks_extract_xfrag_properties(xshock_folder(), image_folder(), xshock_list_file(), edge_id(), output_file());
      bool success = dbsks_extract_positive_xfrag_bhog(xshock_folder(), image_folder(), xshock_list_file(), edge_id(), output_file());

      if (success)
      {
        vcl_cout << "Shape fragment properties extracted sucessfully.\n";
      }
    }
  }
  //</>-------------------------------------------------------------------------


  // Use a shape fragment's BHOG model and apply it to an image to detect the positive fragments
  if (detect_xfrag_using_bhog_model())
  {
    if (print_help())
    {
      vcl_cout << "Usage with " << detect_xfrag_using_bhog_model.option() << " option:\n"
      << image_folder.option() << " : " << image_folder.help() << "\n"
      << image_list_file.option() << " : " << image_list_file.help() << "\n"
      << libsvm_xfrag_bhog_model_file.option() << " : " << libsvm_xfrag_bhog_model_file.help() << "\n"
      << xfrag_geom_file.option() << " : " << xfrag_geom_file.help() << "\n"
      << output_file.option() << " " << output_file.help() << "\n";
      return EXIT_SUCCESS;
    }
    else
    {
      if ( image_folder() == "" || image_list_file() == "" || libsvm_xfrag_bhog_model_file() == "" ||
        xfrag_geom_file() == "" || output_file() == "")
      {
        vcl_cout << "ERROR: invalid input arguments. \n";
        return EXIT_SUCCESS;
      }

      //
      bool success = dbsks_detect_xfrag_using_bhog_model(libsvm_xfrag_bhog_model_file(),
        xfrag_geom_file(), 
        image_folder(),
        image_list_file(),
        output_file());

      if (success)
      {
        vcl_cout << "Finished xfrag detection.\n";
      }
      else
      {
        vcl_cout << "Xfrag detection failed.\n";
      }
      return 0;
    }
  }




  // <extend legs of giraffe models>---------------------------------------------------------
  if (extend_giraffe_legs())
  {
    return dbsks_extend_giraffe_legs() ? EXIT_SUCCESS : EXIT_FAILURE;
  }


  // <recompute bounding boxes of detections that were wrongly computed and saved>-----------
  if (recompute_detection_bbox())
  {
    return dbsks_recompute_detection_bbox() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  //--------------------------------------------------------------------------->
  
  if (print_help() || print_usage_only()) 
  {
    vul_arg_display_usage_and_exit();
    return 0;
  }

  return EXIT_SUCCESS;
}


