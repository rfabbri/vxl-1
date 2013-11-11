/*
 *  RGBImg.cpp
 *  ImageApp
 *
 *  Created by Brendan Moore on 2/3/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "dbseg_levelset2_RGBImg.h"

RGBImg::RGBImg() {}

RGBImg::RGBImg(int w, int h) {

    r =  new ImageObj(w, h);
    g = new ImageObj(w, h);
    b = new ImageObj(w, h);

    width = w;
    height = h;
    
    storeMagicNum("P3");
    storeComments("# Created by ImageApp");
    setMaxVal(255);
    
}


RGBImg::~RGBImg() {
    
    delete r;
    delete g;
    delete b;
    

}


void RGBImg::init(int w, int h) {
    
    r = new ImageObj(w, h);
    g = new ImageObj(w, h);
    b = new ImageObj(w, h);
    
    width = w;
    height = h;
    
    storeMagicNum("P3");
    storeComments("# Created by ImageApp");
    setMaxVal(255);
    
    
    
}



void RGBImg::storeMagicNum(string n) {
    
    mn = n;
    r->storeMagicNum(n);
    g->storeMagicNum(n);
    b->storeMagicNum(n);
}

string RGBImg::getMagicNum() const {
    
    return mn;
}

void RGBImg::storeComments(string c) {
    
    comments = c;
    r->storeComments(c);
    g->storeComments(c);
    b->storeComments(c);

}

string RGBImg::getComments() const {
    
    return comments;
}

void RGBImg::setMaxVal(double m) {
    
    maxVal = m;
    r->setMaxVal(m);
    g->setMaxVal(m);
    b->setMaxVal(m);
    
}

double RGBImg::getMaxVal() const {
    
    return maxVal;
}

int RGBImg::getHeight() const {
    
    return height;
}

int RGBImg::getWidth() const {
    
    return width;
}

void RGBImg::setPixelVal(int x, int y, double rv, double gv, double bv) {
    
    r->setPixelVal(x,y,rv);    
    g->setPixelVal(x,y,gv);
    b->setPixelVal(x,y,bv);
    
    
}

void RGBImg::findMM() {
    
    r->findMM();
    g->findMM();
    b->findMM();
    
}


void RGBImg::display() {
    
    printf("******** RED CHANNEL ************\n");
    r->display();
    printf("\n");
    
    printf("******** GREEN CHANNEL ************\n");
    g->display();
    printf("\n");
    
    printf("******** BLUE CHANNEL ************\n");
    b->display();
    printf("\n");
    
}











