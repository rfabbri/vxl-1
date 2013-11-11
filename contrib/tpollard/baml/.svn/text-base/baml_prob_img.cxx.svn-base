#ifndef _baml_prob_img_cxx_
#define _baml_prob_img_cxx_

#include "baml_prob_img.h"

#include <vcl_cassert.h>
#include <vil/vil_save.h>

vcl_ofstream baml_prob_img::norm_record = vcl_ofstream( "D:/norm_record.txt" );

//-------------------------------------------------
baml_prob_img::baml_prob_img()
{
  d_ = NULL;
  initialize( 1, 1 );
};


//-------------------------------------------------
baml_prob_img::baml_prob_img( unsigned ni, unsigned nj )
{
  d_ = NULL;
  initialize( ni, nj );
};


//---------------------------------------------------
baml_mog*
baml_prob_img::operator()( unsigned i, unsigned j )
{
  unsigned index = (i+j*ni_)*mog_size_;
  baml_mog* new_mog = new baml_mog( num_mixtures_ );
  new_mog->set_component( 0, d_ + index );
  return new_mog;
};


//-------------------------------------------------------
float 
baml_prob_img::prob(
  const vil_image_view<float>& img )
{
  assert( img.ni() == ni_ && img.nj() == nj_ );

  double total_prob = (float)0.0;
  double total_weight = 0;
  for( unsigned i = 0; i < ni_; i++ )
    for( unsigned j = 0; j < nj_; j++ ){
      baml_mog* new_mog = (*this)(i,j);
      if( !new_mog->is_untrained() ){
        double this_weight = 1.0;
        float this_prob = new_mog->prob( (float)(img(i,j)/255.0), vnl_vector<float>() );
        //total_prob += this_prob*this_weight;
        //total_weight+= this_weight;
        if( this_prob > 1.0 ) total_prob += 1.0;
        total_weight += 1.0;
      }
      delete new_mog;
    }
  if( total_weight == 0 ) return -1;
  return (float)(total_prob/total_weight);
};


//------------------------------------------------------
void 
baml_prob_img::normalize_img(
  const vil_image_view<float>& img,
  float& a, float& b )
{
  // Calculate optimal normalization parameters.
  a = (float)1.0; b = (float)0.0;
  float best_prob = 0.0;
  for( float sa = 1.0; sa <= 1.61; sa+=(float).05 ){
    float sb_best = 0.0;
    for( float sb_inc = 100; sb_inc > 5; sb_inc *= .5 ){
      float sb_best_prob = 0.0;
      for( float sb = sb_best-2*sb_inc; sb <= sb_best+2.01*sb_inc; sb+= sb_inc ){
        vil_image_view<float> nimg( img.ni(), img.nj() );
        for( unsigned i = 0; i < nimg.ni(); i++ ){
          for( unsigned j = 0; j < nimg.nj(); j++ ){
            int new_pixel = (int)(sb+sa*img(i,j));
            if( new_pixel < 0 ) new_pixel = 0;
            if( new_pixel > 255 ) new_pixel = 255;
            nimg(i,j) = (float)new_pixel;
          }
        }
        float this_prob = prob( nimg );
        vcl_cerr << this_prob << ' ';
        if( this_prob < 0 ) return;
        if( this_prob > sb_best_prob ){ sb_best_prob = this_prob; sb_best = sb; }
        if( this_prob > best_prob ){ best_prob = this_prob; a = sa; b = sb; }
      } 
      vcl_cerr << '\n';
    }
    vcl_cerr << '\n';
  }
  norm_record << a << ' ' << b << '\n';
};


//-------------------------------------------------------
void 
baml_prob_img::draw_best( 
  vcl_string img_file )
{
  vil_image_view<vxl_byte> img( ni_, nj_ );
  for( unsigned i = 0; i < ni_; i++ )
    for( unsigned j = 0; j < nj_; j++ ){
      baml_mog* new_mog = (*this)(i,j);
      int best_color = (int)floor(new_mog->best_color()*255.0);
      if( !(best_color >= 0) ) best_color = 0;
      if( !(best_color < 256) ) best_color = 255;
      img(i,j) = best_color;
      delete new_mog;
    }
  vil_save( img, img_file.c_str() );
};


//--------------------------------------------------
void 
baml_prob_img::write( 
  vcl_string file_name )
{
  vcl_ofstream s( file_name.c_str(), vcl_ifstream::binary );
  if( s.good() ){
    char* byte_cache;
    unsigned nx[1];
    nx[0] = ni_; byte_cache = (char*)nx; s.write( byte_cache, sizeof(unsigned) );
    nx[0] = nj_; byte_cache = (char*)nx; s.write( byte_cache, sizeof(unsigned) );
    byte_cache = (char*)d_; s.write( byte_cache, d_size()*sizeof(float) );
  }
};


//---------------------------------------------------
void 
baml_prob_img::read( vcl_string file_name )
{
  vcl_ifstream s( file_name.c_str(), vcl_ifstream::binary );
  if( s.good() ){
    char* byte_cache;
    unsigned nx[1];
    byte_cache = (char*)nx; s.read( byte_cache, sizeof(unsigned) ); ni_ = nx[0];
    byte_cache = (char*)nx; s.read( byte_cache, sizeof(unsigned) ); nj_ = nx[0]; 

    delete[] d_;
    d_ = new float[ d_size() ];
    byte_cache = (char*)d_; s.read( byte_cache, d_size()*sizeof(float) );
  }
};


//-------------------------------------------------
void 
baml_prob_img::initialize( unsigned ni, unsigned nj )
{
  nj_ = nj; ni_ = ni; 
  num_mixtures_ = 5;

  baml_mog m( num_mixtures_ );
  mog_size_ = m.component_sizes()(0);
  if( d_ != NULL ) delete[] d_;
  d_ = new float[ d_size() ];

  for( unsigned i = 0; i < ni_; i++ )
    for( unsigned j = 0; j < nj_; j++ ){
      baml_mog* new_mog = (*this)(i,j);
      new_mog->initialize_component(0);
      delete new_mog;
    }
};


//----------------------------------------------------
void 
baml_prob_img::write_pixel(
  int i, int j,
  vcl_string file )
{
  vcl_ofstream ofs( file.c_str() );
  baml_mog* new_mog = (*this)(i,j);

  for(  float c = 0.0; c < 1.0; c+=(float).01 )
    ofs << c << '\t' << new_mog->prob(c,vnl_vector<float>()) << '\n';
  
  delete new_mog;
};

#endif // _baml_prob_img_cxx_
