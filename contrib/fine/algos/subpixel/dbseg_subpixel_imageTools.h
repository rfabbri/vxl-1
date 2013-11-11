#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* void  BilinearInterpolate(int a, int b);*/

void  BilinearInterpolate(double *xCoords, double *yCoords, double *colorArray, double *outArray,int xLen, int yLen, int inputXLen, int inputYLen, int xOrigin, int yOrigin);

void rotate3d(double *inputColorArray, double *outputColorArray,
          float theta, float phi, float psi, 
          int inputXSize, int inputYSize, int inputZSize,
          int outputXSize, int outputYSize, int outputZSize,
          int startX, int startY, int startZ);

#ifdef __cplusplus
}
#endif

