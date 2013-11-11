#include <dbru/dbru_multiple_instance_object.h>
#include <vil/io/vil_io_image_view.h>
#include <vsl/vsl_vector_io.h>
#include <vsol/vsol_polygon_2d.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <dbru/dbru_label.h>

//: Copy constructor
dbru_multiple_instance_object::dbru_multiple_instance_object(dbru_multiple_instance_object const& rhs)
{
  imgs_ = rhs.imgs_;
  edges_ = rhs.edges_;
  poly_ = new vsol_polygon_2d(*rhs.poly_);
  bbox_ = new vsol_box_2d(*rhs.bbox_);
  bigbox_ = new vsol_box_2d(*rhs.bigbox_);
  label_ = new dbru_label(*rhs.label_);
}

vil_image_view<vxl_byte> dbru_multiple_instance_object::get_image_i(int i)
{
    if(i<int(imgs_.size()))
        return imgs_[i];

    return vil_image_view<vxl_byte>();
}

vcl_vector<vsol_line_2d_sptr> dbru_multiple_instance_object::get_edges()
{
    return edges_;
}

vsol_polygon_2d_sptr dbru_multiple_instance_object::get_poly()
{
    return poly_;
}

void dbru_multiple_instance_object::b_write(vsl_b_ostream &os) const
    {
        vsl_b_write(os, version());
        vsl_b_write(os, this->imgs_);
        vsl_b_write(os, this->edges_);
        vsl_b_write(os, this->poly_);
        vsl_b_write(os, this->bbox_);
        vsl_b_write(os, this->bigbox_);
        if (label_) {
            vsl_b_write(os, true);
            label_->b_write(os);
        } else
            vsl_b_write(os, false);

    }

void dbru_multiple_instance_object::b_read(vsl_b_istream &is) 
    {
    if (!is) return;
    short ver;
    vsl_b_read(is, ver);
    unsigned int len=0;
    switch(ver)
        {
    case 1: {
            vsl_b_read(is, this->imgs_);
            vsl_b_read(is, this->edges_);
            vsl_b_read(is, this->poly_);
            vsl_b_read(is, this->bbox_);
            vsl_b_read(is, this->bigbox_);

            break;
            }

    case 2: {
            vsl_b_read(is, this->imgs_);
            vsl_b_read(is, this->edges_);
            vsl_b_read(is, this->poly_);
            vsl_b_read(is, this->bbox_);
            vsl_b_read(is, this->bigbox_);
            bool label_exists;
            vsl_b_read(is, label_exists);
            if (label_exists) {
              this->label_ = new dbru_label();
              this->label_->b_read(is);
            }

            break;
            }

    default: {
            vcl_cerr << "I/O ERROR: dbctrk_tracker_curve::b_read(vsl_b_istream&)\n"
                << "           Unknown version number "<< ver << '\n';
            is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
            return;
             }
        }
    }

short dbru_multiple_instance_object::version() const
{
    return 2;
}

dbru_multiple_instance_object * dbru_multiple_instance_object::clone() const
{
  return new dbru_multiple_instance_object(*this);
}

void vsl_b_write(vsl_b_ostream & os, const dbru_multiple_instance_object * p)
{
    if (!p){
        vsl_b_write(os, false); // Indicate null pointer stored
    }
    else{
        vsl_b_write(os,true);  // Indicate non-null pointer stored
        p->b_write(os);
    }
}
void vsl_b_read(vsl_b_istream &is,dbru_multiple_instance_object * &p)
{
    delete(p);
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr){
        p = new dbru_multiple_instance_object();
        p->b_read(is);
    }
    else
        p = 0;
}

