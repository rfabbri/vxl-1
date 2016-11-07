#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_sstream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>


//Inputs we need

//  1.True Changes image
//  2.A tresholed changes image

// To obtain these we need to:

//  1. True changes folder
//  2. Results folder
//  3. Get the all-changes images and threshold changes
//  4. Iterate throuh the threshold images and obtain data



int main()
{
  vcl_string true_dir = "d:/results/baghdad/roc/true";
  vcl_string detection_dir = "d:\\results/baghdad/roc/mmog1";
  vcl_ofstream roc_stream("d:\\results/baghdad/roc/roc_mmog1.txt");

  vcl_map<vcl_string, vcl_vector<vcl_string>> image_names;
  vcl_vector<float> thresh;
  for (unsigned p = 0; p < 1000; p= p+25)
    thresh.push_back(float(float(p)/1000));
  //...

  //iterate through scenes 
  vcl_vector<vcl_string> scenes;
  scenes.push_back("hiafa");
  scenes.push_back("embassy");
  scenes.push_back("karkh");

  for (unsigned s= 0; s<scenes.size(); s++)
  {

    vcl_string scan_dir = detection_dir + "\\" + scenes[s];
    vcl_vector<vcl_string> names;

    //iterate through changes dir, get all image names that we need to process
    for( vul_file_iterator fit = (scan_dir + "/*.*"); fit; ++fit ){

     
      if( vul_file::is_directory(fit()) )
        continue;
      vcl_string image_name = fit();

      vcl_string file_ext = vul_file::extension(fit());

      //get extension
      if( file_ext !=  ".jpg" && file_ext != ".png" )
        continue;

      names.push_back(vul_file::basename(fit()));

    }
    image_names.insert(vcl_make_pair( scenes[s], names));
  }



  //iterate through all threshold for each image
  for (unsigned n = 0; n< thresh.size(); n++)
  {
    vcl_cerr << n << ' ';

    unsigned long change_marked_change = 0;
    unsigned long change_marked_nonchange = 0;
    unsigned long nonchange_marked_nonchange = 0;
    unsigned long nonchange_marked_change = 0;

    for (vcl_map<vcl_string, vcl_vector<vcl_string>>::iterator mip = image_names.begin();
      mip != image_names.end(); mip++)
    {

      vcl_string true_change_dir = true_dir + "\\" + mip->first;
      vcl_string detected_changes_dir = detection_dir + "\\"  + mip->first;

      vcl_vector<vcl_string> names = mip->second;

      for (vcl_vector<vcl_string>::iterator vit = names.begin(); vit !=names.end(); vit++)
      {
        
        vcl_string prob_image = detected_changes_dir + "\\" + *vit;
        //Read image containing probabilistic changes
        vil_image_view<vxl_byte> prob_change = vil_load( prob_image.c_str() ); 

        unsigned image_height = prob_change.nj();
        unsigned image_width = prob_change.ni();

        //Read image containing true changes
        vcl_string true_change_file =  true_change_dir + "\\" + *vit;
        true_change_file = vul_file::strip_extension( true_change_file );
        true_change_file += ".jpg";
        vil_image_view<vxl_byte> true_change = 
          vil_convert_to_grey_using_average( vil_load(true_change_file.c_str())); 


        //iterate through the image
        vil_image_view<vxl_byte> detected_change(image_width, image_height);
        for( int x = 0; x < image_width; x++ ){
          for( int y = 0; y < image_height; y++ ){
            if( prob_change(x,y) <  thresh[n] *255 ) 
              detected_change(x,y) = 255;
            else
              detected_change(x,y) = 0;
          }
        }

        //Compare thesholded images to the model
        for( unsigned i = 0; i < true_change.ni(); i++ ){
          for( unsigned j = 0; j < true_change.nj()-1; j++ ){
            unsigned thresh_i = i/2;
            unsigned thresh_j = j/2;

            //count changes 
            if(( true_change(i,j) != 255.0 )&& ( true_change(i,j) != 0 ))continue;
            if( true_change(i,j) > 200  && detected_change(thresh_i, thresh_j) == 255 )
              change_marked_change++;
            if( true_change(i,j) > 200 && detected_change(thresh_i, thresh_j) == 0 )
              change_marked_nonchange++;
            if( true_change(i,j) < 50 && detected_change(thresh_i, thresh_j) == 255 )
              nonchange_marked_change++;
            if( true_change(i,j) < 50 && detected_change(thresh_i, thresh_j) == 0 )
              nonchange_marked_nonchange++;

          }
        }

        //save image: for debugging purpouses
        //vcl_stringstream img_out_ss;
        //img_out_ss << vul_file::strip_extension(fit()) <<'_' <<1<< ".png";
        //vcl_string img_out = img_out_ss.str();
        //vil_save( detected_change, img_out.c_str()  );
      }
    }

    //write the text file
    double percent_change_marked_change = change_marked_change/
      (double)(change_marked_change+change_marked_nonchange);
    double percent_nonchange_marked_change = nonchange_marked_change/
      (double)(nonchange_marked_change+nonchange_marked_nonchange);
    roc_stream << percent_change_marked_change << "\t" <<
      percent_nonchange_marked_change << '\n';
    vcl_cerr << '\n';
  }
}
