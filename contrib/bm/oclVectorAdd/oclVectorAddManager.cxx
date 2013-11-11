//this is oclVectorAddManager.cxx
//Author: Brandon Mayer

#include"oclVectorAddManager.h"

oclVectorAddManager* oclVectorAddManager::instance_ = NULL;

oclVectorAddManagerDestroyer oclVectorAddManager::destroyer_; //Definition and hence instance creation

oclVectorAddManager* oclVectorAddManager::instance()
{
	if(!instance_)
	{
		instance_ = new oclVectorAddManager;
		destroyer_.setPointer(instance_);
	}
	return instance_;
}//end oclVectoAddManager::instance()

oclVectorAddManager::oclVectorAddManager()
{
	this->bmocl_manager_instance = bmocl_manager::instance();
	this->bmocl_manager_instance->load_kernel_source("cl/oclVectorAdd.cl");
	vcl_string src = bmocl_manager_instance->program_source();

	vcl_size_t sourceSize[] = {src.size()};
	const char* source = src.c_str();

	if(!sourceSize[0])
	{
		vcl_cerr << "Program Source is empty." << vcl_flush;
		exit(1);
	}
	cl_int status = CL_SUCCESS;

	//CREATE THE PROGRAM
	this->program_ = clCreateProgramWithSource(bmocl_manager_instance->context(),
												1,
												&source,
												sourceSize,
												&status);
	if(status!=CL_SUCCESS)
	{
		vcl_cerr << error_to_string(status) << vcl_flush;
		exit(1);
	}

	//COMPILE THE PROGRAM
	status = clBuildProgram(program_,
		bmocl_manager_instance->number_devices(),
		bmocl_manager_instance->devices(),
		NULL,
		NULL,
		NULL);

	
	if( status != CL_SUCCESS )
	{
		vcl_cerr << error_to_string(status) << vcl_flush;
		exit(1);
	}

	//CREATE THE KERNEL
	this->kernel_ = clCreateKernel(this->program_,
								   "oclVectorAdd",
								   &status);

	if(status != CL_SUCCESS)
	{
		vcl_cerr << error_to_string(status) << vcl_flush;
		exit(1);
	}
	
	//CREATE THE COMMAND QUEUE
	this->queue_ = clCreateCommandQueue(bmocl_manager_instance->context(),
										bmocl_manager_instance->device(0),
										0,
										&status);
	if(status != CL_SUCCESS)
	{
		vcl_cerr << error_to_string(status) << vcl_flush;
		exit(1);
	}

}//end default constructor

vcl_vector<float> oclVectorAddManager::oclAdd(vcl_vector<float> const& vect_a, vcl_vector<float> const& vect_b)
{
	assert(vect_a.size() == vect_b.size());
	unsigned n = vect_a.size();
	//ALLOCATE MEMORY ON THE DEVICE
	float* a = new float[n];
	float* b = new float[n];
	float* c = new float[n];
	for(unsigned i = 0; i < n; ++i)
	{
		a[i] = vect_a[i];
		b[i] = vect_b[i];
	}
	c = oclAdd(a,b,n);

	vcl_vector<float> result(n,0);
	for( unsigned i = 0; i < n; ++i )
		result[i] = c[i];

	return result;
}//end oclAdd vcl_vector<float> wrapper

float* oclVectorAddManager::oclAdd(float* a, float* b, unsigned const& n)
{
	//ALLOCATE MEMORY ON THE HOST TO RECEIVE RESULT
	float *c = new float[n];

	//ALLOCATE DEVICE BUFFERS
	cl_mem dVectA, dVectB, dResult;
	dVectA = clCreateBuffer(this->bmocl_manager_instance->context(),
							CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
							n * sizeof(cl_float), a, 0);

	dVectB = clCreateBuffer(this->bmocl_manager_instance->context(),
							CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
							n * sizeof(cl_float), b, 0);

	dResult = clCreateBuffer(this->bmocl_manager_instance->context(),
							 CL_MEM_WRITE_ONLY,
							 n * sizeof(cl_float), 0, 0);

	//SET UP KERNEL ARGUMENTS
	clSetKernelArg(this->kernel_, 0, sizeof(cl_mem), &dVectA);
	clSetKernelArg(this->kernel_, 1, sizeof(cl_mem), &dVectB);
	clSetKernelArg(this->kernel_, 2, sizeof(cl_mem), &dResult);
	clSetKernelArg(this->kernel_, 3, sizeof(unsigned), &n);

	//SET WORK GROUP SIZE TO MAX
	vcl_size_t wg_size = this->bmocl_manager_instance->max_work_group_size();
	
	//GET THE GLOBAL SIZE BY DIVIDING THE SIZE OF THE VECTOR BY THE WG_SIZE
	//AND ROUNDING UP
	vcl_size_t global_size = RoundUp(n,wg_size);

	cl_int status = CL_SUCCESS;

	//FINALLY LAUNCH THE KERNEL
	status = clEnqueueNDRangeKernel(this->queue_,
									this->kernel_,
									1,
									NULL,
									&global_size,
									&wg_size,
									0,
									NULL,
									NULL);

	if( status != CL_SUCCESS )
	{
		vcl_cerr << error_to_string(status) << vcl_flush;
		exit(1);
	}


	//READ BACK THE RESULT
	status = clEnqueueReadBuffer(this->queue_,
								 dResult,
								 CL_TRUE,
								 0,
								 sizeof(cl_float)*n,
								 c,
								 0,
								 NULL,
								 NULL);

	if( status != CL_SUCCESS )
	{
		vcl_cerr << error_to_string(status) << vcl_flush;
		exit(1);
	}

	return c;
}//end oclAdd