#include "poly_coin3d_proj_observer.h"

#include <vgl/vgl_homg_plane_3d.h> 
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h> 
#include <vgl/vgl_closest_point.h> 
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_det.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>

#include <bgui3d/bgui3d_algo.h>
#include <dbmsh3d/dbmsh3d_mesh_mc.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoIndexedFaceSet.h> 
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/events/SoMouseButtonEvent.h> 
#include <Inventor/SoPath.h> 
#include <Inventor/SoPickedPoint.h> 
#include <Inventor/misc/SoChildList.h> 
#include <Inventor/details/SoDetail.h>

vcl_vector<SoNode*> poly_coin3d_proj_observer::selected_nodes = vcl_vector<SoNode*> (0);

void selectionCallback2(void *userData, SoEventCallback *eventCB)
{
  SoSelection *selection = (SoSelection *) userData;
  const SoEvent *ev =  eventCB->getEvent();
  if (SO_MOUSE_PRESS_EVENT(ev, BUTTON1)) {
    vcl_cout << "Selected!" << vcl_endl;
    vcl_cout << "Num Selected " << selection->getNumSelected() << vcl_endl;
    if (selection->getNumSelected() > 0) {
      SoPath* path = selection->getPath(0);
      SoNode* node = path->getTail();
      if (node->isOfType(SoIndexedFaceSet::getClassTypeId())) {
        vcl_cout << "Found a MESH!" << vcl_endl;
        // put the parent in the list
        poly_coin3d_proj_observer::set_selected(path->getNodeFromTail(1));
        const SoPickedPoint *pp = eventCB->getPickedPoint();
        if (pp) {
          const SoDetail* detail = pp->getDetail(node);
        }
      }
    }
  }
}

poly_coin3d_proj_observer::poly_coin3d_proj_observer(const vpgl_proj_camera<double>& camera, SoNode* root)
: bgui3d_project2d_tableau(camera, root), num_meshes_(0)
{ 
  root_ = (SoSeparator*) root; 
  root_sel_ = new SoSelection;
  root_->addChild(root_sel_);
  root_sel_->policy = SoSelection::SHIFT;
  SoEventCallback* eventCB = new SoEventCallback;
  eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), 
    selectionCallback2, root_);
  root_sel_->addChild(eventCB);
}

void poly_coin3d_proj_observer::update (vgui_message const& msg) 
{
  vgui_message m = const_cast <vgui_message const& > (msg);
  const observable* o = static_cast<const observable*> (m.from);
  observable* o2= const_cast<observable*> (o);
  vcl_string type = o2->type_name();
  if (strcmp(type.data(),"obj_observable") == 0)
  {
    const obj_observable* o = static_cast<const obj_observable*> (m.from);
    obj_observable* observable = const_cast<obj_observable*> (o);
    handle_update(msg, observable);
  }
}

void poly_coin3d_proj_observer::handle_update(vgui_message const& msg, 
                                  obj_observable* observable) 
{
  const vcl_string* str = static_cast<const vcl_string*> (msg.data);
  dbmsh3d_mesh_mc* M = observable->get_object();
  //SoSeparator* root = (SoSeparator*) this->user_scene_root();
  
  if (str->compare("new") == 0) {
    M->IFS_to_MHE();
    M->build_face_IFS ();
    //merge_mesh_coplanar_faces (&M);
    //M->build_face_IFS ();
    M->orient_face_normals();
    M->build_face_IFS ();

    //Draw the whole mesh to a single object
    //SoMaterial *myMaterial = new SoMaterial;
    //myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);  // Green
    
    SoSeparator* mesh = draw_M (M, false, 0.0, COLOR_BLUE);
    mesh->ref();
    vcl_string name = create_mesh_name();
    mesh->setName(SbName(name.data()));
    //root_sel_->addChild(myMaterial);
    SoTransform *myTransform = new SoTransform;
    myTransform->translation.setValue(0, 0, 0);
    myTransform->ref();
    mesh->insertChild(myTransform,0);
    root_sel_->addChild(mesh);

    objects[observable] = mesh;
    meshes[observable] = M->clone();
    this->setup_projection();
    
  } else if (str->compare("move") == 0) {
    
    // find the translation between the new and the old position
    SoSeparator *m = objects[observable];
    dbmsh3d_mesh_mc* prev_mesh = meshes[observable];
   
    // find the translation
    dbmsh3d_vertex* v1 = (dbmsh3d_vertex*) prev_mesh->edgemap(0)->sV();
    dbmsh3d_vertex* v2 = (dbmsh3d_vertex*) M->edgemap(0)->sV();
    vgl_point_3d<double> p1 = v1->get_pt();
    vgl_point_3d<double> p2 = v2->get_pt();
    vgl_point_3d<double> t = p2 - p1;
    SoTransform *myTransform = (SoTransform *) m->getChild(0);//new SoTransform;
    SbVec3f tr = myTransform->translation.getValue();
    double x = p2.x() - p1.x();
    double y = p2.y() - p1.y();
    double z = p2.z() - p1.z();
    //vcl_cout << "X=" << x << " Y=" << y << " Z=" << z << vcl_endl;
    float x1, y1, z1;
    tr.getValue(x1, y1, z1);
    myTransform->translation.setValue(x1+x, y1+y, z1+z);
    //m->insertChild(myTransform,0);
    delete meshes[observable];
    meshes[observable] = M->clone();
    this->render();
    
  } else if (str->compare("update") == 0) {
    SoSeparator *m = objects[observable];
    dbmsh3d_mesh_mc* prev_mesh = meshes[observable];

    SoSeparator* group = (SoSeparator*)this->scene_root();
    SbName n = m->getName();
    vcl_cout << n.getString() << vcl_endl;
    SoSeparator* node = (SoSeparator *) SoNode::getByName(m->getName());
    root_sel_->removeChild(node);
    M->IFS_to_MHE();
    M->build_face_IFS ();
    M->orient_face_normals();
    M->build_face_IFS ();

    //Draw the whole mesh to a single object
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);  // Green
    
    SoSeparator* mesh = draw_M (M, true, 0.0, COLOR_GREEN);
    mesh->ref();
    mesh->setName(n);
    //root->addChild(myMaterial);
    //int num_child = m->getNumChildren();
    //SoNode * node = mesh->getChild(mesh->getNumChildren()-1);
    //m->insertChild(node, num_child-1);
    root_sel_->addChild(mesh);
    //this->view_all();
    objects[observable] = mesh;
    meshes[observable] = M;
  }
    SoWriteAction myAction;
    myAction.getOutput()->openFile("C:\\test_images\\mesh\\root.iv");
    myAction.getOutput()->setBinary(FALSE);
    myAction.apply(root_sel_);
    myAction.getOutput()->closeFile();
    dbmsh3d_save_ply2 (M, "C:\\test_images\\mesh\\out.ply2");
    
}
bool poly_coin3d_proj_observer::handle(const vgui_event& e)
{
  //vcl_cout << e.modifier << vcl_endl;
  if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
    //vcl_cout << "left mouse" << vcl_endl;
    start_x_ = e.wx;
    start_y_ = e.wy;
    left_button_down_ = true;
    return true;
  
  } else if (e.type == vgui_BUTTON_UP && e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
    left_button_down_ = false;
    return true;
  
  } else if (e.type == vgui_MOTION && left_button_down_ && e.modifier == vgui_SHIFT) {
    // first make sure that an object is selected from scene graph
    if (selected_nodes.size() > 0) {
      double x = e.wx;
      double y = e.wy;
      double dist = vcl_sqrt((x-start_x_)*(x-start_x_) + (y-start_y_)*(y-start_y_))/60;
      if (y > start_y_)
        dist *= -1;
      //vcl_cout << "dist=" << dist << vcl_endl;

      SoNode* node = selected_nodes[0];
      vcl_map<obj_observable *, SoSeparator*>::iterator iter = objects.begin();
      while (iter != objects.end()) {
        SoSeparator* sep = iter->second;
        if (node == sep) {
          obj_observable *obs = iter->first;
          obs->move_normal_dir(dist);
        }
        iter++;
      }
    }
    return true;
  } else if (e.type == vgui_BUTTON_DOWN && e.button == vgui_RIGHT && e.modifier == vgui_SHIFT) {
      middle_button_down_ = true;
      start_x_ = e.wx;
      start_y_ = e.wy;
      obj_observable *obs = find_selected_obs();
      if (obs)
        obs->extrude(0);
      return true;
  } else if (e.type == vgui_MOTION && middle_button_down_ == true && e.modifier == vgui_SHIFT) {
      // update these only if there is motion event
      float wx = e.wx;
      float wy = e.wy;
      
      // first make sure that an object is selected from scene graph
      if (selected_nodes.size() > 0) {
        double x = e.wx;
        double y = e.wy;
        double dist = vcl_sqrt((x-start_x_)*(x-start_x_) + (y-start_y_)*(y-start_y_))/20;
        if (y > start_y_)
          dist *= -1;
        //vcl_cout << "dist=" << dist << vcl_endl;

        obj_observable *obs = find_selected_obs();
        if (obs)
            obs->move_extr_face(dist);
      }
      return true;
    } 
    else if (e.type == vgui_BUTTON_UP && e.button == vgui_RIGHT && e.modifier == vgui_SHIFT){
      middle_button_down_ = false;
      return true;
    }

  return bgui3d_project2d_tableau::handle(e);
}

obj_observable* poly_coin3d_proj_observer::find_selected_obs()
{
  SoNode* node = selected_nodes[selected_nodes.size()-1];
  vcl_map<obj_observable *, SoSeparator*>::iterator iter = objects.begin();
  while (iter != objects.end()) {
    SoSeparator* sep = iter->second;
    if (node == sep) {
      obj_observable *obs = iter->first;
      return obs;
    }
    iter++;
  }
  return 0;
}

vcl_string poly_coin3d_proj_observer::create_mesh_name()
{
   vcl_string base = "mesh";

   vcl_stringstream strm;
   strm << vcl_fixed << num_meshes_;
   vcl_string str(strm.str());
   num_meshes_++;
   return (base+str);
}
