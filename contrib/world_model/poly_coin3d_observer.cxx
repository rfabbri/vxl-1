#include "poly_coin3d_observer.h"

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
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/events/SoMouseButtonEvent.h> 
#include <Inventor/SoPath.h>  
#include <Inventor/SoPickedPoint.h> 
#include <Inventor/misc/SoChildList.h> 
#include <Inventor/details/SoDetail.h>
#include <Inventor/details/SoFaceDetail.h>

vcl_vector<SoNode*> poly_coin3d_observer::selected_nodes = vcl_vector<SoNode*> (0);
vcl_vector<const SoPickedPoint*> poly_coin3d_observer::picked_points = vcl_vector<const SoPickedPoint*> (0);

void selectionCallback(void *userData, SoEventCallback *eventCB)
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
        SoIndexedFaceSet* ifs = (SoIndexedFaceSet*)node;
        const SoPickedPoint *pp = eventCB->getPickedPoint();

        // put the parent in the list
        poly_coin3d_observer::set_selected(path->getNodeFromTail(1));
        poly_coin3d_observer::set_picked(pp);
      }
    }
  }
}

poly_coin3d_observer::poly_coin3d_observer(SoNode* root)
: bgui3d_examiner_tableau(root), num_meshes_(0), divide_mode_(false)
{ 
  root_ = (SoSeparator*) root; 
  root_sel_ = new SoSelection;
  root_->addChild(root_sel_);
  root_sel_->policy = SoSelection::SHIFT;
 // SoEventCallback* eventCB = new SoEventCallback;
//  eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), 
//    selectionCallback, root_);
//  root_sel_->addChild(eventCB);
}
 
void poly_coin3d_observer::extrude()
{
  obs_picked_ = find_selected_obs(face_id);
  if ((obs_picked_) && (face_id > -1))
    obs_picked_->extrude(face_id);
}

void poly_coin3d_observer::divide()
{
  obs_picked_ = find_selected_obs(face_id);
  divide_mode_ = true;
  div_pts_.resize(2);
  div_idx_ = 0;
}

void poly_coin3d_observer::update (vgui_message const& msg) 
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

void poly_coin3d_observer::handle_update(vgui_message const& msg, 
                                  obj_observable* observable) 
{
  const vcl_string* str = static_cast<const vcl_string*> (msg.data);
  dbmsh3d_mesh_mc* M = observable->get_object();

  SbVec3f v;
  v.normalize();
////////////////////////////////////
  dbmsh3d_mesh_mc* M_copy = M->clone();
  // tranform the mesh by the rational camera norm_trans()
  vcl_map<int, dbmsh3d_vertex*> vertices = M_copy->vertexmap();
  for(unsigned i=0; i<vertices.size(); i++) {
    dbmsh3d_vertex* v = vertices[i]; 
    vgl_point_3d<double> p = v->get_pt();
    vgl_point_3d<double> tp = T_(vgl_homg_point_3d<double>(p));
    v->set_pt (tp);
  }
  //SoSeparator* root = (SoSeparator*) this->user_scene_root();
  M = M_copy;
//////////////////////////////
  

  if (str->compare("new") == 0) {
    M->IFS_to_MHE();
    M->orient_face_normals();
    
    M->build_face_IFS ();
    SoSeparator* mesh = draw_M (M, false, 0.0, COLOR_BLUE);
    mesh->ref();
    vcl_string name = create_mesh_name();
    mesh->setName(SbName(name.data()));

    SoTransform *myTransform = new SoTransform;
    myTransform->translation.setValue(0, 0, 0);
   /* SoTransform *myTransform = new SoTransform;
    SbMatrix sbm(T_.get(0,0), T_.get(1,0), T_.get(2,0), T_.get(3,0), 
    T_.get(0,1), T_.get(1,1), T_.get(2,1), T_.get(3,1),
    T_.get(0,2), T_.get(1,2), T_.get(2,2), T_.get(3,2),
    T_.get(0,3), T_.get(1,3), T_.get(2,3), T_.get(3,3));
    myTransform->setMatrix(sbm);*/
    myTransform->ref();
    mesh->insertChild(myTransform,0);
    root_sel_->addChild(mesh);
   // this->view_all();

    objects[observable] = mesh;
    meshes[observable] = M->clone();

    
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
    ///M->print_summary(vcl_cout);
    //M->print_topo_summary();
    SoSeparator* group = (SoSeparator*)this->user_scene_root();
    SbName n = m->getName();
    vcl_cout << n.getString() << vcl_endl;
    SoSeparator* node = (SoSeparator *) SoNode::getByName(m->getName());
    root_sel_->removeChild(node);
    //M->IFS_to_MHE();
    //M->build_face_IFS ();
    M->orient_face_normals();
    //M->build_face_IFS ();

    //Draw the whole mesh to a single object
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);  // Green
    M->build_face_IFS ();
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
bool poly_coin3d_observer::handle(const vgui_event& e)
{
  //vcl_cout << "poly_coin3d_observer::handle " << e.type << " " << e.button << " " << e.modifier << vcl_endl;
  if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == vgui_CTRL) {
    //vcl_cout << "1" << vcl_endl;
    SoRayPickAction rp( this->get_viewport_region());
    rp.setPoint(SbVec2s(e.wx, e.wy));
    rp.setRadius(10);
    rp.apply(this->scene_root());
    point_picked_ = rp.getPickedPoint();
    if (point_picked_) {
      SbVec3f point = point_picked_->getPoint();
      float x,y,z;
      point.getValue(x,y,z);
      vcl_cout << "[" << x << "," << y << "," << z << "]" << vcl_endl;
      if (divide_mode_) {
        div_pts_[div_idx_++] = vgl_point_3d<double> (x,y,z);
        if (div_idx_ > 1) {
          vgl_point_3d<double> point1, point2, l1, l2, l3, l4;
          find_intersection_points(face_id, div_pts_[0], div_pts_[1], point1, l1, l2, 
            point2, l3, l4);
          obs_picked_->divide_face(face_id, l1, l2, point1, l3, l4, point2);
          divide_mode_ = false;
        }
      } else {
        point_3d_.set(x, y, z);
        SoPath* path =  point_picked_->getPath();
        SoNode* node = path->getTail();
        if (node->isOfType(SoIndexedFaceSet::getClassTypeId())) {
         vcl_cout << "Found a MESH!" << vcl_endl;
          //SoIndexedFaceSet* ifs = (SoIndexedFaceSet*)node;
          node_selected_ = path->getNodeFromTail(1);
          //point_picked_ = pp;
        }
      }
    } else 
      vcl_cout << "PICKED POINT IS O" << vcl_endl;
  }
  if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
    //vcl_cout << "2" << vcl_endl;
    //vcl_cout << "left mouse" << vcl_endl;
    start_x_ = e.wx;
    start_y_ = e.wy;
    left_button_down_ = true;
    return true;
  
  } else if (e.type == vgui_BUTTON_UP && e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
    left_button_down_ = false;
    return true;
  
  } else if (e.type == vgui_MOTION && /*left_button_down_*/e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
    //vcl_cout << "3" << vcl_endl;
    // first make sure that an object is selected from scene graph
    if (node_selected_) {//(selected_nodes.size() > 0) {
      double x = e.wx;
      double y = e.wy;
      //double dist = vcl_sqrt((x-start_x_)*(x-start_x_) + (y-start_y_)*(y-start_y_))/60;
      double dist = (y-start_y_)/2;
      //if (y > start_y_)
      //  dist *= -1;
      //vcl_cout << "dist=" << dist << vcl_endl;

      //SoNode* node = selected_nodes[0];
      vcl_map<obj_observable *, SoSeparator*>::iterator iter = objects.begin();
      while (iter != objects.end()) {
        SoSeparator* sep = iter->second;
        if (node_selected_ == sep) {
          obj_observable *obs = iter->first;
          obs->move_normal_dir(dist);
        }
        iter++;
      }
      start_y_ = y;
    }
    
    return true;
  } else if (e.type == vgui_BUTTON_DOWN && e.button == vgui_RIGHT && e.modifier == vgui_SHIFT) {
      //vcl_cout << "4" << vcl_endl;
      middle_button_down_ = true;
      start_x_ = e.wx;
      start_y_ = e.wy;
      
  } else if (e.type == vgui_MOTION && e.button == vgui_RIGHT /*middle_button_down_ == true*/ && e.modifier == vgui_SHIFT) {
      //vcl_cout << "5" << vcl_endl;
      // update these only if there is motion event
      float wx = e.wx;
      float wy = e.wy;
      
      // first make sure that an object is selected from scene graph
      if (node_selected_){//(selected_nodes.size() > 0) {
        double x = e.wx;
        double y = e.wy;
        //double dist = vcl_sqrt((x-start_x_)*(x-start_x_) + (y-start_y_)*(y-start_y_))/20;
        double dist = (y-start_y_)/2;
        vcl_cout << dist << vcl_endl;
        start_y_ = y;
        //if (y > start_y_)
        //  dist *= -1;
        vcl_cout << "dist=" << dist << "= " << y << " - " << start_y_ << vcl_endl;

        //obj_observable *obs = find_selected_obs(face_id);
        if (obs_picked_)
            obs_picked_->move_extr_face(dist);
      }
      return true;
    } 
    else if (e.type == vgui_BUTTON_UP && e.button == vgui_RIGHT && e.modifier == vgui_SHIFT){
      ///vcl_cout << "6" << vcl_endl;
      middle_button_down_ = false;
      return true;
    }
  //vcl_cout << "7" << vcl_endl;
  return bgui3d_examiner_tableau::handle(e);
}

obj_observable* poly_coin3d_observer::find_selected_obs(int &fid)
{ 
  vcl_map<obj_observable *, SoSeparator*>::iterator iter = objects.begin();
  while (iter != objects.end()) {
    SoSeparator* sep = iter->second;
    if (node_selected_ == sep) {
      obj_observable *obs = iter->first;
      // find the face index 
      //SbVec3f point = point_picked_->getPoint();
      //float x,y,z;
      //point.getValue(x,y,z);
      //vcl_cout << "[" << point_3d_.x() << "," << point_3d_.y() << "," << point_3d_.z() << "]" << vcl_endl;
      fid = obs->find_closest_face(point_3d_/*vgl_point_3d<double> (x,y,z)*/);
      //vcl_cout << "CLOSEST FACE=" << fid << vcl_endl;
      return obs;
    }
    iter++;
  }
  return 0;
}

//: given the points l1 and l2, finds the exact intersection points on the face with face_id
bool poly_coin3d_observer::find_intersection_points(int id, 
                                                    vgl_point_3d<double> i1, 
                                                    vgl_point_3d<double> i2,
                                                    vgl_point_3d<double> &p1, 
                                                    vgl_point_3d<double>& l1, vgl_point_3d<double>& l2, // end points of the first polygon segment
                                                    vgl_point_3d<double> &p2,
                                                    vgl_point_3d<double>& l3, vgl_point_3d<double>& l4) // end points of the second polygon segment)
{
  vsol_polygon_3d_sptr poly3d = obs_picked_->extract_face(id);

  double *x_list, *y_list, *z_list;
  get_vertices_xyz(poly3d, &x_list, &y_list, &z_list);
  double point1_x, point1_y, point1_z, point2_x, point2_y, point2_z;

  int edge_index1 = vgl_closest_point_to_closed_polygon(point1_x, point1_y, point1_z, 
      x_list, y_list, z_list, poly3d->size(),
      i1.x(), i1.y(), i1.z());
    
  int edge_index2 = vgl_closest_point_to_closed_polygon(point2_x, point2_y, point2_z, 
      x_list, y_list, z_list, poly3d->size(),
      i2.x(), i2.y(), i2.z());

  if (edge_index1 == edge_index2) {
    vcl_cerr << "poly_coin3d_observer::find_intersection_points() -- Both points are on the same edge!!!" << vcl_endl;
    return false;
  }

  l1 = vgl_point_3d<double> (x_list[edge_index1], y_list[edge_index1], z_list[edge_index1]);
  int next_index = edge_index1+1;
  if (next_index == poly3d->size()) 
      next_index = 0;
    l2 = vgl_point_3d<double> (x_list[next_index], y_list[next_index], z_list[next_index]); 
    l3 = vgl_point_3d<double> (x_list[edge_index2], y_list[edge_index2], z_list[edge_index2]);
    next_index = edge_index2+1;
    if (edge_index2+1 == poly3d->size()) 
      next_index = 0;
    l4 = vgl_point_3d<double> (x_list[next_index], y_list[next_index], z_list[next_index]); 
    p1 = vgl_point_3d<double>(point1_x, point1_y, point1_z);
    p2 = vgl_point_3d<double>(point2_x, point2_y, point2_z);
    return true;
}

vcl_string poly_coin3d_observer::create_mesh_name()
{
   vcl_string base = "mesh";

   vcl_stringstream strm;
   strm << vcl_fixed << num_meshes_;
   vcl_string str(strm.str());
   num_meshes_++;
   return (base+str);
}
void poly_coin3d_observer::get_vertices_xyz(vsol_polygon_3d_sptr poly3d, 
                                          double **x, double **y, double **z)
{
  int n = poly3d->size();
  *x = (double*) malloc(sizeof(double) * n);
  *y = (double*) malloc(sizeof(double) * n);
  *z = (double*) malloc(sizeof(double) * n);
  for (int i=0; i<n; i++) {
    (*x)[i] = poly3d->vertex(i)->x();
    (*y)[i] = poly3d->vertex(i)->y();
    (*z)[i] = poly3d->vertex(i)->z();
    //vcl_cout << i << " " << *(poly3d->vertex(i)) << vcl_endl;
  }
}
