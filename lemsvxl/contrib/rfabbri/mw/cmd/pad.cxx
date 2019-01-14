#include <iostream>
#include <iomanip>
#include <sstream>

int main(int argc, char *argv[])
{
  int ndigits=4;
  if (argc == 2) {
    std::istringstream strs(argv[1]);
    strs >> ndigits;
  }

  std::cout << std::setw(ndigits) << std::setfill('0');

  int n;
  while (std::cin >> n)
    std::cout << n;
  std::cout << std::endl;

  return 0;
}
