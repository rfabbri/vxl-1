/*
 *  BVector.h
 *  ImageApp
 *
 *  Created by Brendan Moore on 3/24/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "dbseg_levelset2_Mh.h"

using namespace std;


class BVector
{
    
public:
    BVector();
    BVector(int d);
    ~BVector();
    
    double operator *(BVector *B); // dot product
    
    void normalize();
    
    void calcMag();
    
    void print();
    
    double* v;
    int dim;
    double mag;
};


