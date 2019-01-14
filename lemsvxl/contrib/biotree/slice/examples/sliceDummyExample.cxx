#include <slice/sliceProcessor.h>
#include <slice/sliceEngine.h>
#include <fstream>


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
          std::cerr << "slice " << slice << " val " << slices[slice*w*h + 1] << "\n";
        }

        std::ofstream binaryout("test.slices", std::ios::binary);
        binaryout.write((char*)&w,sizeof(int));
        binaryout.write((char*)&h,sizeof(int));
        binaryout.write((char*)&d,sizeof(int));
        binaryout.write((char*)slices,w*h*d*sizeof(float));
        binaryout.close();
        delete slices;


        //using the engine to save the output to a memory location
        sliceEngine<float> engine;
        engine.addStream("test.slices");
        std::vector<float*> outputs;
        outputs.push_back( new float[w*h*d]);
        engine.setSaveToMemory(outputs);

        dummySliceProcessor proc;
        engine.processWith(&proc);

        slices = outputs[0];
        for(int slice = 0; slice < d; slice++){
          std::cerr << "slice " << slice << " val " << slices[slice*w*h + 1] ;
          std::cerr << " check: " << ((slice==0?0:slice-1) + slice + (slice==d-1?0:slice+1))/3.;
          std::cerr << " diff: " << slices[slice*w*h + 1] - ((slice==0?0:slice-1) + slice + (slice==d-1?0:slice+1))/3.;
          std::cerr << "\n";
        }

        delete slices;

        // using the engine to save the output to disk

        engine.clearStreams();
        engine.addStream("test.slices");
        std::vector<std::string> outfilename; outfilename.push_back("my.out");
        engine.setSaveToDisk(outfilename);
        engine.processWith(&proc);
        
        return 0;
}
