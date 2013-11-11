// This is contrib/fine/pro/dbseg_seg_process.cxx

//:
// \file

#include "dbseg_seg_process.h"

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_sobel_1x3.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_orientations.h>


//: Constructor
dbseg_seg_process::dbseg_seg_process()
{
    if(!parameters()->add( "Alternative Region Fill (for low numbers of regions)?" , "-altFill" , false   ) 
      
    ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_seg_process::~dbseg_seg_process()
{
}


//: Clone the process
bpro1_process* 
dbseg_seg_process::clone() const
{
  return new dbseg_seg_process(*this);
}


//: Return the name of the process
vcl_string
dbseg_seg_process::name()
{
  return "Create Seg Structure";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbseg_seg_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbseg_seg_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("seg_object");
  return to_return;
}


//: Returns the number of input frames to this process
int
dbseg_seg_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_seg_process::output_frames()
{

  return 1; //default
}


//: Run the process on the current frame
bool
dbseg_seg_process::execute()
{
  if ( input_data_[0].size() != 2 )
  {
      vcl_cout << "In dbseg_seg_process::execute() - not exactly two"
               << " input images \n";
      return false;
  }
  clear_output();

  

  //get images
  vidpro1_image_storage_sptr orig_image;
  vidpro1_image_storage_sptr labeled_image;
  labeled_image.vertical_cast(input_data_[0][0]);
  orig_image.vertical_cast(input_data_[0][1]);
  vil_image_resource_sptr image_sptr = orig_image->get_image();
  //vil_image_view<image_sptr->pixel_format()> orig;
      bool altFill;
    parameters()->get_value("-altFill", altFill);

  //create seg_object
    dbseg_seg_storage_sptr stor = new dbseg_seg_storage(static_execute(labeled_image->get_image()->get_view(), image_sptr->get_view(), altFill));

    //new dbseg_seg_object<vxl_byte>(*labeled, orig);

    output_data_[0].push_back(stor);
  return true;
}


//: Finish
bool
dbseg_seg_process::finish()
{
  return true;
}

dbseg_seg_object_base* dbseg_seg_process::static_execute(vil_image_view_base_sptr labeled_image, vil_image_view_base_sptr original, bool altFill) {
    vil_image_view<int>* labeled;
    vil_image_view<vxl_byte> label;
    bool** done;
    bool** curr;
    vil_image_view<vxl_byte> orig;

      // if the original image is greyscale then make it RGB
  if (original->nplanes() == 1) { 
      vil_image_view<vxl_byte>* origPointer = new vil_image_view<vxl_byte>(original->ni(), original->nj(), 3);
      vil_image_view<vxl_byte> tempView = orig;
      for (int n = 0; n < 3; n++) {
          for (int i = 0; i < origPointer->ni(); i++) {
              for (int j = 0; j < origPointer->nj(); j++) {
                  (*origPointer)(i,j,n) = tempView(i,j);
              }
          }
      }
      orig = *origPointer;
  }
  else {
    orig = *original;
  }
  
  //if the segmented image is in boolean format then convert it to vxl_byte
  if (labeled_image->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
    label = *labeled_image;
  }
  else {
      vil_image_view<bool> tempbool = *labeled_image;
      label = vil_image_view<vxl_byte>(tempbool.ni(), tempbool.nj(), tempbool.nplanes());
      for (int i = 0; i < tempbool.ni(); i++) {
          for (int j = 0; j < tempbool.nj(); j++) {
              for (int k = 0; k < tempbool.nplanes(); k++) {
                  if (tempbool(i,j,k)) {
                    label(i,j,k) = 200;
                  }
                  else {
                      label(i,j,k) = 1;
                  }
              }
          }
        }
  }

  // if the segmented image is greyscale then make it RGB
  if (label.nplanes() == 1) { 
    vil_image_view<vxl_byte> templabel = label;
    label = vil_image_view<vxl_byte>(templabel.ni(), templabel.nj(), 3);
      for (int i = 0; i < templabel.ni(); i++) {
          for (int j = 0; j < templabel.nj(); j++) {
              for (int k = 0; k < 3; k++) {
                  label(i,j,k) = templabel(i,j);
              }
          }
      }
  }

    //set up parameters for filling the regions
    labeled = new vil_image_view<int>(orig.ni(), orig.nj(), 1);   
    
    done = new bool*[orig.ni()];
    curr = new bool*[orig.ni()];
      if (!altFill) {
        for (int i = 0; i < orig.ni(); i++) {
            done[i]=new bool[orig.nj()];
            curr[i]=new bool[orig.nj()];
            for (int j = 0; j < orig.nj(); j++) {
                (*labeled)(i,j) = 0;
                done[i][j] = false;
                curr[i][j] = false;
            }
        }
    }

    //fill the regions in the segmented image to create a labeled image

    set<int> colors;

    int regionCount = 0;
    vcl_cout << "Filling Regions..." << vcl_endl;

    if (!altFill) {
        int block = (  orig.ni() ) / 10;

        for (int i = 0; i < orig.ni(); i++) {
            //progress report
            if (block != 0) {
                if (i % block == 0) {
                    vcl_cout << i / block * 10 << "% ";
                }
            }
            for (int j = 0; j < orig.nj(); j++) {
                if (!done[i][j]) {
                    regionCount++;
                    for (int k = 0; k < orig.ni(); k++) {
                        for (int l = 0; l < orig.nj(); l++) {
                            curr[k][l] = false;
                        }
                    }

                    int i2 = i;
                    int j2 = j;
                    int r2 = (int)label(i,j,0);
                    int g2 = (int)label(i,j,1);
                    int b2 = (int)label(i,j,2);
                    fillRegion(i, j, (int)label(i,j,0), (int)label(i,j,1), (int)label(i,j,2), regionCount, done, curr, labeled, label);
                }
            }
        }
    }
    else {
        for (int i = 0; i < orig.ni(); i++) {
            for (int j = 0; j < orig.nj(); j++) {
                colors.insert(label(i,j,0)*1000000+label(i,j,1)*1000+label(i,j,2));
            }
        }
        set<int>::iterator itr;
        regionCount = colors.size();
        map<int, int> colorMap;
        int itrCount = 1;
        for (itr = colors.begin(); itr != colors.end(); ++itr) {
            colorMap[*itr] = itrCount;
            itrCount++;
        }

        
        /*int itrCount = 1;
        for (itr = colors.begin(); itr != colors.end(); ++itr) {*/
            for (int i = 0; i < orig.ni(); i++) {
                for (int j = 0; j < orig.nj(); j++) {
                    //if (orig(i,j,0) == (*itr)/1000000 && orig(i,j,1) == ((*itr)/1000)%1000 && orig(i,j,2) == (*itr)%1000) {
                    (*labeled)(i,j) = colorMap.find(label(i,j,0)*1000000+label(i,j,1)*1000+label(i,j,2))->second;
                    //}
                }
            }
            

            //itrCount++;
        //}
        
        
    
    }
    
    vcl_cout << vcl_endl << regionCount << " regions filled." << vcl_endl;

    delete done;
    delete curr;

    return new dbseg_seg_object<vxl_byte>(*labeled, orig);

}


void dbseg_seg_process::fillRegion(int x, int y, int r, int g, int b, int count, bool** done, bool** curr, vil_image_view<int>* labeled, vil_image_view<vxl_byte>& label) {
    if (x < 0 || y < 0 || x >= labeled->ni() || y >= labeled->nj()) {
        return;
    }
    if (done[x][y] || curr[x][y]) {
        return;
    }
    curr[x][y] = true;
    done[x][y] = (int)label(x,y,0) == r && (int)label(x,y,1) == g && (int)label(x,y,2) == b;
    if (done[x][y]) {
        (*labeled)(x,y) = count;
    }
    else {
        return;
    }
    for (int i = x-1; i <= x+1; i++) {
        for (int j = y-1; j <= y+1; j++) {
            if (!(i < 0 || j < 0 || i >= labeled->ni() || j >= labeled->nj())) {
                if (! (done[i][j] || curr[i][j]) ) {
                    fillRegion(i,j,r,g,b,count, done, curr, labeled, label);
                }
            }
        }
    }


}



