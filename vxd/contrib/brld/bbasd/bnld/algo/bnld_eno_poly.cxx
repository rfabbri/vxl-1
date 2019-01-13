#include "bnld_eno.h"
//:
// \file
// \author Based on original code by  Ricardo Fabbri

void bnld_eno_poly::
print(std::ostream &strm) const
{
   strm << "==== Polynomial ====" << std::endl
        << "order: " << order() << std::endl
        << "coefficients: ";

   for (unsigned i=0; i<=order(); ++i)
      strm << coeff(i) << " ";
   strm << std::endl;
}

bnld_eno_poly operator-(const bnld_eno_poly &f1, const bnld_eno_poly &f2)
{
   bnld_eno_poly diff_poly(2);

   diff_poly[f1.second_order_index] = f1[f1.second_order_index] - f2[f1.second_order_index];
   diff_poly[f1.first_order_index]  = f1[f1.first_order_index] - f2[f1.first_order_index];
   diff_poly[f1.zero_order_index]   = f1[f1.zero_order_index] - f2[f1.zero_order_index];

   return diff_poly;
}

double bnld_eno_poly::
sample(double x) const
{ 
   return x*(coeffs_[second_order_index]*x + coeffs_[first_order_index]) 
          + coeffs_[zero_order_index];
}

// TODO
// - an eval method would be useful for testing...
// - use a class from vnl?
