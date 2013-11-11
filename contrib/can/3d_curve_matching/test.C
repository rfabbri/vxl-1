#include "test.h"
//int X_OFFSET[9] = {0,-1,-1,-2,-1,-3,-2,-3,-1};
//int Y_OFFSET[9] = {-1,-3,-2,-3,-1,-2,-1,-1,0};
#define LARGE_COST 170
#define LARGE_COST_RATIO 0.08
#define LARGE_BOUNDARY_COST 100

void main (int argc, char *argv[])
{
  testDP(argc,argv);

}


void testDP(int argc, char *argv[])
{
  long int ticks;
  long int startTicks;
  double seconds;
  time_t tt1;
  int t1,t2;
  t1 = time(&tt1);
  startTicks = clock();

  cout << "Curve matching" << endl;

  int i;
  Curve<double,double> c1;
  Curve<double,double> c2;
  string fname1(argv[1]);
  string fname2(argv[2]);
  c1.readDataFromFile(fname1);
  c2.readDataFromFile(fname2);


  //compute curvature, torsion
  c1.computeProperties();

  cout <<endl;

  c2.computeProperties();
  
 
  cout << c1.numPoints() << " " << c2.numPoints() << endl;
  cout << "length "<<c1.length() << " " << c2.length() << endl;
  cout << "angle "<<c1.totalAngleChange() << " " << c2.totalAngleChange() << endl;
  

  int l1,l2;
  FILE *fp;
  string datafname;
  
  l1=fname1.find_last_of('/');
  l2=fname1.find_last_of('.');
  datafname=fname1.substr(l1+1,l2-l1-1);
  datafname+="-data.txt";
  //ofstream outfp(mapfname.c_str());
  fp=fopen(datafname.c_str(),"w");
  printf("store Curve 1\n");
  for (i=1;i<c1.numPoints();i++)
    fprintf(fp,"%d %6.3f %6.3f %6.3f %6.3f\n",i,c1._ptArray[i].x(),c1._ptArray[i].y(),
	   c1.angle(i),c1._curvature[i]);

  fclose(fp);

  l1=fname2.find_last_of('/');
  l2=fname2.find_last_of('.');
  datafname=fname2.substr(l1+1,l2-l1-1);
  datafname+="-data.txt";
  fp=fopen(datafname.c_str(),"w");
  printf("store Curve 2\n");
  for (i=1;i<c2.numPoints();i++)
    fprintf(fp,"%d %6.3f %6.3f %6.3f %6.3f\n",i,c2._ptArray[i].x(),c2._ptArray[i].y(),
	   c2.angle(i),c2._curvature[i]);
  
  fclose(fp);

  //DPMatch<Curve<double,double>,double> d1(c1,c2);

  
  vector <double> lambda;
  lambda.push_back(atof(argv[4]));
  NewDPMatch<Curve<double,double>,double> d1(c1,c2,atof(argv[3]),0,lambda,1);
  //CohenDPMatch<double,double> d1(c1,c2);
  
  //cout << d1.n() << " " << d1.m() << endl;

  cout << "do the match"<<endl;
  d1.match();


  cout << endl<<"After Match " << endl;
  //d1.printCost();
  //d1.printMap();
  vector< pair <int,int> > fmap = d1.finalMap();
  vector <double> fmapCost = d1.finalMapCost();

  l1=fname1.find_last_of('/');
  l2=fname1.find_last_of('.');
  string mapfname=fname1.substr(l1+1,l2-l1-1);
  //mapfname.assign(fname1,fname1.size()-4);
  mapfname += '-';
  l1=fname2.find_last_of('/');
  l2=fname2.find_last_of('.');
  mapfname.append(fname2.substr(l1+1,l2-l1-1));
  cout << mapfname << endl;


  string costfname=mapfname;
  //string costarrfname=mapfname;


  mapfname += "-match.txt";
  ofstream outfp(mapfname.c_str());
  
  //cout << "Final Cost = " << d1.finalCost() << endl;
  cout << mapfname << " Final Cost = " << d1.finalCost() << endl;
  outfp << "Final Cost = " << d1.finalCost() << endl;
  for (i=0; i<fmap.size(); i++){
    //cout << fmap[i].first << " " << fmap[i].second << endl;
    outfp << fmap[i].first << " " << fmap[i].second << endl;
  }
  outfp.close();
  
  costfname += "-cost.txt";
  ofstream costfp(costfname.c_str());
  for (i=0; i<fmapCost.size()-1; i++){
    //cout << fmap[i].first << " " << fmap[i].second << endl;
    costfp << fmap[i].first << " "<< fmap[i].second << " "<<fmapCost[i]-fmapCost[i+1] << " "<<(d1.finalCost1())[i]<<endl;
    //costfp << fmapCost[i] << endl;

  }
  costfp << 0<<" "<<0<<" "<<fmapCost[fmapCost.size()-1] << " "<< (d1.finalCost1())[fmapCost.size()-1]<<endl;
  costfp.close();
  

  //costarrfname += "-cost.raw";
  //d1.writeCost(costarrfname);

  t2 = time(&tt1);
  ticks = clock();
  seconds = (double) (ticks-startTicks)/CLOCKS_PER_SEC;  
  cout << "Time : " << seconds << endl;
  cout << "Time2 : " << (t2-t1) << endl;

}

