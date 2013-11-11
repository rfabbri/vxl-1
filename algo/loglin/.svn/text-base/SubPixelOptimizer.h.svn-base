#ifndef  SUBPIXEL_OPTIMIZER_H
#define  SUBPIXEL_OPTIMIZER_H
#include "points.h"
#include "PowellsAlgorithm.h"

#include <vcl_vector.h>
#include <vcl_iostream.h>

template<typename T1, typename T2>
class SubPixelEnergyFunction;

class SubPixelPoint
  {
    private:

        friend class SubPixelEnergyFunction<class T1, class T2>;

        const int     _numDimensions;

        /**
         * values[0]  ----> Direction.
         * values[1]  ----> Position.
         * */
        double              _values[2];
        
        int           _rowPos;
        int           _colPos;
        double        _pixelDir;
        
    public:
        SubPixelPoint(int row, int col, double pixel_direction):_numDimensions(2)
          {
            _values[0] = 0;
            _values[1] = 0;
            reset(row, col, pixel_direction);
           }


        SubPixelPoint(const SubPixelPoint &old):_numDimensions(2),_rowPos(old._rowPos),_colPos(old._colPos),_pixelDir(old._pixelDir) 
          {
            assert(_numDimensions==old._numDimensions);
            
            _values[0]=old._values[0];
            _values[1]=old._values[1];
           }
       
        
        SubPixelPoint& operator=(const SubPixelPoint &old)
          {
            if (this!=&old)
              {
                assert(_numDimensions == old._numDimensions);

                _pixelDir  = old._pixelDir;
                _rowPos    = old._rowPos;
                _colPos    = old._colPos;
                _values[0] = old._values[0];
                _values[1] = old._values[1];
               }
            return *this;
           }
        
        
        ~SubPixelPoint()
         {
          }

       
        void reset(int row, int col, double pixel_direction)
          {
            _values[0] = 0;
            _values[1] = 0;
            
            _rowPos    = row;
            _colPos    = col;
            _pixelDir  = pixel_direction;
           }
        
    
        /**
         * 0 --> Direction.
         * 1 --> Offset (Normal).
         * */

        double& operator[](int i)
          {
            return _values[i];
           }
  

        const double& operator[](int i) const
          {
            return _values[i];
           }
        
        
        int getNumDimensions() const
          {
            return _numDimensions;
           }
   

        int getRowPos()  const
          {
            return _rowPos;
           }
        
        
        int getColPos() const
          {
            return _colPos;
           }

        
        Point2D<double> getSubPixelPoint() const
          {
            Point2D<double> pt;
            
            pt.setY((_values[1]));
            pt.setX(0);

            pt.rotate(-(_pixelDir+_values[0]));
            pt.setX(pt.getX()+_colPos);
            pt.setY(pt.getY()+_rowPos);

            return pt;
           }
   
        
        double getSubPixelDirection() const
          {
            return _pixelDir+_values[0];
           }
        
        
        double getPixelDirection() const
          {
            return _pixelDir;
           }

        
        vcl_string printValues() const
          {
            vcl_ostringstream ostrm;
            ostrm<<" (";
            for (int j=0;j<_numDimensions;j++)
              {
                ostrm<<vcl_setprecision(5)<<vcl_setw(7)<<_values[j]<<"  ";
               }
            ostrm<<" )";
            ostrm<<vcl_endl;
            return ostrm.str();
           }
  

        template<typename DirectionVectorType>
                void getDifferenceVector(DirectionVectorType& dir_vec, const SubPixelPoint &other_pt)
                  {
                    assert((int)dir_vec.size()==_numDimensions);
                    for (int i=0;i<_numDimensions;i++)
                      {
                        dir_vec[i]=_values[i]-other_pt[i];
                       }
                   }
   };


template<typename PointType, typename ConvProductSumComputerType>
class SubPixelEnergyFunction
  {
    
    private:
        typedef typename ConvProductSumComputerType::FilterType      FilterType;
        typedef typename ConvProductSumComputerType::ImageType       ImageType;
        typedef typename ConvProductSumComputerType::OptionsType     OptionsType;
        typedef typename FilterType::FilterFunctionType              FilterFunctionType;

        OptionsType         _options;
        FilterFunctionType _filterFunc;
        FilterType         _filter;
        ImageType          _image;
       
        
        ConvProductSumComputerType _convProductSum;
        int _callCount;

    public:
        SubPixelEnergyFunction(ImageType img, OptionsType& options):_options(options),_filterFunc(options,0,0,0),
                                                                      _filter(options.getFilterRows(), options.getFilterCols(),options.getStepSize(), _filterFunc),
                                                                      _image(img),
                                                                      _convProductSum(img, _filter, options), _callCount(0)
          {
            assert((bool)img);
           }


        double operator()(PointType &pt)
          {
            _callCount++;
            //_filterFunc.setParams(pt._values[0], 0, -pt._values[1]);

            _filterFunc.setParams(pt.getSubPixelDirection(), 0, (-pt[1]));
            _filter.setFunction(_filterFunc);
            return  -_convProductSum(_image,_filter, pt.getRowPos(), pt.getColPos());
           }
        
        ~SubPixelEnergyFunction(){//cout<<" SubPixel Energy Function: Calls ="<<_callCount<<vcl_endl;
         }
  
   };


using namespace Powell_NS;

#define DIMS   2
template<typename TraitsType>
class LLSubPixelOptimizer
  {
    private:
        
        typedef typename TraitsType::FilterType::LogicalLinearTagType                    TagType;
        typedef ConvProductSumComputer<TraitsType>                                       ConvProductSumComputerType;
        typedef SubPixelLLOptions                                                        OptionsType;
        typedef DirectionMatrix<DIMS>                                                    DirectionMatrixType;
        typedef SubPixelPoint                                                            PointType;
        typedef SubPixelEnergyFunction<PointType, ConvProductSumComputerType>            EnergyFunctionType;
        typedef DirectionMatrixType::DirectionVectorType                                 DirectionVectorType;
        typedef SingleDimFunction<PointType, DirectionVectorType, EnergyFunctionType>    SingleDimFunctionType;
        typedef LineOptimizer<BrentsMethod, SingleDimFunctionType>                       LineOptimizerType;
        typedef PowellOptimizer<LineOptimizerType>                                       PowellOptimizerType;

        PowellOptimizerType                                                              _powellOptimizer;
        DirectionMatrixType                                                              _dirMatrix;

    public:
        LLSubPixelOptimizer(double tolerance, double tiny_val, int powel_it_max, int brent_it_max):_powellOptimizer(tolerance, tiny_val,  powel_it_max, brent_it_max)
          {};
        
        void operator()(vil_image_view<unsigned char> image, vcl_vector<EdgePoint> &edge_list,  OptionsType & ops)
          {
            EnergyFunctionType       energy_function(image, ops);
            
            double function_value=0;
            
            PointType pt(0,0,0);

            for (unsigned int i=0;i<edge_list.size();i++)
              {
                assert(TagType::getImageElementType()==edge_list[i].getImageElementType());

                pt.reset(edge_list[i].getPixelY(), edge_list[i].getPixelX(), edge_list[i].getPixelDirection());
                function_value = -(edge_list[i].getPixelConfidence());
                
                _dirMatrix.reset();
                _powellOptimizer(pt, _dirMatrix, energy_function, function_value);
                
                edge_list[i].setSubPixelPoint(pt.getSubPixelPoint());
                edge_list[i].setSubPixelDirection(pt.getSubPixelDirection());
                edge_list[i].setSubPixelConfidence(-function_value);
               }
           };
       
   };

#undef DIMS
#endif      /* SUBPIXEL_OPTIMIZER_H */
