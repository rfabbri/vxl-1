#ifndef LOGICAL_LINEAR_FUNCTIONS_H
#define LOGICAL_LINEAR_FUNCTIONS_H

//#ifdef _LINUX_
//#include <time.h>
//#endif

#include <vcl_cstdlib.h>
#include <vnl/vnl_math.h>

#include "EdgeDetection.h"
//#include "Utils.h"
#include "erfinv.h"

#define               EXP_MINUS_X_SQUARE_ARRAY_SIZE           65536

static  double *_expMinusXSquareArray;
static  double  _argStep;

class LogicalLinearBasisFunctions
  {
    private:
        double _sigma;
        double _gaussianConstant;
        double _invSqrt2Sigma;
        double _invSigmaSquare;
        double _gaussianConstantinvSigma2;
        double _gaussianConstantinvSigma4;
      
    public:
        LogicalLinearBasisFunctions(double sigma)
          {
            _sigma = sigma;
            computeConstants();
           }
        
        ~LogicalLinearBasisFunctions(){};

        double getSigma()
          {
            return _sigma;
           }
        
        void setSigma(double sigma)
          {
            _sigma = sigma;
            computeConstants();
           }

       

        void computeConstants()
          {
            assert(_sigma>0);

            _gaussianConstant            = (1.0/((vcl_sqrt(2*vnl_math::pi))*_sigma));
            _invSqrt2Sigma               = 1.0/(vcl_sqrt(2.0)*_sigma);
            _invSigmaSquare              = 1.0/(_sigma*_sigma);
            _gaussianConstantinvSigma2   = _gaussianConstant*(1.0/(2.0*_sigma*_sigma));
            _gaussianConstantinvSigma4   = _gaussianConstantinvSigma2*(1.0/(2.0*_sigma*_sigma));
           }
        
        /***
         * The Gaussian Family.
         * */
        
       inline double gaussian(double x) const
          {
            return (_gaussianConstant)*(expMinusXSquare(x*_invSqrt2Sigma));
           }


       inline double gaussianDeriv1(double x) const
          {
            return (-_gaussianConstantinvSigma2*x*expMinusXSquare(x*_invSqrt2Sigma));
           }


       inline double gaussianDeriv2(double x)const
          {
           //return  -((gaussian(x) + x*gaussianDeriv1(x))*(_invSigmaSquare));
            return _gaussianConstantinvSigma2*((x*x)*_invSigmaSquare-1)*expMinusXSquare(x*_invSqrt2Sigma);
           }
        

       inline double gaussianDeriv3(double x)const
          {
            //return  -((2*gaussianDeriv1(x) + x*gaussianDeriv2(x))*_invSigmaSquare);
            return _gaussianConstantinvSigma4*x*(2-(x*x*_invSigmaSquare))*expMinusXSquare(x*_invSqrt2Sigma);
           }

        
        inline double gaussianDeriv4(double x)const
          {
            //return -((3*gaussianDeriv2(x)+ x*gaussianDeriv3(x))*_invSigmaSquare);
            return _gaussianConstantinvSigma4*(2-5.0*x*x*_invSigmaSquare+ x*x*x*x*_invSigmaSquare*_invSigmaSquare)*expMinusXSquare(x*_invSqrt2Sigma);
           }


        static double expMinusXSquare(double x)
          {
            if (!_expMinusXSquareArray)
              {
                _expMinusXSquareArray = new double [EXP_MINUS_X_SQUARE_ARRAY_SIZE];
                
                /**
                 * exp(-x^2)     falls rapidly.
                 * exp(-10^2) =  3.7200759760208361e-44
                 * exp(-15^2) =  1.9219477278238491e-98
                 *
                 * Approximation:
                 * For any value x>=15, exp(-x^2) =0.
                 * */
                _argStep              = (double)(15.0/(EXP_MINUS_X_SQUARE_ARRAY_SIZE*1.0));
               
                for (int i=0;i<EXP_MINUS_X_SQUARE_ARRAY_SIZE;++i)
                  {
                    _expMinusXSquareArray[i] = vcl_exp(-((i)*_argStep)*((i)*_argStep));
                   }
               }
              
            if (x<0)
                x =-x;
            
            assert(_argStep>0);
            int index = int((x/_argStep)+0.5);
            
            
            assert(index>=0);
            
            if (index>=EXP_MINUS_X_SQUARE_ARRAY_SIZE)
                return 0;
            else
                return _expMinusXSquareArray[index];
            
           }
        
   };


//The Tags that indicate the type of image element.
template <int HalfCount>
class LLEdgeTag
  {
    private:
        LLEdgeTag(); //Do NOT Instantiate
   
    public:
        enum { HALF_MAX_TAN_COMPONENTS = HalfCount, 
              MAX_TAN_COMPONENTS       = (((int)HALF_MAX_TAN_COMPONENTS)<<1), 
              MAX_NORMAL_COMPONENTS    = 5, 
              NET_COMPONENTS           = MAX_TAN_COMPONENTS*MAX_NORMAL_COMPONENTS, 
              LEFT_LIMIT               = (HALF_MAX_TAN_COMPONENTS-1)*MAX_NORMAL_COMPONENTS,
              RIGHT_LIMIT              = (HALF_MAX_TAN_COMPONENTS+1)*MAX_NORMAL_COMPONENTS};

        inline static vcl_string getFunctionName() 
          {
            return "LogicalLinear-Edge";
           }

        inline static int getImageElementType()
          {
            return IMAGE_ELEMENT_EDGE;
           }

        inline static void normalComponents(const LogicalLinearBasisFunctions& normal_functions, double y, double epsilon, double *result) 
          {
            *result=  normal_functions.gaussianDeriv1(y); 
            ++result;

            *result= -normal_functions.gaussianDeriv2(y+epsilon); 
            ++result;

            *result=  normal_functions.gaussianDeriv2(y-epsilon);  
            ++result;

           
            *result=  normal_functions.gaussianDeriv4(y+epsilon);  
            ++result;
           
           
            *result= -normal_functions.gaussianDeriv4(y-epsilon);  
            //++result;
           }

   };


template <int HalfCount>
class LLPLineTag
  {
    private:
        LLPLineTag(); //Do NOT Instantiate

    public:
        enum { HALF_MAX_TAN_COMPONENTS=HalfCount, 
              MAX_TAN_COMPONENTS=(HalfCount<<1), 
              MAX_NORMAL_COMPONENTS=4, 
              NET_COMPONENTS           = MAX_TAN_COMPONENTS*MAX_NORMAL_COMPONENTS, 
              LEFT_LIMIT               = (HALF_MAX_TAN_COMPONENTS-1)*MAX_NORMAL_COMPONENTS,
              RIGHT_LIMIT              = (HALF_MAX_TAN_COMPONENTS+1)*MAX_NORMAL_COMPONENTS};
    
        static vcl_string getFunctionName() 
          {
            return "LogicalLinear-PLine";
           }


        inline static int getImageElementType()
          {
            return IMAGE_ELEMENT_PLINE;
           }
   
   
        inline static void normalComponents(const LogicalLinearBasisFunctions& normal_functions, double y, double epsilon, double *result) 
          {
            *result=  -normal_functions.gaussianDeriv1(y+epsilon); 
            ++result;

            *result=  normal_functions.gaussianDeriv1(y-epsilon); 
            ++result;

            *result=  normal_functions.gaussianDeriv3(y+epsilon);  
            ++result;

           
            *result=  -normal_functions.gaussianDeriv3(y-epsilon);  
           }
 
   };


template <int HalfCount>
class LLNLineTag
  {
    private:
        LLNLineTag(); //Do NOT Instantiate

    public:
        enum { HALF_MAX_TAN_COMPONENTS=HalfCount, 
              MAX_TAN_COMPONENTS=(HalfCount<<1), 
              MAX_NORMAL_COMPONENTS=4, 
              NET_COMPONENTS           = MAX_TAN_COMPONENTS*MAX_NORMAL_COMPONENTS, 
              LEFT_LIMIT               = (HALF_MAX_TAN_COMPONENTS-1)*MAX_NORMAL_COMPONENTS,
              RIGHT_LIMIT              = (HALF_MAX_TAN_COMPONENTS+1)*MAX_NORMAL_COMPONENTS};
        
        static vcl_string getFunctionName() 
          {
            return "LogicalLinear-NLine";
           }


        inline static int getImageElementType()
          {
            return IMAGE_ELEMENT_NLINE;
           }
   

        inline static void normalComponents(const LogicalLinearBasisFunctions& normal_functions, double y, double epsilon, double *result) 
          {
            *result=  normal_functions.gaussianDeriv1(y+epsilon); 
            ++result;

            *result=  -normal_functions.gaussianDeriv1(y-epsilon); 
            ++result;

            *result=  -normal_functions.gaussianDeriv3(y+epsilon);  
            ++result;

           
            *result=  normal_functions.gaussianDeriv3(y-epsilon);  
           }
 
   };




/*------------------------------------------------*/





class StepFunctionApprox
  {
    private:
        double _rho;
        
        
    public:
        StepFunctionApprox(double rho)
          {
            setRho(rho);
           }


        double getRho()
          {
            return _rho;
           }

        void setRho(double rho)
          {
            _rho    = rho;
           }


        inline double operator()(double x) const
          {
            if (x<-_rho)
                return 0;
            if (x>_rho)
                return 1;
            
            
            return (0.5+ (x/(2.0*_rho)));
           }

   };


class LLElemCombinator
  {
    private:
        double _rho;
        StepFunctionApprox _step;

    public:
        LLElemCombinator(double rho):_step(rho)
          {
            _rho = rho;
           };

        double getRho()
          {
            return _rho;
           }

        void setRho(double rho)
          {
            _rho = rho;
            _step.setRho(_rho);
           }
       
        inline double LLAnd(const double x, const double y) const
          {
            return (x*(1-_step(x)*_step(-y)) + y*(1-_step(y)*_step(-x)));
           };

        inline double LLOr(const double x, const double y) const
          {
            return (x*(1-_step(y)*_step(-x)) + y*(1-_step(x)*_step(-y)));
           };
        
   };


class SubPixelLLOptions :public SubPixelOptionsBase
  {
    private:  

        double   _epsilon;
        double   _maskCreationRho;
        double   _llCombinatorRho;
        double   _stablizerB;
        int      _qualityFactor;

    public:

        SubPixelLLOptions():SubPixelOptionsBase()
          {
            //
            //The Defaults
            //
//            _stepSize               = 1.0;
//            
//            _tangentialSigma        = 2.00*_stepSize;
//            _normalSigma            = 1.40*_stepSize;
//            _threshold              = 250;
//            _nDirs                  = 4;
//            _qualityFactor          = 1;
//

            
            _stepSize               = 1.0;
            
            _tangentialSigma        = 2.0*_stepSize;
            _normalSigma            = 1.4*_stepSize;
            _threshold              = 250;
            _nDirs                  = 4;
            _qualityFactor          = 1;
            
            _epsilon                = (_normalSigma/2.0)+0.3;
            _maskCreationRho        = _tangentialSigma+1;
            _llCombinatorRho        = _tangentialSigma+1;
            _stablizerB             = _maskCreationRho/2.0;

            computeFilterParams(true);
           }
      
        virtual ~SubPixelLLOptions(){};
        
        SubPixelLLOptions(const SubPixelLLOptions& old):SubPixelOptionsBase(old)
          {
            _epsilon                = old._epsilon;
            _maskCreationRho        = old._maskCreationRho; 
            _llCombinatorRho        = old._llCombinatorRho;
            _stablizerB             = old._stablizerB;
            _qualityFactor          = old._qualityFactor;
           }

        virtual SubPixelLLOptions& operator=(const SubPixelLLOptions &old)
          {
            if (this!=&old)
              {
                SubPixelOptionsBase::operator=(old); 
                _epsilon                = old._epsilon;
                _maskCreationRho        = old._maskCreationRho; 
                _llCombinatorRho        = old._llCombinatorRho;
                _stablizerB             = old._stablizerB;
                _qualityFactor          = old._qualityFactor;
               }
            return *this;
           }

        void computeFilterParams(bool set_all_params)
          {
            setFilterDimensions();
           
            if (set_all_params)
              {
                _epsilon                = (_normalSigma/2.0)+0.3;
                _maskCreationRho        = _tangentialSigma+1;
                _llCombinatorRho        = _tangentialSigma+1;
                _stablizerB             = _maskCreationRho/2.0;
                
                
                //            // HALF_MAX_TAN_COMPONENTS = 2
                //            //_epsilon                = _normalSigma/2.0;
                //           //_maskCreationRho        = _tangentialSigma/4.0;
                //           //_llCombinatorRho        = _tangentialSigma/2.0;
                //           //_stablizerB             =   _maskCreationRho/2.0;
                //            
                //            
                //            // HALF_MAX_TAN_COMPONENTS = 3
                //            _epsilon                = _normalSigma/2.0;
                //            _maskCreationRho        = _tangentialSigma/2.0;
                //            _llCombinatorRho        = _tangentialSigma;
                //            _stablizerB             =   _maskCreationRho/2.0;
               }
           }
            
            
        

        double getMaskCreationRho()   const
          {
            return _maskCreationRho;
           }

        double getLLCombinatorRho()   const
          {
            return _llCombinatorRho;
           }
        
        double getEpsilon()           const
          {
            return _epsilon;
           }

        double getStablizerB()        const
          {
            return _stablizerB;
           }

 
        int getQualityFactor()        const
          {
            return _qualityFactor;
           }
       

        double getPowellTolerence()   const
          {
            assert(_qualityFactor>0);  
            switch(_qualityFactor)
              {
                case(1):
                    return 0.01;
                case(2):
                    return 0.01;
                case(3):
                    return 0.01;
                case(4):
                    return 0.01;
                case(5):
                    return 0.01;
                case(6):
                    return 0.005;
                case(7):
                    return 0.005;
                case(8):
                    return 0.005;
                case(9):
                    return 0.005;

                case(10):
                    return 0.001;

                default:
                    return 0.001;
               }
           }

        double getPowellTinyValue() const
          {
            assert(_qualityFactor>0);  
            switch(_qualityFactor)
              {
                case(1):
                    return 1.0e-6;
                case(2):
                    return 1.0e-6;
                case(3):
                    return 1.0e-6;
                case(4):
                    return 1.0e-6;
                case(5):
                    return 1.0e-6;
                case(6):
                    return 1.0e-7;
                case(7):
                    return 1.0e-8;
                case(8):
                    return 1.0e-9;
                case(9):
                    return 1.0e-10;

                case(10):
                    return 1.0e-10;

                default:
                    return 1.0e-11;
               }
           }
        
        int getPowellItMax()          const
          {
            assert(_qualityFactor>0);  
            switch(_qualityFactor)
              {
                case(1):
                    return 1;
                case(2):
                    return 2;
                case(3):
                    return 2;
                case(4):
                    return 3;
                case(5):
                    return 4;
                case(6):
                    return 7;
                case(7):
                    return 10;
                case(8):
                    return 20;
                case(9):
                    return 50;

                case(10):
                    return 100;

                default:
                    return 200;
               }
           }
       

        int getBrentItMax()           const
          {
            assert(_qualityFactor>0);  
            switch(_qualityFactor)
              {
                case(1):
                    return 10;
                case(2):
                    return 10;
                case(3):
                    return 20;
                case(4):
                    return 30;
                case(5):
                    return 40;
                case(6):
                    return 70;
                case(7):
                    return 100;
                case(9):
                    return 150;
                case(10):
                    return 200;
                    
                default:
                    return 300;
               }

           }


   



        
        void setQualityFactor(int val)
          {
            _qualityFactor = val;
           }
        
        void setEpsilon(double eps)
          {
            _epsilon = eps;
           }
 
        void setMaskCreationRho(double val)
          {
            _maskCreationRho= val;
           }


        void setLLCombinatorRho(double val)
          {
            _llCombinatorRho = val;
           }

        void setStablizerB(double val)
          {
            _stablizerB = val;
           }

 

        virtual vcl_string printOptions(vcl_string str="") const
          {
            vcl_string base_options=SubPixelOptionsBase::printOptions(str);

            vcl_ostringstream ostrm;
           
            ostrm<<vcl_endl;
            ostrm<<str<<" Logical-Linear              : "<<vcl_endl;
            ostrm<<str<<" \tEpsilon                   : "<<_epsilon<<vcl_endl;
            ostrm<<str<<" \tMask  Rho                 : "<<_maskCreationRho<<vcl_endl;
            ostrm<<str<<" \tLLCombinator Rho          : "<<_llCombinatorRho<<vcl_endl;
            ostrm<<str<<" \tStabilizerB               : "<<_stablizerB<<vcl_endl;
            ostrm<<str<<" \tQuality Factor            : "<<_qualityFactor<<vcl_endl;
            ostrm<<str<<" \tPowell Tol                : "<<getPowellTolerence()<<vcl_endl;
            ostrm<<str<<" \tPowell Tiny Value         : "<<getPowellTinyValue()<<vcl_endl;
            ostrm<<str<<" \tPowell It Max             : "<<getPowellItMax()<<vcl_endl;
            ostrm<<str<<" \tBrent  It Max             : "<<getBrentItMax()<<vcl_endl;
/*#ifdef _LINUX_
            time_t current_date_and_time;
            time(&current_date_and_time);
            ostrm<<str<<" \tDate Created              : "<<asctime(localtime(&current_date_and_time)); 
#else 
            ostrm<<str<<" \tDate Created              : Produced on Micro$oft !@#$%^&"<<vcl_endl;
#endif
*/
            ostrm<<str<<" \tDate Created              : yet to be implemented in VXL-supported code" << vcl_endl;
            return base_options+ostrm.str();
           }
  


        template <typename CommandLineParserType>
                void registerOptions(CommandLineParserType &parser) 
                  {
                    vcl_ostringstream ostrm;

                    ostrm<<"("<<_tangentialSigma<<") ";
                    parser.register_option(_tangentialSigma,     "tan-sigma",      '\0',   "The Value of Tangential Sigma",            ostrm.str().c_str());
                    ostrm.str("");ostrm.clear();
                    
                    ostrm<<"("<<_normalSigma<<") ";
                    parser.register_option(_normalSigma,         "normal-sigma",   '\0',   "The Value of Normal Sigma",                ostrm.str().c_str());
                    ostrm.str("");ostrm.clear();
                    
                    ostrm<<"("<<_threshold<<") ";
                    parser.register_option(_threshold,           "threshold",      '\0',   "The Threshold",                            ostrm.str().c_str());
                    ostrm.str("");ostrm.clear();
                    
                    ostrm<<"("<<_nDirs<<") ";
                    parser.register_option(_nDirs,               "half-ndirs",          '\0',   "HALF the Number of Pixel Direction",       ostrm.str().c_str());
                    ostrm.str("");ostrm.clear();
                    
                    ostrm<<"("<<_qualityFactor<<") ";
                    parser.register_option(_qualityFactor,       "quality",        '\0',   "Quality of Sub-Pixelization",              ostrm.str().c_str());
                    ostrm.str("");ostrm.clear();
                   
                    ostrm<<"("<<_epsilon<<") ";
                    parser.register_option(_epsilon,       "epsilon",        '\0',   "Epsilon ",              ostrm.str().c_str());
                    ostrm.str("");ostrm.clear();

                    ostrm<<"("<<_maskCreationRho<<") ";
                    parser.register_option(_maskCreationRho,       "mask-creation-rho",        '\0',   "Mask Creation Rho ",              ostrm.str().c_str());
                    ostrm.str("");ostrm.clear();

                    ostrm<<"("<<_llCombinatorRho<<") ";
                    parser.register_option(_llCombinatorRho,       "llcombinator-rho",        '\0',   "LL Combinator Rho ",              ostrm.str().c_str());
                    ostrm.str("");ostrm.clear();
                  
                    ostrm<<"("<<_stablizerB<<") ";
                    parser.register_option(_stablizerB,       "stab",        '\0',   "Stablizer Factor ",              ostrm.str().c_str());
                    ostrm.str("");ostrm.clear();
                   }
   };



//
//TagType == LLEdgeTag  ||  LLPLineTag || LLNLineTag
//
template<typename TagType>
class LLFunction
  {
    private:
        SubPixelLLOptions             _options;
        LogicalLinearBasisFunctions   _tanFunctions;
        LogicalLinearBasisFunctions   _normalFunctions;
        StepFunctionApprox            _stepFunction;

        double                        _tanOffset;
        double                        _normalOffset;
        double                        _theta;
        double                        _cosTheta;
        double                        _sinTheta;
                
        double                        _epsilon;
        double                        _stablizerB;
        double                        *_partitionPoints;

        
    public:
        
        typedef TagType              LogicalLinearTagType; 
        typedef SubPixelLLOptions    OptionsType;
 
        
        LLFunction(SubPixelLLOptions &ops, double theta, double tan_offset, double normal_offset):
                _tanFunctions(ops.getTangentialSigma()), _normalFunctions(ops.getNormalSigma()), _stepFunction(ops.getMaskCreationRho()),
                _epsilon(ops.getEpsilon()), _stablizerB(ops.getStablizerB()), _partitionPoints(0)
          {
            //COMPILE_TIME_ENFORCE(LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS>=1);
          if( _partitionPoints != NULL ) {
            delete _partitionPoints;
          }
      _partitionPoints = new double[ TagType::MAX_TAN_COMPONENTS-1 ];

            _options      = ops;

            _theta        = theta;
            _cosTheta     = vcl_cos(_theta);
            _sinTheta     = vcl_sin(_theta);


            _tanOffset    = tan_offset;
            _normalOffset = normal_offset;

            double tan_sigma   = ops.getTangentialSigma();

            for(int i=0;i<LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS-1;++i)
              {
                _partitionPoints[i]=-tan_sigma*vcl_sqrt(2.0)*erfinv((2.0*(LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS-1-i))/(LogicalLinearTagType::MAX_TAN_COMPONENTS));
               }
            _partitionPoints[LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS-1]=0;

            for(int i=0;i<LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS-1;++i)
              {
                _partitionPoints[LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS+i]=-_partitionPoints[LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS-2-i];
               }


//            for(int i=0;i<LogicalLinearTagType::MAX_TAN_COMPONENTS-1;++i)
//              {
//                cout<<"i ="<<i<<" Partition Point ="<<_partitionPoints[i]<<endl;
//               }
//
           }


        LLFunction(const LLFunction<TagType>  &old):_options(old._options),_tanFunctions(old._tanFunctions), _normalFunctions(old._normalFunctions),_stepFunction(old._stepFunction),
                                                            _epsilon(old._options.getEpsilon()), _stablizerB(old._options.getStablizerB()), _partitionPoints( 0 )
          {
          if( _partitionPoints != NULL ) {
            delete _partitionPoints;
          }
      _partitionPoints = new double[ TagType::MAX_TAN_COMPONENTS-1 ];

            _tanOffset       = old._tanOffset;
            _normalOffset    = old._normalOffset;
            _theta           = old._theta;
            _cosTheta        = old._cosTheta;
            _sinTheta        = old._sinTheta;

            for(int i=0;i<TagType::MAX_TAN_COMPONENTS-1;++i)
              {
                _partitionPoints[i]=old._partitionPoints[i];
               }

           }

       LLFunction<TagType>& operator=(const LLFunction<TagType> &old)
         {
           if (this!=&old)
             {
          if( _partitionPoints != NULL ) {
            delete _partitionPoints;
          }
      _partitionPoints = new double[ TagType::MAX_TAN_COMPONENTS-1 ];

               _options          = old._options;
               _tanFunctions     = old._tanFunctions;
               _normalFunctions  = old._normalFunctions;
               _stepFunction     = old._stepFunction;

               _tanOffset        = old._tanOffset;
               _normalOffset     = old._normalOffset;
               _theta            = old._theta;
               _cosTheta         = old._cosTheta;
               _sinTheta         = old._sinTheta;

               _epsilon          = old._options.getEpsilon();
               _stablizerB       = old._options.getStablizerB();

               for(int i=0;i<TagType::MAX_TAN_COMPONENTS-1;++i)
                 {
                   _partitionPoints[i]=old._partitionPoints[i];
                  }


              }
           return *this;
          }


       ~LLFunction()
         {
         if( _partitionPoints != NULL ) {
            delete _partitionPoints;
          }
          };

   
        void setTanOffset(double tan_offset)
          {
            _tanOffset = tan_offset;
           }

        void setNormalOffset(double normal_offset)
          {
            _normalOffset = normal_offset;
           }
   
 
        void setTheta(double theta)
          {
            _theta        = theta;
            _cosTheta     = cos(_theta);
            _sinTheta     = sin(_theta);

           }

        double getTheta() const
          {
            return _theta;
           }
       

   

        /**
         * NOTE:  X == Tangential.
         *        Y == Normal.
         *        
         **********/

        
        
        /**
         * The Tangential components that are common to all masks (Edge, Postivate line &Negative Line).
         * Numbering is from left to right (i.e., from - to +).
         * Leftmost partition  = 0
         * Rightmost partition = n-1
         * The points are numbered from 0 to n-2
         * ****/

        inline void tangentialComponents(double x, double *result) const
          {
            //COMPILE_TIME_ENFORCE((LogicalLinearTagType::MAX_TAN_COMPONENTS>2));
            double *temp=result;
            
            double gaussian_x = _tanFunctions.gaussian(x);

            //
            //Leftmost Partition.
            //
            
            *result = gaussian_x*(_stepFunction(_partitionPoints[0]-x));
            ++result;

            for (int i=1;i<((int)LogicalLinearTagType::MAX_TAN_COMPONENTS-1);++i)
              {
               *result = gaussian_x*(_stepFunction(_partitionPoints[i]-x)+_stepFunction(x-_partitionPoints[i-1])-1);
               ++result;
               }
          
            //
            //Rightmost Partition.
            //
          
            *result = gaussian_x*_stepFunction(x-_partitionPoints[(int)LogicalLinearTagType::MAX_TAN_COMPONENTS-2]);
            ++result;



            temp[LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS-1] += _stablizerB*_tanFunctions.gaussianDeriv1(x); 
            temp[LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS  ] -= _stablizerB*_tanFunctions.gaussianDeriv1(x); 
            
            return;
           }


//        double tangentialMinus(double x) const
//          {
//            return (_tanFunctions.gaussian(x)*_stepFunction(-x) + _stablizerB*_tanFunctions.gaussianDeriv1(x));
//           }
//
//        double tangentialPlus(double x) const
//          {
//            return (_tanFunctions.gaussian(x)*_stepFunction(x) -_stablizerB*_tanFunctions.gaussianDeriv1(x));
//           }
//
    
        void setParams(double theta, double tan_offset, double norm_offset)
          {
            _theta         = theta;
            _cosTheta      = vcl_cos(_theta);
            _sinTheta      = vcl_sin(_theta);
            
            _tanOffset     = tan_offset;
            _normalOffset  = norm_offset;
           }

 
        /**
         * The Functions used by the specializations.
         * */

      
        int      getMaxNormalComponents() const
          {
            return LogicalLinearTagType::MAX_NORMAL_COMPONENTS;
           }

        int      getHalfMaxTangentialComponents() const
          {
            return LogicalLinearTagType::HALF_MAX_TAN_COMPONENTS;
           }

        int getNetMaxComponents() const
          {
            return (((int)LogicalLinearTagType::MAX_NORMAL_COMPONENTS)*((int)LogicalLinearTagType::MAX_TAN_COMPONENTS));
           }
        
        vcl_string   getFunctionName() const
          {
            return LogicalLinearTagType::getFunctionName();
           };
        
        inline void normalComponents(double y, double *result) const
          {
            LogicalLinearTagType::normalComponents(_normalFunctions, y, _epsilon, result);
            return;
           }
       

        
        inline void rotateAndShift(const double i, const double j, double &x, double &y) const
          {

            x  = ( i*_cosTheta)+(j*_sinTheta)-_tanOffset;
            y  = (-i*_sinTheta)+(j*_cosTheta)-_normalOffset;

            return;
           }
   };





/**
 * LL Edges
 * */

/*
template<>
void LLFunction<LLEdgeTag<1> >::tangentialComponents(double x, double * result) const
  {
    *result=(_tanFunctions.gaussian(x)*_stepFunction(-x) + _stablizerB*_tanFunctions.gaussianDeriv1(x));
    ++result;

    *result=(_tanFunctions.gaussian(x)*_stepFunction(x) -_stablizerB*_tanFunctions.gaussianDeriv1(x));
    
    return;
   };


template<>
void LLFunction<LLPLineTag<1> >::tangentialComponents(double x, double * result) const
  {
    *result=(_tanFunctions.gaussian(x)*_stepFunction(-x) + _stablizerB*_tanFunctions.gaussianDeriv1(x));
    ++result;

    *result=(_tanFunctions.gaussian(x)*_stepFunction(x) -_stablizerB*_tanFunctions.gaussianDeriv1(x));
    
    return;
   };

template<>
void LLFunction<LLNLineTag<1> >::tangentialComponents(double x, double * result) const
  {
    *result=(_tanFunctions.gaussian(x)*_stepFunction(-x) + _stablizerB*_tanFunctions.gaussianDeriv1(x));
    ++result;

    *result=(_tanFunctions.gaussian(x)*_stepFunction(x) -_stablizerB*_tanFunctions.gaussianDeriv1(x));
    
    return;
   };
*/

/* 
 * Utility Class for Functions.  
 * The is needed because C++ does not have templated typedefs.  
 * */

template<int HalfCount>
struct LLEdgeFunction
  {
    typedef  LLFunction<LLEdgeTag<HalfCount> >  type;
   };

template<int HalfCount>
struct LLPLineFunction
  {
    typedef  LLFunction<LLPLineTag<HalfCount> >  type;
   };

template<int HalfCount>
struct LLNLineFunction
  {
    typedef  LLFunction<LLNLineTag<HalfCount> >  type;
   };






#endif         /* LOGICAL_LINEAR_FUNCTIONS_H */
