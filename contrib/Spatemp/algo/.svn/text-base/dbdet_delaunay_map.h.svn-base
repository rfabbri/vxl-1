//: this is contrib/spatemp/algo/dbdet_delaunay_map
#ifndef dbdet_delaunay_map_h
#define dbdet_delaunay_map_h

#include <vcl_map.h>

#include <Spatemp/algo/dbdet_temporal_model.h>
#include <Spatemp/algo/dbdet_temporal_bundle.h>
#include <dbdet/sel/dbdet_curvelet.h>
#include <dbdet/sel/dbdet_edgel.h>
const int MaxVertices = 500;
const int MaxTriangles = 1000;
const int n_MaxPoints = 10; // for the test programm
const double EPSILON = 0.000001;


class dbdet_delaunay_edge{
public:
    dbdet_delaunay_edge(unsigned node1_id,unsigned node2_id);
    ~dbdet_delaunay_edge(){}

    unsigned node1_id_;
    unsigned node2_id_;

    double weight_;

    int edge_id_;

    bool    operator==(dbdet_delaunay_edge e2);
protected:

};

class dbdet_delaunay_triangle{
public:
    dbdet_delaunay_triangle(unsigned node1_id,unsigned node2_id,unsigned node3_id);
    ~dbdet_delaunay_triangle(){}

    unsigned node1_id_;
    unsigned node2_id_;
    unsigned node3_id_;

    double weight_;

    int id_;

    bool is_grouped_;

    int return_third_id(int id1,int id2);

    bool    operator==(dbdet_delaunay_triangle t2);
protected:

};

//: class to store delauany graph betweeen edges.

class dbdet_delaunay_map{

public:
    dbdet_delaunay_map(vcl_vector<dbdet_edgel*> edges,vcl_vector<bool> is_included);
 
    dbdet_delaunay_map(vcl_vector<dbdet_edgel*> edges);
    ~dbdet_delaunay_map(){}

    vcl_vector<dbdet_delaunay_edge> delaunay_edges_;


    void threshold_delaunay_edges(double t);
    void compute_delaunay(vcl_vector<dbdet_edgel*> edges, vcl_vector<bool> ons);
    void recompute_delaunay();
    vcl_vector<int> get_neighbors(int id){return neighbor_map_[id];}
    vcl_vector< vcl_vector<int> > neighbor_map_;
    vcl_vector< vcl_vector<int> > triangle_map_;
    vcl_vector<dbdet_delaunay_triangle> triangles_;


protected:

    vcl_vector<dbdet_edgel*> edges_;
    vcl_vector<bool> is_included_;
};


#endif
