//: This is dbsk3d_ms_node.h
//  MingChing Chang
//  Nov 23, 2004.

#ifndef dbsk3d_ms_node_h_
#define dbsk3d_ms_node_h_

#include <vcl_vector.h>

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_node.h>
#include <dbsk3d/dbsk3d_fs_vertex.h>

class dbsk3d_ms_curve;
class dbsk3d_ms_sheet;
class dbsk3d_fs_mesh;
class dbsk3d_ms_hypg;
class dbsk3d_sg_sa;

//###############################################################
//       dbsk3d_ms_node
//###############################################################


class dbsk3d_ms_node : public dbmsh3d_node
{
protected:
  float   radius_;
  bool    selected_;

public:
  //###### Constructor/Destructor ######
  dbsk3d_ms_node (int id) : dbmsh3d_node (id) {
    radius_ = -1.0f;
    selected_ = false;
  }

  virtual ~dbsk3d_ms_node () {
  }

  //###### Data access functions ######
  const int id () const {
    return id_;
  }
  void set_id (const int id) {
    id_ = id;
  }
  const bool selected() const {
    return selected_;
  }
  void set_selected (const bool s) {
    selected_ = bool(s);
  }

  dbsk3d_fs_vertex* FV() const {
    return (dbsk3d_fs_vertex*) V_;
  }
  dbsk3d_fs_vertex* FV() {
    return (dbsk3d_fs_vertex*) V_;
  }

  const vgl_point_3d<double> pt() const { 
    return V_->pt ();
  }
  float radius() const {
    return radius_;
  }
  void set_radius (const float r) {
    radius_ = r;
  }

  //###### Connectivity Query Functions ######
  bool A14_get_other_2_MCs (const dbsk3d_ms_curve* MC1, const dbsk3d_ms_curve* MC2,
                            dbsk3d_ms_curve*& MC3, dbsk3d_ms_curve*& MC4) const;
  
  bool compute_node_prop (float& grad_r_max, float& grad_r_min,
                          float& corner_a_max, float& corner_a_min);

  //###### Modification Functions ######
  void compute_radius() {
    radius_ = (float) FV()->compute_time();
  }

  //###### Other functions ######
  virtual bool check_integrity ();

  //: The clone function clones everything except the connectivity
  //  when cloning a graph, need to build the connectivity separetely.
  virtual dbmsh3d_vertex* clone () const;
  virtual dbsk3d_ms_node* clone (dbmsh3d_mesh* M2) const;

  virtual void getInfo (vcl_ostringstream& ostrm);
  
  //###### Handle Incident Virtual Curves ######
  bool A14_get_other_2_MCs_nv (const dbsk3d_ms_curve* MC1, const dbsk3d_ms_curve* MC2,
                               dbsk3d_ms_curve* & MC3, dbsk3d_ms_curve* & MC4) const;  

};


//###############################################################
//       dbsk3d_ms_node vcl_FILE READING CLASS
//###############################################################

class ms_node_file_read
{
public:
  int                   id_;
  //: Ids for the incident shock scaffold curves
  vcl_vector<int>       SCids_;
};

//###############################################################
//       dbsk3d_ms_node TEXT FILE I/O FUNCTIONS
//###############################################################

//: File I/O for FS/SHG
void mn_save_text_file (vcl_FILE* fp, dbsk3d_ms_node* MN);
void mn_load_text_file (vcl_FILE* fp, dbsk3d_ms_node* MN, 
                        ms_node_file_read* mn_fileread, dbsk3d_fs_mesh* fs_mesh);
void mn_recover_pointers (dbsk3d_ms_node* MN, ms_node_file_read* mn_fileread,
                          dbsk3d_ms_hypg* shg);

//: File I/O for stand alone SG
void mn_save_text_file_sg (vcl_FILE* fp, dbsk3d_ms_node* MN);
void mn_load_text_file_sg (vcl_FILE* fp, dbsk3d_ms_node* MN, ms_node_file_read* mn_fileread,
                           dbsk3d_sg_sa* sg_sa);

void mn_load_text_file_sg_old (FILE* fp, dbsk3d_ms_node* MN, 
                               ms_node_file_read* mn_fileread, dbsk3d_sg_sa* sg_sa);

void mn_recover_pointers_sg (dbsk3d_ms_node* MN, ms_node_file_read* mn_fileread, dbsk3d_sg_sa* sg_sa);

//###### Clone functions ######
//
void clone_ms_vertex_sg_sa (dbsk3d_ms_node* targetMN, dbsk3d_ms_node* inputMN,
                            dbsk3d_sg_sa* targetSG);

#endif
