#include <wx/wx.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>

#include <vcl_map.h>
#include <vcl_string.h>

#include "basegui_color.h"
#include "basegui_style.h"

static ColorList GlobalColorList;

void lookup_standard_color(StandardColor c, float &r, float &g, float &b)
  {
    ColorList::getGlobalColorList()->lookupStandardColor(c, r,g,b);
   }



void ColorList::lookupStandardColor(StandardColor color, float &r, float &g, float &b)
  {
    vcl_map<StandardColor, vcl_string>::iterator it;

    it = _colorList.find(color);

    if (it==_colorList.end())
      {
        r=0;
        g=0;
        b=0;
        return;
       }
    else
      {
        wxColour *color_ptr = wxTheColourDatabase->FindColour((*it).second.c_str());
    
        if (color_ptr)
          {
            r = color_ptr->Red()/255.0;
            g = color_ptr->Green()/255.0;
            b = color_ptr->Blue()/255.0;
           }
        else
          {
            r=g=b=0;
           }
        return;
       }
   };




ColorList* ColorList::getGlobalColorList()
  {
    //if (!GlobalColorList)
    //    GlobalColorList = new ColorList();
    return &GlobalColorList;
   }



StyleSet ColorAllocator::operator()()
  {
    assert(_blockColors.size()<(MAX_COLORS));
    _count++;
    if (_count>=MAX_COLORS)
        _count =0;  

    while(vcl_find(_blockColors.begin(), _blockColors.end(),StandardColor(_count))!=_blockColors.end())
      {
        _count++;
        if (_count>=MAX_COLORS)
            _count =0;  
       }

    return Color(StandardColor(_count));
   };


