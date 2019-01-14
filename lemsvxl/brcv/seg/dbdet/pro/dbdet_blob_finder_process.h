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
#include <vector>
#include <string>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

//: Derived video process class for computing frame difference
class dbdet_blob_finder_process : public bpro1_process {

public:

    dbdet_blob_finder_process();
    ~dbdet_blob_finder_process();

    std::string name();

    //: Clone the process
    virtual bpro1_process* clone() const;

    int input_frames();
    int output_frames();

    std::vector< std::string > get_input_type();
    std::vector< std::string > get_output_type();

    bool execute();
    bool finish();

    std::vector<std::vector<vsol_polygon_2d_sptr > > getpolygons();
    void write_polygons(std::string filename);

private:

    std::string polyfilename;
    double compute_aspect_ratio(std::vector<vgl_point_2d<double> > pts);
    std::vector<std::vector<vsol_polygon_2d_sptr > > frame_polygons_;
};

#endif
