//this is /contrib/bm/oclVectorAdd.cxx

#include"oclVectorAddManager.h"
#include<vcl_iostream.h>
#include<vcl_cstdlib.h> //FOR RAND
#include<vcl_ctime.h> //FOR TIME


void randomInit( float* vect, unsigned const& size )
{
	for(unsigned i = 0; i < size; ++i)
		vect[i] = vcl_rand() % 10 + 1 ;
}//end randomInit

float* hostVectorAdd( float const* vectA, float const* vectB, unsigned const& size )
{
	float* result = new float[size];
	for( unsigned i = 0; i < size; ++i )
		result[i] = vectA[i] + vectB[i];
	return result;
}//end hostVectorAdd

bool checkEqual(float const* vectA, float const* vectB, unsigned const& size)
{
	bool equal = true;
	for( unsigned i = 0; i < size; ++i )
	{
		if( vectA[i] != vectB[i] )
		{
			equal = false;
			break;
		}
	}
	return equal;
}//end checkEqual

int main()
{
	vcl_cout << "oclVectorAddManager.exe" << vcl_endl;
	oclVectorAddManager* addManager = oclVectorAddManager::instance();

	unsigned size = 12345678;
	float* a = new float[size];
	float* b = new float[size];
	float* gpu_result = new float[size];
	float* cpu_result = new float[size];

	vcl_time_t cpu_start,cpu_end;

	randomInit(a,size);
	randomInit(b,size);

	vcl_time(&cpu_start);
	cpu_result = hostVectorAdd(a,b,size);
	vcl_time(&cpu_end);

	double cpu_performance_seconds = vcl_difftime(cpu_end,cpu_start);

	vcl_cout << "-------------------------------------------------------" << vcl_endl;
	vcl_cout << "CPU finished in " << cpu_performance_seconds << " seconds." << vcl_endl << vcl_endl;


	vcl_time_t gpu_start, gpu_end;

	vcl_time(&gpu_start);
	gpu_result = addManager->oclAdd(a,b,size);
	vcl_time(&gpu_end);

	double gpu_performance_seconds = vcl_difftime(gpu_end,gpu_start);

	vcl_cout << "GPU finished in " << gpu_performance_seconds << " seconds." << vcl_endl << vcl_endl;

	bool cpu_equal_gpu = checkEqual(cpu_result,gpu_result,size);
	if(cpu_equal_gpu)
		vcl_cout << "Results are equal." << vcl_endl;
	else
		vcl_cout << "RESULTS ARE UNEQUAL: BUG SOMEWHERE!" << vcl_endl;
	

	//vcl_cout << "GPU is " << (cpu_performance_seconds/gpu_performance_seconds) * 100 << " times faster than the cpu." << vcl_endl;

	return 0;
}