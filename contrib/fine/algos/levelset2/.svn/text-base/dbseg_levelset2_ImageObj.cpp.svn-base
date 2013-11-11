/*
File: ImageObj.cpp

*/

#include "dbseg_levelset2_ImageObj.h"

ImageObj::ImageObj() {

    mat = new BMatrix();
    w = -1;
    h = -1;
    storeMagicNum("P2");
    storeComments("#Made by ImageApp");
    
    pn = new BVector(3);
    nn = new BVector(3);
    
}


ImageObj::ImageObj(int width, int height)  {
    
    mat = new BMatrix(height, width);
    w = width;
    h = height;    
    storeMagicNum("P2");
    storeComments("#Made by ImageApp");
    
    pn = new BVector(3);
    nn = new BVector(3);

}



ImageObj::ImageObj(const ImageObj& rhs) {
    
    mat = rhs.mat;
    storeMagicNum(rhs.getMagicNum());
    storeComments(rhs.getComments());
    h = rhs.getImageHeight();
    w = rhs.getImageWidth();
    setMaxVal(rhs.getMaxVal());
    setMinVal(rhs.getMinVal());
    mirror = rhs.mirror;
    pn = rhs.pn;
    nn = rhs.nn;

    
}


ImageObj::~ImageObj() {
    
    if(mat != NULL) {
        delete mat;
        mat = NULL;
        
        delete pn;
        pn = NULL;
        
        delete nn;
        nn = NULL;
    }

    
}


void ImageObj::kill() {
    
    mat->kill();
    delete mat;
    mat = NULL;
    
    delete pn;
    pn = NULL;
    
    delete nn;
    nn = NULL;
    
}

void ImageObj::init(int cw, int ch) {
    
    mat->init(ch,cw);
    w = cw;
    h = ch;

    
}




void ImageObj::setPixelVal(int x, int y, double val) {
    
    if(x >= 0 && x < w) {
        if(y >= 0 && y < h) 
            mat->setVal(y,x,val);
    }
}


double ImageObj::getPixelVal(int x, int y) const {
    
    double rVal = -1;
    
    
    if(mirror) {
        
        if(x < 0) {
            x *= -1;
        }
        
        if(x >= w) {
            int t = w-x;
            t -= 2;
            x = w + t;
        }
        
        if(y < 0) {
            y *= -1;
            
        }
        
        if(y >= h) {
            
            int t = h-y;
            t -= 2;
            y = h + t;
        }
    } else {
        
        if((x < 0) || (x >= w))
            return 0;
        if((y < 0) || (y >= h))
            return 0;
    }
    
    
    rVal = mat->getVal(y,x);
    
    return rVal;
}


void ImageObj::storeMagicNum(string n) {
    
    mn = n;
}

string ImageObj::getMagicNum() const {
    
    return mn;
}

void ImageObj::storeComments(string c) {
    
    comments = c;
}

string ImageObj::getComments() const {
    
    return comments;
}

void ImageObj::setMaxVal(double m) {
    
    maxVal = m;
    
}

double ImageObj::getMaxVal() const {
    
    return maxVal;
}

void ImageObj::setMinVal(double m) {
    
    minVal = m;
    
}

double ImageObj::getMinVal() const {
    
    return minVal;
}

int ImageObj::getImageHeight() const {
    
    return h;
}


int ImageObj::getImageWidth() const {
    
    return w;
}

void ImageObj::display() {
    
    cout << getMagicNum() << endl;
    cout << getComments() << endl;
    cout << getImageWidth() << " " << getImageHeight() << endl;
    cout << getMaxVal() << endl;
    
    mat->print();
    
}

void ImageObj::splot(char *str) {
    
    mat->splot(str);
    
}


ImageObj* ImageObj::copy() {
    
    ImageObj *dest = new ImageObj(w,h);
    dest->storeMagicNum(mn);
    dest->storeComments(comments);
    dest->h = h;
    dest->w = w;
    dest->setMaxVal(maxVal);
    dest->setMinVal(minVal);
    dest->mirror = mirror;
    dest->mat = mat->copy();

    return dest;

    
}



ImageObj& ImageObj::operator=(const ImageObj& rhs) {
    
    // check for self assignment
    
    if(mat != NULL)
        delete mat;
    
    mat = new BMatrix();
    
    *mat = *(rhs.mat);
    storeMagicNum(rhs.getMagicNum());
    storeComments(rhs.getComments());
    h = rhs.getImageHeight();
    w = rhs.getImageWidth();
    setMaxVal(rhs.getMaxVal());
    setMinVal(rhs.getMinVal());
    mirror = rhs.mirror;
    
    
    return *this;
    
}



ImageObj& ImageObj::operator *(ImageObj &img) {
    
    double val, a, b;
    ImageObj *rVal = new ImageObj(getImageWidth(), getImageHeight());
    for(int i=0; i<getImageHeight(); i++) {
        for(int j=0; j<getImageWidth(); j++) {
            
            a = getPixelVal(j,i);
            b = img.getPixelVal(j,i);
            
            val = a * b;
            rVal->setPixelVal(j,i,val);
            
        }
    }
    
    rVal->findMM();
    return *rVal;
    
}


ImageObj& ImageObj::operator /(ImageObj &img) {
    
    double val, n, d;
    ImageObj *rVal = new ImageObj(getImageWidth(), getImageHeight());
    for(int i=0; i<getImageHeight(); i++) {
        for(int j=0; j<getImageWidth(); j++) {
            
            n = getPixelVal(j,i);
            d = img.getPixelVal(j,i);
            
            if(d == 0)
                d = .00001;
                
            val = n /d;             
            rVal->setPixelVal(j,i,val);
        }
    }
    
    rVal->findMM();
    return *rVal;
}


void ImageObj::operator +(ImageObj &img) {
    
    double val, a, b;
    for(int i=0; i<getImageHeight(); i++) {
        for(int j=0; j<getImageWidth(); j++) {
            
            a = getPixelVal(j,i);
            b = img.getPixelVal(j,i);
            
            val = a + b;
            setPixelVal(j,i,val);
        }
    }
    
    findMM();

}



ImageObj* ImageObj::operator -(ImageObj *img) {
    
    double val, a, b;
    ImageObj *rVal = new ImageObj(getImageWidth(), getImageHeight());
    for(int i=0; i<getImageHeight(); i++) {
        for(int j=0; j<getImageWidth(); j++) {
            
            a = getPixelVal(j,i);
            b = img->getPixelVal(j,i);
            
            val = a - b;
            
            rVal->setPixelVal(j,i,val);
            
        }
    }
    
    rVal->storeMagicNum("P2");
    rVal->storeComments("#Made by ImageApp");
    
    rVal->findMM();
    return rVal;
    

}

void ImageObj::operator -() {
    
    double val;
    for(int i=0; i<getImageHeight(); i++) {
        for(int j=0; j<getImageWidth(); j++) {
            
            val = getPixelVal(j,i) * -1;
            
            setPixelVal(j,i,val);
            
        }
    }
    
    findMM();
    
}

bool ImageObj::operator ==(ImageObj &img) {
    double val1, val2;
    
    for(int i=0; i<getImageHeight(); i++) {
        for(int j=0; j<getImageWidth(); j++) {
            
            val1 = this->getPixelVal(j,i);
            val2 = img.getPixelVal(j,i);
            
            if(val1 != val2)
                return false;
                                        
        }
    }
    
    return true;
    
}

void ImageObj::findMM() {

    double max=-99999, min =99999, p;
    for(int i=0; i<getImageHeight(); i++) {
        for( int j=0; j<getImageWidth(); j++) {
            p = getPixelVal(j,i);
            if(p < min)
                min = p;

            if(p > max)
                max = p;
        }
    }

    setMaxVal(max);
    setMinVal(min);

}

void ImageObj::erase(double color) {

    for(int i=0; i<getImageHeight(); i++) {
        for(int j=0; j<getImageWidth(); j++) {
            setPixelVal(j,i,color);
        }
    }



}

void ImageObj::calcmean() {

    double sum =0;
    for(int i=0; i<getImageHeight(); i++) {
        for(int j=0; j<getImageWidth(); j++) {
            sum += getPixelVal(j,i);
        }
    }
    

    mean = sum/(h*w);
    cout << "Image Mean: " << mean << endl;
}


void ImageObj::transpose() {
    
    mat->transpose();

    w = mat->getNumCols();
    h = mat->getNumRows();
    
    
}











