// TextProgressBar.h: class header file
// Written by Dave Pacheco (dapachec)
// 
// This class is used to create a textual progress meter.

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>

class TextProgressBar {
  vcl_string itsLabel;
  unsigned int itsTotal;
  vcl_ostream* itsOut;
  unsigned int itsCurrent;
public:
  TextProgressBar(
      vcl_string label = "Progress",
      unsigned int total = 100, 
      vcl_ostream* out = &vcl_cout, 
      unsigned int start = 0) :
    itsLabel(label), itsTotal(total), itsOut(out), itsCurrent(start) {}

  // Prints the new result directly to the screen
  void print() const {
    *itsOut << currentStr() << vcl_flush;
  }

  // Prints the current percent
  vcl_string currentStr() const {
    if(itsCurrent == itsTotal)
      return finishStr();
    
    vcl_ostringstream s;
    s << "\r" << itsLabel << ": " << percent() << "% (" << itsCurrent 
      << " / " << itsTotal << ")"; 
    return s.str();
  }

  // Returns the current percent string
  vcl_string finishStr() const {
    vcl_ostringstream s;
    s << "\r" << itsLabel << ": 100% (" << itsTotal 
      << " / " << itsTotal << ")\n";
    return s.str();
  }

  // Returns the current percent
  unsigned short int percent() const {
    return (unsigned short int) (100 * (double) itsCurrent / itsTotal);
  }

  void update(int i) {
    itsCurrent = i;
  }

  // Increment by 1
  void step() {
    itsCurrent++;
  }

  void finish() {
    itsCurrent = itsTotal;
  }
};
