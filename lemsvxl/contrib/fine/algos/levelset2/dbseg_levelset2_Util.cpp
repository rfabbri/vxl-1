/*
 *  Util.cpp
 *  ImageApp
 *
 *  Created by Brendan Moore on 2/15/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "dbseg_levelset2_Util.h"


/*eli edit
int file_select(struct direct *entry) {
    
    //printf("%s\n", entry->d_name);
    
    if ((strcmp(entry->d_name, ".") == 0) || 
        (strcmp(entry->d_name, "..") == 0) ||
        (entry->d_name[0] == '.')) { 
        return (0);
    } else {
        return (1);
    }
}


void ReadPGM(string filename, ImageObj& image) {
    
    string mn, com;
    int width, height, maxVal;
    double pVal;
    
    ifstream file(filename.c_str());
    
    if (!file.is_open()) {
        printf("\nERROR: Error Opening %s", filename.c_str());
        exit(1);
    }
    
    getline(file, mn);
    getline(file, com);
    
    file >> width >> height >> maxVal;
    
    image.init(width, height);
    
    image.storeMagicNum(mn);
    image.storeComments(com);
    image.setMaxVal((double)maxVal);
    
    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
            file >> pVal;
            image.setPixelVal(x,y,pVal);
        }
    }
    
    file.close();

}



void WritePGM(string filename, const ImageObj& image) {
        
    ofstream file (filename.c_str());
    file << image.getMagicNum() << endl;
    file << image.getComments() << endl;
    file << image.getImageWidth() << " " << image.getImageHeight() << endl;
    //file << image->getMaxVal() << endl;
    file << 255 << endl;
    
    for(int y=0; y < image.getImageHeight(); y++) {
        for(int x=0;  x<image.getImageWidth(); x++) {
            
            file << setw(5) << rnd(image.getPixelVal(x,y));
        }
        file << endl;
    }
    
    file.close();
    
}



void ReadPPM(string filename, RGBImg& image) {
    
    string mn, com;
    int width, height, maxVal;
    double rVal,gVal,bVal;
    
    ifstream file(filename.c_str());
    
    if (!file.is_open()) {
        printf("\nERROR: Error Opening %s", filename.c_str());
        exit(1);
    }
    
    getline(file, mn);
    getline(file, com);
    
    file >> width >> height >> maxVal;
    
    image.init(width, height);
    
    image.storeMagicNum(mn);
    image.storeComments(com);
    image.setMaxVal((double)maxVal);

    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
            file >> rVal;
            file >> gVal;
            file >> bVal;
            image.r->setPixelVal(x,y,rVal);
            image.g->setPixelVal(x,y,gVal);
            image.b->setPixelVal(x,y,bVal);
        }
    }
    
    file.close();
}



void WritePPM(string filename, const RGBImg& image) {
    
    ofstream file (filename.c_str());
    
    file << image.getMagicNum() << endl;
    file << image.getComments() << endl;
    file << image.getWidth() << " " << image.getHeight() << endl;
    file << 255 << endl;
    
    for(int y=0; y < image.getHeight(); y++) {
        for(int x=0;  x< image.getWidth(); x++) {
            
            file << setw(5) << rnd(image.r->getPixelVal(x,y));
            file << setw(5) << rnd(image.g->getPixelVal(x,y));
            file << setw(5) << rnd(image.b->getPixelVal(x,y));
            file << "    ";
        }
        file << endl;
    }
    
    file.close();
}




void ReadJPEG(string filename, RGBImg& dest) {
    
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;        
    int row_stride;
    
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    
    FILE *infile;
    
    if((infile = fopen(filename.c_str(), "rb")) == NULL) {
        fprintf(stderr, "Can't open %s\n", filename.c_str());
        exit(1);
    }
    
    jpeg_stdio_src(&cinfo, infile);
    
    jpeg_read_header(&cinfo,TRUE);
    
    jpeg_start_decompress(&cinfo);
    
    dest.init(cinfo.output_width,cinfo.output_height);
    
    
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, 
                                         JPOOL_IMAGE, row_stride, 1);
    
    int i, x, y=0;
    while(cinfo.output_scanline < cinfo.output_height) {
        x=0;
        jpeg_read_scanlines(&cinfo,buffer,1);
        
        for(i=0; i< row_stride; i+=3) {
            
            dest.r->setPixelVal(x,y,buffer[0][i]);
            dest.g->setPixelVal(x,y,buffer[0][i+1]);
            dest.b->setPixelVal(x,y,buffer[0][i+2]);
            x++;
        }
        y++;
        
        
    }
    
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    
}
*/



/* eli edit

void ReadJPEG(string filename, ImageObj& dest) {
    
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;        
    int row_stride;
    
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    
    FILE *infile;
    
    if((infile = fopen(filename.c_str(), "rb")) == NULL) {
        fprintf(stderr, "Can't open %s\n", filename.c_str());
        exit(1);
    }
    
    jpeg_stdio_src(&cinfo, infile);
    
    jpeg_read_header(&cinfo,TRUE);
    
    jpeg_start_decompress(&cinfo);
    
    dest.init(cinfo.output_width, cinfo.output_height);
    
    
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, 
                                         JPOOL_IMAGE, row_stride, 1);
    
    int i, x, y=0;
    double s;
    while(cinfo.output_scanline < cinfo.output_height) {
        x=0;
        jpeg_read_scanlines(&cinfo,buffer,1);
        
        
        //for(i=0; i< row_stride; i+=3) {
            
        //    s = buffer[0][i] + buffer[0][i+1] + buffer[0][i+2];
        //    s = s/3;
        //    dest.setPixelVal(x,y,s);
        //    x++;
        //}
        
        
        for(i=0; i< row_stride; i++) {
            
            s = buffer[0][i];
            dest.setPixelVal(x,y,s);
            x++;
        }
        
        
        y++;
    }
    
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    

    
}



void WriteJPEG(string filename, const RGBImg& src) {
    
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    
    FILE * outfile;
    
    if ((outfile = fopen(filename.c_str(), "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename.c_str());
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);
    
    cinfo.image_width = src.getWidth();
    cinfo.image_height = src.getHeight();
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    
    jpeg_set_defaults(&cinfo);
    
    jpeg_start_compress(&cinfo, TRUE);
    JSAMPROW row_pointer[1];    
    int row_stride;        
    
    row_stride = src.getWidth() * 3;
    char row_buffer[row_stride];
    int i,x,y=0;
    
    
    
    while (cinfo.next_scanline < cinfo.image_height) {
        
        x=0;
        for(i=0; i<row_stride; i+=3) {
            row_buffer[i] = (char)src.r->getPixelVal(x,y);
            row_buffer[i+1] = (char)src.g->getPixelVal(x,y);
            row_buffer[i+2] = (char)src.b->getPixelVal(x,y);
            x++;
        }
        
        row_pointer[0] = (JSAMPLE*)&row_buffer[0];
        
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
        y++;
    }
    
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
    
}




void WriteJPEG(string filename, const ImageObj& src) {
    
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    
    FILE * outfile;
    
    if ((outfile = fopen(filename.c_str(), "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename.c_str());
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);
    
    cinfo.image_width = src.getImageWidth();
    cinfo.image_height = src.getImageHeight();
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    
    jpeg_set_defaults(&cinfo);
    
    jpeg_start_compress(&cinfo, TRUE);
    JSAMPROW row_pointer[1];    
    int row_stride;        
    
    row_stride = src.getImageWidth() * 3;
    char row_buffer[row_stride];
    int i,x,y=0;
    
    
    
    while (cinfo.next_scanline < cinfo.image_height) {
        
        x=0;
        for(i=0; i<row_stride; i+=3) {
            row_buffer[i] = (char)src.getPixelVal(x,y);
            row_buffer[i+1] = (char)src.getPixelVal(x,y);
            row_buffer[i+2] = (char)src.getPixelVal(x,y);
            x++;
        }
        
        row_pointer[0] = (JSAMPLE*)&row_buffer[0];
        
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
        y++;
    }
    
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
    
}
*/


double rnd(double val) {
    
    double fd, cd;
    
    fd = fabs (val - floor(val));
    cd = fabs (val - ceil(val));
    
    if (fd < cd)
        return floor(val);
    else
        return ceil(val);
    
}

/* eli edit
void MakeDirs() {
    
    struct stat fstat;
    
    if(stat("./images", &fstat) != 0) {
        printf("Creating Output Dirs\n");
        system("mkdir ./images");
    } else {
        printf("Removing Prev Output\n");
        system("rm -r ./images");
        printf("Creating Output Dirs\n");
        system("mkdir ./images");
    }
     
}

*/


void ShiftImage(ImageObj& image) {
    
    image.findMM();
    
    double iMin = image.getMinVal();
    double iMax = image.getMaxVal();
    double M = (double)(MAXVALUE - MINVALUE)/(iMax - iMin);
    double r;
    double pix;
    
    for(int y=0; y<image.getImageHeight(); y++) {
        for(int x=0; x<image.getImageWidth(); x++) {
            
            pix= image.getPixelVal(x,y) - iMin;
            r = M*pix + MINVALUE;
            image.setPixelVal(x,y,rnd(r));
            
        }
    }
    image.findMM();
    
}



void ShiftImage(ImageObj& image, int min, int max) {
    
    image.findMM();
    
    double iMin = image.getMinVal();
    double iMax = image.getMaxVal();
    
    double M = (double)(max - min)/(iMax - iMin);
    double r;
    double pix;
    
    for(int y=0; y<image.getImageHeight(); y++) {
        for(int x=0; x<image.getImageWidth(); x++) {
            
            pix= image.getPixelVal(x,y) - iMin;
            r = M*pix;
            image.setPixelVal(x,y,r);
            
        }
    }
    image.findMM();
    
}


void ShiftImage(ImageObj& image, double newMin, double newMax,
               double iMin, double iMax) {
    
    double M = (double)(newMax - newMin)/(iMax - iMin);
    double r;
    double pix;
    
    for(int y=0; y<image.getImageHeight(); y++) {
        for(int x=0; x<image.getImageWidth(); x++) {
            
            pix= image.getPixelVal(x,y) - iMin;
            r = M*pix + newMin;
            image.setPixelVal(x,y,r);
            
        }
    }
    image.findMM();
    
}

void Threshold(const ImageObj& image, ImageObj& dest, int th) {
    
    dest = image;;
    float val =0;
    
    for(int y=0; y<dest.getImageHeight(); y++) {
        for(int x=0; x<dest.getImageWidth(); x++) {
            
            val = dest.getPixelVal(x,y);
            
            if(val >= th)
                val = 0;
            else
                val = 255;
            
            dest.setPixelVal(x,y,val);
            
            
        }
    }
        
}



void DrawHist(ImageObj& src) {
    

    ShiftImage(src,0,20);
    
    printf("theta\t\tCount\n");
    printf("--------------\n");
    int count;

    
    for(int i=0; i<src.getImageWidth(); i++) {
        
        
        printf("%d\t\t",i);
        
        count = (int)src.getPixelVal(i,0);
        
        for(int j=0; j<count; j++){
            if(j == 15)
                printf("@");
            else
                printf("#");
        }
        
        printf("\n");
    }
    
    printf("\n\n");
    
}



/* eli edit

void ReadImageDir(string path, ImageObj* &dArray) {
    
    struct direct **files;
    struct direct *tmp;
    
    int cnt = scandir(path.c_str(), &files, file_select, NULL);
    
    dArray = new ImageObj[cnt];
    dArray[0].cnt = cnt;
    
    for(int i=0; i<cnt; i++) {
        tmp = files[i];
        string curFile = path + tmp->d_name;
        ReadJPEG(curFile, dArray[i]);
        
    }
}


*/


double BLInterp(const ImageObj& src, double x, double y) {
    
    double dy, dx;
    double val;
    int flx, fly;
    
    dx = x - floor(x);
    dy = y - floor(y);
    
    
    flx = (int)floor(x);
    fly = (int)floor(y);
    
    
    val = (1-dx)*(1-dy)*src.getPixelVal(flx, fly) +
        dx*(1-dy)*src.getPixelVal(flx + 1, fly) +
        (1-dx)*dy*src.getPixelVal(flx, fly + 1) +
        dx*dy*src.getPixelVal(flx + 1, fly + 1);
    
    val = (1-dx)*(1-dy)*src.getPixelVal(flx, fly);
    val = val + dx*(1-dy)*src.getPixelVal(flx + 1, fly);
    val = val +    (1-dx)*dy*src.getPixelVal(flx, fly + 1);
    val = val + dx*dy*src.getPixelVal(flx + 1, fly + 1);
    
    return val;
    
}



void ScaleImage(const ImageObj& src, ImageObj& dest, float pw, float ph) {
    
    int x0 = src.getImageWidth();
    int y0 = src.getImageHeight();
    
    int x1 = (int)/* eli edit round*/(x0*pw);
    int y1 = (int)/* eli edit round*/(y0*ph);

    
    dest.init(x1,y1);
    
    double m1 = (double)x0/x1;
    double m2 = (double)y0/y1;
    double nx, ny;
    double pix;
    
    for(int y=0; y<dest.getImageHeight(); y++) {
        ny = m2*y;
        for(int x=0; x<dest.getImageWidth(); x++) {
            
            nx = m1*x;
            
            pix = BLInterp(src, nx, ny);
            dest.setPixelVal(x,y,pix);
            
            
        }
    }
    
    
}













