//: 040309 Ming: gdtvis_backpt.h
//: Derived class of standard OpenInventor object to put backward pointer 
//: to our datastructure.

#ifndef gdtvis_backpt_h_
#define gdtvis_backpt_h_

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbgdt3d/dbgdt3d_interval.h>
#include <dbgdt3d/dbgdt3d_welm.h>
#include <dbgdt3d/dbgdt3d_shock.h>

void gdtvis_init_vispt_OpenInventor_classes ();

//: geodesic interval element
class gdtvis_interval_SoLineSet : public SoLineSet, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER (gdtvis_interval_SoLineSet);

protected:
public:
  gdtvis_interval_SoLineSet (const gdt_interval* I = NULL);
  virtual ~gdtvis_interval_SoLineSet() {}

  static void initClass() { 
    SO_NODE_INIT_CLASS (gdtvis_interval_SoLineSet, SoLineSet, "SoLineSet"); 
  }
  virtual const char * getFileFormatName() const { 
    return "gdtvis_interval_SoLineSet"; 
  }
};

//: geodesic wavefront element
class gdtvis_welm_SoLineSet : public SoLineSet, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER (gdtvis_welm_SoLineSet);

protected:
public:
  gdtvis_welm_SoLineSet (const gdt_welm* W = NULL);
  virtual ~gdtvis_welm_SoLineSet() {}

  static void initClass() { 
    SO_NODE_INIT_CLASS (gdtvis_welm_SoLineSet, SoLineSet, "SoLineSet"); 
  }
  virtual const char * getFileFormatName() const { 
    return "gdtvis_welm_SoLineSet"; 
  }
};

//: geodesic shock segment
class gdtvis_shock_SoLineSet : public SoLineSet, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER (gdtvis_shock_SoLineSet);

public:
  gdtvis_shock_SoLineSet (const gdt_shock* S = NULL);
  virtual ~gdtvis_shock_SoLineSet() {}

  static void initClass () { 
    SO_NODE_INIT_CLASS(gdtvis_shock_SoLineSet, SoLineSet, "SoLineSet"); 
  }
  virtual const char* getFileFormatName() const { 
    return "gdtvis_shock_SoLineSet"; 
  }
};

#endif


