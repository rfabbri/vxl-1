#ifndef legacy_complex_h_
#define legacy_complex_h_

// Simple complex number class: use hardcoded version later
class complex
{
 private:
  double real;  // Real Part
  double imag;  //  Imaginary Part
 public:
  complex();
  complex(double);
  complex(double, double);
  complex(const complex&);
  complex(gmp_CC);
  complex operator+(complex);
  complex operator-(complex);
  complex operator-() const;
  complex operator*(complex);
  complex operator/(complex);
  complex& operator+=(const complex);
  complex getconjugate() const;
  complex getreciprocal();
  double getmodulus();
  double getreal() const;
  double getimaginary() const;
  bool operator==(complex);
  void operator=(complex);
  void sprint(char*);
};

//                                        CONSTRUCTOR
inline complex::complex() {}
inline complex::complex(double r)
{
  real = r;
  imag = 0;
}

inline complex::complex(double r, double im)
{
  real = r;
  imag = im;
}

//                                 COPY CONSTRUCTOR
inline complex::complex(const complex& c)
{
  this->real = c.real;
  this->imag = c.imag;
}

inline complex::complex(gmp_CC mpfrCC)
{
  real = mpfr_get_d(mpfrCC->re, GMP_RNDN);
  imag = mpfr_get_d(mpfrCC->im, GMP_RNDN);
}

inline void complex::operator=(complex c)
{
  real = c.real;
  imag = c.imag;
}

inline complex complex::operator+(complex c)
{
  complex tmp;
  tmp.real = this->real + c.real;
  tmp.imag = this->imag + c.imag;
  return tmp;
}

inline complex& complex::operator+=(const complex c)
{
  this->real += c.real;
  this->imag += c.imag;
  return *this;
}

inline complex complex::operator-(complex c)
{
  complex tmp;
  tmp.real = this->real - c.real;
  tmp.imag = this->imag - c.imag;
  return tmp;
}

inline complex complex::operator-() const
{
  complex tmp;
  tmp.real = -this->real;
  tmp.imag = -this->imag;
  return tmp;
}

inline complex complex::operator*(complex c)
{
  complex tmp;
  tmp.real = (real * c.real) - (imag * c.imag);
  tmp.imag = (real * c.imag) + (imag * c.real);
  return tmp;
}

inline complex complex::operator/(complex c)
{
  double div = (c.real * c.real) + (c.imag * c.imag);
  complex tmp;
  tmp.real = (real * c.real) + (imag * c.imag);
  tmp.real /= div;
  tmp.imag = (imag * c.real) - (real * c.imag);
  tmp.imag /= div;
  return tmp;
}

inline complex complex::getconjugate() const
{
  complex tmp;
  tmp.real = this->real;
  tmp.imag = this->imag * -1;
  return tmp;
}

inline complex complex::getreciprocal()
{
  complex t;
  t.real = real;
  t.imag = imag * -1;
  double div;
  div = (real * real) + (imag * imag);
  t.real /= div;
  t.imag /= div;
  return t;
}

inline double complex::getmodulus()
{
  double z;
  z = (real * real) + (imag * imag);
  z = sqrt(z);
  return z;
}

inline double complex::getreal() const { return real; }
inline double complex::getimaginary() const { return imag; }
inline bool complex::operator==(complex c)
{
  return (real == c.real) && (imag == c.imag) ? 1 : 0;
}

inline void complex::sprint(char* s)
{
  sprintf(s, "(%lf) + i*(%lf)", real, imag);
}

#endif
