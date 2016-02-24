#include <set>

namespace section
{
    struct exception {};// 为了利用throw而引入的一个特殊的异常
    class should_execute
    {
        std::set<int> flags;
    public:
        bool yes   (int line) { return flags.end() == flags.find(line); }
        void record(int line) { flags.insert(line); }
        void clear ()         { flags.clear(); }
    };
    template<int LINE> struct section
    {
        should_execute &_should_execute;
        int            &_max_depth_line;
        
        section(should_execute&A, int&B)
        : _should_execute(A)
        , _max_depth_line(B)
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
	void throw_exception() { throw exception(); }
}// namespace section



#define SECTION()\
	for (auto&&v=section::section<__LINE__>(should_execute, max_depth_line);v;section::throw_exception())

#define SECTION_TREE(reenter)                     \
	static section::should_execute should_execute;\
	if (reenter) should_execute.clear();          \
	int max_depth_line = 0;                       \
	SECTION()

#include <iostream>
void f(bool reenter=false) try
{
	std::cout<<"1"<<std::endl;
	SECTION_TREE(reenter)
	{
		std::cout<<"2"<<std::endl;
		SECTION() // 子代码段1
		{
			std::cout<<"3"<<std::endl;
		}
		SECTION() // 子代码段2
		{
			std::cout<<"4"<<std::endl;
		}
	}
}
catch(section::exception&)
{
}

int main()
{
	f(    ); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"==========="<<std::endl;
	f(true); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"-----------"<<std::endl;
	f(    ); std::cout<<"==========="<<std::endl;
	return 0;
}
