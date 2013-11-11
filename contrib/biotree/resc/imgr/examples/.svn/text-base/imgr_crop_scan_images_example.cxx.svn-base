// This is contrib/resc/imgr/examples/imgr_crop_scan_images_example.cxx

//: 
// \file    imgr_crop_scan_images_example.cxx
// \brief   This example program uses binary write to crop the scan images to 
// \        a given box sizes. The results is saved as a binary form of 
// \        imgr_bounded_image_view in a *.bin file.
//          
// \author  Gamze Tunali
// \date    April 20, 2006

#include <vcl_string.h>
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

int file_check(vcl_string logfile, vcl_string scanfile, vcl_string boxfile)
{
    //file extension check
    vcl_size_t dot_pos = logfile.find_first_of(".");
    if(vcl_strcmp(logfile.substr(dot_pos+1, 3).data(), "log") != 0 && vcl_strcmp(logfile.substr(dot_pos+1, 3).data(), "LOG") != 0)
    {
      vcl_cout << "***********************************************" << vcl_endl;
      vcl_cout << "The specified log file extension is not correct" << vcl_endl;
      vcl_cout << "***********************************************" << vcl_endl;
      return 1;
    }
    dot_pos = scanfile.find_first_of(".");
    if(vcl_strcmp(scanfile.substr(dot_pos+1, 3).data(), "scn") != 0 && vcl_strcmp(scanfile.substr(dot_pos+1, 3).data(), "SCN") != 0)
    {
      vcl_cout << "************************************************" << vcl_endl;
      vcl_cout << "The specified scan file extension is not correct" << vcl_endl;
      vcl_cout << "************************************************" << vcl_endl;
      return 1;
    }
    dot_pos = boxfile.find_first_of(".");
    if(vcl_strcmp(boxfile.substr(dot_pos+1, 3).data(), "bx3") != 0 && vcl_strcmp(boxfile.substr(dot_pos+1, 3).data(), "BX3") != 0)
    {
      vcl_cout << "***********************************************" << vcl_endl;
      vcl_cout << "The specified box file extension is not correct" << vcl_endl;
      vcl_cout << "***********************************************" << vcl_endl;
      return 1;
    }
    return 1;
}

int main(int argc, char** argv)
{
  vcl_string logfile="", scanfile="", boxfile="", outfile="";

  if (argc < 5) {
    vcl_cerr << "Usage: " << argv[0] << " [-l logfile] [-s scanfile] [-b boxfile] [-o bin_output]" << vcl_endl;
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    vcl_string arg (argv[i]);
    if (arg == vcl_string ("-l")){
      logfile = vcl_string(argv[++i]);
      if (strchr(argv[i],0x22)){
        vcl_cerr << "quote start " << argv[i] << vcl_endl;
        int doneq =0;
        if (strchr(argv[i]+1,0x22))
          vcl_cerr << "quote end " << argv[i] << vcl_endl;
        else do{
          vcl_string arg (argv[++i]);
          logfile = logfile + " " + arg;
          vcl_cerr << " arg in q " << arg << vcl_endl;
          if (strchr(argv[i],0x22)){
             doneq = 1;
             vcl_cerr << "quote end " << argv[i] << vcl_endl;
          }
        }while ((doneq == 0) && (i < argc));
        logfile = logfile.substr(1, logfile.size()-2).data();
      }
    }

    else  if (arg == vcl_string ("-s")) { 
     scanfile = vcl_string(argv[++i]);
     if (strchr(argv[i],0x22)){
        vcl_cerr << "quote start " << argv[i] << vcl_endl;
        int doneq =0;
        if (strchr(argv[i]+1,0x22))
          vcl_cerr << "quote end " << argv[i] << vcl_endl;
        else do{
          vcl_string arg (argv[++i]);
          scanfile = scanfile + " " + arg;
          vcl_cerr << " arg in q " << arg << vcl_endl;
          if (strchr(argv[i],0x22)){
             doneq = 1;
             vcl_cerr << "quote end " << argv[i] << vcl_endl;
          }
        }while ((doneq == 0) && (i < argc));
        scanfile = scanfile.substr(1, scanfile.size()-2).data();
      }

}
    else if (arg == vcl_string ("-b")) { 
     boxfile = vcl_string (argv[++i]); 
     if (strchr(argv[i],0x22)){
        vcl_cerr << "quote start " << argv[i] << vcl_endl;
        int doneq =0;
        if (strchr(argv[i]+1,0x22))
          vcl_cerr << "quote end " << argv[i] << vcl_endl;
        else do{
          vcl_string arg (argv[++i]);
          boxfile = boxfile + " " + arg;
          vcl_cerr << " arg in q " << arg << vcl_endl;
          if (strchr(argv[i],0x22)){
             doneq = 1;
             vcl_cerr << "quote end " << argv[i] << vcl_endl;
          }
        }while ((doneq == 0) && (i < argc));
        boxfile = boxfile.substr(1, boxfile.size()-2).data();
      }

}
    else if (arg == vcl_string ("-o")) { 
     outfile = vcl_string (argv[++i]); 
     if (strchr(argv[i],0x22)){
        vcl_cerr << "quote start " << argv[i] << vcl_endl;
        int doneq =0;
        if (strchr(argv[i]+1,0x22))
          vcl_cerr << "quote end " << argv[i] << vcl_endl;
        else do{
          vcl_string arg (argv[++i]);
          outfile = outfile + " " + arg;
          vcl_cerr << " arg in q " << arg << vcl_endl;
          if (strchr(argv[i],0x22)){
             doneq = 1;
             vcl_cerr << "quote end " << argv[i] << vcl_endl;
          }
        }while ((doneq == 0) && (i < argc));
        outfile = outfile.substr(1, outfile.size()-2).data();
      }

}
    else {
      vcl_cout << "Usage: " << argv[0] << " [-l logfile] [-s scanfile] [-b boxfile] [-o bin_output]" << vcl_endl;
      return 1;
    }
  }
  if (file_check(logfile, scanfile, boxfile) == 0)
    return 1;

  // remove double quotes if any at the beg and end
  vcl_size_t first_q_pos, last_q_pos;

  vcl_string q = "\"";
  first_q_pos = logfile.find("\"");
  last_q_pos = logfile.rfind("\"");
  if (first_q_pos == vcl_string::npos)
    vcl_cout << "double quotes not found at the beginning" << vcl_endl;
  if (last_q_pos == vcl_string::npos)
    vcl_cout << "double quotes not found at the end" << vcl_endl;

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
  //vcl_cout << "SCAN BEFORE\n" << scan << vcl_endl;

  //// read the scan file
  //vcl_ifstream scan_file(scanfile.c_str());
  //scan_file >> scan;
  //scan_file.close();
  //vcl_cout << "SCAN AFTER\n" << scan << vcl_endl; 
  //log.set_scan(scan);
  //
  ////get the box
  //vcl_ifstream box_file(boxfile.c_str());
  //vgl_box_3d<double> box;
  //box.read(box_file);
  //box_file.close();
  //vcl_cout << "BOX\n" << box << vcl_endl;
  //
  //imgr_image_view_3d_base_sptr view_3d = log.get_bounded_view(box);
  ////unused variable unsigned nviews = scan.scan_size();
  //imgr_bounded_image_view_3d<unsigned short> v3d = view_3d;
  //vsl_b_ofstream os(outfile.data());
  //v3d.b_write(os);
  //os.close();
}
