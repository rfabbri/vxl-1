#include "basegui_selection.h"

HandleGeom::HandleGeom(const Point2D<double>& origin_) 
  : RectangleGeom<FilledPolygonGeom>
(origin_,1,1,RectangleGeom<FilledPolygonGeom>::CENTER),
_selected(false)
{
  pushStyle(Color(BLACK));
}
const Point2D<double>& HandleGeom::origin() {return _originPos;}
void HandleGeom::origin(const Point2D<double>& origin_) {
  _originPos=origin_;
  calcCoords();
}
bool HandleGeom::selected(){return _selected;}
void HandleGeom::selected(bool selected_){
  if(_selected!=selected_){
    _selected=selected_;
    pop();
    if(_selected){
      pushStyle(Color(RED));
    }else{
      pushStyle(Color(BLACK));
    }
  }
}

Selection::Selection
(DrawingTool* tool_,GraphicsNode *selectedGeom_, Group* handleGroup_)
 : _tool(tool_), _selectedGeom(selectedGeom_), _handleGroup(handleGroup_)
{}

GroupSelection::GroupSelection
(DrawingTool* tool_,GraphicsNode *selectedGeom_, Group* handleGroup_)
  : Selection(tool_,selectedGeom_,handleGroup_),
    _BR(NULL), _mag(1), _angle(0), _translation(Point2D<double>(0,0))
{
  _initHandleFromGeom();
}
GroupSelection::~GroupSelection(){}

void GroupSelection::_deleteGeom(GraphicsNode* geom_){
  if(geom_){
    _handleGroup->removeChild(geom_);
    delete geom_;
  }
}

Point2D<double> GroupSelection::_xfPt
(const XForm& xf_, const Point2D<double>& pt_) {
  Vect3 v;
  v = xf_*Vect3(pt_.getX(),pt_.getY(),1);
  return Point2D<double>(v[0],v[1]);
}
Point2D<double> GroupSelection::_invPt
(const XForm& xf_, const Point2D<double>& pt_) {
  Vect3 v;
  v = invert(xf_)*Vect3(pt_.getX(),pt_.getY(),1);
  return Point2D<double>(v[0],v[1]);
}

void GroupSelection::_initHandleFromGeom(){

  _origXF=_selectedGeom->getXForm();
  if(_origXF==XForm::identity()){
    _selectedGeom->pushXForm(XForm::identity());
//    _tool->view->sceneGraph()->getWorldBoundingRect
//      (_selectedGeom,_cornersBR[0],_cornersBR[1],_cornersBR[2],_cornersBR[3]);
    for(int cnt=0;cnt<4;cnt++)
      _corners[cnt]=_cornersBR[cnt];
  }else{
    _selectedGeom->loadXForm(XForm::identity());
//    _tool->view->sceneGraph()->getWorldBoundingRect
//      (_selectedGeom,_cornersBR[0],_cornersBR[1],_cornersBR[2],_cornersBR[3]);
    _selectedGeom->loadXForm(_origXF);
    for(int cnt=0;cnt<4;cnt+=2){
      Point2D<double> pt
  (_xfPt(_origXF,Point2D<double>(_cornersBR[cnt],_cornersBR[cnt+1])));
      _corners[cnt]=pt.getX();
      _corners[cnt+1]=pt.getY();
    }
  }  
  _prevXF=_origXF;

  //_deleteGeom(_BR);
  _BR = new RectangleGeom<UnfilledPolygonGeom>
    (Point2D<double>(_cornersBR[0],_cornersBR[1]),
     _cornersBR[2]-_cornersBR[0],_cornersBR[3]-_cornersBR[1],
     RectangleGeom<UnfilledPolygonGeom>::UP_LEFT);
  _BR->pushXForm(_origXF);

  // Use only the right-down handle for now.
  _origHandleOrigs[0]=Point2D<double>(_corners[2],_corners[3]);
  double angle = atan
    ((_corners[2]-_corners[0]) / (-(_corners[3]-_corners[1])) );
  Point2D<double> wh
    (-ROTATION_HANDLE_OFFSET*sin(angle),ROTATION_HANDLE_OFFSET*cos(angle));
  _origHandleOrigs[1]=Point2D<double>   
    (_corners[2]+wh.getX(),_corners[3]+wh.getY());  

  for(int cnt=0;cnt<2;cnt++){
    //_deleteGeom(_handles[cnt]);
    _handles[cnt]
      = new HandleGeom
      (Point2D<double>(_origHandleOrigs[cnt].getX(),_origHandleOrigs[cnt].getY()));
    _handles[cnt]->pushXForm(XForm::identity());
    _handleGroup->addChild(_handles[cnt]);
  }
  _handleGroup->addChild(_BR);

  refreshXForm();
}

void GroupSelection::moveHandles
(const Point2D<double>& offset_, const Point2D<double>& mousePos_,
 HandleGeom* selectedHandle_){
  if(selectedHandle_==NULL){
    moveGeom(offset_);
  }else if(selectedHandle_==_handles[HANDLE_SCALE]){
    moveScaleHandle(mousePos_);
  }else if(selectedHandle_==_handles[HANDLE_ROTATION]){
    moveRotationHandle(mousePos_);
  }
}

void GroupSelection::moveGeom(const Point2D<double>& offset_){
  _translation = Point2D<double>
    (_translation.getX()+offset_.getX(),_translation.getY()+offset_.getY());
  refreshXForm();
}

void GroupSelection::moveScaleHandle(const Point2D<double>& mousePos_){  
  Point2D<double> origPos(_origHandleOrigs[HANDLE_SCALE]);
  Point2D<double> mousePos(mousePos_);
  Point2D<double> offset
    (mousePos.getX()-origPos.getX(),mousePos.getY()-origPos.getY());
  Point2D<double> origWH(_corners[2]-_corners[0], _corners[3]-_corners[1]);

  // We need to chose either x or y because the magnification is uniform.
  if(offset.getX()/offset.getY() < origWH.getX()/origWH.getY()) {
    _mag=(origPos.getX()+offset.getX()-_corners[0])/origWH.getX();
  }else{
    _mag=(origPos.getY()+offset.getY()-_corners[1])/origWH.getY();
  }
  refreshXForm();
}

void GroupSelection::moveRotationHandle(const Point2D<double>& mousePos_){
  Point2D<double> origPos(_origHandleOrigs[HANDLE_ROTATION]);
  Point2D<double> mousePos(mousePos_);
  Point2D<double> offset
    (mousePos.getX()-origPos.getX(),mousePos.getY()-origPos.getY());

  double origAngle = atan
    ((_corners[2]-_corners[0]) / (-(_corners[3]-_corners[1])) );
  double origWH[2] = {_corners[2]-_corners[0],_corners[3]-_corners[1]};
  double origRadius = sqrt((origWH[0]*origWH[0])+(origWH[1]*origWH[1]));
  double newAngle = 
    atan((origPos.getX()+offset.getX()-_corners[0]) / 
   (-(origPos.getY()+offset.getY()-_corners[1]) ) );
  if(origPos.getY()+offset.getY()-_corners[1]<(double)0){
    double angle=newAngle+(double)M_PI;
    newAngle 
      = (angle>(double)M_PI) ? (angle-(M_PI*2)) : 
      ( (angle<-(double)M_PI) ? (angle+(M_PI*2)) : angle);
  }

  // We need to adjust the handle-position according to the original radius.
  _angle=newAngle-origAngle;
  refreshXForm();
}

void GroupSelection::refreshXForm(){
  XForm xf
    (XForm::translation(_translation.getX(),_translation.getY())*
     XForm::translation(_corners[0],_corners[1])*
     XForm::uniform_scale(_mag)*
     XForm::rotation(_angle)*
     XForm::translation(-_corners[0],-_corners[1]));

  _selectedGeom->loadXForm(xf*_origXF);
  _BR->loadXForm(xf*_origXF);

  for(int cnt=0;cnt<2;cnt++){
    _handles[cnt]->origin(_xfPt(xf,_origHandleOrigs[cnt]));
  };
  _prevXF=xf;
}

DirectSelection::DirectSelection
(DrawingTool* tool_,GraphicsNode *selectedGeom_, Group* handleGroup_)
  : Selection(tool_,selectedGeom_,handleGroup_)
{
  _initHandleFromGeom();
}

DirectSelection::~DirectSelection(){}
  
void DirectSelection::_initHandleFromGeom(){
  vcl_vector<Point2D<double> > pts;
  if(_selectedGeom->getPoints_(pts)==0){
    vcl_cout << "Non-supported geom-obj selected." << vcl_endl;
    return;
  }
  if(handles.size()>0) {
    for(vcl_vector<HandleGeom*>::iterator itr=handles.begin() ;
  itr!=handles.end() ; itr++){
      delete (*itr);
    }
    handles.clear();
  }
  for(vcl_vector<Point2D<double> >::iterator itr=pts.begin() ;
      itr!=pts.end() ; itr++){
#if 1
    HandleGeom *geom = new HandleGeom(*itr);
#else
    HandleGeom *geom = new HandleGeom(_tool->view->unproject(*itr));
#endif
    handles.push_back(geom);
    _handleGroup->addChild(geom);
  }
}
void DirectSelection::propagateHandleToGeom(){
  for(unsigned int cnt=0; cnt<handles.size() ; cnt++){
    if(!handles[cnt]->selected())
      continue;
    if(! _selectedGeom->setPoint_(cnt,handles[cnt]->origin())){
      vcl_cout << "Propagation failed. Aborting the process." << vcl_endl;
      return;
    }
  }
}
void DirectSelection::setFlag(bool flag_, const HandleGeom* hg_){
  for(vcl_vector<HandleGeom*>::iterator itr=handles.begin() ;
      itr!=handles.end() ; itr++){
    if(hg_==NULL || (*itr)==hg_){
      (*itr)->selected(flag_);
    }
  }
  return;
}
bool DirectSelection::selected(){
  for(vcl_vector<HandleGeom*>::iterator itr=handles.begin();
      itr!=handles.end() ; itr++){
    if((*itr)->selected()){
      return true;
    }
  }
  return false;
}

void DirectSelection::moveHandles(const Point2D<double>& offset_){
  bool moved=false;
  for(vcl_vector<HandleGeom*>::iterator itr=handles.begin();
      itr!=handles.end() ; itr++){
    if((*itr)->selected()){
      Point2D<double> prevOrig((*itr)->origin());
      (*itr)->origin
  (Point2D<double>(prevOrig.getX()+offset_.getX(),
       prevOrig.getY()+offset_.getY()));
      moved=true;
    }
  }
  if(moved)
    propagateHandleToGeom();
}

SelectionTool::SelectionTool(BaseWindow *win, Group *pgroup)
  : DrawingTool(win, pgroup), _handleGroup(NULL), state(NONE)
{}

void SelectionTool::_initSelectionObj(GraphicsNode*selected_){  
  if(_handleGroup){
    tmp()->removeChild(_handleGroup);
    delete _handleGroup;
  }
  _handleGroup = new Group;
  tmp()->addChild(_handleGroup);
  //_selection->selectedGeom(selected_);
}

DrawingTool::Result SelectionTool::_selectGeom
(const Point2D<double>& coord_)
{
  vcl_vector<HitRecord> hits;
  view->hitTest(hits,coord_,3);
  for(vcl_vector<HitRecord>::iterator itr=hits.begin() ;
      itr!=hits.end(); itr++){
    AffineNode *ag=dynamic_cast<AffineNode*>(itr->hit_object);
    if(!ag)
      continue;
    _initSelectionObj(ag);
    state=GEOM_SELECTED;
    return HANDLED;
  }
  // There was no hit.
  return UNHANDLED;
}

DrawingTool::Result SelectionTool::leftMouseUp(const wxMouseEvent *evt) {
  _evt=evt;
  switch(state) {
  case NONE:
     
      return UNHANDLED;
  case HANDLE_DRAGGED:
    state=GEOM_SELECTED;
    break;
  default:
    break;
  }
  return HANDLED;
}

DrawingTool::Result SelectionTool::rightMouseDown (const wxMouseEvent *evt) 
  { 
    return UNHANDLED; 
   }

DrawingTool::Result SelectionTool::middleMouseDown
(const wxMouseEvent *evt) 
{ return UNHANDLED; }
DrawingTool::Result SelectionTool::middleMouseUp
(const wxMouseEvent *evt) 
{ return UNHANDLED; }

DrawingTool::Result SelectionTool::abort() {
  switch(state) {
  case NONE: 
    return UNHANDLED;
  default:
    tmp()->removeChild(_handleGroup);
    delete _handleGroup;
    state = NONE;
    return ABORTED;
  }
}

GroupSelectionTool::GroupSelectionTool(BaseWindow *win, Group *pgroup) 
  : SelectionTool(win, pgroup), _selectedHandle(NULL)
{}

void GroupSelectionTool::_initSelectionObj(GraphicsNode*selected_){
  SelectionTool::_initSelectionObj(selected_);
  _selection = new GroupSelection(this,selected_,_handleGroup);
}

DrawingTool::Result GroupSelectionTool::_selectHandles
(const Point2D<double>& coord_)
{
  vcl_vector<HitRecord> hits;
  view->hitTest(hits,coord_,3);
  bool handled=false;
  for(vcl_vector<HitRecord>::iterator itr=hits.begin() ;
      itr!=hits.end(); itr++){
    HandleGeom *hg=dynamic_cast<HandleGeom*>(itr->hit_object);
    if(hg){
      _selectedHandle=hg;
      handled=true;
      break;
    }
  }
  if(handled){
    return HANDLED;
  }else{
    _selectedHandle=NULL;
    return UNHANDLED;
  }
}

DrawingTool::Result GroupSelectionTool::keyDown( wxKeyEvent &evt) 
  {
    if(state == GEOM_SELECTED)
      {
        char keyCode=evt.GetKeyCode();
        if(keyCode == 'G')
          {
            Group *gp=_selection->selectedGeom()->parent();
            if(gp && gp!=view->sceneGraph()->rootGroup())
              {
                _initSelectionObj(gp);
               }
            return HANDLED;
           }
       }

    return UNHANDLED;
}

DrawingTool::Result GroupSelectionTool::leftMouseDown
(const wxMouseEvent *evt) {
  Point2D<double> coord(evt->GetX(),evt->GetY());
  Result res;
  switch(state) {
  case NONE:
    res=_selectGeom(coord);
    return res;
  case GEOM_SELECTED:
    if(evt->ShiftDown()){
      
    }else{
      _selectHandles(coord);
#if 1
      _prevMousePos=view->unproject(Point2D<double>(evt->GetX(), evt->GetY()));
#else
      _prevMousePos=Point2D<double>(evt->GetX(), evt->GetY());
#endif
      state=HANDLE_DRAGGED;
      return HANDLED;
    }
  default:
    return UNHANDLED;
  }
}

DrawingTool::Result GroupSelectionTool::mouseMove(const wxMouseEvent *evt) {
  switch(state) {
  case NONE: 
  case GEOM_SELECTED:
    return UNHANDLED;
  case HANDLE_DRAGGED: {
#if 1
    Point2D<double> newMousePos
      =view->unproject(Point2D<double>(evt->GetX(), evt->GetY()));
#else
    Point2D<double> newMousePos(evt->GetX(), evt->GetY());
#endif
    Point2D<double> offset(newMousePos.getX()-_prevMousePos.getX(),
         newMousePos.getY()-_prevMousePos.getY());
    selection()->moveHandles(offset,newMousePos,_selectedHandle);
    _prevMousePos=newMousePos;
    return HANDLED;
  }
  default:
    return UNHANDLED;
  }
}

DrawingTool::Result GroupSelectionTool::rightMouseUp
(const wxMouseEvent *evt) 
{
  switch(state) {
  case NONE: 
    return UNHANDLED;
  case GEOM_SELECTED:
  case HANDLE_DRAGGED: {
    XForm xf=_selection->selectedGeom()->getXForm();
    if(xf==XForm::identity()){
      _selection->selectedGeom()->pop();
    }
    tmp()->removeChild(_handleGroup);
    delete _handleGroup;
    permanent_group = _selection->selectedGeom()->parent();
    permanent_group->removeChild(_selection->selectedGeom());
    state = NONE;
    GfxCommand *c = new AddGeomCommand(_selection->selectedGeom(), permanent_group);    
    return COMPLETED;
  }
  default:
    return UNHANDLED;
  }
}

DirectSelectionTool::DirectSelectionTool(BaseWindow *win, Group *pgroup) 
  : SelectionTool(win, pgroup)
{}

void DirectSelectionTool::_initSelectionObj(GraphicsNode*selected_){
  SelectionTool::_initSelectionObj(selected_);
  _selection = new DirectSelection(this,selected_,_handleGroup);
}

DrawingTool::Result DirectSelectionTool::_selectHandles(const Point2D<double>& coord_)
{
  vcl_vector<HitRecord> hits;
  view->hitTest(hits,coord_,3);
  bool handled=false;
  for(vcl_vector<HitRecord>::iterator itr=hits.begin() ;
      itr!=hits.end(); itr++){
    HandleGeom *hg=dynamic_cast<HandleGeom*>(itr->hit_object);
    if(hg){
      if(!_evt->ShiftDown()){
  selection()->setFlag(false);
      }
      selection()->setFlag(true,hg);
      handled=true;
      break;
    }
  }
  if(handled){
    return HANDLED;
  }else{
    return UNHANDLED;
  }
}

DrawingTool::Result DirectSelectionTool::leftMouseDown(const wxMouseEvent *evt) {
  _evt=evt;
  Point2D<double> coord(evt->GetX(),evt->GetY());
  Result res;
  switch(state) {
  case NONE:
    res=_selectGeom(coord);
    return res;
  case GEOM_SELECTED:
    res=_selectHandles(coord);
    if(selection()->selected()){
#if 1
      _prevMousePos=view->unproject(Point2D<double>(evt->GetX(), evt->GetY()));
#else
      _prevMousePos=Point2D<double>(evt->GetX(), evt->GetY());
#endif
      state=HANDLE_DRAGGED;
    }else{
      state=GEOM_SELECTED;
    }
    return res;
  default:
    return UNHANDLED;
  }
}

DrawingTool::Result DirectSelectionTool::mouseMove(const wxMouseEvent *evt) {
  _evt=evt;
  switch(state) {
  case NONE: 
  case GEOM_SELECTED:
    return UNHANDLED;
  case HANDLE_DRAGGED: {
#if 1
    Point2D<double> newMousePos
      =view->unproject(Point2D<double>(evt->GetX(), evt->GetY()));
#else
    Point2D<double> newMousePos(evt->GetX(), evt->GetY());
#endif
    Point2D<double> offset(newMousePos.getX()-_prevMousePos.getX(),
         newMousePos.getY()-_prevMousePos.getY());
    selection()->moveHandles(offset);
    _prevMousePos=newMousePos;
    return HANDLED;
  }
  default:
    return UNHANDLED;
  }
}

DrawingTool::Result DirectSelectionTool::rightMouseUp(const wxMouseEvent *evt) {
  _evt=evt;
  switch(state) {
  case NONE: 
    return UNHANDLED;
  case GEOM_SELECTED:
  case HANDLE_DRAGGED: {
    tmp()->removeChild(_handleGroup);
    delete _handleGroup;
    permanent_group->removeChild(_selection->selectedGeom());
    state = NONE;
    GfxCommand *c = new AddGeomCommand(_selection->selectedGeom(), permanent_group);    
    return COMPLETED;
  }
  default:
    return UNHANDLED;
  }
}

