/*
 *  RGBImg.h
 *  ImageApp
 *
 *  Created by Brendan Moore on 2/3/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "dbseg_levelset2_Mh.h"
#include "dbseg_levelset2_ImageObj.h"

using namespace std;


class RGBImg
{
    
    
public:
    RGBImg();
    RGBImg(int w, int h);
    ~RGBImg();
    
    void init(int w, int h);
    
    void storeMagicNum(string n);
    string getMagicNum() const;
    
    void storeComments(string c);
    string getComments() const;
    
    void setMaxVal(double m);
    double getMaxVal() const;
        
    int getHeight() const;
    int getWidth() const;
    
    void setPixelVal(int x, int y, double r, double g, double b);
    
    void findMM();
    
    void display();
    

public:
    ImageObj *r;
    ImageObj *g;
    ImageObj *b;
    
    int width, height;
    string mn, comments;
    
    double maxVal;
    
        
};


