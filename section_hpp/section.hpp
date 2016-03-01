#include <set>

namespace section
{
    struct exception {};// 为了利用throw而引入的一个特殊的异常
    class condition
    {
        std::set<int> flags;
    public:
        bool should_execute  (int line) { return flags.end() == flags.find(line); }
        void record_executed (int line) { flags.insert(line); }
        void make_reenterable()         { flags.clear(); }
    };

	class counter
	{
		int _line = 0;
	public:
		void set_line(int line) { _line = line; }
		int  line() { return _line; }
		bool is_line(int line) { return _line == line; }
	};

	class context:public condition, public counter
	{
	};

    template<int LINE> struct section
    {
		context& _context;
        section(context&C): _context(C)
        {
            if (_context.should_execute(LINE))
                _context.set_line(LINE);
        }
        ~section() { if (_context.is_line(LINE)) _context.record_executed(LINE); }
        operator bool() { return _context.should_execute(LINE); }
    };
}// namespace section

#ifdef SECTION_HPP_ENABLE_SAMPLE

#define SECTION()\
    for (auto&&__20160224__=section::section<__LINE__>(section_context);__20160224__;throw section::exception())

#define SECTION_TREE() static section::context section_context;

#define SECTION_TREE_MAKE_REENTER() section_context.make_reenterable();


#include <iostream>
void f() 
{
    std::cout<<"1"<<std::endl;
    SECTION_TREE() try 
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
        SECTION_TREE_MAKE_REENTER()
    } catch (section::exception&) {}
}
int main()
{
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"==========="<<std::endl;
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"-----------"<<std::endl;
    f(); std::cout<<"==========="<<std::endl;
    return 0;
}

#endif// SECTION_HPP_ENABLE_SAMPLE
