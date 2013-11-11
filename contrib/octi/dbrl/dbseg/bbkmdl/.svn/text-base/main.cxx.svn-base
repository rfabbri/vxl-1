#include "init_resource.h"
#include <vil/vil_load.h>

using namespace std;

int main(int argc, char* argv[])

{
	if (argc!=2)
		cout<<"need an input path!";
	else
	{
		init_resource res=init_resource(argv[1]);
		res.create_videostream();
		res.set_inputs();
#ifndef fix
		res.init();
		res.exec();
		string base("C:/Users/Deus/Documents/Brown Courses/ENGN2910x/tree_frames_part2/");
		//string::iterator it=base.end();
	
		string ext(".tiff");
		string prefix;
		int i;
		for (i=1;i<276;i++)
		{
			if (i<10)
				prefix=string("0000");
			if((i>=10)&&(i<100))
				prefix=string("000");
			else if(i>=100)
				prefix=string("00");
			char* tmpNo=new char();
			string number=string(itoa(i,tmpNo,10));
			string path=base+prefix+number+ext;
			string out_ext=".tiff";
			delete tmpNo;
			vil_image_view_base_sptr frame=vil_load(path.c_str(),true);
		res.measure_prob(frame,prefix+number+out_ext);
		cout<<"wrote frame "<<i<<endl;
		}

#elif defined fix
		res.create_process();
		res.fire();
#endif
	}
}