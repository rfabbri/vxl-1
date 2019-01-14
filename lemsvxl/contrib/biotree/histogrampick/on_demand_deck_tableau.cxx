#include "on_demand_deck_tableau.h"

//----------------------------------------------------------------------------
//: Make the next tableau down the list current.
void on_demand_deck_tableau::next()
{
  unsigned int tmp = index_;

  if (tmp+1 >= children.size())
    tmp=0;
  else
    ++tmp;

  if (index_ok(tmp))
    index_=tmp;

  std::cerr << "on_demand_deck_tableau::next " << index_ << std::endl;
  observers.notify();
}

//----------------------------------------------------------------------------
//: Make the next higher tableau current.
void on_demand_deck_tableau::prev()
{
  int tmp = index_;

  if (tmp == 0)
    tmp=children.size()-1;
  else
    --tmp;

  if (index_ok(tmp))
    index_=tmp;

  std::cerr << "on_demand_deck_tableau::prev " << index_ << std::endl;
  observers.notify();
}
