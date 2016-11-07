// FILEIO.CPP

#include <extrautils/msgout.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cmath.h>

//#include <stdio.h>

#include "ishock.h"

//##########################################################
// Read .BND
//##########################################################

double  dSaveRatio=1.0;
double  CON_MAG=1;

#define XX_SHIFT 0
#define YY_SHIFT 0

void Boundary::LoadFile (const char* filename)
{
  //1)If no filename, return.
  if (!filename || !strcmp(filename,"")) {
    vcl_cout<< " No Filename given"<<vcl_endl;
    return;
  }

  //2)Check file type and read it in.
  char *file_extension;
  file_extension=(char *)filename;
  while(file_extension[0]!='\0') file_extension++;
  file_extension -=4;

  if (!strncmp(file_extension,".bnd",4))
    LoadBND (filename);
  else if (!strncmp(file_extension,".txt",4))
    LoadTXT (filename);
  else if (!strncmp(file_extension,".con",4)) {
    if (_bLoadConAsLine)
      LoadCON_Line (filename);
    else
      LoadCON_Point (filename);
  }
  else if (!strncmp(file_extension,".edg",4))
    LoadEDG (filename);
  else if (!strncmp(file_extension,".cem",4))
    LoadCEM (filename);
}

void Boundary::LoadBND (const char* filename)
{
   INPUT_COORD_TYPE x, y, sx, sy, ex, ey;
  INPUT_TAN_TYPE dir;
   double r;
   ARC_NUD nud;
  ARC_NUS nus;
  char buffer[MAX_LEN];
  int version =1;  //default version i.e., old .txt files

  //1)If file open fails, return.
  vcl_ifstream fp (filename, vcl_ios::in);
  if (!fp){
    vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
    return;
  }

  //2)Read in each line until EOF.
  while (fp.getline(buffer,MAX_LEN)) {
    //x=INVALID_COORD; y=INVALID_COORD; sx=INVALID_COORD; sy=INVALID_COORD; 
    //ex=INVALID_COORD; ey=INVALID_COORD; r=INVALID_COORD; cx=INVALID_COORD; cy=INVALID_COORD;

    if (!strncmp(buffer, "Boundary File v2.0", sizeof("Boundary File v2.0")-1))
      version = 2;
    else if (!strncmp(buffer, "Boundary File v3.0", sizeof("Boundary File v2.0")-1))
      version = 3;
    
    //BPOINT with tangent
    else if (!strncmp(buffer, "point-tangent-at", sizeof("point-tangent-at")-1)) {
      sscanf(buffer,"point-tangent-at: (%f %f) (%f)",&(x), &(y), &(dir));
      if (x==INVALID_COORD || y==INVALID_COORD)
        sscanf(buffer,"point-tangent-at: (%f, %f) (%f)",&(x), &(y), &(dir));

        addGUIPoint (XX_SHIFT+x, YY_SHIFT+y, dir);
        continue;
      }

    //BPOINT
    else if (!strncmp(buffer, "point-at", sizeof("point-at")-1)) {
      sscanf(buffer,"point-at: (%f %f)",&(x), &(y));
      if (x==INVALID_COORD || y==INVALID_COORD)
        sscanf(buffer,"point-at: (%f, %f)",&(x), &(y));

      addGUIPoint (XX_SHIFT+x, YY_SHIFT+y);
      continue;
    }

    //BLINE
    else if (!strncmp(buffer, "line-from-to", sizeof("line-from-to")-1)) {
      sscanf(buffer,"line-from-to: (%f %f) (%f %f)", &(sx), &(sy), &(ex), &(ey));
      if (sx==INVALID_COORD || sy==INVALID_COORD || ex==INVALID_COORD || ey==INVALID_COORD)
        sscanf(buffer,"line-from-to: (%f, %f)-(%f, %f)", &(sx), &(sy), &(ex), &(ey));

      addGUILine (XX_SHIFT+sx, YY_SHIFT+sy, XX_SHIFT+ex, YY_SHIFT+ey);
      _bIsPreprocesvcl_singNeeded = true;
      continue;
    }

    //BARC
    else if (!strncmp(buffer, "arc-from-to", sizeof("arc-from-to")-1)) {
      nus = ARC_NUS_SMALL;
      nud = ARC_NUD_CCW;
      if (version ==1)
        sscanf(buffer,"arc-from-to: (%f %f) (%f %f) (%lf)",
              &(sx), &(sy), &(ex), &(ey), &r);
      else
        sscanf(buffer,"arc-from-to: (%f %f) (%f %f) (%lf) (%d %d)", 
              &sx, &sy, &ex, &ey, &r, &nud, &nus);

      Point c = getCenterOfArc (sx, sy, ex, ey, r, nud, nus);

      Point s (sx, sy);
      Point e (ex, ey);
      if (nud==ARC_NUD_CW) {
        Point temp = e;
        e = s;
        s = temp;
        nud = ARC_NUD_CCW;
      }
    
      //Break the big arc into 2
      if (nus==ARC_NUS_LARGE) { //Large arc...
    
        VECTOR_TYPE svector = _vPointPoint (c, s);
        VECTOR_TYPE evector = _vPointPoint (c, e);
        ANGLE_TYPE angle;
        if (nud==ARC_NUD_CCW)
          angle = CCW (svector, evector);
        else
          angle = CCW (evector, svector);
        VECTOR_TYPE mvector;
        if (nud==ARC_NUD_CCW)
          mvector = svector + angle/2;
        else
          mvector = evector + angle/2;
        Point m = _vectorPoint (c, mvector, r);

        BArc* first_arc = (BArc*)addGUIArc (XX_SHIFT+s.x, YY_SHIFT+s.y, XX_SHIFT+m.x, YY_SHIFT+m.y, XX_SHIFT+c.x, YY_SHIFT+c.y, r, nud, ARC_NUS_SMALL);
    
        BPoint* end_bpt = (nud==ARC_NUD_CCW) ? first_arc->e_pt() : first_arc->s_pt();
        addGUIArcBetween (first_arc->e_pt(), Point (e.x+XX_SHIFT, e.y+YY_SHIFT), Point (c.x+XX_SHIFT, c.y+YY_SHIFT), r, nud, ARC_NUS_SMALL);
      }
      else { //Small arc...
        addGUIArc (XX_SHIFT+s.x, YY_SHIFT+s.y, XX_SHIFT+e.x, YY_SHIFT+e.y, XX_SHIFT+c.x, YY_SHIFT+c.y, r, nud, nus);
      }
      _bIsPreprocesvcl_singNeeded = true;
      continue;
    }

  }//end while
   fp.close();
}

void Boundary::LoadTXT (const char* filename)
{
   INPUT_COORD_TYPE x, y, sx, sy, ex, ey;
   double r;
   ARC_NUD nud;
  ARC_NUS nus;
  char buffer[MAX_LEN];

  //1)If file open fails, return.
  vcl_ifstream fp(filename,vcl_ios::in);
  if (!fp){
    vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
    return;
  }

  //2)Read in each line until EOF.
  while (fp.getline(buffer,MAX_LEN)) {
    //x=INVALID_COORD; y=INVALID_COORD; sx=INVALID_COORD; sy=INVALID_COORD; 
    //ex=INVALID_COORD; ey=INVALID_COORD; r=INVALID_COORD; cx=INVALID_COORD; cy=INVALID_COORD;

    //BPOINT
    if (!strncmp(buffer, "point-at", sizeof("point-at")-1)) {
      sscanf(buffer,"point-at: (%f %f)",&(x), &(y));
      if (x==INVALID_COORD || y==INVALID_COORD)
        sscanf(buffer,"point-at: (%f, %f)",&(x), &(y));

      addGUIPoint (XX_SHIFT+x, YY_SHIFT+y);
      continue;
    }

    //BLINE
    else if (!strncmp(buffer, "line-from-to", sizeof("line-from-to")-1)) {
      sscanf(buffer,"line-from-to: (%f %f) (%f %f)", &(sx), &(sy), &(ex), &(ey));
      if (sx==INVALID_COORD || sy==INVALID_COORD || ex==INVALID_COORD || ey==INVALID_COORD)
        sscanf(buffer,"line-from-to: (%f, %f)-(%f, %f)", &(sx), &(sy), &(ex), &(ey));

      addGUILine (XX_SHIFT+sx, YY_SHIFT+sy, XX_SHIFT+ex, YY_SHIFT+ey);
      _bIsPreprocesvcl_singNeeded = true;
      continue;
    }

    //BARC
    else if (!strncmp(buffer, "arc-from-to", sizeof("arc-from-to")-1)) {
      nus = ARC_NUS_SMALL;
      nud = ARC_NUD_CCW;
      sscanf (buffer,"arc-from-to: (%f %f) (%f %f) (%lf)",
            &(sx), &(sy), &(ex), &(ey), &r);

      Point c = getCenterOfArc (sx, sy, ex, ey, r, nud, nus);

      Point s (sx, sy);
      Point e (ex, ey);
      if (nud==ARC_NUD_CW) {
        Point temp = e;
        e = s;
        s = temp;
        nud = ARC_NUD_CCW;
      }
    
      //Break the big arc into 2
      if (nus==ARC_NUS_LARGE) { //Large arc...
    
        VECTOR_TYPE svector = _vPointPoint (c, s);
        VECTOR_TYPE evector = _vPointPoint (c, e);
        ANGLE_TYPE angle;
        if (nud==ARC_NUD_CCW)
          angle = CCW (svector, evector);
        else
          angle = CCW (evector, svector);
        VECTOR_TYPE mvector;
        if (nud==ARC_NUD_CCW)
          mvector = svector + angle/2;
        else
          mvector = evector + angle/2;
        Point m = _vectorPoint (c, mvector, r);

        BArc* first_arc = (BArc*)addGUIArc (XX_SHIFT+s.x, YY_SHIFT+s.y, XX_SHIFT+m.x, YY_SHIFT+m.y, XX_SHIFT+c.x, YY_SHIFT+c.y, r, nud, ARC_NUS_SMALL);
    
        BPoint* end_bpt = (nud==ARC_NUD_CCW) ? first_arc->e_pt() : first_arc->s_pt();
        addGUIArcBetween (first_arc->e_pt(), Point (e.x+XX_SHIFT, e.y+YY_SHIFT), Point (c.x+XX_SHIFT, c.y+YY_SHIFT), r, nud, ARC_NUS_SMALL);
      }
      else { //Small arc...
        addGUIArc (XX_SHIFT+s.x, YY_SHIFT+s.y, XX_SHIFT+e.x, YY_SHIFT+e.y, XX_SHIFT+c.x, YY_SHIFT+c.y, r, nud, nus);
      }
      _bIsPreprocesvcl_singNeeded = true;
      continue;
    }
  }//end while
   fp.close();
}

//Read .CON contour file as a point set.
//This function can read both open and close .CON file.
void Boundary::LoadCON_Point (const char* filename)
{  
  INPUT_COORD_TYPE x, y;
  char buffer[MAX_LEN];
  Point firstPt, prevPt, Pt;
   int nPoints;

  //1)If file open fails, return.
  vcl_ifstream fp(filename,vcl_ios::in);
  if (!fp){
    vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
    return;
  }

  //2)Read in file header.
   fp.getline (buffer,MAX_LEN); //CONTOUR
   fp.getline (buffer,MAX_LEN); //OPEN/CLOSE
   fp >> nPoints;
   vcl_cout << "Number of Points from Contour:" << nPoints << vcl_endl;
  
  //2-1)First point
  fp >> x >> y ; firstPt.x = x;  firstPt.y = y;
  addGUIPoint (x, y);
  prevPt = firstPt;

  //3)Read in 'nPoints' of lines of data.
   for (int i=0; i < nPoints-1; i++) {
    fp >> x >> y; Pt.x = x;  Pt.y = y;
 
    DIST_TYPE d = _distPointPoint (prevPt, Pt);

    if (d > CON_FILE_SAMPLE_POINT_DISTANCE) {
      addGUIPoint (x, y);
      prevPt = Pt;
    }
   }

  //3-2)For the last point...
  fp >> x >> y; Pt.x = x;  Pt.y = y;
  DIST_TYPE d = _distPointPoint (Pt, firstPt);
  if (d > CON_FILE_SAMPLE_POINT_DISTANCE)
    addGUIPoint (x, y);

  fp.close();
}

//Read .CON contour file as a line set.
//This function can read both open and close .CON file.
void Boundary::LoadCON_Line (const char* filename)
{  
  INPUT_COORD_TYPE x, y;
  char buffer[MAX_LEN];
  Point firstPt, prevPt, Pt;
  bool bIsCloseContour = false;
   int nPoints;

  //1)If file open fails, return.
  vcl_ifstream fp(filename,vcl_ios::in);
  if (!fp){
    vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
    return;
  }

  //2)Read in file header.
   fp.getline (buffer,MAX_LEN); //CONTOUR
   fp.getline (buffer,MAX_LEN); //OPEN/CLOSE
  if (!strncmp(buffer, "CLOSE", sizeof("CLOSE")-1))
    bIsCloseContour = true; //set the flag of Open/Close contour.
   fp >> nPoints;
   vcl_cout << "Number of Points from Contour:" << nPoints << vcl_endl;
  
  //2-1)First point
  fp >> x >> y; firstPt.x = x; firstPt.y = y;
  BPoint* firstBPoint = addNonGUIPoint (firstPt.x, firstPt.y);
  prevPt = firstPt;
  BPoint* prevBPoint = firstBPoint;

  //3)Read in 'nPoints' of lines of data.
  BPoint* nextBPoint;
   for (int i=0; i < nPoints-1; i++) {
    fp >> x >> y; Pt.x = x;  Pt.y = y;
    DIST_TYPE d = _distPointPoint (prevPt, Pt);
    if (d > CON_FILE_SAMPLE_POINT_DISTANCE) {
      addGUILineBetween (prevBPoint, Pt, &nextBPoint);

      //Note that the new nextBPoint might be NULL, due to tiny sample point that's less than epsilon.
      if (nextBPoint!=NULL) {
        prevPt = Pt;
        prevBPoint = nextBPoint;
      }
    }
   }

  //3-2)For the last point...
  fp >> x >> y; Pt.x = x;  Pt.y = y;
  //this is a little more complicated
  DIST_TYPE dp = _distPointPoint (prevPt, Pt);
  DIST_TYPE d = _distPointPoint (Pt, firstPt);
  //if the last point is at a reasonable distance 
  //from the previous point as well as the first point
  //add it in
  if (dp > CON_FILE_SAMPLE_POINT_DISTANCE &&
      d > CON_FILE_SAMPLE_POINT_DISTANCE)
  {
    addGUILineBetween (prevBPoint, Pt, &nextBPoint);
    addGUILineBetween (nextBPoint, firstBPoint);
  }
  else //otherwise throw away the last point
  {
    addGUILineBetween (prevBPoint, firstBPoint);
  }

  _bIsPreprocesvcl_singNeeded = true;
  fp.close();
}

//For .EDG File (Result of edge detection (Defined By Ali)
void Boundary::LoadEDG (const char* filename)
{  
  INPUT_COORD_TYPE x, y;
  char buffer[MAX_LEN];
  int numGeometry;
  int ix, iy;
  double idir, iconf, dir, conf;

  //1)If file open fails, return.
  vcl_ifstream fp(filename,vcl_ios::in);
  if (!fp){
    vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
    return;
  }

  //2)Read in each line
  while (fp.getline(buffer,MAX_LEN)) {
    //ignore comment lines and empty lines
    if (strlen(buffer)<2 || buffer[0]=='#')
      continue;

    //read the line with the edge count info
    if (!strncmp(buffer, " EDGE_COUNT=", sizeof(" EDGE_COUNT=")-1)){
      sscanf(buffer," EDGE_COUNT=%d",&(numGeometry));
      continue;
    }

    //the rest should have data
    sscanf(buffer," [%d, %d]   %lf %lf  [%f, %f]  %lf %lf",&(ix), &(iy), 
      &(idir), &(iconf), &(x), &(y), &(dir), &(conf));

    BPoint* bp;
    bp = addGUIPoint (ix, iy, (float)angle02Pi(idir), iconf);
    bp->_bSomething = true;
  }

  fp.close();
}

//For .CEM File (Contour Edge Map File)
void Boundary::LoadCEM (const char* filename)
{
  INPUT_COORD_TYPE x, y;
  char buffer[MAX_LEN];
  int numContours, numTotalEdges, numEdges;
  int ix, iy;
  double idir, iconf;
  double dir , conf;
  double lastX = ISHOCK_DIST_HUGE;
  double lastY = ISHOCK_DIST_HUGE;

  //1)If file open fails, return.
  vcl_ifstream fp(filename,vcl_ios::in);
  if (!fp){
    vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
    return;
  }

  //2)Read in each line
  while (fp.getline(buffer,MAX_LEN)) {

    //ignore comment lines and empty lines
    if (strlen(buffer)<2 || buffer[0]=='#')
      continue;

    //read the line with the contour count info
    if (!strncmp(buffer, "CONTOUR_COUNT=", sizeof("CONTOUR_COUNT=")-1)){
      sscanf(buffer,"CONTOUR_COUNT=%d",&(numContours));
      continue;
    }

    //read the line with the edge count info
    if (!strncmp(buffer, "TOTAL_EDGE_COUNT=", sizeof("TOTAL_EDGE_COUNT=")-1)){
      sscanf(buffer,"TOTAL_EDGE_COUNT=%d",&(numTotalEdges));
      continue;
    }

    //read the beginning of a contour block
    if (!strncmp(buffer, "[BEGIN CONTOUR]", sizeof("[BEGIN CONTOUR]")-1)){
      /*
      //insvcl_tantiate a new contour structure here
      curCon = new BContour(nextAvailableID());
      curCon->elms.clear();

      //add this contour to the contour list
      contourList.push_back(curCon);
      */
      fp.getline(buffer,MAX_LEN);
      sscanf(buffer,"EDGE_COUNT=%d",&(numEdges));

      BPoint* prevBPoint = NULL;

      for (int j=0; j< numEdges; j++){
        //the rest should have data that goes into the current contour
        fp.getline(buffer,MAX_LEN);
        sscanf(buffer," [%d, %d]\t%f\t%lf\t[%f, %f]\t%lf\t%lf",&(ix), &(iy), 
              &(idir), &(iconf), &(x), &(y), &(dir), &(conf));

        //add this edge to the current contour
        //sometimes there are duplicate points we need to weed these out
        if (x==lastX && y==lastY)
          continue;

        //VJ's current CEM is in degrees rather than radians so need to convert
        dir += 90;
        dir *= M_PI/180;

        lastX = x;
        lastY = y;

        BPoint* bp;
        bp = addGUIPoint (x, y, (float)angle02Pi(dir), conf);
        
        if (j>0)
          addGUILineBetween(prevBPoint, bp);

        prevBPoint = bp;
        
        /*
        //mark end points
        if (j==0 || j==numEdges-1)
          bp->_bSomething = false;
        else
          bp->_bSomething = true;

        curCon->elms.push_back(bp);
        */
      }

      fp.getline(buffer,MAX_LEN);
      if (strncmp(buffer, "[END CONTOUR]", sizeof(" [END CONTOUR]")-1))
        assert(false);
    }

  }
  fp.close();
}

//##########################################################
// Write .BND
//##########################################################

void Boundary::SaveBND (const char* filename)
{
   if (!filename){
      vcl_cout<< " No File name given"<<vcl_endl;
      return;
   }

   vcl_ofstream fp(filename);

   if (!fp){
      vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
      return;
   }

   // traverse list to compute image size
   int height = 0;
   int width = 0;
   
   BElement* curBElm;
   BElmListIterator i =BElmList.begin();
  for(; i != BElmList.end(); i++){
    curBElm = (BElement*)(i->second); //dynamic_cast<BElement*>(i->second);
    if (!curBElm->isGUIElm()) continue;

      switch (curBElm->type()) {
         case BPOINT:
              if (((BPoint *)curBElm)->pt().x > width) 
                width = (int) vcl_ceil(((BPoint *)curBElm)->pt().x);
              if (((BPoint *)curBElm)->pt().y > height) 
                height = (int) vcl_ceil(((BPoint *)curBElm)->pt().y);
              break;
         case BLINE:
              if (((BLine *)curBElm)->start().x > width) 
                width = (int) vcl_ceil(((BLine *)curBElm)->start().x);
              if (((BLine *)curBElm)->start().y > height) 
                height = (int) vcl_ceil(((BLine *)curBElm)->start().y);

              if (((BLine *)curBElm)->end().x > width) 
                width = (int) vcl_ceil(((BLine *)curBElm)->end().x);
              if (((BLine *)curBElm)->end().y > height) 
                height = (int) vcl_ceil(((BLine *)curBElm)->end().y);
              break;
         case BARC:        // NOT STRICTLY CORRECT
              if (((BArc *)curBElm)->start().x > width) 
                width = (int) vcl_ceil(((BArc *)curBElm)->start().x);
              if (((BArc *)curBElm)->start().y > height) 
                height = (int) vcl_ceil(((BArc *)curBElm)->start().y);

              if (((BArc *)curBElm)->end().x > width) 
                width = (int) vcl_ceil(((BArc *)curBElm)->end().x);
              if (((BArc *)curBElm)->end().y > height) 
                height = (int) vcl_ceil(((BArc *)curBElm)->end().y);
              break;
         default: break;
      }
   }

   // output header information
   fp <<"Boundary File v3.0"<<vcl_endl;
   fp <<"width: "<< width/dSaveRatio <<vcl_endl;
   fp <<"height: "<< height/dSaveRatio <<vcl_endl;
   fp <<"number-of-elements: "<<nBElement()<<vcl_endl;

   //traverse the list again and output the data
  for(i=BElmList.begin();i!=BElmList.end(); i++){
    curBElm = (BElement*)(i->second); //curBElm = dynamic_cast<BElement*>(i->second);

      if (!curBElm->isGUIElm()) continue;

    //BoundaryLimitHack
    if (_BoundaryLimit == BIG_RECTANGLE || _BoundaryLimit == BIG_CIRCLE)
      if (curBElm->id() <=8) continue;

      fp.precision (15);

      switch (curBElm->type()) {
         case BPOINT:
        if (((BPoint *)curBElm)->tangent() == TANGENT_UNDEFINED){
          fp <<"point-at: ("<<((BPoint *)curBElm)->pt().x/dSaveRatio <<" "<<
          ((BPoint *)curBElm)->pt().y/dSaveRatio <<")"<<vcl_endl;
        }
        else {
          fp <<"point-tangent-at: ("<<((BPoint *)curBElm)->pt().x/dSaveRatio <<" "<<
          ((BPoint *)curBElm)->pt().y/dSaveRatio <<") ("<< ((BPoint *)curBElm)->tangent() <<")" << vcl_endl;
        }
            break;
         case BLINE:
              fp <<"line-from-to: ("<<((BLine *)curBElm)->start().x/dSaveRatio <<" "<<
              ((BLine *)curBElm)->start().y/dSaveRatio <<") ("<<
              ((BLine *)curBElm)->end().x/dSaveRatio <<" "<<
              ((BLine *)curBElm)->end().y/dSaveRatio <<")"<<vcl_endl;
              break;
         case BARC:
              fp <<"arc-from-to: ("<<((BArc *)curBElm)->start().x/dSaveRatio <<" "<<
              ((BArc *)curBElm)->start().y/dSaveRatio <<") ("<<
              ((BArc *)curBElm)->end().x/dSaveRatio <<" "<<
              ((BArc *)curBElm)->end().y/dSaveRatio <<") ("<<
              ((BArc *)curBElm)->R()/dSaveRatio <<") ("<<
              ((BArc *)curBElm)->nud()<<" "<<
              ARC_NUS_SMALL <<")"<<vcl_endl;
              break;
         default: break;
      }
   }
   //close file
   fp.close();
}

//##########################################################
// Write .CON
//##########################################################

void Boundary::SaveCON (const char* filename)
{
   if (!filename){
      vcl_cout<< " No File name given"<<vcl_endl;
      return;
   }

   vcl_ofstream fp(filename);

   if (!fp){
      vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
      return;
   }

   // traverse list to compute image size
   int height = 0;
   int width = 0;
   
   BElement* curBElm;
   BElmListIterator i =BElmList.begin();
  for(; i != BElmList.end(); i++){
    curBElm = (BElement*)(i->second); //curBElm = dynamic_cast<BElement*>(i->second);
    if (!curBElm->isGUIElm()) continue;

      switch (curBElm->type()) {
         case BPOINT:
              if (((BPoint *)curBElm)->pt().x > width) 
                width = (int) vcl_ceil(((BPoint *)curBElm)->pt().x);
              if (((BPoint *)curBElm)->pt().y > height) 
                height = (int) vcl_ceil(((BPoint *)curBElm)->pt().y);
              break;
         case BLINE:
              if (((BLine *)curBElm)->start().x > width) 
                width = (int) vcl_ceil(((BLine *)curBElm)->start().x);
              if (((BLine *)curBElm)->start().y > height) 
                height = (int) vcl_ceil(((BLine *)curBElm)->start().y);

              if (((BLine *)curBElm)->end().x > width) 
                width = (int) vcl_ceil(((BLine *)curBElm)->end().x);
              if (((BLine *)curBElm)->end().y > height) 
                height = (int) vcl_ceil(((BLine *)curBElm)->end().y);
              break;
         case BARC:        // NOT STRICTLY CORRECT
              if (((BArc *)curBElm)->start().x > width) 
                width = (int) vcl_ceil(((BArc *)curBElm)->start().x);
              if (((BArc *)curBElm)->start().y > height) 
                height = (int) vcl_ceil(((BArc *)curBElm)->start().y);

              if (((BArc *)curBElm)->end().x > width) 
                width = (int) vcl_ceil(((BArc *)curBElm)->end().x);
              if (((BArc *)curBElm)->end().y > height) 
                height = (int) vcl_ceil(((BArc *)curBElm)->end().y);
              break;
         default: break;
      }
   }

   // output header information
   fp <<"CONTOUR"<<vcl_endl;
   fp <<"OPEN" <<vcl_endl;
   fp << nBElement() <<vcl_endl;

   //traverse the list again and output the data
  for(i=BElmList.begin();i!=BElmList.end(); i++){
    curBElm = (BElement*)(i->second); //curBElm = dynamic_cast<BElement*>(i->second);

      if (!curBElm->isGUIElm()) continue;

    //BoundaryLimitHack
    if (_BoundaryLimit == BIG_RECTANGLE || _BoundaryLimit == BIG_CIRCLE)
      if (curBElm->id() <=8) continue;

      fp.precision (15);

      switch (curBElm->type()) {
      case BPOINT:
      fp << ((BPoint *)curBElm)->pt().x/dSaveRatio <<" "<<
          ((BPoint *)curBElm)->pt().y/dSaveRatio <<vcl_endl;
      break;
      case BLINE:
      fp << ((BLine *)curBElm)->start().x/dSaveRatio <<" "<<
          ((BLine *)curBElm)->start().y/dSaveRatio <<vcl_endl;
      break;
      case BARC:
      /*fp <<"arc-from-to: ("<<((BArc *)curBElm)->start().x/dSaveRatio <<" "<<
              ((BArc *)curBElm)->start().y/dSaveRatio <<") ("<<
              ((BArc *)curBElm)->end().x/dSaveRatio <<" "<<
              ((BArc *)curBElm)->end().y/dSaveRatio <<") ("<<
              ((BArc *)curBElm)->R()/dSaveRatio <<") ("<<
              ((BArc *)curBElm)->nud()<<" "<<
              ((BArc *)curBElm)->nus()<<")"<<vcl_endl;*/
      break;
      default: break;
      }
   }
   //close file
   fp.close();
}

//##########################################################
// Write .CEM
//##########################################################

void Boundary::SaveCEM (const char* filename)
{
  if (!filename){
      vcl_cout<< " No File name given"<<vcl_endl;
      return;
   }

   vcl_ofstream fp(filename);

   if (!fp){
      vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
      return;
   }

  // output header information
   fp <<"# CONTOUR_EDGE_MAP : Logical-Linear + Shock_Grouping"<<vcl_endl;
   fp <<"# .cem files"<<vcl_endl;
   fp <<"#"<<vcl_endl;
   fp <<"# Format :"<<vcl_endl;
   fp <<"# Each contour block will consist of the following"<<vcl_endl;
   fp <<"# [BEGIN CONTOUR]"<<vcl_endl;
   fp <<"# EDGE_COUNT=num_of_edges"<<vcl_endl;
   fp <<"# [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf "<<vcl_endl;
   fp <<"# ..."<<vcl_endl;
   fp <<"# ..."<<vcl_endl;
   fp <<"# [END CONTOUR]"<<vcl_endl;

  fp<<vcl_endl;

   fp <<" CONTOUR_COUNT="<< contourList.size() << vcl_endl;
   fp <<" TOTAL_EDGE_COUNT="<<vcl_endl;

  BContourListIterator curConPtr = contourList.begin();
  for (; curConPtr != contourList.end(); curConPtr++){
    BContour* curCon = (*curConPtr);

    fp <<" [BEGIN CONTOUR]"<<vcl_endl;
    fp <<" EDGE_COUNT="<< curCon->elms.size() <<vcl_endl;

    //this is assuming that the contours are all made up of points
    BElementListIterator belmPtr = curCon->elms.begin();
    for (; belmPtr != curCon->elms.end(); belmPtr++){
      BPoint* curPt = (BPoint*)(*belmPtr);
      
      //output as subpixel contours
      //fp <<" [0, 0]   0.0 0.0  ["<< curPt->pt().x << ", " << curPt->pt().y <<"]  "<<
      //output as pixel contours
      fp <<" ["<< round(curPt->pt().x) << ", " << round(curPt->pt().y) <<"]  "<<
        curPt->tangent() << " " << curPt->conf() << " [0, 0]   0.0 0.0"  <<vcl_endl;
    }
    
    fp <<" [END CONTOUR]"<<vcl_endl<<vcl_endl;
  }

  //close file
   fp.close();

}
