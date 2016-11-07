// This is contrib/biotree/biolung/biolung_3D_tree.h

#ifndef _biolung_3D_Tree_h
#define _biolung_3D_Tree_h

//:
// \file
// \brief
//
// \author
// This code was received from Hiroko Kitaoka, Department of Engineering, 
// Tokyo University of Agriculture and Technology, Tokyo 184-8588, Japan.
// It implements the geometric model detailed in his paper 
// "A three-dimensional model of the human airway tree", J Appl Physiol 87: 2207-2217, 1999.
//:
// Can added the comments.
//
// \date
// 02/01/05
//
// \verbatim
//  Modifications
// \endverbatim

#define ERROR 0
#define SUCCESS 1

#define MaxStack  40

#ifndef PI
#define PI 3.14159
#endif

#ifndef NULL
#define NULL 0
#endif
  
#define MAX 60000

#define LM 35
#define PA 20

struct biolung_POINT3D
{
  double x;
  double y;
  double z;
};

struct biolung_Vector                      //vector
{
  double l;
  double m;
  double n;
};

struct biolung_Bra                           //structure for branches
{
  biolung_POINT3D p1;
  biolung_POINT3D p2;
  
  double diam;

  int check;
  int lab;

};

struct biolung_Area                   // Equations showing borders of the area
{
  biolung_Vector a[LM];
  double a4[LM];
  biolung_Vector b[LM];
  double b4[LM];
  int c[LM];
  
};

struct biolung_Range                    // Approximate location of the area
{
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double zmin;
  double zmax;
};

struct biolung_Parent          
{
  biolung_POINT3D p[PA];
  biolung_Vector vec[PA];
  double len[PA];
  double diam[PA];
};


class biolung_Airway  // airway branch        
{
  private:
  int Lab,Check,Gen;
  double Ratio, Flow,Diam;
  biolung_POINT3D P1,P2;
  biolung_Vector Vec, Norm;
  biolung_Area Ar;          
  biolung_Range Ra;
  biolung_Parent Par;

  public:
    void FillAw(int lab,int check,int gen,double ratio,double flow, double diam,biolung_POINT3D p1,biolung_POINT3D p2,
      biolung_Vector vec,biolung_Vector norm, biolung_Area ar, biolung_Range ra, biolung_Parent par)
    {
      Lab=lab; Check=check; Gen=gen;  Ratio=ratio; Flow=flow; Diam=diam; 
      P1=p1; P2=p2; Vec=vec; Norm=norm;
      Ar=ar;   Ra=ra;   Par=par;
      return;
    }
    void FillAw(biolung_Airway Q) 
    {
      Lab=Q.Lab; Check=Q.Check; Gen=Q.Gen;Flow=Q.Flow; Ratio=Q.Ratio;  Diam=Q.Diam;  
      P1=Q.P1; P2=Q.P2; Vec=Q.Vec; Norm=Q.Norm;  Ar=Q.Ar; Ra=Q.Ra; Par=Q.Par;
      return;
    }
    void SetLab(int lab) {Lab=lab;}
    int GetLab(void) {return Lab;}
    void SetCoheck(int check) {Check=check;}
    int GetCheck(void) {return Check;}
    void SetGen(int gen) {Gen=gen;}
    int GetGen(void) {return Gen;}
    void SetRatio(double ratio) {Ratio=ratio;}
    double GetRatio(void){return Ratio;}
    void SetFlow(double flow) {Flow=flow;}
    double GetFlow(void) {return Flow;}
    void SetDiam(double diam) {Diam=diam;}
    double GetDiam(void) {return Diam;}    
    void SetP1(biolung_POINT3D p1) {P1=p1;}
    biolung_POINT3D GetP1(void) {return P1;}
    void SetP2(biolung_POINT3D p2) {P2=p2;}
    biolung_POINT3D GetP2(void) {return P2;}
    void SetVec(biolung_Vector vec) {Vec=vec;}
    biolung_Vector GetVec(void) {return Vec;}
    void SetNorm(biolung_Vector norm) {Norm=norm;}
    biolung_Vector GetNorm(void) {return Norm;}
    void SetAr(biolung_Area ar){Ar=ar;}
    biolung_Area GetAr(void){return Ar;}
    void SetRa(biolung_Range ra){Ra=ra;}
    biolung_Range GetRa(void){return Ra;}    
    void SetPar(biolung_Parent par) {Par=par;}
    biolung_Parent GetPar(void) {return Par;}    
};

class biolung_AwStack:biolung_Airway     //stack structure to store airways
{
  private:
    int StackP;    //points to teh top of teh stack
    int stackSize;   // stack size
    biolung_Airway *awst;     //pointer to stack

  public:
    int message;
    biolung_AwStack(int maxStack);
    ~biolung_AwStack();
    void Push(int lab,int check,int gen,double ratio,double flow, double diam, biolung_POINT3D p1, biolung_POINT3D p2, 
      biolung_Vector vec, biolung_Vector norm, biolung_Area ar,biolung_Range ra, biolung_Parent par);
    void Push(biolung_Airway);
    biolung_Airway Pop(void);
    int GetNumber(void) { return StackP;}
};

class biolung_Tree
{
  public:
    double thresh;
    double min,excl, dexp,m_ldr,r_len,l_len;
    int acura;
    double overflow,overflow1;
    int ovnum,ovnum1;
       
    biolung_Airway *aw;
    biolung_Bra bra[MAX];                //array to  store branches
  
    biolung_Tree();
    ~biolung_Tree();  
  
    int shape(biolung_POINT3D p);    // Shape of the whole organ
    double com_ang(double r,double n);

    double distance(biolung_Vector vec, biolung_POINT3D p);
    double com_len(biolung_POINT3D p1,biolung_POINT3D p2);      //length of p1 to p2
    biolung_Vector com_vec(biolung_POINT3D p1,biolung_POINT3D p2);
    biolung_POINT3D  endpoint(biolung_POINT3D p1,biolung_Vector vec, double len);
    biolung_Vector product1(biolung_Vector vec1, biolung_Vector vec2);
    biolung_Vector product2(biolung_Vector vec1, biolung_Vector vec2);  
    biolung_Vector rotate1(biolung_Vector vec1, biolung_Vector vec2, double ang);
    biolung_Vector rotate2(biolung_Vector vec1, biolung_Vector vec2,  double ang);

    biolung_Range comrara(biolung_Area ar,int g,biolung_Range ra,biolung_Parent par);  
    int checkarea(biolung_Area ar,biolung_POINT3D pp,int g, biolung_Parent par);
    biolung_POINT3D GetCenter(biolung_Area ar, int g,biolung_Parent par, biolung_Range ra);
    double DisCenter1(biolung_POINT3D ce, biolung_POINT3D p1,biolung_Vector vec);
    double DisCenter2(biolung_POINT3D ce, biolung_POINT3D p1,biolung_Vector vec);
    double volratio(biolung_Area ar,int g,biolung_Range ra,biolung_Parent par);
    double volratio2(biolung_Area ar, int g,biolung_Range ra,biolung_Parent par,biolung_Vector vec, biolung_POINT3D p);
    double evenplane1(biolung_Area ar, int g,biolung_Range ra,biolung_Parent par,biolung_Vector vec, biolung_POINT3D p,biolung_Vector norm);
    double evenplane2(biolung_Area ar, int g,biolung_Range ra,biolung_Parent par,biolung_Vector vec, biolung_POINT3D p,biolung_Vector norm);

    double seeklength(double diam, double llen, double rlen, biolung_POINT3D p1, biolung_Vector vec, biolung_Area ar, int g, biolung_Parent par);
    double seeklength2(double diam, biolung_POINT3D p1, biolung_Vector vec, biolung_Area ar, int g, biolung_Parent par);
    double seeklengthC(double diam, double llen, double rlen, biolung_POINT3D p1, biolung_Vector vec, biolung_Area ar, int g, biolung_Parent par,biolung_Range ra);
    double seeklength3(double high, double diam, biolung_POINT3D p1, biolung_Vector vec, biolung_Area ar, int g, biolung_Parent par);
    
    double revers_com_ang(double ang,double n);
    double correctdiam(double diam,double r,double ab);
    long computeTree();  
};

#endif



