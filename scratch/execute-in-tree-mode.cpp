#include <iostream>
#include <set>

class should_execute_t
{
	std::set<int> flags;
public:
	bool yes(int line)    { return flags.end() == flags.find(line); }
	void record(int line) { flags.insert(line); }
	void clear()          { flags.clear(); }
};
template<int LINE> struct section
{
	should_execute_t &_should_execute;
	int              &_max_depth_line;
	section(should_execute_t&should_execute, int&max_depth_line)
	: _should_execute(should_execute)
	, _max_depth_line(max_depth_line)
	{
		if (_should_execute.yes(LINE))
			_max_depth_line = LINE;
	}
	~section() 
	{
		if (LINE == _max_depth_line)
			_should_execute.record(LINE);
	}
	operator bool()
	{
		return _should_execute.yes(LINE);
	}
};

#define SECTION()\
	if (auto&&v=section<__LINE__>(should_execute, max_depth_line))

#define TEST_CASE(reenter)                 \
	static should_execute_t should_execute;\
	if (reenter) should_execute.clear();   \
	int max_depth_line = 0;                \
	SECTION()


void f(bool reenter=false)
{
	std::cout<<"1"<<std::endl;
	TEST_CASE(reenter)
	{
		std::cout<<"2"<<std::endl;
		SECTION() // 子代码段1
		{
			std::cout<<"3"<<std::endl;
			return;// 比较讨厌
		}
		SECTION() // 子代码段2
		{
			std::cout<<"4"<<std::endl;
			return;// 比较讨厌
		}
		return;// 比较讨厌
	}
}
int main()
{
	f();std::cout<<"-----------"<<std::endl;
	f();std::cout<<"-----------"<<std::endl;
	f();std::cout<<"-----------"<<std::endl;
	f();std::cout<<"-----------"<<std::endl;
	f();std::cout<<"-----------"<<std::endl;
	f();std::cout<<"==========="<<std::endl;
	f(true);std::cout<<"-----------"<<std::endl;
	f();std::cout<<"-----------"<<std::endl;
	f();std::cout<<"-----------"<<std::endl;
	f();std::cout<<"-----------"<<std::endl;
	f();std::cout<<"-----------"<<std::endl;
	f();std::cout<<"==========="<<std::endl;
	return 0;
}
