#include <memory>
#include <vector>
#include <queue>
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
template<typename F=int>
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


template<typename Unit>
struct group:private std::vector<Unit>
{
	template<size_t N, typename FitnessType, typename Fitness, typename Result>
	Result roulette_wheel(const Fitness&fitnesses)
	{
		FitnessType probabilityOfAccum[N];
		FitnessType accumFitness = 0.0;
	
		for(auto&fitness:fitnesses)
			accumFitness += fitness;
	
		size_t i = 0;
		for(auto&fitness:fitnesses)
		{
			FitnessType p = fitnesses/accumFitness;
			accumProbability += p;
			probabilityOfAccum[i] = accumProbability;
			i++;
		}
	
		Result result;
		return result;
	}
};

/***************************************************
g++ -std=c++11 -DTEST_WITH_MAIN_FOR_GEP_HPP=1 -x c++ %
***************************************************/

#if TEST_WITH_MAIN_FOR_GEP_HPP

int main(int argc, const char*argv[])
{
	return 0;
}

#endif// TEST_WITH_MAIN_FOR_GEP_HPP
