//:
//\file
//\brief Multiview Curve sketch Attributes (mca) command
//\author Ricardo Fabbri, Brown & Rio de Janeiro State U. (rfabbri.github.io)
// with modifications by Anil Usumezbas
//
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <buld/buld_arg.h>
#include <bmcsd/bmcsd_util.h>
#include <bmcsd/bmcsd_curve_3d_sketch_e.h>


#define MW_ASSERT(msg, a, b) if ((a) != (b)) { std::cerr << (msg) << std::endl; abort(); }

//: Reads curve sketch and prints its attributes to stdout in text format readable by
//other programs like matlab.
int
main(int argc, char **argv)
{
  std::string prefix_default(".");
  std::string prefix = prefix_default;

  vul_arg<std::string> a_csk("-curvesketch", "input 3D curve sketch file or directory", "out/");
  vul_arg<bool> a_showlengths("-showlengths", "shows curves lenghts for each attribute", true);
  vul_arg<bool> a_showviews("-showviews", "shows views for each curve", true);
  vul_arg<bool> a_showinliers("-showinliers", "shows inlier views for each curve", false);

  vul_arg_parse(argc,argv);
  std::cout << "\n";

  bmcsd_curve_3d_sketch_e *csk = new bmcsd_curve_3d_sketch_e;
  std::string csk_fname = prefix + "/" + a_csk();
  std::string out_fname = csk_fname + "attributes.txt";
  std::ofstream out_file(out_fname.c_str());
  bool print_to_file = true;

  std::string flag_fname = csk_fname + "flags.txt";
  std::ofstream flag_file(flag_fname.c_str());

  bool retval  = csk->read_dir_format(csk_fname);
  MW_ASSERT(std::string("Error reading 3D curve sketch: ") + csk_fname, retval, true);

  // Print out all costs to stdout.

  //Anil: When printing to file, the following format will be used:
  //
  //[Total Number of Curves]
  //[Total Support] [Length_v0] [Curve_v0] [Curve_v1] [Orig Curve_v0] [Orig Curve_v1] [v0] [v1] 
  //
  //DESCRIPTIONS
  //[Total Support]: Total number of edge support this 3D curve received.
  //[Length_v0]: Length of the image curve in the 1st anchor view that created this 3D curve
  //[Curve_v0]: ID of the curve in the 1st anchor view after epipolar tangency breaking, before pruning
  //[Curve_v1]: ID of the curve in the 2nd anchor view after epipolar tangency breaking, before pruning
  //[Orig Curve_v0]: ID of the curve in the 1st anchor view before epipolar tangency breaking
  //[Orig Curve_v1]: ID of the curve in the 2nd anchor view before epipolar tangency breaking
  //[v0]: ID of the 1st anchor view in the input image sequence
  //[v1]: ID of the 2nd anchor view in the input image sequence

  if(print_to_file)
  {
      assert(csk->num_curves() == csk->attributes().size());
      out_file << csk->num_curves() << std::endl;
      for (unsigned i=0; i < csk->num_curves(); ++i) {
          //out_file << csk->attributes()[i].total_support_;
          //if (a_showlengths())
              //out_file << " " << csk->curves_3d()[i].size();
          
          out_file << " " << csk->attributes()[i].i0() << " " << csk->attributes()[i].i1();
          out_file << " " << csk->attributes()[i].int_id_v0_ << " " << csk->attributes()[i].int_id_v1_;
          out_file << " " << csk->attributes()[i].orig_id_v0_ << " " << csk->attributes()[i].orig_id_v1_;
          
          if (a_showviews()) {
              out_file << " "; 
              out_file << csk->attributes()[i].v_->stereo0() << " " <<
                  csk->attributes()[i].v_->stereo1() << " ";

              //std::cout << " c: ";
              //for (unsigned k=0; k < csk->attributes()[i].v_->num_confirmation_views(); ++k)
              //  std::cout << csk->attributes()[i].v_->confirmation_view(k) << " ";
          }
          if (a_showinliers()) {
              out_file << " i: ";
              for (unsigned k=0; k < csk->attributes()[i].inlier_views_.size(); ++k)
                  out_file << csk->attributes()[i].inlier_views_[k] << " ";
          }
          out_file << std::endl;
      }
      out_file.close();

      flag_file << csk->num_curves() << std::endl;
      for (unsigned i=0; i < csk->num_curves(); ++i)
      {
        std::vector<bool> flags = csk->attributes()[i].certaintyFlags_;
          flag_file << flags.size() << " ";
          for (unsigned j=0; j < flags.size(); ++j)
              flag_file << flags[j] << " ";

          flag_file << std::endl;
      }
      flag_file.close();

  }
  else
  {
    assert(csk->num_curves() == csk->attributes().size());
    std::cout << "Number of curves: " << csk->num_curves() << std::endl;
    for (unsigned i=0; i < csk->num_curves(); ++i) {
      std::cout << csk->attributes()[i].total_support_;
      if (a_showlengths())
        std::cout << " " << csk->curves_3d()[i].size();
      if (a_showviews()) {
        std::cout << " v: "; 
        std::cout << csk->attributes()[i].v_->stereo0() << " " <<
                    csk->attributes()[i].v_->stereo1() << " ";

        // std::cout << " c: ";
        // for (unsigned k=0; k < csk->attributes()[i].v_->num_confirmation_views(); ++k)
        //   std::cout << csk->attributes()[i].v_->confirmation_view(k) << " ";
      }
      if (a_showinliers()) {
        std::cout << " i: ";
        for (unsigned k=0; k < csk->attributes()[i].inlier_views_.size(); ++k)
          std::cout << csk->attributes()[i].inlier_views_[k] << " ";
      }
      std::cout << std::endl;
    }
  }

  return 0;
}
