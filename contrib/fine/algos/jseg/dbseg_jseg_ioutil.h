#ifdef __cplusplus
extern "C" {
#endif

#ifndef __IOUTIL_H
#define __IOUTIL_H

#define I_YUV  1
#define I_RGB  2
#define I_GRAY 3
#define I_PGM  4
#define I_PPM  5
#define I_JPG  6
#define I_GIF  9
#define V_YUV  21
#define V_RGB  22

#define P_SEG 1
#define P_QUA 2
#define P_BW  3

void outputEdge(char *fname,char *exten,unsigned char *RGB0,unsigned char *rmap,
    int ny,int nx,int status,int type,int dim,float displayintensity);

void outputresult(int media_type,char *outfname,unsigned char *RGB,int ny,int nx,
    int dim);

int inputimggif(char *infname,unsigned char **RGB,int *ny,int *nx);
void outputimggif(char *outfname,unsigned char *RGB,int ny,int nx,int dim);

void inputimgraw(char *fname,unsigned char *img,int ny,int nx,int dim);
void outputimgraw(char *fname,unsigned char *img,int ny,int nx,int dim);

void inraw2float (char *fname,float *img,int ny,int nx,int dim);
void outfloat2raw(char *fname,float *img,int ny,int nx,int dim);

void inputimgyuv(char *fname,unsigned char *img,int ny,int nx);
void outputimgyuv(char *fname,unsigned char *img,int ny,int nx);

void inputimgpm(char *fname,unsigned char **img,int *ny,int *nx);
void outputimgpm(char *fname,unsigned char *img,int ny,int nx,int dim);

int inputimgjpg(char *filename,unsigned char **RGB,int *NY,int *NX);
void outputimgjpg(char *filename,unsigned char *RGB,int NY,int NX,int dim);


#endif


#ifdef __cplusplus
}
#endif

