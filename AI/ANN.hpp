
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
        
    }
    
    std::function<void()> inner_lambda_random_initialize;
    artificial_neural_network()
    {
        struct Local
        {
            RandomGenerator randomGenerator;
            std::function<Real(Real)> functions[N_functions];
            struct{
                Real value;
                Integer F;// 函数所对应的索引
            }neurons[N_neurons];
            struct{
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
