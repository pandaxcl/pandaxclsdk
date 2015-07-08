template<int h>
class gene
{
};

template<int ngene, typename Gene, ...>
class chromosome
{
};

namespace Private
{
	template<typename F> struct node_impl_base
	{
		F f;
	};
	template<typename F, int n> struct node_impl;
	template<typename F> struct node_impl<F,0>: public node_impl_base
	{
		std::result_of<F()>::type operator()
		{
			return f();
		}
	};
	template<typename F> struct node_impl<F,1>: public node_impl_base
	{
		template<typename T0>
		std::result_of<F(T0)>::type operator(T0 a0)
		{
			return f(a0);
		}
	};
	template<typename F> struct node_impl<F,2>: public node_impl_base
	{
		template<typename T0, typename T1>
		std::result_of<F(T0,T1)>::type operator(T0 a0, T1 a1)
		{
			return f(a0, a1);
		}
	};
}

template<typename F>
class node:public Private::node_impl<F, F::nops>
{
public:
	const int nops = F::nops;
};

