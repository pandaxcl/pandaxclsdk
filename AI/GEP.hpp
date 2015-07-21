#include <memory>
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

template<
    int N_units,
    int N_genes,
    int N_headers,
    int N_maxops,
    int N_functions,
    int N_terminals,
    typename T_Real=double,
    typename T_DNA_encode=int
>
struct gene_experssion_program
{
    typedef T_Real Real;
    typedef T_DNA_encode DNA_encode;
    class node;
    typedef std::shared_ptr<node> node_ptr;
    struct with_function
    {
        DNA_encode f;
        inline bool is_terminal(gene_experssion_program&GEP)
        {
            return GEP.lambda_is_terminal(f);
        }
        inline size_t arg_count(gene_experssion_program&GEP)
        {
            return GEP.lambda_arg_count(f);
        }
    };
    
    struct DNA:public with_function
    {
        DNA(){ this->f = 0; }
        DNA(const node&n):DNA(n->f) {}
        DNA(const DNA_encode f) { this->f = f; }
    };
    struct gene
    {
        static const int N_tails = N_headers*(N_maxops-1)+1;
        static const int N_DNAs = N_headers+N_tails;
        DNA DNAs[N_DNAs];
        
        node_ptr to_tree(gene_experssion_program&GEP)
        {
            auto K = this->DNAs;
            std::queue<node_ptr> Q;
            size_t p = 0;
            auto T = node_ptr(new node(K[p]));
            p++;
            Q.push(T);
            while(!Q.empty())
            {
                const auto&n = Q.front(); Q.pop();
                if(n->is_terminal(GEP))
                    continue;
                
                size_t a = n->arg_count(GEP);
                for(size_t i=0;i<a;i++)
                {
                    auto m = node_ptr(new node(K[p]));
                    n->add_children(m);m->parent = n;
                    p++;
                    Q.push(m);
                }
            }
            return T;
        }
    public:
        Real eval(gene_experssion_program&GEP)
        {
            node_ptr root = to_tree(GEP);
            return root->eval(GEP);
        }
        
        void evolve_mutate(Real probability, int F_T[])
        {
            Real p = 0.0;
            for (int i=0;i<N_DNAs;i++)
            {
                p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
                if (p <= probability)
                {
                    if (i<N_headers)// 可以选择所有的符号
                        DNAs[i] = F_T[std::rand()%(N_functions+N_terminals)];
                    else// 只能选择终结符
                        DNAs[i] = F_T[N_functions+std::rand()%N_terminals];
                }
            }
        }
        void evolve_reverse(Real probability) // 倒串
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nStart = std::rand()%N_headers;// 倒串只作用到基因的头部
                int nLength = std::rand()%(N_headers-nStart-2) + 2;// 至少有两个DNA，才谈得上倒串
                
                for (int i=nStart; i<nStart+nLength; i++)
                {
                    // 12 345 678
                    // nStart = 2;
                    // nLength = 3;
                    // i=2, 2+3-i+1=4
                    // i=3, 2+3-i+1=3
                    // i=4, 2+3-i+1=2
                    std::swap(DNAs[i], DNAs[nStart+nLength-i+1]);
                }
            }
        }
        
        void evolve_insert_string(Real probability)// 插串
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nStart = std::rand()%(N_headers+N_tails);
                int nLength = std::rand()%(N_headers+N_tails-1)+1;// 至少得有1个基因，才能执行插串
                int nInsert = std::rand()%(N_headers-1)+1; // 因为不能插到基因的第一个位置
                for (int i=nInsert; i<nInsert+nLength && i+nLength<N_headers; i++)// 插入位置的nLength长度的DNA后移，超出头部长度的直接丢弃
                {
                    // 01234 56 7
                    // 12345 67 8
                    // nStart = 5
                    // nLength = 2
                    // nInsert = 3
                    
                    DNAs[i+nLength] = DNAs[i];
                }
                for (int i=nInsert; i<nInsert+nLength; i++)
                {
                    DNAs[i] = DNAs[nStart+i-nInsert];
                }
            }
        }
        
        void evolve_root_insert_string(Real probability)// 根插串
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nStart = std::rand()%(N_headers+N_tails);
                int nLength = std::rand()%(N_headers+N_tails);
                int nInsert = 0; // 根插串就是将插串插到基因的第一个位置
            }
        }
        
        void evolve_single_crossover(Real probability, gene&another)
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nCrossOver = std::rand()%(N_headers+N_tails);
                for (int i=nCrossOver; i<N_headers+N_tails; i++)
                {
                    std::swap(DNAs[i], another.DNAs[i]);
                }
            }
        }
        
        void evolve_double_crossover(Real probability, gene&another)
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nCrossOver = std::rand()%(N_headers+N_tails);
                int nLength = std::rand()%(N_headers+N_tails);
                for (int i=nCrossOver; i<nCrossOver+nLength && i<N_headers+N_tails; i++)
                {
                    std::swap(DNAs[i], another.DNAs[i]);
                }
            }
        }
    };
    
    class chromosome
    {
        gene genes[N_genes];
        Real eval(gene_experssion_program&GEP)
        {
            Real result = 0.0;
            for(auto&gene:genes)
                result += gene.eval(GEP);
            return result;
        }
        void evolve_mutate(Real probability, int F_T[])
        {
            for(auto&g:genes)
                g.evolve_mutate(probability, F_T);
        }
        void evolve_reverse(Real probability) // 倒串
        {
            for(auto&g:genes)
                g.evolve_reverse(probability);
        }
        void evolve_insert_string(Real probability)// 插串
        {
            for(auto&g:genes)
                g.evolve_insert_string(probability);
        }
        void evolve_root_insert_string(Real probability)// 根插串
        {
            for(auto&g:genes)
                g.evolve_root_insert_string(probability);
        }
        
        void evolve_single_crossover(Real probability, chromosome&another)
        {
            for(int i=0; i< N_genes; i++)
                genes[i].evolve_single_crossover(probability, another.genes[i]);
        }
        
        void evolve_double_crossover(Real probability, chromosome&another)
        {
            for(int i=0; i< N_genes; i++)
                genes[i].evolve_double_crossover(probability, another.genes[i]);
        }
        
        void evolve_gene_crossover(Real probability, chromosome&another)
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nCrossOver = std::rand()%(gene::N_headers+gene::N_tails);
                std::swap(genes[nCrossOver], another.genes[nCrossOver]);
            }
        }
    };
    
    class node:public with_function
    {
        std::weak_ptr<node> parent;
        //std::array<node_ptr, N_maxops> children;
        size_t _size = 0;
        node_ptr children[N_maxops];
    public:
        size_t size_of_children() { return this->_size; }
        void add_children(node_ptr&ptr)
        {
            children[_size] = ptr;
            _size++;
        }
        node(const DNA&n):node(n.f) {}
        node(const DNA_encode f) { this->f = f; }
        
        inline gene to_k_expression()
        {
            std::queue<node*> Q;
            gene K;
            Q.push_back(this);
            while(!Q.empty())
            {
                node*n = Q.top();Q.pop_front();
                K.push_back(DNA(*n));
                if(n->is_terminal())
                    continue;
                for(auto&m:children)
                    Q.push_back(m);
            }
            
            return K;
        }
        
        inline Real eval(gene_experssion_program&GEP)
        {
            Real valuesOfChildren[N_maxops];
            for(int i=0; i<this->size_of_children(); i++)
                valuesOfChildren[i] = children[i]->eval(GEP);

            return GEP.lambda_eval(this->f, this->size_of_children(), valuesOfChildren);
        }
    };
    
    typedef typename std::conditional<1 == N_genes, gene, chromosome>::type Unit;

    std::function<bool(DNA_encode)> lambda_is_terminal;
    std::function<int(DNA_encode)> lambda_arg_count;
    std::function<Real(DNA_encode, int argc, Real argv[])> lambda_eval;// 表达式计算函数
    
    std::function<Real(const Unit&)> lambda_fitness;// 适应度函数
    
    
    std::function<void()> inner_lambda_fitness_standard; // 标准适应度计算
    std::function<void()> inner_lambda_selection_roulette_wheel; // 轮盘赌
    std::function<void(Real probability)> inner_lambda_mutation_standard;// 标准变异过程
    
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
                //local->fitnesses[i] = local->units[i].eval(*this);
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
        
        inner_lambda_mutation_standard = [local](Real probability)
        {
            for(int i=0; i<N_units; i++)
            {
                Unit&unit = local->units[i];
                unit.evolve_mutate(probability, local->F_T);
            }
        };
    }
};

/***************************************************
g++ -std=c++11 -DTEST_WITH_MAIN_FOR_GEP_HPP=1 -x c++ %
***************************************************/

#if TEST_WITH_MAIN_FOR_GEP_HPP
#include <map>
int main(int argc, const char*argv[])
{
    typedef gene_experssion_program<
    /*int N_units    */100,
    /*int N_genes    */1,
    /*int N_headers  */8,
    /*int N_maxops   */2,
    /*int N_functions*/4,
    /*int N_terminals*/6>GEP_t;
    
    auto GEP = GEP_t();
    
    typedef GEP_t::Real Real;
    typedef GEP_t::DNA_encode DNA_encode;
    typedef GEP_t::Unit Unit;
    
    
    struct{
        unsigned char token;
        unsigned int argc;
        std::function<Real(int argc, Real argv[])> eval;
    } ops[] = {
        { 0 , 0, nullptr},
        {'+', 2, [](int argc, Real argv[]){return argv[0]+argv[1];}},
        {'-', 2, [](int argc, Real argv[]){return argv[0]-argv[1];}},
        {'*', 2, [](int argc, Real argv[]){return argv[0]*argv[1];}},
        {'/', 2, [](int argc, Real argv[]){return argv[0]/argv[1];}},
    };
    
    std::map<DNA_encode, int> I;
    {int i=0;for (auto&op:ops) {I[op.token] = i;i++;}}
    
    
    GEP.lambda_arg_count = [&I,ops](DNA_encode DNA){return ops[I[DNA]].argc;};
    GEP.lambda_is_terminal = [&I,ops](DNA_encode DNA){return 0 == ops[I[DNA]].argc && 0 != ops[I[DNA]].token;};
    GEP.lambda_fitness = [](const Unit&unit)->Real{ return 0.0; };
    GEP.lambda_eval = [&I,ops](DNA_encode DNA, int argc, Real argv[]){ return ops[I[DNA]].eval(argc, argv); };
    
    GEP_t::gene g;
    
	return 0;
}

#endif// TEST_WITH_MAIN_FOR_GEP_HPP
