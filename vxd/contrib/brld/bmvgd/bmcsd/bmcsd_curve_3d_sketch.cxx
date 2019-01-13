#include "bmcsd_curve_3d_sketch.h"
#include <vul/vul_file.h>
#include <vul/vul_file.h>
#include <vsl/vsl_vector_io.h>


bool bmcsd_curve_3d_sketch::
write_dir_format(std::string dir) const
{
  vul_file::make_directory(dir);

  { // version number 
    std::ofstream f((dir + std::string("/version")).c_str());
    f << dir_format_version() << std::endl;
  }
  { // number 
    std::ofstream f((dir + std::string("/num_curves")).c_str());
    f << num_curves() << std::endl;
  }

  // Write out all curves in binary format. In the future, we might write in
  // another format.
  vul_file::make_directory(dir+"/crvs");
  bool retval = mywritev(dir + "/crvs/rec", ".dat", curves_3d());
  if(!retval) {
    std::cerr << "Error while trying to write 3D curves to file.\n";
    return false;
  }

  { // write attributes in a single file.
    vsl_b_ofstream f(dir + "/attributes.vsl");
    vsl_b_write(f, attr_);
  }

  // Optionally tar-gzip the directory. Perhaps using an external shell script.

  return true;
}

bool bmcsd_curve_3d_sketch::
read_dir_format(std::string dir)
{
  // Optionally tar-gzip the directory. Perhaps using an external shell script.

  short ver = 0;
  { // version number 
    std::ifstream f((dir + "/version").c_str());
    f >> ver;
  }

  switch (ver) {
    case 1: {
              std::cerr << "Obsolete curve sketch version number 1: Run the script mcs_fix_numbering with the right number of digits (ceil(log(ncurves)/log(10))+1, bump up the version to 2, then call this script." 
                << std::endl;
              return false;
    } break;
    case 2: {

    unsigned ncurves;
    std::cout << "read_dir: Reading num_curves\n";
    { // number 
      std::ifstream f((dir + "/num_curves").c_str());
      f >> ncurves;
      crv3d_.resize(ncurves);
    }

    std::cout << "read_dir: Reading 3d curves\n";
    {
      bool retval = myreadv(dir + std::string("/crvs/rec"), ".dat", crv3d_);
      if(!retval) {
        std::cerr << "Error while trying to read 3D curves from dir " << dir << std::endl;
        return false;
      }
    }

    std::cout << "read_dir: Reading attributes\n";
    {
    vsl_b_ifstream f((dir + "/attributes.vsl").c_str());
    vsl_b_read(f, attr_);
#ifndef NDEBUG
    std::cout << "attr_.size(): "<< attr_.size() << " ncurves: " << ncurves << std::endl;
#endif
    assert(attr_.size() == ncurves);
    }

    } break;
    default:
      std::cerr << "I/O ERROR: bmcsd_curve_3d_sketch::read_dir_format\n"
           << "           Unknown version number "<< ver << '\n';
  }

  return true;
}
