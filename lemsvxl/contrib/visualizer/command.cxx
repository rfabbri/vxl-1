#include "basegui_tools.h"

#include "command.h"

AddGeomCommand::AddGeomCommand(GraphicsNode *n, Group *gp) : geom(n), group(gp) {}
AddGeomCommand::~AddGeomCommand() { delete geom; }
GraphicsNode *AddGeomCommand::getGeom() { return geom; }
void AddGeomCommand::setGeom(GraphicsNode *g) { geom=g; }
void AddGeomCommand::execute()   { debug_execute();   group->addChild(geom); }
void AddGeomCommand::unexecute() { debug_unexecute(); group->removeChild(geom); }

MultiCommand::~MultiCommand()
{
  for (int i=0;i<_commands.size();i++)
  {
    delete _commands[i];
    _commands[i]=0;
  }

}

void MultiCommand::addCommand(GfxCommand *command_)
{
  assert(command_!=0);
  _commands.push_back(command_);
}

void MultiCommand::execute()
{
  debug_execute();
  for (unsigned int i=0;i<_commands.size();i++)
  {
    _commands[i]->execute();
  }
}    

void MultiCommand::unexecute()
{
  debug_unexecute();
  for (unsigned int i=0;i<_commands.size();i++)
  {
    _commands[i]->unexecute();
  }
}
