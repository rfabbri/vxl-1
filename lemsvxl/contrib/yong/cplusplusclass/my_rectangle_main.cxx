#include <cplusplusclass/my_rectangle.h>
#include <cplusplusclass/my_rectangle_sptr.h>
#include <cplusplusclass/yong_star.h>
#include <cplusplusclass/yong_star_sptr.h>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>

void sptr_explore();
void user2(my_rectangle_sptr& ptr);
void user3();
void user4();
void fill_vector(std::vector<class T> vector);
void print_area(std::vector<my_rectangle_sptr> const& v);


//implement an sptr
void sptr_explore()
{
    std::cout<<"entering user1"<<std::endl;
    unsigned w = 10;
    unsigned h = 20;
    my_rectangle_sptr rect = new my_rectangle(w,h);
    my_rectangle_sptr copy_rect= rect;
    std::cout<<"ref_count= "<<rect->get_references()<<std::endl;
    user2(rect);
    std::cout<<"ref_count= "<<rect->get_references()<<std::endl;
    user3();
    user4();
    std::cout<<"exiting user1"<<std::endl;
}

//implement a copy of a copy sptr
void user2(my_rectangle_sptr& ptr)
{
    std::cout<<"entering user2"<<std::endl;
    my_rectangle_sptr copy_rect= ptr;
    std::cout<<"ref_count= "<<ptr->get_references()<<std::endl;
    std::cout<<"exiting user2"<<std::endl;
}

//implement an ptr
void user3()
{
    std::cout<<"entering user3"<<std::endl;
    my_rectangle *rect2= new my_rectangle(10,10);
    std::cout<<"ref2_count= "<<rect2->get_references()<<std::endl;
    std::cout<<"exiting user3"<<std::endl;
}

//implement an instance
void user4()
{
    std::cout<<"entering user4"<<std::endl;
    my_rectangle rect3(15,10);
    std::cout<<"ref3_count= "<<rect3.get_references()<<std::endl;
    std::cout<<"exiting user4"<<std::endl;
}

//this function fills v_out with the contents in v_in skipping the null vectors
void fill_vector(std::vector<my_rectangle_sptr> vector_in, std::vector<my_rectangle_sptr>& vector_out)
{        
    for (std::vector<my_rectangle_sptr>::iterator vit = vector_in.begin();
        vit != vector_in.end(); vit++){
            if (*vit==0)
                continue;
            vector_out.push_back(*vit);
    }
}


void fill_map(std::vector<my_rectangle_sptr> vector_in, std::map<unsigned, my_rectangle_sptr> &map_out)
{        
    unsigned key;
    

    for (std::vector<my_rectangle_sptr>::iterator vit = vector_in.begin();
        vit != vector_in.end(); vit++){
            if (*vit==0)
                continue;

            key=(*vit)->area();
            map_out.insert(std::pair<unsigned, my_rectangle_sptr>(key,  *vit));
            
    }
}

//prints area
void print_area(std::vector<my_rectangle_sptr> const& v)
{
    for (unsigned i=0; i<v.size(); i++)
    {    
        //std::cout<<v[i]->area()<<std::endl;
        std::cout<<*v[i]<<std::endl;
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
    std::vector<my_rectangle_sptr> v_in;
    std::vector<my_rectangle_sptr> v_out;


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
    std::random_shuffle(v_out.begin(),v_out.end());
    std::cout<<"random order "<<std::endl;
    print_area(v_out);
    std::sort(v_out.begin(),v_out.end(), my_rectangle::less_than);
    std::cout<<"ascending order "<<std::endl;
    print_area(v_out);

    //create a map

    std::map<unsigned, my_rectangle_sptr> rmap;
    fill_map(v_out,rmap);
    std::cout<< "Map size =" <<rmap.size()<<std::endl;

    std::cout<< "No. element with area = 600 is " <<rmap.count(600)<< std::endl;
    


    while(1);
}

#endif

