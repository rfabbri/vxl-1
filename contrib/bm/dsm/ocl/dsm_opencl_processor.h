//this is /contrib/bm/dsm/ocl/dsm_opencl_processor.h
#ifndef DSM_OPENCL_PROCESSOR_H_
#define DSM_OPENCL_PROCESSOR_H_

#include<bocl/bocl_cl.h>
#include<bocl/bocl_manager.h>
#include<bocl/bocl_mem.h>

class dsm_opencl_processor: public bocl_manager<dsm_opencl_processor>
{
public:
	dsm_opencl_processor(){}
	~dsm_opencl_processor(){}

	virtual bool init();
	virtual bool run();
	virtual bool finish();
protected:
};

#endif DSM_OPENCL_PROCESSOR_H_