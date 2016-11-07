/**************************************************************/
/*	Name: MingChing (mcchang)
//	File: bvis_curve_storage.h
//	Asgn: bvis
//	Date: Thu Aug 28 14:28:55 EDT 2003
***************************************************************/

#ifndef bvis_curve_tableau_header
#define bvis_curve_tableau_header

#include <vgui/vgui_tableau.h>
#include <curvematch/curve_dpmatch_2d.h>
#include <curvematch/curve_dpmatch_2d_sptr.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_utils.h>
#include <vcl_iostream.h>
#include <bvis/bvis_tool_sptr.h>
#include <bvis/bvis_tool.h>
#include <vgl/vgl_distance.h>

class bvis_curve_tableau : public vgui_tableau {
private:
	GLenum gl_mode;

protected:
	curve_2d_sptr curve_;

public:
	bvis_curve_tableau();
	virtual ~bvis_curve_tableau();
	
	virtual bool handle( const vgui_event & );
	
	void set_curve (curve_2d_sptr new_curve) {
		curve_ = new_curve;
	}
	
	void draw_select();
	
	void draw_render();
	
};

#endif
