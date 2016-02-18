#include <iostream>
#include <vector>

template<long>
struct ShouldExecute
{
	static int flag;
	static bool OK() { return flag > 0; }
};
template<long N>
int ShouldExecute<N>::flag = 0;

int main(int argc, const char*argv[])
{
#define CASE(comment)\
	if (isBuild) { *p++; } else { *p--; } \
	p = &ShouldExecute<__LINE__>::flag; \
	if(ShouldExecute<__LINE__>::OK())

	int*p = &ShouldExecute<__LINE__>::flag;
	auto f = [&p](bool isBuild)
	{
		CASE("1"){
			std::cout<<__LINE__<<std::endl;
			CASE("1,1"){
				std::cout<<__LINE__<<std::endl;
			}
			CASE("1,2"){
				std::cout<<__LINE__<<std::endl;
			}
			CASE("1,3"){
				std::cout<<__LINE__<<std::endl;
			}
		}
		CASE("2"){
			std::cout<<__LINE__<<std::endl;
		}
		CASE("3"){
			std::cout<<__LINE__<<std::endl;
			CASE("3,1"){
				std::cout<<__LINE__<<std::endl;
			}
			CASE("3,2"){
				std::cout<<__LINE__<<std::endl;
			}
		}
		CASE("4"){
			std::cout<<__LINE__<<std::endl;
		}
	};
	f(true);
	std::cout<<"==========="<<std::endl;
	f(false);

	return 0;
}
