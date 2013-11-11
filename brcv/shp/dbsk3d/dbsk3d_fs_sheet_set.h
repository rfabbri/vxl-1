//: This is dbsk3d_fs_sheet_set.h
//  MingChing Chang
//  Nov 30, 2006.

#ifndef dbsk3d_fs_sheet_set_h_
#define dbsk3d_fs_sheet_set_h_

#include <vcl_map.h>
#include <vcl_queue.h>

#include <dbsk3d/dbsk3d_fs_sheet.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

class dbsk3d_fs_sheet_set
{
protected:
  //: The set of shock sheets (components) stored in a map.
  vcl_map<int, dbsk3d_fs_sheet*>  sheetmap_;
  dbsk3d_fs_mesh*   fs_mesh_;
  int               fs_sheet_id_counter_;

public:
  //####### Constructor/Destructor #######
  dbsk3d_fs_sheet_set (dbsk3d_fs_mesh* fs_mesh) { 
    fs_mesh_ = fs_mesh;
    fs_sheet_id_counter_ = 0;
  }

  void clear() {
    vcl_map<int, dbsk3d_fs_sheet*>::iterator it = sheetmap_.begin();
    for (; it != sheetmap_.end(); it++) {
      delete (*it).second;
    }  
    sheetmap_.clear();
    fs_sheet_id_counter_ = 0;
  }

  virtual ~dbsk3d_fs_sheet_set () {
    sheetmap_.clear();
  }

  //####### Data Access #######  
  dbsk3d_fs_sheet* sheetmap (const int i) {
    vcl_map<int, dbsk3d_fs_sheet*>::iterator it = sheetmap_.find (i);
    if (it == sheetmap_.end())
      return NULL;
    return (*it).second;
  }
  vcl_map<int, dbsk3d_fs_sheet*>& sheetmap() {
    return sheetmap_;
  }
  dbsk3d_fs_mesh* fs_mesh () const {
    return fs_mesh_;
  }
  void set_fs_mesh (dbsk3d_fs_mesh* fs_mesh) {
    fs_mesh_ = fs_mesh;
  }
  dbmsh3d_mesh* bnd_mesh() {
    return fs_mesh_->bnd_mesh();
  }

  void add_fs_sheet (dbsk3d_fs_sheet* S) {
    sheetmap_.insert (vcl_pair<int, dbsk3d_fs_sheet*> (S->id(), S));
  }
  void remove_fs_sheet (dbsk3d_fs_sheet* S) {
    sheetmap_.erase (S->id());
  }

  //####### Sheet Component Extraction #######
  void build_fs_sheet_set ();
    void propagate_label_S (dbsk3d_fs_sheet* S, dbsk3d_fs_face* inputP);
    void put_neighbor_FF_to_Q (dbsk3d_fs_sheet* S, dbsk3d_fs_face* FF, 
                               vcl_queue<dbsk3d_fs_face*>& FF_queue);

  //####### Sheet Component Operations #######

  void remove_del_fs_sheet (dbsk3d_fs_sheet* S);

  void merge_fs_sheets (dbsk3d_fs_sheet* S1, dbsk3d_fs_sheet* S2);

  unsigned int remove_empty_sheets ();
  
  void remove_invalid_FF ();
  
  bool check_integrity ();
};

#endif
