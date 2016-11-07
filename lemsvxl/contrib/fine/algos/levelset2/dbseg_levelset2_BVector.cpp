/*
 *  BVector.cpp
 *  ImageApp
 *
 *  Created by Brendan Moore on 3/24/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "dbseg_levelset2_BVector.h"



BVector::BVector() {}


BVector::BVector(int d) {
    
    v = new double[d];
    dim = d;
    
}

BVector::~BVector() {
    
    delete[] v;
}



double BVector::operator *(BVector *b) {
    
    
    double val=0;
    
    for(int i=0; i<dim; i++) {
        
        val += v[i]*b->v[i];
        
    }
    
    return val;
}



void BVector::normalize() {

    calcMag();
    double val;
    for(int i=0; i<dim; i++) {
        
        val = v[i];
        val = val/mag;
        v[i] = val;
        
    }
    
}


void BVector::calcMag() {
    
    double val, sum=0;
    for(int i=0; i<dim; i++) {
        
        val = v[i];
        val *= val;
        sum += val;
        
    }
    
    mag = sqrt(sum);
}


void BVector::print() {
    
    int cnt = 0;
    printf("[");
    for(int i=0; i<dim; i++) {
                
        printf(" %5.5f ", v[i]);
        cnt++;
        
        if(cnt == 8) { 
            printf("]\n[");
            cnt = 0;
        }

        
    }
    

}







