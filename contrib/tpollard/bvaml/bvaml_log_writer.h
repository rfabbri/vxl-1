#ifndef _bvaml_log_writer_h_
#define _bvaml_log_writer_h_

//:
// \file
// \brief A class for doing all message/error printing.
// \author Thomas Pollard
// \date 12/20/06
// 
//   This is a class for doing all msg printing in a standard way.  The
// bvaml_log_writer classes are created inside functions for msg/errors
// printed inside that function.  Nested functions with different
// bvaml_log_writers will have the msgs indented appropriately.

#include <vcl_string.h>
#include <vcl_cmath.h>


class bvaml_log_writer {

public:

  // A master bvaml_log_writer should be created using this constructor
  // to set the writing parameters before all future writers for functions
  // are created using the next constructor.  All messages will be written
  // to cerr if write_cerr == true and to write_file if it is not "NONE".
  bvaml_log_writer(
    bool write_cerr,
    vcl_string write_file );

  // Create a new writer for a calling function.
  bvaml_log_writer(
    vcl_string calling_function );

  ~bvaml_log_writer();

  // Print a msgs and errors.
  void print_msg( 
    const vcl_string& msg );
  void print_error(
    const vcl_string& msg );

  // Print a large block of text in its own space.
  void print_block(
    const vcl_string& block_title,
    const vcl_string& msg );

  // Print the status of a process.  Calling start_progress_msg will
  // start the msg and following calls of progress update on numbers
  // between 0 and 1 will give incremental updates on progress.
  void start_progress_msg(
    const vcl_string& msg );
  void progress_update(
    double progress );

protected:

  vcl_string calling_function_;

  int current_progress_;

  // A private log class for maintaining log writing parameters and
  // position within the log.  A static instance of this class will
  // be shared by all bvaml_log_writers.
  class bvaml_log {

  public:

    bvaml_log(
      bool write_cerr,
      vcl_string write_file ) :
        write_cerr_( write_cerr ),
        write_file_( write_file ),
        level( 0 ){}

    // Print this text to output sources, using proper indentation.
    void write(
      vcl_string text,
      bool new_line = true );

    // The current indentation level.
    int level;

  protected:

    bool write_cerr_;
    vcl_string write_file_; // NOT IMPLEMENTED
  };

  static bvaml_log* log_;
};





#endif // _bvaml_log_writer_h_
