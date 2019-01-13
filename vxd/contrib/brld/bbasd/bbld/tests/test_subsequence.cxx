#include <testlib/testlib_test.h>
#include <bbld/bbld_subsequence.h>
#include <functional>

class is_less_than_zero : public std::unary_function<unsigned, bool> {
public:
  //: delta_theta is effective if it is between 0 and vnl_math::pi/2. 
  is_less_than_zero(
      const std::vector<int> &v)
    : v_(v)
  { }

  bool operator() (unsigned i) const {
    return v_[i] < 0;
  }

private:
  const std::vector<int> &v_;
};

MAIN( test_subsequence )
{
  START ("subsequence");

  //: We will partition this std::vector into all contiguous subsequences whose
  // elements are either all <0, or all >= 0.
  std::vector<int> n;
  n.push_back(0);
  n.push_back(-1);
  n.push_back(4);
  n.push_back(4);
  n.push_back(-3);
  n.push_back(-3);
  n.push_back(-3);

  unsigned num_subsequences_gt = 4;

  //: ground truth indicates which contiguous subsequences are negative
  std::vector<bool> is_negative_truth(4);
  is_negative_truth[0] = false;
  is_negative_truth[1] = true;
  is_negative_truth[2] = false;
  is_negative_truth[3] = true;

  bbld_subsequence_set ss;
  bbld_subsequence orig_seq(0, n.size()); 
  orig_seq.set_orig_id(169);  //< any id to represent n
  bbld_contiguous_partition(orig_seq, is_less_than_zero(n), &ss);

  std::cout << "num subsequences " << ss.num_subsequences() << std::endl;
  TEST("num subsequences", ss.num_subsequences(), num_subsequences_gt);
  TEST("total size of subsequences", ss.total_size(), n.size());
  for (unsigned i=0; i < ss.num_subsequences(); ++i) {
    TEST("subsequence original id", ss[i].orig_id(), 169);
    for (unsigned k = ss[i].ini(); k < ss[i].end(); ++k) {
      TEST("subsequence value ok", n[k] < 0, is_negative_truth[i]);
    }
  }
  
  SUMMARY();
}
