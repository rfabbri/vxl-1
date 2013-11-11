/*
 *  Desc.cpp
 *  ImageApp
 *
 *  Created by Brendan Moore on 4/1/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "dbseg_levelset2_Desc.h"



Desc::Desc() {

    numImages = -1;
    hists = NULL;
    rImgs = NULL;
    iImgs = NULL;
    specs = NULL;
}


Desc::Desc(int n) {
    
    numImages = n;
    
    hists = new BMatrix[n];    
    for(int i=0; i<n; i++) {
        hists[i].init(1,256);
        
        for(int k=0; k<256; k++)
            hists[i].setVal(0,k,0);
    }
    
    rImgs = new ImageObj[n];
    iImgs = new ImageObj[n];
    specs = new ImageObj[n];
        
}



Desc::~Desc() {
    
    
    delete[] hists;
    hists = NULL;    
    delete[] rImgs;
    rImgs = NULL;
    delete[] iImgs;
    iImgs = NULL;
    delete[] specs;
    specs = NULL;
    
        
}

void Desc::kill() {
    
    for(int i=0; i<numImages; i++) {
        
        hists[i].kill();
        rImgs[i].kill();
        iImgs[i].kill();
        specs[i].kill();

        
    }
    
    
    delete[] hists;
    hists = NULL;    
    delete[] rImgs;
    rImgs = NULL;
    delete[] iImgs;
    iImgs = NULL;
    delete[] specs;
    specs = NULL;
    
        
}

void Desc::init(int n) {
    
    hists = new BMatrix[n];
    numImages = n;
    
    
    for(int i=0; i<n; i++) {
        hists[i].init(1,256);
        
        for(int k=0; k<256; k++)
            hists[i].setVal(0,k,0);
    }
    
    rImgs = new ImageObj[n];
    iImgs = new ImageObj[n];
    specs = new ImageObj[n];
    
}


void Desc::output(char *path) {
    
    char b[100];
    /* eli edit
    for(int i=0; i<numImages; i++) {
        
        ShiftImage(rImgs[i]);
        sprintf(b,"%s/00%d_rImg.jpg", path, i+1);
        WriteJPEG(b,rImgs[i]);
        
        ShiftImage(iImgs[i]);
        sprintf(b,"%s/00%d_iImg.jpg", path, i+1);
        WriteJPEG(b,iImgs[i]);
        
        sprintf(b,"%s/00%d_sImg.jpg", path, i+1);
        WriteJPEG(b,specs[i]);
        
        sprintf(b,"%s/00%d_hist.ps", path, i+1);
        hists[i].plot(b);
        
        
    }
    */
    
}









