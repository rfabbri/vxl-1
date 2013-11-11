#ifndef LOGICAL_LINEAR_FILTERS_H
#define LOGICAL_LINEAR_FILTERS_H

#include "SubPixel.h"
#include "LogicalLinearFunctions.h"
//#include "Utils.h"
#include <vil/vil_image_view.h>

#define  TINY_VALUE          1e-15  //Use to avoid division by 0


//
//TAGS indicating ONLY_PIXEL and SUB_PIXEL.
//
struct FilterCategoryTagPixelOnly{};
struct FilterCategoryTagSubPixelOnly{};


//Forward declaration. Needed for the friend declaration.
template<typename TraitsClass>
class ConvProductSumComputer;


/*------------------- The combinator ---------------------*/

template<typename FilterTagType>
class LLCombinatorSimple
  {
  enum {TEST_VAL=FilterTagType::NET_COMPONENTS};

    private:
        LLElemCombinator _llElem;
    public:
        LLCombinatorSimple(double rho):_llElem(rho)
          {
           };


        inline double operator()(double (&conv_values)[(int)TEST_VAL])
                  {
                    //COMPILE_TIME_ENFORCE((FilterTagType::HALF_MAX_TAN_COMPONENTS>1));

                    double left=0;
                    double right=0;
                    double mid_left=0;
                    double mid_right=0;

                    
                    left = conv_values[FilterTagType::LEFT_LIMIT-1];
                    for (int i=FilterTagType::LEFT_LIMIT-2;i>=0;--i)
                      {
                        left =  _llElem.LLAnd(conv_values[i], left); 
                       }

                    right = conv_values[FilterTagType::RIGHT_LIMIT];
                    for (int i=FilterTagType::RIGHT_LIMIT+1; i<FilterTagType::NET_COMPONENTS;++i)
                      {
                        right =  _llElem.LLAnd(conv_values[i], right); 
                       }


                    mid_left = conv_values[FilterTagType::LEFT_LIMIT];

                    for (int i=1;i<FilterTagType::MAX_NORMAL_COMPONENTS; ++i)
                      {
                        mid_left = _llElem.LLAnd(conv_values[FilterTagType::LEFT_LIMIT+i], mid_left);
                       }

                    mid_right = conv_values[FilterTagType::RIGHT_LIMIT-1]; 
 
                    for (int i=1;i<FilterTagType::MAX_NORMAL_COMPONENTS; ++i)
                      {
                        mid_right = _llElem.LLAnd(conv_values[FilterTagType::RIGHT_LIMIT-i-1], mid_right);
                       }


                    return  _llElem.LLAnd(_llElem.LLAnd(mid_left, mid_right),_llElem.LLOr(left, right));
                   }


   };


/*
template<>
double LLCombinatorSimple<LLEdgeTag<1> >::operator()(double (&conv_values)[LLEdgeTag<1>::NET_COMPONENTS])
  {
    double temp=conv_values[0];

    for (int i=1;i<(LLEdgeTag<1>::NET_COMPONENTS);i++)
      {
        temp=_llElem.LLAnd(temp, conv_values[i]);
       }
    
    return temp;
   };


template<>
double LLCombinatorSimple<LLPLineTag<1> >::operator()(double (&conv_values)[LLPLineTag<1>::NET_COMPONENTS])
  {
    double temp=conv_values[0];

    for (int i=1;i<(LLPLineTag<1>::NET_COMPONENTS);i++)
      {
        temp=_llElem.LLAnd(temp, conv_values[i]);
       }
    
    return temp;
   };


template<>
double LLCombinatorSimple<LLNLineTag<1> >::operator()(double (&conv_values)[LLNLineTag<1>::NET_COMPONENTS])
  {
    double temp=conv_values[0];

    for (int i=1;i<(LLNLineTag<1>::NET_COMPONENTS);i++)
      {
        temp=_llElem.LLAnd(temp, conv_values[i]);
       }
    
    return temp;
   };
*/




/*---------------------- The Filter -----------------------------*/

template<typename FunctionType, typename FilterCategoryTag=FilterCategoryTagSubPixelOnly>
class LogicalLinearFilter
  {
    public:

        typedef FunctionType                                FilterFunctionType;
        typedef typename FunctionType::LogicalLinearTagType LogicalLinearTagType;
        typedef  FilterCategoryTag                          FilterCategoryTagType;
        
        typedef double *                                    FilterArrayType;
         
  //      template<typename TraitsClass> 
//      friend class ConvProductSumComputer;

    //used to be private, should still be private, friend class problems, probably
    public:
        FilterArrayType  _filterArray;

        FilterArrayType  _filterNormalComponents;
        FilterArrayType  _filterTangentialComponents;

        const int        _nRows;
        const int        _nCols;
        const int        _rowMid;
        const int        _colMid;
        const double     _stepSize;
        
        
        FunctionType     _function;

        const double     _rowMin;
        const double     _rowMax;
        const double     _colMin;
        const double     _colMax;
       

        //Do not use this.
        void operator=(const LogicalLinearFilter<FunctionType, FilterCategoryTagType> &old);
        LogicalLinearFilter(const LogicalLinearFilter<FunctionType, FilterCategoryTagType> &old);



      inline void _allocateArrays()
        {
          FilterCategoryTagType dummy;
          _allocateMainFilterArray(dummy); 
          _filterTangentialComponents = new double[_nRows*_nCols*(int)(LogicalLinearTagType::MAX_TAN_COMPONENTS)];
          _filterNormalComponents     = new double[_nRows*_nCols*(int)(LogicalLinearTagType::MAX_NORMAL_COMPONENTS)]; 
         
          //cout<<" Tan    Components ="<<(int)(LogicalLinearTagType::MAX_TAN_COMPONENTS)<<endl;
          //cout<<" Normal Components ="<<(int)(LogicalLinearTagType::MAX_NORMAL_COMPONENTS)<<endl;
          //cout<<" Net    Components ="<<(int)(LogicalLinearTagType::NET_COMPONENTS)<<endl;
         }
        
      inline void _allocateMainFilterArray(FilterCategoryTagPixelOnly)
        {
          _filterArray=new double[ (int)LogicalLinearTagType::NET_COMPONENTS * _nRows * _nCols ];
         }
      
      inline void _allocateMainFilterArray(FilterCategoryTagSubPixelOnly)
        {
          _filterArray=0;
         }
      
      inline void _fillArray(FilterCategoryTagPixelOnly)
        {
          double row = _rowMin, col=_colMin;
          double x(0), y(0);

          double scratch[LogicalLinearTagType::NET_COMPONENTS];


          for (int i= 0;i<LogicalLinearTagType::NET_COMPONENTS;++i)
              scratch[i]=0;

          double *filter_tan=_filterTangentialComponents;
          double *filter_norm=_filterNormalComponents;

          double *filter_all=_filterArray;

          double temp(0); 
          int    temp_index(0);
          double *filter_norm_temp=filter_norm;
          
          for (row=_rowMin;row<=_rowMax;row +=_stepSize)
            {
              for (col=_colMin;col<=_colMax;col +=_stepSize)
                {
                  _function.rotateAndShift(col,row,x,y);

                  _function.tangentialComponents(x, filter_tan);
                  _function.normalComponents(y, filter_norm);
                  
                  temp_index =0;
                  for (int index=0;index<(LogicalLinearTagType::MAX_TAN_COMPONENTS);++index)
                    {
                      filter_norm_temp = filter_norm;
                      for (int norm_index=0;norm_index<((int)LogicalLinearTagType::MAX_NORMAL_COMPONENTS);++norm_index)
                        {
                          temp = (*filter_norm_temp)*(*filter_tan);
                                  
                          scratch[temp_index] +=temp*temp;
                          (*filter_all)=temp;
                          ++filter_all;
                          ++temp_index;
                          ++filter_norm_temp;
                         }

                      ++filter_tan;
                     }
                  filter_norm +=(int)LogicalLinearTagType::MAX_NORMAL_COMPONENTS;
                 }
             }


          for (int i= 0;i<LogicalLinearTagType::NET_COMPONENTS;++i)
            {
              scratch[i]=1.0/(vcl_sqrt(scratch[i])+TINY_VALUE);
             }

          filter_all=(_filterArray);

          //Normalize the filter. 
          int size((int)LogicalLinearTagType::NET_COMPONENTS*_nRows*_nCols);
          for (int i=0;i<size;++i)
            {
              (*filter_all)   *=scratch[i%LogicalLinearTagType::NET_COMPONENTS];
              filter_all++;
             }
          return;
         };


      inline void _fillArray(FilterCategoryTagSubPixelOnly)
        {
          double row = _rowMin, col=_colMin;
          double x(0), y(0);

          double *filter_tan=_filterTangentialComponents;
          double *filter_norm=_filterNormalComponents;


          for (row=_rowMin;row<=_rowMax;row +=_stepSize)
            {
              for (col=_colMin;col<=_colMax;col +=_stepSize)
                {
                  _function.rotateAndShift(col,row,x,y);

                  _function.tangentialComponents(x, filter_tan);
                  
                  //cout<<" Tangential :";
                  //for (int i=0;i<LogicalLinearTagType::MAX_TAN_COMPONENTS;++i)
                  //    cout<<*(filter_tan+i)<<"  ";
                  //cout<<endl;
                  
                  filter_tan  +=LogicalLinearTagType::MAX_TAN_COMPONENTS;
                  
                  
                  _function.normalComponents(y, filter_norm);
                  
                  //cout<<" Normal :";
                  //for (int i=0;i<LogicalLinearTagType::MAX_NORMAL_COMPONENTS;++i)
                  //    cout<<*(filter_norm+i)<<"  ";
                  //cout<<endl;
                  
                  filter_norm +=LogicalLinearTagType::MAX_NORMAL_COMPONENTS;
          

                 }
             }
    
          
          return;
         };
         
    public:

        
        LogicalLinearFilter(int rows, int cols, double step_size, FunctionType& func):
                _nRows(rows),_nCols(cols),_rowMid((int)_nRows/2),_colMid((int)_nCols/2),_stepSize(step_size),
                _function(func), _rowMin(-_rowMid*_stepSize),_rowMax(-_rowMin),_colMin(-_colMid*_stepSize),_colMax(-_colMin)
          {
            //Ensure that the mask has odd dimensions.
            //cout<<" Rows ="<<rows<<" Cols ="<<cols<<endl;
            assert(rows%2==1);
            assert(cols%2==1);

            _filterArray=0;
            _filterNormalComponents=0;
            _filterTangentialComponents=0;
           
            _allocateArrays();
            FilterCategoryTagType dummy;
            _fillArray(dummy);
           };


      ~LogicalLinearFilter()
        {
          if (_filterArray)
              delete [] _filterArray;
          _filterArray=0;
              
          if (_filterNormalComponents)
                  delete [] _filterNormalComponents;
         _filterNormalComponents=0; 
          
          if (_filterTangentialComponents)
              delete [] _filterTangentialComponents;
          _filterTangentialComponents=0;
         }

       

        
      int getMaxComponents() const
        {
          return _function.getNetMaxComponents();
         }
      



      
      inline void setFunction(FunctionType &func)
        {
          FilterCategoryTagType dummy;
          _function = func;
          _fillArray(dummy);
         }
      
     
      int getNRows() const
        {
          return _nRows;
         }
      
      
      int getNCols() const
        {
          return _nCols;
         }
      
      double getTheta() const
        {
          return _function.getTheta();
         }
      
      double getSigmaRatio() const
        {
          return _function.getSigmaRatio();
         }
      
      vcl_string dumpFilter() const
        {
          vcl_ostringstream ostrm;

          assert(_filterArray!=0);
          for (int comp=0;comp<LogicalLinearTagType::NET_COMPONENTS;comp++) 
            {
              for (int i=0;i<_nRows;i++)
                {
                  //ostrm<<" Row "<<setw(3)<<i<<" : ";
                  for (int j=0;j<_nCols;j++)
                    {
                      ostrm<<" "<<setprecision(4)<<setw(8);
                      if(fabs(_filterArray[(i*_nCols+j)*LogicalLinearTagType::NET_COMPONENTS+comp])>1e-6)
                          ostrm<<_filterArray[(i*_nCols+j)*LogicalLinearTagType::NET_COMPONENTS+comp]<<" ";
                      else
                          ostrm<<0<<" ";
                     }
                  ostrm<<endl;
                 }
                ostrm<<endl<<endl;
               }
            return ostrm.str();
           }


        vcl_string getFilterName() const
          {
            return _function.getFunctionName()+"-Filter";
           }
   };





/*
 * Utility Class for the Filter. Needed because C++ does not
 * permit templated typedefs.
 * */


template<int HalfCount>
struct LLPixelEdgeFilter
  {
    typedef typename LLEdgeFunction<HalfCount>::type                             FilterFunctionType;
    typedef typename FilterFunctionType::LogicalLinearTagType                    FilterTagType;
            
    typedef LogicalLinearFilter<FilterFunctionType, FilterCategoryTagPixelOnly>  type;
    typedef FilterCategoryTagPixelOnly                                           FilterCategoryTagType;
    
    //
    //Needed for compile time assert.
    //
    typedef FilterCategoryTagPixelOnly          PixelOnlyFilter; 
   };

template<int HalfCount>
struct LLPixelPLineFilter
  {
    typedef typename LLPLineFunction<HalfCount>::type                                     FilterFunctionType;
    typedef typename FilterFunctionType::LogicalLinearTagType                             FilterTagType;
                       
    typedef LogicalLinearFilter<FilterFunctionType, FilterCategoryTagPixelOnly>  type;
    typedef FilterCategoryTagPixelOnly                                           FilterCategoryTagType;
    
    //
    //Needed for compile time assert.
    //
    typedef FilterCategoryTagPixelOnly          PixelOnlyFilter; 
   };

template<int HalfCount>
struct LLPixelNLineFilter
  {
    typedef typename  LLNLineFunction<HalfCount>::type                                     FilterFunctionType;
    typedef typename  FilterFunctionType::LogicalLinearTagType                             FilterTagType;
            
    typedef LogicalLinearFilter<FilterFunctionType, FilterCategoryTagPixelOnly>  type;
    typedef FilterCategoryTagPixelOnly                                           FilterCategoryTagType;
    
    //
    //Needed for compile time assert.
    //
    typedef FilterCategoryTagPixelOnly          PixelOnlyFilter; 
   };



template<int HalfCount>
struct LLSubPixelEdgeFilter
  {
    typedef typename LLEdgeFunction<HalfCount>::type                                          FilterFunctionType;
    typedef typename FilterFunctionType::LogicalLinearTagType                                 FilterTagType;
    typedef LogicalLinearFilter<FilterFunctionType, FilterCategoryTagSubPixelOnly>   type;
    typedef FilterCategoryTagSubPixelOnly                                            FilterCategoryTagType;

    //
    //Needed for compile time assert.
    //
    typedef FilterCategoryTagSubPixelOnly     SubPixelFilter; 
   };


template<int HalfCount>
struct LLSubPixelPLineFilter
  {
    typedef typename LLPLineFunction<HalfCount>::type                                         FilterFunctionType;
    typedef typename  FilterFunctionType::LogicalLinearTagType                                 FilterTagType;
    typedef LogicalLinearFilter<FilterFunctionType, FilterCategoryTagSubPixelOnly>   type;
    typedef FilterCategoryTagSubPixelOnly                                            FilterCategoryTagType;

    //
    //Needed for compile time assert.
    //
    typedef FilterCategoryTagSubPixelOnly     SubPixelFilter; 
   };

template<int HalfCount>
struct LLSubPixelNLineFilter
  {
    typedef typename LLNLineFunction<HalfCount>::type                                         FilterFunctionType;
    typedef typename FilterFunctionType::LogicalLinearTagType                                 FilterTagType;
    typedef LogicalLinearFilter<FilterFunctionType, FilterCategoryTagSubPixelOnly>   type;
    typedef FilterCategoryTagSubPixelOnly                                            FilterCategoryTagType;

    //
    //Needed for compile time assert.
    //
    typedef FilterCategoryTagSubPixelOnly     SubPixelFilter; 
   };

/*----------------------- The Convolution Product Sum Computer ------------*/



/**
 * The Traits classes. These are here just for convenience. They gather all 
 * options needed for instantiation in one place. So the template needs just
 * one argument. These are used just by the ConvProductSumComputer class.
 * */

template<int HalfCount,  typename ConvOptionsType=SubPixelLLOptions, typename ConvImageType=vil_image_view<unsigned char> >
struct LLPixelEdgeTraits
  {
   typedef  LLPixelEdgeFilter<HalfCount>                                           FilterUtilityClassType;  
   typedef  typename FilterUtilityClassType::type                                           FilterType;  
   typedef  ConvImageType                                                          ImageType;
   typedef  ConvOptionsType                                                        OptionsType;
   typedef  LLCombinatorSimple<typename FilterUtilityClassType::FilterTagType>     LLCombinatorType;
   };


template<int HalfCount,  typename ConvOptionsType=SubPixelLLOptions, typename ConvImageType=vil_image_view<unsigned char> >
struct LLPixelPLineTraits
  {
   typedef  LLPixelPLineFilter<HalfCount>                                           FilterUtilityClassType;  
   typedef typename  FilterUtilityClassType::type                                           FilterType;  
   typedef  ConvImageType                                                          ImageType;
   typedef  ConvOptionsType                                                        OptionsType;
   typedef  LLCombinatorSimple<typename FilterUtilityClassType::FilterTagType>     LLCombinatorType;
   };


template<int HalfCount,  typename ConvOptionsType=SubPixelLLOptions, typename ConvImageType=vil_image_view<unsigned char> >
struct LLPixelNLineTraits
  {
   typedef  LLPixelNLineFilter<HalfCount>                                           FilterUtilityClassType;  
   typedef typename  FilterUtilityClassType::type                                           FilterType;  
   typedef  ConvImageType                                                          ImageType;
   typedef  ConvOptionsType                                                        OptionsType;
   typedef  LLCombinatorSimple<typename FilterUtilityClassType::FilterTagType>     LLCombinatorType;
   };






template<int HalfCount, typename Options=SubPixelLLOptions, typename Image=vil_image_view<unsigned char> >
struct LLSubPixelEdgeTraits
  {
    typedef  LLSubPixelEdgeFilter<HalfCount>                                        FilterUtilityClassType;  
    typedef typename  FilterUtilityClassType::type                                           FilterType;  
    typedef  Image                                                                  ImageType;
    typedef  Options                                                                OptionsType;
    typedef  LLCombinatorSimple<typename FilterUtilityClassType::FilterTagType>     LLCombinatorType;
   };


template<int HalfCount, typename Options=SubPixelLLOptions, typename Image=vil_image_view<unsigned char> >
struct LLSubPixelPLineTraits
  {
    typedef   LLSubPixelPLineFilter<HalfCount>                                       FilterUtilityClassType;  
    typedef  typename  FilterUtilityClassType::type                                           FilterType;  
    typedef  Image                                                                  ImageType;
    typedef  Options                                                                OptionsType;
    typedef  LLCombinatorSimple<typename FilterUtilityClassType::FilterTagType>     LLCombinatorType;
   };


template<int HalfCount, typename Options=SubPixelLLOptions, typename Image=vil_image_view<unsigned char> >
struct LLSubPixelNLineTraits
  {
    typedef  LLSubPixelNLineFilter<HalfCount>                                       FilterUtilityClassType;  
    typedef  typename FilterUtilityClassType::type                                           FilterType;  
    typedef  Image                                                                  ImageType;
    typedef  Options                                                                OptionsType;
    typedef  LLCombinatorSimple<typename FilterUtilityClassType::FilterTagType>     LLCombinatorType;
   };




/**
 * Get the traits given the FilterUtilType. 
 * Just for convenience.
 * */


template<typename FilterUtilType>
struct LLTraits
  {
    typedef  FilterUtilType                                                         FilterUtilityClassType;  
    typedef  typename FilterUtilityClassType::type                                  FilterType;  
    typedef  vil_image_view< unsigned char >                                        ImageType;
    typedef  SubPixelLLOptions                                                      OptionsType;
    typedef  LLCombinatorSimple<typename FilterUtilityClassType::FilterTagType>     LLCombinatorType;
   };






template<typename TraitsType>
class ConvProductSumComputer
  {
    public:    
        typedef typename TraitsType::FilterUtilityClassType    FilterUtilityClassType;

        typedef typename TraitsType::FilterType                FilterType;
        typedef typename FilterType::FilterFunctionType        FilterFunctionType;
        typedef typename TraitsType::ImageType                 ImageType;
        typedef typename TraitsType::OptionsType               OptionsType;
        typedef typename TraitsType::LLCombinatorType          LLCombinatorType;
        
        typedef typename FilterType::LogicalLinearTagType      FilterTagType;

        // used to be private:, should be fixed -- check friend template above
    public:
        LLCombinatorType        _LLCombinator;
        
        const int               _maskNRows;
        const int               _maskNCols;
        const int               _rowsMid;
        const int               _colsMid;
       
        const int               _imageWidth;
        const int               _imageHeight;
        const int               _imageSize;
        
        const int               _paddedWidth;
        const int               _paddedHeight;
        
        unsigned int*            _imageArray;
        
        

        const int   _scratchBufferSize; 
        double      _scratch1[FilterTagType::NET_COMPONENTS];
        double      _scratch2[FilterTagType::NET_COMPONENTS];


    
    public:
        ConvProductSumComputer(const ImageType img, const FilterType &mask,const OptionsType& ops):
                _LLCombinator(ops.getLLCombinatorRho()), 
        _maskNRows(mask.getNRows()),
        _maskNCols(mask.getNCols()),
        _rowsMid((int)vcl_floor((double)_maskNRows/2)),
        _colsMid((int)vcl_floor((double)_maskNCols/2)),
        
        _imageWidth(img.ni()),
        _imageHeight(img.nj()),
        _imageSize(_imageWidth* _imageHeight),
        _paddedWidth(_imageWidth  +2*_colsMid),
        _paddedHeight(_imageHeight +2*_rowsMid),
        _scratchBufferSize(FilterTagType::NET_COMPONENTS)

          {
            for (int i=0;i<_scratchBufferSize;i++)
              {
                _scratch1[i]=0;
                _scratch2[i]=0;
               }

            _imageArray  = new unsigned int [_paddedWidth*_paddedHeight];

                    
            int i=0,j=0;
            for (int x=0;x<_paddedWidth;x++)
              {
                i=x-_colsMid;

                if (i<0)
                    i=0;
                if (i>=_imageWidth)
                    i=_imageWidth-1;
                
                for (int y=0;y<_paddedHeight;y++)
                  {
                    j=y-_rowsMid;
                    
                    if (j<0)
                        j=0;
                    if (j>=_imageHeight)
                        j=_imageHeight-1;
                   
                   
                    _imageArray[y*_paddedWidth+x] = img(i,j);

                    //assert(img->r(i,j)==img->g(i,j));
                    //assert(img->g(i,j)==img->b(i,j));
                   }
               }

           }
       

       ~ConvProductSumComputer()
         {
           delete [] _imageArray;
           _imageArray=0;
          }
                               
       // Used ONLY for SUBPIXEL Convolution
       // ImageType argument is NOT used.
     inline double operator()(const ImageType image, const FilterType &mask, int row_pos, int col_pos)
       {
         assert(_maskNRows    == mask.getNRows());
         assert(_maskNCols    == mask.getNCols());
         assert(_rowsMid      == (int)vcl_floor((double)_maskNRows/2));
         assert(_colsMid      == (int)vcl_floor((double)_maskNCols/2));

         //Ensure that this is SUBPIXEL
         //TODO Re-enable COMPILE_TIME_CHECK????
         //COMPILE_TIME_CHECK( typename FilterUtilityClassType::SubPixelFilter *dummy; dummy=0);

         double *filter_tan    = mask._filterTangentialComponents;
         double *filter_norm   = mask._filterNormalComponents;


         for (int i=0;i<FilterTagType::NET_COMPONENTS;++i)
           {
             _scratch1[i]=0;
             _scratch2[i]=0;
            }

         int pixel_value(0);
         double temp(0);
         int x(0), y(0);

         int index=0;



         col_pos     +=_colsMid;
         row_pos     +=_rowsMid;
         
         const int xstart  = col_pos + _colsMid; 
         const int xend    = col_pos - _colsMid;
         const int ystart  = row_pos + _rowsMid; 
         const int yend    = row_pos - _rowsMid;

         assert(xstart<_paddedWidth);
         assert(xend>=0);
         assert(ystart<_paddedHeight);
         assert(yend>=0);


         /**
          * Core Loop
          * */
         
         int image_pos=_paddedWidth*ystart+xstart;
         
         for (y=ystart;y>=yend;--y)
           {
             for(x=xstart;x>=xend;--x)
               {
                 pixel_value  = _imageArray[image_pos];
                 --image_pos;

                 
                 index=0;
                 for (int tan_component=0;tan_component<FilterTagType::MAX_TAN_COMPONENTS;++tan_component)
                   {
                     for (int norm_component=0; norm_component<FilterTagType::MAX_NORMAL_COMPONENTS;++norm_component)
                       {
                         temp = (filter_norm[norm_component])*(*filter_tan);
                         _scratch1[index]+= temp*temp;
                         _scratch2[index]+= temp*pixel_value;
                         ++index;
                        }
                     ++filter_tan;
                    }
                 filter_norm +=FilterTagType::MAX_NORMAL_COMPONENTS;
                }
             //Go to the previous row.
             image_pos =image_pos +_maskNCols-_paddedWidth;
            }



         for (int component=0;component<FilterTagType::NET_COMPONENTS;++component)
           {
             _scratch2[component] /=vcl_sqrt(_scratch1[component]+TINY_VALUE);
            }

        

         return _LLCombinator(_scratch2);
        }

       // Used ONLY for PIXEL Convolution
       // ImageType argument is NOT used.
     inline void operator()(const ImageType image,   const FilterType &mask, int row_pos, int col_pos, double &theta_val, double &theta_plus_180_val)
       {
         assert(_maskNRows      == mask.getNRows());
         assert(_maskNCols      == mask.getNCols());
         assert(_rowsMid        == (int)vcl_floor((double)_maskNRows/2));
         assert(_colsMid        == (int)vcl_floor((double)_maskNCols/2));

         //COMPILE_TIME_CHECK(typename FilterUtilityClassType::PixelOnlyFilter *dummy; dummy=0;);

         double *filter_all =mask._filterArray;

         for (int i=0;i<(int)FilterTagType::NET_COMPONENTS;++i)
           {
             _scratch1[i]=0;
             _scratch2[i]=0;
            }


         int pixel_value(0);
         int x(0), y(0);

         col_pos     +=_colsMid;
         row_pos     +=_rowsMid;
         
         const int xstart  = col_pos + _colsMid; 
         const int xend    = col_pos - _colsMid;
         const int ystart  = row_pos + _rowsMid; 
         const int yend    = row_pos - _rowsMid;

         assert(xstart<_paddedWidth);
         assert(xend>=0);
         assert(ystart<_paddedHeight);
         assert(yend>=0);


         /**
          * Core Loop
          * */
         
         int image_pos=_paddedWidth*ystart+xstart;
         
         for (y=ystart;y>=yend;--y)
           {
             for(x=xstart;x>=xend;--x)
               {
                 pixel_value  = _imageArray[image_pos];
                 --image_pos;
                 for (int component=0;component<((int)FilterTagType::NET_COMPONENTS);++component)
                     _scratch1[component] += filter_all[component]*(double)pixel_value;
                 
                 filter_all += (int)FilterTagType::NET_COMPONENTS;
                }
             //Go to the previous row.
             image_pos =image_pos +_maskNCols-_paddedWidth;
            }

         /***********************************************/
         
         theta_val     = _LLCombinator(_scratch1);

         if (FilterTagType::getImageElementType()!=IMAGE_ELEMENT_EDGE)
           {
             theta_plus_180_val =0;
             return;
            }


         for (int i=0;i<(int)FilterTagType::NET_COMPONENTS;++i)
           {
             _scratch1[i] *=-1;
            }

         theta_plus_180_val = _LLCombinator(_scratch1);
         return;
        }

   };



#endif    /* LOGICAL_LINEAR_FILTERS_H */
