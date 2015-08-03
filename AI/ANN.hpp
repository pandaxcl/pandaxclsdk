
template
<
    int N_neurons,
    int N_connections,
    int N_functions,
    typename Real=double,
    typename Integer=int,
    typename RandomGenerator=std::default_random_engine
>
class artificial_neural_network
{
    void train()
    {
        struct Local
        {
            RandomGenerator randomGenerator;
        };
        auto local = std::make_shared<Local>(new Local());
    }
    
    std::function<void()> inner_lambda_random_initialize;
    std::function<void()> inner_lambda_compute;
    std::function<void(std::ostream&)> inner_lambda_save;
    std::function<void(std::istream&)> inner_lambda_load;
    std::function<void()> inner_lambda_simplify;
    artificial_neural_network()
    {
        struct Local
        {
            std::function<Real(Real)> functions[N_functions];
            struct{
                Real value;
                bool has_value = false;
                Integer f = 0;// 函数所对应的索引
            }neurons[N_neurons];
            
            struct{
                bool enabled = true;
                Integer i;
                Integer j;
                Real weight;
            }connections[N_connections];
        };
        
        auto local = std::make_shared<Local>(new Local());
    }
};

#if TEST_WITH_MAIN_FOR_ANN_HPP
int main(int argc, const char*argv[])
{
    return 0;
}
#endif
