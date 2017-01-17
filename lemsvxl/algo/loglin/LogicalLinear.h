#ifndef  LOGICAL_LINEAR_H
#define  LOGICAL_LINEAR_H

#include <vcl_string.h>
#include <vcl_sstream.h>

#include <vcl_cstdlib.h>
#include "EdgeDetection.h"
#include "LogicalLinearFilters.h"
#include "LogicalLinearPixel.h"
#include "SubPixelOptimizer.h"


/**
 * NOTE: 
 * (1) Static functions in the tags.
 * */



template<int HalfCount, typename ImageClass=vil_image_view<unsigned char> >
class LogicalLinearEdgeDetector 
  {

    public:

        typedef SubPixelLLOptions    OptionsType;
        typedef ImageClass           ImageType;
        ImageClass    _img;
    
    private:
        OptionsType                    _options;
   
    public:
        
        LogicalLinearEdgeDetector()
          {
           };

        ~LogicalLinearEdgeDetector(){};
        
        
        LogicalLinearEdgeDetector(const LogicalLinearEdgeDetector& old)
          {
            _options = old._options;
            _img     = old._img;
           }
       
        LogicalLinearEdgeDetector& operator=(const LogicalLinearEdgeDetector& old)
          {
            if (this!=&old)
              {
                _options= old._options;
                _img    = old._img;
               }
            return *this;
           }
       
        void setImage(ImageClass img)
          {
            assert((bool)(img));
            _img = img;
           }

        OptionsType getOptions() const
          {
            return _options;
           }
       

        void setOptions(const OptionsType &ops)
          {
            _options = ops;
           }
        

        template<typename OutputDS>
                void getPixelEdges(OutputDS &edges , bool bool_nms)
                  {
                    vcl_cout<<" Getting Pixel Edges .."<<vcl_endl;
                    LogicalLinearPixelEdgeDetector< HalfCount , vil_image_view< unsigned char > >  pixel_edge_detector(_img, _options);
                    pixel_edge_detector.getPixelEdges(edges , bool_nms);
                    vcl_cout<<" Got a Total of  "<<edges.size()<<" Pixel Edges"<<vcl_endl;
                   };
        
               
        template<typename OutputDS>
                void getPixelPLines(OutputDS &plines)
                  {
                    vcl_cout<<" Getting Pixel PLines .."<<vcl_endl;
                    LogicalLinearPixelEdgeDetector< HalfCount , vil_image_view< unsigned char > >  pixel_pline_detector(_img, _options);
                    pixel_pline_detector.getPixelPLines(plines);
                    vcl_cout<<" Got a Total of  "<<plines.size()<<" Pixel PLines"<<vcl_endl;
                   };
        
    
        template<typename OutputDS>
                void getPixelNLines(OutputDS &nlines)
                  {
                    vcl_cout<<" Getting Pixel NLines .."<<vcl_endl;
                    LogicalLinearPixelEdgeDetector< HalfCount , vil_image_view< unsigned char > >  pixel_pline_detector(_img, _options);
                    pixel_pline_detector.getPixelNLines(nlines);
                    vcl_cout<<" Got a Total of  "<<nlines.size()<<" Pixel NLines"<<vcl_endl;
                   };

       
 
        
        
 
        
        template<typename OutputDS>
                void getSubPixelEdges(OutputDS &edges , bool bool_nms)
                  {
                    getPixelEdges(edges , bool_nms);
                   
                    LLSubPixelOptimizer<LLSubPixelEdgeTraits<HalfCount> >  sub_pixel_optimizer(_options.getPowellTolerence(), _options.getPowellTinyValue(), _options.getPowellItMax(), _options.getBrentItMax());
                    sub_pixel_optimizer(_img, edges, _options);
                    return;
                   };
        
  
        template<typename OutputDS>
                void getSubPixelPLines(OutputDS &plines)
                  {
                    getPixelPLines(plines);
                    
                    LLSubPixelOptimizer<LLSubPixelPLineTraits<HalfCount> >  sub_pixel_optimizer(_options.getPowellTolerence(), _options.getPowellTinyValue(), _options.getPowellItMax(), _options.getBrentItMax());
                    sub_pixel_optimizer(_img, plines, _options);
                    return;
                   };
       
        template<typename OutputDS>
                void getSubPixelNLines(OutputDS &nlines)
                  {
                    getPixelNLines(nlines);
                    
                    LLSubPixelOptimizer<LLSubPixelNLineTraits<HalfCount> >  sub_pixel_optimizer(_options.getPowellTolerence(), _options.getPowellTinyValue(), _options.getPowellItMax(), _options.getBrentItMax());
                    sub_pixel_optimizer(_img, nlines, _options);
                    return;
                   };

       

        vcl_string getName() 
         {
           return "Logical-Linear";
          }

   };








#endif     /* LOGICAL_LINEAR_H */

