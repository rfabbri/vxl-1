#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include "PuzzleSolving.h"


/*  --spinner--
//  commenting out ALL global variables b/c they are unsafe.  if you don't initialize them you, the
//  code will still use them, no complaints, but your results will be odd, you may not know why
//GLOBAL VARIABLES
//int NUM_COARSE_OUT;
//int NUM_FINE_OUT;

//Number of top states.  All are displayed.
int NUM_TOP_STATES;

//Number of states returned by iterateSearch().
int NUM_STATES_ITER;
int DISPLAY_GRID_ROWS;
int DISPLAY_GRID_COLS;
double GRID_BORDER;
double SCREEN_SIZE;
//int LOCAL_REG_ITER_PAIR;
int LOCAL_REG_ITER_PUZ;
//int MIN_MAP_SIZE_CRSE;
//int MIN_MAP_SIZE_FINE; 
//int FINE_SAMPLE_SKIP;
//double MAX_FINE_COST;


//double OLAP_THRESH_HIGH;

//Overlap threshold.  Lower means more options eliminated
//double OLAP_THRESH_LOW;


double DIST_THRESH_HIGH;
double DIST_THRESH_LOW;
//double DIST_COEF;     
double LENGTH_THRESH_HIGH;
//double LENGTH_THRESH_LOW;
//double LENGTH_COEF; 
double DIAG_THRESH_HIGH;
//double DIAG_THRESH_LOW; 
//double DIAG_COEF;
//double EDGE_COEF;
double REMATCH_THRESH; 
//double LOCAL_REG_THRESH_HIGH;
//double LOCAL_REG_THRESH_LOW;
//double INITIAL_SMOOTH;
//double COARSE_SAMPLE_SIZE;
double RESAMPLE_SIZE;
double CONST_THRESH;
double EDGE_REWARD;
double EDGE_PENALTY;
bool CONTOUR_FLAG;bool IMAGE_FLAG;

vcl_string IMAGE_PATH;
vcl_string IMAGE_TYPE;    
vcl_string MASK_PATH;
vcl_string MASK_TYPE;
int VICINITY;
double DIST_STEP;
double MIN_TAN_TURN;
int TANGENT_LENGTH;

bool CONTINUOUS_SEARCH;
int IDENTICAL_BONUS;
double CULLING_RANGE; //Maximum difference in cost between lowest cost and costs of other candidates
double INIT_LENGTH_COEF;
double INIT_DIAG_COEF;
double INIT_DIST_COEF;
bool DISPLAY_INFORMATION;
//bool TIMING_ON;

double CLOSED_JUNCTION_BONUS;
//double CLOSE_ANGLE_THRESH;
//int MOVE_BACK;
//int NUM_POINTS_AVERAGED;
//int DISTANCE_THRESH_1;
//int DISTANCE_THRESH_2;
//int SMOOTH_EDGE_BONUS;
*/

//List of indexes of pairwise matches categorized by piece number.
//All pairs featuring the i'th piece will be listed in order of increasing cost
//in the _pairMatchesByPiece[i] element.
//moved this next one into puzzleSolving.h
//vcl_vector<vcl_vector<int> > _pairMatchesByPiece;
vcl_vector<vcl_vector<vcl_vector<int> > > _matchesByPair;


int _totalIterateSearchTicks=0;
int _totalSearchTicks=0;
int _totalPairMatchTicks=0;
int _totalTestDPTicks=0;
int _totalComboMatchTicks=0;
int _totalLocalRegTicks=0;
int _totalFineScaleMatchTicks =0;

void PuzzleSolving::printContours(){ 
    vcl_cout << "BEGIN puzzle solving contours information______" << vcl_endl;
    vcl_cout << "size of _Contours: " << _Contours.size() << vcl_endl; 
    for( unsigned i = 0; i < _Contours.size(); i++ ){
        vcl_cout << "Contour [" << i << "] has " << _Contours[i].numPoints() << " points. " << vcl_endl;
    }
    vcl_cout << "END puzzle solving contours information______" << vcl_endl;
};

void PuzzleSolving::setContours( vcl_vector<Curve<double,double> >  &contoursIN ){ 
        //this will envoke the copy ctor and we will get a local copy
        _Contours = contoursIN;
        _nPieces = _Contours.size();
};

void PuzzleSolving::preProcessCurves(){
    vcl_cout << "Preprocessing curves..." << vcl_endl;
    for(int i = 0; i < _nPieces; i++) {
        vcl_cout << "  preprocessing contour [" << i << "]." << vcl_endl;
        _Contours[i].smooth(INITIAL_SMOOTH);
        _Contours[i].coarseResample(COARSE_SAMPLE_SIZE);
    }
    vcl_cout << "Preprocessing complete." << vcl_endl;
}

/*
void PuzzleSolving::setGlobals() {

  NUM_COARSE_OUT = config::val("NUM_COARSE_OUT",30);
  NUM_FINE_OUT = config::val("NUM_FINE_OUT",5);
  NUM_TOP_STATES = config::val("NUM_TOP_STATES",9);
  NUM_STATES_ITER = config::val("NUM_STATES_ITER",5);
  
  DISPLAY_GRID_ROWS = config::val("DISPLAY_GRID_ROWS",3);
  DISPLAY_GRID_COLS = config::val("DISPLAY_GRID_COLS",3); 
  GRID_BORDER = config::val("GRID_BORDER",1.0f);
  SCREEN_SIZE = config::val("SCREEN_SIZE",100.0f);

  LOCAL_REG_ITER_PAIR = config::val("LOCAL_REG_ITER_PAIR",2);
  LOCAL_REG_ITER_PUZ = config::val("LOCAL_REG_ITER_PUZ",5);
  
  MIN_MAP_SIZE_CRSE = config::val("MIN_MAP_SIZE_CRSE",2);
  MIN_MAP_SIZE_FINE = config::val("MIN_MAP_SIZE_FINE",25);
  
  FINE_SAMPLE_SKIP = config::val("FINE_SAMPLE_SKIP",5);
  
  MAX_FINE_COST = config::val("MAX_FINE_COST",100.0f);

  OLAP_THRESH_HIGH = config::val("OLAP_THRESH_HIGH",20.0f);
  OLAP_THRESH_LOW = config::val("OLAP_THRESH_LOW",15.0f);
  
  DIST_THRESH_HIGH = config::val("DIST_THRESH_HIGH",5.0f);
  DIST_THRESH_LOW = config::val("DIST_THRESH_LOW",3.0f);
  DIST_COEF = config::val("DIST_COEF",5.0f);     
  
  LENGTH_THRESH_HIGH = config::val("LENGTH_THRESH_HIGH",100.0f);
  LENGTH_THRESH_LOW = config::val("LENGTH_THRESH_LOW",50.0f);
  LENGTH_COEF = config::val("LENGTH_COEF",-1.0f);
  
  DIAG_THRESH_HIGH = config::val("DIAG_THRESH_HIGH",5.0f);
  DIAG_THRESH_LOW = config::val("DIAG_THRESH_LOW",3.0f);
  DIAG_COEF = config::val("DIAG_COEF",-5.0f);

  EDGE_COEF = config::val("EDGE_COEF",-0.5f);
  
  REMATCH_THRESH = config::val("REMATCH_THRESH",-20.0f);
  
  LOCAL_REG_THRESH_HIGH = config::val("LOCAL_REG_THRESH_HIGH",15.0f);
  LOCAL_REG_THRESH_LOW = config::val("LOCAL_REG_THRESH_LOW",1.0f);
  
  INITIAL_SMOOTH = config::val("INITIAL_SMOOTH",5.0f);
  COARSE_SAMPLE_SIZE = config::val("COARSE_SAMPLE_SIZE",25.0f);
  RESAMPLE_SIZE = config::val("RESAMPLE_SIZE",1.0f);

  CONST_THRESH = config::val("CONST_THRESH",0.99f);
  EDGE_REWARD = config::val("EDGE_REWARD",5.0f);
  EDGE_PENALTY = config::val("EDGE_PENALTY",1.0f);

  if(config::val("IMAGE_FLAG",0)==1) IMAGE_FLAG=1;
  else IMAGE_FLAG=0;

  if(config::val("CONTOUR_FLAG",1)==1) CONTOUR_FLAG=1;
  else CONTOUR_FLAG=0;

  IMAGE_PATH = config::val("IMAGE_PATH","img");
  IMAGE_TYPE = config::val("IMAGE_TYPE","jpg");    
  MASK_PATH = config::val("MASK_PATH","msk");
  MASK_TYPE = config::val("MASK_TYPE","bmp");

  VICINITY = config::val("VICINITY",30);
  DIST_STEP  = config::val("DIST_STEP",2.0f);
  MIN_TAN_TURN  = config::val("MIN_TAN_TURN",float((M_PI*30)/180.0));
  TANGENT_LENGTH  = config::val("TANGENT_LENGTH",10);

  //Boolean whose value determines whether a button press 
  //is necessary to move on to next phase of reassembly.
  //1 means no button press required
  CONTINUOUS_SEARCH = config::val("CONTINUOUS_SEARCH", 0);

  //Bonus for reaching same state independantly
  IDENTICAL_BONUS = config::val("IDENTICAL_BONUS", 20);  
  
  //Defines maximum difference between the lowest cost of any state and the highest included in the next
  //iteration
  CULLING_RANGE = config::val("CULLING_RANGE", 100);

  //Flag for displaying information
  DISPLAY_INFORMATION = config::val("DISPLAY_INFORMATION", 1);

  //Turns timing information on and off.
  TIMING_ON = config :: val("TIMING_ON", 0);

  CLOSED_JUNCTION_BONUS = config::val("CLOSED_JUNCTION_BONUS", 25);

  CLOSE_ANGLE_THRESH = config::val("CLOSE_ANGLE_THRESH", float(M_PI/10.0));

  MOVE_BACK = config::val("MOVE_BACK", 25);

  NUM_POINTS_AVERAGED = config::val("NUM_POINTS_AVERAGED", 15);

  DISTANCE_THRESH_1 = config::val("DISTANCE_THRESH_1", 10);
  DISTANCE_THRESH_2 = config::val("DISTANCE_THRESH_2", 15);
  SMOOTH_EDGE_BONUS = config::val("SMOOTH_EDGE_BONUS", 5);


}
*/

/*
void PuzzleSolving::imToggle(){
  IMAGE_FLAG=(!IMAGE_FLAG);
}

void PuzzleSolving::lnToggle(){
  CONTOUR_FLAG=(!CONTOUR_FLAG);
}
*/

/*
void PuzzleSolving::timingToggle(){
  TIMING_ON = (!TIMING_ON);
  vcl_cout << "\n**************\n" << vcl_endl;
  vcl_cout << "TIMING_ON is: " << TIMING_ON <<vcl_endl;
  vcl_cout << "\n**************\n" << vcl_endl;
  //MSGOUT(0);
}

void PuzzleSolving::timingDisplay(){

  vcl_cout << "\n\n***********************************************\n\n" << vcl_endl;
  vcl_cout << "Total iterateSearch() time: " << (double)_totalIterateSearchTicks/CLOCKS_PER_SEC << vcl_endl;
  vcl_cout << "Total search() time: " << (double)_totalSearchTicks/CLOCKS_PER_SEC << vcl_endl;
  vcl_cout << "Total pairMatch() time: " << (double)_totalPairMatchTicks/CLOCKS_PER_SEC << vcl_endl;
  vcl_cout << "Total testDP() time: " << (double)_totalTestDPTicks/CLOCKS_PER_SEC << vcl_endl;
  vcl_cout << "Total comboMatch() time: " << (double)_totalComboMatchTicks/CLOCKS_PER_SEC << vcl_endl;
  vcl_cout << "Total localReg() time: " << (double)_totalLocalRegTicks/CLOCKS_PER_SEC << vcl_endl;
  vcl_cout << "Total fineScaleMatch() time: " << (double)_totalFineScaleMatchTicks/CLOCKS_PER_SEC << vcl_endl;
  vcl_cout << "\n\n***********************************************\n\n" << vcl_endl;
  //MSGOUT(1);
}
*/

int PuzzleSolving::loadFiles(const vcl_string& batchFileName){

  //  struct Combined_Contour{
  //  int index[10];
  //  int map[10][10][10];
  //};
  
  _nPieces=0;
  _filename = batchFileName;
  /*
  //Variables for timing
  int startTicks = clock();
  int endTicks;
  double seconds;
  */

  //int l1,l2;
  int i;
  vcl_string file,path,name,iname,mname,ename;
  
  vcl_ifstream infp(batchFileName.c_str());
  
  if(infp==NULL) 
    exit(-1);
  
  vcl_cout << vcl_endl << "Loading Contours:" << vcl_endl;
  vcl_cout <<         "-----------------" << vcl_endl;
  while(infp.eof()!=1) {
    char fname[256];
    Curve<double,double> this_curve;
    Curve<double,double> this_ecurve;
    infp.getline(fname,256);
    file=fname;
    
    if(strlen(fname)>2) {
      vcl_cout << file << vcl_endl;
      fflush(stdout);
      
      this_curve.readDataFromFile(fname);
      
      _Contours.push_back(this_curve);
      _filenames.push_back(fname);
      
      /*
      if(IMAGE_FLAG) {
  l1=file.find_last_of('/');
  l2=file.find_last_of('.');
  path=file.substr(0,l1+1);
  name=file.substr(l1+1,l2-l1-8);
  iname=path+IMAGE_PATH+"/"+name+"."+IMAGE_TYPE;
  mname=path+MASK_PATH+"/"+name+"."+MASK_TYPE;
  ename=path+"/erd/"+name+"_e-points.con";
  this_ecurve.readDataFromFile(ename);
  _EContours.push_back(this_ecurve);
  _imagenames.push_back(iname);
  _masknames.push_back(mname);
  
  vcl_cout << iname << vcl_endl;
  vcl_cout << mname << vcl_endl;
  vcl_cout << ename << vcl_endl;
      }
      */

      _nPieces++;
    }
  }
  infp.close();
  vcl_cout << vcl_endl;

  vcl_cout << "Preprocessing Contours:"<< vcl_endl;
  vcl_cout << "-----------------------"<< vcl_endl;

  //int orient=_Contours[0].orient();
  for(i=0;i<_nPieces;i++) {
    //if(_Contours[i].orient()!=orient)
    // _Contours[i].invert();


    _Contours[i].smooth(INITIAL_SMOOTH);
    _Contours[i].coarseResample(COARSE_SAMPLE_SIZE);

    /*
    if(IMAGE_FLAG) {
      RGBAImage *img = new RGBAImage(_imagenames[i].c_str());
      RGBImage *mask = new RGBImage(_masknames[i].c_str());
      img->setMask(mask);
      delete mask;
      _EContours[i].imageProfile(img);
      delete img;
    }
    */

    vcl_cout << " " << i+1;
    fflush(stdout);
  }
  vcl_cout << vcl_endl << vcl_endl;
  
  //Curve<double,double> c = _Contours[0];
  
  //Group *output1 = new Group;
  //drawLines(output1,&c,BLUE);
  //localRootNode()->addChild(output1);
  //   print((const char*)"piece1.ps",(GraphicsView*)_canvas->view(),2.0,(GraphicsNode*)output);
  //for(int cnr=0;cnr<c.numExtrema();cnr++) {
  //int c_ind=c.coarseRef(c.extrema(cnr));
  //PointGeom *pt = new PointGeom(c.x(c_ind),c.y(c_ind));
  //pt->loadStyle(Color(BLACK) & PointSize(3));
  //output1->addChild(pt);
  //}
  //   print((const char*)"piece2.ps",(GraphicsView*)_canvas->view(),2.0,(GraphicsNode*)output);
  //for(int pts=0;pts<c.coarseNumPoints();pts++) {
  //PointGeom *pt = new PointGeom(c.Cx(pts),c.Cy(pts));
  //pt->loadStyle(Color(RED) & PointSize(2));
  //output1->addChild(pt);
  //   }
  //   print((const char*)"piece3.ps",(GraphicsView*)_canvas->view(),2.0,(GraphicsNode*)output);

  /*
  endTicks = clock();
  seconds= (double) (endTicks-startTicks)/CLOCKS_PER_SEC;
  vcl_cout << "Curve loading and Preprocessing time: "<<seconds<<" seconds" << vcl_endl;
  vcl_cout << "In minutes: "<< (int) (seconds/60) <<":"<< ((seconds/60)- (int) (seconds/60))*60<<vcl_endl;
  vcl_cout << vcl_endl;
  */
  return 0;
}

searchState PuzzleSolving::pairMatch() {

  //vcl_cout << "______PuzzleSolving pairMatch() BEGIN" << vcl_endl;
 
  int i,j,k;
  
  //variables that keep track of the time for the process;

  /*
  int startTicks = clock();
  int endTicks;
  double seconds;
  */

  searchState init = searchState(_Contours);
  vcl_vector<map_with_cost> this_pair;
  /*
  if(IMAGE_FLAG)
    init=searchState(_Contours,_EContours);  
  else
    init=searchState(_Contours);
    */
  
  //int l1,l2;
  vcl_string exname1,exname2;
  
  if (_nPieces>0)
    //    for(i=0;i<1;i++)
    //      for(j=i+1;j<2;j++) {
    for(i=0;i<_nPieces-1;i++){
        for(j=i+1;j<_nPieces;j++) {
          /*
          //--spinner--
          //i do not have _filenames implemented as it would require more steps during
          //the interfacing, all i do is set up _Contours, so this code must go
      l1=_filenames[j].find_last_of('/');
      l2=_filenames[j].find_last_of('.');
      exname1=_filenames[j].substr(l1+1,l2-l1-1);
      l1=_filenames[i].find_last_of('/');
      l2=_filenames[i].find_last_of('.');
      exname2=_filenames[i].substr(l1+1,l2-l1-1);
      vcl_cout <<"Matching "<< exname1 << " to " << exname2 << vcl_endl;  
      fflush(stdout); 
            */

            vcl_cout << "matching piece [" << i << "] to piece [" << j << "]." << vcl_endl;
    
            this_pair=testDP(_Contours[j],_Contours[i]);
      if(this_pair.size()>0) {
               int this_pair_size_int = static_cast<int>(this_pair.size());
               assert(this_pair_size_int>=0);
        for(k=0;k<this_pair_size_int;k++){
            double cost = this_pair[k].first;
            intMap map = this_pair[k].second;
            Curve<double, double>  c1 = _Contours[j];
            Curve<double, double>  c2 = _Contours[i];
            double d1, d2, d3;

            regContour(&c1,&c2,map);
            localReg(&c1,&c2,&d1,&d2,&d3,1);
            cost-=SMOOTH_EDGE_BONUS*edgeCheck(&c1, &c2, map);
            init.addMatch(map,cost,j,i);
            }
        }
    }
 }

  //vcl_cout << "______PuzzleSolving pairMatch() END" << vcl_endl;

  return init;
}


/*
void PuzzleSolving::dispTopPair(searchState this_state, int i) {

  if(this_state.numMatch()==0)
    return;

  //DISPLAY_GRID_ROWS = 1;
  //DISPLAY_GRID_COLS = 1; 

  int index; 
  indexedMeasures sorted_cost = this_state.cost;
  vcl_sort(sorted_cost.begin(),sorted_cost.end(),cost_ind_less());

  int num=sorted_cost.size();
  
  index=sorted_cost[i].second;
  int p1=this_state.p1(index);
  int p2=this_state.p2(index);
  
  
  Curve<double,double>* c1 = this_state.piece(p1);
  Curve<double,double>* c2 = this_state.piece(p2);

  
  
  this_state.process.push_back(p2);
  this_state.process.push_back(p1);
  this_state.nProcess=2;
  
  double d1,d2,d3, olap;
  

  regContour(c1,c2,this_state.map(index));
  this_state.merge(c2);
  localReg(c1,c2,&d1,&d2,&d3,1);
  olap = detectOverlap(c1,c2);
  this_state.merge(c1);
  
  //this_state.tCost+=DIST_COEF*d1+LENGTH_COEF*sqrt(d2)+DIAG_COEF*sqrt(d3);
  //this_state.sCost=DIST_COEF*d1+LENGTH_COEF*sqrt(d2)+DIAG_COEF*sqrt(d3);
  

  //Data output code
  vcl_cout << "Rank of Match: " << i+1 << vcl_endl;
  vcl_cout << "--------------" << vcl_endl;
  vcl_cout << "Piece " << p1+1 << " matched with Piece " << p2+1 <<vcl_endl;
  vcl_cout << "Distance Measure : " << d1 << "\nWeighted Distance Measure: " << d1*DIST_COEF << vcl_endl;
  vcl_cout << "Length Measure: " << d2 << "\nWeighted Length Measure: " << sqrt(d2)*LENGTH_COEF << vcl_endl;
  vcl_cout << "Diagnostic Measure: " << d3 << "\nWeighted Diagnostic Measure: " << sqrt(d3)*DIAG_COEF << vcl_endl;
  vcl_cout << "Final Cost: " << this_state.cost[index].first<< vcl_endl;
  vcl_cout << "Overlap: " << olap << vcl_endl;
  vcl_cout << vcl_endl;  
 
  
  ////////////deleteAll();
 

  //initialCheck draws them!!
  initialCheck(c1, c2, this_state.map(index));

}


//Function that categorizes the matches made according to piece number
void PuzzleSolving::categorizePairMatches(searchState this_state){
  int numPiece = this_state.numPiece();
  int numMatch = this_state.numMatch();
  int pieceCnt, rankCnt;
  vcl_vector<int> pairs;


  //Sort the pairwise matches by cost.
  indexedMeasures sorted_cost = this_state.cost;
    vcl_sort(sorted_cost.begin(),sorted_cost.end(),cost_ind_less());

  //Cycle through all the matches for each piece, finding all the matches corresponding
  //to the current piece and putting them into the list.
  //Could also be done in a more efficient manner by going through the list
  //and for each match, slotting things in for each featured piece.
  for (pieceCnt =0; pieceCnt<numPiece; pieceCnt++){
    for (rankCnt = 0; rankCnt<numMatch; rankCnt++){
      
      int index = sorted_cost[rankCnt].second;
      if ((this_state.p1(index)==pieceCnt)||(this_state.p2(index)==pieceCnt))
        pairs.push_back(rankCnt);
    }
    _pairMatchesByPiece.push_back(pairs);
    pairs.clear();
  }
}


//Return the number of pairwise matches featuring the pieceNumber'th piece
int PuzzleSolving::catPairMatchSize(int pieceNumber){
  
  return _pairMatchesByPiece[pieceNumber].size();
}



//Display matches by piece and then by order.
void PuzzleSolving::dispTopPairOfPiece(searchState this_state, int pieceNumber, int count) {

  if(this_state.numMatch()==0)
    return;

  //DISPLAY_GRID_ROWS = 1;
  //DISPLAY_GRID_COLS = 1; 


  //Deals with the case where no matches are found
  if (_pairMatchesByPiece[pieceNumber].size()==0){
    vcl_cout <<"\n\n*******************************\n" << vcl_endl;
    vcl_cout <<"No matches for this piece!!!" << vcl_endl;
    vcl_cout <<"\n\n*******************************\n" << vcl_endl;
    return;
    }

  int rank=_pairMatchesByPiece[pieceNumber][count];

  indexedMeasures sorted_cost = this_state.cost;
  vcl_sort(sorted_cost.begin(),sorted_cost.end(),cost_ind_less());


  int index = sorted_cost[rank].second;

  int p1=this_state.p1(index);
  int p2=this_state.p2(index);
  
  
  Curve<double,double>* c1 = this_state.piece(p1);
  Curve<double,double>* c2 = this_state.piece(p2);
  
  regContour(c1,c2,this_state.map(index));
  
  this_state.process.push_back(p2);
  this_state.process.push_back(p1);
  this_state.nProcess=2;
  
  double d1,d2,d3, olap;
  
  this_state.merge(c2);
  localReg(c1,c2,&d1,&d2,&d3,1);
  olap = detectOverlap(c1,c2);
  this_state.merge(c1);
  

  //Data output code
  if (count==0) 
    vcl_cout << "Number of Matches found for Piece: " << _pairMatchesByPiece[pieceNumber].size() << vcl_endl;
  vcl_cout << "Match Number for Piece: " << count +1<< vcl_endl;
  vcl_cout << "Rank of Match: " << rank+1 << vcl_endl;
  vcl_cout << "--------------" << vcl_endl;
  vcl_cout << "Piece " << p1+1 << " matched with Piece " << p2+1 <<vcl_endl;
  vcl_cout << "Distance Measure : " << d1 << "\nWeighted Distance Measure: " << d1*DIST_COEF << vcl_endl;
  vcl_cout << "Length Measure: " << d2 << "\nWeighted Length Measure: " << sqrt(d2)*LENGTH_COEF << vcl_endl;
  vcl_cout << "Diagnostic Measure: " << d3 << "\nWeighted Diagnostic Measure: " << sqrt(d3)*DIAG_COEF << vcl_endl;
  vcl_cout << "Final Cost: " << this_state.cost[index].first<< vcl_endl;
  vcl_cout << "Overlap: " << olap << vcl_endl;
  vcl_cout << vcl_endl;  

  ////////////deleteAll();
  initialCheck(c1, c2, this_state.map(index));
  //MSGOUT(0);
  
  
}


///Function that categorizes the matches made according to pair.
//The resulting data structure is a numPiece by numPiece matrix whose
//elements consist of vectors of int's.  These int's are the ranks of the 
//pairwise matches which correspond to the pair matching the the coordinates
//of the element in the matrix.

void PuzzleSolving::categorizeMatchesByPair(searchState this_state){
  int numPiece = this_state.numPiece();
  int numMatch = this_state.numMatch();
  int pieceCntOuter, pieceCntInner;
  vcl_vector<int> pairs;
  vcl_vector<vcl_vector <int> > temp;
    _matchesByPair.clear();
  //Sort the pairwise matches by cost.
  indexedMeasures sorted_cost = this_state.cost;
    vcl_sort(sorted_cost.begin(),sorted_cost.end(),cost_ind_less());

  //Cycles through all possible combinations of pieceCntOuter and
  //pieceCntInner.  When they are equal, a vector of a single 0 is pushed back
  //onto that location.  When they are not, an ordered list of all the
  //matches of the pair corresponding to that location in the matrix
  //is put in place.

  for (pieceCntOuter =0; pieceCntOuter<numPiece; pieceCntOuter++){
    for (pieceCntInner = 0; pieceCntInner<numPiece; pieceCntInner++){
      if (pieceCntInner ==pieceCntOuter)
        pairs.push_back(0);

      else {
      for (int matchRank =0; matchRank < numMatch; matchRank++){
      
        int index = sorted_cost[matchRank].second;
        if ((this_state.p1(index)==pieceCntOuter)&&(this_state.p2(index)==pieceCntInner)||
          (this_state.p1(index)==pieceCntInner)&&(this_state.p2(index)==pieceCntOuter))
        pairs.push_back(matchRank);
        }
        }

      temp.push_back(pairs);
      pairs.clear();
      }

    
    _matchesByPair.push_back(temp);
    temp.clear();
  }
}

//Displays matches grouped by pair according to the parameters given
void PuzzleSolving::dispByPair(searchState this_state, int firstIndex, int secondIndex, int count) {

  if(this_state.numMatch()==0)
    return;

  //DISPLAY_GRID_ROWS = 1;
  //DISPLAY_GRID_COLS = 1; 


  if (_matchesByPair[firstIndex][secondIndex].size()==0){
    vcl_cout <<"\n\n*******************************\n" << vcl_endl;
    vcl_cout <<"No matches for this pair!!!" << vcl_endl;
    vcl_cout <<"\n\n*******************************\n" << vcl_endl;
    //MSGOUT(0);
    return;
    }

  int rank=_matchesByPair[firstIndex][secondIndex][count];

  indexedMeasures sorted_cost = this_state.cost;
  vcl_sort(sorted_cost.begin(),sorted_cost.end(),cost_ind_less());


  int index = sorted_cost[rank].second;

  int p1=this_state.p1(index);
  int p2=this_state.p2(index);
  
  
  Curve<double,double>* c1 = this_state.piece(p1);
  Curve<double,double>* c2 = this_state.piece(p2);
  
  regContour(c1,c2,this_state.map(index));
  
  this_state.process.push_back(p2);
  this_state.process.push_back(p1);
  this_state.nProcess=2;
  
  double d1,d2,d3, olap;
  
  this_state.merge(c2);
  localReg(c1,c2,&d1,&d2,&d3,1);
  olap = detectOverlap(c1,c2);
  this_state.merge(c1);
  

  //Data output code
  vcl_cout << "Pair of Piece " << firstIndex +1<< " with Piece " << secondIndex+1 << vcl_endl;
  if (count==0) 
    vcl_cout << "Number of Matches found for Pair: " << _matchesByPair[firstIndex][secondIndex].size() << vcl_endl;
  vcl_cout << "Match Number for Pair: " << count+1 << vcl_endl;
  vcl_cout << "Rank of Match: " << rank+1 << vcl_endl;
  vcl_cout << "--------------" << vcl_endl;
  vcl_cout << "Distance Measure : " << d1 << "\nWeighted Distance Measure: " << d1*DIST_COEF << vcl_endl;
  vcl_cout << "Length Measure: " << d2 << "\nWeighted Length Measure: " << sqrt(d2)*LENGTH_COEF << vcl_endl;
  vcl_cout << "Diagnostic Measure: " << d3 << "\nWeighted Diagnostic Measure: " << sqrt(d3)*DIAG_COEF << vcl_endl;
  vcl_cout << "Final Cost: " << this_state.cost[index].first<< vcl_endl;
  vcl_cout << "Overlap: " << olap << vcl_endl;
  vcl_cout << vcl_endl;  

  ////////////deleteAll();
  initialCheck(c1, c2, this_state.map(index));
  //MSGOUT(0);
  
  
}



int PuzzleSolving::returnIndex(searchState this_state, int firstIndex, int secondIndex, int count){

  if (_matchesByPair[firstIndex][secondIndex].size()==0)
    return 0;

  int rank=_matchesByPair[firstIndex][secondIndex][count];

    indexedMeasures sorted_cost = this_state.cost;
    vcl_sort(sorted_cost.begin(),sorted_cost.end(),cost_ind_less());
  int index = sorted_cost[rank].second;
   return index;
}

//Returns the size of the vector of integers in a given location
//in the 2x2 matrix.
int PuzzleSolving::pairMatchSize(int firstIndex, int secondIndex){

  return _matchesByPair[firstIndex][secondIndex].size();
}
*/


//Characterize Curve function:

void PuzzleSolving::characterizeCurves(){

  vcl_vector<double> totals, temp;
  for (int j=0; j <4; j++){
    totals.push_back(0);
    temp.push_back(0);
    }

  if (_Contours.size()>0){
    vcl_cout << "Individual Curve Information:" << vcl_endl;
    vcl_cout << "------------------------------------" <<vcl_endl;
    
    
    for(unsigned i=0; i< _Contours.size(); i++){
        _Contours[i].computeProperties();
        temp = curveInfo(i);
        totals[0] += temp[0];
        totals[1] += temp[1];
        totals[2] += temp[2];
        totals[3] += temp[3];
      }

    vcl_cout << "\nGeneral *.puz File Information:" << vcl_endl;
    vcl_cout << "-------------------------------" << vcl_endl;
    vcl_cout << "Number of contours: " << _Contours.size() << "\n" << vcl_endl;
    vcl_cout << "Total Number of Points in *.puz file: " << totals[0] << vcl_endl;
    vcl_cout << "Total Length of Contours in *.puz file: " << totals[1] << vcl_endl;
    vcl_cout << "Average Length per Point: " << totals[1]/totals[0] << vcl_endl;
    vcl_cout << "Average Number of Points per Unit arclength: " << totals[0]/totals[1] << vcl_endl;
    vcl_cout << "Total Absolute Value of Angle Rotation: " << totals[2] << vcl_endl;
    vcl_cout << "Average Absolute Value of Angle Change (per Point): " << totals[2]/totals[0] << vcl_endl;
    vcl_cout << "Average Absolute Value of Angle Change (per unit arclength): " << totals[2]/totals[1] << vcl_endl;
    vcl_cout << "Total Absolute Angle Change Difference : " << totals[3] << vcl_endl;
    vcl_cout << "Average Absolute Value of Angle Change Difference (per Point): " << totals[3]/totals[0] << vcl_endl;
    vcl_cout << "Average Absolute Value of Angle Change Difference (per unit arclength): " << totals[3]/totals[1] << vcl_endl;
    vcl_cout << vcl_endl <<vcl_endl;
    //MSGOUT(0);

    }
  }


//Function for printing out various items of information about the curves contained in the
//specified *.puz file.
//First element: number of points
//Second element: length of contour
//Third element: absolute value of total angle rotated through
//Fourth element: absolute value of total angle change difference from point to point.
vcl_vector<double> PuzzleSolving::curveInfo(int i){
    vcl_cout << "Curve " << i+1 << " information:" << vcl_endl;
    vcl_cout << "-------------------------------" << vcl_endl;

    int numPoints = _Contours[i].numPoints();
    vcl_cout << "Number of points: " << numPoints << vcl_endl;

    double length = _Contours[i].totalLength(0, numPoints-1);
    vcl_cout << "Total length of contour: "<< length << vcl_endl;
    

    vcl_cout << "Average Distance per Point: "<< length/((double) numPoints) << vcl_endl;

    double angleTotal = 0.0;
    double angleDiffTotal =0.0;
    double tempAngle=0;
  

    //Calculates the total angle changes over the length of the contour.
    for (int cnt=0; cnt<numPoints; cnt++){
        angleTotal += fabs(_Contours[i].angle(cnt));
        angleDiffTotal += fabs(_Contours[i].angle(cnt)-tempAngle);
        tempAngle = _Contours[i].angle(cnt);
      }

    vcl_cout << "Average absolute Angle Change: " << angleTotal/(double)numPoints << vcl_endl;
    vcl_cout << "Total Absolute Angle Change: " << angleTotal << vcl_endl;
    vcl_cout << "Average Absolute Angle Change Difference from one point to the next: " << angleDiffTotal / (double) numPoints<<vcl_endl;
    vcl_cout << "Total Absolute Angle Change Difference from one point to the next: " << angleDiffTotal << vcl_endl;

    vcl_cout <<vcl_endl;
     //MSGOUT(0);

    vcl_vector<double> totals;
    totals.push_back((double) numPoints);
    totals.push_back(length);
    totals.push_back(angleTotal);
    totals.push_back(angleDiffTotal);

  return totals;

  }

vcl_vector<searchState> PuzzleSolving::search(vcl_vector<searchState> all_states) {  
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    int i,k;
#endif
  
    //Option for continuous searching- button press not necessary
    //if (CONTINUOUS_SEARCH){
    //XXX
    if(1){
        

        //while(all_states[0].nProcess < _nPieces){

            if(all_states[0].nProcess == _nPieces){
                return all_states;
            }
    
            //startTicksPhase = clock();
            vcl_cout << "Working..." << vcl_endl;
    
            vcl_vector<searchState> all_states2;
            indexedMeasures tCosts;
    
            for( unsigned i = 0; i < all_states.size(); i++) {
                if( all_states[i].active ){
                    vcl_vector<searchState> top_state = iterateSearch(all_states[i],0);
    
                    //Drawing states found with their costs
                    for (unsigned a=0; a <top_state.size(); a++){
                        //drawState(&top_state[a],a);
                     vcl_cout << a << ": tCost: " << top_state[a].tCost << vcl_endl;
                    }
        
                    double min_cost=0.0;
                    double sc;
                    for(unsigned k=0;k<top_state.size();k++) {
                      sc=top_state[k].sCost;
                      tCosts.push_back(vcl_pair<double,int>(top_state[k].tCost,all_states2.size()));
                      all_states2.push_back( top_state[k] );
                         if( sc < min_cost ){
                             min_cost = sc;
                         }
                    } 
                }
            }
    
            if( all_states2.size() == 0 ){
                return all_states;
            }
    
            vcl_sort(tCosts.begin(),tCosts.end(),cost_ind_less());
            all_states.clear();
    
    
            int add;
            int index;
            double top=0.0;
            for( unsigned i = 0; i < all_states2.size(); i++) {
    
                index=tCosts[i].second;
                add=1;
                if(all_states.size()>0) {
                    for(unsigned k=0;k<all_states.size();k++) {
                        if(all_states2[index]==all_states[k]) {
                            all_states[k].tCost-= IDENTICAL_BONUS;
                            add=0;
                            break;
                        }
                    }
                } else {
                    top=all_states2[index].tCost;
                }
              
                if( add == 1 ){
                    if(all_states2[index].tCost-top<CULLING_RANGE){
                        all_states.push_back(all_states2[index]);
                    }
                }
                if( all_states.size() == NUM_TOP_STATES && all_states[0].nProcess > 2 ){
                    break;
                }
            }


            all_states2.clear();
            tCosts.clear();
          
            if( all_states[0].nProcess > 2 ){
              
                int num_disp=NUM_TOP_STATES;
                     int all_states_size_int = all_states.size();
                if((all_states_size_int<num_disp) && !(all_states_size_int<0)){
                    num_disp=all_states.size();
                }

                for( int z = 0; z < num_disp; z++) {
                    all_states[z].structure();
                    //drawState(&all_states[i],i);
                    vcl_cout <<"tCost: "<< all_states[z].tCost << vcl_endl;
                    vcl_cout <<"sCost: "<< all_states[z].sCost << vcl_endl;
                }

                vcl_cout << "Completed Phase " << all_states[0].nProcess << vcl_endl;

            }
        //}
    } else {
        
        //Ming:
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
        int sz = all_states.size();
        int nn = _nPieces;
#endif
        /*  //why is this done? Looks like it will return here every time
        if (all_states.size()!=0){
            vcl_cout << "puzzleSolving::search ABORTING" << vcl_endl;
            return NULL;
        }
        */

        if( all_states[0].nProcess == _nPieces ){
            return all_states;
        }
        
        vcl_cout << "Working..." << vcl_endl;

        vcl_vector<searchState> all_states2;
        indexedMeasures tCosts;

        for(unsigned i = 0; i < all_states.size(); i++ ){
            if( all_states[i].active ){
                vcl_vector<searchState> top_state = iterateSearch(all_states[i],0);
  
                double min_cost=0.0;
                double sc;
                for(unsigned k = 0; k < top_state.size(); k++ ){
                    sc=top_state[k].sCost;
                    tCosts.push_back(vcl_pair<double,int>(top_state[k].tCost,all_states2.size()));
                    all_states2.push_back(top_state[k]);
                    if( sc < min_cost ){
                        min_cost=sc;
                    }
                }
            }
        }


        if( all_states2.size() == 0 ) {
            return all_states;
        }

        vcl_sort(tCosts.begin(),tCosts.end(),cost_ind_less());
        all_states.clear();


        int add;
        int index;
        double top;
        for(unsigned i=0;i<all_states2.size();i++) {
            index=tCosts[i].second;
            add=1;
            if(all_states.size()>0) {
                for(unsigned k=0;k<all_states.size();k++) {
                    if(all_states2[index]==all_states[k]) {
                        all_states[k].tCost-=IDENTICAL_BONUS;
                        add=0;
                        break;
                    }
                }
            } else {
                top=all_states2[index].tCost;
            }


            if(add == 1 && all_states2[index].tCost-top < CULLING_RANGE){
                all_states.push_back(all_states2[index]);
            }
            if(all_states.size()==NUM_TOP_STATES && all_states[0].nProcess>2) {
                break;
            }
        }

        all_states2.clear();
        tCosts.clear();

        if(all_states[0].nProcess>2) {
  
            unsigned num_disp=NUM_TOP_STATES;
            if( all_states.size() < num_disp ){
                num_disp=all_states.size();
            }
  
            for( unsigned i = 0; i < num_disp; i++ ){
              all_states[i].structure();
              //drawState(&all_states[i],i);
              vcl_cout << "tCost: "<< all_states[i].tCost << " sCost: " <<  all_states[i].sCost << vcl_endl;
            }

            vcl_cout << "Completed Phase " << all_states[0].nProcess << vcl_endl;

        }

    }

    return all_states;
}



//***************************************************************************************
//Edge Continuity Functionality


int PuzzleSolving::initialCheck(Curve<double, double>* c1, Curve<double, double>* c2, intMap map){

  //Initialize variables
  int startIndx1 = map[0].first;
  int startIndx2 = map[0].second;
  int endIndx1 = map[map.size()-1].first;
  int endIndx2 = map[map.size()-1].second;
  int numPts1 = c1->numPoints();
  int numPts2 = c2->numPoints();
  int numberContinuousEdges=0;
  double angle1=0;
  double angle2=0;
  //Boolean variables that indicate whether the map moves in forward or reverse order for each curve
  //bool movesForward1;
  //bool movesForward2;

  //Move the two contours close to one another
  regContour(c1,c2,map);

  c1->computeProperties();
  c2->computeProperties();


    // new displaying of points 

    // new displaying of points 

 
/*
//For displaying points:
  ////////////deleteAll();
  Group *line_node = new Group;
  Group *text_node = new Group;
  localRootNode()->addChild(line_node);
  localRootNode()->addChild(text_node);

  //ostringstream os;
  //os << state->tCost;
  

   drawLines(line_node,c1,BLUE);
   drawLines(line_node,c2,RED);

  PuzPoint<double> j = c1->point(startIndx1);
  PointGeom *junc = new PointGeom(j.x(),j.y());
   junc->loadStyle(Color(BLUE) & PointSize(7));
   line_node->addChild(junc);
  

  //TextGeom* blah = new TextGeom(1,1, "Hello?");
  //blah->loadStyle(Color(BLUE));
  //text_node->addChild(blah);
  


  PuzPoint<double> j2 = c1->point(endIndx1);
  PointGeom *junc2 = new PointGeom(j2.x(),j2.y());
   junc2->loadStyle(Color(BLUE) & PointSize(7));
   line_node->addChild(junc2);

  PuzPoint<double> j3 = c2->point(startIndx2);
  PointGeom *junc3 = new PointGeom(j3.x(),j3.y());
   junc3->loadStyle(Color(ORANGE) & PointSize(7));
   line_node->addChild(junc3);

  PuzPoint<double> j4 = c2->point(endIndx2);
  PointGeom *junc4 = new PointGeom(j4.x(),j4.y());
   junc4->loadStyle(Color(ORANGE) & PointSize(7));
   line_node->addChild(junc4);

  for (int counter = 0; counter < c1->numExtrema(); counter++){
    PuzPoint<double> p1 = c1->point(c1->coarseRef(c1->extrema(counter)));
    PointGeom *corner1 = new PointGeom(p1.x(),p1.y());
    corner1->loadStyle(Color(GREEN) & PointSize(5));
    line_node->addChild(corner1);
    }

  for (int counter2 = 0; counter2 < c2->numExtrema(); counter2++){
    PuzPoint<double> p2 = c2->point(c2->coarseRef(c2->extrema(counter2)));
    PointGeom *corner2 = new PointGeom(p2.x(),p2.y());
    corner2->loadStyle(Color(GREEN) & PointSize(5));
    line_node->addChild(corner2);
    }
        */


  

  vcl_cout << "\n*************************************************************\n" <<vcl_endl;
  vcl_cout << "Number of points for c1: " << c1->numPoints() << vcl_endl;
   vcl_cout << "Number of points for c2: " << c2->numPoints() << vcl_endl;
  vcl_cout << "Start point on curve 1: " << c1->x(startIndx1) << " , " << c1->y(startIndx1) << vcl_endl;
  vcl_cout << "Start point on curve 2: " << c2->x(startIndx2) << " , " << c2->y(startIndx2) << vcl_endl;
  vcl_cout << "End point on curve 1: " << c1->x(endIndx1) << " , " << c1->y(endIndx1) << vcl_endl;
  vcl_cout << "End point on curve 2: " << c2->x(endIndx2) << " , " << c2->y(endIndx2) << vcl_endl;
   //MSGOUT(0);


  //If the intMap goes forward for c1 move back along the contour
  //and find the average of the angles of the tangents over the curve.
  //if (movesForward1){
    int startTestIndx1 = startIndx1 - 25;
    while (startTestIndx1 < 0)
      startTestIndx1 += numPts1;
    if (startTestIndx1 >= numPts1)
      startTestIndx1 %=numPts1;

    for (int i =0; i < 15; i++){
      if (startTestIndx1+i+1>numPts1)
        startTestIndx1=-i;
      angle1 += c1->angle(startTestIndx1+i);

      PuzPoint<double> pnt = c1->point(startTestIndx1+i);
      //PointGeom *mark = new PointGeom(pnt.x(), pnt.y());
      //mark->loadStyle(Color(GREEN) & PointSize(5));
      //line_node->addChild(mark);


    }
    angle1 = angle1/15;
    if (angle1 < -M_PI)
      angle1=2*M_PI-angle2;
    if (angle1 < 0)
      angle1 +=M_PI;
    if (angle1 >= M_PI)
      angle1 -= M_PI;

    PuzPoint<double> startPoint1 = c1->point(startIndx1);
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    double dx1 = cos(angle1)*60;
    double dy1 = sin(angle1)*60;
#endif
        /*
    LineGeom *xx1 = new LineGeom(startPoint1.x()-dx1, startPoint1.y()-dy1,
      startPoint1.x()+dx1, startPoint1.y()+dy1);
    line_node->addChild(xx1);
    xx1->loadStyle(Color(BLUE) & LineWidth(2));
        */

    vcl_cout << "Angle 1: (Start End) " << angle1 << vcl_endl;
    //MSGOUT(0);




    int startTestIndx2 = startIndx2+25;
    if (startTestIndx2 >= numPts2)
      startTestIndx2 %= numPts2;

    for (int k =0; k >= -14; k--){
      if (startTestIndx2+k < 0)
        startTestIndx2 = numPts2-k-1;
      angle2 += c2->angle(startTestIndx2+k);

        PuzPoint<double> pnt2 = c2->point(startTestIndx2+k);
                /*
      PointGeom *mark2 = new PointGeom(pnt2.x(), pnt2.y());
      mark2->loadStyle(Color(GREEN) & PointSize(5));
      line_node->addChild(mark2);
            */

      }
    angle2 = angle2/15;
    if (angle2 < -M_PI)
      angle2=2*M_PI-angle2;
    if (angle2 < 0)
      angle2 +=M_PI;
    if (angle2 >= M_PI)
      angle2 -= M_PI;

    PuzPoint<double> startPoint2 = c2->point(startIndx2);
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    double dx = cos(angle2)*60;
    double dy = sin(angle2)*60;
#endif
        /*
    LineGeom *xx2 = new LineGeom(startPoint2.x()-dx, startPoint2.y()-dy,
      startPoint2.x()+dx, startPoint2.y()+dy);
    line_node->addChild(xx2);
    xx2->loadStyle(Color(ORANGE) & LineWidth(2));
        */

    vcl_cout << "Angle 2: (Start End) " << angle2 << vcl_endl;
    //MSGOUT(0);


if ((anglesAreClose(angle1, angle2))&&(distanceCheck(c1, c2, map[0].first, map[0].second))){

   vcl_cout << "*****************************" << vcl_endl;
   vcl_cout <<  "Start side has edge continuity" << vcl_endl;
   vcl_cout << "*****************************" << vcl_endl;
   //MSGOUT(0);
   numberContinuousEdges++;
    }

if (!distanceCheck(c1, c2, map[0].first, map[0].second)){
  vcl_cout << "Distance check 1 failed!" << vcl_endl;
  //MSGOUT(0);
  }

 angle1 = 0;
 angle2 = 0;



 
    int endTestIndx1 = endIndx1+25;
    if (endTestIndx1 >= numPts1)
      endTestIndx1 %= numPts1;

    for (int i2 =0; i2 >= -14; i2--){
      if (endTestIndx1+i2 < 0)
        endTestIndx1 = numPts1-i2-1;
      angle1 += c1->angle(endTestIndx1+i2);


      PuzPoint<double> pnt = c1->point(endTestIndx1+i2);
            /*
      PointGeom *mark = new PointGeom(pnt.x(), pnt.y());
      mark->loadStyle(Color(RED) & PointSize(5));
      line_node->addChild(mark);
            */

      }

    angle1 = angle1/15;
    if (angle1 < -M_PI)
      angle1=2*M_PI-angle2;
    if (angle1 < 0)
      angle1 +=M_PI;
    if (angle1 >= M_PI)
      angle1 -= M_PI;

    PuzPoint<double> endPoint1 = c1->point(endIndx1);
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    double dx2 = cos(angle1)*60;
    double dy2 = sin(angle1)*60;
#endif
        /*
    LineGeom *xx3 = new LineGeom(endPoint1.x()-dx2, endPoint1.y()-dy2,
      endPoint1.x()+dx2, endPoint1.y()+dy2);
    line_node->addChild(xx3);
    xx3->loadStyle(Color(BLUE) & LineWidth(2));
        */

    vcl_cout << "Angle 1: (End end) " << angle1 << vcl_endl;
    //MSGOUT(0);


    int endTestIndx2 = endIndx2 - 25;
    while (endTestIndx2 < 0)
      endTestIndx2 += numPts2;
    if (endTestIndx2 >= numPts2)
      endTestIndx2 %=numPts2;

    for (int k2 =0; k2 < 15; k2++){
      if (endTestIndx2+k2+1>numPts2)
        endTestIndx2=0-k2;
      angle2 += c2->angle(endTestIndx2+k2);

      PuzPoint<double> pnt2 = c2->point(endTestIndx2+k2);
            /*
      PointGeom *mark2 = new PointGeom(pnt2.x(), pnt2.y());
      mark2->loadStyle(Color(RED) & PointSize(5));
      line_node->addChild(mark2);
            */

    }
    angle2 = angle2/15;
    if (angle2 < -M_PI)
      angle2=2*M_PI-angle2;
    if (angle2 < 0)
      angle2 +=M_PI;
    if (angle2 >= M_PI)
      angle2 -= M_PI;

    PuzPoint<double> endPoint2 = c2->point(endIndx2);
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    double dx3 = cos(angle2)*60;
    double dy3 = sin(angle2)*60;
#endif
        /*
    LineGeom *xx4 = new LineGeom(endPoint2.x()-dx3, endPoint2.y()-dy3,
      endPoint2.x()+dx3, endPoint2.y()+dy3);
    line_node->addChild(xx4);
    xx4->loadStyle(Color(ORANGE) & LineWidth(2));
        */

    vcl_cout << "Angle 2: (End end) " << angle2 << vcl_endl;
    //MSGOUT(0);


if ((anglesAreClose(angle1, angle2))&&(distanceCheck(c1, c2, map[map.size()-1].first, map[map.size()-1].second))){

   vcl_cout << "*****************************" << vcl_endl;
   vcl_cout <<  "End side has edge continuity" << vcl_endl;
   vcl_cout << "*****************************" << vcl_endl;
   //MSGOUT(0);
   numberContinuousEdges++;
   }

if (!distanceCheck(c1, c2, map[map.size()-1].first, map[map.size()-1].second)){
  vcl_cout << "Distance check 2 failed!" << vcl_endl;
  //MSGOUT(0);
  }


 if (numberContinuousEdges==0)
  vcl_cout << "No good edge continuity" << vcl_endl;
 //MSGOUT(0);
 //line_node->multXForm(autoScale(line_node,GRID_BORDER,GRID_BORDER,SCREEN_SIZE));
 return numberContinuousEdges;

}




vcl_vector<searchState> 
PuzzleSolving::iterateSearch(searchState this_state, int f) {
  
  int i,index;
  
  bool flag=0;
  if(f==1) flag=1;
  
  searchState temp_state;
  vcl_vector<searchState> states;

  //In the current configuration, flag is never 1.
  if(flag)
    this_state.comboMatch();
  
  //Sort the pairwise matchces
  //indexedMeasures sorted_cost = this_state.cost;
  //vcl_sort(sorted_cost.begin(),sorted_cost.end(),cost_ind_less());
  //FORK: sort _matches
  this_state.sortPairwiseMatches();
   
  //FORK this will copy everything->its been tested->no pointer problems (w/ my PairWiseMatch class)
  temp_state = this_state;
  /*
      //FORK - copy constructor test{
        this_state.sanityCheckMatchesByIndexNPS();
        temp_state.sanityCheckMatchesByIndexNPS();
        vcl_cout << "in ITERATE SEARCH: Testing copy constructor of searchState: " << vcl_endl;
        for( int i = 0; i < temp_state._matches.size(); i++ ){
            vcl_cout << i;
            if( &(temp_state._matches[i]) == &(this_state._matches[i]) ) {
                vcl_cout << " FAILED: address of both matches are same" << vcl_endl;
                assert(0);
            }

            vcl_cout << ":passed ";
        }
        vcl_cout << vcl_endl;
       
      //}
      */

  //If there have not yet been any pieces added:
  if(this_state.nProcess==0) { 

   

     //FORK
    //int num=sorted_cost.size();
    int num = this_state._matches.size();
    //assert( num == sorted_cost.size() );

    if(num>2*NUM_TOP_STATES) num=2*NUM_TOP_STATES;
    
    for(i=0;i<num;i++) {
    
    //Refreshing...
    //GetGlobalPluginManager[]->GetPluginFromName("PUZZLESOLVING")->GetPluginWindow
    //wxGetApp()->Update();

      //FORK
      //index=sorted_cost[i].second;
      //int p1=this_state.p1(index);
      //int p2=this_state.p2(index);
      index = this_state._matches[i].myIndex;
      //assert( index == sorted_cost[i].second );
      int p1 = this_state._matches[i].whichCurves.first;
      int p2 = this_state._matches[i].whichCurves.second;
      //assert( p1 == this_state.p1(index) );
      //assert( p2 == this_state.p2(index) );
      
      
      Curve<double,double>* c1 = this_state.piece(p1);
      Curve<double,double>* c2 = this_state.piece(p2);
      
      //FORK
      //regContour(c1,c2,this_state.map(index));
      regContour( c1, c2, this_state._matches[i].pointMap );
      //to check: run the sanity test
      //this_state.sanityCheckMatchesByIndexNPS(); //DEBUG

      this_state.process.push_back(p2);
      this_state.process.push_back(p1);
      this_state.nProcess = 2;
      
      double d1,d2,d3;

      intMap tmap;

      //i don't think this affects FORK direclty
    //Add piece c2 to the puzzle.
      this_state.merge(c2);
    //this_state.merged()->computeProperties();


      //FORK{
    //Add piece c1 to the puzzle.
      tmap = localReg(c1,this_state.merged(),&d1,&d2,&d3,LOCAL_REG_ITER_PUZ);
    //double costReductionORIG = SMOOTH_EDGE_BONUS*edgeCheck(c1, c2, this_state.map(index));
      
    double costReduction = SMOOTH_EDGE_BONUS*edgeCheck(c1, c2, this_state._matches[i].pointMap );
      //assert( costReductionORIG == costReduction );
      //this_state.sanityCheckMatchesByIndexNPS(); //DEBUG
      //}


      this_state.merge(c1);
    //this_state.merged()->computeProperties();
  
      
      int l=tmap.size();

    //Find junction points
      PuzPoint<double> pt1=c1->point(tmap[0].first);
      PuzPoint<double> pt2=c2->point(tmap[0].second);
      PuzPoint<double> pt3=c1->point(tmap[l-1].first);
      PuzPoint<double> pt4=c2->point(tmap[l-1].second);
      PuzPoint<double> j1((pt1.x()+pt2.x())/2.0,(pt1.y()+pt2.y())/2.0);
      PuzPoint<double> j2((pt3.x()+pt4.x())/2.0,(pt3.y()+pt4.y())/2.0);

    
    //Add the junction points to the list of junction points
      vcl_vector<int> jj;
      jj.push_back(p1);
      jj.push_back(p2);
      this_state.open_junc.push_back(vcl_pair<vcl_vector<int>,PuzPoint<double> >(jj,j1));
      this_state.open_junc.push_back(vcl_pair<vcl_vector<int>,PuzPoint<double> >(jj,j2));

    //Update the tCost and sCost
      this_state.tCost+=DIST_COEF*d1+LENGTH_COEF*sqrt(d2)+DIAG_COEF*sqrt(d3)-costReduction;
    
      this_state.sCost=DIST_COEF*d1+LENGTH_COEF*sqrt(d2)+DIAG_COEF*sqrt(d3)-costReduction;
      
    //Categorize this match as a new edge.
      this_state.new_edge=vcl_pair<int,int>(p1,p2);

    //Check to make sure that the match satisfies the minimum diagnostic threshold.
      if(d3>DIAG_THRESH_LOW){
          states.push_back(this_state);
      }
      this_state = temp_state;
      

      /*
      //FORK - copy constructor test{
        this_state.sanityCheckMatchesByIndexNPS();
        temp_state.sanityCheckMatchesByIndexNPS();
        vcl_cout << "in ITERATE SEARCH: Testing copy constructor of searchState: " << vcl_endl;
        for( int i = 0; i < temp_state._matches.size(); i++ ){
            vcl_cout << i;
            if( &(temp_state._matches[i]) == &(this_state._matches[i]) ) {
                vcl_cout << " FAILED: address of both matches are same" << vcl_endl;
                assert(0);
            }
            
            
            vcl_cout << ":passed ";
        }
        vcl_cout << vcl_endl;
      //}
        */
    }
  }
  
  else { //else for the "if (state.nprocess == 0)"
    
  
    int num_good=0;
    for(i=0;i<this_state.numMatch();i++) { // main loop

      //this_state.sanityCheckMatchesByIndexNPS();
      //temp_state.sanityCheckMatchesByIndexNPS();
      
     //Refreshing...
     //wxApp::GetTopWindow()->Update();
     //wxGetApp()->Update();

      //FORK
      double matchescost = this_state._matches[i].cost;
      if( matchescost == DP_VERY_LARGE_COST ){
          //its ok, theyre gunna be skipped anyways
          break;
      }
      /*
      //FORK
      int oldindex = sorted_cost[i].second;
      index = this_state._matches[i].myIndex;
      if( index != oldindex ){
          double matchescost = this_state._matches[i].cost;
          double sortedscost = sorted_cost[i].first;
          if( matchescost == DP_VERY_LARGE_COST &&
              sortedscost == DP_VERY_LARGE_COST ){
              //its ok, theyre gunna be skipped anyways
              break;
          } else {

              this_state.printPairwiseMatchesNPS();
              //also print sorted_cost[i].second
              for( int i = 0; i < sorted_cost.size(); i++ ){
                  vcl_cout << "sorted cost of (" << i << ") cost:[" << sorted_cost[i].first << "] index:[" << sorted_cost[i].second << "]" << vcl_endl;
              }
              assert(0);
          }
      }
      */
      
      //FORK{
      //int p1=this_state.p1(index);
      //int p2=this_state.p2(index);
      int p1 = this_state._matches[i].whichCurves.first;
      int p2 = this_state._matches[i].whichCurves.second;
      //assert( p1 == this_state.p1(index) );
      //assert( p2 == this_state.p2(index) );
      //}

      Curve<double,double>* c1;
      Curve<double,double>* c2;

      int chk1=1;
      int chk2=1;
      vcl_vector<int> proc = this_state.process;

    //Make sure that the pieces that we are dealing with have not
    //yet been added to the puzzle.
      if(vcl_find(proc.begin(),proc.end(),p1)==proc.end())
    chk1=0;
      if(vcl_find(proc.begin(),proc.end(),p2)==proc.end())
    chk2=0;
      if(p2>=_nPieces)
    chk2=1;

      int add,ext;
      
      //FORK
      //if(this_state.cost[index].first==DP_VERY_LARGE_COST){
      //    break;
      //}
      //assert( this_state._matches[i].cost == this_state.cost[index].first );
      if( this_state._matches[i].cost == DP_VERY_LARGE_COST ){
          //this_state.sanityCheckMatchesByIndexNPS();//DEBUG
          //temp_state.sanityCheckMatchesByIndexNPS();//DEBUG
          break;
      }
    
      //FORK{
    //If we are dealing with a match whose first piece has been added
    //and whose second piece has been added or has an index
    //greater than or equal to the number of pieces then we set the
    //cost to be very high.
      //if(chk1==1 && chk2==1) {
    //  this_state.cost[index].first=DP_VERY_LARGE_COST;
      //  temp_state.cost[index].first=DP_VERY_LARGE_COST;
      //}
      //this_state.sanityCheckMatchesByIndexNPS();
      //temp_state.sanityCheckMatchesByIndexNPS();
      if(chk1==1 && chk2==1) {
          //OLD
          //this_state.cost[index].first=DP_VERY_LARGE_COST;
          //temp_state.cost[index].first=DP_VERY_LARGE_COST;
          //NEW
          this_state._matches[i].cost = DP_VERY_LARGE_COST;
          temp_state._matches[i].cost = DP_VERY_LARGE_COST;

          //this_state.sanityCheckMatchesByIndexNPS();
          //temp_state.sanityCheckMatchesByIndexNPS();
      }
      

          

      //}

    //Otherwise if the values of chk1 and 2 are different
    //and (p2>=_nPieces)==flag is 1.
      else if(chk1 != chk2 && (p2>=_nPieces)==flag) {

  //if(p2>=_nPieces) {
  //  //vcl_cout << "New Match!" << vcl_endl;
  //  p2%=_nPieces;
  //  c1=this_state.piece(p1);
  //  c2=this_state.constr(p2);
  //  add=p1;
        //
  //  regContour(c1,c2,this_state.map(index));
  //}

    //FORK
  intMap fmap; //, fmapOLD;
  Curve<double, double> *c1EdgeCheck=this_state.piece(p1);
  Curve<double, double> *c2EdgeCheck=this_state.piece(p2);
  
  //Check to see which piece has been added already
   if(chk1==0) {
    add=p1;
    ext=p2;
        //FORK{
    //fmap=this_state.map(index); 
        fmap = this_state._matches[i].pointMap;
        //fmapOLD = this_state.map(index);
        //assert( fmap == fmapOLD && &fmap != &fmapOLD );
        //}
  }
  else {
    add=p2;
    ext=p1;
      //FORK{
    //fmap=this_state.map(index);
      fmap = this_state._matches[i].pointMap;
      //fmapOLD = this_state.map(index);
      //assert( fmap == fmapOLD && &fmap != &fmapOLD );
      //}

    //Swap the order of the matches to make the indices from p2 be the first
    //and from p1 to be the second.
      for(unsigned cnt=0;cnt<fmap.size();cnt++){
        //FORK
        //fmapOLD[cnt]=vcl_pair<int,int>(fmapOLD[cnt].second,fmapOLD[cnt].first);
        fmap[cnt]=vcl_pair<int,int>(fmap[cnt].second,fmap[cnt].first);
      }
  }

  c2=this_state.piece(ext);
  

  double dist_thresh=10.0;

  bool triple_cond = 0;
  bool cont=1;

  //See if adding the next piece will line up with at least one junction already present
  if(this_state.nProcess==2) {
    cont=0;
    PuzPoint<double> pe1=this_state.open_junc[0].second;
    PuzPoint<double> pe2=this_state.open_junc[1].second;
    PuzPoint<double> pn1=c2->point(fmap[0].second);
    PuzPoint<double> pn2=c2->point(fmap[fmap.size()-1].second);
    if(pointDist(pe1,pn1)<dist_thresh) cont=1;
    if(pointDist(pe1,pn2)<dist_thresh) cont=1;
    if(pointDist(pe2,pn1)<dist_thresh) cont=1;
    if(pointDist(pe2,pn2)<dist_thresh) cont=1;
  }
  
  //If so, then proceed.
  if(cont) {
        //FORK -- regContour makes a copy of fmap, it wont change it, this is OK
    c1=this_state.piece(add);
    regContour(c1,c2,fmap);
      //FORK
    //regContour(c1EdgeCheck, c2EdgeCheck, this_state.map(index));
      regContour(c1EdgeCheck, c2EdgeCheck, this_state._matches[i].pointMap );
      //this_state.sanityCheckMatchesByIndexNPS();
      //temp_state.sanityCheckMatchesByIndexNPS();

      //FORK{
    //double costReduction2OLD = SMOOTH_EDGE_BONUS*edgeCheck(c1EdgeCheck, c2EdgeCheck, this_state.map(index));
      double costReduction2 = SMOOTH_EDGE_BONUS*edgeCheck(c1EdgeCheck, c2EdgeCheck, this_state._matches[i].pointMap );
      //assert( costReduction2OLD == costReduction2 );
      //}
    
    double olap=0.0;
    double ol;
    for(int pp=0;pp<this_state.nProcess;pp++) {
      ol = detectOverlap(c1,this_state.piece(this_state.process[pp]));
      if(ol>olap)
        olap=ol;
    }

    //FORK{
    if(olap>OLAP_THRESH_LOW) {
      //this_state.cost[index].first=DP_VERY_LARGE_COST;
      //temp_state.cost[index].first=DP_VERY_LARGE_COST;
        //NEW
        this_state._matches[i].cost = DP_VERY_LARGE_COST;
        temp_state._matches[i].cost = DP_VERY_LARGE_COST;
        //assert( this_state.cost[index].first == this_state._matches[i].cost &&
        //        temp_state.cost[index].first == temp_state._matches[i].cost );

        //DEBUG
        //this_state.sanityCheckMatchesByIndexNPS();
        //temp_state.sanityCheckMatchesByIndexNPS();
    } else {
      
      
      //vcl_cout << "-" << vcl_endl;
      double dist_meas,leng_meas,diag_meas;
      
      //long int startTicks = clock(); 
      localReg(c1,this_state.merged(),&dist_meas,&leng_meas,&diag_meas,LOCAL_REG_ITER_PUZ);
      
      vcl_vector<vcl_pair<int,PuzPoint<double> > > newj;
      
    //Add the new edge to the list.
      this_state.old_edges.push_back(this_state.new_edge);

    //Add the new edges to the list of old edges.
      for(unsigned cnt=0;cnt<this_state.new_edges.size();cnt++)
        this_state.old_edges.push_back(this_state.new_edges[cnt]);
      this_state.new_edges.clear();
      this_state.new_edge=vcl_pair<int,int>(p1,p2);

   

      for(int pc=0;pc<this_state.nProcess;pc++) {
        double d1,d2,d3;
        int pp=this_state.process[pc];
        Curve<double,double> ct = *c1;
        Curve<double,double> *c2a = this_state.piece(pp);
        intMap tmap;
        
        tmap=localReg(&ct,c2a,&d1,&d2,&d3,1);



        if(d2/d1>5.0) {
        if(pp!=p1 && pp!=p2)
          this_state.new_edges.push_back(vcl_pair<int,int>(pp,add));
        int l=tmap.size();
        
        //Take care of junctions again  
        PuzPoint<double> pt1=c1->point(tmap[0].first);
        PuzPoint<double> pt2=c2a->point(tmap[0].second);
        PuzPoint<double> pt3=c1->point(tmap[l-1].first);
        PuzPoint<double> pt4=c2a->point(tmap[l-1].second);
        PuzPoint<double> j1((pt1.x()+pt2.x())/2.0,(pt1.y()+pt2.y())/2.0);
        PuzPoint<double> j2((pt3.x()+pt4.x())/2.0,(pt3.y()+pt4.y())/2.0);
        newj.push_back(vcl_pair<int,PuzPoint<double> >(pp,j1));
        newj.push_back(vcl_pair<int,PuzPoint<double> >(pp,j2));
        }
      }
      
      for(unsigned nj=0;nj<newj.size();nj++) {
        PuzPoint<double> pn = newj[nj].second;
        bool addj=1;
        
        for(unsigned oj=0;oj<this_state.open_junc.size();oj++) {
    
        PuzPoint<double> po = this_state.open_junc[oj].second;
        if(pointDist(po,pn)<40.0) {
        addj=0;
        vcl_vector<int> other = this_state.open_junc[oj].first;
        int l=other.size();
        PuzPoint<double> pa((po.x()*(double)(l-1)+pn.x())/(double)l,
                (po.y()*(double)(l-1)+pn.y())/(double)l);
        if(vcl_find(other.begin(),other.end(),add)==other.end()) {
          this_state.open_junc[oj].first.push_back(add);
          this_state.open_junc[oj].second=pa;
        }
        else{
          this_state.open_junc[oj].second=pa;
          this_state.closed_junc.push_back(this_state.open_junc[oj]);
          vcl_vector<vcl_pair<vcl_vector<int>,PuzPoint<double> > > temp_junc;
          for(unsigned pop=0;pop<oj;pop++)
            temp_junc.push_back(this_state.open_junc[pop]);
          for(unsigned pop2=oj+1;pop2<this_state.open_junc.size();pop2++)
            temp_junc.push_back(this_state.open_junc[pop2]);
          this_state.open_junc=temp_junc;
          this_state.tCost -= CLOSED_JUNCTION_BONUS;
          triple_cond = 1;
      }
      break;
    }
        }
        if(addj) {
    vcl_vector<int> jj;
    jj.push_back(newj[nj].first);
    jj.push_back(add);
    this_state.open_junc.push_back(vcl_pair<vcl_vector<int>,PuzPoint<double> >(jj,newj[nj].second));
        }
      }
      
      triple_cond = triple_cond || (this_state.nProcess>2);
      
      //long int ticks = clock();
      //double seconds = (double) (ticks-startTicks)/CLOCKS_PER_SEC; 
    
      vcl_cout << "------------------------------"<< vcl_endl;
      vcl_cout << vcl_endl;
      vcl_cout << vcl_endl;
      vcl_cout << "Piece "<< p1 << " matching with Piece " << p2 << vcl_endl;
     //MSGOUT(0);

    if (DISPLAY_INFORMATION){
          if (triple_cond)
                vcl_cout << "Triple condition: Met"<<vcl_endl;
          else
              vcl_cout << "Triple condition: Not Met" << vcl_endl;
          
          vcl_cout <<"Distance Measure: " << dist_meas << vcl_endl;
          vcl_cout <<"Weighted Distance Measure: " << dist_meas*DIST_COEF << vcl_endl;
          vcl_cout <<"Length Measure: " << leng_meas << vcl_endl;
          vcl_cout <<"Weighted Length Measure: " << sqrt(leng_meas)*LENGTH_COEF << vcl_endl;
          vcl_cout <<"Diagnostic Measure: " << diag_meas << vcl_endl; 
          vcl_cout <<"Weighted Diagnostic Measure: " << sqrt(diag_meas)*DIAG_COEF << vcl_endl;
            vcl_cout <<"Total Cost of Match: " << this_state._matches[i].cost << vcl_endl;
        }



      this_state.tCost += DIST_COEF*dist_meas+LENGTH_COEF*sqrt(leng_meas)+DIAG_COEF*sqrt(diag_meas)-costReduction2;
      this_state.sCost  = DIST_COEF*dist_meas+LENGTH_COEF*sqrt(leng_meas)+DIAG_COEF*sqrt(diag_meas)-costReduction2;

      this_state.process.push_back(add);
      this_state.nProcess++;
      

      vcl_cout << "sCost: " << this_state.sCost << vcl_endl;

     //MSGOUT(0);
  
      if(leng_meas>LENGTH_THRESH_HIGH && triple_cond && 
         ((diag_meas>DIAG_THRESH_LOW && dist_meas<DIST_THRESH_LOW) || 
    (diag_meas>DIAG_THRESH_HIGH && dist_meas<DIST_THRESH_HIGH))) {
        
        this_state.merge(c1);
        //this_state.updateCost();
        if(flag)
      this_state.purge();
        states.push_back(this_state);
        num_good++;
        if(num_good==NUM_STATES_ITER) break;

        //vcl_cout << " " << num_good << vcl_endl;
        //fflush(stdout);
      }
    }
  }
      }
      this_state = temp_state;
      //FORK - copy constructor test{
        //this_state.sanityCheckMatchesByIndexNPS();
        //temp_state.sanityCheckMatchesByIndexNPS();
      /*
        vcl_cout << "in ITERATE SEARCH: Testing copy constructor of searchState: " << vcl_endl;
        for( int i = 0; i < temp_state._matches.size(); i++ ){
            vcl_cout << i;
            if( &(temp_state._matches[i]) == &(this_state._matches[i]) ) {
                vcl_cout << " FAILED: address of both matches are same" << vcl_endl;
                assert(0);
            }
        
            vcl_cout << ":passed ";
        }
        vcl_cout << vcl_endl;
      //}
      */

      
    }
  }


  return states;
}

/*
void PuzzleSolving::initGL(){
  _linesGroup = new Group;  
  localRootNode()->addChild(_linesGroup);
}

void PuzzleSolving::drawLines(Group* node, Curve<double,double>* C, StandardColor color) {

  double x1,y1,x2,y2,osx,osy;
  int i;
  
  PolylineGeom *pline = new PolylineGeom();
  pline->loadStyle(Color(color) & LineWidth(0.5));    
  node->addChild(pline);
  
  for(i=0;i<C->numPoints();i++) {  
    Point2D<double> pt1(C->x(i),C->y(i));
    pline->push_back(pt1);
  }
  Point2D<double> pt1(C->x(0),C->y(0));
  pline->push_back(pt1);

  if(C->lumCheck())
    for(i=0;i<C->numPoints();i++) {  
      x1=C->x(i);
      y1=C->y(i);
      x2=x1+C->grad_x(i);
      y2=y1+C->grad_y(i);
      osx=(x2-x1)/2.0;
      osy=(y2-y1)/2.0;

      LineGeom *line = new LineGeom(Point2D<double>(x1,y1),Point2D<double>(x2,y2));
      line->loadStyle(Color(color));
      line->multXForm(XForm::translation(x1,y1)*XForm::rotation(3.1416/2.0)*
          XForm::translation(-x1-osx,-y1-osy));
      node->addChild(line);
    }
}

searchState PuzzleSolving::drawLayout(Group* state_node, searchState state, vcl_vector<int> tree) {

  int i;
  int proc=0;
  vcl_vector<int> procL;
  
  Group* line_node = new Group;
  state_node->addChild(line_node);
  localRootNode()->addChild(state_node);
  StandardColor color = RED;
  
  for(i=0;i<tree.size();i++){
    vcl_cout << "Tree " << i << vcl_endl;
    vcl_cout << "tree[i]:"<< tree[i] << vcl_endl;
    vcl_cout << "state.p1(tree[i]): "<< state.p1(tree[i]) << vcl_endl;
    vcl_cout << "state.p2(tree[i]): "<< state.p2(tree[i]) << vcl_endl; 
    vcl_cout << "state.cost[tree[i]].first "<< state.cost[tree[i]].first << vcl_endl;

  }
  int p=state.p2(tree[0]);
  drawLines(line_node,state.piece(p),color);
  procL.push_back(p);

  while(proc<tree.size())
    for(i=0;i<tree.size();i++) {
      
      int p1=state.p1(tree[i]);
      int p2=state.p2(tree[i]);      
      int chk1=1;
      int chk2=1;
      if(vcl_find(procL.begin(),procL.end(),p1)==procL.end())
  chk1=0;
      if(vcl_find(procL.begin(),procL.end(),p2)==procL.end())
  chk2=0;
      
      if(chk1!=chk2) {

  bool flip=0;
  int add=p1;

  if(chk1==1 && chk2==0){
    flip=1;
    add=p2;
  }
  
  regContour(state.piece(p1),state.piece(p2),state.map(tree[i]),flip);
  drawLines(line_node,state.piece(add),color);
  
  procL.push_back(add);
  proc++;
      }  
    }
  
  state_node->multXForm(autoScale(line_node,GRID_BORDER,GRID_BORDER,SCREEN_SIZE));

  return state;
}

void PuzzleSolving::drawState(searchState *state, int window) {

  if(state->nProcess==0)
    return;

  double x,y;
  double size=((SCREEN_SIZE-((DISPLAY_GRID_ROWS+1.0)*GRID_BORDER))/
         (double)DISPLAY_GRID_ROWS);
  int i,r,c,p;

  r=floor((double)window/(double)DISPLAY_GRID_ROWS);
  c=window%DISPLAY_GRID_COLS;

  x=GRID_BORDER*(double)(c+1)+size*(double)c;
  y=GRID_BORDER*(double)(r+1)+size*(double)r;
  
  //vcl_cout << r << " " << c << " " << x << " " << y << vcl_endl;

  Group* line_node = new Group;
  Group* image_node = new Group;
  Group* state_node = new Group;
  
  state_node->addChild(image_node);
  state_node->addChild(line_node);
  localRootNode()->addChild(state_node);
  
  StandardColor color = RED;
  for(i=0;i<state->numCon();i++) {
    drawLines(line_node,state->constr(i),color);
  }
  color = BLUE;
  for(i=0;i<state->nProcess;i++) {
    p = state->process[i];
        
    if(IMAGE_FLAG) {
      intMap map;
      for(int mp=0;mp<state->piece(p)->numPoints();mp+=10)
    map.push_back(vcl_pair<int,int>(mp,mp));
      Curve<double,double> temp = _Contours[p];

      // XXX_TRANSFORM how to get the transform
      XForm3x3 trans = regContour(&temp,state->piece(p),map);
      
      RGBAImage *img = new RGBAImage(_imagenames[p].c_str());
      RGBImage *mask = new RGBImage(_masknames[p].c_str());
      img->setMask(mask);
      delete mask;
      
      ImageGeom *image = new ImageGeom(img,0,0,img->width(),0);
  
      
      image_node->addChild(image);
      image->multXForm(trans);
    }

    if(i==state->nProcess-1)
      color = RED;
    drawLines(line_node,state->piece(p),color);
    if(IMAGE_FLAG)
      drawLines(line_node,&(state->_ecList[p]),color);
  } 
    
  color=GREEN;
  for(i=0;i<state->open_junc.size();i++) {
    PuzPoint<double> j = state->open_junc[i].second; 
    PointGeom *junc = new PointGeom(j.x(),j.y());
    junc->loadStyle(Color(color) & PointSize(10));
    line_node->addChild(junc);
  }
  color=ORANGE;
  for(i=0;i<state->closed_junc.size();i++) {
    PuzPoint<double> j = state->closed_junc[i].second; 
    PointGeom *junc = new PointGeom(j.x(),j.y());
    junc->loadStyle(Color(color) & PointSize(10));
    line_node->addChild(junc);
  }

  double px1,py1,px2,py2;
  
  vcl_pair<double,double> cen1=
    center(state->piece(state->new_edge.first));
  px1 = cen1.first;
  py1 = cen1.second;

  vcl_pair<double,double> cen2=
    center(state->piece(state->new_edge.second));
  px2 = cen2.first;
  py2 = cen2.second; 
  
  
  


  LineGeom* gline = new LineGeom(px1,py1,px2,py2);
  gline->loadStyle(Color(BLACK) & LineWidth(3));
  line_node->addChild(gline);
  int cnt;
  for(cnt=0;cnt<state->new_edges.size();cnt++) {
    cen1=center(state->piece(state->new_edges[cnt].first));
    px1 = cen1.first;
    py1 = cen1.second;  
    
    cen2=center(state->piece(state->new_edges[cnt].second));
    px2 = cen2.first;
    py2 = cen2.second;
    
    gline = new LineGeom(px1,py1,px2,py2);
    gline->loadStyle(Color(LIGHT_GRAY) & LineWidth(3));
    line_node->addChild(gline);
  }

  for(cnt=0;cnt<state->old_edges.size();cnt++) {
    cen1=center(state->piece(state->old_edges[cnt].first));
    px1 = cen1.first;
    py1 = cen1.second;
    
    cen2=center(state->piece(state->old_edges[cnt].second));
    px2 = cen2.first;
    py2 = cen2.second; 
    
    gline = new LineGeom(px1,py1,px2,py2);
    gline->loadStyle(Color(LIGHT_GRAY) & LineWidth(1));
    line_node->addChild(gline);
  }  

  state_node->multXForm(autoScale(line_node,x,y,size));
  
  if(!CONTOUR_FLAG)
    state_node->removeChild(line_node);
  
  state->node=state_node;
}

void PuzzleSolving::deleteAll() {
  localRootNode()->eraseAllChildren();
}

XForm3x3 PuzzleSolving::autoScale(Group *node, double X, double Y, double size) {
  
  double x1,x2,y1,y2;
  sg()->getWorldBoundingRect(node,x1,y1,x2,y2);
  
  double x_wid=x2-x1;
  double y_wid=y2-y1;
  double size2,xos,yos;
  
  if(x_wid>y_wid) {
    size2=x_wid;
    xos=0;
    yos=(x_wid-y_wid)/2;
  }
  else {
    size2=y_wid;
    xos=(y_wid-x_wid)/2;
    yos=0;
  }
  
  double scale = size/size2;
  
  return XForm::translation(X,Y)*XForm::uniform_scale(scale)*XForm::translation(xos-x1,yos-y1);
}
*/

// **********************************************************************


searchState::searchState(vcl_vector<Curve<double,double> > contours) {
  active=1;
  _cList=contours;
  nProcess=0;
  tCost=0;
  sCost=0;
  _numMatch=0;
  _num_new=0;
}

searchState::searchState(vcl_vector<Curve<double,double> > contours,
       vcl_vector<Curve<double,double> > econs) {
  active=1;
  _cList=contours;
  _ecList=econs;
  nProcess=0;
  tCost=0;
  sCost=0;
  _numMatch=0;
  _num_new=0;

  for(unsigned i=0;i<_cList.size();i++){
    vcl_cout << _ecList[i].numPoints() << vcl_endl;
    _cList[i].eCurve=&_ecList[i];
    vcl_cout <<_cList[i].eCurve->numPoints() << vcl_endl;
    vcl_cout << &(_ecList[i]) << vcl_endl; 
  }
}

bool searchState::operator==(searchState state2) {
  
  if(nProcess != state2.nProcess)
    return 0;
  
  vcl_vector<int> proc1 = process;
  vcl_vector<int> proc2 = state2.process;
  
  assert(proc1.size()==proc2.size());

  vcl_sort(proc1.begin(),proc1.end());
  vcl_sort(proc2.begin(),proc2.end()); 
  for(unsigned i=0;i<proc1.size();i++) {
    if(proc1[i] != proc2[i])
      return 0;
  }

  Curve<double,double> c1;
  Curve<double,double> c2;
  intMap map;
  int pts = 0;

  for(unsigned i=0;i<proc1.size();i++) {
    int num=piece(proc1[i])->coarseNumPoints();
    for(int j=0;j<num;j++) {
      map.push_back(vcl_pair<int,int>(pts,pts));
      c1.append(piece(proc1[i])->Cpoint(j));
      c2.append(state2.piece(proc1[i])->Cpoint(j));
      pts++;
    }
  }

  assert(c1.numPoints() == c2.numPoints());
  regContour(&c1,&c2,map);
  
  return c1==c2;
}

void searchState::merge(Curve<double,double>* cv) {

  if(_merged.numPoints()==0) {
    _merged=*cv;
    return;
  }
    

  vcl_pair<double,int> dist;
  Curve<double,double> new_merge;
  for(int i=0;i<cv->numPoints();i++) {
    dist=ptDist(cv->x(i),cv->y(i),&_merged);
    if(dist.first>10.0)
      new_merge.append(cv->point(i));
  }

  for(int i=0;i<_merged.numPoints();i++) {
    dist=ptDist(_merged.x(i),_merged.y(i),cv);
    if(dist.first>10.0)
      new_merge.append(_merged.point(i));
  }

  _merged=new_merge;
}

void searchState::structure() {

  _constr.clear();

  Curve<double,double> temp;
  vcl_vector<Curve<double,double> > frags;
  
  temp.append(_merged.point(0));
  for(int i=1;i<_merged.numPoints();i++) {
    if(pointDist(_merged.x(i),_merged.y(i),_merged.x(i-1),_merged.y(i-1))>15.0) {
      frags.push_back(temp);
      temp.empty();
    }
    temp.append(_merged.point(i));
  }
  if(temp.numPoints()>0)
    frags.push_back(temp);

  temp.empty();
  vcl_vector<int> used;  
  for(unsigned i=0;i<frags.size();i++)
    used.push_back(0);
  
  double d,mind;
  int ref;

  for(unsigned i=0;i<frags.size();i++) {
    if(used[i]==0) {
      used[i]=1;
      temp=frags[i];
      ref=-1;
//SOMETHING VERY WEIRD IS GOING ON HERE -- PLEASE FIX!!! -MM      
      while(ref!=static_cast<int>(i)) {
  mind=DP_VERY_LARGE_COST;
  for(unsigned j=i;j<frags.size();j++) {
    if(used[j]==0 || j==i) {
      d=pointDist(temp.x(temp.numPoints()-1),temp.y(temp.numPoints()-1),
      frags[j].x(0),frags[j].y(0));
      if(d<mind) {
        mind=d;
        ref=j;
      }
    }
  }
//SOMETHING VERY WEIRD IS GOING ON HERE -- PLEASE FIX!!! -MM      
  if(ref!=static_cast<int>(i)) {
    for(int q=0;q<frags[ref].numPoints();q++)
      temp.append(frags[ref].point(q));
    used[ref]=1;
  }
      }
      if(temp.numPoints()>100) {
  temp.computeProperties();
  _constr.push_back(temp);
      }
    }
  }
}



//Runs testDP on all the possible combinations?
void searchState::comboMatch() {
  
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  int k;
#endif
  vcl_vector<map_with_cost> this_pair;
  vcl_vector<int> proc = process;
  int num=_cList.size();

  //Variables for timing
  /*
  int ticks, startTicks;
  startTicks = clock();
  */

  if(_num_new>0) {
    vcl_cout << "Warning! New Matches Not Purged!" << vcl_endl;
    return;
  }

  //structure();

  for(int i=0;i<numCon();i++)
    _constr[i].coarseResample(COARSE_SAMPLE_SIZE);

  for(int i=0;i<numCon();i++)
    for(int j=0;j<num;j++)
      if(vcl_find(proc.begin(),proc.end(),j)==proc.end()) {
    this_pair=testDP(_cList[j],_constr[i]);
    vcl_cout << "Matching: " << j+1 << " to " << i+num+1 << vcl_endl;
    if(this_pair.size()>0)
      for(unsigned k=0;k<this_pair.size();k++) {
        addMatch(this_pair[k].second,this_pair[k].first,j,i+num);
        _num_new++;
      }
      }
      /*
  if (TIMING_ON){
    ticks = clock();
    _totalComboMatchTicks += (ticks-startTicks);
    }
      */


}


void searchState::addMatch(intMap map, double cst, int c1, int c2) {
  
  Curve<double,double> curve1 = _cList[c1];
  Curve<double,double> curve2 = _cList[c2];
  curve1.eCurve=0;
  curve2.eCurve=0;

  XForm3x3 theXform = regContour(&curve1,&curve2,map);

  /*
  _xForms.push_back(theXform = regContour(&curve1,&curve2,map));
  _maps.push_back(map);
  cost.push_back(vcl_pair<double,int>(cst,_numMatch));
  _pairs.push_back(vcl_pair<int,int>(c1,c2));
  */

  /*
  //add the pairwiseMatch class
  pairwiseMatch *newMatch = new pairwiseMatch();
  newMatch->cost = cst;
  newMatch->myIndex = _numMatch;
  newMatch->pointMap = intMap(map);
  newMatch->whichCurves = vcl_pair<int,int>(c1,c2);
  newMatch->xForm = new XForm3x3( theXform );
  _matches.push_back(newMatch);
  */
  //non-pointer style
  pairwiseMatch newMatch;
  newMatch.cost = cst;
  newMatch.myIndex = _numMatch;
  newMatch.pointMap = intMap(map);
  newMatch.whichCurves = vcl_pair<int,int>(c1,c2);
  newMatch.xForm = XForm3x3( theXform );
  _matches.push_back(newMatch);

  _numMatch++;
} 

//non pointer style

void searchState::sortPairwiseMatches(){
    //vcl_sort( _matches.begin(), _matches.end(), pairwiseMatch() );
    vcl_sort( _matches.begin(), _matches.end(), pairwiseMatchSort() );
}

/*
void searchState::printPairwiseMatchesNPS(){
    vcl_cout << "For all (" << _matches.size() << "): class then vectors: [COST][INDEX][C1][C2]" << vcl_endl;
    for( int i = 0; i < _matches.size(); i++ ){
        vcl_cout << i << " + [" << _matches[i].cost << "][" << _matches[i].myIndex << "][" << _matches[i].whichCurves.first << "][" << _matches[i].whichCurves.second << "]   ";
        vcl_cout << "[" << cost[i].first << "][" << cost[i].second << "][" << _pairs[i].first << "][" << _pairs[i].second << "]" << vcl_endl;
    }
}

void searchState::printPairwiseMatchesSortedNPS(){
    vcl_cout << "For all (" << _matches.size() << "): class then vectors: [COST][INDEX][C1][C2]  as they are individuall sorted by cost" << vcl_endl;
    //get the sort of costs
    indexedMeasures blah = cost;
    vcl_sort(blah.begin(),blah.end(),cost_ind_less());

    for( int i = 0; i < _matches.size(); i++ ){
        vcl_cout << i << " + [" << _matches[i].cost << "][" << _matches[i].myIndex << "][" << _matches[i].whichCurves.first << "][" << _matches[i].whichCurves.second << "]   ";
        vcl_cout << "[" << blah[i].first << "][" << blah[i].second << "][" << _pairs[blah[i].second].first << "][" << _pairs[blah[i].second].second << "]" << vcl_endl;
    }
}

void searchState::printPairwiseMatchesByIndexNPS(){
    vcl_cout << "For all (" << _matches.size() << "): class then vectors: [COST][INDEX][C1][C2] --BY-INDEX--" << vcl_endl;
    for( int i = 0; i < _matches.size(); i++ ){
        vcl_cout << i << " + [" << _matches[i].cost << "][" << _matches[i].myIndex << "][" << _matches[i].whichCurves.first << "][" << _matches[i].whichCurves.second << "]   ";
        vcl_cout << "[" << cost[_matches[i].myIndex].first << "][" << cost[_matches[i].myIndex].second << "][" << _pairs[_matches[i].myIndex].first << "][" << _pairs[_matches[i].myIndex].second << "]" << vcl_endl;
    }
}

void searchState::sanityCheckMatchesByIndexNPS(){

    //if( _matches.size() != cost.size() ||
    //    _matches.size() != _maps.size() ){
    if( _matches.size() != cost.size() ){
        vcl_cout << "SANITY CHECK FAILED: matches size != cost size" << vcl_endl;
        return;
    }

    for( int i = 0; i < _matches.size(); i++ ){

        if( _matches[i].cost != cost[_matches[i].myIndex].first ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: cost" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        if( &(_matches[i].cost) == &(cost[_matches[i].myIndex].first) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: cost addr ==" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        if( _matches[i].myIndex != cost[_matches[i].myIndex].second ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: index" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        if( &(_matches[i].myIndex) == &(cost[_matches[i].myIndex].second) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: index addr ==" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        if( _matches[i].pointMap != _maps[_matches[i].myIndex] ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: maps" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        if( &(_matches[i].pointMap) == &(_maps[_matches[i].myIndex]) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: maps addr ==" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        if( _matches[i].whichCurves.first != _pairs[_matches[i].myIndex].first ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 1" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        if( &(_matches[i].whichCurves.first) == &(_pairs[_matches[i].myIndex].first) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 1 addr ==" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        if( _matches[i].whichCurves.second != _pairs[_matches[i].myIndex].second ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 2" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        if( &(_matches[i].whichCurves.second) == &(_pairs[_matches[i].myIndex].second) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 2 addr ==" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
        //if( *(_matches[i]->xForm) != _xForms[_matches[i]->myIndex] ){
        //    vcl_cout << "SANITY CHECK " << i << " FAILED: xforms" << vcl_endl;
        //    printPairwiseMatchesByIndex();
        //    assert(0);
        //}
        if( &(_matches[i].xForm) == &(_xForms[_matches[i].myIndex]) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: xforms addr ==" << vcl_endl;
            printPairwiseMatchesByIndexNPS();
            assert(0);
        }
    }        
    
    vcl_cout << "sanity check ByIndex passed for all [" << _matches.size() << "]" << vcl_endl;

}

void searchState::sanityCheckMatchesNPS(){

    if( _matches.size() != cost.size() ||
        _matches.size() != _maps.size() ){
        vcl_cout << "SANITY CHECK FAILED: matches size != cost or maps size" << vcl_endl;
        return;
    }

    for( int i = 0; i < _matches.size(); i++ ){

        if( _matches[i].cost != cost[i].first ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: cost" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        if( &(_matches[i].cost) == &(cost[i].first) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: cost addr ==" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        if( _matches[i].myIndex != cost[i].second ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: index" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        if( &(_matches[i].myIndex) == &(cost[i].second) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: index addr ==" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        if( _matches[i].pointMap != _maps[i] ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: maps" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        if( &(_matches[i].pointMap) == &(_maps[i]) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: maps addr ==" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        if( _matches[i].whichCurves.first != _pairs[i].first ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 1" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        if( &(_matches[i].whichCurves.first) == &(_pairs[i].first) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 1 addr ==" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        if( _matches[i].whichCurves.second != _pairs[i].second ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 2" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        if( &(_matches[i].whichCurves.second) == &(_pairs[i].second) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 2 addr ==" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
        //if( *(_matches[i]->xForm) != _xForms[i] ){
        //    vcl_cout << "SANITY CHECK " << i << " FAILED: xforms" << vcl_endl;
        //    assert(0);
        //}
        if( &(_matches[i].xForm) == &(_xForms[i]) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: xforms addr ==" << vcl_endl;
            printPairwiseMatchesNPS();
            assert(0);
        }
    }        
    
    vcl_cout << "sanity check passed for all [" << _matches.size() << "]" << vcl_endl;

}
*/

/* //pointer style
void searchState::sortPairwiseMatches(){
    vcl_sort( _matches.begin(), _matches.end(), pairwiseMatch() );
}

void searchState::printPairwiseMatches(){
    vcl_cout << "For all (" << _matches.size() << "): class then vectors: [COST][INDEX][C1][C2]" << vcl_endl;
    for( int i = 0; i < _matches.size(); i++ ){
        vcl_cout << i << " + [" << _matches[i]->cost << "][" << _matches[i]->myIndex << "][" << _matches[i]->whichCurves.first << "][" << _matches[i]->whichCurves.second << "]   ";
        vcl_cout << "[" << cost[i].first << "][" << cost[i].second << "][" << _pairs[i].first << "][" << _pairs[i].second << "]" << vcl_endl;
    }
}
void searchState::printPairwiseMatchesByIndex(){
    vcl_cout << "For all (" << _matches.size() << "): class then vectors: [COST][INDEX][C1][C2] --BY-INDEX--" << vcl_endl;
    for( int i = 0; i < _matches.size(); i++ ){
        vcl_cout << i << " + [" << _matches[i]->cost << "][" << _matches[i]->myIndex << "][" << _matches[i]->whichCurves.first << "][" << _matches[i]->whichCurves.second << "]   ";
        vcl_cout << "[" << cost[_matches[i]->myIndex].first << "][" << cost[_matches[i]->myIndex].second << "][" << _pairs[_matches[i]->myIndex].first << "][" << _pairs[_matches[i]->myIndex].second << "]" << vcl_endl;
    }
}

void searchState::sanityCheckMatchesByIndex(){

    if( _matches.size() != cost.size() ||
        _matches.size() != _maps.size() ){
        vcl_cout << "SANITY CHECK FAILED: matches size != cost or maps size" << vcl_endl;
        return;
    }

    for( int i = 0; i < _matches.size(); i++ ){

        if( _matches[i]->cost != cost[_matches[i]->myIndex].first ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: cost" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        if( &(_matches[i]->cost) == &(cost[_matches[i]->myIndex].first) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: cost addr ==" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        if( _matches[i]->myIndex != cost[_matches[i]->myIndex].second ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: index" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        if( &(_matches[i]->myIndex) == &(cost[_matches[i]->myIndex].second) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: index addr ==" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        if( _matches[i]->pointMap != _maps[_matches[i]->myIndex] ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: maps" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        if( &(_matches[i]->pointMap) == &(_maps[_matches[i]->myIndex]) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: maps addr ==" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        if( _matches[i]->whichCurves.first != _pairs[_matches[i]->myIndex].first ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 1" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        if( &(_matches[i]->whichCurves.first) == &(_pairs[_matches[i]->myIndex].first) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 1 addr ==" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        if( _matches[i]->whichCurves.second != _pairs[_matches[i]->myIndex].second ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 2" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        if( &(_matches[i]->whichCurves.second) == &(_pairs[_matches[i]->myIndex].second) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 2 addr ==" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
        //if( *(_matches[i]->xForm) != _xForms[_matches[i]->myIndex] ){
        //    vcl_cout << "SANITY CHECK " << i << " FAILED: xforms" << vcl_endl;
        //    printPairwiseMatchesByIndex();
        //    assert(0);
        //}
        if( (_matches[i]->xForm) == &(_xForms[_matches[i]->myIndex]) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: xforms addr ==" << vcl_endl;
            printPairwiseMatchesByIndex();
            assert(0);
        }
    }        
    
    vcl_cout << "sanity check ByIndex passed for all [" << _matches.size() << "]" << vcl_endl;

}

void searchState::sanityCheckMatches(){

    if( _matches.size() != cost.size() ||
        _matches.size() != _maps.size() ){
        vcl_cout << "SANITY CHECK FAILED: matches size != cost or maps size" << vcl_endl;
        return;
    }

    for( int i = 0; i < _matches.size(); i++ ){

        if( _matches[i]->cost != cost[i].first ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: cost" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        if( &(_matches[i]->cost) == &(cost[i].first) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: cost addr ==" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        if( _matches[i]->myIndex != cost[i].second ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: index" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        if( &(_matches[i]->myIndex) == &(cost[i].second) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: index addr ==" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        if( _matches[i]->pointMap != _maps[i] ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: maps" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        if( &(_matches[i]->pointMap) == &(_maps[i]) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: maps addr ==" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        if( _matches[i]->whichCurves.first != _pairs[i].first ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 1" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        if( &(_matches[i]->whichCurves.first) == &(_pairs[i].first) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 1 addr ==" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        if( _matches[i]->whichCurves.second != _pairs[i].second ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 2" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        if( &(_matches[i]->whichCurves.second) == &(_pairs[i].second) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: curves 2 addr ==" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
        //if( *(_matches[i]->xForm) != _xForms[i] ){
        //    vcl_cout << "SANITY CHECK " << i << " FAILED: xforms" << vcl_endl;
        //    assert(0);
        //}
        if( (_matches[i]->xForm) == &(_xForms[i]) ){
            vcl_cout << "SANITY CHECK " << i << " FAILED: xforms addr ==" << vcl_endl;
            printPairwiseMatches();
            assert(0);
        }
    }        
    
    vcl_cout << "sanity check passed for all [" << _matches.size() << "]" << vcl_endl;

}
*/
  
void searchState::purge() {
  int cnt=_num_new;
  //FORK
  for(int i=0;i<cnt;i++) {
    //_maps.pop_back();
    //cost.pop_back();
    //_pairs.pop_back();
    _numMatch--;
    _num_new--;
  }  
  _matches.empty();
}

void searchState::updateCost() {

  int i;
  int total=0;
  int exter=0;
  int inter=0;

  for(i=0;i<nProcess;i++)
    total+=piece(process[i])->numPoints();
  
  exter=merged()->numPoints();
  inter=static_cast<int>(floor((double)(total-exter)/2.0));

  sCost=static_cast<double>((double)exter/(double)inter);

}



// **********************************************************************


 
vcl_vector<map_with_cost>
testDP(Curve<double,double> c1, Curve<double,double> c2) {
  
  int i,j,k,s1,ji,ki;
  Curve<double,double> c1i,c2i,c1_cs,c2_cs,c1i_cs,c2i_cs;
  /*
  long int ticks;
  long int startTicks;
  double seconds;
  */

  
  //startTicks = clock();

  // set up contours
 
  c1i=c1; c1i.invert();      
  c2i=c2; c2i.invert();      
  c1_cs=c1; 
  c2_cs=c2; 
  c1i_cs=c1i;
  c2i_cs=c2i;
  
  c1_cs.becomeCoarse();     //
  c2_cs.becomeCoarse();     //  Coarse scale versions
  c1i_cs.becomeCoarse();    //
  c2i_cs.becomeCoarse();    //
  
  int n=c1_cs.numPoints();
  int m=c2_cs.numPoints();
  int num_corners1=c1_cs.numExtrema();
  int num_corners2=c2_cs.numExtrema();

  // set up handling of matches
  vcl_vector<map_with_cost > allMaps;
  vcl_vector<map_with_cost > allMaps2;
  vcl_vector<map_with_cost> maps_out;
  map_with_cost map_and_cost;
  double cost;
  intMap fmap;
  intMap fmapi;
  vcl_vector<int> off_k;
  vcl_vector<int> off_j;
  
  int num_maps=0;
  int no_match_found = 1;

  //
  // begin loop - tries all possible pairs of 
  // corner points as starting points for match
  //

  //vcl_cout << "Matching...    ";
  //vcl_cout << "Coarse Scale Matching" << vcl_endl;
  for(int kr=0;kr<num_corners2;kr++) {
    k=c2_cs.extrema(kr);
    ki=c2i_cs.extrema(kr);
    
    for(int jr=0;jr<num_corners1;jr++) {  
      j=c1_cs.extrema(jr);
      ji=c1i_cs.extrema(jr);
       
      // extend correspondence in both directions
      DPMatch<Curve<double,double>,double> d1(c1i_cs,c2_cs,ji,k);
      DPMatch<Curve<double,double>,double> d2(c1_cs,c2i_cs,j,ki);

      d1.match();
      d2.match();

      //d1.printCost();
      //d2.printCost();
      
      //d1.printFinalMap();
      fmap = d1.finalMap();
      s1 = fmap.size();
      cost=d1.finalCost();
     
      if(s1>=MIN_MAP_SIZE_CRSE && cost<0) {  

  // correct map for starting point offset
  for (i=0;i<s1;i++){    
    int index1,index2;
    index1=fmap[i].first+ji;
    if(index1>n-1) index1-=n;
    index2=fmap[i].second+k;
    if(index2>m-1) index2-=m;
    fmap[i].first=index1;
    fmap[i].second=index2;
  }
  map_and_cost.first=cost;
  map_and_cost.second=fmap;
  allMaps.push_back(map_and_cost);

  num_maps++;
  no_match_found=0;
  
//   vcl_cout << "Forward:" << vcl_endl;
//   for(int mp=fmap.size()-1;mp>=0;mp--)
//     vcl_cout << fmap[mp].first << " ";
//   vcl_cout << vcl_endl;
//   for(int mp=fmap.size()-1;mp>=0;mp--)
//     vcl_cout << fmap[mp].second << " ";
//   vcl_cout << vcl_endl << vcl_endl;
  
      }
      
      //d2.printFinalMap(); 
      fmap = d2.finalMap();
      cost=d2.finalCost();
      fmapi = fmap;
      s1 = fmap.size();
     
      if(s1>=MIN_MAP_SIZE_CRSE && cost<0) {  

  // correct map for starting point offset - reverse direction  
  for (i=0;i<s1;i++){    
    int index1,index2;
    index1=fmap[i].first+j;
    if(index1>n-1) index1-=n;
    index2=fmap[i].second+ki;
    if(index2>m-1) index2-=m;
    fmapi[s1-i-1].first=n-index1-1;
    fmapi[s1-i-1].second=m-index2-1;
  }
  map_and_cost.first=cost;
  map_and_cost.second=fmapi;
  allMaps.push_back(map_and_cost);

  num_maps++;
  no_match_found=0;
    
//   vcl_cout << "Reverse:" << vcl_endl;
//   for(int mp=0;mp<fmapi.size();mp++)
//     vcl_cout << fmapi[mp].first << " ";
//   vcl_cout << vcl_endl;
//   for(int mp=0;mp<fmapi.size();mp++)
//     vcl_cout << fmapi[mp].second << " ";
//   vcl_cout << vcl_endl << vcl_endl;

      } 
      
    }
    //printf("\b\b\b\b%3i%%",(int)((double)(kr+1)/ref_ind2.size()*100));
    //fflush(stdout);
  }
  //vcl_cout << vcl_endl;
  
  if(no_match_found==0){ // No match cond
               
    //vcl_cout << "Fine Scale Matching" << vcl_endl;

    vcl_vector<Curve<double,double> > group;
    int add,cnt;

    vcl_sort(allMaps.begin(),allMaps.end(),map_cost_less());

  //Make sure the overlap of each match is not too high
  //and that it has not already been added.
  int allmaps_size_int = static_cast<int>(allMaps.size());
    for (j=0;j<allmaps_size_int;j++) {
      fmap = allMaps[j].second;
      regContour(&c1i_cs,&c2_cs,fmap);
      c1_cs=c1i_cs;
      c1_cs.invert();
      if(detectOverlap(&c1_cs,&c2_cs)<OLAP_THRESH_HIGH) {
      add=1;
      if(group.size()>0) {
        int group_size_int = static_cast<int>(group.size());
        for(cnt=0;cnt<group_size_int;cnt++) {
         if(c1i_cs==group[cnt]) {
          add=0;
          break;
      }
    }
  }

  


  if(add==1) {
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    unsigned siz = fmap.size();
#endif
    map_and_cost=fineScaleMatch(c1i,c2,fmap);
    allMaps2.push_back(map_and_cost);
    group.push_back(c1i_cs);
  }
      }
      if(allMaps2.size()==NUM_COARSE_OUT)
  break;
    }
    
    int num_fine = allMaps2.size();
    if(num_fine>NUM_FINE_OUT)
      num_fine=NUM_FINE_OUT;
    
    vcl_sort(allMaps2.begin(),allMaps2.end(),map_cost_less());
    for (j=0;j<num_fine;j++) { 
      if(allMaps2[j].first>MAX_FINE_COST)
      break;
      maps_out.push_back(allMaps2[j]);
    }
  }
  
  /*
  ticks = clock();
  _totalTestDPTicks += (ticks-startTicks);
  seconds = (double) (ticks-startTicks)/CLOCKS_PER_SEC;
  vcl_cout << " | " << maps_out.size() << " Matches | Time: " << seconds << vcl_endl;
  //MSGOUT(0);
  */
  return maps_out;
}


map_with_cost 
fineScaleMatch(Curve<double,double> c1i, Curve<double,double> c2, intMap fmap) {

  int iMap,i,j;
  int nf=c1i.numPoints();
  int mf=c2.numPoints();
  intMap fine_map;
  map_with_cost map_and_cost;
  Curve<double,double> c1_sub;
  Curve<double,double> c2_sub;
  
  //TIMING_ON variables:
  /*
  int ticks, startTicks;
  startTicks = clock();
  */


  double distance_measure=0;
  double length_measure=0;
  double diagnostic_measure=0;


  int st1=0;
  int st2=0;
  int en1=0;
  int en2=0;
  int type=0;

  if(c1i.isCorner(fmap[fmap.size()-1].first))
    st1=1;
  if(c2.isCorner(fmap[fmap.size()-1].second))
    st2=1;
  if(c1i.isCorner(fmap[0].first))
    en1=1;
  if(c2.isCorner(fmap[0].second))
    en2=1;

  //vcl_cout << st1 << " " << st2 << " " << en1 << " " << en2 << vcl_endl; 

  //Make sure we are dealing with curves that have at least 2 corners between them
  type=st1+st2+en1+en2-2;
  if(type<0) {
    vcl_cout << vcl_endl << "warning - invalid match" << vcl_endl;
    map_and_cost.first=DP_VERY_LARGE_COST;
    map_and_cost.second=fmap;
    return map_and_cost;
  }


  //Find the starting and ending points
  int c1_start = c1i.coarseRef(fmap[fmap.size()-1].first);
  int c2_start = c2.coarseRef(fmap[fmap.size()-1].second);
  int c1_end = c1i.coarseRef(fmap[0].first);
  int c2_end = c2.coarseRef(fmap[0].second);

  //vcl_cout << c1_start << "(" << st1 << ") " << c2_start << "(" << st2 << ") "
  //<< c1_end   << "(" << en1 << ") " << c2_end   << "(" << en2 << ")" << vcl_endl;
    
  //vcl_cout << "plot(" << c1i.x(c1_start) << "," << c1i.y(c1_start) << ",'o');"
  //     << "plot(" << c2.x(c2_start)  << "," << c2.y(c2_start)  << ",'o');";
  //vcl_cout << "plot(" << c1i.x(c1_end)   << "," << c1i.y(c1_end)   << ",'o');"
  //     << "plot(" << c2.x(c2_end)    << "," << c2.y(c2_end)    << ",'o');" << vcl_endl;  

  //vcl_cout << "Start: (" << c1i.x(c1_start) << "," << c1i.y(c1_start) << ") "
  //     << "(" << c2.x(c2_start)  << "," << c2.y(c2_start)  << ") ";
  //vcl_cout << "End: (" << c1i.x(c1_end)   << "," << c1i.y(c1_end)   << ") "
  //     << "(" << c2.x(c2_end)    << "," << c2.y(c2_end)    << ") " << vcl_endl; 
  
  //Find the distances between the starting and ending points of each curve
  double dist1=pointDist(c1i.point(c1_start),c1i.point(c1_end));
  double dist2=pointDist(c2.point(c2_start),c2.point(c2_end));

  //vcl_cout << dist1 << " -- " << dist2 << vcl_endl;
  
  //Determine if type 1 match.
  if(st1==1 && st2==1 && en1==1 && en2==1) {
    if(fabs(dist1-dist2)>10.0)
      type=1;
  }      

  //Sorting out starting point.
  else if(en1==1 && en2==1) {
    if(dist1<dist2) {
      double dif=dist2-dist1;
      double pdif=DP_VERY_LARGE_COST;
      while(dif<pdif) {
  c2_start++;
  c2_start%=mf;
  pdif=dif;
  dif=fabs(pointDist(c2.point(c2_start),c2.point(c2_end))-dist1);
      }
      c2_start--;
      c2_start=(c2_start+mf)%mf;
    }
    else {
      double dif=dist1-dist2;
      double pdif=DP_VERY_LARGE_COST;
      while(dif<pdif) {
  c1_start++;
  c1_start%=nf;
  pdif=dif;
  dif=fabs(pointDist(c1i.point(c1_start),c1i.point(c1_end))-dist2);
      }
      c1_start--;
      c1_start=(c1_start+nf)%nf;
    }
  }
  else {
    if(dist1<dist2) {
      double dif=dist2-dist1;
      double pdif=DP_VERY_LARGE_COST;
      while(dif<pdif) {
  c2_end--;
  c2_end=(c2_end+mf)%mf;
  pdif=dif;
  dif=fabs(pointDist(c2.point(c2_start),c2.point(c2_end))-dist1);
      }
      c2_end++;
      c2_end%=mf;
    }
    else {
      double dif=dist1-dist2;
      double pdif=DP_VERY_LARGE_COST;
      while(dif<pdif) {
  c1_end--;
  c1_end=(c1_end+nf)%nf;
  pdif=dif;
  dif=fabs(pointDist(c1i.point(c1_start),c1i.point(c1_end))-dist2);
      }
      c1_end++;
      c1_end%=nf;
    }
  }
 
  dist1=pointDist(c1i.point(c1_start),c1i.point(c1_end));
  dist2=pointDist(c2.point(c2_start),c2.point(c2_end));

  //vcl_cout << c1_start << "(" << st1 << ") " << c2_start << "(" << st2 << ") "
  //       << c1_end   << "(" << en1 << ") " << c2_end   << "(" << en2 << ")" << vcl_endl;
  //  vcl_cout << dist1 << " -- " << dist2 << vcl_endl;
  
  c1_sub.append(c1i.point(c1_start));
  if(c1_end < c1_start) c1_end+=nf;
  int offset1=static_cast<int>(floor((double)((c1_end-c1_start)%FINE_SAMPLE_SKIP)/2.0));
  for(iMap=c1_start+offset1;iMap<c1_end;iMap+=FINE_SAMPLE_SKIP)
    c1_sub.append(c1i.point(iMap%nf));
  c1_end%=nf;
  c1_sub.append(c1i.point(c1_end));
 
  c2_sub.append(c2.point(c2_start));
  if(c2_end < c2_start) c2_end+=mf;
  int offset2=static_cast<int>(floor((double)((c2_end-c2_start)%FINE_SAMPLE_SKIP)/2.0));
  for(iMap=c2_start+offset2;iMap<c2_end;iMap+=FINE_SAMPLE_SKIP)
    c2_sub.append(c2.point(iMap%mf));
  c2_end%=mf;
  c2_sub.append(c2.point(c2_end));  
  
  c1_sub.computeProperties();
  c2_sub.computeProperties();
  
  FineDPMatch<Curve<double,double>,double> fsmatch(c1_sub,c2_sub);
  fsmatch.match();
  fine_map=fsmatch.finalMap();
  
  int index1,index2;
  int s=fine_map.size();
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  int s1=c1_sub.numPoints();
  int s2=c2_sub.numPoints();
#endif

  fine_map[0].first=c1_end;
  fine_map[0].second=c2_end; 
  
  for (i=1;i<s-1;i++) {

    index1=((fine_map[i].first-1)*FINE_SAMPLE_SKIP+offset1+c1_start) % nf;
    fine_map[i].first=index1;

    index2=((fine_map[i].second-1)*FINE_SAMPLE_SKIP+offset2+c2_start) % mf;
    fine_map[i].second=index2;
  }
  
  fine_map[s-1].first=c1_start;
  fine_map[s-1].second=c2_start;

  regContour(&c1i,&c2,fine_map);
  
 //  vcl_cout << "Start: (" << c1i.x(c1_start) << "," << c1i.y(c1_start) << ") "
//        << "(" << c2.x(c2_start)  << "," << c2.y(c2_start)  << ") ";
//   vcl_cout << "End: (" << c1i.x(c1_end)   << "," << c1i.y(c1_end)   << ") "
//        << "(" << c2.x(c2_end)    << "," << c2.y(c2_end)    << ") " << vcl_endl; 

  c1i.invert();
  
  intMap map = localReg(&c1i,&c2,&distance_measure,&length_measure,
          &diagnostic_measure,LOCAL_REG_ITER_PAIR);
  regContour(&c1i,&c2,map);
  
  localReg(&c1i,&c2,&distance_measure,&length_measure,
     &diagnostic_measure,LOCAL_REG_ITER_PAIR);
  
  c1_start = map[map.size()-1].first;
  c2_start = map[map.size()-1].second;
  c1_end = map[0].first;
  c2_end = map[0].second;
  
  //  vcl_cout << "Start: (" << c1i.x(c1_start) << "," << c1i.y(c1_start) << ") "
  //       << "(" << c2.x(c2_start)  << "," << c2.y(c2_start)  << ") ";
  //  vcl_cout << "End: (" << c1i.x(c1_end)   << "," << c1i.y(c1_end)   << ") "
  //       << "(" << c2.x(c2_end)    << "," << c2.y(c2_end)    << ") " <<  vcl_endl; 
  
  //vcl_cout << map[0].first << " " << map[0].second << " "
  //     << map[map.size()-1].first << " " << map[map.size()-1].second << vcl_endl;
    

  //double ang_T = config::val("ANGLE_SIM_THRESH",0.2f);
  double ang_T = ANGLE_SIM_THRESH;
  //-spinner-
  double dl=0.0;
  double lum1,lum2;
  double ang=0.0;
  double gx1,gy1,gx2,gy2;
  int cost_flag;

  if(c1i.lumCheck() && c2.lumCheck()) {
    int map_size_int = static_cast<int>(map.size());
    for(j=0;j<map_size_int;j++) {
      
      cost_flag=0;

      lum1=c1i.lum(map[j].first);
      lum2=c2.lum(map[j].second);

      if((lum1>0) != (lum2>0)) {
      cost_flag=1;
    }
      else if(lum1>0) {
      cost_flag=1;
      
      gx1=c1i.grad_x(map[j].first);
      gy1=c1i.grad_y(map[j].first);
      gx2=c2.grad_x(map[j].second);
      gy2=c2.grad_y(map[j].second);
  
  ang=acos((gx1*gx2+gy1*gy2)/(sqrt(gx1*gx1+gy1*gy1)*sqrt(gx2*gx2+gy2*gy2)));
  if(ang>3.1416/2.0)
    ang=3.1416-ang;
  
  if(ang<ang_T)
    cost_flag=2;
      }
      
      //if(cost_flag==1)
      //dl += fabs(lum1-lum2);
      if(cost_flag==2)
  dl += lum1*lum2/(1.0+2.0*ang);
    }
    vcl_cout << dl << vcl_endl;
  }

  double cost = DIST_COEF*distance_measure + 
    LENGTH_COEF*sqrt(length_measure) + 
    DIAG_COEF*sqrt(diagnostic_measure) + EDGE_COEF*dl;
  
  //vcl_cout << distance_measure << " " << length_measure << " " << diagnostic_measure;

  //if(distance_measure>3.0) cost+=25.0;
  if(length_measure<LENGTH_THRESH_LOW) cost+=DP_VERY_LARGE_COST;
  if(diagnostic_measure<(DIAG_THRESH_LOW-1.5) && type!=2) cost+=DP_VERY_LARGE_COST;
  if(map.size()<MIN_MAP_SIZE_FINE) cost+=DP_VERY_LARGE_COST;
  //cost-=10.0*(double)type;

  //vcl_cout << " " << cost << vcl_endl << vcl_endl;

  //vcl_cout << distance_measure<< " " << length_measure << " " 
  //     << diagnostic_measure << " " << type << " " << cost << vcl_endl;

  map_and_cost.first=cost;
  map_and_cost.second=map;

  /*
  if(TIMING_ON){
  ticks = clock();
  _totalFineScaleMatchTicks += (ticks-startTicks);
    }
      */

  return map_and_cost;
}    


intMap
localReg(Curve<double,double>* cv, Curve<double,double>* mg, 
   double* dis,double* len, double* dgn, int num_iter) {
  
  double dif=DP_VERY_LARGE_COST;
  indexedMeasures d_list;

  //A list of the distance of each point of cv from mg
  vcl_pair<double,int> dist;
  Curve<double,double> last;
  double t_dist=0;
  double length,diag;
  intMap map;
  int iter,i,j,start,index,i2;
  int sec=0;
  
//   int num_pts1=cv->numPoints();
//   int num_pts2=mg->numPoints();
//   bool flip=0;
//
//   if(num_pts1>num_pts2) {
//     flip=1;
//     Curve<double,double> *tmp = cv;
//     cv=mg;
//     mg=tmp;
//   }



  //TIMING_ON variables
  /*
  int ticks, startTicks;
  startTicks = clock();
  */

  double T = LOCAL_REG_THRESH_HIGH;
  double T2 = LOCAL_REG_THRESH_LOW;
  int num = cv->numPoints();
  int ncp = cv->coarseNumPoints();

  for(iter=0;iter<num_iter;iter++) {

    last=*cv;
    t_dist=0.0;
    map.clear();
    d_list.clear();
   
    start=-1;  

    if(ncp==0) {
      for(i=0;i<num;i++)
  d_list.push_back(ptDist(cv->x(i),cv->y(i),mg));
    //Locate the start of the aligning of the curve.
      if(start==-1)
  if(dist.first>T)
    start=i; 
    }
    
    else {
      int end=0;
      int st;
      for(j=0;j<ncp;j++) {
  st=end;
  if(j==ncp-1) end=num;
  else end=cv->coarseRef(j);
  assert(end<=num);
  dist=ptDist(cv->Cx(j),cv->Cy(j),mg);
  if(dist.first<100.0) {
    for(i=st;i<end;i++) {
      dist=ptDist(cv->x(i),cv->y(i),mg);
      d_list.push_back(dist);
      if(start==-1)
        if(dist.first>T)
    start=i;
    }
  }  
  else {
    if(start==-1) 
      start=st; 
    for(i=st;i<end;i++)
      d_list.push_back(vcl_pair<double,int>(100.0,0));
  }
      }
    }
    assert(static_cast<int>(d_list.size())==num);
    
    
    if(start==-1) {
      for(i=0;i<num;i++) {
  map.push_back(vcl_pair<int,int>(i,d_list[i].second));
  t_dist+=d_list[index].first;
      }
    }
    
    else {
      
      double cave;
      double pave=0;
      int flag=0;
      
      for(i=1;i<num+1;i++) {
  index=(i+start)%num;
  assert(index>=0 && index<num);
  
  if(flag==0) {
    if(d_list[index].first<T) {
      flag=1;
      pave=100;
    }
  }
  if(flag==1) {
    //vcl_cout << index << " ";
    cave=(d_list[(index-1+num)%num].first +
    d_list[index].first +
    d_list[(index+1)%num].first)/3.0;
    if((pave-cave)<0.1 || cave<T2) {
      //vcl_cout << vcl_endl << index << "-->";
      sec=0;
      flag=2;
    }
    pave=cave;
  }
  
  if(flag==2) {
    map.push_back(vcl_pair<int,int>(index,d_list[index].second));
    t_dist+=d_list[index].first;
    sec++;
    if(d_list[index].first>T){
      //vcl_cout << index << vcl_endl;
      flag=3;
    }
  }
  if(flag==3) {
    pave=100;
    i2=index;
    int interval=sec;
    for(j=0;j<interval;j++) {
      //vcl_cout << i2 << " ";
      assert(i2>=0 && i2<num);
      cave=(d_list[(i2-1+num)%num].first +
      d_list[i2].first +
      d_list[(i2+1)%num].first)/3.0;
      if((pave-cave)<0.1 || cave<T2)
        break;
      map.pop_back();
      t_dist-=d_list[i2].first;
      sec--;
      i2--;
      i2=(i2+num)%num;
      pave=cave;
    }
    if(sec<5) {
      //vcl_cout << "*** ";
      for(j=0;j<sec;j++) {
        //vcl_cout << i2 << " ";
        map.pop_back();
        t_dist-=d_list[i2].first;
        i2--;
        i2=(i2+num)%num;
      }
    }
    flag=0;
    //vcl_cout << vcl_endl << vcl_endl;
  }
      }
    }
    
    if(map.size()<2) {
      *dis=DP_VERY_LARGE_COST;
      *len=0;
      *dgn=0;
      return map;
    }
    
    t_dist/=map.size();
    
    regContour(cv,mg,map);
    dif=last-*cv;
    if(dif<1.0) 
      break; 
  }

  Curve<double,double> ave;
  int map_size_int = static_cast<int>(map.size());
  for(i=0;i<map_size_int;i++)
    ave.append((cv->x(map[i].first)+mg->x(map[i].second))/2.0,
         (cv->y(map[i].first)+mg->y(map[i].second))/2.0);

  
  double l;
  length=0.0;
  vcl_pair<int,int> endPts;
  for(i=0;i<ave.numPoints()-1;i++)
    for(j=i+1;j<ave.numPoints();j++) {
      l=pointDist(ave.x(i),ave.y(i),ave.x(j),ave.y(j));
      if(l>length) {
  length=l;
  endPts.first=i;
  endPts.second=j;
      }  
    }
  
  intMap dmap;
  Curve<double,double> dummy;
  
  dummy.append(0.0,0.0);
  dummy.append(length,0.0);  
  
  dmap.push_back(vcl_pair<int,int>(endPts.first,0));
  dmap.push_back(vcl_pair<int,int>(endPts.second,1)); 

  regContour(&ave,&dummy,dmap);
  diag=flat(&ave);
  
  *dis=t_dist;
  *len=length;
  *dgn=diag;

//   if(flip)
//     for(int cnt=0;cnt<map.size();cnt++)
//       map[cnt]=vcl_pair<int,int>(map[cnt].second,map[cnt].first);  
/*  
  if(TIMING_ON){
  ticks = clock();
  _totalLocalRegTicks += (ticks-startTicks);
    }
      */

  return map;
}


double detectOverlap(Curve<double,double>* c1, Curve<double,double>* c2) {

  vcl_pair<double,int> dist;
  double m_dist=0.0;
  int num1=c1->coarseNumPoints();
  int num2=c2->coarseNumPoints();
  bool oFlag = 1; //(c1->orient()==c2->orient());

  if(num1>0) {
    
    for(int i=0;i<num1;i++) {
      if(inPolygon(c1->Cx(i),c1->Cy(i),c2)==oFlag) {
      dist=ptDist(c1->Cx(i),c1->Cy(i),c2);
  if(dist.first>m_dist)
    m_dist=dist.first;
      }
    }
  }
  
  else {
   
    for(int i=0;i<c1->numPoints();i++) {
      if(inPolygon(c1->x(i),c1->y(i),c2)==oFlag) {
  dist=ptDist(c1->x(i),c1->y(i),c2);
  if(dist.first>m_dist)
    m_dist=dist.first;
      }
    }  
  }
  
  if(num2>0) {
    
    for(int i=0;i<num2;i++) {
      if(inPolygon(c2->Cx(i),c2->Cy(i),c1)==oFlag) {
  dist=ptDist(c2->Cx(i),c2->Cy(i),c1);
  if(dist.first>m_dist)
    m_dist=dist.first;
      }
    }
  }
  
  else {
    
    for(int i=0;i<c2->numPoints();i++) {
      if(inPolygon(c2->x(i),c2->y(i),c1)==oFlag) {
  dist=ptDist(c2->x(i),c2->y(i),c1);
  if(dist.first>m_dist)
    m_dist=dist.first;
      }
    }
  }

  return m_dist;
} 

bool inPolygon(double x, double y, Curve<double,double>* c) {
  
  double t_angle;
  double p1x,p1y,p2x,p2y;
  int num=c->coarseNumPoints();
  
  t_angle=0;

  if(num>0) {

    for(int i=0;i<num;i++) {
      p1x=c->Cx(i%num);
      p1y=c->Cy(i%num);
      p2x=c->Cx((i+1)%num);
      p2y=c->Cy((i+1)%num);
      
      t_angle+=angleDiff(atan2(p2y-y,p2x-x),atan2(p1y-y,p1x-x));
    }
  }

  else {
    
    num=c->numPoints();
    
    for(int i=0;i<num;i++) {
      p1x=c->x(i%num);
      p1y=c->y(i%num);
      p2x=c->x((i+1)%num);
      p2y=c->y((i+1)%num);
      
      t_angle+=angleDiff(atan2(p2y-y,p2x-x),atan2(p1y-y,p1x-x));
    }
  }
  
  assert((fabs(t_angle)>6.0 && fabs(t_angle)<6.5) || fabs(t_angle)<0.1);
  return fabs(t_angle)>1.0;
}

vcl_pair<double,int> ptDist(double x, double y, Curve<double,double>* c) {

  double d;
  vcl_pair<double,int> dist;
  int i;
  int num=c->coarseNumPoints();
  
  //vcl_cout << num << " ";

  if(num==0) {
    
    dist.first=pointDist(x,y,c->x(0),c->y(0));
    dist.second=0;
    for(i=1;i<c->numPoints();i++) {
      d=pointDist(x,y,c->x(i),c->y(i));
      if(d<dist.first) {
  dist.first=d;
  dist.second=i;
      }   
    }  
  }
  
  //vcl_cout << dist.second;

  else {
  //if(num>0) {

    dist.first=pointDist(x,y,c->Cx(0),c->Cy(0));
    dist.second=0;
    for(i=1;i<num;i++) {
      d=pointDist(x,y,c->Cx(i),c->Cy(i));
      if(d<dist.first) {
  dist.first=d;
  dist.second=i;
      }
    }
    
    //vcl_cout << " " << c->coarseRef(dist.second);

    int start=c->coarseRef((dist.second-1+num)%num);
    int end=c->coarseRef((dist.second+1)%num);
    num=c->numPoints();
    if(start>end) end+=num;
    
    dist.first=pointDist(x,y,c->x(start),c->y(start));
    dist.second=start;
    for(i=start+1;i<end;i++) {
      d=pointDist(x,y,c->x(i%num),c->y(i%num));
      if(d<dist.first) {
  dist.first=d;
  dist.second=i%num;
      }
    }
  }
  //vcl_cout << vcl_endl;
  return dist;
}
 
//definition in PuzzleSolving.h, bool flip = 0
XForm3x3 regContour(Curve<double,double> *c1, Curve<double,double> *c2, intMap map, bool flip, XForm3x3 *out) {
  
  if(map.size()<2)
    return XForm::identity();

  if(flip) {
    Curve<double,double> *ct = c1;
    c1=c2;
    c2=ct;
    for(unsigned cnt=0;cnt<map.size();cnt++)
        map[cnt]=vcl_pair<int,int>(map[cnt].second,map[cnt].first);
  }

  vcl_vector<PuzPoint<double> > p1;
  vcl_vector<PuzPoint<double> > p2;  
  double c1x=0,c1y=0,c2x=0,c2y=0; 
  double h1=0,h2=0,h3=0,h4=0;
  double a,b,theta,Tx,Ty;
  int i,s;

  s=map.size();
  
  for(i=s-1;i>=0;i--) {
    p1.push_back(c1->point(map[i].first));
    p2.push_back(c2->point(map[i].second));
  }

  for(i=0;i<s;i++) {
    c1x+=p1[i].x();
    c1y+=p1[i].y();
    c2x+=p2[i].x();
    c2y+=p2[i].y();
  }
  c1x/=s;c1y/=s;c2x/=s;c2y/=s;

  for(i=0;i<s;i++) {
    p1[i]=PuzPoint<double>(p1[i].x()-c1x,p1[i].y()-c1y);
    p2[i]=PuzPoint<double>(p2[i].x()-c2x,p2[i].y()-c2y);
  }

  for(i=0;i<s;i++) {
    h1+=p1[i].x()*p2[i].x();
    h2+=p1[i].x()*p2[i].y();
    h3+=p1[i].y()*p2[i].x();
    h4+=p1[i].y()*p2[i].y();
  }

  a=h2-h3;
  b=h1+h4;
  
  theta=atan2(-a,b); 
  Tx=c2x - ( cos(theta)*c1x + sin(theta)*c1y);
  Ty=c2y - (-sin(theta)*c1x + cos(theta)*c1y);
  
  c1->rotateTranslate(theta,Tx,Ty);

  //converting theta in radians to degrees
  theta *= 57.2957;

  XForm3x3 axf, bxf;
  axf.translateMe(Tx,Ty);
  bxf.rotateMe(-theta);

  XForm3x3::mult( axf, bxf );

  if(out){
      XForm3x3::deep_copy(out, axf);
  }
  
  return axf;
}


double flat(Curve<double,double> *c) {

  int i,n;
  double a,b,x,y;
  double mean_x=0;
  double mean_y=0;
  double xx=0;
  double xy=0;
  double Err=0;

  n=c->numPoints();

  for(i=0;i<n;i++) {
    x=c->x(i);
    y=c->y(i);
    mean_x+=x;
    mean_y+=y;
    xx+=x*x;
    xy+=x*y;
  }
  mean_x/=n;
  mean_y/=n;

  a=(mean_y*xx-mean_x*xy)/(xx-n*mean_x*mean_x);
  b=(xy-n*mean_x*mean_y) /(xx-n*mean_x*mean_x);

  for(i=0;i<n;i++) {
    Err+=fabs(c->y(i)-(a+b*c->x(i)));
  }
  Err/=n;
  
  return Err;
}

/*
void imSmooth(RGBAImage *img, int window, 
        vcl_vector<double> *edge_x, vcl_vector<double> *edge_y,
        vcl_vector<double> *lum) {

  int i,j,k;
  int ref1,ref2,ref3;
  
  int osA=3;
  int osR=2;
  int osG=1;
  int osB=0;

  unsigned char* dat=img->data();
  int height=img->height();
  int width=img->width();
  vcl_vector<double> gauss;
  vcl_vector<unsigned char> temp1,temp2;
  double r,g,b;
  double weight,t_weight;

  for(i=0;i<=2*window;i++)
    gauss.push_back(exp(-(pow((double)(i-window),2)/(2*pow((double)window/3.0,2)))));
  
  for(i=0;i<width*height*4;i++)
    temp1.push_back(dat[i]);

  temp2=temp1;

  for(i=window;i<height-window;i++)
    for(j=window;j<width-window;j++) {
      
      r=0;g=0;b=0;
      t_weight=0;

      ref1=(i*width+j)*4;
      if(temp1[ref1+osA]==255) {
  for(k=-window;k<=window;k++) {

      ref2=((i)*width+(j+k))*4;
      if(temp1[ref2+osA]==255) {

        weight=gauss[k+window];
        r+=(double)temp1[ref2+osR]*weight;
        g+=(double)temp1[ref2+osG]*weight;
        b+=(double)temp1[ref2+osB]*weight;
        t_weight+=weight;
      }
    }
  temp2[ref1+osR]=r/t_weight;
  temp2[ref1+osG]=g/t_weight;
  temp2[ref1+osB]=b/t_weight;
      }
    }
  
  for(i=window;i<height-window;i++)
    for(j=window;j<width-window;j++) {
      
      r=0;g=0;b=0;
      t_weight=0;
      
      ref1=(i*width+j)*4;
      if(temp2[ref1+osA]==255) {
  for(k=-window;k<=window;k++) {

      ref2=((i+k)*width+(j))*4;
      if(temp2[ref2+osA]==255) {
        
        weight=gauss[k+window];
        r+=(double)temp2[ref2+osR]*weight;
        g+=(double)temp2[ref2+osG]*weight;
        b+=(double)temp2[ref2+osB]*weight;
        t_weight+=weight;
      }
    }
  temp1[ref1+osR]=r/t_weight;
  temp1[ref1+osG]=g/t_weight;
  temp1[ref1+osB]=b/t_weight;
      }
    }

  double rx,gx,bx,ry,gy,by,ex,ey,e_mag;
  double ev=config::val("EDGE_VIEW",10.0f);

  for(i=0;i<height;i++)
    for(j=0;j<width;j++) {
      edge_x->push_back(0.0);
      edge_y->push_back(0.0);
      lum->push_back(0.0);
    }
      

  for(i=1;i<height;i++)
    for(j=1;j<width;j++) {
      ref1=(i*width+j)*4;
      ref2=(i*width+(j-1))*4;
      ref3=((i-1)*width+j)*4;
      if(temp1[ref1+osA]==255 && temp1[ref2+osA]==255 && temp1[ref3+osA]==255) {
  rx=(double)temp1[ref1+osR]-(double)temp1[ref2+osR];
  gx=(double)temp1[ref1+osG]-(double)temp1[ref2+osG];
  bx=(double)temp1[ref1+osB]-(double)temp1[ref2+osB];
  ry=(double)temp1[ref1+osR]-(double)temp1[ref3+osR];
  gy=(double)temp1[ref1+osG]-(double)temp1[ref3+osG];
  by=(double)temp1[ref1+osB]-(double)temp1[ref3+osB];

  ex=0.3*rx+0.59*gx+0.11*bx;
  ey=0.3*ry+0.59*gy+0.11*by;

  e_mag=sqrt(ex*ex+ey*ey);

  dat[ref1+osR]=e_mag*ev;
  dat[ref1+osG]=e_mag*ev;
  dat[ref1+osB]=e_mag*ev;
  
  (*edge_x)[i*width+j]=ex*ev;
  (*edge_y)[i*width+j]=ey*ev;
  (*lum)[i*width+j]=(0.3*(double)temp1[ref1+osR]+
         0.59*(double)temp1[ref1+osG]+
         0.11*(double)temp1[ref1+osB]);
      }  
    }
}
*/

double const_test(XForm3x3 T, Vect3 P1, Vect3 P2, Vect3 P3, Vect3 P4) {

  double d;
  double scale=(dist(P1,P2)+dist(P1,P3)+dist(P1,P4)+
    dist(P2,P3)+dist(P2,P4)+dist(P3,P4))/6.0;
  
  Vect3 P1p=T*P1;
  Vect3 P2p=T*P2;
  Vect3 P3p=T*P3;
  Vect3 P4p=T*P4;

  d=(dist(P1,P1p)+dist(P2,P2p)+dist(P3,P3p)+dist(P4,P4p))/scale;
  
  //vcl_cout << "FIGS: " << d*scale << " " << scale << " " << d << vcl_endl;

  d=exp(-d);
  return d;
}

vcl_pair<double,double> center(Curve<double,double> *c) {

  double mx=0;
  double my=0;
  int N=c->coarseNumPoints();
  
  for(int i=0;i<N;i++) {
    mx+=c->Cx(i);
    my+=c->Cy(i);
  }

  mx/=N;
  my/=N;

  return vcl_pair<double,double>(mx,my);
}



int edgeCheck(Curve<double, double>* c1, Curve<double, double>* c2, intMap map){

  //Initialize variables
  int startIndx1 = map[0].first;
  int startIndx2 = map[0].second;
  int endIndx1 = map[map.size()-1].first;
  int endIndx2 = map[map.size()-1].second;
  int numPts1 = c1->numPoints();
  int numPts2 = c2->numPoints();
  int numberContinuousEdges=0;
  double angle1=0;
  double angle2=0;
  //Boolean variables that indicate whether the map moves in forward or reverse order for each curve
    //unreferenced: -spinner-
  //bool movesForward1;
  //bool movesForward2;

  //Move the two contours close to one another
  regContour(c1,c2,map);

  c1->computeProperties();
  c2->computeProperties();

 



  //If the intMap goes forward for c1 move back along the contour
  //and find the average of the angles of the tangents over the curve.
  //if (movesForward1){
    int startTestIndx1 = startIndx1 - static_cast<int>(MOVE_BACK);
    while (startTestIndx1 < 0)
      startTestIndx1 += numPts1;
    if (startTestIndx1 >= numPts1)
      startTestIndx1 %=numPts1;

    for (int i =0; i < NUM_POINTS_AVERAGED; i++){
      if (startTestIndx1+i+1>numPts1)
        startTestIndx1=-i;
      angle1 += c1->angle(startTestIndx1+i);

    


    }
    angle1 = angle1/NUM_POINTS_AVERAGED;
    if (angle1 < -M_PI)
      angle1=2*M_PI-angle2;
    if (angle1 < 0)
      angle1 +=M_PI;
    if (angle1 >= M_PI)
      angle1 -= M_PI;

    
    int startTestIndx2 = startIndx2 + static_cast<int>(MOVE_BACK);
    if (startTestIndx2 >= numPts2)
      startTestIndx2 %= numPts2;

    for (int k =0; k > -NUM_POINTS_AVERAGED; k--){
      if (startTestIndx2+k < 0)
        startTestIndx2 = numPts2-k-1;
      angle2 += c2->angle(startTestIndx2+k);

      }

    angle2 = angle2/NUM_POINTS_AVERAGED;
    if (angle2 < -M_PI)
      angle2=2*M_PI-angle2;
    if (angle2 < 0)
      angle2 +=M_PI;
    if (angle2 >= M_PI)
      angle2 -= M_PI;


if ((anglesAreClose(angle1, angle2))&&(distanceCheck(c1, c2, map[0].first, map[0].second)))
   numberContinuousEdges++;
    

 angle1 = 0;
 angle2 = 0;

 
    int endTestIndx1 = endIndx1 + static_cast<int>(MOVE_BACK);
    if (endTestIndx1 >= numPts1)
      endTestIndx1 %= numPts1;

    for (int i2 =0; i2 > -NUM_POINTS_AVERAGED; i2--){
      if (endTestIndx1+i2 < 0)
        endTestIndx1 = numPts1-i2-1;
      angle1 += c1->angle(endTestIndx1+i2);
      }

    angle1 = angle1/NUM_POINTS_AVERAGED;
    if (angle1 < -M_PI)
      angle1=2*M_PI-angle2;
    if (angle1 < 0)
      angle1 +=M_PI;
    if (angle1 >= M_PI)
      angle1 -= M_PI;

    
    int endTestIndx2 = endIndx2 - static_cast<int>(MOVE_BACK);
    while (endTestIndx2 < 0)
      endTestIndx2 += numPts2;
    if (endTestIndx2 >= numPts2)
      endTestIndx2 %=numPts2;

    for (int k2 =0; k2 < NUM_POINTS_AVERAGED; k2++){
      if (endTestIndx2+k2+1>numPts2)
        endTestIndx2=0-k2;
      angle2 += c2->angle(endTestIndx2+k2);
    }
    angle2 = angle2/NUM_POINTS_AVERAGED;

    
    if (angle2 < -M_PI)
      angle2=2*M_PI-angle2;
    if (angle2 < 0)
      angle2 +=M_PI;
    if (angle2 >= M_PI)
      angle2 -= M_PI;
      

 if ((anglesAreClose(angle1, angle2))&&(distanceCheck(c1, c2, map[map.size()-1].first, map[map.size()-1].second)))
   numberContinuousEdges++;
   



 return numberContinuousEdges;


}

//Checks to see whether the two corners being dealt with are close enough together.
//First it checks to see whether there are two extrema near enough to each other
//and then determines whether the extrema on the first curve is close enough to the
//beginning of match.
bool distanceCheck(Curve<double, double> *c1, Curve<double, double> *c2, int checkIndex1, int checkIndex2){


  PuzPoint<double> checkPoint1 = c1->point(checkIndex1);
  PuzPoint<double> checkPoint2 = c2->point(checkIndex2);

  //Cycles through all combinations of corners in order to compare their distances from
  //one another.
  for (int i=0; i < c1->numExtrema(); i++){
    for (int j=0; j < c2->numExtrema(); j++){

      int index = c1->coarseRef(c1->extrema(i));
      PuzPoint<double> point1 = c1->point(index);
      PuzPoint<double> point2 = c2->point(c2->coarseRef(c2->extrema(j)));

      //The distance of this pair of corners from each other
      double distance1 = 
        sqrt((point1.x()-point2.x())*(point1.x()-point2.x())+(point1.y()-point2.y())*(point1.y()-point2.y()));

      //The distance of the corner on contour 1 from the start of the match
      double distance2 = 
        sqrt((point1.x()-checkPoint1.x())*(point1.x()-checkPoint1.x())
        +(point1.y()-checkPoint1.y())*(point1.y()-checkPoint1.y()));

      //The distance of the corner on contour 1 from the start of the match
      double distance3 = 
        sqrt((point2.x()-checkPoint2.x())*(point2.x()-checkPoint2.x())
        +(point2.y()-checkPoint2.y())*(point2.y()-checkPoint2.y()));

      //If the distances are small, return 1
      if ((distance1 < DISTANCE_THRESH_1)&&((distance2 < DISTANCE_THRESH_2)||(distance3 < DISTANCE_THRESH_2)))
        return 1;
      

    }
  }

  return 0; 
}


//Determines whether two angles in radians are within a certain distance of each other
bool anglesAreClose(double angle1, double angle2){

  //Make sure that the angles are within the range of 0 to 2*pi
  while(angle1<0)
    angle1+=2*M_PI;
  while(angle2<0)
    angle2+=2*M_PI;
  while(angle1>2*M_PI)
    angle1-=2*M_PI;
  while(angle2>2*M_PI)
    angle2-=2*M_PI;

  //Check to see if the angles are currently lined up
  if ((angle2>(angle1-CLOSE_ANGLE_THRESH))&&(angle2<(angle1+CLOSE_ANGLE_THRESH)))
    return 1;

  if ((fabs(angle1-angle2)<(M_PI+CLOSE_ANGLE_THRESH))&&(fabs(angle1-angle2)>(M_PI-CLOSE_ANGLE_THRESH)))
    return 1;

  return 0;


}



