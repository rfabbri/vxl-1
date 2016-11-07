int tree(vnl_matrix <double> P,double HoC, vnl_vector <float>V,  vcl_vector <int>VC) 
{


  vnl_vector <double> v(3,0);
  vcl_vector <vnl_vector <double> > C0,C1,C2;

  vcl_vector <vcl_vector <vnl_vector <double> > >C;
  
  

  double R_HoC=0.0;
  
  
  double x01=vcl_fabs(P(1,0)-P(0,0));
  double x07=vcl_fabs(P(7,0)-P(0,0));
  double x79=vcl_fabs(P(7,0)-P(9,0));
  double real_length=vcl_fabs(P(0,0)-P(7,0));
  
 ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  vcl_cout<<"recognizer"<<vcl_endl;
  vcl_cout<<HoC<<vcl_endl;                         ///*******
  vcl_cout<<P<<vcl_endl;                           ///*******
  //vcl_cout<<P(7,0)<<vcl_endl;                    ///*******
 // vcl_cout<<P(9,0)<<vcl_endl;                    ///*******
  vcl_cout<<x01<<" "     ;                         ///*******
  vcl_cout<<x07<<" "     ;                         ///*******
  vcl_cout<<x79<<vcl_endl;                         ///*******
  vcl_cout<<x01/vcl_fabs(P(0,0)-P(7,0))<<" ";      ///*******
  vcl_cout<<P(7,2)/HoC<<vcl_endl;                  ///*******
  vcl_cout<<"======"<<vcl_endl;                  ///*******
  ////////////////////////////////////////////////////*******


  vnl_vector <int>choice(VC.size());
 // vcl_cerr<<VC[0]<< " ";
  int index=0;
  for (unsigned i=0;i<VC.size();i++) {
    int min=5000000;
    int min_j=100;
    for (unsigned j=0;j<VC.size();j++) {

      if (VC[j]<min) {

        min=VC[j];
        min_j=j;
      }
    }
   // vcl_cerr<<min_j<<" ";
    VC[min_j]=10000000;
    choice[min_j]=index;
    index++;
  }

 // for (unsigned i=0;i<choice.size();i++) 
  //  vcl_cerr<<choice[i]<< " ";
 // vcl_cerr<<vcl_endl;

  float decision=999;
  
  for (unsigned Ci=0;Ci<14;Ci++) {
  if (choice[Ci] ==0 &&x01/vcl_fabs(P(0,0)-P(7,0))>.20+V[0] && HoC-.9 < 4.0+V[1]) decision = 00;
  // else if (HoC<4.4) decision=8;
   if ( choice[Ci] ==1  && x01/vcl_fabs(P(0,0)-P(7,0))>.16+V[2] && HoC-.9 < 3.5+V[3]) decision = 01; // big offset 0 and 1
   if ( choice[Ci] ==2  && HoC < 4.0+V[4]) decision = 02;                                    // small height
   if ( choice[Ci] ==3  && (HoC-.9)/vcl_fabs(P(0,0)-P(7,0)) < .2+V[5]) decision = 03;           //height/length

  //else if ((HoC-.9) < 4) decision = 04;
  //if (real_length<12) decision = 02;

   if ( choice[Ci] ==4  && HoC>7+V[6]) decision = 28; 
   if ( choice[Ci] ==5  && HoC-.9>5.9+V[7]) decision = 23;  // if too high, van..

  

  //if (x01/vcl_fabs(P(0,0)-P(7,0))>.3 && HoC-.9 < 4.5) decision = 05;
   if ( choice[Ci] ==6  && P(7,2)>3.5&&HoC<4.5+V[8]) decision = 17;
  // else if ((P(7,2))/(HoC)>.6) decision = 18;
   if ( choice[Ci] ==7  && (P(7,2))/(HoC-.9)>.95+V[9]) decision = 19;
  // if ((P(7,0)-.9)/(HoC-.9)<.4) decision = 29;
  //// else if (P(7,2)/HoC>.5) decision = 19;



  // else if (P(3,2)<1.0) decision = 20;  //  rear botom low mini van
   if ( choice[Ci] ==8  && HoC>6.5+ V[20] /**added later :was typo **/) decision=25; //this is good
  //else if (x07/vcl_fabs(P(0,0)-P(7,0))<.2 && HoC> 5.5) decision = 27;//bad 11-1-2006

   if ( choice[Ci] ==9  && (P(7,2))/(HoC-.9)>.75+V[10]&&HoC<6.0&&HoC>5+V[11]) decision = 16;
   if ( choice[Ci] ==10  && P(7,2)/HoC<.4 +V[12]&&HoC>6+V[13]) decision = 29; 
  //else if (P(3,2)>2.2) decision = 10;  // rear botom high suv

   if ( choice[Ci] ==11  && (P(7,2))/(HoC-.9)>.65+V[14] && vcl_fabs(P(0,0)-P(7,0)+V[15])>12 ) decision = 16.1;
   if ( choice[Ci] ==12  && x01/vcl_fabs(P(0,0)-P(7,0))>.20+V[16] && HoC-.9 < 4.5+V[17]) decision = 0.1;//위치와 순서와 값 매우 좋음...
   if ( choice[Ci] ==13  && x01/vcl_fabs(P(0,0)-P(7,0))>.20 +V[18]&& HoC-.9 < 5.5+V[19]) decision = 0.2;//나쁨?


  //if (choice[Ci]==14) decision=10.1;
  if (decision!=999) return decision;
  }
  //decision=888;
  decision=10.1;
  //vcl_cerr<<decision<<" ";
  return (decision);



}

///////////////////////////////////////////////////////////
//
//    recognizer
//
///////////////////////////////////////////////////////////

//*//int recognizer(vnl_matrix <double> P,double HoC,char* argv)
int recognizer(vnl_matrix <double> P,double HoC)
{
  vnl_vector <double> v(3,0);
  vcl_vector <vnl_vector <double> > C0,C1,C2;

  vcl_vector <vcl_vector <vnl_vector <double> > >C;
  double x,y,z;

  x=-179.21471; y= -25.48460;  z= 28.59190;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x=12.97000;  cube_y= 6.39340;  cube_z= 7.46310;             break;
  x=-132.89429; y= -22.26056;  z= 42.49110;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 7.18900;  cube_y= 4.76700;  cube_z= 4.86460;             break;
  x=-102.15789; y= -23.64121;  z= 42.24343;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 6.18280;  cube_y= 4.44620;  cube_z= 3.01100;     break;
  x=-98.18877;  y= -30.71624;  z= 27.09789;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 5.98590;  cube_y= 4.40370;  cube_z= 3.62240;             break;
  x=-81.45436;  y= -22.11407;  z= 41.59693;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 8.65970;  cube_y= 2.83270;  cube_z= 3.51150;             break;
  x=-57.59297;  y= -28.70307;  z= 26.55219;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x=10.84180;  cube_y= 4.76220;  cube_z= 4.84040;             break;
  x=-12.12567;  y= -27.03229;  z= 18.72117;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 4.24410;  cube_y= 3.92240;  cube_z= 2.59930;       break;
  x=-12.23820;  y=  26.67023;  z= 18.98727;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 4.93660;  cube_y= 3.81930;  cube_z= 1.78740;             break;
  x=-57.12216;  y=  28.55730;  z= 26.44823;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 9.86200;  cube_y= 4.98380;  cube_z= 2.76390;             break;
  x=-81.82614;  y=  22.49641;  z= 41.58564;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 7.21800;  cube_y= 3.10390;  cube_z= 2.85120;             break;

  x= -172.91833; y=  -30.13112;  z=  30.19587;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.38700 ;  cube_y=   7.46940;  cube_z=    3.44540;            break;
  x= -162.98083; y=  -23.80763;  z=  50.85197;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=11.55100;  cube_y=    8.99240;  cube_z=    5.95980;            break;
  x= -95.42235;  y=  -26.12093;  z=  48.01413;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=12.22960 ;  cube_y=   8.72150;  cube_z=    2.70570;            break;
  x= -94.42485;  y=  -30.89255;  z=  31.54360;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.12040;  cube_y=    6.78130;  cube_z=    2.97720;            break;
  x= -73.21947;  y=  -24.48868;  z=  48.17738;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.70360;  cube_y=    7.92530;  cube_z=    3.49310;            break;
  x= -54.77978;  y=  -28.78197;  z=  32.33453;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x= 9.52330;  cube_y=    7.59310;  cube_z=    1.74170;            break;
  x= -9.01974;   y=  -27.10253;  z=  24.14972;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x= 8.19839;  cube_y=    7.95370;  cube_z=   11.26400;            break;
  x= -8.92432;   y=   26.29308;  z=  24.41268;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x= 7.36289;  cube_y=    7.28910;  cube_z=   11.13400;            break;
  x= -54.49910;  y=   28.81713;  z=  32.24610;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.78870 ;  cube_y=   8.53330;  cube_z=    1.81310;   break;
  x= -73.28730 ; y=   24.19770;  z=  48.27782;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.53240 ;  cube_y=   7.51110;  cube_z=    4.48150;            break;

  x= -182.70140; y=  -33.00098;  z=  36.03196;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 11.32500;  cube_y=  2.66220;  cube_z=  5.30500;               break;
  x= -172.25520; y=  -25.58108;  z=  57.31650;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 10.41800;  cube_y=  8.78890;  cube_z=  6.26010;               break;
  x= -93.23690;  y=  -28.57264;  z=  54.92082;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 18.29850;  cube_y=  8.09550;  cube_z=  4.93300;               break;
  x= -92.03020;  y=  -35.13788;  z=  35.35490;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 15.75600;  cube_y=  5.29950;  cube_z=  4.74010;               break;
  x= -71.11060;  y=  -25.93910;  z=  54.39806;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 17.44500;  cube_y=  8.24900;  cube_z=  4.54850;               break;
  x= -45.97132;  y=  -31.52034;  z=  35.75554;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 14.54310;  cube_y=  5.06220;  cube_z=  2.10600;               break;
  x= -9.98616;   y=  -25.91536;  z=  23.66608;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 6.10600;   cube_y=  9.14360;  cube_z= 15.62190;               break;
  x= -9.95002;   y=   25.62242;  z=  23.88524;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 6.10850;   cube_y=  9.14360;  cube_z= 15.62690;               break;
  x= -46.07720;  y=   31.31362;  z=  35.90390;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 14.54140;  cube_y=  5.06220;  cube_z=  3.15410;               break;
  x= -71.21410;  y=   25.79662;  z=  54.47188;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 17.44350;  cube_y=  8.24900;  cube_z=  4.91610;               break;

  C.push_back(C0);C.push_back(C1);C.push_back(C2);
 
  /*///##comented out 10-2-2006##/// vcl_cout<<"x01 distance :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    ///##comented out 10-2-2006##/// vcl_cout<<C[i][1][0]-C[i][0][0]<<" ";
  }*/

  double R_HoC=0.0;
  ///##comented out 10-2-2006##/// vcl_cout<<"HoC :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    ///##comented out 10-2-2006##/// vcl_cout<<C[i][1][2]/12.0<<" ";
  }

  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"LEN :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    ///##comented out 10-2-2006##/// vcl_cout<<vcl_fabs(C[i][0][0]-C[i][7][0])/12.0<<" ";
  }

  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;

  double CR_x01_07[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"x01/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_x01_07[i]=(C[i][1][0]-C[i][0][0])/(C[i][7][0]-C[i][0][0]);
    ///##comented out 10-2-2006##/// vcl_cout<<CR_x01_07[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;

  double CR_x19_07[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"x19/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_x19_07[i]=(C[i][9][0]-C[i][1][0])/(C[i][7][0]-C[i][0][0]);
    ///##comented out 10-2-2006##/// vcl_cout<<CR_x19_07[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;

  double CR_x79_07[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"x79/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_x79_07[i]=vcl_abs(C[i][9][0]-C[i][7][0])/(C[i][7][0]-C[i][0][0]);
    ///##comented out 10-2-2006##/// vcl_cout<<CR_x79_07[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;


  double CR_z37_39[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"z37/z39 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_z37_39[i]=vcl_abs(C[i][7][2])/(C[i][9][2]);
    ///##comented out 10-2-2006##/// vcl_cout<<CR_z37_39[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;

  double CR_x07_z39[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"z39/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_x07_z39[i]=vcl_abs((C[i][9][2])/((C[i][7][0]-C[i][0][0])));
    ///##comented out 10-2-2006##/// vcl_cout<<CR_x07_z39[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;


  ///##comented out 10-2-2006##/// vcl_cout<<"-------------\n"<<"input:"<<" "<<" \n--------------"<<vcl_endl;
  double x01=vcl_fabs(P(1,0)-P(0,0));
  double x07=vcl_fabs(P(7,0)-P(0,0));
  double x79=vcl_fabs(P(7,0)-P(9,0));
  double real_length=vcl_fabs(P(0,0)-P(7,0));
  ///##comented out 10-2-2006##/// vcl_cout<<"HoC:"<<HoC-0.9<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"length: "<<real_length<<vcl_endl;

  ///##comented out 10-2-2006##/// vcl_cout<<"x01/x07 ratio  : "<<x01<<"  ratio "<<x01/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x19/x07 ratio  : "<<vcl_fabs(P(1,0)-P(9,0))/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x79/x07 ratio  : "<<x79/x07<<vcl_endl ;
  ///##comented out 10-2-2006##/// vcl_cout<<"z3             : "<<P(3,2)<<vcl_endl;
  /////////bug//////##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(3,2)/HoC<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(7,2)/HoC<<vcl_endl;


  ///##comented out 10-2-2006##/// vcl_cout<<"z9/x07  ratio  : "<<(HoC-.9)/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;


  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  vcl_cout<<HoC<<vcl_endl;                         ///*******
  vcl_cout<<P<<vcl_endl;                           ///*******
  //vcl_cout<<P(7,0)<<vcl_endl;                    ///*******
 // vcl_cout<<P(9,0)<<vcl_endl;                    ///*******
  vcl_cout<<x01<<" "     ;                         ///*******
  vcl_cout<<x07<<" "     ;                         ///*******
  vcl_cout<<x79<<vcl_endl;                         ///*******
  vcl_cout<<x01/vcl_fabs(P(0,0)-P(7,0))<<" ";      ///*******
  vcl_cout<<P(7,2)/HoC<<vcl_endl;                  ///*******
  vcl_cout<<"======"<<vcl_endl;                  ///*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******


  int decision=999;
  if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 4.5) decision = 00;
  else if (x01/vcl_fabs(P(0,0)-P(7,0))>.16 && HoC-.9 < 3.5) decision = 01; // big offset 0 and 1
  else if (HoC-.9 < 3.0) decision = 02;                                    // small height
  else if ((HoC-.9)/vcl_fabs(P(0,0)-P(7,0)) < .2) decision = 03;           //height/length
  //if (real_length<12) decision = 02;

  else if (HoC-.9>5.9) decision = 23;  // if too high, van..
 // if (P(3,2)>2.0) decision = 10;  // rear botom high suv

 // if (P(3,2)<1.0) decision = 20;  //  rear botom low mini van
  else if (P(2,2)>2.5 && P(3,2)>2.5) decision = 11; 
  else if (P(2,2)>3.5) decision = 12;////danger????

  /////////else if (P(3,2)<2.0&&P(2,2)<2.5) decision = 21;


  //if (x01/vcl_fabs(P(0,0)-P(7,0))>.3 && HoC-.9 < 4.5) decision = 05;
 // if ((P(7,0)-.9)/(HoC-.9)>.5) decision = 19;
 // if ((P(7,0)-.9)/(HoC-.9)<.4) decision = 29;
 //// else if (P(7,2)/HoC>.5) decision = 19;
 //// else if (P(7,2)/HoC<.4) decision = 29;
  else if (P(3,2)>2.0) decision = 10;  // rear botom high suv

  else if (P(3,2)<1.0) decision = 20;  //  rear botom low mini van

  else decision=999;
 

  double error0,error1,error2,error3,error4,error5;
  //double error0
  return (decision);
}

int recognizer2(vnl_matrix <double> P,double HoC)
{
  vnl_vector <double> v(3,0);
  vcl_vector <vnl_vector <double> > C0,C1,C2;

  vcl_vector <vcl_vector <vnl_vector <double> > >C;
  double x,y,z;

  x=-179.21471; y= -25.48460;  z= 28.59190;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x=12.97000;  cube_y= 6.39340;  cube_z= 7.46310;             break;
  x=-132.89429; y= -22.26056;  z= 42.49110;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 7.18900;  cube_y= 4.76700;  cube_z= 4.86460;             break;
  x=-102.15789; y= -23.64121;  z= 42.24343;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 6.18280;  cube_y= 4.44620;  cube_z= 3.01100;     break;
  x=-98.18877;  y= -30.71624;  z= 27.09789;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 5.98590;  cube_y= 4.40370;  cube_z= 3.62240;             break;
  x=-81.45436;  y= -22.11407;  z= 41.59693;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 8.65970;  cube_y= 2.83270;  cube_z= 3.51150;             break;
  x=-57.59297;  y= -28.70307;  z= 26.55219;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x=10.84180;  cube_y= 4.76220;  cube_z= 4.84040;             break;
  x=-12.12567;  y= -27.03229;  z= 18.72117;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 4.24410;  cube_y= 3.92240;  cube_z= 2.59930;       break;
  x=-12.23820;  y=  26.67023;  z= 18.98727;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 4.93660;  cube_y= 3.81930;  cube_z= 1.78740;             break;
  x=-57.12216;  y=  28.55730;  z= 26.44823;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 9.86200;  cube_y= 4.98380;  cube_z= 2.76390;             break;
  x=-81.82614;  y=  22.49641;  z= 41.58564;  v(0)=x; v(1)=y; v(2)=z; C0.push_back(v);//cube_x= 7.21800;  cube_y= 3.10390;  cube_z= 2.85120;             break;

  x= -172.91833; y=  -30.13112;  z=  30.19587;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.38700 ;  cube_y=   7.46940;  cube_z=    3.44540;            break;
  x= -162.98083; y=  -23.80763;  z=  50.85197;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=11.55100;  cube_y=    8.99240;  cube_z=    5.95980;            break;
  x= -95.42235;  y=  -26.12093;  z=  48.01413;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=12.22960 ;  cube_y=   8.72150;  cube_z=    2.70570;            break;
  x= -94.42485;  y=  -30.89255;  z=  31.54360;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.12040;  cube_y=    6.78130;  cube_z=    2.97720;            break;
  x= -73.21947;  y=  -24.48868;  z=  48.17738;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.70360;  cube_y=    7.92530;  cube_z=    3.49310;            break;
  x= -54.77978;  y=  -28.78197;  z=  32.33453;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x= 9.52330;  cube_y=    7.59310;  cube_z=    1.74170;            break;
  x= -9.01974;   y=  -27.10253;  z=  24.14972;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x= 8.19839;  cube_y=    7.95370;  cube_z=   11.26400;            break;
  x= -8.92432;   y=   26.29308;  z=  24.41268;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x= 7.36289;  cube_y=    7.28910;  cube_z=   11.13400;            break;
  x= -54.49910;  y=   28.81713;  z=  32.24610;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.78870 ;  cube_y=   8.53330;  cube_z=    1.81310;   break;
  x= -73.28730 ; y=   24.19770;  z=  48.27782;  v(0)=x; v(1)=y; v(2)=z; C1.push_back(v);//cube_x=10.53240 ;  cube_y=   7.51110;  cube_z=    4.48150;            break;

  x= -182.70140; y=  -33.00098;  z=  36.03196;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 11.32500;  cube_y=  2.66220;  cube_z=  5.30500;               break;
  x= -172.25520; y=  -25.58108;  z=  57.31650;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 10.41800;  cube_y=  8.78890;  cube_z=  6.26010;               break;
  x= -93.23690;  y=  -28.57264;  z=  54.92082;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 18.29850;  cube_y=  8.09550;  cube_z=  4.93300;               break;
  x= -92.03020;  y=  -35.13788;  z=  35.35490;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 15.75600;  cube_y=  5.29950;  cube_z=  4.74010;               break;
  x= -71.11060;  y=  -25.93910;  z=  54.39806;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 17.44500;  cube_y=  8.24900;  cube_z=  4.54850;               break;
  x= -45.97132;  y=  -31.52034;  z=  35.75554;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 14.54310;  cube_y=  5.06220;  cube_z=  2.10600;               break;
  x= -9.98616;   y=  -25.91536;  z=  23.66608;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 6.10600;   cube_y=  9.14360;  cube_z= 15.62190;               break;
  x= -9.95002;   y=   25.62242;  z=  23.88524;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 6.10850;   cube_y=  9.14360;  cube_z= 15.62690;               break;
  x= -46.07720;  y=   31.31362;  z=  35.90390;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 14.54140;  cube_y=  5.06220;  cube_z=  3.15410;               break;
  x= -71.21410;  y=   25.79662;  z=  54.47188;  v(0)=x; v(1)=y; v(2)=z; C2.push_back(v);//cube_x= 17.44350;  cube_y=  8.24900;  cube_z=  4.91610;               break;

  C.push_back(C0);C.push_back(C1);C.push_back(C2);
 
  /*///##comented out 10-2-2006##/// vcl_cout<<"x01 distance :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    ///##comented out 10-2-2006##/// vcl_cout<<C[i][1][0]-C[i][0][0]<<" ";
  }*/

  double R_HoC=0.0;
  ///##comented out 10-2-2006##/// vcl_cout<<"HoC :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    ///##comented out 10-2-2006##/// vcl_cout<<C[i][1][2]/12.0<<" ";
  }

  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"LEN :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    ///##comented out 10-2-2006##/// vcl_cout<<vcl_fabs(C[i][0][0]-C[i][7][0])/12.0<<" ";
  }

  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;

  double CR_x01_07[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"x01/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_x01_07[i]=(C[i][1][0]-C[i][0][0])/(C[i][7][0]-C[i][0][0]);
    ///##comented out 10-2-2006##/// vcl_cout<<CR_x01_07[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;

  double CR_x19_07[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"x19/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_x19_07[i]=(C[i][9][0]-C[i][1][0])/(C[i][7][0]-C[i][0][0]);
    ///##comented out 10-2-2006##/// vcl_cout<<CR_x19_07[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;

  double CR_x79_07[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"x79/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_x79_07[i]=vcl_abs(C[i][9][0]-C[i][7][0])/(C[i][7][0]-C[i][0][0]);
    ///##comented out 10-2-2006##/// vcl_cout<<CR_x79_07[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;


  double CR_z37_39[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"z37/z39 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_z37_39[i]=vcl_abs(C[i][7][2])/(C[i][9][2]);
    ///##comented out 10-2-2006##/// vcl_cout<<CR_z37_39[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;

  double CR_x07_z39[3];
  ///##comented out 10-2-2006##/// vcl_cout<<"z39/x07 ratio  :" ;
  for (unsigned i=0;i<3;i++) 
  {

    ///##comented out 10-2-2006##/// vcl_cout<<i<<":" ;
    CR_x07_z39[i]=vcl_abs((C[i][9][2])/((C[i][7][0]-C[i][0][0])));
    ///##comented out 10-2-2006##/// vcl_cout<<CR_x07_z39[i]<<" ";
  }
  ///##comented out 10-2-2006##/// vcl_cout<<vcl_endl;


  ///##comented out 10-2-2006##/// vcl_cout<<"-------------\n"<<"input:"<<" "<<" \n--------------"<<vcl_endl;
  double x01=vcl_fabs(P(1,0)-P(0,0));
  double x07=vcl_fabs(P(7,0)-P(0,0));
  double x79=vcl_fabs(P(7,0)-P(9,0));
  double real_length=vcl_fabs(P(0,0)-P(7,0));
  ///##comented out 10-2-2006##/// vcl_cout<<"HoC:"<<HoC-0.9<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"length: "<<real_length<<vcl_endl;

  ///##comented out 10-2-2006##/// vcl_cout<<"x01/x07 ratio  : "<<x01<<"  ratio "<<x01/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x19/x07 ratio  : "<<vcl_fabs(P(1,0)-P(9,0))/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x79/x07 ratio  : "<<x79/x07<<vcl_endl ;
  ///##comented out 10-2-2006##/// vcl_cout<<"z3             : "<<P(3,2)<<vcl_endl;
  /////////bug//////##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(3,2)/HoC<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(7,2)/HoC<<vcl_endl;


  ///##comented out 10-2-2006##/// vcl_cout<<"z9/x07  ratio  : "<<(HoC-.9)/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;


  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  vcl_cout<<"recognizer"<<vcl_endl;
  vcl_cout<<HoC<<vcl_endl;                         ///*******
  vcl_cout<<P<<vcl_endl;                           ///*******
  //vcl_cout<<P(7,0)<<vcl_endl;                    ///*******
 // vcl_cout<<P(9,0)<<vcl_endl;                    ///*******
  vcl_cout<<x01<<" "     ;                         ///*******
  vcl_cout<<x07<<" "     ;                         ///*******
  vcl_cout<<x79<<vcl_endl;                         ///*******
  vcl_cout<<x01/vcl_fabs(P(0,0)-P(7,0))<<" ";      ///*******
  vcl_cout<<P(7,2)/HoC<<vcl_endl;                  ///*******
  vcl_cout<<"======"<<vcl_endl;                  ///*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******


  int decision=999;
  if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 4.5) decision = 00;
 // else if (HoC<4.4) decision=8;
  else if (x01/vcl_fabs(P(0,0)-P(7,0))>.16 && HoC-.9 < 3.5) decision = 01; // big offset 0 and 1
  else if (HoC < 4.0) decision = 02;                                    // small height
  else if ((HoC-.9)/vcl_fabs(P(0,0)-P(7,0)) < .2) decision = 03;           //height/length

  //else if ((HoC-.9) < 4) decision = 04;
  //if (real_length<12) decision = 02;

  else if (HoC>7) decision = 28; 
  else if (HoC-.9>5.9) decision = 23;  // if too high, van..

  //else if (P(2,2)>2.5 && P(3,2)>2.5) decision = 11; 
 /// else if (P(2,2)>3.5) decision = 12;////danger????

  /////////else if (P(3,2)<2.0&&P(2,2)<2.5) decision = 21;


  //if (x01/vcl_fabs(P(0,0)-P(7,0))>.3 && HoC-.9 < 4.5) decision = 05;
   else if (P(7,2)>3.5&&HoC<4.5) decision = 17;
 // else if ((P(7,2))/(HoC)>.6) decision = 18;
  else if ((P(7,2))/(HoC-.9)>.95) decision = 19;
 // if ((P(7,0)-.9)/(HoC-.9)<.4) decision = 29;
 //// else if (P(7,2)/HoC>.5) decision = 19;
 


// else if (P(3,2)<1.0) decision = 20;  //  rear botom low mini van
 else if (HoC>6.5) decision=25; //this is good
 else if (x07/vcl_fabs(P(0,0)-P(7,0))<.2 && HoC> 5.5) decision = 27;//bad 11-1-2006

   else if ((P(7,2))/(HoC-.9)>.75&&HoC<6.0) decision = 16;
else if (P(7,2)/HoC<.4 &&HoC>5.5) decision = 29; 
else if (P(3,2)>2.2) decision = 10;  // rear botom high suv
  else decision=999;
 

  double error0,error1,error2,error3,error4,error5;
  //double error0
  return (decision);
}



float recognizer3(vnl_matrix <double> P,double HoC)
{
  vnl_vector <double> v(3,0);
  vcl_vector <vnl_vector <double> > C0,C1,C2;

  vcl_vector <vcl_vector <vnl_vector <double> > >C;
  
  

  double R_HoC=0.0;
  
  
  double x01=vcl_fabs(P(1,0)-P(0,0));
  double x07=vcl_fabs(P(7,0)-P(0,0));
  double x79=vcl_fabs(P(7,0)-P(9,0));
  double real_length=vcl_fabs(P(0,0)-P(7,0));
  ///##comented out 10-2-2006##/// vcl_cout<<"HoC:"<<HoC-0.9<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"length: "<<real_length<<vcl_endl;

  ///##comented out 10-2-2006##/// vcl_cout<<"x01/x07 ratio  : "<<x01<<"  ratio "<<x01/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x19/x07 ratio  : "<<vcl_fabs(P(1,0)-P(9,0))/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x79/x07 ratio  : "<<x79/x07<<vcl_endl ;
  ///##comented out 10-2-2006##/// vcl_cout<<"z3             : "<<P(3,2)<<vcl_endl;
  /////////bug//////##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(3,2)/HoC<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(7,2)/HoC<<vcl_endl;


  ///##comented out 10-2-2006##/// vcl_cout<<"z9/x07  ratio  : "<<(HoC-.9)/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;


  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  vcl_cout<<"recognizer"<<vcl_endl;
  vcl_cout<<HoC<<vcl_endl;                         ///*******
  vcl_cout<<P<<vcl_endl;                           ///*******
  //vcl_cout<<P(7,0)<<vcl_endl;                    ///*******
 // vcl_cout<<P(9,0)<<vcl_endl;                    ///*******
  vcl_cout<<x01<<" "     ;                         ///*******
  vcl_cout<<x07<<" "     ;                         ///*******
  vcl_cout<<x79<<vcl_endl;                         ///*******
  vcl_cout<<x01/vcl_fabs(P(0,0)-P(7,0))<<" ";      ///*******
  vcl_cout<<P(7,2)/HoC<<vcl_endl;                  ///*******
  vcl_cout<<"======"<<vcl_endl;                  ///*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******


  float decision=999;
  if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 4.0) decision = 00;
  // else if (HoC<4.4) decision=8;
  else if (x01/vcl_fabs(P(0,0)-P(7,0))>.16 && HoC-.9 < 3.5) decision = 01; // big offset 0 and 1
  else if (HoC < 4.0) decision = 02;                                    // small height
  else if ((HoC-.9)/vcl_fabs(P(0,0)-P(7,0)) < .2) decision = 03;           //height/length

  //else if ((HoC-.9) < 4) decision = 04;
  //if (real_length<12) decision = 02;

  else if (HoC>7) decision = 28; 
  else if (HoC-.9>5.9) decision = 23;  // if too high, van..

  //else if (P(2,2)>2.5 && P(3,2)>2.5) decision = 11; 
  /// else if (P(2,2)>3.5) decision = 12;////danger????

  /////////else if (P(3,2)<2.0&&P(2,2)<2.5) decision = 21;


  //if (x01/vcl_fabs(P(0,0)-P(7,0))>.3 && HoC-.9 < 4.5) decision = 05;
  else if (P(7,2)>3.5&&HoC<4.5) decision = 17;
  // else if ((P(7,2))/(HoC)>.6) decision = 18;
  else if ((P(7,2))/(HoC-.9)>.95) decision = 19;
  // if ((P(7,0)-.9)/(HoC-.9)<.4) decision = 29;
  //// else if (P(7,2)/HoC>.5) decision = 19;



  // else if (P(3,2)<1.0) decision = 20;  //  rear botom low mini van
  else if (HoC>6.5) decision=25; //this is good
  //else if (x07/vcl_fabs(P(0,0)-P(7,0))<.2 && HoC> 5.5) decision = 27;//bad 11-1-2006

  else if ((P(7,2))/(HoC-.9)>.75&&HoC<6.0&&HoC>5) decision = 16;
  else if (P(7,2)/HoC<.4 &&HoC>6) decision = 29; 
  //else if (P(3,2)>2.2) decision = 10;  // rear botom high suv

  else if ((P(7,2))/(HoC-.9)>.65 && vcl_fabs(P(0,0)-P(7,0))>12 ) decision = 16.1;
else if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 4.5) decision = 0.1;//위치와 순서와 값 매우 좋음...
else if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 5.5) decision = 0.2;//나쁨?


  else decision=10.1;


  double error0,error1,error2,error3,error4,error5;
  //double error0
  return (decision);
}



float recognizer4(vnl_matrix <double> P,double HoC,vnl_vector <float> V)
{
  vnl_vector <double> v(3,0);
  vcl_vector <vnl_vector <double> > C0,C1,C2;

  vcl_vector <vcl_vector <vnl_vector <double> > >C;
  
  

  double R_HoC=0.0;
  
  
  double x01=vcl_fabs(P(1,0)-P(0,0));
  double x07=vcl_fabs(P(7,0)-P(0,0));
  double x79=vcl_fabs(P(7,0)-P(9,0));
  double real_length=vcl_fabs(P(0,0)-P(7,0));
  ///##comented out 10-2-2006##/// vcl_cout<<"HoC:"<<HoC-0.9<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"length: "<<real_length<<vcl_endl;

  ///##comented out 10-2-2006##/// vcl_cout<<"x01/x07 ratio  : "<<x01<<"  ratio "<<x01/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x19/x07 ratio  : "<<vcl_fabs(P(1,0)-P(9,0))/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x79/x07 ratio  : "<<x79/x07<<vcl_endl ;
  ///##comented out 10-2-2006##/// vcl_cout<<"z3             : "<<P(3,2)<<vcl_endl;
  /////////bug//////##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(3,2)/HoC<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(7,2)/HoC<<vcl_endl;


  ///##comented out 10-2-2006##/// vcl_cout<<"z9/x07  ratio  : "<<(HoC-.9)/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;


  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  vcl_cout<<"recognizer"<<vcl_endl;
  vcl_cout<<HoC<<vcl_endl;                         ///*******
  vcl_cout<<P<<vcl_endl;                           ///*******
  //vcl_cout<<P(7,0)<<vcl_endl;                    ///*******
 // vcl_cout<<P(9,0)<<vcl_endl;                    ///*******
  vcl_cout<<x01<<" "     ;                         ///*******
  vcl_cout<<x07<<" "     ;                         ///*******
  vcl_cout<<x79<<vcl_endl;                         ///*******
  vcl_cout<<x01/vcl_fabs(P(0,0)-P(7,0))<<" ";      ///*******
  vcl_cout<<P(7,2)/HoC<<vcl_endl;                  ///*******
  vcl_cout<<"======"<<vcl_endl;                  ///*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******


  float decision=999;

  //**//if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 4.0) decision = 00;
  if (x01/vcl_fabs(P(0,0)-P(7,0))>V[0] && HoC-.9 < V[1]) decision = 00;

  //**//else if (x01/vcl_fabs(P(0,0)-P(7,0))>.16 && HoC-.9 < 3.5) decision = 01; // big offset 0 and 1
  else if (x01/vcl_fabs(P(0,0)-P(7,0))>V[2] && HoC-.9 < V[3]) decision = 01; // big offset 0 and 1
  
  else if (HoC < V[4]/*4.0*/) decision = 02;                                    // small height
  
  else if ((HoC-.9)/vcl_fabs(P(0,0)-P(7,0)) < V[5]/*.2*/) decision = 03;           //height/length


  else if (HoC>V[6]/*7*/) decision = 28; 

  else if (HoC-.9>V[7]/*5.9*/) decision = 23;  // if too high, van..


  /////////else if (P(3,2)<2.0&&P(2,2)<2.5) decision = 21;



  else if (P(7,2)>V[8]/*3.5*/&&HoC<V[9]/*4.5*/) decision = 17;
  // else if ((P(7,2))/(HoC)>.6) decision = 18;
  else if ((P(7,2))/(HoC-.9)>V[10]/*.95*/) decision = 19;
  // if ((P(7,0)-.9)/(HoC-.9)<.4) decision = 29;
  //// else if (P(7,2)/HoC>.5) decision = 19;



  // else if (P(3,2)<1.0) decision = 20;  //  rear botom low mini van
  else if (HoC>6.5) decision=25; //this is good
  //else if (x07/vcl_fabs(P(0,0)-P(7,0))<.2 && HoC> 5.5) decision = 27;//bad 11-1-2006

  else if ((P(7,2))/(HoC-.9)>.75&&HoC<6.0&&HoC>5) decision = 16;
  else if (P(7,2)/HoC<.4 &&HoC>6) decision = 29; 
  //else if (P(3,2)>2.2) decision = 10;  // rear botom high suv

  else if ((P(7,2))/(HoC-.9)>.65 && vcl_fabs(P(0,0)-P(7,0))>12 ) decision = 16.1;
  else if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 4.5) decision = 0.1;//위치와 순서와 값 매우 좋음...
  //else if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 5.5) decision = 0.2;//나쁨?


  else decision=10.1;


  double error0,error1,error2,error3,error4,error5;
  //double error0
  return (decision);
}

//random generation


float recognizer_RG(vnl_matrix <double> P,double HoC,vnl_vector <float> V)
{
  vnl_vector <double> v(3,0);
  vcl_vector <vnl_vector <double> > C0,C1,C2;

  vcl_vector <vcl_vector <vnl_vector <double> > >C;
  
  

  double R_HoC=0.0;
  
  
  double x01=vcl_fabs(P(1,0)-P(0,0));
  double x07=vcl_fabs(P(7,0)-P(0,0));
  double x79=vcl_fabs(P(7,0)-P(9,0));
  double real_length=vcl_fabs(P(0,0)-P(7,0));
  ///##comented out 10-2-2006##/// vcl_cout<<"HoC:"<<HoC-0.9<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"length: "<<real_length<<vcl_endl;

  ///##comented out 10-2-2006##/// vcl_cout<<"x01/x07 ratio  : "<<x01<<"  ratio "<<x01/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x19/x07 ratio  : "<<vcl_fabs(P(1,0)-P(9,0))/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"x79/x07 ratio  : "<<x79/x07<<vcl_endl ;
  ///##comented out 10-2-2006##/// vcl_cout<<"z3             : "<<P(3,2)<<vcl_endl;
  /////////bug//////##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(3,2)/HoC<<vcl_endl;
  ///##comented out 10-2-2006##/// vcl_cout<<"z7/z9   ratio  : "<<P(7,2)/HoC<<vcl_endl;


  ///##comented out 10-2-2006##/// vcl_cout<<"z9/x07  ratio  : "<<(HoC-.9)/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;


  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  vcl_cout<<"recognizer"<<vcl_endl;
  vcl_cout<<HoC<<vcl_endl;                         ///*******
  vcl_cout<<P<<vcl_endl;                           ///*******
  //vcl_cout<<P(7,0)<<vcl_endl;                    ///*******
 // vcl_cout<<P(9,0)<<vcl_endl;                    ///*******
  vcl_cout<<x01<<" "     ;                         ///*******
  vcl_cout<<x07<<" "     ;                         ///*******
  vcl_cout<<x79<<vcl_endl;                         ///*******
  vcl_cout<<x01/vcl_fabs(P(0,0)-P(7,0))<<" ";      ///*******
  vcl_cout<<P(7,2)/HoC<<vcl_endl;                  ///*******
  vcl_cout<<"======"<<vcl_endl;                  ///*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******


  float decision=999;
  if (x01/vcl_fabs(P(0,0)-P(7,0))>.20+V[0] && HoC-.9 < 4.0+V[1]) decision = 00;
  // else if (HoC<4.4) decision=8;
  else if (x01/vcl_fabs(P(0,0)-P(7,0))>.16+V[2] && HoC-.9 < 3.5+V[3]) decision = 01; // big offset 0 and 1
  else if (HoC < 4.0+V[4]) decision = 02;                                    // small height
  else if ((HoC-.9)/vcl_fabs(P(0,0)-P(7,0)) < .2+V[5]) decision = 03;           //height/length

  //else if ((HoC-.9) < 4) decision = 04;
  //if (real_length<12) decision = 02;

  else if (HoC>7+V[6]) decision = 28; 
  else if (HoC-.9>5.9+V[7]) decision = 23;  // if too high, van..

  //else if (P(2,2)>2.5 && P(3,2)>2.5) decision = 11; 
  /// else if (P(2,2)>3.5) decision = 12;////danger????

  /////////else if (P(3,2)<2.0&&P(2,2)<2.5) decision = 21;


  //if (x01/vcl_fabs(P(0,0)-P(7,0))>.3 && HoC-.9 < 4.5) decision = 05;
  else if (P(7,2)>3.5&&HoC<4.5+V[8]) decision = 17;
  // else if ((P(7,2))/(HoC)>.6) decision = 18;
  else if ((P(7,2))/(HoC-.9)>.95+V[9]) decision = 19;
  // if ((P(7,0)-.9)/(HoC-.9)<.4) decision = 29;
  //// else if (P(7,2)/HoC>.5) decision = 19;



  // else if (P(3,2)<1.0) decision = 20;  //  rear botom low mini van
  else if (HoC>6.5+   V[20]   ) decision=25; //this is good
  //else if (x07/vcl_fabs(P(0,0)-P(7,0))<.2 && HoC> 5.5) decision = 27;//bad 11-1-2006

  else if ((P(7,2))/(HoC-.9)>.75+V[10]&&HoC<6.0&&HoC>5+V[11]) decision = 16;
  else if (P(7,2)/HoC<.4 +V[12]&&HoC>6+V[13]) decision = 29; 
  //else if (P(3,2)>2.2) decision = 10;  // rear botom high suv

  else if ((P(7,2))/(HoC-.9)>.65+V[14] && vcl_fabs(P(0,0)-P(7,0)+V[15])>12 ) decision = 16.1;
else if (x01/vcl_fabs(P(0,0)-P(7,0))>.20+V[16] && HoC-.9 < 4.5+V[17]) decision = 0.1;//위치와 순서와 값 매우 좋음...
else if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 +V[18]&& HoC-.9 < 5.5+V[19]) decision = 0.2;//나쁨?


  else decision=10.1;


  double error0,error1,error2,error3,error4,error5;
  //double error0
  return (decision);
}

//////////////////

//////////////////

// 5-23-2007

float recognizer052307(vnl_matrix <double> P,double HoC,vnl_vector <float> V)
{

  bool show=true;

  vnl_vector <double> v(3,0);
  vcl_vector <vnl_vector <double> > C0,C1,C2;
  vcl_vector <vcl_vector <vnl_vector <double> > >C;
 

  double R_HoC=0.0;
  
  double x01=vcl_fabs(P(1,0)-P(0,0));
  double x07=vcl_fabs(P(7,0)-P(0,0));
  double x79=vcl_fabs(P(7,0)-P(9,0));
  double real_length=vcl_fabs(P(0,0)-P(7,0));

  if (show)  vcl_cout<<"HoC:"<<HoC-0.9<<vcl_endl;
  if (show)  vcl_cout<<"length: "<<real_length<<vcl_endl;

  if (show)  vcl_cout<<"x01/x07 ratio  : "<<x01<<"  ratio "<<x01/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  if (show)  vcl_cout<<"x19/x07 ratio  : "<<vcl_fabs(P(1,0)-P(9,0))/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  if (show)  vcl_cout<<"x79/x07 ratio  : "<<x79/x07<<vcl_endl ;
  if (show)  vcl_cout<<"z3             : "<<P(3,2)<<vcl_endl;
  /////////bug///if (show)  vcl_cout<<"z7/z9   ratio  : "<<P(3,2)/HoC<<vcl_endl;
  if (show)  vcl_cout<<"z7/z9   ratio  : "<<P(7,2)/HoC<<vcl_endl;


  if (show)  vcl_cout<<"z9/x07  ratio  : "<<(HoC-.9)/vcl_fabs(P(0,0)-P(7,0))<<vcl_endl;
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  vcl_cout<<"recognizer"<<vcl_endl;
  vcl_cout<<HoC<<vcl_endl;                         ///*******
  vcl_cout<<P<<vcl_endl;                           ///*******
  //vcl_cout<<P(7,0)<<vcl_endl;                    ///*******
  // vcl_cout<<P(9,0)<<vcl_endl;                    ///*******
  vcl_cout<<x01<<" "     ;                         ///*******
  vcl_cout<<x07<<" "     ;                         ///*******
  vcl_cout<<x79<<vcl_endl;                         ///*******
  vcl_cout<<x01/vcl_fabs(P(0,0)-P(7,0))<<" ";      ///*******
  vcl_cout<<P(7,2)/HoC<<vcl_endl;                  ///*******
  vcl_cout<<"======"<<vcl_endl;                  ///*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******
  ////////////////////////////////////////////////////*******

  float decision=999;
  //**//if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 4.0) decision = 00;
  if (x01/vcl_fabs(P(0,0)-P(7,0))>V[0] && HoC-.9 < V[1]) decision = 00;

  //**//else if (x01/vcl_fabs(P(0,0)-P(7,0))>.16 && HoC-.9 < 3.5) decision = 01; // big offset 0 and 1
  else if (x01/vcl_fabs(P(0,0)-P(7,0))>V[2] && HoC-.9 < V[3]) decision = 01; // big offset 0 and 1
  
  else if (HoC < V[4]/*4.0*/) decision = 02;                                    // small height
  
  else if ((HoC-.9)/vcl_fabs(P(0,0)-P(7,0)) < V[5]/*.2*/) decision = 03;           //height/length


  else if (HoC>V[6]/*7*/) decision = 28; 

  else if (HoC-.9>V[7]/*5.9*/) decision = 23;  // if too high, van..


  /////////else if (P(3,2)<2.0&&P(2,2)<2.5) decision = 21;



  else if (P(7,2)>V[8]/*3.5*/&&HoC<V[9]/*4.5*/) decision = 17;
  // else if ((P(7,2))/(HoC)>.6) decision = 18;
  else if ((P(7,2))/(HoC-.9)>V[10]/*.95*/) decision = 19;
  // if ((P(7,0)-.9)/(HoC-.9)<.4) decision = 29;
  //// else if (P(7,2)/HoC>.5) decision = 19;



  // else if (P(3,2)<1.0) decision = 20;  //  rear botom low mini van
  else if (HoC>6.5) decision=25; //this is good
  //else if (x07/vcl_fabs(P(0,0)-P(7,0))<.2 && HoC> 5.5) decision = 27;//bad 11-1-2006

  else if ((P(7,2))/(HoC-.9)>.75&&HoC<6.0&&HoC>5) decision = 16;
  else if (P(7,2)/HoC<.4 &&HoC>6) decision = 29; 
  //else if (P(3,2)>2.2) decision = 10;  // rear botom high suv

  else if ((P(7,2))/(HoC-.9)>.65 && vcl_fabs(P(0,0)-P(7,0))>12 ) decision = 16.1;
  else if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 4.5) decision = 0.1;//위치와 순서와 값 매우 좋음...
  //else if (x01/vcl_fabs(P(0,0)-P(7,0))>.20 && HoC-.9 < 5.5) decision = 0.2;//나쁨?


  else decision=10.1;


  double error0,error1,error2,error3,error4,error5;
  //double error0
  return (decision);
}

