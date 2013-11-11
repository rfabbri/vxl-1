#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include <vcl_vector.h>

#include "basegui_scenegraph.h"

class GfxCommand {
protected:
  bool _executed;
  void debug_execute()   { assert(!_executed); _executed = true;  }
  void debug_unexecute() { assert(can_undo()); assert( _executed); _executed = false; }

public:
  GfxCommand() : _executed(false) {}
  virtual ~GfxCommand() {}

  virtual bool can_undo() { return true; }
  virtual void execute() = 0;
  virtual void unexecute() = 0;
};

class ReverseCommand : public GfxCommand {
  GfxCommand *cmd;
public:
  ReverseCommand(GfxCommand *c) : cmd(c) { _executed=true; }
  ~ReverseCommand() { } // do nothing... cmd is owned by other things

  virtual void execute() { debug_execute(); cmd->unexecute(); }
  virtual void unexecute() { debug_unexecute(); cmd->execute(); }
};

class AddGeomCommand : public GfxCommand {
  GraphicsNode *geom;
  Group *group;
public:
  AddGeomCommand(GraphicsNode *n, Group *gp);
  ~AddGeomCommand();

  GraphicsNode *getGeom();
  void setGeom(GraphicsNode *g);

  void execute();
  void unexecute();
};

class MultiCommand :public GfxCommand
  {
    private:
        vcl_vector<GfxCommand *>_commands;
    
    public:
       virtual ~MultiCommand();

       void addCommand(GfxCommand *command_);
       void execute();
       void unexecute();
   };

#endif
