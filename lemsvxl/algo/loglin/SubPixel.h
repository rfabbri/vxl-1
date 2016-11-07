#ifndef SUB_PIXEL_H
#define SUB_PIXEL_H

#define STEP_SIZE                      1.0
#define TANGENTIAL_SIGMA               3.0*STEP_SIZE
#define NORMAL_SIGMA                   (TANGENTIAL_SIGMA/3.0)
#define FILTER_SIZE_FACTOR             4.0 //3 //2.5

#define MAX_DIRS                        4
#define THRESHOLD                       200

#include <string>
#include <sstream>
#include <vcl_cmath.h>

class SubPixelOptionsBase
  {
    protected:
        double _stepSize;
        double _tangentialSigma;
        double _normalSigma;

        
        double _filterRows;
        double _filterCols;
        double _filterSizeFactor;          //Needed for computing filter size.

        
        int    _nDirs;                     //Needed for pixel LL.
        double _threshold;                   
        
    public:
        SubPixelOptionsBase()
          {
           _stepSize               = STEP_SIZE;            
           _tangentialSigma        = TANGENTIAL_SIGMA;          
           _normalSigma            = NORMAL_SIGMA;
           _filterSizeFactor       = FILTER_SIZE_FACTOR;
           
           setFilterDimensions();
           
           _nDirs                  = MAX_DIRS;
           _threshold              = THRESHOLD;
           }

        virtual ~SubPixelOptionsBase(){};
        
        SubPixelOptionsBase(const SubPixelOptionsBase &old)
          {
            assign(old);
           }
       

        virtual SubPixelOptionsBase& operator=(const SubPixelOptionsBase &old)
          {
            if (this!=&old)
              {
                assign(old);
               }
            return *this;
           }

        void assign(const SubPixelOptionsBase &old)
          {
            _stepSize               = old._stepSize;               
            _tangentialSigma        = old._tangentialSigma; 
            _normalSigma            = old._normalSigma;             
            _filterRows             = old._filterRows;             
            _filterCols             = old._filterCols;             
            _filterSizeFactor       = old._filterSizeFactor;
            
            _nDirs                  = old._nDirs;
            _threshold              = old._threshold;
           }

        
        
        void setFilterDimensions()
          {
            _filterRows              = 2*vcl_ceil((_filterSizeFactor*(vnl_math_max(_tangentialSigma, _normalSigma))))+1;
            _filterCols              = _filterRows;
           }

        
        void setTangentialSigma(double gauss_sigma)
          {
            _tangentialSigma = gauss_sigma;
            setFilterDimensions();
           }
               
        void setNormalSigma(double deriv_sigma)
          {
            _normalSigma     = deriv_sigma;   
            setFilterDimensions();
           }


        void setNDirs(int dirs)
          {
            _nDirs = dirs;
           }


        void setThreshold(double val)
          {
            _threshold = val;
           }
        
        
        void setStepSize(double val)
          {
            _stepSize = val;
           }

        
        void setFilterSizeFactor(double val)
          {
            _filterSizeFactor = val;
           }
       


        double getStepSize()        const
          {
            return _stepSize;
           }
        
        double getTangentialSigma() const
          {
            return _tangentialSigma;
           }
        
        double getNormalSigma()     const
          {
            return _normalSigma;
           }
 
       
        int getFilterRows()      const
          {
            return (int)_filterRows;
           }
       
        int getFilterCols()      const
          {
            return (int)_filterCols;
           }
       

        
        
        

   
        int getNDirs()   const
          {
            return _nDirs;
           }

   
        double getThreshold() const
          {
            return _threshold;
           }
  

        double getFilterSizeFactor() const
          {
            return _filterSizeFactor;
           }
               
        

        
        
        virtual vcl_string printOptions(vcl_string str="") const
          {
            vcl_ostringstream ostrm;
            ostrm<<str<<" Base-Options :"<<vcl_endl;
            ostrm<<str<<" \tStep-Size                          : "<<_stepSize<<vcl_endl;
            ostrm<<str<<" \tTangential-Sigma                   : "<<_tangentialSigma<<vcl_endl;
            ostrm<<str<<" \tNormal-Sigma                       : "<<_normalSigma<<vcl_endl;
            ostrm<<str<<" \tFilter-Rows                        : "<<_filterRows<<vcl_endl;
            ostrm<<str<<" \tFilter-Cols                        : "<<_filterCols<<vcl_endl;
            ostrm<<str<<" \tFilter-Size-Factor                 : "<<_filterSizeFactor<<vcl_endl;
            ostrm<<str<<" \tPixel Directions (Half)            : "<<_nDirs<<vcl_endl;
            ostrm<<str<<" \tThreshold                          : "<<_threshold;
           

            return ostrm.str();
           }
        
   };







#endif   /* SUB_PIXEL_H */
