#include "buld_exp_stat.h"
#include <fstream>
#include <sstream>
#include <vul/vul_awk.h>

void buld_exp_stat::print_stats()
{
  std::cout << "N+: " << positive_cnt_ << " N-: " << negative_cnt_ << " TP: " << TP_ << " FP: " << FP_ << " TN: " << TN_ << " FN: " << FN_ << std::endl;
  if (all_done())
    std::cout << "confirmed! N+ equals TP + FN and N- equals FP + TN\n";
  else
    std::cout << "problem! exp not finished! N+ does not equal TP + FN or N- does not equal FP + TN\n";
  std::cout << "TPR: " << TPR() << " FPR: " << FPR() << " recall: " << recall() << " precision: " << precision() << std::endl;
}
void buld_exp_stat::print_only_stats()
{
  std::cout << "N+: " << positive_cnt_ << " N-: " << negative_cnt_ << " TP: " << TP_ << " FP: " << FP_ << " TN: " << TN_ << " FN: " << FN_ << std::endl;
}

void buld_exp_stat::print_stats(std::ofstream& of)
{
  of << "N+: " << positive_cnt_ << " N-: " << negative_cnt_ << " TP: " << TP_ << " FP: " << FP_ << " TN: " << TN_ << " FN: " << FN_ << std::endl;
  if (all_done())
    of << "confirmed! N+ equals TP + TN and N- equals FP + TN\n";
  else
    of << "problem! exp not finished! N+ does not equal TP + TN or N- does not equal FP + TN\n";
  of << "TPR: " << TPR() << " FPR: " << FPR() << " recall: " << recall() << " precision: " << precision() << std::endl;
}
//: on the first line, assumes the format, e.g.: 
//  N+: 21 N-: 0 TP: 6 FP: 15 TN: 0 FN: 0
void buld_exp_stat::read_stats(std::ifstream& ifs)
{
  vul_awk awk(ifs);
  for( ; awk ; ++awk ) {
    if ( awk.NF() != 12 ) continue; 
    std::stringstream np1(awk[1]);  np1 >> positive_cnt_;
    std::stringstream np2(awk[3]);  np2 >> negative_cnt_;
    std::stringstream np3(awk[5]);  np3 >> TP_;
    std::stringstream np4(awk[7]);  np4 >> FP_;
    std::stringstream np5(awk[9]);  np5 >> TN_;
    std::stringstream np6(awk[11]);  np6 >> FN_;
    break;
  }
}

//: for some applications it is not possible to find number of true negatives
//properly, in that case TN is not printed.
void buld_exp_stat::print_stats(std::string name, std::ofstream& of, bool print_FN)
{
  if (print_FN)
    of << "<statistics name=\"" << name << "\" TP=\"" << TP_ << "\" FP=\"" << FP_ << "\" TN=\"" << TN_ << "\" FN=\"" << FN_ << "\"></statistics>\n";
  else
    of << "<statistics name=\"" << name << "\" TP=\"" << TP_ << "\" FP=\"" << FP_ << "\" FN=\"" << FN_ << "\"></statistics>\n";
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use buld_exp_stat as a brdb_value
void vsl_b_write(vsl_b_ostream & os, buld_exp_stat const &ph)
{
  std::cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use buld_exp_stat as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & is, buld_exp_stat &ph)
{
  std::cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use buld_exp_stat as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, buld_exp_stat* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new buld_exp_stat();
    vsl_b_read(is, *ph);
  }
  else
    ph = 0;
}

void vsl_b_write(vsl_b_ostream& os, const buld_exp_stat* &ph)
{
  if (ph==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}
