//This is transition_table.cxx

#include <vcl_map.h>
#include <vcl_iostream.h>

int main()
{
    typedef vcl_map<unsigned, vcl_map<unsigned,vcl_map<unsigned,unsigned> > > 
        transition_array;
    transition_array transition_array_;
    transition_array_[1][1][0] = 1;
    transition_array_[1][1][1] = 1;
    transition_array_[1][1][2] = 1;
    transition_array_[1][2][3] = 1;

    vcl_cout << "transition_array_[1].count() = "
             << transition_array_[1].count(3) << vcl_endl;

    return 0;
}  
