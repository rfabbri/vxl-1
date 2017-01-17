#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vcl_cstdio.h>

#include "Curve.h"
#include "DPMatch.h"
#include "NewDPMatch.h"

static void test_crvmtch3D()
{
  double R1 = 1.0; //curvature coefficient
  double R2 = 0.1; //torsion coefficient
  vcl_vector<double> lambda;
  lambda.push_back(1.0);

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
  string fname1("D:/MyDocs/courses/en256/project/code-and-data/helix/mid/helix_c10_r10.con");
  string fname2("D:/MyDocs/courses/en256/project/code-and-data/helix/mid/helix_c10_r10_uneven.con");
  c1.readDataFromFile(fname1);
  c2.readDataFromFile(fname2);

//  FILE *fid = vcl_fopen("D:/MyDocs/courses/en256/project/code-and-data/helix/test.txt", "w");

  //compute curvature, torsion
  c1.computeProperties();

  vcl_cout << "Printing curvature" << vcl_endl;
  for(int i=0; i<c1.numPoints(); i++)
    vcl_cout << c1.curvature(i) << vcl_endl;

  vcl_cout << "Printing torsion" << vcl_endl;
  for(int i=0; i<c1.numPoints(); i++)
    vcl_cout << c1.torsion(i) << vcl_endl;

//  vcl_fclose(fid);

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

  NewDPMatch<Curve<double,double>,double> d1(c1, c2, R1, R2, lambda, 1);

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

//  string mapfname2 = "D:\\MyDocs\\courses\\en256\\project\\data\\match.txt";


  mapfname += "-match.txt";
  ofstream outfp(mapfname.c_str());

  //cout << "Final Cost = " << d1.finalCost() << endl;
  cout << mapfname << " Final Cost = " << d1.finalCost() / ((c1.length() + c2.length())/20) << endl;
  outfp << "Final Cost = " << d1.finalCost() / ((c1.length() + c2.length())/20) << endl;
  outfp << fmap.size() << endl;
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

TESTMAIN(test_crvmtch3D);
