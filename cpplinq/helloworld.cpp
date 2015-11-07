#include <cpplinq/linq.hpp>
#include <iostream>

int computes_a_sum ()
{
    using namespace cpplinq;
    int ints[] = {3,1,4,1,5,9,2,6,5,4};
    
    auto result = from (std::begin(ints), std::end(ints))
    . where ([](int i) {return i%2 == 0;}) // Keep only even numbers
    . aggregate(0, [](int sum, int x){return sum+x;}) // Sum remaining numbers
    ;
    return result;
}

int main(int argc, const char*argv[])
{
    std::cout<<computes_a_sum()<<std::endl;
	return 0;
}
