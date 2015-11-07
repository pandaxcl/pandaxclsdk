#include <cpplinq.hpp>
#include <iostream>

int computes_a_sum ()
{
    using namespace cpplinq;
    int ints[] = {3,1,4,1,5,9,2,6,5,4};
    
    auto result = from_array (ints)
    >> where ([](int i) {return i%2 == 0;}) // Keep only even numbers
    >> sum()
//    >> aggregate(0, [](int sum, int x){return sum+x;}) // Sum remaining numbers
    ;
    return result;
}

void do_something_about_customer();

int main(int argc, const char*argv[])
{
    std::cout<<computes_a_sum()<<std::endl;
    do_something_about_customer();
	return 0;
}

void do_something_about_customer()
{
    using namespace cpplinq;
    struct customer
    {
        std::size_t id;
        std::string first_name;
        std::string last_name;
        
        customer (std::size_t id = 0, std::string first_name = "", std::string last_name = "")
        :id(std::move(id)), first_name(std::move(first_name)), last_name(std::move(last_name))
        {
        }
    };
    
    customer customers[] = {
        customer (1 , "Bill"    , "Gates"   ),
        customer (2 , "Steve"   , "Jobs"    ),
        customer (4 , "Linus"   , "Torvalds"),
        customer (11, "Steve"   , "Ballmer" )
    };
    
    auto f = from_array(customers) >> first_or_default();
    std::cout << f.last_name << std::endl; // prints Gates
    
    auto l = from_array(customers) >> last_or_default();
    std::cout << l.last_name << std::endl; // prints Ballmer
    
    
    struct customer_address
    {
        std::size_t id;
        std::size_t customer_id;
        std::string country;
        
        customer_address(std::size_t id, std::size_t customer_id, std::string country)
        : id(std::move (id)), customer_id (std::move(customer_id)), country(std::move(country))
        {
        }
    };
    
    customer_address customer_addresses[] =
    {
        customer_address (1, 1, "USA"       ),
        customer_address (2, 4, "Finland"   ),
        customer_address (3, 11, "USA"      ),
    };
    
    auto result = from_array(customers)
    >> join (
             from_array(customer_addresses),
             [](customer const & c) {return c.id;},
             [](customer_address const & ca) {return ca.customer_id;},
             [](customer const & c, customer_address const & ca) {return std::make_pair (c, ca);});
    
    result >> for_each([](std::pair<customer, customer_address> const & p) {
        std::cout << p.first.last_name << " lives in " << p.second.country << std::endl;
    });
    
}