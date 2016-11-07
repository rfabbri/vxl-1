#include "../DPMatch.C"

//template class Curve<double,double>;

typedef Curve<double,double> curvedoubledouble;

template class DPMatch< curvedoubledouble, double >;
