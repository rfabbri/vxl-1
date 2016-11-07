#include "cedt3d.h"
void slicePrint(int* arr,const int& numEl, const int& w, const int& h)
{
        char garbage;
        int i;

        for(i = 0; i < numEl; i++) {
                printf("%d ",arr[i]);    
                if( (i+1) % w == 0) printf("\n");
                if( (i+1) % (w*h) ==0) {
                        printf("\n"); 
                        scanf("%c",&garbage);
                }
        }

}
void slicePrint(unsigned* arr,const int& numEl, const int& w, const int& h)
{
        char garbage;
        int i;

        for(i = 0; i < numEl; i++) {
                printf("%d ",arr[i]);    
                if( (i+1) % w == 0) printf("\n");
                if( (i+1) % (w*h) ==0) {
                        printf("\n"); 
                        scanf("%c",&garbage);
                }
        }

}
void slicePrint(double* arr,const int& numEl, const int& w, const int& h)
{
        char garbage;
        int i;

        for(i = 0; i < numEl; i++) {
                printf("%1.1f ",arr[i]);    
                if( (i+1) % w == 0) printf("\n");
                if( (i+1) % (w*h) ==0) {
                        printf("\n"); 
                        scanf("%c",&garbage);
                }
        }

}

void slicePrintxyz(double* arr, int xdim, int ydim, int zdim)
{
int i;
        //unused char garbage;
        /*
        for(int iz=0; iz<zdim; iz++) {
                for(int iy=0; iy<ydim; iy++) {
                        for(int ix=0; ix<xdim; ix++) {
*/
            for(int iz=2; iz<zdim-2; iz++) {
                for(int iy=2; iy<ydim-2; iy++) {
                        for(int ix=2; ix<xdim-2; ix++) {

                            i = iz*xdim*ydim + iy*xdim + ix;
                                printf("%1.1f ",arr[i]);    
                        } 
                        printf("\n");
                }
                printf("\n");
                //scanf("%c",&garbage);
        }
    
}


void slicePrint(const vil3d_image_view<vxl_byte> arr)
{
        char garbage;
        //unused int i;


        for(int k = 0; k < static_cast<int>(arr.nk()); k++){
                for(int j = 0; j < static_cast<int>(arr.nj()); j++){
                        for(int i = 0; i < static_cast<int>(arr.ni()); i++){
                                printf("%1.1d ",arr(i,j,k));    
                        }
                        printf("\n"); 
                }
                        printf("\n"); 
                        scanf("%c",&garbage);
        }

}
void slicePrint(const vil3d_image_view<float> arr)
{
        char garbage;
        //unused int i;

        printf("slices = zeros(%d,%d,%d);\n ",arr.ni(),arr.nj(),arr.nk());


        for(int k = 0; k < static_cast<int>(arr.nk()); k++){
                for(int j = 0; j < static_cast<int>(arr.nj()); j++){
                        for(int i = 0; i < static_cast<int>(arr.ni()); i++){
                                printf("%1.3f ",arr(i,j,k));    
                        }
                        printf(";\n"); 
                }
                        printf("];\n"); 
                        scanf("%c",&garbage);
        }

}
void slicePrint(const vil3d_image_view<double> arr)
{
        char garbage;
        //unused int i;


        for(int k = 0; k < static_cast<int>(arr.nk()); k++){
                for(int j = 0; j < static_cast<int>(arr.nj()); j++){
                        for(int i = 0; i < static_cast<int>(arr.ni()); i++){
                                printf("%1.1f ",arr(i,j,k));    
                        }
                        printf("\n"); 
                }
                        printf("\n"); 
                        scanf("%c",&garbage);
        }

}

/*
double minDist(const int& x, const int& y, const int& z, const int& xdim, const int& ydim, const int& zdim, double* arr)
{
        int iSliceSize = xdim*ydim;
        int ix,iy,iz;
        int i = z*iSliceSize + y*xdim + x;

        if(arr[i] == 0) return 0;
        double minDist = -1; 
        double dist = 0;

        for(iz=0; iz<zdim; iz++) {
                for(iy=0; iy<ydim; iy++) {
                        for(ix=0; ix<xdim; ix++) {
                                i = iz*iSliceSize + iy*xdim + ix;
                                if(arr[i] == 0){
                                        dist = (x-ix)*(x-ix) +(y-iy)*(y-iy) +(z-iz)*(z-iz);
                                        if(minDist < 0 || dist < minDist ){ 
                                                minDist = dist;
                                        }
                                }    
                        } 
                }
        }
        return minDist;
}





*/
