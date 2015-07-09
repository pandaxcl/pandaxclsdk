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

struct with_function
{
	int f;
};

class DNA:public with_function
{
};
class gene:private std::vector<DNA>
{
};

class node:public with_function
{
	std::weak_ptr<node> parent;
	std::vector<std::shared_ptr<node>> children;

public:
	std::result_of<F(decltype(children))>::type operator()
	{
		return f(children);
	}

	gene to_k_expression()
	{
	}
	void from_k_expression(const gene&k)
	{
	}
};

class chromosome:private std::vector<gene>
{
};

