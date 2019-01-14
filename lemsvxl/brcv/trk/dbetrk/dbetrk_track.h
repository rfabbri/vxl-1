#ifndef _dbetrk_track_h_
#define _dbetrk_track_h_

#include <iostream>
#include <dbetrk/dbetrk_edge_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vector>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_point_2d.h>
#include <utility>


class cells{

public:
    cells(){}
    ~cells(){}
    int xmin,ymin,xmax,ymax;
    std::vector<dbetrk_edge_sptr> points;
};


class dbetrk_track{

public:
    dbetrk_track();
    ~dbetrk_track(){}

    bool fill_dbetrk_edges(int frame);

  
    void intialize(std::vector<dbetrk_edge_sptr>  tracked_dbetrk_edges_);
    double smooth(vil_image_view<float> in,vbl_array_2d<float> g2d);
    void assign_rgb_values(vil_image_view<float> p0,
                           vil_image_view<float> p1,
                           vil_image_view<float> p2, 
                           int r,int maskradius,
                           dbetrk_edge_sptr enode);


    bool compute_edges(int frame1,int frame2);
    bool intialize_cells(int w,int h);
    //: set parameters
    void setmotion(int m){motion_=m;}
   
    //: cost functions
    double compute_cost_IHS(dbetrk_edge_sptr e1, dbetrk_edge_sptr e2);
    double compute_cost_dist(dbetrk_edge_sptr e1, dbetrk_edge_sptr e2);
    

    double error_grad(dbetrk_edge_sptr e1,dbetrk_edge_sptr e2, double theta);


    void compute_particles(int frameno,
                           std::vector<std::pair<double,std::vector<dbetrk_edge_sptr> > > &particledata, 
                           int particleno=500);

    
    std::vector<std::vector<cells> >  cell_matrix;
    std::vector<dbetrk_edge_sptr> dbetrk_edges;
    std::vector<std::vector<dbetrk_edge_sptr> > tracked_dbetrk_edges;
    std::vector<vtol_edge_2d_sptr >  input_curves_;



    std::vector<vil_image_view<float> > plane0;
    std::vector<vil_image_view<float> > plane1;
    std::vector<vil_image_view<float> > plane2;

    std::vector<std::vector<std::pair<double,std::vector<std::pair<dbetrk_edge_sptr,dbetrk_edge_sptr> > > > > particlemap;

    float std_x;
    float std_y;
    float std_theta;
    int samplenum;
    float weight1,weight2;
        int frame_;

protected:
    bool track();
    int motion_;
    vgl_point_2d<double> mean_point(std::vector<dbetrk_edge_sptr> listofedges);

    
    
 
};


#endif
