/*
ImageObj.h
*/

#pragma once

#include "dbseg_levelset2_Mh.h"
#include "dbseg_levelset2_BVector.h"
#include "dbseg_levelset2_BMatrix.h"

using namespace std;


class ImageObj
{

    
public:
    ImageObj();  // must call init before use
    ImageObj(int w, int h);
    ImageObj(const ImageObj& rhs); // copy constructor
    ImageObj(BMatrix bm);
    ~ImageObj();
    
    // Functions:
public:
    
    void init(int cw, int ch);
    void kill();
    void setPixelVal(int x, int y, double val);
    double getPixelVal(int x, int y) const;
    
    
    void storeMagicNum(string n);
    string getMagicNum() const;
    
    void storeComments(string c);
    string getComments() const;
    
    void setMaxVal(double m);
    double getMaxVal() const;
    
    void setMinVal(double m);
    double getMinVal() const;

    void findMM();
    
    int getImageHeight() const;
    int getImageWidth() const;
    
    void display();
    void splot(char *str);

    void operator+(ImageObj &img);    
    ImageObj* operator-(ImageObj *img);
    ImageObj& operator*(ImageObj &img);
    ImageObj& operator/(ImageObj &img);
    ImageObj& operator=(const ImageObj& img);
    void operator-();
    bool operator==(ImageObj &img);
    
    ImageObj* copy();
    void erase(double color);
    void calcmean();
    
    void transpose();
    
public:
    
    BMatrix *mat;
    bool mirror;
    double mean;
    string comments, mn;
    int w, h, cnt;
    double maxVal, minVal;
    
    double imgScale;
    
    BVector* pn;
    BVector* nn;
    
};


