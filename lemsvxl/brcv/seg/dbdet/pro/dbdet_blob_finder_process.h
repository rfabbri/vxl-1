// This is pro/dbdet_blob_finder_process.h
#ifndef dbdet_blob_finder_process_h_
#define dbdet_blob_finder_process_h_

//:
// \file
// \brief 
// \author Vishal Jain(vj@lems.brown.edu)
// \date 11/11/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

//: Derived video process class for computing frame difference
class dbdet_blob_finder_process : public bpro1_process {

public:

    dbdet_blob_finder_process();
    ~dbdet_blob_finder_process();

    vcl_string name();

    //: Clone the process
    virtual bpro1_process* clone() const;

    int input_frames();
    int output_frames();

    vcl_vector< vcl_string > get_input_type();
    vcl_vector< vcl_string > get_output_type();

    bool execute();
    bool finish();

    vcl_vector<vcl_vector<vsol_polygon_2d_sptr > > getpolygons();
    void write_polygons(vcl_string filename);

private:

    vcl_string polyfilename;
    double compute_aspect_ratio(vcl_vector<vgl_point_2d<double> > pts);
    vcl_vector<vcl_vector<vsol_polygon_2d_sptr > > frame_polygons_;
};

#endif
