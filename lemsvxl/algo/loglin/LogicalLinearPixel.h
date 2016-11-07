#ifndef LOGICAL_LINEAR_PIXEL_H
#define LOGICAL_LINEAR_PIXEL_H

#include <vcl_vector.h>
#include "LogicalLinearFilters.h"


/**
 * Class for pixel level LL EDGE/PLINE/NLINE Detection.
 * Intended to replace Lee Iverson's code.
 * */

typedef SubPixelLLOptions   PixelLLOptions;
class RGBImage;

template<int HalfCount , typename ImageType=vil_image_view<unsigned char> >
class LogicalLinearPixelEdgeDetector
  {
    private:
        PixelLLOptions _options;
        ImageType      _image;   //Do NOT Delete.

    public:
        LogicalLinearPixelEdgeDetector(ImageType img, const PixelLLOptions& ops):_options(ops)
          {
            _image   = img;
           };

        ~LogicalLinearPixelEdgeDetector(){};

        template<typename OutputDS>
                void getPixelEdges(OutputDS &edges, bool bool_nms)
                  {
                    typedef LLPixelEdgeFilter<HalfCount> FilterUtilType;
                    FilterUtilType *dummy=0;

                    getFromImage(edges, dummy , bool_nms);
                   };

        template<typename OutputDS>
                void getPixelPLines(OutputDS &plines)
                  {
                    typedef LLPixelPLineFilter<HalfCount> FilterUtilType;
                    FilterUtilType *dummy=0;

                    getFromImage(plines, dummy , false);
                   };

        template<typename OutputDS>
                void getPixelNLines(OutputDS &nlines)
                  {
                    vcl_cout<<" Getting NLINES "<<vcl_endl;
                    typedef LLPixelNLineFilter<HalfCount> FilterUtilType;
                    FilterUtilType *dummy=0;

                    getFromImage(nlines, dummy , false );
                   }



        struct NMS_VALUES {
          double confidence_;
          double angle_;
          bool delete_;
        };
        
        //
        //Get EDGES/PLINES/NLINES whatever.
        //The process is the same only the filter is different.
        //The dummy FilterType argument is needed to satisify Windows.
        //
        template<typename FilterUtilType, typename OutputDS>
                void  getFromImage(OutputDS &vals, FilterUtilType* dummy , bool bool_nms) 
                  {
                    typedef typename FilterUtilType::type                      FilterType;
                    typedef typename FilterType::FilterFunctionType            FilterFunctionType;
                    typedef typename FilterFunctionType::LogicalLinearTagType  FilterTagType;


                    assert((bool)_image);
                    FilterFunctionType filter_func(_options,0,0,0);
                    FilterType         filter(_options.getFilterRows(), _options.getFilterCols(),  _options.getStepSize(),filter_func);
                    
                    int width  = _image.ni();
                    int height = _image.nj();
                    int ndirs  = _options.getNDirs();
                    assert(ndirs>0);

                    int size   = width*height;

                    double theta = 0;

                    double *convolved_values_theta          = new double [ndirs*size];
                    double *convolved_values_theta_plus_180 = new double [ndirs*size];


                    vcl_cout<<" Convolving   :";
                    vcl_cout.flush();
                    for (int i=0;i<ndirs;i++)
                      {
                        vcl_cout<<".."<<i<<"..";

                        vcl_cout.flush();
                        theta    = ((i*vnl_math::pi)/(ndirs*1.0));
                        filter_func.setParams(theta,0,0);
                        filter.setFunction(filter_func);

                        convolve(convolved_values_theta+i*size, convolved_values_theta_plus_180+i*size, filter , dummy);
                       }
                    vcl_cout<<" Done" <<vcl_endl;


                    //double *max_conf_theta          = max_element(convolved_values_theta,          convolved_values_theta+ndirs*size);
                    //double *max_conf_theta_plus_180 = max_element(convolved_values_theta_plus_180, convolved_values_theta_plus_180+ndirs*size);

                    //double max_conf=max(*max_conf_theta,*max_conf_theta_plus_180);


                    //double thresh_val= max_conf*(1-_options.getThreshold());

                    double thresh_val=_options.getThreshold();

                    if( bool_nms && FilterTagType::getImageElementType()==IMAGE_ELEMENT_EDGE && ndirs > 1 ) {
                      vcl_cout << " Thresholding with NMS :";

                      NMS_VALUES * nms_values = new NMS_VALUES[ ndirs * size * 2 ];              
                      
                      for( int j = 0 ; j<ndirs; j++ )
                      {
                        vcl_cout << ".." << j << "..";

                        theta    = ((j*vnl_math::pi)/(ndirs*1.0));
                        
                        nms_create(convolved_values_theta+j*size,          
                                      nms_values+j*size, 
                                      theta);
                        nms_create(convolved_values_theta_plus_180+j*size, 
                                      nms_values+(j+ndirs)*size, 
                                      theta+vnl_math::pi);  
                      }
                      nms_at_pixel( nms_values );
                      nms_across_pixel( nms_values );
                      nms_threshold( nms_values , thresh_val );
                      convert_nms_to_output( nms_values , vals , FilterTagType::getImageElementType() );
                      vcl_cout<<" Done" <<vcl_endl;

                      delete [] nms_values;
                      
                    } else {
                      vcl_cout<<" Thresholding :"; 
                      for (int j=0;j<ndirs;j++)
                      {
                        vcl_cout<<".."<<j<<"..";

                        theta    = ((j*vnl_math::pi)/(ndirs*1.0));
                        threshold(convolved_values_theta+j*size,          vals, theta,      thresh_val, FilterTagType::getImageElementType());
                        if (FilterTagType::getImageElementType()==IMAGE_ELEMENT_EDGE)
                          threshold(convolved_values_theta_plus_180+j*size, vals, theta+vnl_math::pi, thresh_val, FilterTagType::getImageElementType());
                      }
                      vcl_cout<<" Done" <<vcl_endl;

                    }
                    
                    delete [] convolved_values_theta;
                    delete [] convolved_values_theta_plus_180;
                   }


        template<typename FilterUtilType>
                void convolve(double *convolved_values_theta, double *convolved_values_theta_plus_180, typename FilterUtilType::type &filter , FilterUtilType* dummy)
                  { 
                    assert((bool)_image);
                    typedef typename FilterUtilType::type::LogicalLinearTagType TagType;
                    ConvProductSumComputer<LLTraits<FilterUtilType> >  conv_product_sum(_image, filter, _options);


                    int width  = _image.ni();
                    int height = _image.nj();


                    assert(convolved_values_theta          != 0);
                    assert(convolved_values_theta_plus_180 != 0);

                    int pos;

                    for (int i=0;i<height;++i)
                      {
                        pos = i*width;
                        for (int j=0;j<width;++j)
                          {
                            conv_product_sum(_image, filter, i,j,convolved_values_theta[pos] ,convolved_values_theta_plus_180[pos]) ; 
                            pos++;
                           }
                       }
                   }



        template<typename OutputDS>
          void threshold(double *convolved_values, OutputDS &vals, double dir, double thresh_val, int image_element_type)
          {
            //vcl_cout<<" Threshold ="<<thresh_val<<vcl_endl;
            typedef typename OutputDS::value_type  value_type;

            assert((bool)_image);

            int width  = _image.ni();
            int height = _image.nj();
            assert(convolved_values!=0);

            for (int i=0;i<height;i++)
            {
              for (int j=0;j<width;j++)
              {
                if (convolved_values[i*width+j]>=thresh_val)
                {
                  vals.push_back(value_type(Point2D<double>(j,i), dir, convolved_values[i*width+j],image_element_type));
                }
              }
            }
          };

        void nms_threshold(NMS_VALUES* nms_values, double thresh_val)
        {
          assert((bool)_image);

          int width  = _image.ni();
          int height = _image.nj();
          int ndirs  = _options.getNDirs();
          int size = width * height; 
          
          for( int n = 0 ; n < ndirs * 2  ; n++ ) {
            for( int i = 0 ; i < height ; i++ ) {
              for( int j = 0 ; j < width ; j++ ) {
                if( nms_values[n*size + i*width+j].confidence_ < thresh_val ) {
                  nms_values[n*size + i*width+j].delete_ = true;
                }
              }
            }
          }
        };

        void nms_create(double *convolved_values, NMS_VALUES* nms_values, double dir)
        {
          assert((bool)_image);

          int width  = _image.ni();
          int height = _image.nj();
          assert(convolved_values!=0);

          for (int i=0;i<height;i++)
          {
            for (int j=0;j<width;j++)
            {
              if( convolved_values[ i * width + j ] > 0 ) {   
                nms_values[i*width+j].confidence_ = convolved_values[i*width+j];
              } else {
                nms_values[ i * width + j ].confidence_ = 0;
              }
              
              nms_values[i*width+j].angle_ = dir;
              nms_values[i*width+j].delete_ = false;
            }
          }
        };
        
        void nms_at_pixel( NMS_VALUES* nms_values ) 
        {
          int width = _image.ni();
          int height = _image.nj();
          int size = width * height; 
          int n = 0;
          int ndirs  = _options.getNDirs();
          //0th case
          for( int i = 0 ; i < height ; i++ ) {
            for( int j = 0 ; j < width ; j++ ) {
              if( nms_values[ n*size + i*width + j ].confidence_ < nms_values[ (ndirs*2-1)*size + i*width + j ].confidence_ ||
                  nms_values[ n*size + i*width + j ].confidence_ < nms_values[ (n+1)*size + i*width + j ].confidence_ ) {
                nms_values[ n*size + i*width + j ].delete_ = true;
              }
            }
          }

          // general         
          for( n = 1 ; n < ndirs * 2 - 1 ; n++ )
          {
            for( int i = 0 ; i < height ; i++ ) {
              for( int j = 0 ; j < width ; j++ ) {
                if( nms_values[ n*size + i*width + j ].confidence_ < nms_values[ (n-1)*size + i*width + j ].confidence_ ||
                    nms_values[ n*size + i*width + j ].confidence_ < nms_values[ (n+1)*size + i*width + j ].confidence_ ) {
                  nms_values[ n*size + i*width + j ].delete_ = true;
                }
              }
            }
          }

          n = ndirs*2 - 1;
          // n-1th case
          for( int i = 0 ; i < height ; i++ ) {
            for( int j = 0 ; j < width ; j++ ) {
              if( nms_values[ n*size + i*width + j ].confidence_ < nms_values[ (n-1)*size + i*width + j ].confidence_ ||
                  nms_values[ n*size + i*width + j ].confidence_ < nms_values[ i*width + j ].confidence_ ) {
                nms_values[ n*size + i*width + j ].delete_ = true;
              }
            }
          }
        };

        template<typename OutputDS>
          void convert_nms_to_output( NMS_VALUES* nms_values , OutputDS & vals , int image_element_type ) 
          {

            typedef typename OutputDS::value_type  value_type;

            int width = _image.ni();
            int height = _image.nj();
            int size = width * height;
            int ndirs  = _options.getNDirs();
            for( int n = 0 ; n < ndirs*2  ; n++ ) {
              for( int i = 0 ; i < height ; i++ ) {
                for( int j = 0 ; j < width ; j++ ) {
                  if( nms_values[n*size + i*width + j].confidence_ > 0 && !nms_values[n*size + i*width + j].delete_ ) {
                    vals.push_back(value_type(Point2D<double>(j,i), 
                                              nms_values[n*size + i*width + j].angle_, 
                                              nms_values[n*size + i*width + j].confidence_,
                                              image_element_type));
                  }
                }
              }
            }

          };



        void nms_across_pixel( NMS_VALUES* nms_values ) 
        {
          int width = _image.ni();
          int height = _image.nj();
          int size = width * height; 
          int ndirs  = _options.getNDirs();

          // general         
          for( int n = 0 ; n < ndirs * 2  ; n++ ) {
            for( int i = 0 ; i < height ; i++ ) {
              for( int j = 0 ; j < width ; j++ ) {
                if( !nms_values[ n*size + i*width + j ].delete_ ) {
                 nms_check_pixel( nms_values , j , i , n ); 
                }
              }
            }
          }
        };


        // MULTIPLE things could be done to make this function far more
        // efficient... for example, in the <, > comparisons, divisions could be
        // changed into multiplications on the other side, values don't have to
        // be looked up multiple times, vnl_math::pi could be looked up for
        // fractional values instead of divided.... but for now, things will be
        // left as-is to make for more readable code. i.e. when you see a
        // division by 2 of a sum of 2 things, you immediately understand that
        // it's an average.
        
        void nms_check_pixel( NMS_VALUES* nms_values , int j , int i , int n )
        { 
          int width = _image.ni();
          int height = _image.nj();
          int size = width * height; 
          int ndirs  = _options.getNDirs();
          
          if( i == 0 || j == 0 || i == height - 1 || j == width - 1 ) {
            return;
          }
          
          double phi = nms_values[ n*size + i*width + j ].angle_ - ( vnl_math::pi / 2.0 );
          double sin_of_phi = vcl_sin( phi );
          double cos_of_phi = vcl_cos( phi );
          
          double abs_sin_of_phi = vcl_fabs( sin_of_phi );
          double abs_cos_of_phi = vcl_fabs( cos_of_phi );
          
          if( abs_sin_of_phi < TINY_VALUE ) {

            // do the case of left and right value comparison over all n
            // directions
            
            for( int a = 0 ; a < ndirs * 2 ; a++ ) {
              if( nms_values[ n*size + i*width + j ].confidence_ < nms_values[ a*size + i*width + (j-1) ].confidence_ ||
                  nms_values[ n*size + i*width + j ].confidence_ < nms_values[ a*size + i*width + (j+1) ].confidence_ ) {
                nms_values[ n*size + i*width + j ].delete_ = true;
                return;
              }
            } 
            return;
            
          } else if( abs_cos_of_phi < TINY_VALUE ) {

            // do the case of up and down value comparison over all n directions

            for( int a = 0 ; a < ndirs * 2 ; a++ ) {
              if( nms_values[ n*size + i*width + j ].confidence_ < nms_values[ a*size + (i-1)*width + j ].confidence_ ||
                  nms_values[ n*size + i*width + j ].confidence_ < nms_values[ a*size + (i+1)*width + j ].confidence_ ) {
                nms_values[ n*size + i*width + j ].delete_ = true;
                return;
              }
            }
            
            return;

          } else if( vcl_fabs( abs_sin_of_phi - vcl_cos( vnl_math::pi / 4.0 ) ) < TINY_VALUE ) {

            // do the case of diagonal comparison over all n directions -- must
            // check to see which diagonal is right.

            if( sin_of_phi * cos_of_phi > 0 ) {
            // check to the upper right and bottom left
           
              for( int a = 0 ; a < ndirs * 2 ; a++ ) {
                for( int b = 0 ; b < ndirs * 2 ; b++ ) {
                  if( nms_values[ n*size + i*width + j ].confidence_ < 
                      (nms_values[ a*size + (i)*width + (j+1) ].confidence_ + nms_values[ b*size + (i+1)*width + j ].confidence_)/2.0 ||
                      nms_values[ n*size + i*width + j ].confidence_ < 
                      (nms_values[ a*size + (i)*width + (j-1) ].confidence_ + nms_values[ b*size + (i-1)*width + j ].confidence_)/2.0 ) {
                    nms_values[ n*size + i*width + j ].delete_ = true;
                    return;
                  }
                }
              }
            } else {
            // check to the upper left and bottom right
             
              for( int a = 0 ; a < ndirs * 2 ; a++ ) {
                for( int b = 0 ; b < ndirs * 2 ; b++ ) {
                  if( nms_values[ n*size + i*width + j ].confidence_ < 
                      (nms_values[ a*size + (i)*width + (j+1) ].confidence_ + nms_values[ b*size + (i-1)*width + j].confidence_)/2.0  ||
                      nms_values[ n*size + i*width + j ].confidence_ < 
                      (nms_values[ a*size + (i)*width + (j-1) ].confidence_ + nms_values[ b*size + (i+1)*width + j].confidence_)/2.0 ) {
                    nms_values[ n*size + i*width + j ].delete_ = true;
                    return;
                  }
                }
              }
            }


            return;

          } else {
            // generic check that works for all other angles

            //quad 1
            if( sin_of_phi > 0 && cos_of_phi > 0 ) {
              if( sin_of_phi > cos_of_phi ) {

                double intersection = cos_of_phi + ( 1 - sin_of_phi ) * ( cos_of_phi / sin_of_phi );
                for( int a = 0 ; a < ndirs * 2 ; a++ ) {
                  for( int b = 0 ; b < ndirs * 2 ; b++ ) {
                    if( nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i-1)*width + j + 1 ].confidence_ 
                        + (1 - intersection) * nms_values[ b*size + (i-1)*width + j ].confidence_ ||
                        nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i+1)*width + j - 1 ].confidence_
                        + (1 - intersection) * nms_values[ b*size + (i+1)*width + j ].confidence_ ) {
                      nms_values[ n*size + i*width + j ].delete_ = true;
                    }
                  }
                }
              } else {

                double intersection = ( sin_of_phi / cos_of_phi ) * ( 1 - cos_of_phi ) + sin_of_phi;
                for( int a = 0 ; a < ndirs * 2 ; a++ ) {
                  for( int b = 0 ; b < ndirs * 2 ; b++ ) {
                    if( nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i-1)*width + j + 1 ].confidence_ 
                        + (1 - intersection) * nms_values[ b*size + i*width + j+1 ].confidence_ ||
                        nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i+1)*width + j - 1 ].confidence_
                        + (1 - intersection) * nms_values[ b*size + i*width + j - 1 ].confidence_ ) {
                      nms_values[ n*size + i*width + j ].delete_ = true;
                    }
                  }
                }
              }
            }
            // quad 2
            else if( sin_of_phi >0 && cos_of_phi < 0 ) {
              if( sin_of_phi > abs_cos_of_phi ) {

                double intersection = abs_cos_of_phi + ( 1 - sin_of_phi ) * ( abs_cos_of_phi / sin_of_phi );
                for( int a = 0 ; a < ndirs * 2 ; a++ ) {               
                  for( int b = 0 ; b < ndirs * 2 ; b++ ) {
                    if( nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i-1)*width + j - 1 ].confidence_ 
                        + (1 - intersection) * nms_values[ b*size + (i-1)*width + j ].confidence_ ||
                        nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i+1)*width + j + 1 ].confidence_
                        + (1 - intersection) * nms_values[ b*size + (i+1)*width + j ].confidence_ ) {
                      nms_values[ n*size + i*width + j ].delete_ = true;
                    }
                  }
                } 
              } else {

                double intersection = ( sin_of_phi / abs_cos_of_phi ) * ( 1 - abs_cos_of_phi ) + sin_of_phi;
                for( int a = 0 ; a < ndirs * 2 ; a++ ) {               
                  for( int b = 0 ; b < ndirs * 2 ; b++ ) {
                    if( nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i-1)*width + j - 1 ].confidence_ 
                        + (1 - intersection) * nms_values[ b*size + i*width + j-1 ].confidence_ ||
                        nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i+1)*width + j + 1 ].confidence_
                        + (1 - intersection) * nms_values[ b*size + i*width + j + 1 ].confidence_ ) {
                      nms_values[ n*size + i*width + j ].delete_ = true;
                    }
                  } 
                }
              }
            }
            // quad 3
            else if( sin_of_phi < 0 && cos_of_phi < 0 ) {
              if( abs_sin_of_phi > abs_cos_of_phi ) {

                double intersection = abs_cos_of_phi + ( 1 - abs_sin_of_phi ) * ( abs_cos_of_phi / abs_sin_of_phi );
                for( int a = 0 ; a < ndirs * 2 ; a++ ) {               
                  for( int b = 0 ; b < ndirs * 2 ; b++ ) {
                    if( nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i-1)*width + j + 1 ].confidence_ 
                        + (1 - intersection) * nms_values[ b*size + (i-1)*width + j ].confidence_ ||
                        nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i+1)*width + j - 1 ].confidence_
                        + (1 - intersection) * nms_values[ b*size + (i+1)*width + j ].confidence_ ) {
                      nms_values[ n*size + i*width + j ].delete_ = true;
                    }
                  }
                } 
              } else {

                double intersection = ( abs_sin_of_phi / abs_cos_of_phi ) * ( 1 - abs_cos_of_phi ) + abs_sin_of_phi;
                for( int a = 0 ; a < ndirs * 2 ; a++ ) {               
                  for( int b = 0 ; b < ndirs * 2 ; b++ ) {
                    if( nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i-1)*width + j + 1 ].confidence_ 
                        + (1 - intersection) * nms_values[ b*size + i*width + j+1 ].confidence_ ||
                          nms_values[ n*size + i*width + j ].confidence_ <
                          intersection * nms_values[ a*size + (i+1)*width + j - 1 ].confidence_
                          + (1 - intersection) * nms_values[ b*size + i*width + j - 1 ].confidence_ ) {
                        nms_values[ n*size + i*width + j ].delete_ = true;
                      }
                    }
                }
              }
            }
            // quad 4
            else {
              if( abs_sin_of_phi > cos_of_phi ) {

                double intersection = abs_cos_of_phi + ( 1 - abs_sin_of_phi ) * ( abs_cos_of_phi / abs_sin_of_phi );
                for( int a = 0 ; a < ndirs * 2 ; a++ ) {               
                  for( int b = 0 ; b < ndirs * 2 ; b++ ) {

                    if( nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i-1)*width + j - 1 ].confidence_ 
                        + (1 - intersection) * nms_values[ b*size + (i-1)*width + j ].confidence_ ||
                        nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i+1)*width + j + 1 ].confidence_
                        + (1 - intersection) * nms_values[ b*size + (i+1)*width + j ].confidence_ ) {
                      nms_values[ n*size + i*width + j ].delete_ = true;
                    }
                  }
                }

              } else {

                double intersection = ( abs_sin_of_phi / abs_cos_of_phi ) * ( 1 - abs_cos_of_phi ) + abs_sin_of_phi;
                for( int a = 0 ; a < ndirs * 2 ; a++ ) {               
                  for( int b = 0 ; b < ndirs * 2 ; b++ ) {

                    if( nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i-1)*width + j - 1 ].confidence_ 
                        + (1 - intersection) * nms_values[ b*size + i*width + j-1 ].confidence_ ||
                        nms_values[ n*size + i*width + j ].confidence_ <
                        intersection * nms_values[ a*size + (i+1)*width + j + 1 ].confidence_
                        + (1 - intersection) * nms_values[ b*size + i*width + j + 1 ].confidence_ ) {
                      nms_values[ n*size + i*width + j ].delete_ = true;
                    }
                  }
                }
              }
            }

            return;
          }

        };

  };








#endif      /* LOGICAL_LINEAR_PIXEL_H */
