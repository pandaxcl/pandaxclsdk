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
    template<int LINE> struct section
    {
        condition &_condition;
        int       &_counter;
        
        section(condition&A, int&B): _condition(A), _counter(B)
        {
            if (_condition.should_execute(LINE))
                _counter = LINE;
        }
        ~section() { if (LINE == _counter) _condition.record_executed(LINE); }
        operator bool() { return _condition.should_execute(LINE); }
    };
}// namespace section



#define SECTION()\
    for (auto&&__20160224__=section::section<__LINE__>(section_condition, section_counter);__20160224__;throw section::exception())

#define SECTION_TREE()                            \
    static section::condition section_condition;  \
    int                       section_counter = 0;

#define SECTION_TREE_MAKE_REENTER()\
    section_condition.make_reenterable();


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
