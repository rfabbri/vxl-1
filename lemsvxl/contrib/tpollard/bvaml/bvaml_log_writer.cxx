#ifndef _bvaml_log_writer_cxx_
#define _bvaml_log_writer_cxx_

#include "bvaml_log_writer.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>


bvaml_log_writer::bvaml_log* bvaml_log_writer::log_ = 0;


//----------------------------------------------
bvaml_log_writer::bvaml_log_writer(
  bool write_cerr,
  vcl_string write_file )
{
  calling_function_ = "bvaml_log_writer";
  if(log_)
    print_error( "initialization failed, master writer already created." );
  else{
    log_ = new bvaml_log( write_cerr, write_file );
    print_msg( "initialization succeeded" );
  }
};


//----------------------------------------------
bvaml_log_writer::bvaml_log_writer(
  vcl_string calling_function )
{
  calling_function_ = calling_function;
  if(!log_){
    log_ = new bvaml_log( true, "NONE" );
    print_error( "bvaml_log was not initialized; using default params." );
  }
  log_->level++;
};


//----------------------------------------------
bvaml_log_writer::~bvaml_log_writer()
{
  log_->level--;
};


//----------------------------------------------
void
bvaml_log_writer::print_msg( 
  const vcl_string& msg )
{
  vcl_stringstream text;
  text << calling_function_ << ": " << msg;
  log_->write( text.str() );
};


//----------------------------------------------
void
bvaml_log_writer::print_error( 
  const vcl_string& msg )
{
  vcl_stringstream text;
  text << calling_function_ << ": ERROR: " << msg;
  log_->write( text.str() );
};


//-----------------------------------------------
void 
bvaml_log_writer::print_block(
  const vcl_string& block_title,
  const vcl_string& msg )
{
  vcl_stringstream text;
  text << calling_function_ << ": " << block_title << ':';
  log_->write( text.str() );
  vcl_cerr << "\n\n" << msg << "\n";
};


//---------------------------------------------------
void 
bvaml_log_writer::start_progress_msg(
  const vcl_string& msg )
{
  vcl_stringstream text;
  text << calling_function_ << ": " << msg << ':' ;
  log_->write( text.str() );
  current_progress_ = 0;
};


//---------------------------------------------------
void 
bvaml_log_writer::progress_update(
  double progress )
{
  if( floor( 10*progress ) > current_progress_ ){
    current_progress_ = (int)floor( 10*progress );
    vcl_stringstream update;
    update << ' ' << current_progress_ << "0%";
    log_->write( update.str(), false );
  }
};


//-----------------------------------------------
void 
bvaml_log_writer::bvaml_log::write(
  vcl_string text,
  bool new_line )
{
  vcl_stringstream display_text;
  if( new_line ){
    display_text << '\n';
    for( int i = 1; i < level; i++ )
      display_text << "  ";
  }
  display_text << text;

  // Eventually there will be more here.
  if( write_cerr_ )
    vcl_cerr << display_text.str();
};


#endif // _bvaml_log_writer_cxx_
