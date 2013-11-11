// This is brcv/rec/dbru/dbru_multiple_instance.h

//:
// \file
// \brief Class to hold edges of the same vehicle coming over multiple fr
//
// \author Vishal Jain
// \date 
//      
// \endverbatim

#ifndef _dbru_multiple_instance_object_
#define _dbru_multiple_instance_object_

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_view.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>

#include <dbru/dbru_label_sptr.h>
#include <dbru/dbru_label.h>

class dbru_multiple_instance_object:public vbl_ref_count
    {
    public:
        //: constructor
        dbru_multiple_instance_object(){};
       
        //: Copy constructor
        dbru_multiple_instance_object(dbru_multiple_instance_object const & rhs);
        //: destructor
        ~dbru_multiple_instance_object(){};
        //: initialize the images
        void set_images(vcl_vector<vil_image_view<vxl_byte> > images){imgs_=images;}
        void set_edges(vcl_vector<vsol_line_2d_sptr>  edges){edges_=edges;}
        void set_poly(vsol_polygon_2d_sptr poly){poly_=poly;}
        void set_bbox(vsol_box_2d_sptr bbox){bbox_=bbox;}
        void set_bigbox(vsol_box_2d_sptr bbox){bigbox_=bbox;}
        void set_label(dbru_label * label){label_=label;}
        
        unsigned imgs_size() { return imgs_.size(); }
        unsigned edgels_size() { return edges_.size(); }

        vil_image_view<vxl_byte> get_image_i(int i);
        vcl_vector<vil_image_view<vxl_byte> >& get_images() { return imgs_; }
        vcl_vector<vsol_line_2d_sptr> get_edges();
        vsol_polygon_2d_sptr get_poly();
        vsol_box_2d_sptr get_box() { return bbox_; }
        vsol_box_2d_sptr get_img_box() { return bigbox_; }
        dbru_label * get_label() { return label_; }

        //: Binary save self to stream.
        void b_write(vsl_b_ostream &os) const;

        //: Binary load self from stream.
        void b_read(vsl_b_istream &is);

        //: Return IO version number;
        short version() const;

        virtual dbru_multiple_instance_object * clone() const;

    private:
        vcl_vector<vil_image_view<vxl_byte> > imgs_;
        vcl_vector<vsol_line_2d_sptr>  edges_;
        vsol_polygon_2d_sptr poly_;
        vsol_box_2d_sptr bbox_;
        // image box, outer one
        vsol_box_2d_sptr bigbox_;

        dbru_label *label_;
    };

//: Binary save dbctrk_tracker_curve_sptr to stream.
void vsl_b_write(vsl_b_ostream & os, const dbru_multiple_instance_object * p);
//: Binary load dbctrk_tracker_curve_sptr to stream
void vsl_b_read(vsl_b_istream &is,dbru_multiple_instance_object * &p);

//void vsl_add_to_binary_loader(dbru_multiple_instance_object const& b);

#endif
