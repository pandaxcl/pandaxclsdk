//
//  boost.cpp
//  study-opengl
//
//  Created by 熊 春雷 on 16/1/1.
//  Copyright © 2016年 pandaxcl. All rights reserved.
//

#include <catch.hpp>
#include <boost/proto/proto.hpp>

// Define a placeholder type
template<int I>
struct placeholder
{};

struct calculator_context
: boost::proto::callable_context< calculator_context const >
{
    // Values to replace the placeholders
    std::vector<double> args;
    
    // Define the result type of the calculator.
    // (This makes the calculator_context "callable".)
    typedef double result_type;
    
    // Handle the placeholders:
    template<int I>
    double operator()(boost::proto::tag::terminal, placeholder<I>) const
    {
        return this->args[I];
    }
};



TEST_CASE("boost::proto", "[BOOST][active]")
{
    using namespace boost;
    // Define the Protofied placeholder terminals
    proto::terminal<placeholder<0> >::type const _1 = {{}};
    proto::terminal<placeholder<1> >::type const _2 = {{}};
    
    
    calculator_context ctx;
    ctx.args.push_back(45); // the value of _1 is 45
    ctx.args.push_back(50); // the value of _2 is 50
    
    // Create an arithmetic expression and immediately evaluate it
    double d = proto::eval( (_2 - _1) / _2 * 100, ctx );
    
    // This prints "10"
    std::cout << d << std::endl;

}