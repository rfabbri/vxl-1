/*************************************************************************
*    NAME: Daniel Spinosa
*    FILE: bfrag2D.cpp
*    DATE: 
*************************************************************************/

#include "bfrag2D.h"

bfrag_curve bfrag2D::getTopContourAsCurve(){
  assert(topContourCurveMade);
  return topContourCurve;
}

bfrag2D::bfrag2D(){

  //vcl_cout << "made a new UNINDEXED bfrag2D" << vcl_endl;

  topRGBImage = 0;
  botRGBImage = 0;
  topPolygon = 0;
  botPolygon = 0;

  topCurveFn = "";
  botCurveFn = "";
  topImageFn = "";
  botImageFn = "";

  topContourCurveMade = 0;
  botContourCurveMade = 0;

  _centerX = 0;
  _centerY = 0;

}

bfrag2D::bfrag2D( int index, vcl_string s ){

  myName = s;
  myIndex = index;

  //vcl_cout << "MADE NEW BFRAG2D index: " << myIndex << vcl_endl;

  topRGBImage = 0;
  botRGBImage = 0;
  topPolygon = 0;
  botPolygon = 0;

  topCurveFn = "";
  botCurveFn = "";
  topImageFn = "";
  botImageFn = "";

  topContourCurveMade = 0;
  botContourCurveMade = 0;

  _centerX = 0;
  _centerY = 0;


}

void bfrag2D::popupInfo(bool &drawTopImage, bool &drawBotCurve, bool &drawBotImage){

  vgui_dialog info( "bFrag2D Information..." );
  //info.set_modal(true);
  char infomsg[2048];


  sprintf( infomsg, "Bfrag Info\n Name: %s ", myName.c_str());
  info.message( infomsg );
  sprintf( infomsg, "Index: %i", myIndex );
  info.message( infomsg );
  sprintf( infomsg, "Top Curve Filename: %s", topCurveFn.c_str() );
  info.message( infomsg );
  if(topRGBImage->size() != 0){
    sprintf( infomsg, "Top Image Filename: %s", topImageFn.c_str() );
    info.message( infomsg );  
    info.checkbox( "Draw Top Image?", drawTopImage );
  } else info.message( "Piece does not have a Top Image" );
  if(botPolygon){
    sprintf( infomsg, "Bot Curve Filename: %s", botCurveFn.c_str() );
    info.message( infomsg );
    info.checkbox( "Draw Bottom Curve?", drawBotCurve );
  } else info.message( "Piece does not have a Bottom Curve" );
  if(botRGBImage->size() != 0){
    sprintf( infomsg, "Bot Image Filename: %s", botImageFn.c_str() );
    info.message( infomsg );
    info.checkbox( "Draw Bottom Image?", drawBotImage );
  } else info.message( "Piece does not have a Bottom Image" );

  info.set_cancel_button(0);
  info.ask();
}

bfrag2D::~bfrag2D()
{

  //vcl_cout << "deleted bfrag2D index: " << myIndex << vcl_endl;

}

/*******************************************************************************
*
*  load a contour (either .con or .cem, determiend by filename extension) and
*  store it as the contour for the top or bottom of this bfrag, as determiend
*  by the boolean top (by default, load as top)
*
*******************************************************************************/
int bfrag2D::loadContour( vcl_string fn, bool top ){

  //its ok if were tryin to load a blank bottom contour, but we MUST have top
  if( !top && fn.size() == 0 ) return 0;

  //vcl_cout << "bfrag [" << myIndex << "] loading contour \"" << fn;
  //if(top) vcl_cout << "\" as TOP contour" << vcl_endl;
  //else    vcl_cout << "\" as BOTTOM contour" << vcl_endl;

  //determine extension and call appropriate loader, appropriately
  if( strncmp( fn.c_str() + fn.size() - 4, ".con", 4 ) == 0 || 
    strncmp( fn.c_str() + fn.size() - 4, ".CON", 4 ) == 0 ){
      //vcl_cout << "Loading a \".con\" file: " << fn << vcl_endl;

      if(top){
        topCurveFn = fn;
        int err = loadCON( fn, topPolygon, topContourCurve );
        if(err == 0){
          calcCenter();
          topContourCurveMade = true;
          topContourCurve.frag_id_ = this->frag_id_;
//          topContourCurve.setFrag(this);
//          topContourCurve.setTop(true);
        }
        return err;
      } else {
        botCurveFn = fn;
        int err = loadCON( fn, botPolygon, botContourCurve );
        if(err == 0){
          botContourCurveMade = true;
          botContourCurve.frag_id_ = this->frag_id_;
//          botContourCurve.setFrag(this);
//          botContourCurve.setTop(false);
        }
        return err;
      }


  } else if( strncmp( fn.c_str() + fn.size() - 4, ".cem", 4 ) == 0 ||
    strncmp( fn.c_str() + fn.size() - 4, ".CEM", 4 ) == 0 ) {
      //vcl_cout << "Loading up a \".cem\" file: " << fn << vcl_cout;

      if(top){
        topCurveFn = fn;
        int err = loadCEM( fn, topPolygon, topContourCurve );
        if( err == 0){
          calcCenter();
          topContourCurveMade = true;
          topContourCurve.frag_id_ = this->frag_id_;
//          topContourCurve.setFrag(this);
//          topContourCurve.setTop(true);
        }
        return err;
      } else {
        botCurveFn = fn;
        int err = loadCEM( fn, botPolygon, botContourCurve );
        if( err == 0 ){
          botContourCurveMade = true;
          botContourCurve.frag_id_ = this->frag_id_;
//          botContourCurve.setFrag(this);
//          botContourCurve.setTop(false);
        }
        return err;
      }

  } else {
    //file was not a .cem or .con
    vcl_cout << "ERROR: \"" << fn << "\" is not a .cem or .con file! -Aborting-" << vcl_endl;

    return -1;

  }


  //execution will not reach this point
  return 0;

}

/*******************************************************************************
*
*  load an image (determined by extension, loaded with vil) and 
*  store it as the image for the top or bottom of this bfrag, as determiend
*  by the boolean top (by default, load as top)
*
*******************************************************************************/
int bfrag2D::loadImage( vcl_string fn, bool top ){

  if( fn.size() == 0 ) return 0;

  //vcl_cout << "bfrag [" << myIndex << "] loading image \"" << fn;
  if(top){
    //vcl_cout << "\" as TOP image" << vcl_endl;

    topRGBImage = vil_load( fn.c_str() );
    if( topRGBImage->size() == 0 ){
      vcl_cout << "ERROR: image " << fn << " could not be loaded." << vcl_endl;
      return -1;
    }

    topImageFn = fn;

    //vcl_cout << "_______________________________________________" << vcl_endl;
    //vcl_cout << "Image just loaded: " << fn << " has [" << topRGBImage.nplanes() << "planes." << vcl_endl;


  } else {
    //vcl_cout << "\" as BOTTOM image" << vcl_endl;

    botRGBImage = vil_load( fn.c_str() );
    if( botRGBImage->size() == 0 ){
      vcl_cout << "ERROR: image " << fn << " could not be loaded." << vcl_endl;
      return -1;
    }

    botImageFn = fn;
  }




  //on success
  return 0;

}




/***************************************************
* this code was borrowed and adapted from 
* vidpro1_load_cem_process.cxx
***************************************************/


int bfrag2D::loadCEM( vcl_string fn, vsol_polygon_2d_sptr &givenContour, bfrag_curve &givenCurve ){

  vcl_cout << "***WARNING*** loadCEM is not yet supported!" << vcl_endl;

  double x, y;
  char lineBuffer[1024];
  int numContours, numTotalEdges, numEdges;
  int ix, iy;
  double idir, iconf, dir, conf;

  // new vvector to store the points
  vcl_vector< vsol_spatial_object_2d_sptr > contours;

  //1)If file open fails, return.
  vcl_ifstream infp(fn.c_str(), vcl_ios::in);

  if (!infp){
    vcl_cout << "Error opening file  " << fn.c_str() << vcl_endl;
    return -1;
  }

  //2)Read in each line
  while (infp.getline(lineBuffer,1024)) {

    //ignore comment lines and empty lines
    if (strlen(lineBuffer)<2 || lineBuffer[0]=='#')
      continue;

    //read the line with the contour count info
    if (!strncmp(lineBuffer, "CONTOUR_COUNT=", sizeof("CONTOUR_COUNT=")-1)){
      sscanf(lineBuffer,"CONTOUR_COUNT=%d",&(numContours));
      continue;
    }

    //read the line with the edge count info
    if (!strncmp(lineBuffer, "TOTAL_EDGE_COUNT=", sizeof("TOTAL_EDGE_COUNT=")-1)){
      sscanf(lineBuffer,"TOTAL_EDGE_COUNT=%d",&(numTotalEdges));
      continue;
    }

    //read the beginning of a contour block
    if (!strncmp(lineBuffer, "[BEGIN CONTOUR]", sizeof("[BEGIN CONTOUR]")-1)){

      //discarding other information for now...should really be outputting edgels
      vcl_vector< vsol_point_2d_sptr > points;

      infp.getline(lineBuffer,1024);
      sscanf(lineBuffer,"EDGE_COUNT=%d",&(numEdges));

      for (int j=0; j< numEdges; j++){
        //the rest should have data that goes into the current contour
        infp.getline(lineBuffer,1024);
        sscanf(lineBuffer," [%d, %d]\t%lf\t%lf\t[%lf, %lf]\t%lf\t%lf",&(ix), &(iy),
          &(idir), &(iconf), &(x), &(y), &(dir), &(conf));

        //VJ's current CEM is in degrees rather than radians so need to convert
        dir += 90;
        dir *= vnl_math::pi/180;


        vsol_point_2d_sptr newPt = new vsol_point_2d (x,y);
        points.push_back(newPt);

//        givenCurve.append(x,y);
        givenCurve.append(vgl_point_2d<double>(x,y));
      }

      infp.getline(lineBuffer,1024);

      while (strncmp(lineBuffer, "[END CONTOUR]", sizeof(" [END CONTOUR]")-1)) {
        infp.getline(lineBuffer,1024);
      }

      //can also use vsol_polygon_2d in same fashion...
      givenContour = new vsol_polygon_2d(points);

    }
  }

//  givenCurve.computeProperties();
  givenCurve.compute_properties();

  infp.close();

  //on success
  return 0;
}




/***********************************************************
* .con file loading code adapted from
*  bsol_intrinsic_curve_2d.cxx
***********************************************************/

int bfrag2D::loadCON( vcl_string fn, vsol_polygon_2d_sptr &givenContour, bfrag_curve &givenCurve ){

  double x, y;
  int nPoints;
  char buffer[2048];
  vcl_vector< vsol_point_2d_sptr > points;

  //1)If file open fails, return.
  vcl_ifstream fp(fn.c_str(), vcl_ios::in);
  if (!fp) {
    vcl_cout<<" : Unable to Open "<< fn << vcl_endl;
    return -1;
  }

  //2)Read in file header.
  fp.getline(buffer,2000); //CONTOUR
  //fp.getline(buffer,2000); //OPEN/CLOSE
  char openFlag[200];
  fp.getline(openFlag,200);

  //check to see if its open or closed and set that in the Curve
  if (!strncmp(openFlag,"OPEN",4))
    givenCurve.is_open_ = true;
  else if (!strncmp(openFlag,"CLOSE",5))
    givenCurve.is_open_ = false;
  else
  {
    vcl_cout << "Invalid File " << fn << vcl_endl;
    vcl_cout << "Should be OPEN/CLOSE: " << openFlag << vcl_endl;
    return -1;
  }


  fp >> nPoints;
  //vcl_cout << "Number of Points from Contour:" << nPoints << vcl_endl;

  for (int i=0; i < nPoints; i++) {
    fp >> x >> y;
    //add_vertex(x,y);
    vsol_point_2d_sptr newPt = new vsol_point_2d (x,y);
    points.push_back(newPt);

    //put it into a puzzle solving style curve as well
//    givenCurve.append(x,y);
    givenCurve.append(vgl_point_2d<double>(x,y));
  }

  //can also use vsol_polygon_2d in same fashion...
  givenContour = new vsol_polygon_2d(points);

  //the Curve does this in its load (altho it seems UNNECESSARY as computeProperties is done
  //twice during the preProcessing that puzzleSolving makes each curve perform
  //this is how the old code was designed however, and will remain for now (at least)
  givenCurve.compute_properties();

  fp.close();


  return 0;
}

void bfrag2D::calcCenter()
{
  _maxX = static_cast<int>(vcl_floor(topContourCurve.max_x()+0.5));
  _minX = static_cast<int>(vcl_floor(topContourCurve.min_x()+0.5));
  _maxY = static_cast<int>(vcl_floor(topContourCurve.max_y()+0.5));
  _minY = static_cast<int>(vcl_floor(topContourCurve.min_y()+0.5));

  _centerX = static_cast<int>(vcl_floor(((_maxX - _minX) * 0.5) + _minX+0.5));
  _centerY = static_cast<int>(vcl_floor(((_maxY - _minY) * 0.5) + _minY+0.5));
}


