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

// Forward-declare an expression wrapper
template<typename Expr>
struct calculator;

// Define a calculator domain. Expression within
// the calculator domain will be wrapped in the
// calculator<> expression wrapper.
struct calculator_domain
: boost::proto::domain< boost::proto::generator<calculator> >
{};

// Define a calculator expression wrapper. It behaves just like
// the expression it wraps, but with an extra operator() member
// function that evaluates the expression.
template<typename Expr>
struct calculator
: boost::proto::extends<Expr, calculator<Expr>, calculator_domain>
{
    typedef
    boost::proto::extends<Expr, calculator<Expr>, calculator_domain>
    base_type;
    
    calculator(Expr const &expr = Expr())
    : base_type(expr)
    {}
    
    typedef double result_type;
    
    // Overload operator() to invoke proto::eval() with
    // our calculator_context.
    double operator()(double a1 = 0, double a2 = 0) const
    {
        calculator_context ctx;
        ctx.args.push_back(a1);
        ctx.args.push_back(a2);
        
        return boost::proto::eval(*this, ctx);
    }
};

TEST_CASE("boost::proto", "[BOOST][active]")
{
    using namespace boost;
    
    SECTION("Hello World")
    {
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

    SECTION("domain")
    {
        // Define the Protofied placeholder terminals, in the
        // calculator domain.
        calculator<proto::terminal<placeholder<0> >::type> const _1;
        calculator<proto::terminal<placeholder<1> >::type> const _2;
        
        double result = ((_2 - _1) / _2 * 100)(45.0, 50.0);
        REQUIRE(result == (50.0 - 45.0) / 50.0 * 100);
    }

}