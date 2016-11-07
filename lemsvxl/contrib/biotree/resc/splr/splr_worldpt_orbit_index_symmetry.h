#if !defined(SPLR_WORLDPT_ORBIT_INDEX_SYMMETRY_H_)
#define SPLR_WORLDPT_ORBIT_INDEX_SYMMETRY_H_)

class worldpt_orbit_index_symmetry {
  public:
  //modifies the orbit index to be canonical,
  //and returns the transformation that
  //moves a worldpt into the canonical set
  virtual worldpt_transformation operator()(orbit_index &t) = 0;
}
#endif
