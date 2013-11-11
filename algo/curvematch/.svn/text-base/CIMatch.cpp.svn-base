#include "CIMatch.h"
#include <vcl_cmath.h>
//#include "msgout.h"


//###############################################################
//    bsol_intrinsic_curve_2d-Image-Matching TANCODE DEFINITIONS.
//###############################################################

//DELTAXY_TO_TANCODE_TABLE
//Gives (deltax, deltay), return the corresponding TanCode
//in DELTAXY_TO_TANCODE_TABLE[deltay+3][deltax+3]
// N/A is marked as -1
short int DELTAXY_TO_TANCODE_TABLE[][7] = {
  {-1, 27, 28, -1, 29, 30, -1},
  {26, -1, 14, -1, 15, -1, 31},
  {25, 13,  6,  7,  8, 16, 32},
  {-1, -1,  5,  0,  1, -1, -1},
  {24, 12,  4,  3,  2,  9, 17},
  {23, -1, 11, -1, 10, -1, 18},
  {-1, 22, 21, -1, 20, 19, -1},
};

//Ming: can improve the speed by setting up a 7x7 table
inline short int DeltaXYStepToTanCode (short int deltax, short int deltay, short int jstep)
{
  int tancode = DELTAXY_TO_TANCODE_TABLE[deltay+3][deltax+3];
  assert (tancode !=-1);

  switch (jstep) {
  case 1: break;
  case 2: tancode += 50; break;
  case 3: tancode += 100; break;
  default: assert (0);
  }

  return tancode;
}

inline short int PrevXYStepToTanCode (short int prevx, short int prevy, short int jstep)
{
  short int tanCode = DeltaXYStepToTanCode (-prevx, -prevy, jstep);
  return tanCode;
  //int deltax = -prevx;
  //int deltay = -prevy;
  //int tancode = DELTAXY_TO_TANCODE_TABLE[deltay+3][deltax+3];
  //assert (tancode !=-1);
  //switch (jstep) {
  //case 1: break;
  //case 2: tancode += 50; break;
  //case 3: tancode += 100; break;
  //default: assert (0);
  //}
  //return tancode;
}

inline void TanCodeToDeltaXYStep (short int tancode, short int& deltax, short int& deltay, short int& jstep)
{
  if (tancode>100) {
    jstep = 3;
    tancode -= 100;
  }
  else if (tancode>50) {
    jstep = 2;
    tancode -= 50;
  }
  else {
    jstep = 1;
  }

  switch (tancode) {
  //3x3
  case 0: deltax=0; deltay=0; break;
  case 1: deltax=1; deltay=0; break;
  case 2: deltax=1; deltay=1; break;
  case 3: deltax=0; deltay=1; break;
  case 4: deltax=-1; deltay=1; break;
  case 5: deltax=-1; deltay=0; break;
  case 6: deltax=-1; deltay=-1; break;
  case 7: deltax=0; deltay=-1; break;
  case 8: deltax=1; deltay=-1; break;
  //5x5
  case 9: deltax=2; deltay=1; break;
  case 10: deltax=1; deltay=2; break;
  case 11: deltax=-1; deltay=2; break;
  case 12: deltax=-2; deltay=1; break;
  case 13: deltax=-2; deltay=-1; break;
  case 14: deltax=-1; deltay=-2; break;
  case 15: deltax=1; deltay=-2; break;
  case 16: deltax=2; deltay=-1; break;
  //7x7
  case 17: deltax=3; deltay=1; break;
  case 18: deltax=3; deltay=2; break;
  case 19: deltax=2; deltay=3; break;
  case 20: deltax=1; deltay=3; break;
  case 21: deltax=-1; deltay=3; break;
  case 22: deltax=-2; deltay=3; break;
  case 23: deltax=-3; deltay=2; break;
  case 24: deltax=-3; deltay=1; break;
  case 25: deltax=-3; deltay=-1; break;
  case 26: deltax=-3; deltay=-2; break;
  case 27: deltax=-2; deltay=-3; break;
  case 28: deltax=-1; deltay=-3; break;
  case 29: deltax=1; deltay=-3; break;
  case 30: deltax=2; deltay=-3; break;
  case 31: deltax=3; deltay=-2; break;
  case 32: deltax=3; deltay=-1; break;
  default: assert (0);
  }
}

//Just the inverse of the TanCodeToDeltaXY() result
inline void TanCodeToPrevXYStep (short int tancode, short int& prevx, short int& prevy, short int& jstep)
{
  TanCodeToDeltaXYStep (tancode, prevx, prevy, jstep);
  prevx = -prevx;
  prevy = -prevy;
}

//This function is wired.
inline short int DeltaXYToStep (short int deltax, short int deltay)
{
  if (deltax==0 && deltay==0)
    return 0;

  ////deltax = vcl_abs(deltax);
  ////deltay = vcl_abs(deltay);
  if (deltax>2 || deltay>2)
    return 3;
  if (deltax>1 || deltay>1)
    return 2;
  if (deltax>0 || deltay>0)
    return 1;
  else
    assert (0);
  return 0;
}

//Instead of the 2D convolving way of implementation,
//using a 1D loop into each TanCodeTable...
//[TanCode, deltax, deltay]
#define  TANCODETABLE3X3_SIZE  9
short int TANCODETABLE3X3[][3] = {
  { 0,  0,  0},
  { 1,  1,  0},
  { 2,  1,  1},
  { 3,  0,  1},
  { 4, -1,  1},
  { 5, -1,  0},
  { 6, -1, -1},
  { 7,  0, -1},
  { 8,  1, -1},
};
//[TanCode, deltax, deltay]
#define  TANCODETABLE5X5_SIZE  8
short int TANCODETABLE5X5[][3] = {
  { 9,  2,  1},
  {10,  1,  2},
  {11, -1,  2},
  {12, -2,  1},
  {13, -2, -1},
  {14, -1, -2},
  {15,  1, -2},
  {16,  2, -1},
};
//[TanCode, deltax, deltay]
#define  TANCODETABLE7X7_SIZE  16
short int TANCODETABLE7X7[][3] = {
  {17,  3,  1},
  {18,  3,  2},
  {19,  2,  3},
  {20,  1,  3},
  {21, -1,  3},
  {22, -2,  3},
  {23, -3,  2},
  {24, -3,  1},
  {25, -3, -1},
  {26, -3, -2},
  {27, -2, -3},
  {28, -1, -3},
  {29,  1, -3},
  {30,  2, -3},
  {31,  3, -2},
  {32,  3, -1},
};

//###############################################################

CurveImageMatch::CurveImageMatch ()
{
  //_image = NULL;
  DPTable = NULL;
  DPPrevTangent = NULL;
  _alpha = 1;
  _beta = 0;

  //default value
  _curveFileName = "";
  _imageFileName = "";
  _startPointX = -1;
  _startPointY = -1;
  setSearchWindow (-1, -1, -1, -1);

  nmsbuf = NULL;
  tanbuf = NULL;

  _TemplateSize = BOGUS_CIM_TEMPLATE_SIZE;
  _TemplateDepth = -1;
  _nBestMatch = 1; //-1

  //output
  _BestMatchingCost = NULL;
  _outputCurve = NULL;
  _fmap = NULL;
}

void CurveImageMatch::Match ()
{
  int j, y, x, t;

  //1)Initialize the D.P. tables... dimension size(CurveLength)*size(_imageWidth)*size(_imageHeight)
  //  Note that we are searching in the window defined.
  if (DPTable)
    freeDPMemory ();
  allocateDPMemory ();

  //debug
  double sx = _inputCurve->x(0);
  double sy = _inputCurve->y(0);
  double ex = _inputCurve->x(_inputCurveLength-1);
  double ey = _inputCurve->y(_inputCurveLength-1);

  double endPointX = _startPointX-_inputCurve->x(0) + _inputCurve->x(_inputCurveLength-1);
  double endPointY = _startPointY-_inputCurve->y(0) + _inputCurve->y(_inputCurveLength-1);

  //1-2)Compute the canny magnitude map and dir map...
  computeCannyMagnitudeDir ();

  //2)Initialize the 3D DPTable.
  for (j=0; j<_inputCurveLength; j++)
    for (y=0; y<_SearchWindowHeight; y++)
      for (x=0; x<_SearchWindowWidth; x++) {
        DPTable[j][y][x] = CIM_COST_INFINITY;
        DPPrevTangent[j][y][x] = CIM_TANCODE_NA;
      }
  //2-2)Step 0, cost of the startingPoint is zero.
  //DPTable[0][_startPointY][_startPointX] = 0;
  //DPPrevTangent[0][_startPointY][_startPointX] = CIM_TANCODE_0;


  //3)Step 0, cost of the VirtualStartingTemplate is 0, all the others are infinity!
  short int deltax, deltay, tancode;
  if (_VirtualTemplateSize==CIM_TEMPLATE_1) {
    DPTable[0][_startPointY][_startPointX] = 0;
    DPPrevTangent[0][_startPointY][_startPointX] = 0;
  }
  else {
    if (_VirtualTemplateSize>=CIM_TEMPLATE_3X3)
      for (t=0; t<TANCODETABLE3X3_SIZE; t++) {
        tancode = TANCODETABLE3X3[t][0];
        deltax = TANCODETABLE3X3[t][1];
        deltay = TANCODETABLE3X3[t][2];
        DPTable[0][_startPointY+deltay][_startPointX+deltax] = 0;
        DPPrevTangent[0][_startPointY+deltay][_startPointX+deltax] = tancode;
      }
    if (_VirtualTemplateSize>=CIM_TEMPLATE_5X5)
      for (t=0; t<TANCODETABLE5X5_SIZE; t++) {
        tancode = TANCODETABLE5X5[t][0];
        deltax = TANCODETABLE5X5[t][1];
        deltay = TANCODETABLE5X5[t][2];
        DPTable[0][_startPointY+deltay][_startPointX+deltax] = 0;
        DPPrevTangent[0][_startPointY+deltay][_startPointX+deltax] = tancode;
      }
    if (_VirtualTemplateSize>=CIM_TEMPLATE_7X7)
      for (t=0; t<TANCODETABLE7X7_SIZE; t++) {
        tancode = TANCODETABLE7X7[t][0];
        deltax = TANCODETABLE7X7[t][1];
        deltay = TANCODETABLE7X7[t][2];
        DPTable[0][_startPointY+deltay][_startPointX+deltax] = 0;
        DPPrevTangent[0][_startPointY+deltay][_startPointX+deltax] = tancode;
      }
  }

  //4)Step 1-n: general DP steps: fill each (x,y) in the DPTable[j]
  j=1;
  while (j<_inputCurveLength) {
    //For each step j
    for (y=0; y<_SearchWindowHeight; y++) //_imageHeight
      for (x=0; x<_SearchWindowWidth; x++) { //_imageWidth
        //for each pixel
        //look for Non-Infinity entry in the DPTable in step (j-1) in the 3x3 template
        //and compute the cost from it, sum the cost with the cost in the table.
        //(each new cost = prev_cost_from table + new added cost)
        //The final best cost for this position is the minimum of these costs.

        //Put all possible cost into newCostXY, minCostXY is the minimum of them.
        double minCostXY = CIM_COST_INFINITY, newCostXY;
        int    minCostTangent = CIM_TANCODE_NA;

        //5-1)For the point (x,y), i fixes, j moves one step.
        if (DPTable[j-1][y][x] == CIM_COST_INFINITY)
          newCostXY = CIM_COST_INFINITY;
        else {
          double newCurveCost0 = computeCost (
            j-1, //j1=j-1
            j, //j2=j
            x, y, //(x,y)to(x,y)
            x, y,
            DPPrevTangent[j-1][y][x]);
          newCostXY = DPTable[j-1][y][x] + newCurveCost0;
        }
        if (newCostXY < minCostXY) {
          minCostXY = newCostXY;
          minCostTangent = CIM_TANCODE_0;
        }

        //5-2)3x3, Instead of the 2D convolving way of implementation,
        //    using a 1D loop into each TanCodeTable...
        //    [TanCode, deltax, deltay]
        for (t=0; t<TANCODETABLE3X3_SIZE; t++) {
          tancode = TANCODETABLE3X3[t][0];
          deltax = TANCODETABLE3X3[t][1];
          deltay = TANCODETABLE3X3[t][2];

          //5-2-1)For the (x,y), skip becuase it's 5-1)i fix.
          if (tancode==0) //(deltax==0 && deltay==0)
            continue;

          int xx = x+deltax;
          int yy = y+deltay;
          int jstep = 1;

          //5-2-2)Other 8 Points in the template (1-8)...
          //Be careful about the boundary conditions.

          //long i, j;
          //if ((i >= 0) && (i < j))   ==   if ((unsigned long)i < j)

          if (xx>=0 && xx<_SearchWindowWidth && yy>=0 && yy<_SearchWindowHeight) {

            /*//5-2-3)  For the previous point, don't allow self-intersecting!
            if (deltax==prevx && deltay==prevy)
              continue;*/

            if (DPTable[j-1][yy][xx] == CIM_COST_INFINITY) {
              newCostXY = CIM_COST_INFINITY;
            }
            else { //Look for Non-Infinity entry...

              //5-2-4)j fixes, i moves in one of the 8 directions,
              double newCurveCost1 = computeCost (
                j-1, //j1=j-1
                j-1, //j2=j-1
                xx, yy, //(xx,yy)to(x,y)
                x, y,
                DPPrevTangent[j-1][yy][xx]);

              //5-2-5)Both i and j move one step (i has 8 directions).
              double newCurveCost2 = computeCost (
                j-1, //j1=j-1
                j, //j2=j
                xx, yy, //(xx,yy)to(x,y)
                x, y,
                DPPrevTangent[j-1][yy][xx]);

              double newCurveCost = min2 (newCurveCost1, newCurveCost2);
              newCostXY = DPTable[j-1][yy][xx] + newCurveCost;

              if (_TemplateDepth>1) {
                double newCurveCost3;
                if (j<2)
                  newCurveCost3 = CIM_COST_INFINITY;
                else {
                  //5-2-6)j moves 2, i moves in one of the 8 directions.
                  newCurveCost3 = computeCost (
                    j-2, //j1=j-2
                    j, //j2=j
                    xx, yy, //(xx,yy)to(x,y)
                    x, y,
                    DPPrevTangent[j-2][yy][xx]);
                  newCurveCost3 += DPTable[j-2][yy][xx];
                }
                if (newCurveCost3 < newCostXY) {
                  newCostXY = newCurveCost3;
                  jstep = 2;
                }
              }

              if (_TemplateDepth>2) {
                double newCurveCost4;
                if (j<3)
                  newCurveCost4 = CIM_COST_INFINITY;
                else {
                  //5-2-7)j moves 3, i moves in one of the 8 directions.
                  newCurveCost4 = computeCost (
                    j-3, //j1=j-3
                    j, //j2=j
                    xx, yy, //(xx,yy)to(x,y)
                    x, y,
                    DPPrevTangent[j-3][yy][xx]);
                  newCurveCost4 += DPTable[j-3][yy][xx];
                }
                if (newCurveCost4 < newCostXY) {
                  newCostXY = newCurveCost4;
                  jstep = 3;
                }
              }

            }
          }//end all points in the boundary conditions.
          if (newCostXY < minCostXY) { //3x3
            minCostXY = newCostXY;
            //determine minCostTangent by deltax and deltay. (inverse delta here!)
            minCostTangent = PrevXYStepToTanCode (deltax,deltay, jstep); //
          }
        }//end loop TanTable3x3

        //5-2-8)After all, for each (x,y) set minCostXY to DPTable[j][y][x]
        DPTable[j][y][x] = minCostXY;
        DPPrevTangent[j][y][x] = minCostTangent;

        //5-3)5x5, Instead of the 2D convolving way of implementation,
        //    using a 1D loop into each TanCodeTable...
        //    [TanCode, deltax, deltay]
        if (_TemplateSize < CIM_TEMPLATE_5X5)
          continue;
        for (t=0; t<TANCODETABLE5X5_SIZE; t++) {
          tancode = TANCODETABLE5X5[t][0];
          deltax = TANCODETABLE5X5[t][1];
          deltay = TANCODETABLE5X5[t][2];

          //5-3-1)For the (x,y), skip becuase it's 5-1)i fix.
          //if (tancode==0) //(deltax==0 && deltay==0)
          //  continue;

          int xx = x+deltax;
          int yy = y+deltay;
          int jstep = 1;

          //5-3-2)Other 8 Points in the template (9-16)...
          //Be careful about the boundary conditions.
          if (xx>=0 && xx<_SearchWindowWidth && yy>=0 && yy<_SearchWindowHeight) {

            /*//5-3-3)  For the previous point, don't allow self-intersecting!
            if (deltax==prevx && deltay==prevy)
              continue;*/

            if (DPTable[j-1][yy][xx] == CIM_COST_INFINITY) {
              newCostXY = CIM_COST_INFINITY;
            }
            else { //Look for Non-Infinity entry...

              //5-3-4)j fixes, i moves in one of the 8 directions,
              double newCurveCost1 = computeCost (
                j-1, //j1=j-1
                j-1, //j2=j-1
                xx, yy, //(xx,yy)to(x,y)
                x, y,
                DPPrevTangent[j-1][yy][xx]);

              //5-3-5)Both i and j move one step (i has 8 directions).
              double newCurveCost2 = computeCost (
                j-1, //j1=j-1
                j, //j2=j
                xx, yy, //(xx,yy)to(x,y)
                x, y,
                DPPrevTangent[j-1][yy][xx]);

              double newCurveCost = min2 (newCurveCost1, newCurveCost2);
              newCostXY = DPTable[j-1][yy][xx] + newCurveCost;

              if (_TemplateDepth>1) {
                double newCurveCost3;
                if (j<2)
                  newCurveCost3 = CIM_COST_INFINITY;
                else {
                  //5-3-6)j moves 2, i moves in one of the 8 directions.
                  newCurveCost3 = computeCost (
                    j-2, //j1=j-2
                    j, //j2=j
                    xx, yy, //(xx,yy)to(x,y)
                    x, y,
                    DPPrevTangent[j-2][yy][xx]);
                  newCurveCost3 += DPTable[j-2][yy][xx];
                }
                if (newCurveCost3 < newCostXY) {
                  newCostXY = newCurveCost3;
                  jstep = 2;
                }
              }

              if (_TemplateDepth>2) {
                double newCurveCost4;
                if (j<3)
                  newCurveCost4 = CIM_COST_INFINITY;
                else {
                  //5-3-7)j moves 3, i moves in one of the 8 directions.
                  newCurveCost4 = computeCost (
                    j-3, //j1=j-3
                    j, //j2=j
                    xx, yy, //(xx,yy)to(x,y)
                    x, y,
                    DPPrevTangent[j-3][yy][xx]);
                  newCurveCost4 += DPTable[j-3][yy][xx];
                }
                if (newCurveCost4 < newCostXY) {
                  newCostXY = newCurveCost4;
                  jstep = 3;
                }
              }

            }
          }//end all points in the boundary conditions.
          if (newCostXY < minCostXY) { //5x5
            minCostXY = newCostXY;
            //determine minCostTangent by deltax and deltay. (inverse delta here!)
            minCostTangent = PrevXYStepToTanCode (deltax,deltay, jstep);
          }
        }//end loop TanTable5x5

        //5-3-8)After all, for each (x,y) set minCostXY to DPTable[j][y][x]
        if (DPTable[j][y][x] > minCostXY) {
          DPTable[j][y][x] = minCostXY;
          DPPrevTangent[j][y][x] = minCostTangent;
        }

        //5-4)7x7, Instead of the 2D convolving way of implementation,
        //    using a 1D loop into each TanCodeTable...
        //    [TanCode, deltax, deltay]
        if (_TemplateSize < CIM_TEMPLATE_7X7)
          continue;
        for (t=0; t<TANCODETABLE7X7_SIZE; t++) {
          tancode = TANCODETABLE7X7[t][0];
          deltax = TANCODETABLE7X7[t][1];
          deltay = TANCODETABLE7X7[t][2];

          //5-4-1)For the (x,y), skip becuase it's 5-1)i fix.
          //if (tancode==0) //(deltax==0 && deltay==0)
          //  continue;

          int xx = x+deltax;
          int yy = y+deltay;
          int jstep = 1;

          //5-4-2)Other 16 Points in the template (17-32)...
          //Be careful about the boundary conditions.
          if (xx>=0 && xx<_SearchWindowWidth && yy>=0 && yy<_SearchWindowHeight) {

            /*//5-4-3)  For the previous point, don't allow self-intersecting!
            if (deltax==prevx && deltay==prevy)
              continue;*/

            if (DPTable[j-1][yy][xx] == CIM_COST_INFINITY) {
              newCostXY = CIM_COST_INFINITY;
            }
            else { //Look for Non-Infinity entry...

              //5-4-4)j fixes, i moves in one of the 16 directions,
              double newCurveCost1 = computeCost (
                j-1, //j1=j-1
                j-1, //j2=j-1
                xx, yy, //(xx,yy)to(x,y)
                x, y,
                DPPrevTangent[j-1][yy][xx]);

              //5-4-5)Both i and j move one step (i has 16 directions).
              double newCurveCost2 = computeCost (
                j-1, //j1=j-1
                j, //j2=j
                xx, yy, //(xx,yy)to(x,y)
                x, y,
                DPPrevTangent[j-1][yy][xx]);

              double newCurveCost = min2 (newCurveCost1, newCurveCost2);
              newCostXY = DPTable[j-1][yy][xx] + newCurveCost;

              if (_TemplateDepth>1) {
                double newCurveCost3;
                if (j<2)
                  newCurveCost3 = CIM_COST_INFINITY;
                else {
                  //5-4-6)j moves 2, i moves in one of the 16 directions.
                  newCurveCost3 = computeCost (
                    j-2, //j1=j-2
                    j, //j2=j
                    xx, yy, //(xx,yy)to(x,y)
                    x, y,
                    DPPrevTangent[j-2][yy][xx]);
                  newCurveCost3 += DPTable[j-2][yy][xx];
                }
                if (newCurveCost3 < newCostXY) {
                  newCostXY = newCurveCost3;
                  jstep = 2;
                }
              }

              if (_TemplateDepth>2) {
                double newCurveCost4;
                if (j<3)
                  newCurveCost4 = CIM_COST_INFINITY;
                else {
                  //5-4-7)j moves 3, i moves in one of the 16 directions.
                  newCurveCost4 = computeCost (
                    j-3, //j1=j-3
                    j, //j2=j
                    xx, yy, //(xx,yy)to(x,y)
                    x, y,
                    DPPrevTangent[j-3][yy][xx]);
                  newCurveCost4 += DPTable[j-3][yy][xx];
                }
                if (newCurveCost4 < newCostXY) {
                  newCostXY = newCurveCost4;
                  jstep = 3;
                }
              }

            }
          }//end all points in the boundary conditions.
          if (newCostXY < minCostXY) { //7x7
            minCostXY = newCostXY;
            //determine minCostTangent by deltax and deltay. (inverse delta here!)
            minCostTangent = PrevXYStepToTanCode (deltax,deltay, jstep);
          }
        }//end loop TanTable7x7

        //5-4-8)After all, for each (x,y) set minCostXY to DPTable[j][y][x]
        if (DPTable[j][y][x] > minCostXY) {
          DPTable[j][y][x] = minCostXY;
          DPPrevTangent[j][y][x] = minCostTangent;
        }

      }//end loop x
    j++; //next step
    vcl_cout << "Step/Total: "<< j <<"/"<< _inputCurveLength;
    //STATUSOUT;
  }//DP_step loop

  //6)Output: After D.P., Put the first _nBestMatch optimal result
  //  in _outputCurve[i] and build _fmap[i]...
  //6-1)Get the BestMatchingCost[i] and position in the DP Table
  //The best match is the i=0 one.
  for (int i=0; i<_nBestMatch; i++) {
    j = _inputCurveLength-1;
    _BestMatchingCost[i] = CIM_COST_INFINITY;
    int bestX, bestY;
    for (y=0; y<_SearchWindowHeight; y++) {
      for (x=0; x<_SearchWindowWidth; x++) {
        if (DPTable[j][y][x] < _BestMatchingCost[i] &&
           x>endPointX-5 && x<endPointX+5 &&
           y>endPointY-5 && y<endPointY+5) {
          _BestMatchingCost[i] = DPTable[j][y][x];
          bestX=x;
          bestY=y;
        }
      }
    }
    assert (!(bestX<0));
    assert (!(bestY<0));
    //Delete the best one so that we can get second best later on.
    DPTable[j][bestY][bestX] = CIM_COST_INFINITY*2;

    //6-2)Build the Output bsol_intrinsic_curve_2d. (OutputCurve is in absolute position)
    j = _inputCurveLength-1;  //index of the DPTable (n-1 to 0)
    int ii=0;            //index of the image curve  (0 to m)
    vcl_pair <int,int> p (j,ii);
    _fmap[i].push_back (p);
    short int curx = bestX;
    short int cury = bestY;
    _outputCurve[ii].add_vertex (curx+_SearchWindowLeft, cury+_SearchWindowTop);
    while (j>0) {
      short int prevx, prevy, jstep;
      TanCodeToPrevXYStep (DPPrevTangent[j][cury][curx], prevx, prevy, jstep);

      prevx += curx;
      prevy += cury;
      ii++;
      j -= jstep;

      _outputCurve[i].add_vertex (prevx+_SearchWindowLeft, prevy+_SearchWindowTop);

      vcl_pair <int,int> p (j,ii);
      _fmap[i].push_back (p);

      curx = prevx;
      cury = prevy;
    }
  }//end for i
}

void CurveImageMatch::allocateDPMemory (void)
{
  int j, y;

  //Initialize the D.P. tables... dimension size(CurveLength)*size(_imageWidth)*size(_imageHeight)
  _inputCurveLength  = _inputCurve->size();
  //_imageWidth      = _image->width();
  //_imageHeight    = _image->height();

  DPTable = new double**[_inputCurveLength];
  for (j=0; j<_inputCurveLength; j++) {
    DPTable[j] = new double*[_SearchWindowHeight]; //_imageHeight
    for (y=0; y<_SearchWindowHeight; y++) //_imageHeight
      DPTable[j][y] = new double[_SearchWindowWidth]; //_imageWidth
  }

  DPPrevTangent = new int**[_inputCurveLength];
  for (j=0; j<_inputCurveLength; j++) {
    DPPrevTangent[j] = new int*[_SearchWindowHeight]; //_imageHeight
    for (y=0; y<_SearchWindowHeight; y++) //_imageHeight
      DPPrevTangent[j][y] = new int[_SearchWindowWidth]; //_imageWidth
  }

  //Output
  _BestMatchingCost = new double[_nBestMatch];
  _outputCurve = new bsol_intrinsic_curve_2d[_nBestMatch];
  _fmap = new FinalMapType[_nBestMatch];
}

void CurveImageMatch::freeDPMemory (void)
{
  int j, y;

  ////////////////// 3X3 //////////////////
  for (j=0; j<_inputCurveLength; j++) {
    for (y=0; y<_SearchWindowHeight; y++) //_imageHeight
      delete []DPTable[j][y];
    delete []DPTable[j];
    DPTable[j] = NULL;
  }
  delete []DPTable;
  DPTable = NULL;

  for (j=0; j<_inputCurveLength; j++) {
    for (y=0; y<_SearchWindowHeight; y++) //_imageHeight
      delete []DPPrevTangent[j][y];
    delete []DPPrevTangent[j];
    DPPrevTangent[j] = NULL;
  }
  delete []DPPrevTangent;
  DPPrevTangent = NULL;

  //Output
  if (_BestMatchingCost)
    delete _BestMatchingCost;
  _BestMatchingCost = NULL;
  if (_outputCurve)
  delete []_outputCurve;
  _outputCurve = NULL;
  if (_fmap)
  delete []_fmap;
  _fmap = NULL;
}

///#include "points.h"

double bengCostImage (int i1x, int i1y, int i2x, int i2y, int prevx, int prevy)
{
  //avoid going back!
  //If no previous point, it means going back, overlapping, cost should be HUGE
  if (prevx==i1x && prevy==i1y)
    return CIM_COST_INFINITY;

  double tangent01 = vcl_atan2 ((double)i1y-prevy, (double)i1x-prevx);
  double tangent12 = vcl_atan2 ((double)i2y-i1y, (double)i2x-i1x);
  double result = curve_angleDiff (tangent12, tangent01); //current-previous
  return result;
}

double orientCostImage (int i1x, int i1y, int i2x, int i2y, double startTan)
{
  //If no new orientation, no cost
  if (i2x==i1x && i2y==i1y)
    return 0;

  double tangent12 = vcl_atan2 ((double)i2y-i1y, (double)i2x-i1x);
  double result = curve_angleDiff (tangent12, startTan); //current-previous
  return result;
}

//Two kinds of bsol_intrinsic_curve_2d-Image Matching!
//1)Use the gradient information in curve 1
//2)Look for the max gradient in curve 2

//Cost = alpha*CurveCost + beta*ImageCost
double CurveImageMatch::computeCost (int j1, int j2, int i1x, int i1y, int i2x, int i2y, int prevTangent)
{
  //1)CurveCost
  //1-A)StretchCost
  double dStretchCost;
  if (j1==j2 || (i1x==i2x&&i1y==i2y)) {
    dStretchCost = 10;
  }
  else {
    double curvelength1 = stretchCost (_inputCurve, j2, j1);
    double curvelength2 = hypot (i2x-i1x, i2y-i1y);
    dStretchCost = vcl_fabs(curvelength1-curvelength2);
  }

  //1-B)OrientationCost
  //double orientCost1 = _inputCurve.bendCost (j2, 0);
  //double orientCost2 = orientCostImage (i1x, i1y, i2x, i2y, _startTan);
  //double orientCost = vcl_fabs(orientCost1-orientCost2);

  //1-C)BendingCost
  double dBendCost;
  if (j1==0 || prevTangent==CIM_TANCODE_0)
    dBendCost = 0;
  else {

    double bendCost1 = bendCost (_inputCurve, j2, j1);
    short int prevx, prevy, jstep;
    if (prevTangent==CIM_TANCODE_NA) {
      //assert (0); PREV_COST = INFINITY
      return CIM_COST_INFINITY;
    }
    TanCodeToPrevXYStep (prevTangent, prevx, prevy, jstep);
    prevx += i1x;
    prevy += i1y;
    double bendCost2 = bengCostImage (i1x, i1y, i2x, i2y, prevx, prevy);
    if (bendCost2 == CIM_COST_INFINITY)
      return CIM_COST_INFINITY;
    dBendCost = vcl_fabs(bendCost2 - bendCost1);
  }

  #define _R 3 //10

  double CurveCost = _R*dStretchCost + dBendCost; //+_R*0.2*orientCost;

  //2)Image Gradient Cost
  int curTangent = DeltaXYStepToTanCode (i2x-i1x, i2y-i1y, 1);
  double ImageCost1 = computeImageCostCanny (i1x, i1y, curTangent);
  double ImageCost2 = computeImageCostCanny (i2x, i2y, curTangent);
  double ImageCost = (ImageCost1+ImageCost2);

  int step = DeltaXYToStep (i2x-i1x, i2y-i1y);
  ImageCost *= step;

  #define _R2 5 //10
  ImageCost *= _R2;

  //3)Total Cost
  double totalCost = _alpha*CurveCost + _beta*ImageCost;
  if (totalCost<1E-15 && totalCost>0)
    return 0;
  else
    return totalCost;
}

  //double ImageCost1_p1 = computeImageCostCanny (_inputCurve.x(j1)-_oldStartPointX+_startPointX,
  //                               _inputCurve.y(j1)-_oldStartPointY+_startPointY,
  //                               curTangent);
  //double ImageCost1_p2 = computeImageCostCanny (_inputCurve.x(j2)-_oldStartPointX+_startPointX,
  //                               _inputCurve.y(j2)-_oldStartPointY+_startPointY,
  //                               curTangent);
  //double ImageCost2 = _R2*(ImageCost1_p1+ImageCost1_p2);
  //double ImageCost = vcl_fabs (ImageCost1 - ImageCost2);

int GradientTemplate[][9] = {
  { 0, 1, 1, //0
    0, 0, 0,
    0,-1,-1},
  { 0, 0, 1, //1
    0, 0, 1,
   -1,-1, 0},
  { 0, 0, 0, //2
   -1, 0, 1,
   -1, 0, 1},
  {-1, 0, 0, //3
   -1, 0, 0,
    0, 1, 1},
  {-1,-1, 0, //4
    0, 0, 0,
    1, 1, 0},
  { 0,-1,-1, //5
    1, 0, 0,
    1, 0, 0},
  { 1, 0,-1, //6
    1, 0,-1,
    0, 0, 0},
  { 1, 1, 0, //7
    0, 0,-1,
    0, 0,-1}
};

double CurveImageMatch::computeImageCost (int posx, int posy, int dir)
{

  double imageCost=0.0;
  /*
  //No cost defined for the same point!
  if (dir==8)
    return 0.5; //some single point contrast (sigma) is also good here!! 0

  i=0; sum=0;
  for (y=-1; y<2; y++) {
    for (x=-1; x<2; x++) {
      px = posx+x;
      py = posy+y;
      if (px>=0 && px<_image->width() && py>=0 && py<_image->height()) {
        //pix: gray-value of the pixel
        pix = _image->r(px, py)*0.3+_image->g(px, py)*0.59+_image->b(px, py)*0.11;
        sum += (pix-128) * GradientTemplate[dir][i];
      }
      i++;
    }
  }
  sum = vcl_fabs (sum);
  imageCost = (512-sum)/512; //0: perfect edge, 1: no edge, normalized!*/
  return imageCost;
}

//#include "canny_edge.h"

void CurveImageMatch::computeCannyMagnitudeDir (void)
{
/*  int x, y;

  if (_image==NULL) {
    COUT<< "No image!" <<vcl_endl;
    MSGOUT (0);
    return;
  }

  RGBImage* img = _image;
  int w = img->width();
  int h = img->height();

  double low_threshold  = 0.4;// Canny recommends 0.33 - 0.5 //0.2
  double high_threshold = 0.8;//0.8
  double sigma = 1; //0.6; //;

  //These two buffer are Column-first based buffer
  unsigned char* imgbuf = new unsigned char[h*w];
  unsigned char* edgebuf = new unsigned char[h*w];

  nmsbuf = new short int [h*w];
  tanbuf = new float[h*w];

  for (x=0; x<w; x++)
    for (y=0; y<h; y++)
      imgbuf[x*h+y] = (img->r(x,y)+img->g(x,y)+img->b(x,y))/3;

  canny (imgbuf, w, h, sigma, low_threshold, high_threshold, nmsbuf, edgebuf, tanbuf);

  //normalize the magnitudebuf
  max_nms = -1E5;
  for (y=0; y<h; y++){
    for (x=0; x<w; x++) {
      if (nmsbuf[x*h+y] > max_nms)
        max_nms = nmsbuf[x*h+y];
    }
  }

  _edgeMagImage = new EdgeImage (w, h);
  for (y=0; y<h; y++){
    for (x=0; x<w; x++) {
      //edgeimg->setPix (x, y, 255 - edgebuf[x*h+y]); //because 255 codes NOEDGE
      _edgeMagImage->setPix (x, y, nmsbuf[x*h+y]*255/max_nms);
      _edgeMagImage->setTan (x, y, tanbuf[x*h+y]);
      //img->setR(x,y, imgbuf[x*h+y]);
      //img->setG(x,y, imgbuf[x*h+y]);
      //img->setB(x,y, imgbuf[x*h+y]);
    }
  }

  COUT<< "\nCanny Edge..." <<vcl_endl;
  COUT<< "High Threshold = " << high_threshold;
  COUT<< ", Low Threshold = " << low_threshold;
  COUT<< ", Sigma = " << sigma <<vcl_endl;
  COUT<< "Image Size: "<< img->width() <<"x"<< img->height() <<", ";

  MSGOUT (0);

  delete imgbuf, edgebuf;*/
}

//0: perfect edge, 1: no edge, normalized!
double CurveImageMatch::computeImageCostCanny (int posx, int posy, int dir)
{
  //pos is relative
/*  posx += _SearchWindowLeft;
  posy += _SearchWindowTop;

  double imageCost = ((double)_edgeMagImage->pix (posx, posy))/255;

  if (imageCost==0)
    return 10;

  //Now, imageCost for (x, y) is between 0 and 1
  //0: high gradient, low cost
  //1: low gradient,  high cost

  //No cost defined for the same point!
  if (dir==0)
    return 1-imageCost; //some single point contrast (sigma) is also good here!! 0.5

  //approximate the edge confidence!!
  //if (imageCost<0.2)
  //  imageCost = 0;
  //if (imageCost>0.5)
  //  imageCost = 1;

  //Edge direction.
  short int deltax, deltay, jstep;
  TanCodeToDeltaXYStep (dir, deltax, deltay, jstep);
  double norm = hypot (deltax, deltay);
  double dx = deltax/norm;
  double dy = deltay/norm;

  //double tanx = cos(tanbuf[posx*h+posy]);
  //double tany = sin(tanbuf[posx*h+posy]);
  double tanx = cos(_edgeMagImage->tan(posx,posy));
  double tany = sin(_edgeMagImage->tan(posx,posy));
  double dot = tanx*dx + tany*dy;
  double dotcost = vcl_fabs(dot);
  assert (dotcost <=1 && dotcost>=0);

  imageCost = dotcost*imageCost;

  assert (imageCost <=1 && imageCost>=0);
  return 1-imageCost;  */
  return 1;
}

  //RGBImage* img = _image;
  //int w = img->width();
  //int h = img->height();

  //double edgeness = (double) nmsbuf[posx*h+posy]/max_nms; //0(no edge), to 1(perfect edge).
  //imageCost = edgeness; //Edge magnitude
  //assert (imageCost <=1 && imageCost>=0);

/*
//0: perfect edge, 1: no edge, normalized!
double CurveImageMatch::computeImageCost1Canny (int posx, int posy, int dir)
{
  double imageCost;

  //pos is relative
  posx += _SearchWindowLeft;
  posy += _SearchWindowTop;

  RGBImage* img = _image;
  int w = img->width();
  int h = img->height();

  double edgeness = (double) nmsbuf[posx*h+posy]/max_nms; //0(no edge), to 1(perfect edge).
  imageCost = edgeness; //Edge magnitude
  //assert (imageCost <=1 && imageCost>=0);

  //No cost defined for the same point!
  if (dir==0)
    return imageCost; //some single point contrast (sigma) is also good here!! 0.5

  //approximate the edge confidence!!
  if (imageCost<0.2)
    imageCost = 0;
  if (imageCost>0.5)
    imageCost = 1;

  //Edge direction.
  short int deltax, deltay, jstep;
  TanCodeToDeltaXYStep (dir, deltax, deltay, jstep);
  double norm = hypot (deltax, deltay);
  double dx = deltax/norm;
  double dy = deltay/norm;

  double tanx = cos(tanbuf[posx*h+posy]);
  double tany = sin(tanbuf[posx*h+posy]);
  double dot = tanx*dx + tany*dy;
  double dotcost = vcl_fabs(dot);
  assert (dotcost <=1 && dotcost>=0);

  //dotcost = 1-dotcost;
  imageCost = dotcost*imageCost;
  imageCost = 1-imageCost;

  return imageCost;
}*/

CurveImageMatch::~CurveImageMatch (void)
{
  freeDPMemory ();
  if (nmsbuf)
    delete nmsbuf;

  nmsbuf = NULL;
  if (tanbuf)
    delete tanbuf;
  tanbuf = NULL;
}

//Resample inputCuve into outputCurve using type...
//assume outputCurve is empty for use...
void ResampleCurve (RESAMPLE_CURVE_TYPE resample_type, bsol_intrinsic_curve_2d* inputCurve, bsol_intrinsic_curve_2d* outputCurve)
{
  int i;
  double x, y, prevx, prevy;

  if (resample_type==RESAMPLE_CURVE_UNIT) {
    prevx = inputCurve->x(0);
    prevy = inputCurve->y(0);
    outputCurve->add_vertex (prevx, prevy);

    for (i=1; i<inputCurve->size(); i++) {
      x = inputCurve->x(i);
      y = inputCurve->y(i);
      double dist = hypot(x-prevx, y-prevy);
      if (dist>3) {
        vcl_cout << "hypot dist > 3" <<vcl_endl;
      }
      if (dist>2) {
        outputCurve->add_vertex ((x+prevx)/2, (y+prevy)/2);
        outputCurve->add_vertex (x, y);
        prevx = x;
        prevy = y;
      }
      else if (dist>1) {
        outputCurve->add_vertex (x, y);
        prevx = x;
        prevy = y;
      }
    }

    //1-2)fix the starting tangent of the curve!
    int intx = outputCurve->x(0);
    int inty = outputCurve->y(0);
    outputCurve->modify_vertex (0, intx, inty);
    intx = outputCurve->x(1);
    inty = outputCurve->y(1);
    outputCurve->modify_vertex (1, intx, inty);
    outputCurve->computeProperties ();
  }
  else if (resample_type==RESAMPLE_CURVE_GRID) {
    prevx = (int) inputCurve->x(0);
    prevy = (int) inputCurve->y(0);
    outputCurve->add_vertex (prevx, prevy);

    for (i=1; i<inputCurve->size(); i++) {
      x = (int) inputCurve->x(i);
      y = (int) inputCurve->y(i);

      if (!(x==prevx && y==prevy)) {
        outputCurve->add_vertex (x, y);
        prevx = x;
        prevy = y;
      }
    }

    //1-2)fix the starting tangent of the curve!
    int intx = outputCurve->x(0);
    int inty = outputCurve->y(0);
    outputCurve->modify_vertex (0, intx, inty);
    intx = outputCurve->x(1);
    inty = outputCurve->y(1);
    outputCurve->modify_vertex (1, intx, inty);
    outputCurve->computeProperties ();
  }
  else { ////RESAMPLE_CURVE_OFF: do nothing!
    *outputCurve = *inputCurve;
  }
}
