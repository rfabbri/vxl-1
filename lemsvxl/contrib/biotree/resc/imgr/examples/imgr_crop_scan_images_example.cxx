// This is contrib/resc/imgr/examples/imgr_crop_scan_images_example.cxx

//: 
// \file    imgr_crop_scan_images_example.cxx
// \brief   This example program uses binary write to crop the scan images to 
// \        a given box sizes. The results is saved as a binary form of 
// \        imgr_bounded_image_view in a *.bin file.
//          
// \author  Gamze Tunali
// \date    April 20, 2006

#include <string>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_save.h>
#include <vil/vil_memory_image.h>
//#include <imgr/file_formats/imgr_skyscan_log.h>
//#include <imgr/imgr_image_view_3d_base_sptr.h>
//#include <imgr/imgr_scan_resource_sptr.h>
//#include <imgr/imgr_scan_resource.h>
//#include <imgr/imgr_scan_resource_io.h>
//#include <imgr/imgr_bounded_image_view_3d.h>
////#include <bil/bil_bounded_image_view.h>
//#include <xscan/xscan_scan.h>

int file_check(std::string logfile, std::string scanfile, std::string boxfile)
{
    //file extension check
    std::size_t dot_pos = logfile.find_first_of(".");
    if(std::strcmp(logfile.substr(dot_pos+1, 3).data(), "log") != 0 && std::strcmp(logfile.substr(dot_pos+1, 3).data(), "LOG") != 0)
    {
      std::cout << "***********************************************" << std::endl;
      std::cout << "The specified log file extension is not correct" << std::endl;
      std::cout << "***********************************************" << std::endl;
      return 1;
    }
    dot_pos = scanfile.find_first_of(".");
    if(std::strcmp(scanfile.substr(dot_pos+1, 3).data(), "scn") != 0 && std::strcmp(scanfile.substr(dot_pos+1, 3).data(), "SCN") != 0)
    {
      std::cout << "************************************************" << std::endl;
      std::cout << "The specified scan file extension is not correct" << std::endl;
      std::cout << "************************************************" << std::endl;
      return 1;
    }
    dot_pos = boxfile.find_first_of(".");
    if(std::strcmp(boxfile.substr(dot_pos+1, 3).data(), "bx3") != 0 && std::strcmp(boxfile.substr(dot_pos+1, 3).data(), "BX3") != 0)
    {
      std::cout << "***********************************************" << std::endl;
      std::cout << "The specified box file extension is not correct" << std::endl;
      std::cout << "***********************************************" << std::endl;
      return 1;
    }
    return 1;
}

int main(int argc, char** argv)
{
  std::string logfile="", scanfile="", boxfile="", outfile="";

  if (argc < 5) {
    std::cerr << "Usage: " << argv[0] << " [-l logfile] [-s scanfile] [-b boxfile] [-o bin_output]" << std::endl;
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    std::string arg (argv[i]);
    if (arg == std::string ("-l")){
      logfile = std::string(argv[++i]);
      if (strchr(argv[i],0x22)){
        std::cerr << "quote start " << argv[i] << std::endl;
        int doneq =0;
        if (strchr(argv[i]+1,0x22))
          std::cerr << "quote end " << argv[i] << std::endl;
        else do{
          std::string arg (argv[++i]);
          logfile = logfile + " " + arg;
          std::cerr << " arg in q " << arg << std::endl;
          if (strchr(argv[i],0x22)){
             doneq = 1;
             std::cerr << "quote end " << argv[i] << std::endl;
          }
        }while ((doneq == 0) && (i < argc));
        logfile = logfile.substr(1, logfile.size()-2).data();
      }
    }

    else  if (arg == std::string ("-s")) { 
     scanfile = std::string(argv[++i]);
     if (strchr(argv[i],0x22)){
        std::cerr << "quote start " << argv[i] << std::endl;
        int doneq =0;
        if (strchr(argv[i]+1,0x22))
          std::cerr << "quote end " << argv[i] << std::endl;
        else do{
          std::string arg (argv[++i]);
          scanfile = scanfile + " " + arg;
          std::cerr << " arg in q " << arg << std::endl;
          if (strchr(argv[i],0x22)){
             doneq = 1;
             std::cerr << "quote end " << argv[i] << std::endl;
          }
        }while ((doneq == 0) && (i < argc));
        scanfile = scanfile.substr(1, scanfile.size()-2).data();
      }

}
    else if (arg == std::string ("-b")) { 
     boxfile = std::string (argv[++i]); 
     if (strchr(argv[i],0x22)){
        std::cerr << "quote start " << argv[i] << std::endl;
        int doneq =0;
        if (strchr(argv[i]+1,0x22))
          std::cerr << "quote end " << argv[i] << std::endl;
        else do{
          std::string arg (argv[++i]);
          boxfile = boxfile + " " + arg;
          std::cerr << " arg in q " << arg << std::endl;
          if (strchr(argv[i],0x22)){
             doneq = 1;
             std::cerr << "quote end " << argv[i] << std::endl;
          }
        }while ((doneq == 0) && (i < argc));
        boxfile = boxfile.substr(1, boxfile.size()-2).data();
      }

}
    else if (arg == std::string ("-o")) { 
     outfile = std::string (argv[++i]); 
     if (strchr(argv[i],0x22)){
        std::cerr << "quote start " << argv[i] << std::endl;
        int doneq =0;
        if (strchr(argv[i]+1,0x22))
          std::cerr << "quote end " << argv[i] << std::endl;
        else do{
          std::string arg (argv[++i]);
          outfile = outfile + " " + arg;
          std::cerr << " arg in q " << arg << std::endl;
          if (strchr(argv[i],0x22)){
             doneq = 1;
             std::cerr << "quote end " << argv[i] << std::endl;
          }
        }while ((doneq == 0) && (i < argc));
        outfile = outfile.substr(1, outfile.size()-2).data();
      }

}
    else {
      std::cout << "Usage: " << argv[0] << " [-l logfile] [-s scanfile] [-b boxfile] [-o bin_output]" << std::endl;
      return 1;
    }
  }
  if (file_check(logfile, scanfile, boxfile) == 0)
    return 1;

  // remove double quotes if any at the beg and end
  std::size_t first_q_pos, last_q_pos;

  std::string q = "\"";
  first_q_pos = logfile.find("\"");
  last_q_pos = logfile.rfind("\"");
  if (first_q_pos == std::string::npos)
    std::cout << "double quotes not found at the beginning" << std::endl;
  if (last_q_pos == std::string::npos)
    std::cout << "double quotes not found at the end" << std::endl;

  if ((first_q_pos == 0) && (last_q_pos == logfile.size()-1))
     logfile = logfile.substr(first_q_pos+1, logfile.size()-2);

  first_q_pos = scanfile.find("\"");
  last_q_pos = scanfile.rfind("\"");
  if ((first_q_pos == 0) && (last_q_pos == scanfile.size()-1))
     scanfile = scanfile.substr(first_q_pos+1, logfile.size()-2);

  first_q_pos = boxfile.find("\"");
  last_q_pos = boxfile.rfind("\"");
  if ((first_q_pos == 0) && (last_q_pos == boxfile.size()-1))
     boxfile = boxfile.substr(first_q_pos+1, logfile.size()-2);

  //// read the log file
  //imgr_skyscan_log log(logfile.data());
  //xscan_scan scan = log.get_scan();
  //std::cout << "SCAN BEFORE\n" << scan << std::endl;

  //// read the scan file
  //std::ifstream scan_file(scanfile.c_str());
  //scan_file >> scan;
  //scan_file.close();
  //std::cout << "SCAN AFTER\n" << scan << std::endl; 
  //log.set_scan(scan);
  //
  ////get the box
  //std::ifstream box_file(boxfile.c_str());
  //vgl_box_3d<double> box;
  //box.read(box_file);
  //box_file.close();
  //std::cout << "BOX\n" << box << std::endl;
  //
  //imgr_image_view_3d_base_sptr view_3d = log.get_bounded_view(box);
  ////unused variable unsigned nviews = scan.scan_size();
  //imgr_bounded_image_view_3d<unsigned short> v3d = view_3d;
  //vsl_b_ofstream os(outfile.data());
  //v3d.b_write(os);
  //os.close();
}
