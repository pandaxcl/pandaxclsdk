#include <memory>
#include <vector>
#include <queue>
#include <cmath>
#include <random>
#include <cassert>
//namespace Private
//{
//	template<typename F> struct node_impl_base
//	{
//		std::weak_ptr<node> parent;
//		std::vector<std::shared_ptr<node>> children;
//		F f;
//	};
//	template<typename F, int n> struct node_impl;
//	template<typename F> struct node_impl<F,0>: public node_impl_base
//	{
//		std::result_of<F()>::type operator()
//		{
//			return f();
//		}
//	};
//	template<typename F> struct node_impl<F,1>: public node_impl_base
//	{
//		template<typename T0>
//		std::result_of<F(T0)>::type operator(T0 a0)
//		{
//			return f(a0);
//		}
//	};
//	template<typename F> struct node_impl<F,2>: public node_impl_base
//	{
//		template<typename T0, typename T1>
//		std::result_of<F(T0,T1)>::type operator(T0 a0, T1 a1)
//		{
//			return f(a0, a1);
//		}
//	};
//}

template<typename F=int> class node;
template<typename F=int>
struct with_function
{
	F f;
	bool is_terminal()
	{
		return f < 0;
	}
	size_t args_count()
	{
		return 2;
	}
};

template<typename F=int>
class DNA:public with_function<F>
{
	DNA(const node<F>&n) { this->f = n->f; }
	DNA(const F f) { this->f = f; }
};
template<typename F=int,typename Real=double>
class gene:private std::vector<DNA<F>>
{
	typedef std::shared_ptr<node<F>> node_ptr;
	node_ptr to_tree()
	{
		const gene<F>&K = *this;
		std::queue<node_ptr> Q;
		size_t p = 0;
		auto T = node_ptr(new node<F>(K[p]));
		p++;
		Q.push_back(T);
		while(!Q.empty())
		{
			auto n = Q.top(); Q.pop_front();
			if(n->is_terminal())
				continue;

			size_t a = n->args_count();
			for(size_t i=0;i<a;i++)
			{
				auto m = node_ptr(new node<F>(K[p]));
				n.children.push_back(m);
				p++;
				Q.push_back(m);
			}
		}
		return T;
	}
    template<int N_functions, int N_terminals>
    void mutate(Real probability, int F_T[])
    {
        Real p = 0.0;
        for (int i=0;i<this->size();i++)
        {
            p = static_cast<Real>(std::rand())/RAND_MAX;
            if (p <= probability)
            {

            }
        }
    }
};

template<typename F/*=int*/>
class node:public with_function<F>
{
	std::weak_ptr<node<F>> parent;
	std::vector<std::shared_ptr<node<F>>> children;

public:
	node(const DNA<F>&n) { this->f = n->f; }
	node(const F f) { this->f = f; }
	//std::result_of<F(decltype(children))>::type operator()
	//{
	//	return f(children);
	//}

	gene<F> to_k_expression()
	{
		std::queue<node<F>*> Q;
		gene<F> K;
		Q.push_back(this);
		while(!Q.empty())
		{
			node<F>*n = Q.top();Q.pop_front();
			K.push_back(DNA<F>(*n));
			if(n->is_terminal())
				continue;
			for(auto&m:children)
				Q.push_back(m);
		}

		return K;
	}
};

template<typename F=int>
class chromosome:private std::vector<gene<F>>
{
};


template<int N_units, typename Unit, int N_functions, int N_terminals, typename Real=double>
struct gene_experssion_program
{
    std::function<Real(const Unit&)> lambda_fitness;// 适应度函数
    std::function<void()> inner_lambda_fitness_standard; // 标准适应度计算
    std::function<void()> inner_lambda_selection_roulette_wheel; // 轮盘赌
    
//    template<int N_args, int N_result>
    gene_experssion_program()
    {
        struct Local
        {
            Unit*units;
            Unit __units_front[N_units];
            Unit __units_back[N_units];
            Real fitnesses[N_units];
            int F_T[N_functions+N_terminals];
            
            Local()
            {
                units = __units_back;
            }
            inline Unit*next_generation()
            {
                assert(nullptr != units);
                return units == __units_front? __units_back : __units_front;
            }
        };
        
        auto local = std::shared_ptr<Local>(new Local());
        std::srand((unsigned int)time(nullptr));
        
        inner_lambda_fitness_standard = [local,this]()
        {
            for(int i = 0; i<N_units; i++)
            {
                local->fitnesses[i] = this->lambda_fitness(local->units[i]);
            }
        };
        
        inner_lambda_selection_roulette_wheel = [local]()
        {
            Real probabilityOfAccum[N_units];
            Real totalFitness = 0.0;
            
            for(int i = 0; i<N_units; i++)
                totalFitness += local->fitnesses[i];
            
            Real accumProbability = 0.0;
            for(int i = 0; i<N_units; i++)
            {
                Real p = local->fitnesses[i]/totalFitness;
                accumProbability += p;
                probabilityOfAccum[i] = accumProbability;
            }
            
            
            Unit*nextGeneration = local->next_generation();
            Real p = static_cast<Real>(std::rand())/RAND_MAX;

            for (int i=0; i<N_units; i++)
            {
                for (int j=0;j<N_units;j++)
                {
                    if(p <= probabilityOfAccum[j])
                    {
                        nextGeneration[i] = local->units[j];
                    }
                }
            }
            
            local->units = nextGeneration;
        };
    }
};

/***************************************************
g++ -std=c++11 -DTEST_WITH_MAIN_FOR_GEP_HPP=1 -x c++ %
***************************************************/

#if TEST_WITH_MAIN_FOR_GEP_HPP

int main(int argc, const char*argv[])
{
    auto GEP = gene_experssion_program<100, chromosome<>, 10, 10>();
	return 0;
}

#endif// TEST_WITH_MAIN_FOR_GEP_HPP
