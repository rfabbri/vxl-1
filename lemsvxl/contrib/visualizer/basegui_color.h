#ifndef COLOR_H
#define COLOR_H

#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>




/***
 * @brief The set of ``standard colors'' used in the plugins.
 *
 * @warning Do NOT assign values to the enums. Use the defaults.
 * (Have a look at class ColorAllocator to understand why.
 * */
enum StandardColor
  {
    AQUAMARINE,         
    BLACK,              
    BLUE,               
    BLUE_VIOLET,        
    BROWN,              
    CADET_BLUE,         
    CORAL,              
    CORNFLOWER_BLUE,    
    CYAN,               
    DARK_GRAY,          
    DARK_GREEN,         
    DARK_OLIVE_GREEN,   
    DARK_ORCHID,        
    DARK_SLATE_BLUE,    
    DARK_SLATE_GRAY ,   
    DARK_TURQUOISE,     
    DIM_GRAY,           
    FIREBRICK,          
    FOREST_GREEN,       
    GOLD,               
    GOLDENROD,          
    GRAY,               
    GREEN,              
    GREEN_YELLOW,       
    INDIAN_RED,         
    KHAKI,              
    LIGHT_BLUE,         
    LIGHT_GRAY,         
    LIGHT_STEEL_BLUE,   
    LIME_GREEN,         
    MAGENTA,            
    MAROON,             
    MEDIUM_AQUAMARINE,  
    MEDIUM_BLUE,        
    MEDIUM_FOREST_GREEN,
    MEDIUM_GOLDENROD,   
    MEDIUM_ORCHID,      
    MEDIUM_SEA_GREEN,   
    MEDIUM_SLATE_BLUE,  
    MEDIUM_SPRING_GREEN,
    MEDIUM_TURQUOISE,   
    MEDIUM_VIOLET_RED,  
    MIDNIGHT_BLUE,      
    NAVY,               
    ORANGE,             
    ORANGE_RED,         
    ORCHID,             
    PALE_GREEN,         
    PINK,               
    PLUM,               
    PURPLE,             
    RED,                
    SALMON,             
    SEA_GREEN,          
    SIENNA,             
    SKY_BLUE,           
    SLATE_BLUE,         
    SPRING_GREEN,       
    STEEL_BLUE,         
    TAN,                
    THISTLE,            
    TURQUOISE,          
    VIOLET,             
    VIOLET_RED,         
    WHEAT,              
    WHITE,              
    YELLOW,             
    YELLOW_GREEN,
    MAX_COLORS
   };

class ColorList
  {
private:
        vcl_map<StandardColor, vcl_string>  _colorList;

    public:
        ColorList()
          {
            _colorList[AQUAMARINE]          =  "AQUAMARINE";
            _colorList[BLACK]               =  "BLACK";
            _colorList[BLUE]                =  "BLUE";
            _colorList[BLUE_VIOLET]         =  "BLUE VIOLET";
            _colorList[BROWN]               =  "BROWN";
            _colorList[CADET_BLUE]          =  "CADET BLUE";
            _colorList[CORAL]               =  "CORAL";
            _colorList[CORNFLOWER_BLUE]     =  "CORNFLOWER BLUE";
            _colorList[CYAN]                =  "CYAN";
            _colorList[DARK_GRAY]           =  "DARK GREY";
            _colorList[DARK_GREEN]          =  "DARK GREEN";
            _colorList[DARK_OLIVE_GREEN]    =  "DARK OLIVE GREEN";
            _colorList[DARK_ORCHID]         =  "DARK ORCHID";
            _colorList[DARK_SLATE_BLUE]     =  "DARK SLATE BLUE";
            _colorList[DARK_SLATE_GRAY ]    =  "DARK SLATE GREY";
            _colorList[DARK_TURQUOISE]      =  "DARK TURQUOISE";
            _colorList[DIM_GRAY]            =  "DIM GREY";
            _colorList[FIREBRICK]           =  "FIREBRICK";
            _colorList[FOREST_GREEN]        =  "FOREST GREEN";
            _colorList[GOLD]                =  "GOLD";
            _colorList[GOLDENROD]           =  "GOLDENROD";
            _colorList[GRAY]                =  "GREY";
            _colorList[GREEN]               =  "GREEN";
            _colorList[GREEN_YELLOW]        =  "GREEN YELLOW";
            _colorList[INDIAN_RED]          =  "INDIAN RED";
            _colorList[KHAKI]               =  "KHAKI";
            _colorList[LIGHT_BLUE]          =  "LIGHT BLUE";
            _colorList[LIGHT_GRAY]          =  "LIGHT GREY";
            _colorList[LIGHT_STEEL_BLUE]    =  "LIGHT STEEL BLUE";
            _colorList[LIME_GREEN]          =  "LIME GREEN";
            _colorList[MAGENTA]             =  "MAGENTA";
            _colorList[MAROON]              =  "MAROON";
            _colorList[MEDIUM_AQUAMARINE]   =  "MEDIUM AQUAMARINE";
            _colorList[MEDIUM_BLUE]         =  "MEDIUM BLUE";
            _colorList[MEDIUM_FOREST_GREEN] =  "MEDIUM FOREST GREEN";
            _colorList[MEDIUM_GOLDENROD]    =  "MEDIUM GOLDENROD";
            _colorList[MEDIUM_ORCHID]       =  "MEDIUM ORCHID";
            _colorList[MEDIUM_SEA_GREEN]    =  "MEDIUM SEA GREEN";
            _colorList[MEDIUM_SLATE_BLUE]   =  "MEDIUM SLATE BLUE";
            _colorList[MEDIUM_SPRING_GREEN] =  "MEDIUM SPRING GREEN";
            _colorList[MEDIUM_TURQUOISE]    =  "MEDIUM TURQUOISE";
            _colorList[MEDIUM_VIOLET_RED]   =  "MEDIUM VIOLET RED";
            _colorList[MIDNIGHT_BLUE]       =  "MIDNIGHT BLUE";
            _colorList[NAVY]                =  "NAVY";
            _colorList[ORANGE]              =  "ORANGE";
            _colorList[ORANGE_RED]          =  "ORANGE RED";
            _colorList[ORCHID]              =  "ORCHID";
            _colorList[PALE_GREEN]          =  "PALE GREEN";
            _colorList[PINK]                =  "PINK";
            _colorList[PLUM]                =  "PLUM";
            _colorList[PURPLE]              =  "PURPLE";
            _colorList[RED]                 =  "RED";
            _colorList[SALMON]              =  "SALMON";
            _colorList[SEA_GREEN]           =  "SEA GREEN";
            _colorList[SIENNA]              =  "SIENNA";
            _colorList[SKY_BLUE]            =  "SKY BLUE";
            _colorList[SLATE_BLUE]          =  "SLATE BLUE";
            _colorList[SPRING_GREEN]        =  "SPRING GREEN";
            _colorList[STEEL_BLUE]          =  "STEEL BLUE";
            _colorList[TAN]                 =  "TAN";
            _colorList[THISTLE]             =  "THISTLE";
            _colorList[TURQUOISE]           =  "TURQUOISE";
            _colorList[VIOLET]              =  "VIOLET";
            _colorList[VIOLET_RED]          =  "VIOLET RED";
            _colorList[WHEAT]               =  "WHEAT";
            _colorList[WHITE]               =  "WHITE";
            _colorList[YELLOW]              =  "YELLOW";
            _colorList[YELLOW_GREEN]        =  "YELLOW GREEN";

           }
      
        void lookupStandardColor(StandardColor c, float &r, float &g, float &b);
         
        static ColorList* getGlobalColorList();
   };



void lookup_standard_color(StandardColor c, float &r, float &g, float &b);
// r, g, b are returned in the range 0..1 (a la OpenGL)


class StyleSet;
class ColorAllocator
  {
    private:
        int _count;
        vcl_vector <StandardColor > _blockColors;

    public:
        ColorAllocator()
          {
            _count = -1;
            block(WHITE);
           }
   
        StyleSet operator()();
  
        void block(StandardColor col)
          {
            if (col!=MAX_COLORS)
              {
                if(vcl_find(_blockColors.begin(), _blockColors.end(),col)==_blockColors.end())
                  {
                    _blockColors.push_back(col);
                   }
               }
            assert(_blockColors.size()<(int)MAX_COLORS-1);
           }

        bool unBlock(StandardColor col)
          {
            vcl_vector<StandardColor>::iterator it;
            
            it =vcl_find(_blockColors.begin(), _blockColors.end(),col);
            if (it==_blockColors.end())
                return false;
            
            _blockColors.erase(it);
            return true;
           }

        StandardColor color()
          {
            return (StandardColor(_count));
           }
   };


#if 0 
//Luminence = 0.3*R +0.59*G + 0.11*B.
class ColorLuminizer
  {
    private:
        int _count;
        double _rStep1;
        double _rStep2;
        double _rStep3;

        double _ratio1;
        double _ratio2;
        double _ratio3;

        int    _maxColors;
    public:
        ColorLuminizer(int cnt=0)
          {
            _count = cnt;
           
            _rStep1 = -30;
            _rStep2 = 20;
            _rStep3 = 40;
            
            _ratio1 = 0;
            _ratio2 = 0.5;
            _ratio3 = 100;

            _maxColors= 9;
           }
        
        vcl_vector<double> operator()(double val) 
          {
            switch(_count)
              {
                case(0):
                      {
                        return getNewColors(val, _rStep1, _ratio1);
                       }

                case(1):
                      {
                        return getNewColors(val, _rStep1, _ratio2);
                       }

                case(2):
                      {
                        return getNewColors(val, _rStep1, _ratio3);
                       }

                case(3):
                      {
                        return getNewColors(val, _rStep2, _ratio1);
                       }

                case(4):
                      {
                        return getNewColors(val, _rStep2, _ratio2);
                       }

                case(5):
                      {
                        return getNewColors(val, _rStep2, _ratio3);
                       }

                case(6):
                      {
                        return getNewColors(val, _rStep3, _ratio1);
                       }

                case(7):
                      {
                        return getNewColors(val, _rStep3, _ratio2);
                       }

                case(8):
                      {
                        return getNewColors(val, _rStep3, _ratio3);
                       }
                
                default:
                      {
                        //Should not get here.
                        assert(false);
                       }
               }

            //Should not get here.
            assert(false);
            vcl_vector<double> dummy;
            dummy.push_back(0);
            dummy.push_back(0);
            dummy.push_back(0);

            return dummy;
           };
                
        void setCount(int cnt)
          {
            _count = cnt;
            _count %=_maxColors;
           }

        vcl_vector<double> getNewColors(double pixel_val, double delta_r, double g_by_b_ratio)
          {
            vcl_vector<double> final_vals;

            double new_r=0, new_g=0, new_b=0;

            double delta_g=0, delta_b=0;

            new_r = pixel_val + delta_r;
            if (new_r<0)
                new_r = 0;
            if (new_r>255)
                new_r = 255;
           
            delta_b = (-0.3*delta_r)/((0.59*g_by_b_ratio)+0.11);
            delta_g = g_by_b_ratio*delta_b;

            new_g  = pixel_val+delta_g;
            if (new_g<0)
                new_g = 0;
            if (new_g>255)
                new_g = 255;

            new_b  = pixel_val+delta_b;
            if (new_b<0)
                new_b = 0;
            if (new_b>255)
                new_b = 255;

            final_vals.push_back(new_r);
            final_vals.push_back(new_g);
            final_vals.push_back(new_b);
            return final_vals;
           }


        void operator++()
          {
            _count++;
            _count %= _maxColors;
           }
   };
#endif



#endif   /* COLOR_H */
