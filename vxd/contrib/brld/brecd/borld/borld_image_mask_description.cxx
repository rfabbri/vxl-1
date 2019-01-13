//:
// \file
// \brief 
// \author Based on original code by      Ozge Can Ozcanli (@lems.brown.edu)
// \date        10/03/07
#include "borld_image_mask_description.h"

#include <iostream>

unsigned borld_image_mask_description::version()
{
  return 0;
}

void borld_image_mask_description::b_read()
{
  std::cout << "IMPLEMENT: borld_image_mask_description::b_read()\n";
}

void borld_image_mask_description::b_write()
{
  std::cout << "IMPLEMENT: borld_image_mask_description::b_write()\n";
}

//: nothing to write to the xml file --> just empty implementation to prevent abstract class instantiation error (error C2259)
void borld_image_mask_description::write_xml(std::ostream& os)
{
}
