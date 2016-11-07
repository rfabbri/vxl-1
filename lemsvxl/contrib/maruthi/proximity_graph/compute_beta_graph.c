/*==========================================================
 * arrayProduct.c - example in MATLAB External Interfaces
 *
 * Multiplies an input scalar (multiplier) 
 * times a 1xN matrix (inMatrix)
 * and outputs a 1xN matrix (outMatrix)
 *
 * The calling syntax is:
 *
 *		outMatrix = arrayProduct(multiplier, inMatrix)
 *
 * This is a MEX-file for MATLAB.
 * Copyright 2007-2008 The MathWorks, Inc.
 *
 *========================================================*/
/* $Revision: 1.1.10.2 $ */

#include "mex.h"
#include "math.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

/* The computational routine */
void create_graph(double* distanceMatrix, mwSize rows, mwSize cols, double beta, double* pg_graph) {
    mwIndex ni, nj, nk;
    
    bool flag=true;
    /* multiply each element y by x */
    for (ni=0; ni<rows; ni++) 
    {
        for ( nj=ni+1; nj < cols ; nj++) 
        {
            flag=true;
            
            double dij=distanceMatrix[ni*cols+nj];
            for ( nk=0; nk < cols ; nk++) 
            {
                if (nk != ni && nk != nj) 
                {
                    double dik=distanceMatrix[ni*cols+nk];
                    double djk=distanceMatrix[nj*cols+nk];
                    if ( beta < 1) {
                        /* Beta_ less than 1 case */
                        if (  (dij*dij)
                        > ((dik*dik) +
                                (djk*djk) +
                                2*sqrt(1-(beta*beta))*
                                (dik*djk)  )) {
                            flag=false;
                            break;
                        }
                    }
                    else 
                    {
                        /* Beta_ greater than 1 case */
                        if ( dij*dij > MAX(
                                ((dik*dik)*((2/beta)-1))+(djk*djk)
                                , (dik*dik)+(((2/beta)-1)*(djk*djk)) )) {
                            
                            flag=false;
                            break;
                            
                        }
                        
                    }
 
                    
                }

                
            }
            
            if ( flag)
            {
                pg_graph[ni*cols+nj]=1.0;
                pg_graph[nj*cols+ni]=1.0;
            }
            
        }
    }
}

/* The gateway function */
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    double* distanceMatrix;         /* NxM input matrix */
    double beta;                    /* beta */  
    mwSize rows,cols;                /* Size of Matrix */ 
    double *pg_graph;               /* output matrix */

    /* check for proper number of arguments */
    if(nrhs!=2) {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs","Two inputs required.");
    }
    if(nlhs!=1) {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs","One output required.");
    }
    
    /* First argument must be a matrix */
    if (mxGetNumberOfDimensions(prhs[0]) != 2){
        mexErrMsgTxt("Input argument must be two dimensional\n");
    }
    
    /* beta must be a scalar */
    if( !mxIsDouble(prhs[1]) || 
         mxIsComplex(prhs[1]) ||
         mxGetNumberOfElements(prhs[1])!=1 ) {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:notScalar","Beta must be a scalar.");
    }
    
    /* Get distance matrix sizes */
    distanceMatrix =  mxGetPr(prhs[0]);
    rows  = mxGetM(prhs[0]);
    cols  = mxGetN(prhs[0]);
    
    /* get the value of the scalar input  */
    beta = mxGetScalar(prhs[1]);

    /* create the output matrix */
    plhs[0] = mxCreateDoubleMatrix(rows,cols,mxREAL);

    /* get a pointer to the real data in the output matrix */
    pg_graph = mxGetPr(plhs[0]);

    /* call the computational routine */
    create_graph(distanceMatrix,rows,cols,beta,pg_graph);
}
