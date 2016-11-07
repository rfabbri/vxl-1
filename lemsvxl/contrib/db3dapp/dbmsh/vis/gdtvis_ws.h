//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/gdtvis_ws.h
//  MingChing Chang
//  Aug. 09, 2005.

#ifndef _gdtvis_ws_h_
#define _gdtvis_ws_h_

#include <dbmsh3d/gdt/gdt_interval.h>
#include <dbmsh3d/gdt/gdt_manager.h>

class SoSeparator;
class SbColor;

SoSeparator* gdtws_draw_W_in_Qw (gdt_ws_manager* ws_manager, const SbColor color, 
                                 const int nSamples, const float lineWidth=3.0f);

SoSeparator* gdt_draw_snodes_vispt (gdt_ws_manager* ws_manager, 
                                    const float size);

#endif


