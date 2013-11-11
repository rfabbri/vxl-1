#include <cplusplusclass/my_rectangle.h>
#include <cplusplusclass/my_rectangle_sptr.h>
#include <cplusplusclass/yong_star.h>
#include <cplusplusclass/yong_star_sptr.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>

void sptr_explore();
void user2(my_rectangle_sptr& ptr);
void user3();
void user4();
void fill_vector(vcl_vector<class T> vector);
void print_area(vcl_vector<my_rectangle_sptr> const& v);


//implement an sptr
void sptr_explore()
{
    vcl_cout<<"entering user1"<<vcl_endl;
    unsigned w = 10;
    unsigned h = 20;
    my_rectangle_sptr rect = new my_rectangle(w,h);
    my_rectangle_sptr copy_rect= rect;
    vcl_cout<<"ref_count= "<<rect->get_references()<<vcl_endl;
    user2(rect);
    vcl_cout<<"ref_count= "<<rect->get_references()<<vcl_endl;
    user3();
    user4();
    vcl_cout<<"exiting user1"<<vcl_endl;
}

//implement a copy of a copy sptr
void user2(my_rectangle_sptr& ptr)
{
    vcl_cout<<"entering user2"<<vcl_endl;
    my_rectangle_sptr copy_rect= ptr;
    vcl_cout<<"ref_count= "<<ptr->get_references()<<vcl_endl;
    vcl_cout<<"exiting user2"<<vcl_endl;
}

//implement an ptr
void user3()
{
    vcl_cout<<"entering user3"<<vcl_endl;
    my_rectangle *rect2= new my_rectangle(10,10);
    vcl_cout<<"ref2_count= "<<rect2->get_references()<<vcl_endl;
    vcl_cout<<"exiting user3"<<vcl_endl;
}

//implement an instance
void user4()
{
    vcl_cout<<"entering user4"<<vcl_endl;
    my_rectangle rect3(15,10);
    vcl_cout<<"ref3_count= "<<rect3.get_references()<<vcl_endl;
    vcl_cout<<"exiting user4"<<vcl_endl;
}

//this function fills v_out with the contents in v_in skipping the null vectors
void fill_vector(vcl_vector<my_rectangle_sptr> vector_in, vcl_vector<my_rectangle_sptr>& vector_out)
{        
    for (vcl_vector<my_rectangle_sptr>::iterator vit = vector_in.begin();
        vit != vector_in.end(); vit++){
            if (*vit==0)
                continue;
            vector_out.push_back(*vit);
    }
}


void fill_map(vcl_vector<my_rectangle_sptr> vector_in, vcl_map<unsigned, my_rectangle_sptr> &map_out)
{        
    unsigned key;
    

    for (vcl_vector<my_rectangle_sptr>::iterator vit = vector_in.begin();
        vit != vector_in.end(); vit++){
            if (*vit==0)
                continue;

            key=(*vit)->area();
            map_out.insert(vcl_pair<unsigned, my_rectangle_sptr>(key,  *vit));
            
    }
}

//prints area
void print_area(vcl_vector<my_rectangle_sptr> const& v)
{
    for (unsigned i=0; i<v.size(); i++)
    {    
        //vcl_cout<<v[i]->area()<<vcl_endl;
        vcl_cout<<*v[i]<<vcl_endl;
    }
}

#if 0

int main()
{
    unsigned w = 10;

    //function to explore what happens with ref_count()
    //and when smart pointer delete themselves
    sptr_explore();

    //a bunch of instances to fill a vector
    my_rectangle_sptr rect0 = new my_rectangle();
    rect0 = 0;
    my_rectangle_sptr rect1 = new my_rectangle();
    
    //vectors of my_rectangle_sptr
    vcl_vector<my_rectangle_sptr> v_in;
    vcl_vector<my_rectangle_sptr> v_out;


    //fill first 5 elements with null pointers
    for (unsigned i=0; i<5; i++)
    {
        v_in.push_back(rect0);
    }

    for (unsigned i=0; i<10; i++)
    {
        my_rectangle_sptr rect1 = new my_rectangle(10,10*i);
        v_in.push_back(rect1);
    }
    
    
    fill_vector(v_in, v_out);
    vcl_random_shuffle(v_out.begin(),v_out.end());
    vcl_cout<<"random order "<<vcl_endl;
    print_area(v_out);
    vcl_sort(v_out.begin(),v_out.end(), my_rectangle::less_than);
    vcl_cout<<"ascending order "<<vcl_endl;
    print_area(v_out);

    //create a map

    vcl_map<unsigned, my_rectangle_sptr> rmap;
    fill_map(v_out,rmap);
    vcl_cout<< "Map size =" <<rmap.size()<<vcl_endl;

    vcl_cout<< "No. element with area = 600 is " <<rmap.count(600)<< vcl_endl;
    


    while(1);
}

#endif

