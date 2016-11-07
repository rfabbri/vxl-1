// This is contrib/fine/pro/dbseg_seg_load_process.cxx

//:
// \file

#include <vcl_iostream.h>

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>

#include <vidpro1/vidpro1_repository.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>


#include <structure/dbseg_seg_object.h>

#include "dbseg_seg_load_process.h"

//: Constructor
dbseg_seg_load_process::dbseg_seg_load_process() : bpro1_process()
{
  if( !parameters()->add( "Segmentation Structure file <filename...>" , "-seg_filename" , bpro1_filepath("","*") ) 
      )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_seg_load_process::~dbseg_seg_load_process()
{
}


//: Clone the process
bpro1_process*
dbseg_seg_load_process::clone() const
{
  return new dbseg_seg_load_process(*this);
}


//: Return the name of the process
vcl_string dbseg_seg_load_process::name()
{
  return "Load Segmentation Structure";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbseg_seg_load_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  // no input type required
  to_return.clear();

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbseg_seg_load_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  // output type
  to_return.push_back( "seg" );

  return to_return;
}


dbseg_seg_object_base* dbseg_seg_load_process::static_execute(bpro1_filepath seg_path) {
  fstream in(seg_path.path.c_str(), fstream::in);
    dbseg_seg_object_base* obj;
    //load data
    double version;
    in >> version;
    if (version == 1) {
        int nplanes;
        in >> nplanes;

        int numIDs;
        in >> numIDs;
        list<int> validIDs;
        int idCount = 0;
        for (int j = 0; j < numIDs; j++) {
            int tempID;
            in >> tempID;
            if (tempID > idCount) {
                idCount = tempID;
            }
            validIDs.push_back(tempID);
        }
        idCount++;

        vector<int> parent_list;
        vector<list<int>> children_list;
        vector<dbseg_seg_spatial_object<vxl_byte>*> object_list;
        vector<list<int>> neighbor_list;

        parent_list.resize(idCount);
        children_list.resize(idCount);
        object_list.resize(idCount);
        neighbor_list.resize(idCount);


        list<int>::iterator i;

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int tempParent;
            in >> tempParent;
            parent_list[*i] = tempParent;
        }

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int tempChildCount;
            in >> tempChildCount;
            for (int j = 0; j < tempChildCount; j++) {
                int tempChild;
                in >> tempChild;
                children_list[*i].push_back(tempChild);
            }
        }

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int tempNeighborCount;
            in >> tempNeighborCount;
            for (int j = 0; j < tempNeighborCount; j++) {
                int tempNeighbor;
                in >> tempNeighbor;
                neighbor_list[*i].push_back(tempNeighbor);
            }
        }

        int maxDepth;
        in >> maxDepth;
        vector<list<int>> depth_list;
        depth_list.resize(maxDepth+1);

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int depth, bottom, top, left, right, r, g, b;
            in >> depth;
            in >> bottom;
            in >> top;
            in >> left;
            in >> right;
            in >> r;
            in >> g;
            in >> b;
            int tempNumOuterPoints;
            in >> tempNumOuterPoints;
            list<vgl_polygon<double>*> contours;
            vgl_polygon<double> tempPoly;
            tempPoly.new_sheet();
            for (int j2 = 0; j2 < tempNumOuterPoints; j2++) {
                double tempX, tempY;
                in >> tempX;
                in >> tempY;
                tempPoly.push_back(tempX, tempY);
            }
            contours.push_back(new vgl_polygon<double>(tempPoly));
            
            set<int> contained;
            int tempNumContained;
            in >> tempNumContained;
            for (int j2 = 0; j2 < tempNumContained; j2++) {
                int tempContained;
                in >> tempContained;
                contained.insert(tempContained);
            }
            for (int j2 = 0; j2 < tempNumContained; j2++) {
                tempPoly.clear();
                tempPoly.new_sheet();
                int tempNumInnerPoints;
                in >> tempNumInnerPoints;
                for (int j3 = 0; j3 < tempNumInnerPoints; j3++) {
                    double tempX, tempY;
                    in >> tempX;
                    in >> tempY;
                    tempPoly.push_back(tempX, tempY);
                }
                contours.push_back(new vgl_polygon<double>(tempPoly));
            }
            vector<vxl_byte> tempDataVector;
            tempDataVector.push_back(r);
            tempDataVector.push_back(g);
            tempDataVector.push_back(b);
            object_list[*i]=(new dbseg_seg_spatial_object<vxl_byte>(*i, depth, bottom, top, left, right, tempDataVector, contained, contours));
            depth_list[depth].push_back(*i);

        }

        int ni, nj;
        in >> ni;
        in >> nj;
        vil_image_view<int>* labeled = new vil_image_view<int>(ni, nj, 1);
        vil_image_view<vxl_byte>* image = new vil_image_view<vxl_byte>(ni,nj, 3);

        for (int x = 0; x < ni; x++) {
            for (int y = 0; y < nj; y++) {
                int tempLabel;
                in >> tempLabel;
                (*labeled)(x,y) = tempLabel;
            }
        }

        for (int j = 0; j < 3; j++) {
            for (int x = 0; x < ni; x++) {
                for (int y = 0; y < nj; y++) {
                    int tempValue;
                    in >> tempValue;
                    (*image)(x,y,j) = tempValue;
                }
            }
        }

      




    //fstream in(seg_path.path.c_str(), fstream::in);

    //vsl_b_read(vsl_b_istream &is, dbseg_seg_object<T>* &p);

      // create the storage data structure
      
      obj = new dbseg_seg_object<vxl_byte>(*labeled, *image, validIDs, object_list, parent_list, children_list, depth_list, neighbor_list);
    }
    if (version == 1.1) {
        int nparts;
        in >> nparts;
        vector<int> spine_parts;
        spine_parts.resize(nparts);
        for (int j = 0; j < nparts; j++) {
            int tempID;
            in >> tempID;
            spine_parts[j] = tempID;
        }

        in >> nparts;
        vector<double> disk_cornersX;
        vector<double> disk_cornersY;
        disk_cornersX.resize(nparts);
        disk_cornersY.resize(nparts);
        for (int j = 0; j < nparts; j++) {
            double tempCoord;
            in >> tempCoord;
            disk_cornersX[j] = tempCoord;
            in >> tempCoord;
            disk_cornersY[j] = tempCoord;

        }

        int nplanes;
        in >> nplanes;

        int numIDs;
        in >> numIDs;
        list<int> validIDs;
        int idCount = 0;
        for (int j = 0; j < numIDs; j++) {
            int tempID;
            in >> tempID;
            if (tempID > idCount) {
                idCount = tempID;
            }
            validIDs.push_back(tempID);
        }
        idCount++;

        vector<int> parent_list;
        vector<list<int>> children_list;
        vector<dbseg_seg_spatial_object<vxl_byte>*> object_list;
        vector<list<int>> neighbor_list;

        parent_list.resize(idCount);
        children_list.resize(idCount);
        object_list.resize(idCount);
        neighbor_list.resize(idCount);


        list<int>::iterator i;

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int tempParent;
            in >> tempParent;
            parent_list[*i] = tempParent;
        }

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int tempChildCount;
            in >> tempChildCount;
            for (int j = 0; j < tempChildCount; j++) {
                int tempChild;
                in >> tempChild;
                children_list[*i].push_back(tempChild);
            }
        }

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int tempNeighborCount;
            in >> tempNeighborCount;
            for (int j = 0; j < tempNeighborCount; j++) {
                int tempNeighbor;
                in >> tempNeighbor;
                neighbor_list[*i].push_back(tempNeighbor);
            }
        }

        int maxDepth;
        in >> maxDepth;
        vector<list<int>> depth_list;
        depth_list.resize(maxDepth+1);

        for (i = validIDs.begin(); i != validIDs.end(); ++i) {
            int depth, bottom, top, left, right, r, g, b;
            in >> depth;
            in >> bottom;
            in >> top;
            in >> left;
            in >> right;
            in >> r;
            in >> g;
            in >> b;
            int tempNumOuterPoints;
            in >> tempNumOuterPoints;
            list<vgl_polygon<double>*> contours;
            vgl_polygon<double> tempPoly;
            tempPoly.new_sheet();
            for (int j2 = 0; j2 < tempNumOuterPoints; j2++) {
                double tempX, tempY;
                in >> tempX;
                in >> tempY;
                tempPoly.push_back(tempX, tempY);
            }
            contours.push_back(new vgl_polygon<double>(tempPoly));
            
            set<int> contained;
            int tempNumContained;
            in >> tempNumContained;
            for (int j2 = 0; j2 < tempNumContained; j2++) {
                int tempContained;
                in >> tempContained;
                contained.insert(tempContained);
            }
            for (int j2 = 0; j2 < tempNumContained; j2++) {
                tempPoly.clear();
                tempPoly.new_sheet();
                int tempNumInnerPoints;
                in >> tempNumInnerPoints;
                for (int j3 = 0; j3 < tempNumInnerPoints; j3++) {
                    double tempX, tempY;
                    in >> tempX;
                    in >> tempY;
                    tempPoly.push_back(tempX, tempY);
                }
                contours.push_back(new vgl_polygon<double>(tempPoly));
            }
            vector<vxl_byte> tempDataVector;
            tempDataVector.push_back(r);
            tempDataVector.push_back(g);
            tempDataVector.push_back(b);
            object_list[*i]=(new dbseg_seg_spatial_object<vxl_byte>(*i, depth, bottom, top, left, right, tempDataVector, contained, contours));
            depth_list[depth].push_back(*i);

        }

        int ni, nj;
        in >> ni;
        in >> nj;
        vil_image_view<int>* labeled = new vil_image_view<int>(ni, nj, 1);
        vil_image_view<vxl_byte>* image = new vil_image_view<vxl_byte>(ni,nj, 3);

        for (int x = 0; x < ni; x++) {
            for (int y = 0; y < nj; y++) {
                int tempLabel;
                in >> tempLabel;
                (*labeled)(x,y) = tempLabel;
            }
        }

        for (int j = 0; j < 3; j++) {
            for (int x = 0; x < ni; x++) {
                for (int y = 0; y < nj; y++) {
                    int tempValue;
                    in >> tempValue;
                    (*image)(x,y,j) = tempValue;
                }
            }
        }

      




    //fstream in(seg_path.path.c_str(), fstream::in);

    //vsl_b_read(vsl_b_istream &is, dbseg_seg_object<T>* &p);

      // create the storage data structure
      
      obj = new dbseg_seg_object<vxl_byte>(*labeled, *image, validIDs, object_list, parent_list, children_list, depth_list, neighbor_list, spine_parts, disk_cornersX, disk_cornersY);

    }
    else {

    }
    in.close();
    return obj;

}

//: Run the process on the current frame
bool
dbseg_seg_load_process::execute()
{
    vul_timer t;
    t.mark();
  bpro1_filepath seg_path;
  parameters()->get_value( "-seg_filename" , seg_path );

  //no input storage class for this process
  clear_output();



 /* vsl_b_istream in2(&in);

    int version;
    vsl_b_read(in2, version);
    int nplanes;
    vsl_b_read(in2, nplanes);
    int numIDs;
    vsl_b_read(in2, numIDs);
    vcl_vector<int> validIDs;
    vsl_b_read(in2, validIDs);

    vsl_b_read(in2, obj);*/
    dbseg_seg_storage_sptr seg_storage = dbseg_seg_storage_new();  

  seg_storage->add_object(static_execute(seg_path));
    
  output_data_[0].push_back(seg_storage);
  vcl_cout << "Segmentation Structure loaded successfully in: " << t.real()/1000 << " seconds" << vcl_endl;

  return true;
}


//: Finish
bool dbseg_seg_load_process::finish() 
{
  return true;
}


//: Returns the number of input frames to this process
int
dbseg_seg_load_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_seg_load_process::output_frames()
{
  return 1;
}










/*
int numIDs;
in >> numIDs;
list<int> validIDs;
int idCount = 0;
for (int j = 0; j < numIDs; j++) {
    int tempID;
    in >> tempID;
    if (tempID > idCount) {
        idCount = tempID;
    }
    validIDs.push_back(tempID);
}
idCount++;

vector<int> parent_list;
vector<list<int>> children_list;
vector<seg_spatial_object*> object_list;
vector<list<int>> neighbor_list;

parent_list.resize(idCount);
children_list.resize(idCount);
object_list.resize(idCount);
neighbor_list.resize(idCount);


list<int>::iterator i;

for (i = validIDs.begin(); i != validIDs.end(); ++i) {
    int tempParent;
    //in >> tempChild;
    in >> tempParent;
    parent_list[*i] = tempParent;
}

for (i = validIDs.begin(); i != validIDs.end(); ++i) {
    int tempChildCount;
    in >> tempChildCount;
    for (int j = 0; j < tempChildCount; j++) {
        int tempChild;
        in >> tempChild;
        children_list[*i].push_back(tempChild);
    }
}

for (i = validIDs.begin(); i != validIDs.end(); ++i) {
    int tempNeighborCount;
    in >> tempNeighborCount;
    for (int j = 0; j < tempNeighborCount; j++) {
        int tempNeighbor;
        in >> tempNeighbor;
        neighbor_list[*i].push_back(tempNeighbor);
    }
}

int maxDepth;
in >> maxDepth;
vector<list<int>> depth_list;
depth_list.resize(maxDepth+1);

for (i = validIDs.begin(); i != validIDs.end(); ++i) {
    int depth, bottom, top, left, right, r, g, b;
    in >> depth;
    in >> bottom;
    in >> top;
    in >> left;
    in >> right;
    in >> r;
    in >> g;
    in >> b;
    int tempNumOuterPoints;
    in >> tempNumOuterPoints;
    list<vgl_polygon<double>*> contours;
    vgl_polygon<double> tempPoly;
    tempPoly.new_sheet();
    for (int j2 = 0; j2 < tempNumOuterPoints; j2++) {
        double tempX, tempY;
        in >> tempX;
        in >> tempY;
        tempPoly.push_back(tempX, tempY);
    }
    contours.push_back(&tempPoly);
    
    list<int> contained;
    int tempNumContained;
    in >> tempNumContained;
    for (int j2 = 0; j2 < tempNumContained; j2++) {
        int tempContained;
        in >> tempContained;
        contained.push_back(tempContained);
    }
    for (int j2 = 0; j2 < tempNumContained; j2++) {
        tempPoly.clear();
        tempPoly.new_sheet();
        int tempNumInnerPoints;
        in >> tempNumInnerPoints;
        for (int j3 = 0; j3 < tempNumInnerPoints; j3++) {
            double tempX, tempY;
            in >> tempX;
            in >> tempY;
            tempPoly.push_back(tempX, tempY);
        }
        contours.push_back(&tempPoly);
    }
    object_list.push_back(new seg_spatial_object(*i, depth, bottom, top, left, right, r, g, b, contained, contours);
    depth_list[depth].push_back(*i);
}

int ni, nj;
in >> ni;
in >> nj;
vil_image_view<int>* labeled = new vil_image_view<int>(ni, nj, 1);
vil_image_view<vxl_byte>* image = new vil_image_view<vxl_byte>(ni,nj, 3);

for (int x = 0; x < ni; x++) {
    for (int y = 0; y < nj; y++) {
        int tempLabel;
        in >> tempLabel;
        (*labeled)(x,y) = tempLabel;
    }
}

for (int j = 0; j < 3; j++) {
    for (int x = 0; x < ni; x++) {
        for (int y = 0; y < nj; y++) {
            int tempValue;
            in >> tempValue;
            (*image)(x,y,j) = tempValue;
        }
    }
}

seg_object* obj = new seg_object(*labeled, *image, validIDs, object_list, parent_list, children_list, depth_list, neighbor_list);



*/

