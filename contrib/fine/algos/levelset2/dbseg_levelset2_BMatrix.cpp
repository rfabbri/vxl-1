/*
File: BMatrix.cpp

  
    
*/

#include "dbseg_levelset2_BMatrix.h"

BMatrix::BMatrix() {

    norm = -1;
    rowMaj = true;
    nrows = -1;
    ncols = -1;
    m = NULL;
    
    


}


BMatrix::BMatrix(int r, int c){
    
    nrows = r;
    ncols = c;
    norm = -1;
    m = vnl_matrix<double>(r, c);
    //m = new double[nrows * ncols];
    rowMaj = true;
    

    
    
}


BMatrix::BMatrix(const BMatrix& rhs) {
    
    nrows = rhs.getNumRows();
    ncols = rhs.getNumCols();
    
    m=vnl_matrix<double>(nrows, ncols);
    //m = new double[nrows * ncols];
    for(int i=0; i<nrows; i++) {
        for(int j=0; j<ncols; j++) {
            setVal(i,j,rhs.getVal(i,j));
        }
    }
    

    
}


void BMatrix::init(int r, int c) {
    
    nrows = r;
    ncols = c;
    m = vnl_matrix<double>(r, c);
    //m = new double[nrows * ncols];

}


BMatrix::~BMatrix() {
    /*
    if(m != NULL) {
        //delete[] m;
        m = NULL;
    }*/
    

    
}

void BMatrix::kill() {

    //delete[] m;
    m = NULL;

    
}        


void BMatrix::setVal(int r, int c, double val) {
    
    
    m(r, c) = val;
    //m[r*ncols + c] = val;
}


double BMatrix::getVal(int r, int c) const {
    
    double rVal = -1;
    
    if(r < 0)
        return 0;
    
    if(r >= nrows)
        return 0;
    
    if(c < 0)
        return 0;
    
    if(c >= ncols)
        return 0;
    
    rVal = m(r,c);
    //rVal = m[r*ncols + c];
    
    return rVal;
}



int BMatrix::getNumRows() const {
    
    return nrows;
}

int BMatrix::getNumCols() const {
    
    return ncols;
}

void BMatrix::print() {
    /*
    double val;
    printf("\n");
    for(int i=0; i<nrows; i++) {
        for(int j=0; j<ncols; j++) {
            val =  getVal(i,j);
            
            printf("%10.5f",val);
        }
        printf("\n");
    }
    printf("   \n");
    */
}


BMatrix* BMatrix::copy() {
    
    BMatrix *dest = new BMatrix(nrows,ncols);
    
    for(int i=0; i<nrows; i++) {
        for(int j=0; j<ncols; j++) {
            dest->setVal(i,j,getVal(i,j));
        }
    }
    
    return dest;
    
}


BMatrix* BMatrix::operator +(BMatrix &m) { 
    
    double v1, v2;
    BMatrix *rVal = new BMatrix(m.getNumRows(), m.getNumCols());
    for(int i=0; i<nrows; i++) {
        for(int j=0; j<ncols; j++) {
            
            v1 = getVal(i,j);
            v2 = m.getVal(i,j);
            
            rVal->setVal(i,j,v1+v2);
            
        }
    }
    
    return rVal;
}


BMatrix* BMatrix::operator -(BMatrix &m) { 
    
    double v1, v2;
    BMatrix *rVal = new BMatrix(m.getNumRows(), m.getNumCols());    
    for(int i=0; i<nrows; i++) {
        for(int j=0; j<ncols; j++) {
            
            v1 = getVal(i,j);
            v2 = m.getVal(i,j);
            
            rVal->setVal(i,j,v1-v2);
            
        }
    }
    
    return rVal;
}


BMatrix* BMatrix::operator*(BMatrix &mat) {
    double sum;
    BMatrix *rMat = new BMatrix(nrows, mat.ncols);
    
    for(int i=0; i<nrows; i++){
        for(int j=0; j<mat.getNumCols(); j++) {
            sum = 0;
            for(int k=0; k<ncols; k++) {
                sum += getVal(i,k) * mat.getVal(k,j);
            }
            rMat->setVal(i,j,sum);
        }
    }
    
    //this->print();
    //mat.print();
    //rMat->print();
    
    return rMat;
    
}


void BMatrix::operator *(double val) {
    
    for(int i=0; i<nrows; i++){
        for(int j=0; j<ncols; j++) {
            setVal(i,j,getVal(i,j)*val);
        }
    }
        
}

void BMatrix::operator =(BMatrix& rhs) { 
    
    double val;
    
    //if(m != NULL)
        //delete[] m;
    
    nrows = rhs.getNumRows();
    ncols = rhs.getNumCols();
    
    m = vnl_matrix<double>(nrows, ncols);
    //m = new double[nrows*ncols]; 
    
    
    for(int i=0; i<nrows; i++) {
        for(int j=0; j<ncols; j++) {
            
            val = rhs.getVal(i,j);
            
            setVal(i,j,val);
            
        }
    }
    
    
}



double BMatrix::convolve(const BMatrix &iMat) const {
    

    double aVal, iVal, sum=0, mult=0;
    
    // convolve
    int k = getNumCols()-1;
    for(int j=0; j<iMat.getNumCols(); j++) {
                
        aVal = getVal(0,k);
        //aVal = getVal(0,j);
        iVal = iMat.getVal(0,j);
                
        mult = aVal*iVal;
        sum += mult;
        k--;
                
    }
            
    return sum;
    
}



void BMatrix::transpose() {
    
    /*
    double val;
    int i,j;
    gsl_matrix *src =  gsl_matrix_calloc (getNumRows(), getNumCols());
    gsl_matrix *dest =  gsl_matrix_calloc (getNumCols(), getNumRows());

    
    // copy to src
    for(i=0; i<getNumRows(); i++) {
        for(j=0; j<getNumCols(); j++) {
            val = getVal(i,j);
            gsl_matrix_set (src, i, j, val);    
        }
    }
    
    gsl_matrix_transpose_memcpy(dest, src);
    
    free(m);
    m = (double*)calloc(dest->size1 * dest->size2, sizeof(double));
    nrows = dest->size1;
    ncols = dest->size2;
    
    //copy from dest
    for (i=0; i<dest->size1; i++) {
        for(j=0;j<dest->size2; j++) {
            val = gsl_matrix_get (dest,i, j);
            setVal(i,j,val);
        }
        
    }
    
    
    gsl_matrix_free (src);    
    gsl_matrix_free (dest);    
    */
    m.transpose();
    nrows=m.rows();
    ncols = m.cols();
    
}


void BMatrix::multTest() {
    
    BMatrix a = BMatrix(1,3);
    a.setVal(0,0,1);
    a.setVal(0,1,2);
    a.setVal(0,2,3);
    a.print();
    
    BMatrix b = BMatrix(1,3);
    b.setVal(0,0,2);
    b.setVal(0,1,3);
    b.setVal(0,2,4);
    
    b.print();
    
    b.transpose();
    
    b.print();
    
    
    BMatrixPtr r = a * b;
    r->print();
    
    b.transpose();
    
    double cr = a.convolve(b);
    cout << "Convolve: " << (int)cr << endl;
    
    
}



void BMatrix::CalcNorm() {
    
    double val = 0, sum=0;
    for(int r=0; r < getNumRows(); r++) {            
        for(int c=0; c < getNumCols(); c++) {
            val = getVal(r,c);
            sum += val;
        }
        
    }    
    
    
    norm = sum;
    
}


void BMatrix::IntKer() {
    
    float gmin = getVal(0,0);
    float nf = 1.0/gmin;
    double val;
    int nval;
    for(int r=0; r < getNumRows(); r++) {            
        for(int c=0; c < getNumCols(); c++) {
            val = getVal(r,c);
            nval = (int)nf*val;
            setVal(r,c,val);
        }
        
    }    
    

    
    
}

void BMatrix::Normalize() {
    
    double val;
    CalcNorm();
    for(int r=0; r < getNumRows(); r++) {            
        for(int c=0; c < getNumCols(); c++) {
            val = getVal(r,c);
            val /= norm;
            setVal(r,c,val);
        }
        
    }    


}

double BMatrix::round(double val) {
    
    double fd, cd;
    
    fd = fabs (val - floor(val));
    cd = fabs (val - ceil(val));
    
    if (fd < cd)
        return floor(val);
    else
        return ceil(val);

}

BMatrix* BMatrix::SubMatrix(int sr, int sc, int size) {
    
    BMatrix *rmat = new BMatrix(size,size);
    int i=sr;
    int j=sc;
    
    for(int r=0; r<size; r++) {
        for(int c=0; c<size; c++) {
        
            rmat->setVal(r,c,getVal(i,j));
            j++;
            
        }
        i++;
        j=sc;
    }
    
    return rmat;
    
}


void BMatrix::writeplot(char *path) {
    /*
    
    ofstream file (path);
    
    for(int i=0; i<nrows*ncols; i++)
        file << m[i] << endl;
    
    
    file.close();
    */
    
}


void BMatrix::plot(char *str) {
    
    char b[100];
    gnuplot_ctrl *gp = gnuplot_init();
    
    
    gnuplot_setstyle(gp, "boxes") ;
    
    sprintf(b,"Bins");
    gnuplot_set_xlabel(gp,b);
    
    sprintf(b,"Count");
    gnuplot_set_ylabel(gp,b);
    
    double* m2 = new double[nrows*ncols];
    for (int i = 0; i < 
    
    gnuplot_plot_x(gp, m, nrows*ncols, str);
    //sleep(1);
    
    gnuplot_hardcopy_colour(gp,str);

       gnuplot_close(gp);
    //sleep(1) ;
    
}


void BMatrix::graph() {
    
    char b[100];
    gnuplot_ctrl* gp = gnuplot_init();
    
    
    gnuplot_setstyle(gp, "boxes") ;
    
    sprintf(b,"Bins");
    gnuplot_set_xlabel(gp,b);
    
    sprintf(b,"Count");
    gnuplot_set_ylabel(gp,b);
    
    
    gnuplot_plot_x(gp, m, nrows*ncols, "GRAPH");
    //gnuplot_hardcopy_colour(gp,str);
    
    //sleep(3) ;
    
       gnuplot_close(gp);

    
    
    
}



void BMatrix::splot(char *str) {
    
    char b[100];
    
    gnuplot_ctrl *gp = gnuplot_init();
    
    gnuplot_setstyle(gp, "lines") ;
    
    sprintf(b,"X Axis");
    gnuplot_set_xlabel(gp,b);
    
    sprintf(b,"Y Axis");
    gnuplot_set_ylabel(gp,b);
    
    sprintf(b,"Z Axis");
    gnuplot_set_zlabel(gp,b);
    
    gnuplot_cmd(gp,"set mouse");
    gnuplot_cmd(gp,"set key right");
    gnuplot_cmd(gp,"set view 40,30");
    
    gnuplot_cmd(gp,"set pm3d");
    gnuplot_cmd(gp,"set palette gray");
    gnuplot_cmd(gp,"set grid");
    gnuplot_cmd(gp,"set contour");
    gnuplot_cmd(gp,"set term x11");
    

    
    gnuplot_splot_grid(gp, m, nrows, ncols, str);
    
    printf("Type 'd' to continue\n");
    scanf("%s",b);
    //sleep(1);

    printf("Writing Surface Plot\n");
    
    //gnuplot_cmd(gp,"set hidden3d");
    gnuplot_hardcopy_colour(gp,str);
    
    gnuplot_close(gp);
    
    
    //sleep(1) ;
    
}














