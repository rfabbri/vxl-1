#line 1 "d:/projects/lemsvxl/src/contrib/Gregistration/exe/multiple_frame_polygon.h"




























extern "C" __declspec(dllimport) unsigned int intializemodel(int ni,int nj);
extern "C" __declspec(dllimport)  void updatemodel(unsigned int handle, unsigned char *
input_img,unsigned int maxComp,float initialstd,float intialweight,int
ni,int nj);

extern "C" __declspec(dllimport) void savemodel(unsigned int handle,char *filename);
extern "C" __declspec(dllimport) unsigned int loadmodel(char *filename);

extern "C" __declspec(dllimport)  void updateedgemodel(unsigned int handle, unsigned char *
input_img,unsigned int maxComp,float initialstd,float intialweight,int
ni,int nj);

extern "C" __declspec(dllimport)  unsigned int getpolygon(unsigned int handle,unsigned char* img_data,float sigmathresh,float minweightthresh,
                                       int min_no_of_pixels,int max_no_of_pixels,float erosionrad,
                                       float postdilationerosionrad,float dilationrad,int ni,int nj,float sigma,int rad_of_uncert);









