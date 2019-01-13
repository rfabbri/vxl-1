// This is gel/gkll/gkll_multi_view_data.h
#ifndef gkll_multi_view_data_
#define gkll_multi_view_data_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Class to hold the matched points through multi views
// \author
//   L. Guichard
//
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim
//--------------------------------------------------------------------------------
#include <vector>
#include <mvl/NViewMatches.h>
#include <vbl/vbl_ref_count.h>
#include <iosfwd>

template <class T>
class gkll_multi_view_data : public vbl_ref_count
{
 public:
  gkll_multi_view_data();
  gkll_multi_view_data(int nbviews);
  gkll_multi_view_data(gkll_multi_view_data<T> const& x)
    : vbl_ref_count(), nbviews_(x.nbviews_), nbfeatures_(x.nbfeatures_),
      size_vect_ft_(x.size_vect_ft_), matchnum_(x.matchnum_),
      closed_track_(x.closed_track_), MVM(x.MVM), all_pts(x.all_pts) {}
  ~gkll_multi_view_data();

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
  int get_nb_views() const { return nbviews_; }

  void remove(int view_num, T match);

  std::ostream& print(std::ostream& str);

 private:
  int nbviews_;
  int nbfeatures_; //!< number of features wishing to be detected
  int size_vect_ft_;//!< the number of features in all images
  int matchnum_; //!< the number of tracks
  bool closed_track_;

  NViewMatches MVM;
  std::vector<T> all_pts;
};

#endif // gkll_multi_view_data_
