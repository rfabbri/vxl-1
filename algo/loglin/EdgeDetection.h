#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include <vcl_vector.h>
#include <vcl_utility.h>
#include "points.h"
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vcl_fstream.h>
//#include <sstream>
//#include <fstream>
//#include <iomanip>
//#include "Tokenizer.h"

enum IMAGE_ELEMENT_TYPE {IMAGE_ELEMENT_EDGE=1, IMAGE_ELEMENT_PLINE=2, IMAGE_ELEMENT_NLINE=4};


class EdgePoint
  {
    public:

    private:
        typedef Point2D<double> PointType;

        int _imageElementType;

        PointType   _pixelPoint;
        double      _pixelDirection;
        double      _pixelConfidence;
        
        PointType   _subPixelPoint;
        double      _subPixelDirection;
        double      _subPixelConfidence;


    public:

        EdgePoint(PointType pt, double dir, double conf, int image_element_type)
          {
            setValues(pt, dir, conf, pt, dir, conf, image_element_type);
           };

        EdgePoint(PointType pt, double dir, double conf, PointType s_pt, double s_dir, double s_conf, int image_element_type)
          {
            setValues(pt, dir, conf, s_pt, s_dir, s_conf, image_element_type);
           };

        EdgePoint(const EdgePoint &old)
          {
            setValues(old._pixelPoint, old._pixelDirection, old._pixelConfidence, old._subPixelPoint, old._subPixelDirection, old._subPixelConfidence, old._imageElementType);
           };


        EdgePoint& operator=(const EdgePoint &old)
          {
            if (this!=&old)
              {
                setValues(old._pixelPoint, old._pixelDirection, old._pixelConfidence, old._subPixelPoint, old._subPixelDirection, old._subPixelConfidence, old._imageElementType);
               }
            return *this;
           };


        inline void setValues(const PointType &pixel_pt, const double &pixel_dir, const double &pixel_conf, const PointType &sub_pt, const double &sub_dir, const double &sub_conf, int image_element_type)
          {
            _pixelPoint          = pixel_pt;
            _pixelDirection      = pixel_dir;
            _pixelConfidence     = pixel_conf;
            
            
            _subPixelPoint       = sub_pt;
            _subPixelDirection   = sub_dir;
            _subPixelConfidence  = sub_conf;

            _imageElementType    = image_element_type;
           }


//
//        inline void setValues(const PointType &pixel_pt, const double &pixel_dir, const double &pixel_conf)
//          {
//            setValues(pixel_pt, pixel_dir, pixel_conf, pixel_pt, pixel_dir, pixel_conf);
//           }
//
        virtual ~EdgePoint(){};




        //bool operator<(const EdgePoint &other)
        //  {
        //    return (_subPixelConfidence<other._subPixelConfidence);
        //   }

        void setPixelConfidence(double conf)
          {
            _pixelConfidence = conf;
           }

        PointType getPixelPoint()      const
          {
            return _pixelPoint;
           }
        

        double getPixelDirection()     const
          {
            return _pixelDirection;
           }
       
        double getPixelConfidence()    const
          {
            return _pixelConfidence;
           }
        
        int getPixelX() const
          {
            return (int)_pixelPoint.getX();
           }
       
        int getPixelY() const
          {
            return (int)_pixelPoint.getY();
           }









        void setSubPixelPoint(PointType pt)
          {
            _subPixelPoint = pt;
           }

        void addToSubPixelPoint(PointType diff)
          {
            _subPixelPoint = _subPixelPoint+diff;
           }

        void addToSubPixelDirection(double diff)
          {
            _subPixelDirection +=diff;
           }

        void setSubPixelDirection(double dir)
          {
            _subPixelDirection = dir;
           }

        void setSubPixelConfidence(double conf)
          {
            _subPixelConfidence = conf;
           }

        double getSubPixelX()  const
          {
            return _subPixelPoint.x();
           }

        double getSubPixelY()  const
          {
            return _subPixelPoint.y();
           }

        double getSubPixelDirection()  const
          {
            return _subPixelDirection;
           }

        double getSubPixelConfidence() const
          {
            return _subPixelConfidence;
           }

        PointType getSubPixelPoint()   const
          {
            return _subPixelPoint;
           }


        void printValues(vcl_ostream& os) const
          {
            if (_imageElementType==IMAGE_ELEMENT_EDGE)
                os<<"EDGE :";
            else
                if (_imageElementType==IMAGE_ELEMENT_PLINE)
                    os<<"PLINE :";
                else
                    if (_imageElementType==IMAGE_ELEMENT_NLINE)
                        os<<"NLINE :";

            
            os<<" "<<_pixelPoint<<"   "<<_pixelDirection<<" "<<_pixelConfidence<<"  "<<_subPixelPoint<<"  "<<_subPixelDirection<<" "<<_subPixelConfidence<<vcl_endl;
           }


        int getImageElementType() const
          {
            return _imageElementType;
           }


        vcl_string getDebugInformation()  const
          {
            vcl_ostringstream info;

            info<<" Pix Pt="<<_pixelPoint<<" Dir= "<<_pixelDirection*(180.0/vnl_math::pi)<<" Conf= "<<_pixelConfidence<<vcl_endl;
            info<<" Sub Pt="<<_subPixelPoint<<" Dir= "<<_subPixelDirection*(180.0/vnl_math::pi)<<" Conf= "<<_subPixelConfidence<<vcl_endl;
            return info.str();
           }
   };


//
//ANL ==>Analytical.
//
/*class AnlObject
  {
    private:
        int _grayLevel;
        vector<Point2D<double> >  _outline;

    public:
        AnlObject(int gray_level)
          {
            _grayLevel=gray_level;
           }
        
        AnlObject(int gray_level, vector<Point2D<double> > &bnd):_grayLevel(gray_level)
          {
            for (unsigned int i=0;i<bnd.size();i++)
              {
                _outline.push_back(bnd[i]);
               }
           }
      
        AnlObject(const AnlObject& old):_grayLevel(old._grayLevel)
          {
            for (unsigned int i=0;i<old._outline.size();i++)
              {
                _outline.push_back(old._outline[i]);
               }
           }
       
        AnlObject& operator=(const AnlObject& old)
          {
            if (this!=&old)
              {
                _grayLevel = old._grayLevel;
                _outline.clear();
                for (unsigned int i=0;i<old._outline.size();i++)
                  {
                    _outline.push_back(old._outline[i]);
                   }
               }
            return *this;
           }
        
        void setGrayLevel(int gray_level)
          {
            _grayLevel = gray_level;
           }
   
   
        void push_back(Point2D<double> &pt)
          {
            _outline.push_back(pt);
           }
   
        void push_back(double x, double y)
          {
            _outline.push_back(Point2D<double> (x,y));
           }

        void printValues(ostream &os) const
          {
            cout<<_grayLevel<<": ";
            for (unsigned int i=0;i<_outline.size();i++)
              {
                cout<<setprecision(10)<<setw(15)<<_outline[i]<<"  ";
               }
            cout<<endl;
           }
        void clear()
          {
            _outline.clear();
           }

        int getGrayLevel() const
          {
            return _grayLevel;
           }


        
        int size() const
          {
            return _outline.size();
           }
        Point2D<double> & operator[](int i)
          {
            assert(i>=0);
            assert(i<(int)_outline.size());
            return _outline[i];
           }

        vector<Point2D<double> >  getOutline() const
          {
            return _outline;
           }
   };
*/

inline vcl_ostream& operator<<(vcl_ostream &os, const EdgePoint &pt)
  {
    pt.printValues(os); 
    return os;
   }

/*
inline ostream& operator<<(ostream &os, const AnlObject &obj)
  {
    obj.printValues(os); 
    return os;
   }
*/

class EdgeWriter
  {
    public:
        template<typename OutputDS>
                void operator()(const OutputDS &edges, vcl_string filename, vcl_string header)
                  {
                    vcl_cerr<<" Creating Edge File = "<<filename<<vcl_endl;
                    vcl_ofstream output_file(filename.c_str());
                    assert(output_file.is_open());

                    output_file<<"# EDGE_MAP : "<<header<<vcl_endl;
                    output_file<<"# Version     :    4 "<<vcl_endl;
                    output_file<<"# (Add [0.5, 0.5] to both pixel and sub-pixel for better display).  "<<vcl_endl;
                    output_file<<"# Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf "<<vcl_endl;
                    output_file<<" EDGE_COUNT="<<edges.size()<<vcl_endl;
                    output_file<<vcl_endl<<vcl_endl;

                    for (unsigned int i=0;i<edges.size();i++)
                      {
                        edges[i].printValues(output_file);
                       }

                    output_file.close();
                   };
   };

/*
template<typename EdgeElementType>
class EdgFileParser
  {
    private:
      
    public:
        EdgFileParser()
          {
           };
        
        void operator()(vector<EdgeElementType> &edges, string filename)
          {
            double pixel_pt_x = 0, pixel_pt_y = 0, pixel_dir = 0, pixel_conf = 0;
            double sub_pt_x   = 0, sub_pt_y   = 0, sub_dir   = 0, sub_conf   = 0;
            int line_num=0;
            
            string buffer;
            ifstream fp;
           
            string delimiters="[], :";
            SepFromString separators(delimiters);
            vector<string> tokens;

            
            fp.open(filename.c_str(),ios::in);
            if (fp.fail())
              {
                cout<<" Error : Unable to open "<<filename<<endl;
                return;
               }
            
            getline(fp, buffer);

            //Chop out the initial stuff 
            while((!fp.eof())&&((buffer.length()<3)||(buffer[0]=='#')||(buffer[1]=='E')))
              {
                getline(fp, buffer);
                line_num++;
               }

            int offset=0;
            int image_element_type=0;
            //Read in the stuff;
            while(!fp.eof())
              {
                tokens.clear();
                
                StringTokenizer<SepFromString>::Tokenize(tokens, buffer, separators);

                if (tokens.size()==8)
                    offset=0;
                if (tokens.size()==9)
                    offset=1;
                
                if (!((tokens.size()==8)||(tokens.size()==9)))
                  {
                    cerr<<" Error in File "<<filename<<" On Line "<<line_num<<". Tokens Obtained ="<<tokens.size()<<" Expected ="<<8<<endl;
                    break;
                   }
                else
                  {
                    pixel_pt_x = strtod(tokens[0+offset].c_str(),0);
                    pixel_pt_y = strtod(tokens[1+offset].c_str(),0);
                    pixel_dir  = strtod(tokens[2+offset].c_str(),0);
                    pixel_conf = strtod(tokens[3+offset].c_str(),0);

                    sub_pt_x   = strtod(tokens[4+offset].c_str(),0);
                    sub_pt_y   = strtod(tokens[5+offset].c_str(),0);
                    sub_dir    = strtod(tokens[6+offset].c_str(),0);
                    sub_conf   = strtod(tokens[7+offset].c_str(),0);
                    
                   
                    if (tokens.size()==9)
                      {
                        if (tokens[0]=="EDGE")    
                          {
                            image_element_type=IMAGE_ELEMENT_EDGE;
                           }
                        else
                          {
                            if (tokens[0]=="PLINE")
                                image_element_type=IMAGE_ELEMENT_PLINE;
                            else
                                if (tokens[0]=="NLINE")
                                    image_element_type=IMAGE_ELEMENT_NLINE;
                                else
                                  {
                                    cerr<<filename<<" ["<<line_num<<"] : Unknown Element Type ="<<tokens[0]<<endl;
                                    image_element_type=1;
                                   }
                           }

                       }
                    else
                        image_element_type=IMAGE_ELEMENT_EDGE;
                    
                    edges.push_back(EdgePoint(Point2D<double>(pixel_pt_x, pixel_pt_y), pixel_dir, pixel_conf, Point2D<double>(sub_pt_x, sub_pt_y),
                            sub_dir, sub_conf,image_element_type));
                   }
                getline(fp, buffer);
                line_num++;
               }

            fp.close();
           }
   
   };


class AnlFileParser
  {
    private:
      
    public:
        AnlFileParser()
          {
           };
        
        template<typename AnlObjectType>
                void operator()(vector<AnlObjectType> &objects, string filename)
                  {
                    int gray_value(0);
                    double x(0), y(0);
                    AnlObjectType temp(0);
                    

                    string buffer;
                    stringstream read;
                    ifstream fp;
                    
                    fp.open(filename.c_str(),ios::in);
                    if (fp.fail())
                      {
                        cout<<" Error : Unable to open "<<filename<<endl;
                        return;
                       }
                    
                    
                    
                    string delimiters=":,( )";
                    //delimiters.push_back(':');
                    //delimiters.push_back('(');
                    //delimiters.push_back(',');
                    //delimiters.push_back(')');
                    //delimiters.push_back(' ');
                    
                    SepFromString separators(delimiters);
                    vector<string> tokens;
                    while(!fp.eof())
                      {
                        getline(fp, buffer);
                        read.str("");
                        read.clear();
                        temp.clear();
                        
                        tokens.clear();
                        StringTokenizer<SepFromString>::Tokenize(tokens, buffer, separators);
                        
                        //cout<<" Number of Tokens ="<<tokens.size()<<endl;
                        
                        if (tokens.size()==0)
                            break;
                        if ((int)(tokens.size())%2==0)
                            cout<<" Warning: "<<filename<<" Seems to be missing one co-ordinate"<<endl;
                        
                        read<<tokens[0];
                        read>>gray_value;
                        temp.setGrayLevel(gray_value);
                        

                        
                        for (unsigned int i=1; i<tokens.size();i++)
                          {
                            read.str("");
                            read.clear();
                            
                            read<<tokens[i];
                            
                            if ((i-1)%2==0)
                              {
                                read>>x;
                               }
                            else
                              {
                                read>>y;
                                temp.push_back((int)x, (int)y);
                               }
                           }
                        
                        objects.push_back(temp);
                       }
                    
                    fp.close();
                   }
   
   };

*/

#endif     /* EDGE_DETECTION_H */
