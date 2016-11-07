#include "basegui_drawingwindow.h"

#include "command.h"
#include "basegui_style.h"


BEGIN_EVENT_TABLE(DrawingWindow, BaseWindow)
    EVT_MENU(VIS_SET_PEN_COLOR, DrawingWindow::OnPenColor)
    EVT_MENU(VIS_SET_PEN_WIDTH, DrawingWindow::OnPenWidth)
    EVT_MENU_RANGE(VIS_DRAWING_FIRST,VIS_DRAWING_LAST, DrawingWindow::OnToolSelectionEvent)
END_EVENT_TABLE()

DrawingWindow::DrawingWindow(MainGUIWindow *win, const std::string &name, 
           BasePlugin *plugin)
  : BaseWindow(win, name, plugin), _drawing_group(NULL)
{
  _drawing_group = new Group;
  sg()->rootGroup()->addChild(_drawing_group);
}

DrawingWindow::~DrawingWindow() {}

GTool* DrawingWindow::getToolFromID(int id) 
{
  GTool *tool=NULL;

  switch(id) {
    case VIS_DRAWING_DELETE:
      tool = new DeleteTool(this); break;
    case VIS_DRAWING_POINT:
      tool= new PointTool<PointGeom> (this, _drawing_group); break;
    case VIS_DRAWING_POINT_FREEHAND:
      tool = new PointFreehandTool<PointGeom>(this, _drawing_group); break;
    case VIS_DRAWING_LINE:
      tool = new LineTool<LineGeom>(this, _drawing_group); break;
    case VIS_DRAWING_POLYLINE:
      tool = new PolylineTool<PolylineGeom>(this, _drawing_group); break;
    case VIS_DRAWING_POLYGON:
      tool = new PolygonTool<UnfilledPolygonGeom>(this, _drawing_group); break;
    case VIS_DRAWING_POLYLINE_FREEHAND:
      tool = new PolylineFreehandTool<PolylineGeom>(this, _drawing_group); break;
    case VIS_DRAWING_POLYGON_FREEHAND:
      tool = new PolygonFreehandTool<UnfilledPolygonGeom>(this, _drawing_group); break;
    case VIS_DRAWING_ARC:
      tool = new ArcTool<ArcGeom>(this, _drawing_group); break;
    case VIS_DRAWING_ARC_SPLINE:
      tool = new ArcSplineTool<ArcGeom>(this, _drawing_group); break;
    case VIS_DRAWING_EULER_SPIRAL_SPLINE:
      tool = new EulerSpiralTool(this, _drawing_group); break;
    case VIS_DRAWING_BI_ARC_SPLINE:
      tool = new BiArcTool(this, _drawing_group); break;
    case VIS_DRAWING_ARROW:
      tool = new ArrowTool(this, _drawing_group); break;
    //case VIS_DRAWING_DIRECT_SELECTION:
    //  tool = new DirectSelectionTool(this, _drawing_group); break;
    //case VIS_DRAWING_GROUP_SELECTION:
    //  tool = new GroupSelectionTool(this, _drawing_group); break;
    default:
    tool = BaseWindow::getToolFromID(id);
    break;
  }

  return tool;
}

void DrawingWindow::addCustomMenus()
{
  _tools_menu = new wxMenu;
  _tools_menu->Append(VIS_SET_PEN_COLOR, "Pen &Color\tCtrl-c");
  _tools_menu->Append(VIS_SET_PEN_WIDTH, "Pen &Width\tCtrl-w");
    _tools_menu->AppendSeparator();
  _tools_menu->Append(VIS_DRAWING_DELETE, "&Delete");
  _tools_menu->Append(VIS_DRAWING_POINT, "&Point");
  _tools_menu->Append(VIS_DRAWING_POINT_FREEHAND, "Points freehand");
  _tools_menu->Append(VIS_DRAWING_LINE, "&Line");
  _tools_menu->Append(VIS_DRAWING_POLYLINE, "Po&lyline");
  _tools_menu->Append(VIS_DRAWING_POLYGON, "Poly&gon");
  _tools_menu->Append(VIS_DRAWING_POLYLINE_FREEHAND, "Polyline freehand");
  _tools_menu->Append(VIS_DRAWING_POLYGON_FREEHAND, "Polygon freehand");
  _tools_menu->Append(VIS_DRAWING_ARC, "&Arc");
  _tools_menu->Append(VIS_DRAWING_ARC_SPLINE, "Arc &Spline");
  _tools_menu->Append(VIS_DRAWING_EULER_SPIRAL_SPLINE, "&Euler Spiral Spline");
  _tools_menu->Append(VIS_DRAWING_BI_ARC_SPLINE, "&Bi Arc Spline");
  _tools_menu->Append(VIS_DRAWING_ARROW, "A&rrow");

  _mainMenubar->Append(_tools_menu, "&Tools");
}

void DrawingWindow::OnPenColor()
{
  wxColourDialog dialog(this);
  dialog.SetTitle("Choose the Pen colour");
  if (dialog.ShowModal() == wxID_OK){
    wxColour col = dialog.GetColourData().GetColour();
    vcl_cout<<" Vals = "<<(int)col.Red()<<"  "<<(int)col.Green()<<"  "<<(int)col.Blue()<<vcl_endl;
    //mergeStyle(Color((float)col.Red(), (float)col.Green(), (float)col.Blue()));
  }
}

void DrawingWindow::OnPenWidth()
{
  long res = wxGetNumberFromUser( "Choose Pen Width\n"
                                    "Even two rows of text.",
                                    "Enter Width:", " ",
                                     50, 0, 100, this );
  if (res>0){
    //DrawingTool::mergeStyle(LineWidth(res));  
  }
}

