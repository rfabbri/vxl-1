// TextProgressBar.h: class header file
// Written by Dave Pacheco (dapachec)
// 
// This class is used to create a textual progress meter.

#include <iostream>
#include <string>
#include <sstream>

class TextProgressBar {
  std::string itsLabel;
  unsigned int itsTotal;
  std::ostream* itsOut;
  unsigned int itsCurrent;
public:
  TextProgressBar(
      std::string label = "Progress",
      unsigned int total = 100, 
      std::ostream* out = &std::cout, 
      unsigned int start = 0) :
    itsLabel(label), itsTotal(total), itsOut(out), itsCurrent(start) {}

  // Prints the new result directly to the screen
  void print() const {
    *itsOut << currentStr() << std::flush;
  }

  // Prints the current percent
  std::string currentStr() const {
    if(itsCurrent == itsTotal)
      return finishStr();
    
    std::ostringstream s;
    s << "\r" << itsLabel << ": " << percent() << "% (" << itsCurrent 
      << " / " << itsTotal << ")"; 
    return s.str();
  }

  // Returns the current percent string
  std::string finishStr() const {
    std::ostringstream s;
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
