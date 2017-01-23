/* 
g++ -I/usr/local/include boost-coroutine-test.cpp -std=c++11 -L/usr/local/lib -lboost_coroutine -lboost_context-mt
*/

#include <boost/coroutine2/all.hpp>
#include <iostream>

int main(int argc, const char*argv[])
{
	typedef boost::coroutines2::coroutine<int> coro_t;

	coro_t::pull_type source(
		[&](coro_t::push_type&sink){
			int first = 1, second = 1;
			sink(first);
			sink(second);
			for (int i=0; i<8; i++)
			{
				int third = first + second;
				first = second;
				second = third;
				sink(third);
			}
		}
	);

	for (auto i:source)
		std::cout << i << " ";
	std::cout<<std::endl;

	return 0;
}
