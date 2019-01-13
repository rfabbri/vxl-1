//
// vsrl_region_disparity.cxx
//
// Routines to calculate disparity based on regions
// rather than pixels.
//
// G.W. Brooksby
// 09/13/03, 09/17/03
// Written at ORD on two LONG layovers...

#include "vsrl_region_disparity.h"
#include <iostream>
#include <vtol/vtol_intensity_face.h>

vsrl_region_disparity::vsrl_region_disparity()
{
  l_img_ = NULL;
  r_img_ = NULL;
  d_img_ = NULL;
  this->init();
}

vsrl_region_disparity::vsrl_region_disparity(vil1_image* li, vil1_image* ri)
{
  l_img_ = li;
  r_img_ = ri;
  this->init();
}

vsrl_region_disparity::~vsrl_region_disparity()
{
  delete reg_disp_img_;
}

void vsrl_region_disparity::init()
{
  std::cout << "vsrl_region_disparity::init()\n";
  if_regions_ = NULL;
  digi_regions_ = NULL;
  reg_disp_img_ = NULL;
  region_type_ = UNDEFINED;
}

void vsrl_region_disparity::SetRegions(std::vector<vtol_intensity_face_sptr>* regs)
{
  std::cout << "vsrl_region_disparity::SetRegions(intensity faces)\n";
  if_regions_ = regs;
  region_type_ = INTENSITY_FACE;
}

void vsrl_region_disparity::SetRegions(std::vector<vdgl_digital_region*>* regs)
{
  std::cout << "vsrl_region_disparity::SetRegions(digital regions)\n";
  digi_regions_ = regs;
  region_type_ = DIGITAL_REGION;
}

bool vsrl_region_disparity::Execute()
{
  bool result;
  switch (region_type_)
  {
   case INTENSITY_FACE:
    std::cout << "vsrl_region_disparity::Execute: region type: INTENSITY_FACE\n";
    result = run_intensity_faces();
    break;
   case DIGITAL_REGION:
    std::cout << "vsrl_region_disparity::Execute: region type: DIGITAL_REGION\n";
    result = run_digital_regions();
    break;
   case UNDEFINED:
   default:
    std::cerr << "vsrl_region_disparity::Execute: Error: region type is UNDEFINED.\n";
    result = false;
    break;
  }
  return result;
}

bool vsrl_region_disparity::run_intensity_faces()
{
  // Remember, Intensity Faces contain digital regions.
  // Make sure intensity faces were established before proceeding.
  bool result = false;
  std::cerr << "vsrl_region_disparity::run_intesity_faces...\n";
  if (if_regions_ == NULL) {
    std::cerr << "vsrl_region_disparity::run_intesity_faces: Error:\n"
             << "No intensity faces present.\n";
    return false;
  }

  // overwrite anything that may exist.  If digi_regions_ doesn't start from
  // scratch, it may have garbage in it and it may produce garbage.
  if (digi_regions_ != NULL) {
    delete digi_regions_;
    digi_regions_ = new std::vector<vdgl_digital_region*>;
  }

  for (std::vector<vtol_intensity_face_sptr>::iterator fit = if_regions_->begin();
       fit != if_regions_->end(); fit++) {
    vdgl_digital_region* reg = (*fit)->cast_to_digital_region();
    digi_regions_->push_back(reg);
  }
  // Now we have a vector of digital regions, pass it off...

  this->run_digital_regions();

  return result;
}

bool vsrl_region_disparity::run_digital_regions()
{
  std::cerr << "vsrl_region_disparity::run_digital_regions...\n";
  // Error checking
  if ( digi_regions_ == NULL) {
    std::cerr << "vsrl_region_disparity::run_digital_regions: Error:\n"
             << "No digital regions present.\n";
    return false;
  }

  // Up-front prep.
  // create the new disparity image in memory...
  if ( reg_disp_img_ != NULL) delete reg_disp_img_; // Clear out the old...
  reg_disp_img_ = new vil1_memory_image_of<double>(d_img_->width(),d_img_->height());  // Prepare for the new...
  for (int c=0; c<reg_disp_img_->width(); c++) {
    for (int r=0; r<reg_disp_img_->height(); r++) {
      (*reg_disp_img_)(c,r)=0.0; // Start with a zero'd buffer
    }
  }

  // iterate through all the regions
  std::vector<vdgl_digital_region*>::iterator rit;
  for (rit = (*digi_regions_).begin(); rit != (*digi_regions_).end(); rit++) {
    // For each region, get the pixel coordinates
    double avg=0;
    int npix=0;
    for ( (*rit)->reset(); (*rit)->next();) {
      int x = int((*rit)->X()), y = int((*rit)->Y()); // get coordinates of this pixel
      // Get the disparity from the disparity image for the corresponding pixels.
      // Average the disparities across the entire region.
      avg += int((*d_img_)(x,y));
      npix++;
    }
    avg /= npix;  // Final division to get the average

    std::cout << "Region average disparity: " << avg
             << " Number of Pixels: " << npix << std::endl;

    // Now that we have the average for the region, insert
    // it into every pixel of that region in the disparity image
    for ( (*rit)->reset(); (*rit)->next();) {
      int x = int((*rit)->X()), y = int((*rit)->Y()); // get coordinates of this pixel
      // Insert the average disparity into a new disparity image.
      (*reg_disp_img_)(x,y) = avg;
    }
  }
  return true;
}

