#include <iostream>
#include <vector>
struct flag
{
	int _count;
	explicit flag(int count):_count(count) { std::cout<<"构造"<<std::endl;}
	~flag() { _count --; std::cout<<"析构"<<std::endl; if(0==_count) throw; }
	operator bool() 
	{ 
		std::cout<< ". count = " << _count << std::endl;
		return _count>0; 
	}
};

int main(int argc, const char*argv[])
{

#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_CATCH_UNIQUE_NAME_LINE( name, line ) INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line )
#define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __LINE__ )
//#define CAT(name) INTERNAL_CATCH_UNIQUE_NAME(name)
#define CAT(name) INTERNAL_CATCH_UNIQUE_NAME_LINE(name, __LINE__)

#define CASE(n)\
	static flag CAT(v)(n);\
	if(auto CAT(vv)=CAT(v))

	auto f = []()
	{
		CASE(3){
			std::cout<<__LINE__<<std::endl;
			CASE(1){
				std::cout<<__LINE__<<std::endl;
			}
			CASE(1){
				std::cout<<__LINE__<<std::endl;
			}
			CASE(1){
				std::cout<<__LINE__<<std::endl;
			}
		}
		CASE(1){
			std::cout<<__LINE__<<std::endl;
		}
		CASE(2){
			std::cout<<__LINE__<<std::endl;
			CASE(1){
				std::cout<<__LINE__<<std::endl;
			}
			CASE(1){
				std::cout<<__LINE__<<std::endl;
			}
		}
		CASE(1){
			std::cout<<__LINE__<<std::endl;
		}
	};

	while(true)
	{
		try { f(); break; } catch(...) {}
	}

	return 0;
}
