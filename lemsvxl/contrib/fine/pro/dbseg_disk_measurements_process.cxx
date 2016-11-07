//This is contrib/fine/pro/dbseg_disk_measurements_process.cxx

//:
// \file


#include "dbseg_disk_measurements_process.h"

#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
/*#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/algovtk/dbseg_disk_measurements_process.h>
*/


// ----------------------------------------------------------------------------
//: Constructor
dbseg_disk_measurements_process::
dbseg_disk_measurements_process() : bpro1_process()
{
  if( !parameters()->add( "Number of frames (the current should be the last frame):" , 
    "-num_frames", (int)1 ) ||
    !parameters()->add( "Number of disks"   , "-numdisks" , (int)1 ) ||
    !parameters()->add( "Data spacing dx"   , "-dx" , 1.0f ) ||
    !parameters()->add( "Data spacing dy"   , "-dy" , 1.0f ) ||
    !parameters()->add( "Data spacing dz"   , "-dz" , 1.0f ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}
//
// ----------------------------------------------------------------------------
//: Destructor
dbseg_disk_measurements_process::
~dbseg_disk_measurements_process()
{
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbseg_disk_measurements_process::
clone() const
{
  return new dbseg_disk_measurements_process(*this);
}

// ----------------------------------------------------------------------------
//: Return the name of the process
vcl_string dbseg_disk_measurements_process::
name()
{
  return "Disk Measurements";
}


// ----------------------------------------------------------------------------
//: Returns the number of input frames to this process
int dbseg_disk_measurements_process::
input_frames()
{
  int num_frames = -1;
  this->parameters()->get_value( "-num_frames" , num_frames );
  if (num_frames < 0)
    vcl_cout << "ERROR: number of frames is non-negative.\n";
  return num_frames;
}


// ----------------------------------------------------------------------------
//: Returns the number of output frames from this process
int dbseg_disk_measurements_process::
output_frames()
{
  return 0;
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbseg_disk_measurements_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("seg");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbseg_disk_measurements_process::
get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}


// ----------------------------------------------------------------------------
//: Run the process on the current frame
bool dbseg_disk_measurements_process::
execute()
{
  // parse input data

  /*// mesh filename
   bpro1_filepath mesh_path;
  this->parameters()->get_value( "-mesh_filename" , mesh_path );    
  vcl_string mesh_filename = mesh_path.path;
  */
  // data spacing
  float dx, dy, dz;
  this->parameters()->get_value( "-dx" , dx );    
  this->parameters()->get_value( "-dy" , dy );    
  this->parameters()->get_value( "-dz" , dz );   
    


  // number of frames
  int num_frames = -1;
  this->parameters()->get_value( "-num_frames" , num_frames );

    // number of disks
  int num_disks = 0;
    this->parameters()->get_value( "-numdisks" , num_disks );

  if (num_frames <= 0)
  {
    vcl_cerr << "In dbseg_disk_measurements_process::execute() - number of frames " 
      << "must be a positive integer number.\n";
    return false;
  
  }

  if (num_disks <= 0)
  {
    vcl_cerr << "In dbseg_disk_measurements_process::execute() - number of disks " 
      << "must be a positive integer number.\n";
    return false;
  
  }

  if ((int)(this->input_data_.size()) < num_frames)
  {
    vcl_cerr << "In dbseg_disk_measurements_process::execute() - not exactly " << num_frames
             << " input segs \n";
    return false;
  }

  /*vcl_cout << "Grouping the images into volumetric data.\n";
  vil3d_image_view<vxl_byte > img3d;

  // determine size of the image
  vidpro1_image_storage_sptr top_frame;
  top_frame.vertical_cast(this->input_data_[0][0]);
  unsigned ni = top_frame->get_image()->ni();
  unsigned nj = top_frame->get_image()->nj();

  img3d.set_size(ni, nj, num_frames+2);
  img3d.fill(0);
*/
  vcl_vector<dbseg_seg_object<vxl_byte>*> thesegs;
  
  //puts segs into vector
  for (int i=0; i<num_frames; ++i)
  {
    dbseg_seg_storage_sptr seg_storage;
    seg_storage.vertical_cast(input_data_[num_frames-1-i][0]);
    dbseg_seg_object<vxl_byte>* seg = static_cast<dbseg_seg_object<vxl_byte>*>(seg_storage->get_object());
    thesegs.push_back(seg);
  }
    vector<double> vol, provol, prodis;
    vol.resize(num_disks);
    vol.assign(num_disks, 0);

    provol.resize(num_disks);
    provol.assign(num_disks, 0);

    prodis.resize(num_disks);
    prodis.assign(num_disks, 0);

    for (int i = 0; i < num_frames; i++) {
        for (int j = 0; j < num_disks; j++) {
            int d;
            switch (j) {
                case 0:
                    d = 1;
                    break;
                case 1:
                    d = 3;
                    break;
                case 2:
                    d = 5;
                    break;
                case 3:
                    d = 7;
                    break;
            }
            vol[j] += dz * thesegs[i]->get_area(thesegs[i]->get_spine_part(d));

            vgl_polygon<double> poly = vgl_polygon<double>();
            poly.clear();
            poly.new_sheet();
            vgl_polygon<double> outline = *(thesegs[i]->get_outline(thesegs[i]->get_spine_part(d)).front());
            double x1, x2, y1, y2, x3, y3, x4, y4;
            thesegs[i]->get_disk_corners(1, x1, y1, x2, y2);
            //vgl_point_2d<double> top = vgl_closest_point(outline, vgl_point_2d<double>(x1, y1));
            //vgl_point_2d<double> bottom = vgl_closest_point(outline, vgl_point_2d<double>(x2, y2));
            int tn, bn, tnLeft, bnLeft;
            /*tn = 0;
            bn = 0;*/

            //new plan - just go around the polygon until the points closest to the right of the line are found
            double lineSlope = (y2-y1) / (x2-x1);

            //determine if the slope of the line is negative or positive
            int plusminus = 1;
            if ( lineSlope > 0 ) {
                plusminus = -1;
            }
            //now to the right of the line the y value must be greater than the line equation * plusminus

            //first start just to find something to the right of the line
            //move in a counter clockwise direction (i.e. add to the itr)
            int tempItr = 0;
            while (outline[0][tempItr].y() * plusminus < plusminus * (lineSlope * (outline[0][tempItr].x() - x1) + y1) ) {
                tempItr++;
                if (tempItr == outline[0].size()) {
                    tempItr = 0;
                }
            }

            //now that we're to the right of the outline, keep circling until you get to the left and the point just previous will be the top boundary
            while (outline[0][tempItr].y() * plusminus > (plusminus * (lineSlope * (outline[0][tempItr].x() - x1) + y1)) ) {
                double testy = outline[0][tempItr].y();
                double testx = outline[0][tempItr].x();
                double testright = plusminus * (lineSlope * (outline[0][tempItr].x() -x1) +y1);
                tempItr++;
                if (tempItr == outline[0].size()) {
                    tempItr = 0;
                }
            }
            tnLeft = tempItr;
            tn = tempItr - 1;
            if (tn < 0) {
                tn = outline[0].size() - 1;
            }

            //now circle around until you get back inside and that's the bottom boundary
            while (outline[0][tempItr].y() * plusminus < plusminus * (lineSlope * (outline[0][tempItr].x() - x1) + y1) ) {
                tempItr++;
                if (tempItr == outline[0].size()) {
                    tempItr = 0;
                }
            }
            bn = tempItr;
            bnLeft = tempItr - 1;
            if (bnLeft < 0) {
                bnLeft = outline[0].size() - 1;
            }

            //now find the intersecting point with the vertical line along the horizontal line between the two top boundary points
            double tempTx, tempTy, tempBx, tempBy;
        
            x3 = outline[0][tnLeft].x();
            y3 = outline[0][tnLeft].y();
            x4 = outline[0][tn].x();
            y4 = outline[0][tn].y();


            double ua = ( (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3) ) / ( (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1) );
            tempTx = x1 + ua*(x2-x1);
            tempTy = y1 + ua*(y2-y1);
            
            //do the same for the bottom part
            x3 = outline[0][bnLeft].x();
            y3 = outline[0][bnLeft].y();
            x4 = outline[0][bn].x();
            y4 = outline[0][bn].y();

            
            ua = ( (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3) ) / ( (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1) );
            tempBx = x1 + ua*(x2-x1);
            tempBy = y1 + ua*(y2-y1);


            // put the points into the polygon
            // start with all the points to the right of the line
            for (int q = bn; q != tn; q++) {
                if (q == outline[0].size()) {
                    q = 0;
                }
                poly[0].push_back(vgl_point_2d<double>(outline[0][q].x(), outline[0][q].y()));
                if (q == tn) {
                    break;
                }
            }


            //now push back the top boundary point
            poly[0].push_back(vgl_point_2d<double>(tempTx, tempTy));

            //now push back the bottom boundary point
            poly[0].push_back(vgl_point_2d<double>(tempBx, tempBy));

            thesegs[i]->set_disk_pro_poly(d, poly);
        
            provol[j] += dz * vgl_area<double>(poly);

            //find furthest distance
            double farDis = 0;
            int farPoint = 0;
            for (int q = 0; q < poly[0].size(); q++) {
                double tempDis = pow( pow( (x1 - x2) * (y2 - poly[0][q].y()) - (x2 - poly[0][q].x()) * (y1 - y2) , 2), .5) / 
                    pow( pow(x1 - x2, 2) + pow(y1 - y2, 2), .5);
                if (tempDis > farDis) {
                    farDis = tempDis;
                    farPoint = q;
                }
            }
            thesegs[i]->set_disk_pro(j+1, poly[0][farPoint].x(), poly[0][farPoint].y());
            
            //check if it is the biggest protrusion so far
            if (farDis > prodis[j]) {
                prodis[j] = farDis;
            }
        }
    }
    
    //print summary
    vcl_cout << "Measurements Summary:" << vcl_endl;
    for (int j = 0; j < num_disks; j++) {
        vcl_cout << "Total volume of disk " << j+1 << ": " << vol[j] << vcl_endl;
        vcl_cout << "Total protrusion volume of disk " << j+1 << ": " << provol[j] << vcl_endl;
        vcl_cout << "Max protrusion distance of disk " << j+1 << ": " << prodis[j] << vcl_endl;
    }
  return true;
}


// ----------------------------------------------------------------------------
//: Finish
bool dbseg_disk_measurements_process::
finish() 
{
  return true;
}









