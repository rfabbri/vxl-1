//: This is lemsvxlsrc/brcv/shp/dbskr/pro/dbskr_process.h
//  Revision: Dec 24, 2005   Ming-Ching Chang

#ifndef dbsk3dr_process_h_
#define dbsk3dr_process_h_

#include <dbmsh3dr/pro/dbmsh3dr_process.h>
#include <dbsk3d/pro/dbsk3d_process.h>

#include <dbsk3dr/dbsk3dr_match.h>
#include <dbsk3dr/pro/dbsk3dr_pro_cmdpara.h>

class dbsk3dr_pro : public dbmsh3dr_pro_base
{
protected:
  dbsk3dr_match*   shock_match_;

public:
  //====== Constructor/Destructor ======
  dbsk3dr_pro (dbsk3d_pro* p1, dbsk3d_pro* p2) : dbmsh3dr_pro_base (p1, p2) {
    shock_match_ = new dbsk3dr_match ();
  }

  virtual ~dbsk3dr_pro () {
    delete shock_match_;
  }

  //====== Data access functions ======
  dbsk3d_pro* p0 () {
    return (dbsk3d_pro*) pro_[0];
  }
  dbsk3d_pro* p1 () {
    return (dbsk3d_pro*) pro_[1];
  }
  dbsk3dr_match* shock_match() {
    return shock_match_;
  }

  //====== Shock Matching Process ======
  //option 1: shock graph matching.
  //option 2: shock hypergraph matching using curve length as compatibility.
  //option 3: shock hypergraph matching using curve sum radius over length as compatibility.
  //option 4: shock hypergraph matching using D.P. curve distance as compatibility.
  //option 5: shock hypergraph matching using D.P. shock curve distance as compatibility.
  //option 6: option 5 + plus integration of the global R in node compatibility.
  //option 7: option 3 plus integration of the global (R, T) in node compatibility.
  // Return false if numerical problem occurs.
  bool run_shock_match (const int option, const bool register_rigid,
                        const bool match_subset_of_curves,
                        int max_ms_curves);

  bool save_shock_match_result_file (const vcl_string& filename, const int verbose = 1);

  bool run_shock_match_icp (const int max_iter, const float conv_th);
};

#endif
