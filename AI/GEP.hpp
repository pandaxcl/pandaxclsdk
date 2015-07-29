#include <memory>
#include <queue>
#include <cmath>
#include <random>
#include <cassert>
#include <string>
#include <algorithm>
#include <iostream>
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
//    int N_functions,
//    int N_terminals,
    typename T_DNA_encode=int,
    typename T_Real=double

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
        inline int arg_count(gene_experssion_program&GEP)
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
    class gene
    {
        enum
        {
            N_tails = N_headers*(N_maxops-1)+1,
            N_DNAs = N_headers+N_tails,
        };

        DNA DNAs[N_DNAs];
    public:
        node_ptr to_tree(gene_experssion_program&GEP) const
        {
            //std::cout<<"============= to_tree ============="<<std::endl;
            auto K = this->DNAs;
            std::queue<node_ptr> Q;
            size_t p = 0;
            auto T = node_ptr(new node(K[p]));
            p++;
            Q.push(T);
            while(!Q.empty())
            {
                auto n = Q.front();
                Q.pop();
                if(n->is_terminal(GEP))
                    continue;
                
                int a = n->arg_count(GEP);
                assert(a > 0);
                assert(a <= N_maxops);
                for(int i=0;i<a;i++)
                {
                    //this->dump(std::cout, true);
                    //T->dump(std::cout, 0);
                    assert(p < N_DNAs);
                    auto m = node_ptr(new node(K[p]));
                    n->add_children(m);
                    p++;
                    Q.push(m);
                    //std::cout<<"Q.size() = "<<Q.size()<<" N_maxops = "<<N_maxops<<" a = "<<a<<" p = "<<p<<std::endl;
                }
            }
            return T;
        }
        void from_string(const std::basic_string<DNA_encode>&str)
        {
            assert(str.size() == N_DNAs);
            for(int i=0; i<N_DNAs; i++)
                DNAs[i] = str[i];
        }
        std::basic_string<DNA_encode> to_string()const
        {
            std::basic_string<DNA_encode> str;
            for(auto&DNA:DNAs)
                str.push_back(DNA.f);
            return str;
        }
        void dump(std::ostream&os, bool moreReadable=false)const
        {
            if(moreReadable)
            {
                for(int i=0;i<N_DNAs;i++)
                    os << i%10;
                os << std::endl;
            }
            for(auto&DNA:DNAs)
                os << DNA.f;
            os << std::endl;
        }
        void random_initialize(int F_count, int T_count, DNA_encode F_T[])
        {
            for (int i=0; i<N_headers; i++)// 头部可以是任何符号
                DNAs[i] = F_T[std::rand()%(F_count+T_count)];
            
            for (int i=N_headers; i<N_DNAs; i++)// 尾部只能是终结符
            {
                if (1 == T_count)
                    DNAs[i] = F_T[F_count];
                else if(T_count > 1)
                    DNAs[i] = F_T[F_count+std::rand()%T_count];
                else
                    assert(false);
            }
            
        }
        Real eval(gene_experssion_program&GEP) const
        {
            node_ptr root = to_tree(GEP);
            return root->eval(GEP);
        }
        
        void evolve_mutate(Real probability, int F_count, int T_count, DNA_encode F_T[])
        {
            Real p = 0.0;
            for (int i=0;i<N_DNAs;i++)
            {
                p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
                if (p <= probability)
                {
                    if (i<N_headers)// 可以选择所有的符号
                        DNAs[i] = F_T[std::rand()%(F_count+T_count)];
                    else// 只能选择终结符
                        DNAs[i] = F_T[F_count+std::rand()%T_count];
                }
            }
        }
        void evolve_reverse(Real probability) // 倒串
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nStart = std::rand()%N_headers;// 倒串只作用到基因的头部
                int nLength = 0;
                switch(N_headers-nStart)
                {
                    case 0:return;
                    case 1:return;
                    case 2:nLength = 2;break;
                    case 3:nLength = std::rand()%2+2;break;
                    default:
                        nLength = std::rand()%(N_headers-nStart-2)+2;// 至少有两个DNA，才谈得上倒串
                }
                
                for (int i=nStart; i<nStart+nLength/2; i++)
                {
                    // 12 345 678
                    // nStart = 2;
                    // nLength = 3;
                    // i=2, 2+3-i+1=4
                    // i=3, 2+3-i+1=3
                    // i=4, 2+3-i+1=2
                    std::swap(DNAs[i], DNAs[nStart+nLength-(i-nStart)-1]);
                }
            }
        }

        void insert_string(int nStart, int nLength, int nInsert)
        {
            if (nStart == nInsert)// 被插入串的位置和插入的位置相同，意味着插入之后结果不变，所以就没有插入的必要了
                return;
            
            DNA bk_DNAs[N_DNAs];// 因为操作的是同一个序列，因此难免会重复，所以就先备份出来
            for(int i=0; i<N_DNAs; i++)
                bk_DNAs[i] = DNAs[i];
            
            for (int i=nInsert; i+nLength<N_headers; i++)// 插入位置的nLength长度的DNA后移，超出头部长度的直接丢弃
            {
                if (i>=nStart && i<nStart+nLength)// 被选中的插串所在位置的内容不需要向后移动
                    continue;
                DNAs[i+nLength] = bk_DNAs[i];
            }
            for (int i=nInsert; i<nInsert+nLength && i<N_headers; i++)// 将插串插入到指定位置，超出头部长度的直接丢弃
            {
                DNAs[i] = bk_DNAs[nStart+i-nInsert];
            }
        }
        
        void evolve_insert_string(Real probability)// 插串
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                // 1. 选择的插串的开始点可以是任何地方
                int nStart = std::rand()%N_DNAs;
                // 2. 获取插串的长度
                int nLength = 0;
                switch(N_DNAs-nStart-1)
                {
                    case 0: nLength = 1;break;
                    case 1: nLength = std::rand()%2 + 1; break;// 至少得有1个基因，才能执行插串
                    default:nLength = std::rand()%(N_DNAs-nStart-1)+1;// 至少得有1个基因，才能执行插串
                }
                int nInsert = std::rand()%(N_headers-1)+1; // 因为不能插到基因的第一个位置
                // 3. 执行插串
                insert_string(nStart, nLength, nInsert);
            }
        }
        
        void evolve_root_insert_string(Real probability, const std::function<bool(DNA_encode)>&is_function)// 根插串
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                // 1. 选择的插串的开始点可以是任何地方
                int nStart = std::rand()%N_DNAs;
                // 2. 向后扫描发现函数才可以继续执行根插串操作
                for(int i=nStart;i<N_headers;i++)
                {
                    if (is_function(DNAs[i].f))
                        break;
                    nStart++;
                }
                // 3. 获取插串的长度
                int nLength = 0;
                switch(N_DNAs-nStart-1)
                {
                    case 0: nLength = 1;break;
                    case 1: nLength = std::rand()%2 + 1; break;// 至少得有1个基因，才能执行插串
                    default:nLength = std::rand()%(N_DNAs-nStart-1)+1;// 至少得有1个基因，才能执行插串
                }
                int nInsert = 0; // 根插串就是将插串插到基因的第一个位置
                insert_string(nStart, nLength, nInsert);
            }
        }
        
        void evolve_single_crossover(Real probability, gene&another)
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nCrossOver = std::rand()%(N_DNAs-1)+1;//单点交叉，交叉点不能是第一个
                for (int i=nCrossOver; i<N_DNAs; i++)
                    std::swap(DNAs[i], another.DNAs[i]);
            }
        }
        
        void evolve_double_crossover(Real probability, gene&another)
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nCrossOver = std::rand()%(N_DNAs-1-1)+1;//两点交叉，交叉点不能是第一个和最后一个
                int nLength = std::rand()%(N_DNAs);
                for (int i=nCrossOver; i<nCrossOver+nLength && i<N_DNAs; i++)
                    std::swap(DNAs[i], another.DNAs[i]);
            }
        }
    };
    
    class chromosome
    {
        gene genes[N_genes];
        void random_initialize(int F_count, int T_count, DNA_encode F_T[])
        {
            for(auto&gene:genes)
                gene.random_initialize(F_count, T_count, F_T);
        }
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
    
    class node:public with_function, public std::enable_shared_from_this<node>
    {
        friend gene;
        std::weak_ptr<node> parent;
        size_t _size = 0;
        node_ptr children[N_maxops];
    public:
        size_t size_of_children() { return this->_size; }
        void add_children(node_ptr&ptr)
        {
            children[_size] = ptr;
            _size++;
            ptr->parent = this->shared_from_this();
        }
        void dump(std::ostream&os, int level)
        {
            for(int i=0; i<level; i++)
                os<<"| ";
            os << this->f <<std::endl;
            for(int i=0; i< size_of_children(); i++)
                children[i]->dump(os, level+1);
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

            return GEP.lambda_eval(this->f, static_cast<int>(this->size_of_children()), valuesOfChildren);
        }
    };
    
    typedef typename std::conditional<1 == N_genes, gene, chromosome>::type Unit;

    std::function<bool(DNA_encode)> lambda_is_terminal;
    std::function<bool(DNA_encode)> lambda_is_function;
    std::function<int(DNA_encode)> lambda_arg_count;
    std::function<Real(DNA_encode, int argc, Real argv[])> lambda_eval;// 表达式计算函数
//    std::function<DNA_encode(int index)> lambda_operator_index_to_DNA_encode;// 初始化种群时候需要的一个映射函数
    
    std::function<Real(const Unit&)> lambda_fitness;// 适应度函数
    
    
    std::function<void(int F_count, int T_count, DNA_encode F_T[])> inner_lambda_random_initialize;// 随机初始化
    std::function<std::pair<Real, Unit>()> inner_lambda_fitness_compute; // 适应度计算
    std::function<void()> inner_lambda_selection_roulette_wheel; // 轮盘赌
    std::function<void(Real probability, int F_count, int T_count, DNA_encode F_T[])> inner_lambda_evolve_mutation;// 标准变异过程
    std::function<void(Real probability)> inner_lambda_evolve_reverse;
    std::function<void(Real probability)> inner_lambda_evolve_insert_string;
    std::function<void(Real probability)> inner_lambda_evolve_root_insert_string;
    std::function<void(Real probability)> inner_lambda_evolve_single_crossover;
    std::function<void(Real probability)> inner_lambda_evolve_double_crossover;
    std::function<void(Real probability)> inner_lambda_evolve_gene_crossover;
    
    gene_experssion_program()
    {
        struct Local
        {
            Unit*units;
            Unit __units_front[N_units];
            Unit __units_back[N_units];
            Real fitnesses[N_units];
            //int F_T[N_functions+N_terminals];
            
            Local()
            {
                units = __units_back;
            }
            inline Unit*current_units()
            {
                return this->units;
            }
            inline Unit*buffer_units()
            {
                assert(nullptr != units);
                return units == __units_front? __units_back : __units_front;
            }
            inline void swap_units()
            {
                units = buffer_units();
            }
        };
        
        auto local = std::shared_ptr<Local>(new Local());
        
        inner_lambda_random_initialize = [local](int F_count, int T_count, DNA_encode F_T[])
        {
            for(int i = 0; i<N_units; i++)
            {
                local->units[i].random_initialize(F_count, T_count, F_T);
            }
        };
        
        inner_lambda_fitness_compute = [local,this]()
        {
            Real bestFitness = 0;
            int bestIndex = 0;
            for(int i = 0; i<N_units; i++)
            {
                local->fitnesses[i] = this->lambda_fitness(local->units[i]);
                if (0 == i)
                {
                    bestFitness = local->fitnesses[i];
                    bestIndex = i;
                }
                else
                {
                    if (local->fitnesses[i] > bestFitness)
                    {
                        bestFitness = local->fitnesses[i];
                        bestIndex = i;
                    }
                }
            }
            return std::make_pair(bestFitness, local->units[bestIndex]);
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
            
            
            Unit*buffer_units = local->buffer_units();
            Real p = static_cast<Real>(std::rand())/RAND_MAX;

            for (int i=0; i<N_units; i++)
            {
                for (int j=0;j<N_units;j++)
                {
                    if(p <= probabilityOfAccum[j])
                    {
                        buffer_units[i] = local->units[j];
                    }
                }
            }
            
            local->swap_units();
        };
        
        inner_lambda_evolve_mutation = [local](Real probability, int F_count, int T_count, DNA_encode F_T[])
        {
            for(int i=0; i<N_units; i++)
            {
                Unit&unit = local->units[i];
                unit.evolve_mutate(probability, F_count, T_count, F_T);
            }
        };
        
        inner_lambda_evolve_reverse = [local](Real probability)
        {
            for(int i=0; i<N_units; i++)
            {
                local->units[i].evolve_reverse(probability);
            }
        };
        inner_lambda_evolve_insert_string = [local](Real probability)
        {
            for(int i=0; i<N_units; i++)
            {
                local->units[i].evolve_insert_string(probability);
            }
        };
        inner_lambda_evolve_root_insert_string = [local,this](Real probability)
        {
            for(int i=0; i<N_units; i++)
            {
                local->units[i].evolve_root_insert_string(probability, this->lambda_is_function);
            }
        };
        
        auto crossover = [local](Real probability, const std::function<void(Real probability, Unit&a, Unit&b)>&f)
        {
            Unit*currentUnits = local->current_units();
            Unit*bufferUnits = local->buffer_units();
            for(int i=0; i<N_units; i+=2)
            {
                bufferUnits[i] = currentUnits[i];
                bufferUnits[i+1] = currentUnits[i+1];
                f(probability, bufferUnits[i], bufferUnits[i+1]);
            }
            if (1 == N_units%2)// 如果是奇数个单位，那么就需要考虑最后一个单位的处理方式
            {
                bufferUnits[N_units-1] = currentUnits[N_units-1];// 最后一个直接进入下一代
            }
            local->swap_units();
        };
        inner_lambda_evolve_single_crossover = [local,crossover](Real probability)
        {
            crossover(probability, [](Real probability, Unit&a, Unit&b){
                a.evolve_single_crossover(probability, b);
            });
        };
        inner_lambda_evolve_double_crossover = [local,crossover](Real probability)
        {
            crossover(probability, [](Real probability, Unit&a, Unit&b){
                a.evolve_double_crossover(probability, b);
            });
        };
        inner_lambda_evolve_gene_crossover = [local,crossover](Real probability)
        {
//            crossover(probability, [](Real probability, Unit&a, Unit&b){
//                a.evolve_gene_crossover(probability, b);
//            });
        };
    }
    
    struct API
    {
        struct OP_t
        {
            DNA_encode DNA;
            int argc;
            std::function<Real(int argc, Real argv[])> eval;
        };
        
        template<int N_ops>
        struct FT_t
        {
            DNA_encode F_T[N_ops];
            int F_count = 0;
            int T_count = 0;
            void import(OP_t*ops, gene_experssion_program&GEP)
            {// 按照先Function后Terminal的方式进行排序重组
                for (int i=0; i<N_ops; i++)
                {
                    if (GEP.lambda_is_function(ops[i].DNA))
                    {
                        F_T[F_count] = ops[i].DNA;
                        F_count ++;
                    }
                }
                for (int i=0; i<N_ops; i++)
                {
                    if (GEP.lambda_is_terminal(ops[i].DNA))
                    {
                        F_T[F_count+T_count] = ops[i].DNA;
                        T_count ++;
                    }
                }
            }
        };
        
    };
};

/***************************************************
g++ -std=c++11 -DTEST_WITH_MAIN_FOR_GEP_HPP=1 -x c++ %
***************************************************/

#if TEST_WITH_MAIN_FOR_GEP_HPP
#include <map>
#include <iostream>
int main(int argc, const char*argv[])
{
    //std::srand((unsigned int)time(nullptr));
    std::srand(199999);
    typedef gene_experssion_program<
    /*int N_units    */100,
    /*int N_genes    */1,
    /*int N_headers  */8,
    /*int N_maxops   */2,
    /*typename T_DNA_encode*/char>GEP_t;
    
    
    auto GEP = GEP_t();
    
    typedef GEP_t::Real Real;
    typedef GEP_t::DNA_encode DNA_encode;
    typedef GEP_t::Unit Unit;
    
    const int a = 0;
    Real variables[1] = {0.0};
    
    GEP_t::API::OP_t ops[] = {
        {'+', 2, [](int argc, Real argv[]){return argv[0]+argv[1];}},
        {'-', 2, [](int argc, Real argv[]){return argv[0]-argv[1];}},
        {'*', 2, [](int argc, Real argv[]){return argv[0]*argv[1];}},
        {'/', 2, [](int argc, Real argv[]){return argv[0]/argv[1];}},
        {'a', 0, [&variables](int argc, Real argv[]){return variables[a];}},
    };
    
    const int N_ops = sizeof(ops)/sizeof(GEP_t::API::OP_t);
    
    std::map<DNA_encode, int> I;
    for (int i=0; i<N_ops; i++) {I[ops[i].DNA] = i;}
    
    auto is_terminal = [&I,ops](DNA_encode DNA){return 0 == ops[I[DNA]].argc && 0 != ops[I[DNA]].DNA;};
    auto is_function = [&I,ops](DNA_encode DNA){return ops[I[DNA]].argc  > 0 && 0 != ops[I[DNA]].DNA;};
    

    GEP.lambda_arg_count = [&I,ops](DNA_encode DNA){return ops[I[DNA]].argc;};
    GEP.lambda_is_terminal = [&is_terminal](DNA_encode DNA){return is_terminal(DNA);};
    GEP.lambda_is_function = [&is_function](DNA_encode DNA){return is_function(DNA);};
    GEP.lambda_eval = [&I,ops](DNA_encode DNA, int argc, Real argv[]){ return ops[I[DNA]].eval(argc, argv); };

    GEP_t::API::FT_t<N_ops> ft;
    ft.import(ops, GEP);
    
    {
        GEP_t::gene g1, g2;
        {
            g1.random_initialize(ft.F_count, ft.T_count, ft.F_T);
            g2.random_initialize(ft.F_count, ft.T_count, ft.F_T);
        }
        {
            g1.dump(std::cout, true);
            g2.dump(std::cout);
        }
        {
            auto root = g1.to_tree(GEP);
            root->dump(std::cout, 0);
            variables[a] = 0.5;
            std::cout<<"g1.eval(GEP) = "<<g1.eval(GEP)<<std::endl;
        }
        {
            auto root = g2.to_tree(GEP);
            root->dump(std::cout, 0);
            variables[a] = 0.5;
            std::cout<<"g2.eval(GEP) = "<<g2.eval(GEP)<<std::endl;
        }
        {
            g1.dump(std::cout, true);
            g2.dump(std::cout, false);
            g1.evolve_single_crossover(2.0, g2);
            g1.dump(std::cout, true);
            g2.dump(std::cout, false);
            
            std::cout<< g1.to_string() <<std::endl;
            std::cout<< g2.to_string() <<std::endl;
        }
        
        {
            g1.dump(std::cout, true);
            g2.dump(std::cout, false);
            g1.evolve_double_crossover(2.0, g2);
            g1.dump(std::cout, true);
            g2.dump(std::cout, false);
        }
        
        {
            g1.dump(std::cout, true);
            g1.evolve_reverse(2.0);
            g1.dump(std::cout, true);
        }
        
        {
            g1.dump(std::cout, true);
            g1.evolve_insert_string(2.0);
            g1.dump(std::cout, true);
        }
        
        {
            g1.dump(std::cout, true);
            g1.evolve_root_insert_string(2.0, GEP.lambda_is_function);
            g1.dump(std::cout, true);
        }
    }
    
    // 开始进行完整的GEP运算
    {
        GEP.lambda_fitness = [&GEP,&variables](const Unit&unit)->Real
        {
            auto y = [](Real a){ return a*a/3 + 2*a;/* 目标方程: y = a*a/3 + 2*a */ };
            Real sum = 0.0;
            for(Real x = -10.0; x<10.0; x += 1.0)
            {
                variables[a] = x;// 为了unit执行求值，需要先赋予变量值
                Real dy = y(x) - unit.eval(GEP);
                
                sum += 100-std::abs(dy);
            }
            if (std::isnan(sum))
                sum = 0;
            return sum;
        };
        
        GEP.inner_lambda_random_initialize(ft.F_count, ft.T_count, ft.F_T);
        for (int i=0; i<200; i++)
        {
            auto best = GEP.inner_lambda_fitness_compute();
            {
                Real fitness = best.first;
                auto unit = best.second;
                std::cout<<"~~~~~~~~~~~~~~~~~~~~ i = "<<i<<", best fitness = " << fitness <<" ~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;
                unit.dump(std::cout, true);
                unit.to_tree(GEP)->dump(std::cout, 0);
            }
            GEP.inner_lambda_selection_roulette_wheel();
            GEP.inner_lambda_evolve_mutation(0.044, ft.F_count, ft.T_count, ft.F_T);
            GEP.inner_lambda_evolve_reverse(0.1);
            GEP.inner_lambda_evolve_insert_string(0.1);
            GEP.inner_lambda_evolve_root_insert_string(0.1);
            GEP.inner_lambda_evolve_single_crossover(0.4);
            GEP.inner_lambda_evolve_double_crossover(0.2);
            GEP.inner_lambda_evolve_gene_crossover(0.1);
        }
    }
	return 0;
}

#endif// TEST_WITH_MAIN_FOR_GEP_HPP
