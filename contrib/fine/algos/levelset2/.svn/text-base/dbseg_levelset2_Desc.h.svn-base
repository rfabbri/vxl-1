/*
 *  Desc.h
 *  ImageApp
 *
 *  Created by Brendan Moore on 4/1/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "dbseg_levelset2_Mh.h"
#include "dbseg_levelset2_BMatrix.h"
#include "dbseg_levelset2_ImageObj.h"
#include "dbseg_levelset2_Util.h"

using namespace std;


class Desc
{
    
public:
    Desc();
    Desc(int n);
    ~Desc();
    
    void kill();
    
    void init(int n);
    void print();
    void output(char *path);
        
    BMatrix *hists;
    double rm, normrm;
    ImageObj *rImgs, *iImgs;
    ImageObj *specs;
    int numImages;
    
    
    
};


