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
    for (auto&&__20160224__=section::section<__LINE__>(should_execute, max_depth_line);__20160224__;section::throw_exception())

#define SECTION_TREE_BEGIN()                      \
    static section::should_execute should_execute;\
    int max_depth_line = 0;                       \
    try {

#define SECTION_TREE_END()         \
    should_execute.clear();        \
    } catch(section::exception&) {}

#include <iostream>
void f() 
{
    SECTION_TREE_BEGIN()
    {
        std::cout<<"1"<<std::endl;
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
    SECTION_TREE_END()
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
