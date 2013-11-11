 // This is contrib/biotree/biov/biov_examiner_tableau.h
#ifndef biov_examiner_tableau_h_
#define biov_examiner_tableau_h_

//:
// \file
// \brief  Examiner viewer tableau for volume rendering
// \author Jason Mallios (jmallios@cs.brown.edu)
// \date   7/01/05
//
// \verbatim
//  Modifications
//    K. Kang - 6/23/06 Changed way of slidding working 
//              so that it slide smoothly with large scene graph 
//    K. Kang - (1) use second camera to show the static color map
//              (2) use cone as an arrow in color map to pick 
//              thresholds
//              (3) rewrite the event handler for moving the 
//              thresholding slider
//              (4) reverse the order of color map so that 0 indexed 
//              color appearing at left top corner of client window 
// \endverbatim

#include <vcl_iostream.h>

#include <bgui3d/bgui3d_fullviewer_tableau.h>
#include <biov/biov_examiner_tableau_sptr.h>
#include <biov/biov_transfer_function.h>
#include <VolumeViz/nodes/SoTransferFunction.h>
#include <Inventor/nodes/SoImage.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCone.h>

class SbSphereSheetProjector;
class SbRotation;
class SoTimerSensor;

enum Process {
SPIN=0,
DRAG,
PAN,
ZOOM,
IDLE,
SEEK
};

struct Log
{
int size;
SbVec2f pos1;
SbVec2f pos2;
SbVec2f pos3;
};

/* defined but not used
static void dummy()
{
  vcl_cerr << "Dummy function called\n";
}*/

/* defined but not used
static void quit()
{
  exit(0);
}*/

//:  Examiner viewer tableau for 3D scenes
class biov_examiner_tableau : public bgui3d_fullviewer_tableau
{
public:
  enum SeekDistance {
    SEEK_FAR,
    SEEK_HALF,
    SEEK_NEAR,
    SEEK_ZERO
    };
  //: Constructor
  biov_examiner_tableau(SoNode * scene_root = NULL, SoTransferFunction * n_transfunc = NULL);

  //: Destructor
  virtual ~biov_examiner_tableau();

  //: Handle vgui events
  virtual bool handle(const vgui_event& event);

  //: Return the type name of this tableau
  vcl_string type_name() const;

  //: Builds a popup menu
  virtual void get_popup(const vgui_popup_params&, vgui_menu &m);

  //: Render the scene graph (called on draw events)
  virtual bool render();

  //: Converts the 2D image plane point to 3D world coordinates (handy for SoCamera::pointAt())
  SbVec3f screenToWorld(const SbVec2f & screenpos);

  //: Pans the camera
  void pan(const SbVec2f & currpos, const SbVec2f & prevpos, const float aspect_ratio);

  //: Zooms the camera
  void zoom( float aspectratio );

  //: Spins the camera around the scene
  void spin(const SbVec2f& currpos, const SbVec2f &prevpos);

  //: Reorients the camera to the specified rotation
  void reorient_camera(const SbRotation & rot);

  //: Updates position log
  void update_log(SbVec2f pos);

  //: Resets position log
  void reset_log();

  //: Seeks for the object under a mouse click and zooms to it
  void seek_to_point(SbVec2s pos);

  //: What to do if events are idle, if enabled
  bool idle();

  //: Set the visibility of the axis cross
  void set_axis_visible(bool state);

  //: Return true if the axis cross is visible
  bool axis_visible() const;

  void set_seek_distance( SeekDistance seek );

  //void seeksensorCB(void * data, SoSensor * s);

  vgui_menu create_menus();

  //: Called by biov_transfer_function if a change is made
  void transfer_callback(const bool & remap, const int & n_min,
                         const int & n_max, const int & n_map);

protected:
  //: draw the axis
  void draw_axis_cross();

  //: Draw an arrow for the axis representation
  void draw_arrow();

  //: Populate the colormap array and set the image
  void loadColorMap();

  //: Given the current map, retrieve the string that represents it.
  vcl_string getCMapString();

  //: Position the color map bar in the top left corner of screen
  //  and scale it screen height.
  void positionCMap();


private:
  SbVec2s last_viewport_sz_;
  SbVec2f last_pos_;
  vgui_button last_down_button_;
  SbSphereSheetProjector* spin_projector_;
  int spin_sample_counter_;
  SbRotation spin_increment_;
  Process    last_process_;
  Log log_;
  SeekDistance seek_distance_; 
  SoTimerSensor* _seekSensor;
  bool axis_visible_;
  int last_timestamp_;
  SbViewportRegion * cmap_vp_;
  SoImage * cmap_image_;
  SoOrthographicCamera* cmapCamera_;
  SoTransform *cmap_transform, *min_transform, *max_transform;
  int cmap_height_, cmap_width_;
  SoCone *min_mark_, *max_mark_;
  bool min_bar_grabbed_, max_bar_grabbed_;

public:
  SbRotation _fromRot;
  SbRotation _toRot;
  SbVec3f _fromPos;
  SbVec3f _toPos;
  biov_transfer_function * btf;
  SoTransferFunction * transfunc;
  int min, max, map;
};


//: Create a smart pointer to a biov_examiner_tableau.
struct biov_examiner_tableau_new : public biov_examiner_tableau_sptr
{
  typedef biov_examiner_tableau_sptr base;
  biov_examiner_tableau_new(SoNode* scene_root = NULL, SoTransferFunction * n_transfunc = NULL)
   : base(new biov_examiner_tableau(scene_root, n_transfunc)) { }
};




#endif // biov_examiner_tableau_h_
