//:
// \file
// \program to extract extrema responses of images
// \author Brandon Mayer
// \date 14 May 09
//

#include<brip/brip_vil_float_ops.h> //fast extrema method

#include<vcl_iostream.h> //cout etc...
#include<vcl_fstream.h> //write file output
#include<vcl_string.h> // std::string

#include<vil/vil_convert.h> //to convert images to and from vxl_byte and float
#include<vil/vil_load.h> //load images
#include<vil/vil_save.h> //save images
#include<vil/vil_image_view.h> //image views


#include<vnl/vnl_vector.h> //vector to create the histogram

#include<vul/vul_file_iterator.h> //file iterator to parse files
#include<vul/vul_file.h> //strip extension from directory





using namespace std;

int main(int argc, char *argv[])
{


  //on my lems machine the directories are...
  //Image Path:
  //       /media/DATAPART1/BrandonDataFolder/VehicleRecognition/NormalizedImages
  //Output Path:
  //       /media/DATAPART1/BrandonDataFolder/VehicleRecognition/OperatorOutput

  //argv[1] is the path to images
  //argv[2] is the path to output the operator response

  vcl_string inDir, outDir, filename,f;
  if(argc > 1)
    {
      inDir=argv[1];
      outDir=argv[2];
    }
  else
    {
      inDir="/media/DATAPART1/BrandonDataFolder/VehicleRecognition/NormalizedImages";
      outDir="/media/DATAPART1/BrandonDataFolder/VehicleRecognition/OperatorOutput";
    }

  inDir = inDir+"/*.png";


  vil_image_view<vxl_byte> image,opByte;

  vil_image_view<float> temp, opResponse;

 

  float opMin,opMax;

  vil_image_view<float>::iterator itBegin;
  vil_image_view<float>::iterator itEnd;



  //use fn() as filename
  for(vul_file_iterator fn=inDir; fn; ++fn)
   {
      vcl_cout << fn() << vcl_endl;
      filename = vul_file::strip_directory(fn());
      filename = vul_file::strip_extension(filename);
      vcl_cout << filename << vcl_endl;

      //load image
      image = vil_load(fn());

      
      temp.set_size(image.ni(),image.nj(),image.nplanes());
      opResponse.set_size(image.ni(),image.nj(),image.nplanes());
      opByte.set_size(image.ni(),image.nj(),image.nplanes());

      //convert input image to float for processing
      for(unsigned i = 0; i < image.ni(); ++i)
 for(unsigned j = 0; j < image.nj(); ++j)
   temp(i,j) = static_cast<float>(image(i,j));
      
      //obtain operator response
      opResponse = brip_vil_float_ops::fast_extrema(temp,2,1,-45);

      //write the operator response out to a file so i can view it in matlab
      //ofstr.open(outDir+filename+
      // ofstr.open(outDir+filename+"_opResponseFloat.txt");
      //  if(ofstr){
      //    for(;itBegin!=itEnd;++itBegin)
      //      ofstr << *itBegin << " ";
      //  }
   

      //find min and max of opResponse
      itBegin = opResponse.begin();
      itEnd = opResponse.end();
      opMin = *itBegin;
      opMax = *itEnd;
      ++itBegin;
      for(;itBegin!=itEnd;++itBegin)
       {
         if(*itBegin < opMin)
           opMin = *itBegin;
         if(*itBegin > opMax)
           opMax = *itBegin;
       } 
      
      vcl_cout << "opMin: " << opMin << vcl_endl;
      vcl_cout << "opMax: " << opMax << vcl_endl;
      
      vil_convert_stretch_range_limited(opResponse,opByte,opMin,opMax);

      //save the opByte as .png
      vcl_string tempFilename = outDir+"/"+filename+"_opByte.png";
      vil_save(opByte,tempFilename.c_str());

      vcl_string tempFilename2 = outDir+"/"+filename+"_opByte.txt";
      vcl_ofstream outStream(tempFilename2.c_str(), vcl_ios_out);
      if(outStream){
        for(unsigned j=0;j<opByte.nj();++j){
          for(unsigned i=0;i<opByte.ni();++i){
            outStream << (int)opByte(i,j)<< ' ';
          }
   outStream << '\n';
 }
      }
  

      //get histogram

      vnl_vector<unsigned> hist(unsigned(256),0);
      vil_image_view<vxl_byte>::iterator opByte_it = opByte.begin();
      vil_image_view<vxl_byte>::iterator opByteEnd = opByte.end();
      for(;opByte_it!=opByteEnd;++opByte_it){
        hist[(*opByte_it)]=hist[(*opByte_it)]+1;
      }

      //normalize the histogram
      hist = hist.normalize();
 
      
      f = outDir + "/" + filename + "_histogram.txt";
      vcl_ofstream ofstr(f.c_str(), vcl_ios_out);
      if(ofstr){
        ofstr << hist << endl;
      }
      ofstr.close();
      
      
      
      //vil_image_view<float>::iterator it_begin = opResponse.begin();
      //vil_image_view<float>::iterator it_end = opResponse.end();
      
      //for(;it_begin!=it_end;++it_begin){ ++hist[*it_begin];}    

      // normalize histogram to yeild probability distribution
      //hist/=(image.size());
      
      // write to file
      //filename = vul_file::strip_directory(inDir);
      //filename = vul_file::strip_extension(filename);
      //filename = outDir+filename+".txt";
      
      //vcl_cout << vcl_endl << filename << vcl_endl;
      
   }
  
  return 0;
}
