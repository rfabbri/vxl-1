#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>

int main(int argc, char *argv[])
{
  int ndigits=4;
  if (argc == 2) {
    vcl_istringstream strs(argv[1]);
    strs >> ndigits;
  }

  vcl_cout << vcl_setw(ndigits) << vcl_setfill('0');

  int n;
  while (vcl_cin >> n)
    vcl_cout << n;
  vcl_cout << vcl_endl;

  return 0;
}
