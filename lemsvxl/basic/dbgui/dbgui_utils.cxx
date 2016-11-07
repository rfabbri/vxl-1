// This is breye/dbgui/dbgui_utils.cxx
//:
// \file

#include "dbgui_utils.h"
#include <gl2ps/gl2ps.h>
#include <vgui/vgui.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vul/vul_file.h>
#include <vcl_iostream.h>

//: Render the tableaux heirarchy to PostScript
void dbgui_utils::
render_to_ps(const vgui_tableau_sptr& tableau)
{
  GLint file_type = 1;
  
  vgui_dialog save_ps_dlg("Render to PostScript");
  static vcl_string file_name = "";
  static vcl_string ext = "*.ps";
  save_ps_dlg.file("File:", ext, file_name);
  vcl_vector<vcl_string> types;
  types.push_back("(PS)  PostScript");
  types.push_back("(EPS) Encapsulated PostScript");
  types.push_back("(PDF) Portable Document Format");
  types.push_back("(TEX) Text Only");
  int type = 0;
  save_ps_dlg.choice("Output Type", types, type);
  if( !save_ps_dlg.ask())
    return;

  vcl_string output_type = "";
  switch(type){
    case 0: output_type = ".ps";  break;
    case 1: output_type = ".eps"; break;
    case 2: output_type = ".pdf"; break;
    case 3: output_type = ".txt"; break;
  }
  dbgui_utils::render_to_ps(tableau, file_name, output_type);

  return;
}










//------------------------------------------------------------------------------
//: Render a tableau hierarchy to a PostScript file
// Options for output_type: ".ps", ".eps", ".pdf", ".txt" 
// If file_type not provided, extension of output_file will be used
void dbgui_utils::
render_to_ps(const vgui_tableau_sptr& tableau, 
             const vcl_string file_name, const vcl_string& output_type)
{
  vcl_string ext = output_type;
  if (ext == "") // default option, get extension from output file
  {
    ext = vul_file::extension(file_name);
  }

  // Determine output type
  GLint file_type = 1;
  
  if (ext == ".ps")
  {
    file_type = GL2PS_PS;
  }
  else if (ext == ".eps")
  {
    file_type = GL2PS_EPS;
  }
  else if (ext == ".pdf")
  {
    file_type = GL2PS_PDF;
  }
  else if (ext == ".txt")
  {
    file_type = GL2PS_TEX;
  }
  else
  {
    vcl_cout << "\nUnknow output file type. No file created.\n";
    return;
  }

  FILE *fp = fopen(file_name.c_str(), "wb");

  if (!fp)
  {
    vcl_cerr << "\nERROR: can't open file: " << file_name << " for writing.\n";
    return;
  }

  GLint buffsize = 0, state = GL2PS_OVERFLOW;
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  while( state == GL2PS_OVERFLOW ){
    buffsize += 1024*1024;
    gl2psBeginPage ( "MyTitle", "MySoftware", viewport,
                     file_type, GL2PS_BSP_SORT,
                     GL2PS_SILENT | GL2PS_SIMPLE_LINE_OFFSET | GL2PS_NO_BLENDING |
                     GL2PS_OCCLUSION_CULL | GL2PS_BEST_ROOT,
                     GL_RGBA, 0, NULL, 0, 0, 0,
                     buffsize, fp, file_name.c_str() );
    tableau->handle(vgui_DRAW);
    state = gl2psEndPage();
  }
  fclose(fp);

  return;
}



