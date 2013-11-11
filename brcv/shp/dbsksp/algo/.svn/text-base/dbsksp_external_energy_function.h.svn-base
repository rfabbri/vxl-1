// This is shp/dbsksp/dbsksp_external_energy_function.h
#ifndef dbsksp_external_energy_function_h_
#define dbsksp_external_energy_function_h_

//:
// \file
// \brief A class to computer external energy of a shock graph
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 17, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <vil/vil_image_view.h>
#include <dbsksp/dbsksp_shapelet.h>




// ============================================================================
// dbsksp_external_energy_function
// ============================================================================

//: A vector representing the free parameters of a shock graph
class dbsksp_external_energy_function
{
public:

  // CONSTRUCTORS / DESTRUCTORS -----------------------------------------------
  //: Constructor
  dbsksp_external_energy_function();
  
  //: Destructor
  virtual ~dbsksp_external_energy_function(){};

  // DATA ACCESS --------------------------------------------------------------
  

  // MAIN FUNCTIONS -----------------------------------------------------------
  //:  The main function. Compute external energy for a shock graph
  virtual double f(const dbsksp_shock_graph_sptr& x) = 0;

  //: Compute external energy for a shapelet
  virtual double f(const dbsksp_shapelet_sptr& s) = 0;

  //: Integrate energy along each boundary separately
  // which_side = 0 for left boundary, = 1 for right boundary
  virtual double f(const dbsksp_shapelet_sptr& s, int which_side) = 0;


  // MEMBER VARIABLES ---------------------------------------------------------
protected:
  
};


// ============================================================================
// dbsksp_potential_energy_function
// ============================================================================
//: Cost function for a shock snake, given a potential field (e.g. dt)
class dbsksp_potential_energy_function: public dbsksp_external_energy_function
{
  // CONSTRUCTORS / DESTRUCTORS ------------------------------------------------
public:
  //: Constructor
  dbsksp_potential_energy_function();

  //: Destructor
  virtual ~dbsksp_potential_energy_function();


  // DATA ACCESS --------------------------------------------------------------
  


  //: Get and Set the sampling length
  double sampling_length() const {return this->sampling_length_; }
  void set_sampling_length(double ds){ this->sampling_length_ = ds; }

  //: Get and Set potential image
  const vil_image_view<float >& potential_field() const 
  {return this->potential_field_; }
  
  void set_potential_field(const vil_image_view<float >& potential_field);

  //: Return potential at a particular position
  double potential_at(double x, double y) const;


  double grad_x(double x, double y) const;
  double grad_y(double x, double y) const;
  

  // THE MAIN FUNCTION --------------------------------------------------------
  
  //:  The main function. Compute external energy given a shock graph
  virtual double f(const dbsksp_shock_graph_sptr& x);
  virtual double f(const dbsksp_shapelet_sptr& shapelet);

  //: Integrate energy along each boundary separately
  // which_side = 0 for left boundary, = 1 for right boundary
  virtual double f(const dbsksp_shapelet_sptr& s, int which_side) {return 0;};

  // MEMBER VARIABLES ---------------------------------------------------------
protected:
  
  double sampling_length_;

  //: potential energy image
  vil_image_view<float > potential_field_;
  vil_image_view<float > grad_x_;
  vil_image_view<float > grad_y_;

};





//: Cost function for a shock snake, given a potential field (e.g. dt)
class dbsksp_no_alignment_potential_energy_function: public dbsksp_potential_energy_function
{

// ------------------ CONSTRUCTORS / DESTRUCTORS ------------------  
public:
  //: Constructor
  dbsksp_no_alignment_potential_energy_function(): dbsksp_potential_energy_function(), d_min_(-1), d_max_(1e10)
  {
  }

  //: Destructor
  virtual ~dbsksp_no_alignment_potential_energy_function(){}  

  // ------------------ THE MAIN FUNCTION ------------------
  
  //:  The main function. Compute external energy given a shock graph
  //virtual double f(const dbsksp_shock_graph_sptr& x);
  virtual double f(const dbsksp_shapelet_sptr& shapelet);

  //: Integrate energy along each boundary separately
  // which_side = 0 for left boundary, = 1 for right boundary
  virtual double f(const dbsksp_shapelet_sptr& s, int which_side){return 0;};

  double d_min_;
  double d_max_;

  // ------------------ MEMBER VARIABLES ------------------
protected:
};






// ============================================================================
// dbsksp_potential_integrator
// ============================================================================

//: Potential energy integrator - sum up energy along the object
class dbsksp_potential_integrator : public dbsksp_external_energy_function
{
public:

  // --------------------------------------------------------------------------
  //                  CONSTRUCTORS / DESTRUCTORS 
  // --------------------------------------------------------------------------
  //: Constructor
  dbsksp_potential_integrator() : dbsksp_external_energy_function(),
    sampling_length_(1){};
  
  //: Destructor
  virtual ~dbsksp_potential_integrator(){};


  // --------------------------------------------------------------------------
  //                  DATA ACCESS
  // --------------------------------------------------------------------------
  
  //: Get and Set the sampling length
  double sampling_length() const {return this->sampling_length_; }
  void set_sampling_length(double ds){ this->sampling_length_ = ds; }

  //: Get and Set potential image
  const vil_image_view<float >& potential_field() const 
  {return this->potential_field_; }
  void set_potential_field(const vil_image_view<float >& potential_field)
  { 
    this->potential_field_ = potential_field; 
    this->ni_ = potential_field.ni(); 
    this->nj_ = potential_field.nj();
  }
  

  // --------------------------------------------------------------------------
  //                  MAIN FUNCTIONS
  // --------------------------------------------------------------------------

  //: Integrate energy along a shock graph
  virtual double f(const dbsksp_shock_graph_sptr& x);

  //: Integrate energy along the two boundary of a shapelet
  virtual double f(const dbsksp_shapelet_sptr& s);

  //: Integrate energy along each boundary separately
  // which_side = 0 for left boundary, = 1 for right boundary
  virtual double f(const dbsksp_shapelet_sptr& s, int which_side);

  //: Sum of energy at a set of points
  float f(const vcl_vector<vgl_point_2d<double > >& pts, float default_value = 0);

  // --------------------------------------------------------------------------
  //                  SUPPORT FUNCTIONS
  // --------------------------------------------------------------------------

  //: Return potential at a particular position
  double potential_at(double x, double y) const;


  // --------------------------------------------------------------------------
  //                  MEMBER VARIABLES
  // --------------------------------------------------------------------------

protected:
  //: sampling rate
  double sampling_length_;

  //: default value when the sampled point is outside the image
  double default_value_;

  // size of the image
  int ni_;
  int nj_;

  //: potential energy image
  vil_image_view<float > potential_field_;
  
};

#endif // shp/dbsksp/dbsksp_external_energy_function.h









