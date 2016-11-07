/*
 *  BPoint.cpp
 *  ImageApp
 *
 *  Created by Brendan Moore on 3/24/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "dbseg_levelset2_BPoint.h"



BPoint::BPoint() {

    x = -1;
    y = -1;
    prev = NULL;
    next = NULL;
    

}


BPoint::BPoint(int cx, int cy) {
    
    x = cx;
    y = cy;
    prev = NULL;
    next = NULL;
    
}


BPoint::~BPoint() {}

void BPoint::print() {
    
    printf("(%d,%d\n)", x,y); 
    
}







