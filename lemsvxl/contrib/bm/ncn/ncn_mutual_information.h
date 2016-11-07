//this is contrib/bm/ncn_mutual_information.h
#ifndef ncn_mutual_information_h_
#define ncn_mutual_information_h_
//
// \file
// \author Brandon A. Mayer
// \date June 2, 2010
//
// A class to store a mutual information value and the index into the candidate set for 
// sorting and defining a non-compact neighborhood
// 
// \verbatim
// Modifications:
// None Yet.
// \endverbatim

class ncn_mutual_information
{
public:
    ncn_mutual_information(): mi_(0), index_(0){}
    ncn_mutual_information(double mi, unsigned index): mi_(mi),index_(index){}
    double mi() const {return mi_;}
    unsigned index() const {return index_;}
    bool operator < (ncn_mutual_information const& mi) const {return mi_ < mi.mi();}
private:
    double mi_;
    unsigned index_;
};

#endif //ncn_mutual_information_h_