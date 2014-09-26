template<typename F, typename...Etc>
struct overloaded_function:F, overloaded_function<Etc...>
{
	overloaded_function(F const&f, Etc...etc):F(f),overloaded_function<Etc...>(etc...){}
	using F::operator();
	using overloaded_function<Etc...>::operator();
};
template<typename F> struct overloaded_function<F>:F 
{ 
	using F::operator();
	overloaded_function(F const&f):F(f){}
};

template<typename...Etc>
overloaded_function<Etc...> make_overloaded_function(Etc...etc)
{
	return overloaded_function<Etc...>(etc...);
}

#if PANDAXCL_OVERLOADED_FUNCTION_TEST
// :map \\ :!g++ -DPANDAXCL_OVERLOADED_FUNCTION_TEST=1 -xc++ -std=c++1y %<CR>
#include <iostream>
int main(int argc, const char*argv[])
{
	{// normal lambda
		int x = 100;
		auto f = make_overloaded_function(
		[&x](int a){std::cout<<"a = "<<a<<std::endl; x = 123;},
		[&x](const char* s){std::cout<<"s = "<<s<<std::endl; x *= 2;}
		);
		f(1);std::cout<<"x="<<x<<std::endl;
		f("pandaxcl");std::cout<<"x="<<x<<std::endl;
	}
	{// generic lambda
		int x = 100;
		auto f = make_overloaded_function(
		[&x](int a, auto b){std::cout<<"a = "<<a<<std::endl; x = 123;},
		[&x](const char* s, auto b){
			// look at "b.hello();" line, it can simulate local template!!!
			b.hello();// notice here: it is an ERROR, but can PASS compile if not used!!!
			std::cout<<"s = "<<s<<std::endl; x *= 2;
		});
		f(1, 0);std::cout<<"x="<<x<<std::endl;
		//f("pandaxcl", 0);std::cout<<"x="<<x<<std::endl;// not use this call!!!
	}
	return 0;
}
#endif//PANDAXCL_OVERLOADED_FUNCTION_TEST
