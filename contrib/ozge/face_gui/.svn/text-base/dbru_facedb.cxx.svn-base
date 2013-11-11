#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_vector_io.h>
#include <vcl_cassert.h>
#include <dbinfo/dbinfo_observation.h>
#include <ozge/face_gui/dbru_facedb.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_new.h>

dbru_facedb::dbru_facedb() 
{
}
dbru_facedb::~dbru_facedb()
{
  //facedb_->clear();
  facedb_.clear();
  
}

void dbru_facedb::clear()
{
  facedb_.clear();
}

void dbru_facedb::
add_subject(vcl_vector<vil_image_resource_sptr> subject)
{
  facedb_.push_back(subject);
}

void dbru_facedb::
add_subject(vcl_vector<vil_image_resource_sptr> subject, unsigned i)
{
  if (i < facedb_.size()) {
    for (unsigned j = 0; j < subject.size(); j++)
      facedb_[i].push_back(subject[j]);
  }
}

//: get a particular subject
vcl_vector<vil_image_resource_sptr>& dbru_facedb::
get_subject(unsigned i)
{
  assert(i < facedb_.size());
  return facedb_[i];
}

//: get a particular image of a particular subject in facedb
vil_image_resource_sptr dbru_facedb::
get_face(unsigned i, unsigned j) 
{
  if (i < facedb_.size()) {
    if (j < facedb_[i].size())
      return facedb_[i][j];
    else
      return 0;
  } else
    return 0;
}

void dbru_facedb::print(vcl_ostream& os) const
{
  unsigned nsubjects = this->n_subjects();
  os << this->is_a() << " [\n"
     << "n subjects " << nsubjects << '\n';
  os << "\n ]\n";
}

//: Binary save self to stream.
void dbru_facedb::b_write(vsl_b_ostream &os) const
{
  //dbru_facedb& facedb = (dbru_facedb&)*this;  
  vsl_b_write(os, version());
  unsigned int size = facedb_.size();
  bool flag = false;
  if (size != 0) {
    flag = true;
    vsl_b_write(os, flag);
    vsl_b_write(os, size);
    for (unsigned i = 0; i<size; i++) {
      bool flagi = false;
      unsigned sizei = facedb_[i].size();
      if (sizei != 0) {
        flagi = true;
        vsl_b_write(os, flagi);
        vsl_b_write(os, sizei);
        for (unsigned j = 0; i<sizei; j++) {
          vil_image_resource_sptr image_ = facedb_[i][j];
          if(image_){
            vsl_b_write(os, true);
            vsl_b_write(os, int(image_->pixel_format()));
            switch(image_->pixel_format())
            {
            case VIL_PIXEL_FORMAT_BYTE:
              {  
                vil_image_view<vxl_byte> image = image_->get_view();
                vsl_b_write(os, image);
                break;
              }
            case VIL_PIXEL_FORMAT_FLOAT:
              {  
                vil_image_view<float> image = image_->get_view();
                vsl_b_write(os, image);
                break;
              }
            case VIL_PIXEL_FORMAT_DOUBLE:
              {  
                vil_image_view<double> image = image_->get_view();
                vsl_b_write(os, image);
                break;
              }    
            default:
              vcl_cerr << "I/O ERROR: This pixel format is not supported\n";
              return; 
            }
          }
        }
      } else {
        vsl_b_write(os, flagi);
      }
    }
  } else 
    vsl_b_write(os, flag);
}

//: Binary load self from stream.
void dbru_facedb::b_read(vsl_b_istream &is)
{
  //this->clear();
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        bool flag = false;
        vsl_b_read(is, flag);
        if (flag) {
          unsigned int size;
          vsl_b_read(is, size);  
          for (unsigned i = 0; i<size; i++) {
            bool flagi = false;
            vsl_b_read(is, flagi);
            if (flagi) {
              unsigned sizei;
              vsl_b_read(is, sizei);
              vcl_vector<vil_image_resource_sptr > subj;
              for (unsigned j = 0; i<sizei; j++) {
                vil_image_resource_sptr image_;
                bool is_valid;
                vsl_b_read(is, is_valid);
                if(is_valid){
                  int format;
                  vil_pixel_format pixel_format;
                  vsl_b_read(is, format); pixel_format=vil_pixel_format(format);
                  switch(pixel_format)
                  {
                    case VIL_PIXEL_FORMAT_BYTE:
                      {
                        vil_image_view<vxl_byte> image;
                        vsl_b_read(is, image);
                        image_ = vil_new_image_resource_of_view(image);
                        break;
                      }
                    case VIL_PIXEL_FORMAT_FLOAT:
                      {
                        vil_image_view<float> image;
                        vsl_b_read(is, image);
                        image_ = vil_new_image_resource_of_view(image);
                        break;
                      }
                    case VIL_PIXEL_FORMAT_DOUBLE:
                      {
                        vil_image_view<double> image;
                        vsl_b_read(is, image);
                        image_ = vil_new_image_resource_of_view(image);
                        break;
                      }      
                    default:
                      vcl_cerr << "I/O ERROR: This pixel format is not supported\n";
                      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
                      return; 
                    }
                    
                  }
                subj.push_back(image_);
              }
              this->add_subject(subj);
            }
          }
        } 
      }
    }
}


