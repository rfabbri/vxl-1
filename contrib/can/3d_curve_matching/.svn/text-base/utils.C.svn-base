#include "utils.h"
#define ARCLENSAMPLE 0.1

template <class Type>
Type fixAngleMPiPi(Type a){
  if (a < -M_PI)
    return a+2*M_PI;
  else if (a > M_PI)
    return a-2*M_PI;
  else
    return a;
}

template <class Type>
Type fixAngleZTPi(Type a){
  if (a < 0)
    return a+2*M_PI;
  else if (a > 2*M_PI)
    return a-2*M_PI;
  else
    return a;
}

//Does a1-a2
template <class Type>
Type angleDiff(Type a1, Type  a2){
  a1=fixAngleMPiPi(a1);
  a2=fixAngleMPiPi(a2);
  if (a1 > a2)
    if (a1-a2 > M_PI)
      return a1-a2-2*M_PI;
    else
      return a1-a2;
  else if (a2 > a1)
    if (a1-a2 < -M_PI)
      return a1-a2+2*M_PI;
    else
      return a1-a2;
  return 0.0;
}


template <class Type>
Type angleAdd(Type a1, Type  a2){
  Type a=a1+a2;
  
  if (a > M_PI)
    return a-2*M_PI;
  if (a < -M_PI)
    return a+2*M_PI;
  else
    return a;
}

template<class floatType>
floatType pointDist(Point<floatType>a ,Point<floatType>b){
  return sqrt(pow(a.x()-b.x(),2.0)+pow(a.y()-b.y(),2.0));
}

map<string, vector<string> > readParamters(int argc, char *argv[]){
  int i=1;
  map<string, vector<string> > params;
  string key;
  vector<string> args;

  while(i<argc){
    
    if (argv[i][0] == '-'){
      if (i>1 && key.length()>0)
	params[key]=args;
      args.clear();
      key=argv[i];
    }
    else{
      args.push_back(argv[i]);
    }
    i++;
  }
  if (i>1 && key.length()>0)
    params[key]=args;
  

  map<string, vector<string> >::iterator iter;
  for(iter=params.begin();iter != params.end();iter++){
    cout << "key " << (*iter).first << endl;
    for (int j=0;j<(*iter).second.size();j++)
      cout << (*iter).second[j] << " ";
    cout << endl;
  }
  return params;
}

//vector<string>  readParamters(int argc, char *argv[])
// {
//   int i;
//   cout  << "Reading paramters" << endl;
//   // String of paramters: [file1,file2,n11,n12,n21,n22,R1,R2,scale]
//   vector<string> params;
//   for (i=0;i<9;i++)
//     params.push_back("0");
  
//   //cout << argc << endl;
//   //fflush(stdout);
//   //for (i=0;i<argc;i++)
//   //  cout << argv[i] << endl;
//   //fflush(stdout);
//   for (--argc, ++argv; argc; --argc, ++argv ) {
//     if ( **argv == '-' ) {  
//       ++*argv;
//       --argc;
//       //cout << *argv << endl;
//       if (strcmp( *argv, "f1")== 0){
// 	//cout << *argv << endl;
// 	//cout << *++argv << endl;
// 	string tmp(*++argv);
// 	params[0] = tmp;
//       }
//       else if (strcmp( *argv, "f2")== 0){
// 	++argv;
// 	//cout << *argv << endl;
// 	params[1].assign(*argv);
//       }
//       else if (strcmp( *argv, "nodes")== 0){
// 	params[2].assign(*++argv);
// 	params[3].assign(*++argv);
// 	params[4].assign(*++argv);
// 	params[5].assign(*++argv);
//       }
//       else if (strcmp( *argv, "R1")== 0)
// 	params[6].assign(*++argv);
//       else if (strcmp( *argv, "R2")== 0)
// 	params[7].assign(*++argv);
//       else if (strcmp( *argv, "scale")== 0)
// 	params[8].assign(*++argv);
      
//     }
//   }
//   //for (i=0;i<9;i++)
//   //  cout << i << " - " << params[i] << endl;
//   return params;
// }

template<class floatType>
floatType strainCost(floatType a1,floatType a2,floatType b1,floatType b2,
		     floatType c1,floatType c2,floatType A1,floatType A2,
		     floatType B1,floatType B2,floatType C1,floatType C2){

  floatType lambda=1E-2,mu=1E-2;
  floatType Area,U;
  floatType e11,e22,e12;

  //cout << a1 << " " << a2 << endl;
  //cout << b1 << " " << b2 << endl;
  //cout << c1 << " " << c2 << endl;

  //   Area = fabs((b1*c2-b2*c1)+a1*(b2-c2)+a2*(c1-b1));
  //   e11 = fabs((b2-c2)*(A1-a1) + (c2-a2)*(B1-b1) + (a2-b2)*(C1-c1));
  //   e22 = fabs((c1-b1)*(A2-a2) + (a1-c1)*(B2-b2) + (b1-a1)*(C2-c2));
  //   e12 = fabs((c1-b1)*(A1-a1) + (a1-c1)*(B1-b1) + (b1-a1)*(C1-c1)+
  // 	 (b2-c2)*(A2-a2) + (c2-a2)*(B2-b2) + (a2-b2)*(C2-c2));
  Area = ((b1*c2-b2*c1)+a1*(b2-c2)+a2*(c1-b1));
  e11 = ((b2-c2)*(A1-a1) + (c2-a2)*(B1-b1) + (a2-b2)*(C1-c1));
  e22 = ((c1-b1)*(A2-a2) + (a1-c1)*(B2-b2) + (b1-a1)*(C2-c2));
  e12 = ((c1-b1)*(A1-a1) + (a1-c1)*(B1-b1) + (b1-a1)*(C1-c1)+
	 (b2-c2)*(A2-a2) + (c2-a2)*(B2-b2) + (a2-b2)*(C2-c2));
  printf("A=%7.2f E11=%7.2f E2=%7.2f E12=%7.2f \n",Area,e11,e22,e12);
  printf("a: (%6.2f,%6.2f) (%6.2f,%6.2f) (%6.2f,%6.2f) %6.2f\n",a1,a2,b1,b2,c1,c2,Area);
  printf("A: (%6.2f,%6.2f) (%6.2f,%6.2f) (%6.2f,%6.2f) %6.2f\n",A1,A2,B1,B2,C1,C2,(B1*C2-B2*C1)+A1*(B2-C2)+A2*(C1-B1));
  if (Area == 0){
    U=0.0;
    printf("a: (%6.2f,%6.2f) (%6.2f,%6.2f) (%6.2f,%6.2f) %6.2f\n",a1,a2,b1,b2,c1,c2,Area);
    printf("A: (%6.2f,%6.2f) (%6.2f,%6.2f) (%6.2f,%6.2f) %6.2f\n",A1,A2,B1,B2,C1,C2,(B1*C2-B2*C1)+A1*(B2-C2)+A2*(C1-B1));
  }
  else
    U=((lambda+2*mu)*(pow(e11,2)+pow(e22,2))+2*lambda*e11*e22+4*mu*pow(e12,2))/Area;
  
  cout << Area << " " << U << " " << endl;
  return U;
}

// template<class ptType, class floatType>
// void interpolateCurves(const Curve<ptType,floatType> &old, Curve<ptType,floatType> &interp){

//   int i;
//   int numSamples;

//   int numPoints=old.numPoints();
//   floatType ratio;

//   int interpPoints=0;
//   for (i=1;i<numPoints;i++){
//     floatType currDs = old.arcLength(i)-old.arcLength(i-1);
//     if (currDs>ARCLENSAMPLE){
//       numSamples=currDs/ARCLENSAMPLE;
//       for (j=1;j<=numSamples-1;j++){
	
//       }
//     }
//   }
// }
