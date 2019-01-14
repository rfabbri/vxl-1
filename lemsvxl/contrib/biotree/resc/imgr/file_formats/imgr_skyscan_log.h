//: This is /contrib/biotree/resc/imgr/file_formats/imgr_skyscan_log.h

#ifndef imgr_skyscan_log_h_
#define imgr_skyscan_log_h_

//: 
// \file    imgr_skyscan_log.h
// \brief   File format for the SkyScan log file 
//          NOTE: This is Brown copyrighted material which contains SkyScan 
//          proprietary information. Disseminating any information 
//          from this file is prohibited.
// \author  H. Can Aras
// \date    2005-06-15
// 

#include <vxl_config.h>
#include <xscan/xscan_scan.h>
#include <resc/imgr/file_formats/imgr_skyscan_log_header.h>
#include <vil/vil_image_resource_sptr.h>
#include <resc/imgr/imgr_scan_images_resource.h>

class imgr_skyscan_log : public imgr_scan_images_resource
{
public:
  //: Constructor
  imgr_skyscan_log(std::string log_fname);
  //: Destructor
  virtual ~imgr_skyscan_log();

  virtual xscan_scan get_scan() const;
  virtual void set_scan(xscan_scan scan);
  virtual std::vector<vil_image_resource_sptr> get_images() const;
  virtual std::vector<std::string> get_imagenames();

  void compute_scan();
  
  imgr_skyscan_log_header header() { return header_; }
  std::string images_fname() { return images_fname_; }
protected:
  imgr_skyscan_log_header header_;
  std::string images_fname_;
  mutable std::vector<std::string> filenames_;
};

// XML write
void x_write(std::ostream& os, imgr_skyscan_log log);

#endif

