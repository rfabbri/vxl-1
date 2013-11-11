#include <Spatemp/algo/dbdet_delaunay_map.h>
#include <dbdet/sel/dbdet_edgel.h>

int main()
{
    vcl_vector<dbdet_edgel*> edges;
    //edges.push_back(new dbdet_edgel(vgl_point_2d<double>(0,0),0.0,0.0,0.0,0));
    //edges.push_back(new dbdet_edgel(vgl_point_2d<double>(0,10),0.0,0.0,0.0,1));
    //edges.push_back(new dbdet_edgel(vgl_point_2d<double>(5,5),0.0,0.0,0.0,2));
    //edges.push_back(new dbdet_edgel(vgl_point_2d<double>(10,0),0.0,0.0,0.0,3));
    //edges.push_back(new dbdet_edgel(vgl_point_2d<double>(10,10),0.0,0.0,0.0,4));

    edges.push_back(new dbdet_edgel(vgl_point_2d<double>(-117.1613217111385000,32.7103913962800800),0.0,0.0,0.0,0));
    edges.push_back(new dbdet_edgel(vgl_point_2d<double>(-117.1703290363452100,32.7104470424023500),0.0,0.0,0.0,1));
    edges.push_back(new dbdet_edgel(vgl_point_2d<double>(-117.1682018256883000, 32.7145577202166040),0.0,0.0,0.0,2));
    edges.push_back(new dbdet_edgel(vgl_point_2d<double>(-117.1700655819055500,32.7170675019933480),0.0,0.0,0.0,3));
    edges.push_back(new dbdet_edgel(vgl_point_2d<double>(-117.1656765782523700,32.7186671643681630),0.0,0.0,0.0,4));
    edges.push_back(new dbdet_edgel(vgl_point_2d<double>(-117.1633942534826900, 32.7156642662183330),0.0,0.0,0.0,5));
    edges.push_back(new dbdet_edgel(vgl_point_2d<double>(-117.1636024880840500, 32.7119968579811770),0.0,0.0,0.0,6));
    edges.push_back(new dbdet_edgel(vgl_point_2d<double>(-117.1636024880840500, 32.7119968579811770),0.0,0.0,0.0,7));


  


    edges[0]->id=0;
    edges[1]->id=1;
    edges[2]->id=2;
    edges[3]->id=3;
    edges[4]->id=4;
    edges[5]->id=5;
    edges[6]->id=6;
        edges[7]->id=7;
    dbdet_delaunay_map d(edges);

    for(unsigned i=0;i<d.neighbor_map_.size();i++)
    {
        vcl_cout<<i << ": ";
        for(unsigned j=0;j<d.neighbor_map_[i].size();j++)
        {
            vcl_cout<<d.neighbor_map_[i][j]<<" ";
        }
        vcl_cout<<"\n";
    }

    return 0;
}
