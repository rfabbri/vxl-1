/*
ImageApp.cpp

  Desc:    This is the entry point for the application
 Usage:    ImageApp <inputfile> 
  
    
*/

// includes
#include "Mh.h"
#include "Util.h"
#include "ImageObj.h"
#include "BMatrix.h"
#include "RGBImg.h"
#include "BVector.h";
#include "BPoint.h";
#include "Desc.h";

using namespace std;


// defines
#define MAXDIST 5



// globals
int g_iCnt=0;
int gVerbose = 0;
double e =  2.71828,  pi=3.14159;


void TwoDConvolve(const ImageObj& image, const BMatrix& sm, ImageObj& dest) {
    
    int nr = sm.getNumRows();
    int nc = sm.getNumRows();
    int iw = image.getImageWidth();
    int ih = image.getImageHeight();
    BMatrix nMat =  BMatrix(nr, nc);
    dest.init(iw, ih);
    
    double min = 99999, max = -99999;
    int rch = (sm.getNumCols()-1)/2;;
        
    for(int y=0; y<image.getImageHeight(); y++) {
        for(int x=0; x<image.getImageWidth(); x++) {
            double cVal = -1;
            
            //make Neighbor BMatrix
            double val;
            
            int j=-rch;
            for(int r=0; r<nMat.getNumRows(); r++) {
                int i=-rch;
                for(int c=0; c<nMat.getNumCols(); c++) {
                    val = image.getPixelVal(x+i,y+j);
                    nMat.setVal(r,c,val);
                    i++;
                }
                j++;
            }
            
            //nMat.print();
            //sm->print();    
            
            cVal = sm.convolve(nMat);
            
            if(cVal < min)
                min = cVal;
            
            if(cVal > max)
                max = cVal;
            
            dest.setPixelVal(x,y,cVal);
            
        }
    }
    

    dest.setMaxVal(max);
    dest.setMinVal(min);
    

}


void ReadInitContour(const ImageObj &src, BPoint &plist) {
    
    int w = src.getImageWidth();
    int h = src.getImageHeight();
    double val;
    BPoint *cur = &plist;
    
    
    for(int y=0; y<h; y++) {
        for(int x=0; x<w; x++) {
        
            val = src.getPixelVal(x,y);
            
            if(val == 0) {
                cur->next = new BPoint(x,y);
                cur->next->prev = cur;
                cur = cur->next;
            }
            
            
        }
    }
    
    
}


void CreateInitContour(const ImageObj& srcImg, BPoint& plist, ImageObj& binImg) {
    
    
    int w = srcImg.getImageWidth();
    int h = srcImg.getImageHeight();
    

    
    BPoint *cur = &plist;
    
    
    int rWidth = 20;
    int rHeight = 20;
    

    int startx = w/2 - rWidth/2;
    int starty = h/2 - rHeight/2;
    
        
    for(int x=startx; x<=startx+rWidth; x++) {
                        
        
        cur->next = new BPoint(x,starty);
        cur->next->prev = cur;
        cur = cur->next;

        cur->next = new BPoint(x,starty+rHeight);
        cur->next->prev = cur;
        cur = cur->next;
        
        
        
    }
    
    for(int y=starty; y<starty+rHeight; y++) {
        
        
        cur->next = new BPoint(startx,y);
        cur->next->prev = cur;
        cur = cur->next;
        
        cur->next = new BPoint(startx+rWidth,y);
        cur->next->prev = cur;
        cur = cur->next;
        
        
    }

}



void PlotSurface(const ImageObj& src) {
    
    char b[100];
    ImageObj sml;
    ScaleImage(src, sml, .2, .2);

    printf("Building Plot\n");
    sprintf(b, "./images/plot.ps");
    sml.splot(b);

    
}



void WritePList(BPoint& plist, ImageObj& dest) {
    
    char b[100];
    int x, y, i=0, j=0;
    BPoint *cur = &plist;
    cur = cur->next;
    
    ImageObj* tmp;
    tmp = dest.copy();

    while(cur != NULL) {
        
        x = cur->x;
        y = cur->y;

        tmp->setPixelVal(x+i,y+j,0);
        cur = cur->next;
        
    }
    
    sprintf(b,"./images/0%d_cont.jpg", ++g_iCnt); 
    WriteJPEG(b, *tmp);

    delete tmp;
    
}



double GetSpeed(const ImageObj& src, int x, int y) {
    
    double I, T=50, sig=10, rval;
    long aval, diff;
    
    I = src.getPixelVal(x,y);
    
    diff =(long)(I-T);
    aval = abs(diff);
    
    
    rval = sig - aval;
    
    return rval;
}





void LevelSet(ImageObj& src, ImageObj& binImg, BPoint& plist, ImageObj& fImg) {
    
    
    BPoint *cur;
    double phi;
    int ax, ay, i=0, x, y;
    double cval, gmag, f, sum;

    
    

    while(i < 1) { // when to stop the whole process;
        
        cur = &plist;
        cur = cur->next;
    
            
        while(cur != NULL) {
            
            ax = cur->x;
            ay = cur->y;
            sum =0;
            phi = 0;

            
            //gmag = RicGMag(src, ax, ay);
            gmag = 0;
            
            for(int j=-5; j<=5; j++) {
                for(int i=-5; i<=5; i++) {
                    
                    x = ax+i;
                    y = ay+j;
                    
                    cval = binImg.getPixelVal(x,y);
                    
                    f = fImg.getPixelVal(x,y);
                
                /*    
                    if(f > -.5 &&  gmag < 80)
                        f= -10000; //  its bgd
                    else 
                        f = 10000; // its obj
                */    
                    
                    if(f < 0 &&  gmag < 120)
                        f= -10000; //  its bgd
                    else 
                        f = 10000; // its obj
                    
                    
                    
                    
                    /*
                    if(gmag < GTHRESH)
                        f = -10000;
                    else
                        f = 10000;
                     */
                    
                    
                    //phi = cval - gmag*f;
                    phi = cval - f;
                    sum += phi;
                    
                    
                }
            }
            
            sum /= 25;
            
            
            binImg.setPixelVal(ax,ay,sum);
            cur = cur->next;
        }    
        
        i++;
    }

        
}





void DeleteList(BPoint& plist) {
    
    
    BPoint *cur = &plist;
    BPoint *tmp;
    
    cur = cur->next;
    
    while(cur != NULL) {
        
        tmp = cur->next;
        delete cur;
        cur = tmp;
        
    }
    
    
    plist.next = NULL;



}



void DT(BPoint& plist, ImageObj& dest) {
    
    int x, y, cx, cy;
    double val;
    double sf = 25;
    
    BPoint* cur = &plist;
    cur = cur->next;
    while(cur != NULL) {
        
        x = cur->x;
        y = cur->y;
        
        dest.setPixelVal(x,y,0);

        
        for(int d=1; d < MAXDIST; d++) {
            for(int j=-d; j<=d; j++) {
                cy = y+j;
                for(int i=-d; i<=d; i++) {
                    
                    cx = x+i;
                    
                    val = dest.getPixelVal(cx,cy);
                    if(d*sf < val) 
                        dest.setPixelVal(cx,cy,d*sf);
                    
                }
            }
        }
        cur = cur->next;
    }
    

    //compute surface normal
    double dx, dy, dyx, dxy, tmp;
    
    x = plist.next->x;
    y = plist.next->y;
    
    dx = dest.getPixelVal(x+1,y) - dest.getPixelVal(x,y);
    dy = dest.getPixelVal(x,y-1) - dest.getPixelVal(x,y);
    
    dyx = (dest.getPixelVal(x+1,y-1) - dest.getPixelVal(x+1,y+1))/2.0;
    dxy = (dest.getPixelVal(x+1,y-1) - dest.getPixelVal(x-1,y-1))/2.0;
    
    tmp = sqrt(dx*dx + ((dyx+dy)/2.0)*((dyx+dy)/2.0) + 0);
    
    dest.pn->v[0] = dx/tmp;
    
    tmp = sqrt(dy*dy + ((dxy+dx)/2.0)*((dxy+dx)/2.0) + 0);
    
    dest.pn->v[1] = dy/tmp;
    
    dest.pn->v[2] = 0;
    
    WritePGM("./images/dt.cpp",dest);
    //PlotSurface(dest);
    
    
    
}


int main(int argc, char* argv[]) {
    
    char b[100];
    string srcImgPath, initContPath;
    ImageObj srcImg, initCont, dtImg;
    BPoint plist = BPoint();
    
    if(argc != 4) {
        printf("Usage: ImageApp <Src Image> <Init Contour>  <0/1 (1 for verbose output)>\n");
        exit(0);
    }
    
    srcImgPath = argv[1];
    initContPath = argv[2];
    gVerbose = atoi(argv[3]);
    
    MakeDirs();
    ReadJPEG(srcImgPath, srcImg);
    srcImg.cnt = 1;
    
    ReadJPEG(initContPath, initCont);
    initCont.cnt = 1;
    
    
    dtImg.init(srcImg.getImageWidth(),srcImg.getImageHeight());
    dtImg.erase(255);
    
    printf("Building Initial Contour\n");
    ReadInitContour(initCont, plist);
    //CreateInitContour(srcImg, plist, dtImg);
    WritePList(plist, dtImg);
    
    DT(plist, dtImg);
    
    WriteJPEG("./images/DT.jpg", dtImg);
    
}










