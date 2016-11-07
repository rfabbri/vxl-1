#include <slice/sliceProcessor.h>
#include <slice/sliceEngine.h>
#include <vcl_fstream.h>


int main(int argc, char* argv[])
{        
        int w = 100;
        int h = 100;
        int d = 100;

        float* slices = new float[w*h*d];

        for(int slice = 0; slice < d; slice++){
                for(int i = 0; i < w*h; i++){
                        slices[slice*w*h + i] = slice;
                }
        }

        for(int slice = 0; slice < d; slice++){
          vcl_cerr << "slice " << slice << " val " << slices[slice*w*h + 1] << "\n";
        }

        vcl_ofstream binaryout("test.slices", vcl_ios_binary);
        binaryout.write((char*)&w,sizeof(int));
        binaryout.write((char*)&h,sizeof(int));
        binaryout.write((char*)&d,sizeof(int));
        binaryout.write((char*)slices,w*h*d*sizeof(float));
        binaryout.close();
        delete slices;


        //using the engine to save the output to a memory location
        sliceEngine<float> engine;
        engine.addStream("test.slices");
        vcl_vector<float*> outputs;
        outputs.push_back( new float[w*h*d]);
        engine.setSaveToMemory(outputs);

        dummySliceProcessor proc;
        engine.processWith(&proc);

        slices = outputs[0];
        for(int slice = 0; slice < d; slice++){
          vcl_cerr << "slice " << slice << " val " << slices[slice*w*h + 1] ;
          vcl_cerr << " check: " << ((slice==0?0:slice-1) + slice + (slice==d-1?0:slice+1))/3.;
          vcl_cerr << " diff: " << slices[slice*w*h + 1] - ((slice==0?0:slice-1) + slice + (slice==d-1?0:slice+1))/3.;
          vcl_cerr << "\n";
        }

        delete slices;

        // using the engine to save the output to disk

        engine.clearStreams();
        engine.addStream("test.slices");
        vcl_vector<vcl_string> outfilename; outfilename.push_back("my.out");
        engine.setSaveToDisk(outfilename);
        engine.processWith(&proc);
        
        return 0;
}
