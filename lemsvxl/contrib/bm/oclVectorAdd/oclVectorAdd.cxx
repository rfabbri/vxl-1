//this is /contrib/bm/oclVectorAdd.cxx

#include"oclVectorAddManager.h"
#include<iostream>
#include<cstdlib> //FOR RAND
#include<ctime> //FOR TIME


void randomInit( float* vect, unsigned const& size )
{
	for(unsigned i = 0; i < size; ++i)
		vect[i] = std::rand() % 10 + 1 ;
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
	std::cout << "oclVectorAddManager.exe" << std::endl;
	oclVectorAddManager* addManager = oclVectorAddManager::instance();

	unsigned size = 12345678;
	float* a = new float[size];
	float* b = new float[size];
	float* gpu_result = new float[size];
	float* cpu_result = new float[size];

	std::time_t cpu_start,cpu_end;

	randomInit(a,size);
	randomInit(b,size);

	std::time(&cpu_start);
	cpu_result = hostVectorAdd(a,b,size);
	std::time(&cpu_end);

	double cpu_performance_seconds = std::difftime(cpu_end,cpu_start);

	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << "CPU finished in " << cpu_performance_seconds << " seconds." << std::endl << std::endl;


	std::time_t gpu_start, gpu_end;

	std::time(&gpu_start);
	gpu_result = addManager->oclAdd(a,b,size);
	std::time(&gpu_end);

	double gpu_performance_seconds = std::difftime(gpu_end,gpu_start);

	std::cout << "GPU finished in " << gpu_performance_seconds << " seconds." << std::endl << std::endl;

	bool cpu_equal_gpu = checkEqual(cpu_result,gpu_result,size);
	if(cpu_equal_gpu)
		std::cout << "Results are equal." << std::endl;
	else
		std::cout << "RESULTS ARE UNEQUAL: BUG SOMEWHERE!" << std::endl;
	

	//std::cout << "GPU is " << (cpu_performance_seconds/gpu_performance_seconds) * 100 << " times faster than the cpu." << std::endl;

	return 0;
}