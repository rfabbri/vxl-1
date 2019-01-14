//This is transition_table.cxx

#include <map>
#include <iostream>

int main()
{
    typedef std::map<unsigned, std::map<unsigned,std::map<unsigned,unsigned> > > 
        transition_array;
    transition_array transition_array_;
    transition_array_[1][1][0] = 1;
    transition_array_[1][1][1] = 1;
    transition_array_[1][1][2] = 1;
    transition_array_[1][2][3] = 1;

    std::cout << "transition_array_[1].count() = "
             << transition_array_[1].count(3) << std::endl;

    return 0;
}  
