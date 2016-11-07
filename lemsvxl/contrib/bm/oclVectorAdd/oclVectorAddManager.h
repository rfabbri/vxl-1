#ifndef oclVectorAddManager_h_
#define oclVectorAddManager_h_

#include<vbl/vbl_smart_ptr.h>
#include"bmocl_manager_sptr.h"
#include<vcl_cassert.h>


class oclVectorAddManagerDestroyer;

class oclVectorAddManager: public vbl_ref_count
{
public:
	
	~oclVectorAddManager(){}

	//runs the oclVectorAdd kernel in parallel and returns the answer
	vcl_vector<float> oclAdd(vcl_vector<float> const& a, vcl_vector<float> const& b);

	float* oclAdd(float* a, float* b, unsigned const& size);

	static oclVectorAddManager* instance();

protected:
	oclVectorAddManager();
	bmocl_manager_sptr bmocl_manager_instance;

	static oclVectorAddManager* instance_;
	cl_program program_;
	cl_command_queue queue_;
	cl_kernel kernel_;
	//this is kept on global memory so will the destructor of oclVectorManagerDestroyer class will be
	//called when program exists.
	static oclVectorAddManagerDestroyer destroyer_;

	
}; //end oclVectorAddManager class

class oclVectorAddManagerDestroyer
{
public:
	oclVectorAddManagerDestroyer(oclVectorAddManager* managerPtr = 0){managerPtr_ = managerPtr;}
	~oclVectorAddManagerDestroyer()
	{
		if(managerPtr_ != 0)
		{
			delete managerPtr_;
			managerPtr_ = 0;
		}
	}
	void setPointer( oclVectorAddManager* managerPtr) {managerPtr_ = managerPtr;}
private:
	oclVectorAddManager* managerPtr_;
}; //end oclVectorAddManagerDestroyer class

#endif //oclVectorAddManager_h_