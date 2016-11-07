#ifndef EDGE_DETECTOR_H
#define EDGE_DETECTOR_H

//#include "BasePlugin.h"
//#include "BaseWindow.h"
//#include "PluginManager.h"
//#include "DrawingWindow.h"

#include <vcl_algorithm.h>
#include "points.h"
#include "EdgeDetection.h"
#include "LogicalLinear.h"
#include <vcl_string.h>
#include <vcl_iostream.h>
//#include <fstream>
//#include <sstream>
#include <vcl_map.h>
#include <vcl_cmath.h>

//#include <wx/wx.h>




//using namespace std;



/*class EdgeDetectorApp : public wxApp
  {
    public:
        bool OnInit();
        int  OnExit();
   };
DECLARE_APP(EdgeDetectorApp);
*/
template<typename DetectorType>
class EdgeDetector
  {
    private:
        
        DetectorType  _edgeDetector;
        string        _filename;
        EdgeWriter    _writer;
        
    public:
  
        typedef typename DetectorType::OptionsType OptionsType;

        EdgeDetector()
          {
           }
        
        ~EdgeDetector(){};

        void operator()(string input_filename, string output_filename, int image_elem, bool only_pixel, int half_max_tan_partitions)
          {
            vcl_vector<EdgePoint> edge_list;
            vcl_vector<EdgePoint> pline_list;
            vcl_vector<EdgePoint> nline_list;

            EdgeWriter writer;

            RGBImage *image = new RGBImage(input_filename.c_str());
            if (!image)
                return ;

            image->convertToGrayScale();
            _edgeDetector.setImage(image);
         
            if (image_elem&IMAGE_ELEMENT_EDGE)
              {
                detectEdges(edge_list, only_pixel, half_max_tan_partitions);
               };
            
            if (image_elem&IMAGE_ELEMENT_PLINE)
              {
                detectPLines(pline_list, only_pixel, half_max_tan_partitions);
               };

            if(image_elem&IMAGE_ELEMENT_NLINE)
              {
                detectNLines(nline_list, only_pixel, half_max_tan_partitions);
               }
            

            string header = _edgeDetector.getName()+"\n";
            header +=_edgeDetector.getOptions().printOptions("#");

            vcl_ostringstream ostrm;
            ostrm<<"# \t Half Tangential Partitions  : "<<half_max_tan_partitions<<vcl_endl;
            if (only_pixel)
                ostrm<<"# \t Convergence                 : ONLY_PIXEL"<<vcl_endl;
            else
                ostrm<<"# \t Convergence                 : SUB_PIXEL"<<vcl_endl;

            ostrm    <<"# Input Image Width              : "<<image->width()<<vcl_endl;
            ostrm    <<"# Input Image Height             : "<<image->height()<<vcl_endl;
            ostrm    <<"# Input File                     : "<<input_filename<<vcl_endl;
            ostrm    <<"# Output File                    : "<<output_filename<<vcl_endl;
           
            header +=ostrm.str();
            
            copy (pline_list.begin(), pline_list.end(), back_inserter(edge_list));
            copy (nline_list.begin(), nline_list.end(), back_inserter(edge_list));
            
            writer(edge_list, output_filename, header);
            delete image;
            return;
           }


        int detectEdges(vcl_vector<EdgePoint> &edge_list, bool only_pixel, int half_max_tan_partitions)
          {
            
            /**
             * The .template construct used in the following code is explained 
             * in : C++ Templates: The Complete Guide by David Vandevoorde, Nicolai M. Josuttis.
             *
             * In a nutshell, it is needed to tell the compiler that the <> that follow 
             * indicate templates and not a "less than" and   a "greater than" sign.
             * */
            
            if (only_pixel)
              {
                switch(half_max_tan_partitions)
                  {
                    case(1):
                        _edgeDetector.template getPixelEdges<1>(edge_list);break;
                    case(2):
                        _edgeDetector.template getPixelEdges<2>(edge_list);break;
                    case(3):
                        _edgeDetector.template getPixelEdges<3>(edge_list);break;
                    case(4):
                        _edgeDetector.template getPixelEdges<4>(edge_list);break;
                    case(5):
                        _edgeDetector.template getPixelEdges<5>(edge_list);break;
                    default:
                        //Should not get here.
                        assert(false);
                   }
               }
            else
              {
                switch(half_max_tan_partitions)
                  {    
                    case(1):
                        _edgeDetector.template getSubPixelEdges<1>(edge_list);break;
                    case(2):
                        _edgeDetector.template getSubPixelEdges<2>(edge_list);break;
                    case(3):
                        _edgeDetector.template getSubPixelEdges<3>(edge_list);break;
                    case(4):
                        _edgeDetector.template getSubPixelEdges<4>(edge_list);break;
                    case(5):
                        _edgeDetector.template getSubPixelEdges<5>(edge_list);break;
                    default:
                        //Should not get here.
                        assert(false);
                   }
               }
            

            return 0; 
           };

        int detectPLines(vcl_vector<EdgePoint> &edge_list, bool only_pixel, int half_max_tan_partitions)
          {
            
            /**
             * The .template construct used in the following code is explained 
             * in : C++ Templates: The Complete Guide by David Vandevoorde, Nicolai M. Josuttis.
             *
             * In a nutshell, it is needed to tell the compiler that the <> that follow 
             * indicate templates and not a "less than" and   a "greater than" sign.
             * */
            
            if (only_pixel)
              {
                switch(half_max_tan_partitions)
                  {
                    case(1):
                        _edgeDetector.template getPixelPLines<1>(edge_list);break;
                    case(2):
                        _edgeDetector.template getPixelPLines<2>(edge_list);break;
                    case(3):
                        _edgeDetector.template getPixelPLines<3>(edge_list);break;
                    case(4):
                        _edgeDetector.template getPixelPLines<4>(edge_list);break;
                    case(5):
                        _edgeDetector.template getPixelPLines<5>(edge_list);break;
                    default:
                        //Should not get here.
                        assert(false);
                   }
               }
            else
              {
                switch(half_max_tan_partitions)
                  {    
                    case(1):
                        _edgeDetector.template getSubPixelPLines<1>(edge_list);break;
                    case(2):
                        _edgeDetector.template getSubPixelPLines<2>(edge_list);break;
                    case(3):
                        _edgeDetector.template getSubPixelPLines<3>(edge_list);break;
                    case(4):
                        _edgeDetector.template getSubPixelPLines<4>(edge_list);break;
                    case(5):
                        _edgeDetector.template getSubPixelPLines<5>(edge_list);break;
                    default:
                        //Should not get here.
                        assert(false);
                   }
               }
            

            return 0; 
           }

        int detectNLines(vcl_vector<EdgePoint> &edge_list, bool only_pixel, int half_max_tan_partitions)
          {
            
            /**
             * The .template construct used in the following code is explained 
             * in : C++ Templates: The Complete Guide by David Vandevoorde, Nicolai M. Josuttis.
             *
             * In a nutshell, it is needed to tell the compiler that the <> that follow 
             * indicate templates and not a "less than" and   a "greater than" sign.
             * */
            
            if (only_pixel)
              {
                switch(half_max_tan_partitions)
                  {
                    case(1):
                        _edgeDetector.template getPixelNLines<1>(edge_list);break;
                    case(2):
                        _edgeDetector.template getPixelNLines<2>(edge_list);break;
                    case(3):
                        _edgeDetector.template getPixelNLines<3>(edge_list);break;
                    case(4):
                        _edgeDetector.template getPixelNLines<4>(edge_list);break;
                    case(5):
                        _edgeDetector.template getPixelNLines<5>(edge_list);break;
                    default:
                        //Should not get here.
                        assert(false);
                   }
               }
            else
              {
                switch(half_max_tan_partitions)
                  {    
                    case(1):
                        _edgeDetector.template getSubPixelNLines<1>(edge_list);break;
                    case(2):
                        _edgeDetector.template getSubPixelNLines<2>(edge_list);break;
                    case(3):
                        _edgeDetector.template getSubPixelNLines<3>(edge_list);break;
                    case(4):
                        _edgeDetector.template getSubPixelNLines<4>(edge_list);break;
                    case(5):
                        _edgeDetector.template getSubPixelNLines<5>(edge_list);break;
                    default:
                        //Should not get here.
                        assert(false);
                   }
               }
            

            return 0; 
           }
        
        
        OptionsType getOptions() const
          {
            return _edgeDetector.getOptions();
           }

        void setOptions(const OptionsType & ops)
          {
            _edgeDetector.setOptions(ops);
           }
  

        string getName() 
          {
            return _edgeDetector.getName();
           }

        void printFilters() const
          {
            typedef LLPixelEdgeFilter<1>  FilterUtilType;

            OptionsType ops=_edgeDetector.getOptions();
            
            FilterUtilType::FilterFunctionType filter_function(ops,M_PI,0,0);
            FilterUtilType::type               filter(ops.getFilterRows(), ops.getFilterCols(), ops.getStepSize(),filter_function);

            cout<<filter.dumpFilter()<<vcl_endl;
           }

        void readAnlFile(string input_filename)
          {
            vcl_vector<AnlObject> obj_list;
            AnlFileParser parser;
            parser(obj_list, input_filename);

            for (unsigned int i=0;i<obj_list.size();i++)
              {
                cout<<obj_list[i];
               }
           }
   
   };





#endif    /*  EDGE_DETECTION_TEST_WINDOW_H */
