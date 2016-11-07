//This is contrib/fine/pro/dbseg_disk_bin_images_process.cxx

//:
// \file


#include "dbseg_disk_bin_images_process.h"

#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
/*#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/algovtk/dbseg_disk_bin_images_process.h>
*/


// ----------------------------------------------------------------------------
//: Constructor
dbseg_disk_bin_images_process::
dbseg_disk_bin_images_process() : bpro1_process()
{
  if( !parameters()->add( "Number of frames (the current should be the last frame):" , 
    "-num_frames", (int)1 ) ||
    !parameters()->add( "Number of disks"   , "-numdisks" , (int)1 ) ||
    !parameters()->add( "Disks"   , "-savedisks" , (bool)true )  || 
    !parameters()->add( "Protrusions"   , "-savepros" , (bool)true )  || 
    !parameters()->add( "Disks minus Protrusions"   , "-savenotpros" , (bool)true )  || 
    !parameters()->add( "Bones"   , "-savebones" , (bool)true )  || 
    !parameters()->add( "Spine"   , "-savespine" , (bool)true )    || 
    !parameters()->add( "Filename prefix" , "-filename", bpro1_filepath("",".png") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}
//
// ----------------------------------------------------------------------------
//: Destructor
dbseg_disk_bin_images_process::
~dbseg_disk_bin_images_process()
{
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbseg_disk_bin_images_process::
clone() const
{
  return new dbseg_disk_bin_images_process(*this);
}

// ----------------------------------------------------------------------------
//: Return the name of the process
vcl_string dbseg_disk_bin_images_process::
name()
{
  return "Disk Binary Images";
}


// ----------------------------------------------------------------------------
//: Returns the number of input frames to this process
int dbseg_disk_bin_images_process::
input_frames()
{
  int num_frames = -1;
  this->parameters()->get_value( "-num_frames" , num_frames );
  if (num_frames < 0)
    vcl_cout << "ERROR: number of frames is non-negative.\n";
  return num_frames;
}


// ----------------------------------------------------------------------------
//: Returns the number of output frames from this process
int dbseg_disk_bin_images_process::
output_frames()
{
  return 0;
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbseg_disk_bin_images_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("seg");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbseg_disk_bin_images_process::
get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}


// ----------------------------------------------------------------------------
//: Run the process on the current frame
bool dbseg_disk_bin_images_process::
execute()
{
  // parse input data

  /*// mesh filename
   bpro1_filepath mesh_path;
  this->parameters()->get_value( "-mesh_filename" , mesh_path );    
  vcl_string mesh_filename = mesh_path.path;
  */


  // number of frames
  int num_frames = -1;
  this->parameters()->get_value( "-num_frames" , num_frames );

    // number of disks
  int num_disks = 0;
    this->parameters()->get_value( "-numdisks" , num_disks );

    // what to save
    bool save_disks, save_spine, save_pros, save_bones, save_notpros;
    this->parameters()->get_value( "-savedisks" , save_disks );
    this->parameters()->get_value( "-savepros" , save_pros );
    this->parameters()->get_value( "-savenotpros" , save_notpros );
    this->parameters()->get_value( "-savespine" , save_spine );
    this->parameters()->get_value( "-savebones" , save_bones );

    bpro1_filepath file;
    this->parameters()->get_value( "-filename" , file );

  if (num_frames <= 0)
  {
    vcl_cerr << "In dbseg_disk_bin_images_process::execute() - number of frames " 
      << "must be a positive integer number.\n";
    return false;
  
  }

  if (num_disks <= 0)
  {
    vcl_cerr << "In dbseg_disk_bin_images_process::execute() - number of disks " 
      << "must be a positive integer number.\n";
    return false;
  
  }

  if ((int)(this->input_data_.size()) < num_frames)
  {
    vcl_cerr << "In dbseg_disk_bin_images_process::execute() - not exactly " << num_frames
             << " input segs \n";
    return false;
  }

  /*vcl_cout << "Grouping the images into volumetric data.\n";
  vil3d_image_view<vxl_byte > img3d;

  // determine size of the image
  vidpro1_image_storage_sptr top_frame;
  top_frame.vertical_cast(this->input_data_[0][0]);
  unsigned ni = top_frame->get_image()->ni();
  unsigned nj = top_frame->get_image()->nj();

  img3d.set_size(ni, nj, num_frames+2);
  img3d.fill(0);
*/
  vcl_vector<dbseg_seg_object<vxl_byte>*> thesegs;
  
  //puts segs into vector
  for (int i=0; i<num_frames; ++i)
  {
    dbseg_seg_storage_sptr seg_storage;
    seg_storage.vertical_cast(input_data_[num_frames-1-i][0]);
    dbseg_seg_object<vxl_byte>* seg = static_cast<dbseg_seg_object<vxl_byte>*>(seg_storage->get_object());
    thesegs.push_back(seg);
  }

   int ni = thesegs[0]->get_labeled_image().ni();
   int nj = thesegs[0]->get_labeled_image().nj();

    vil_image_view<vxl_byte> binaryImage = vil_image_view<vxl_byte>(ni, nj);
    vil_image_view<vxl_byte> binaryImage2 = vil_image_view<vxl_byte>(ni, nj);
    
    for (int i = 0; i < num_frames; i++) {
        std::stringstream ss;
        ss << i;

        //reset binary image
        for (int ni2 = 0; ni2 < ni; ni2++) {
            for (int nj2 = 0; nj2 < nj; nj2++) {
                binaryImage(ni2,nj2) = 0;
            }
        }
        
        //save the disks
        if (save_disks || save_notpros) {
            for (int j = 0; j < num_disks; j++) {
                int d;
                //convert to spine part
                switch (j) {
                    case 0:
                        d = 1;
                        break;
                    case 1:
                        d = 3;
                        break;
                    case 2:
                        d = 5;
                        break;
                    case 3:
                        d = 7;
                        break;
                }
                //get the tempID of the spine part
                int tempID = thesegs[i]->get_spine_part(d);

                //go through the bounding box of the object and color in the binary image
                int left, right, top, bottom;
                left = thesegs[i]->get_object_list()[tempID]->get_left();
                right = thesegs[i]->get_object_list()[tempID]->get_right();
                top = thesegs[i]->get_object_list()[tempID]->get_top();
                bottom = thesegs[i]->get_object_list()[tempID]->get_bottom();
                for (int itrI = left; itrI <= right; itrI++) {
                    for (int itrJ = top; itrJ <= bottom; itrJ++) {
                        if (thesegs[i]->get_pixel_ID(itrI, itrJ, 1)==tempID) {
                            binaryImage(itrI,itrJ) = 255;
                        }
                    }
                }
            }
            if (save_disks) {
                std::string savefile = (file.path).substr(0,file.path.length()-4);
                //add a 0 in front of single digits
                
                if (i < 10) {
                    savefile += "_disks0";
                    savefile += ss.str();
                    savefile += ".png";
                }
                else {
                    savefile += "_disks";
                    savefile += ss.str();
                    savefile += ".png";
                }
                vil_save(binaryImage, savefile.c_str());
            }
        }
        //binaryImage2 = vil_image_view<vxl_byte>(binaryImage);
        //reset binary image2
        for (int ni2 = 0; ni2 < ni; ni2++) {
            for (int nj2 = 0; nj2 < nj; nj2++) {
                binaryImage2(ni2,nj2) = 0;
            }
        }

        //save the disk protrusions
        if (save_pros || save_notpros) {
            for (int j = 0; j < num_disks; j++) {
                int d;
                //convert to spine part
                switch (j) {
                    case 0:
                        d = 1;
                        break;
                    case 1:
                        d = 3;
                        break;
                    case 2:
                        d = 5;
                        break;
                    case 3:
                        d = 7;
                        break;
                }
                //get the tempID of the spine part
                int tempID = thesegs[i]->get_spine_part(d);
                vgl_polygon<double> tempPoly = thesegs[i]->get_disk_pro_poly(j+1);
                //go through the bounding box of the object and color in the binary image
                int left, right, top, bottom;
                left = thesegs[i]->get_object_list()[tempID]->get_left();
                right = thesegs[i]->get_object_list()[tempID]->get_right();
                top = thesegs[i]->get_object_list()[tempID]->get_top();
                bottom = thesegs[i]->get_object_list()[tempID]->get_bottom();
                for (int itrI = left; itrI <= right; itrI++) {
                    for (int itrJ = top; itrJ <= bottom; itrJ++) {
                        if (tempPoly.contains(itrI, itrJ)) {
                            binaryImage2(itrI,itrJ) = 255;
                        }
                    }
                }
            }
            if (save_pros) {
                std::string savefile = (file.path).substr(0,file.path.length()-4);
                //add a 0 in front of single digits
                if (i < 10) {
                    savefile += "_pros0";
                    savefile += ss.str();
                    savefile += ".png";
                }
                else {
                    savefile += "_pros";
                    savefile += ss.str();
                    savefile += ".png";
                }
                vil_save(binaryImage2, savefile.c_str());
            }
        }
    
        //save the disks minus the protrusions
        if (save_notpros) {
            for (int ni2 = 0; ni2 < ni; ni2++) {
                for (int nj2 = 0; nj2 < nj; nj2++) {
                    //set protrusions portions to be black on disk image
                    if (binaryImage2(ni2, nj2) == 255) {
                        //int a = binaryImage2(ni2, nj2);
                        binaryImage(ni2, nj2) = 0;
                    }
                }
            }
            std::string savefile = (file.path).substr(0,file.path.length()-4);
            //add a 0 in front of single digits
            if (i < 10) {
                savefile += "_notpros0";
                savefile += ss.str();
                savefile += ".png";
            }
            else {
                savefile += "_notpros";
                savefile += ss.str();
                savefile += ".png";
            }
            vil_save(binaryImage, savefile.c_str());
        
        }

        //reset binary image
        for (int ni2 = 0; ni2 < ni; ni2++) {
            for (int nj2 = 0; nj2 < nj; nj2++) {
                binaryImage(ni2,nj2) = 0;
            }
        }


    //save bones
        if (save_bones) {
            //there are two bones surrounding each disk, but they are shared in between
            
            // do the first bone first always
            for (int j = -1; j < num_disks; j++) {
                int b;
                //convert to spine part
                switch (j) {
                    case -1:
                        b = 0; //first bone
                        break;
                    case 0:
                        b = 2;
                        break;
                    case 1:
                        b = 4;
                        break;
                    case 2:
                        b = 6;
                        break;
                    case 3:
                        b = 8;
                        break;
                }
                //get the tempID of the spine part
                int tempID = thesegs[i]->get_spine_part(b);

                //go through the bounding box of the object and color in the binary image
                int left, right, top, bottom;
                left = thesegs[i]->get_object_list()[tempID]->get_left();
                right = thesegs[i]->get_object_list()[tempID]->get_right();
                top = thesegs[i]->get_object_list()[tempID]->get_top();
                bottom = thesegs[i]->get_object_list()[tempID]->get_bottom();
                for (int itrI = left; itrI <= right; itrI++) {
                    for (int itrJ = top; itrJ <= bottom; itrJ++) {
                        if (thesegs[i]->get_pixel_ID(itrI, itrJ, 1)==tempID) {
                            binaryImage(itrI,itrJ) = 255;
                        }
                    }
                }
            }
        
            std::string savefile = (file.path).substr(0,file.path.length()-4);
            //add a 0 in front of single digits
            if (i < 10) {
                savefile += "_bones0";
                savefile += ss.str();
                savefile += ".png";
            }
            else {
                savefile += "_bones";
                savefile += ss.str();
                savefile += ".png";
            }
            vil_save(binaryImage, savefile.c_str());
        }

        //reset binary image
        for (int ni2 = 0; ni2 < ni; ni2++) {
            for (int nj2 = 0; nj2 < nj; nj2++) {
                binaryImage(ni2,nj2) = 0;
            }
        }


        //save spine
        if (save_spine) {
            
                        
            //get the tempID of the spine
            int tempID = thesegs[i]->get_spine_part(9);

            //go through the bounding box of the object and color in the binary image
            int left, right, top, bottom;
            left = thesegs[i]->get_object_list()[tempID]->get_left();
            right = thesegs[i]->get_object_list()[tempID]->get_right();
            top = thesegs[i]->get_object_list()[tempID]->get_top();
            bottom = thesegs[i]->get_object_list()[tempID]->get_bottom();
            for (int itrI = left; itrI <= right; itrI++) {
                for (int itrJ = top; itrJ <= bottom; itrJ++) {
                    if (thesegs[i]->get_pixel_ID(itrI, itrJ, 1)==tempID) {
                        binaryImage(itrI,itrJ) = 255;
                    }
                }
            }
        
            std::string savefile = (file.path).substr(0,file.path.length()-4);
            //add a 0 in front of single digits
            if (i < 10) {
                savefile += "_spine0";
                savefile += ss.str();
                savefile += ".png";
            }
            else {
                savefile += "_spine";
                savefile += ss.str();
                savefile += ".png";
            }
            vil_save(binaryImage, savefile.c_str());    
        }


    }    

    
    
  return true;
}


// ----------------------------------------------------------------------------
//: Finish
bool dbseg_disk_bin_images_process::
finish() 
{
  return true;
}









