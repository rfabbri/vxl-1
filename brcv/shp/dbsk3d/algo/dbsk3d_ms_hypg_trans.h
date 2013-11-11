//: This is dbsk3d_ms_hypg_trans.h
//  Medial Scaffold Hypergraph Transitions Regularization
//  MingChing Chang 2006, 11/17 

#ifndef dbsk3d_hypg_trans_h_
#define dbsk3d_hypg_trans_h_

#include <vcl_map.h>
#include <dbmsh3d/algo/dbmsh3d_fmm.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>

//: Medial scaffold transform types.
#define MS_XFORM_TYPE                 char
#define MS_XFORM_TYPE_BOGUS           '?'
#define MSXT_TAB_SPLICE               'T'
#define MSXT_A5_CURVE_CONTRACT        '5'
#define MSXT_A12A3I_CURVE_CONTRACT    'I'
#define MSXT_A15_CURVE_CONTRACT       'C'
#define MSXT_A15_SHEET_CONTRACT       'S'
#define MSXT_A14_SHEET_CONTRACT       '4'
#define MSXT_A1A3II_N_N_MERGE         'N'
#define MSXT_A12A3I_N_C_MERGE         '1'
#define MSXT_A14_C_C_MERGE            '3'
#define MSXT_A1A3II_C_C_MERGE         '2'
#define MSXT_A1A5_N_C_MERGE           '6'

#define MSXT_FAIL       0
#define MSXT_SKIP       1
#define MSXT_SUCCESS    2

class dbsk3d_ms_hypg_trans
{
protected:
  dbsk3d_ms_hypg*       ms_hypg_;

  //: Greedy MS transition regularization parameters.
  float Wts_;       //weight of tab splice cost.
  float Tts_;       //threshold of tab splice cost.
  float Wcc_;       //weight of curve contract cost.
  float Tcc_;       //threshold of curve contract cost.
  float Wc5_;       //weight of A5 curve contract cost.
  float Tc5_;       //threshold of A5 curve contract cost.
  float Wsc_;       //weight of sheet contract cost.
  float Tsc_;       //threshold of sheet contract cost.
  float Wnnm_;      //weight of node-node merge cost.
  float Tnnm_;      //threshold of node-node merge cost.
  float Wncm_;      //weight of node-curve merge cost.
  float Tncm_;      //threshold of node-curve merge cost.
  float Wccm_;      //weight of curve-curve merge cost.
  float Tccm_;      //threshold of curve-curve merge cost.
  float Tncm_a1a5_; //threshold of A1A5 node-curve merge cost.
  float cmxth_;     //threshold of curve merge transform validity.

  bool  b_merge_xform_;

  //: Priority queue of rank ordered shock transforms.
  vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > > ms_xform_Q_;
  
public:
  //###### Constructor/Destructor ######
  dbsk3d_ms_hypg_trans (dbsk3d_ms_hypg* ms_hypg) {
    ms_hypg_ = ms_hypg;

    Wts_ = -1;
    Tts_ = -1;
    Wcc_ = -1;
    Tcc_ = -1;
    Wc5_ = -1;
    Tc5_ = -1;
    Wsc_ = -1;
    Tsc_ = -1;
    Wnnm_ = -1;
    Tnnm_ = -1;
    Wncm_ = -1;
    Tncm_ = -1;
    Wccm_ = -1;
    Tccm_ = -1;
    Tncm_a1a5_ = -1;
    cmxth_ = -1;

    b_merge_xform_ = true;
  }
  virtual ~dbsk3d_ms_hypg_trans () {
    ms_xform_Q_.clear();
  }

  //###### Data access functions ######
  vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >& ms_xform_Q() {
    return ms_xform_Q_;
  }
  dbsk3d_ms_hypg* ms_hypg () {
    return ms_hypg_;
  }
  dbsk3d_fs_mesh* fs_mesh() {
    return ms_hypg_->fs_mesh();
  }
  void set_ms_hypg (dbsk3d_ms_hypg* ms_hypg) {
    ms_hypg_ = ms_hypg;
  }
  bool b_merge_xform () const {
    return b_merge_xform_;
  }
  void set_b_merge_xform (const bool b) {
    b_merge_xform_ = (bool) b;
  }
  const float cmxth () const {
    return cmxth_;
  }

  //###### Medial scaffold transition regularization ######
  //: Setup parameters for MS transition regularization.
  void ms_trans_regul_set_params (const float Wts, const float Tts,
                                  const float Wcc, const float Tcc,
                                  const float Wc5, const float Tc5,
                                  const float Wsc, const float Tsc,
                                  const float Wnnm, const float Tnnm,
                                  const float Wncm, const float Tncm,
                                  const float Wccm, const float Tccm,
                                  const float Tncm_a1a5,
                                  const float cmxth);

  //: Initialization the greedy iteration of medial scaffold regularization.
  void ms_trans_regul_init ();

  //: Medial scaffold regularization by applying shock transforms 
  //  across transitions (topological changes) in a greey iteration.
  void ms_trans_regul_iters (const int debug_stop_id1 = -1, const int debug_stop_id2 = -1);

  //###### Medial scaffold transform functions for general cases ######
  
  //: A1A3-I or A12A3-II Splice transform on ms_sheet.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int sheet_splice_xform (dbsk3d_ms_sheet* MS, const bool modify_bnd);

  //: A5 Contract transform on ms_curve (and neighboring swallow-tail.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A5_curve_contract_xform (dbsk3d_ms_curve* MC, const bool modify_bnd);

  //: A12A3-I curve-contract transform.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A12A3I_curve_contract_xform (dbsk3d_ms_curve* MC, const bool modify_bnd);

  //: A15 curve-contract transform.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A15_curve_contract_xform (dbsk3d_ms_curve* MC, const bool modify_bnd);

  //: A15 sheet-contract transform.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A15_sheet_contract_xform (dbsk3d_ms_sheet* MS, const bool modify_bnd);
  
  //: A14 sheet-contract transform.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A14_sheet_contract_xform (dbsk3d_ms_sheet* MS, const bool modify_bnd);
  
  //: A1A3-II node-node merge transform.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A1A3II_n_n_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_node* MN2, 
                              const float cost, const bool modify_bnd);

  //: A12A3-I node-curve merge transform.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A12A3I_n_c_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2, 
                              const float cost, const bool modify_bnd);

  //: A14 curve-curve merge transform.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A14_c_c_merge_xform (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                           const float cost, const bool modify_bnd);

  //: A1A3-II curve-curve merge transform.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A1A3II_c_c_merge_xform (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                              const float cost, const bool modify_bnd);

  //: A1A5 node-curve merge transform.
  //  return 0: failed, 1: skipped, 2: successfully done.
  int A1A5_n_c_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2, 
                            const float cost, const bool modify_bnd);

  //###### Functions for hybrid case involving two or more xforms ######

  void MC_A5_trim_single_L (dbsk3d_ms_curve* MC);


  //###### Medial scaffold transition queue handling ######
  void try_add_MS_to_Q (dbsk3d_ms_sheet* MS);
  void try_add_MS_splice_to_Q (dbsk3d_ms_sheet* MS);
  void try_add_MS_A15_contract_to_Q (dbsk3d_ms_sheet* MS);
  void try_add_MS_A14_contract_to_Q (dbsk3d_ms_sheet* MS);

  void try_add_MC_to_Q (dbsk3d_ms_curve* MC);
  void try_add_MC_A5_contract_to_Q (dbsk3d_ms_curve* MC);
  void try_add_MC_A12A3I_contract_to_Q (dbsk3d_ms_curve* MC);
  void try_add_MC_A15_contract_to_Q (dbsk3d_ms_curve* MC);
  void try_add_A12A3I_n_c_merge_to_Q (dbsk3d_ms_curve* MC);
  void try_add_A14_c_c_merge_to_Q (dbsk3d_ms_curve* MC);
  void try_add_A1A3II_c_c_merge_to_Q (dbsk3d_ms_curve* MC);
  void try_add_A1A5_n_c_merge_to_Q (dbsk3d_ms_curve* MC);
  
  void try_add_MN_to_Q (dbsk3d_ms_node* MN);
  void try_add_A1A3II_n_n_merge_to_Q (dbsk3d_ms_node* MN);
  void try_add_A12A3I_n_c_merge_to_Q (dbsk3d_ms_node* MN);
  void try_add_A1A5_n_c_merge_to_Q (dbsk3d_ms_node* MN);
  
  void try_add_modified_to_Q (vcl_set<dbsk3d_ms_sheet*>& modified_MS_set, 
                              vcl_set<dbsk3d_ms_curve*>& modified_MC_set, 
                              vcl_set<dbsk3d_ms_node*>& modified_MN_set);  
  
  void _add_xform_to_Q (const float cost, const char type, const void* data1, const void* data2) {
    assert (data1 != NULL || data2 != NULL);
    vcl_pair<void*, void*> data ((void*) data1, (void*) data2);
    vcl_pair<char, vcl_pair<void*, void*> > xform (type, data);
    ms_xform_Q_.insert (vcl_pair<float, vcl_pair<char, vcl_pair<void*, void*> > > (cost, xform));  
  }

  void _pop_xform_from_Q (float& cost, char& type, dbsk3d_ms_sheet* &MS, 
                          dbsk3d_ms_curve*& MC1, dbsk3d_ms_curve*& MC2,
                          dbsk3d_ms_node*& MN1, dbsk3d_ms_node*& MN2);

  //: Brutely search the ms_xform_Q[] for MS and remove the entry.
  void _remove_MS_from_Q (const dbsk3d_ms_sheet* MS);

  //: Brutely search the ms_xform_Q[] for MC and remove the entry.
  void _remove_MC_from_Q (const dbsk3d_ms_curve* MC);

  //: Brutely search the ms_xform_Q[] for MN and remove the entry.
  void _remove_MN_from_Q (const dbsk3d_ms_node* MN);

  bool _check_integrity_Q ();

  void _print_Q ();

  //: Add virtual curves from the detected MS transition queue.
  void add_trans_virtual_curves ();

  int add_vl_sheet_splice (dbsk3d_ms_sheet* MS);
  int add_vl_A5_curve_contract (dbsk3d_ms_curve* MC);
  int add_vl_A12A3I_curve_contract (dbsk3d_ms_curve* MC);
  int add_vl_A15_curve_contract (dbsk3d_ms_curve* MC);
  int add_vl_A15_sheet_contract (dbsk3d_ms_sheet* MS);
  int add_vl_A14_sheet_contract (dbsk3d_ms_sheet* MS);
};

#endif
