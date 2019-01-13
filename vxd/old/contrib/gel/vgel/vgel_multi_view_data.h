// This is gel/vgel/vgel_multi_view_data.h
#ifndef vgel_multi_view_data_
#define vgel_multi_view_data_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//--------------------------------------------------------------------------------
//:
// \file
// \brief Class to hold the matched points through multi views
// \author
//   L. Guichard
//--------------------------------------------------------------------------------
#include <vector>
#include <iosfwd>
#include <mvl/NViewMatches.h>
#include <vbl/vbl_ref_count.h>

template <class T>
class vgel_multi_view_data: public vbl_ref_count
{
 public:
  vgel_multi_view_data();
  vgel_multi_view_data(int nbviews);
  // copy constructor - compiler-provided one sets ref_count to nonzero which is wrong -PVr
  vgel_multi_view_data(vgel_multi_view_data const& d)
    : vbl_ref_count(), nbviews_(d.nbviews_), nbfeatures_(d.nbfeatures_),
      size_vect_ft_(d.size_vect_ft_), matchnum_(d.matchnum_),
      closed_track_(d.closed_track_), MVM(d.MVM), all_pts(d.all_pts) {}
  ~vgel_multi_view_data();

  void new_track();
  void close_track();

  void set_params(int nbviews);
  void set(int view_num,int matchnum,T);
  void set(int view_num,T);

  void get(int, std::vector<T> &);
  void get(int, int, std::vector<T> &,
           std::vector<T> &);
  void get(int, int, int, std::vector<T> &,
           std::vector<T> &,
           std::vector<T> &);

  bool get_pred_match(int view_num,T obj,T & res);
  int get_nb_views() const {return nbviews_;}

  void remove(int view_num, T match);

  std::ostream& print(std::ostream& str);

 private:
  int nbviews_;
  int nbfeatures_;  //!< number of features wishing to be detected
  int size_vect_ft_;//!< the number of features in all images
  int matchnum_;    //!< the number of tracks
  bool closed_track_;

  NViewMatches MVM;
  std::vector<T> all_pts;
};

#endif // vgel_multi_view_data_
