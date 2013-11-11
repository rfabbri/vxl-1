#ifndef SMW_WORLD_H_
#define SMW_WORLD_H_
//this is contrib/bm/smw/smw_world_frames.h

#include "smw/smw_graph.h"

#include <vcl_fstream.h>

#include <vil/vil_image_view.h>


class smw_world
{
public:
    //create empty world
    smw_world():ni_(0),nj_(0){}
    
    smw_world(unsigned const& ni, unsigned const& nj);

    smw_world(vil_image_view<vxl_byte> const& img);

    ~smw_world(){}
    
    bool set_size(unsigned const&ni, unsigned const&nj);

    bool update(vil_image_view<vxl_byte> const& img);

    void size(unsigned& rows, unsigned& cols);
    
    unsigned ni(){return ni_;}
    
    unsigned nj(){return nj_;}

    vil_image_view<vxl_byte> change_map();

    //write a text file to disk that describes the world.
    //Number of graphs, number of nodes per graph and each 
    //mean and standard deviation.
    void print_log_file(vcl_string const& file_full_path);


private:
    unsigned ni_;
    unsigned nj_;
    vcl_vector< vcl_vector<smw_graph_gaussian_frames> > world_; 
};

#endif //SMW_WORLD_H_
