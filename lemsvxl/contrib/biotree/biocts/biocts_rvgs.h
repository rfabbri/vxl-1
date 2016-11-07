// This is contrib/biotree/biocts/biocts_rvgs.h

#ifndef biocts_rvgs_h
#define biocts_rvgs_h

//:
// \file
// \brief
//
// \author
// Rahul is the author.
// Can added the comments (I am not sure if this is a 3rd party library)
//
// \date
// 02/01/05
//
// \verbatim
//  Modifications
// \endverbatim

long Bernoulli(double p);
long Binomial(long n, double p);
long Equilikely(long a, long b);
long Geometric(double p);
long Pascal(long n, double p);
long Poisson(double m);

double Uniform(double a, double b);
double Exponential(double m);
double Erlang(long n, double b);
double Normal(double m, double s);
double Lognormal(double a, double b);
double Chisquare(long n);
double Student(long n);

#endif


