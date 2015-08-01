#include <memory>
#include <queue>
#include <cmath>
#include <random>
#include <cassert>
#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>

template<typename ForwardIterator>
void crossover_single(typename std::iterator_traits<ForwardIterator>::difference_type nCrossOver,
                      ForwardIterator begin1, ForwardIterator end1, ForwardIterator begin2)
{
    typedef typename std::iterator_traits<ForwardIterator>::difference_type difference_type;
    ForwardIterator it1 = begin1;
    ForwardIterator it2 = begin2;
    std::advance(it1, nCrossOver);
    std::advance(it2, nCrossOver);
    for (;it1 != end1;it1++, it2++)
        std::swap(*it1, *it2);
}
template<typename ForwardIterator>
void crossover_double(typename std::iterator_traits<ForwardIterator>::difference_type nCrossOver,
                      typename std::iterator_traits<ForwardIterator>::difference_type nLength,
                      ForwardIterator begin1, ForwardIterator end1, ForwardIterator begin2)
{
    typedef typename std::iterator_traits<ForwardIterator>::difference_type difference_type;
    ForwardIterator it1 = begin1;ForwardIterator it1_end = begin1;
    ForwardIterator it2 = begin2;
    std::advance(it1, nCrossOver);std::advance(it1_end, nCrossOver+nLength);
    std::advance(it2, nCrossOver);
    for(;it1 != it1_end && it1 != end1; it1++, it2++)
        std::swap(*it1, *it2);
}

template<typename UnitForwardIterator, typename RealForwardIterator, typename RandomGenerator=std::default_random_engine>
void selection_roulette_wheel(UnitForwardIterator beginUnits, UnitForwardIterator endUnits,
                              RealForwardIterator beginFitness, RealForwardIterator beginAccum,
                              UnitForwardIterator beginNextUnits, RandomGenerator&randomGenerator)
{
    typedef typename std::iterator_traits<UnitForwardIterator>::difference_type difference_type;
    typedef typename std::iterator_traits<RealForwardIterator>::value_type Real;
    difference_type N_units = std::distance(beginUnits, endUnits);
    Real totalFitness = 0.0;
    
    RealForwardIterator endFitness = beginFitness; std::advance(endFitness, N_units);
    UnitForwardIterator endNextUnits = beginNextUnits; std::advance(endNextUnits, N_units);
//    RealForwardIterator endAccum = beginAccum; std::advance(endAccum, N_units);

    for (RealForwardIterator it=beginFitness; it!=endFitness; it++)
        totalFitness += *it;
    
    Real accumProbability = 0.0;
    for (RealForwardIterator it=beginFitness, itAccum=beginAccum; it!=endFitness; it++,itAccum++)
    {
        Real p = *it/totalFitness;
        accumProbability += p;
        *itAccum = accumProbability;
    }

    for (UnitForwardIterator i=beginNextUnits;i!=endNextUnits; /* i++ */)
    {
        Real p = static_cast<Real>(randomGenerator())/RandomGenerator::max();
        RealForwardIterator itAccum=beginAccum;
        for (UnitForwardIterator j=beginUnits;j!=endUnits;j++,itAccum++)
        {
            if(p <= *itAccum)
            {
                *i = *j;
                i++;
                break;
            }
        }
    }
}

template<typename DNA_encode=int, typename Real=double>
struct gene_experssion_program_api
{
    struct operator_t
    {
        DNA_encode DNA;
        int argc;
        std::function<Real(int argc, Real argv[])> eval;
    };
    
    template<int N_ops>
    struct functions_and_terminals_t
    {
        DNA_encode F_T[N_ops];
        int F_count = 0;
        int T_count = 0;
        
        void import(operator_t*ops, const std::function<bool(DNA_encode)>&is_function, const std::function<bool(DNA_encode)>&is_terminal)
        {// 按照先Function后Terminal的方式进行排序重组
            for (int i=0; i<N_ops; i++)
            {
                if (is_function(ops[i].DNA))
                {
                    F_T[F_count] = ops[i].DNA;
                    F_count ++;
                }
            }
            for (int i=0; i<N_ops; i++)
            {
                if (is_terminal(ops[i].DNA))
                {
                    F_T[F_count+T_count] = ops[i].DNA;
                    T_count ++;
                }
            }
        }
    };
};


template<
    int N_units,
    int N_genes,
    int N_headers,
    int N_maxargs,
    int N_ops,
    typename T_DNA_encode=int,
    typename T_Real=double
>
struct gene_experssion_program
{
    typedef T_Real Real;
    typedef T_DNA_encode DNA_encode;
    class node;
    typedef std::shared_ptr<node> node_ptr;
    typedef typename gene_experssion_program_api<DNA_encode,Real>::template functions_and_terminals_t<N_ops> functions_and_terminals_t;
    
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
    class chromosome;
    class gene
    {
        friend chromosome;
        enum
        {
            N_tails = N_headers*(N_maxargs-1)+1,
            N_DNAs = N_headers+N_tails,
        };

        DNA DNAs[N_DNAs];
    public:
        gene(){}
        gene(const std::basic_string<DNA_encode>&str)
        {
            from_string(str);
        }
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
                assert(a <= N_maxargs);
                for(int i=0;i<a;i++)
                {
                    if (p >= N_DNAs)
                    {
                        this->dump(std::cout, true);
                        T->dump(std::cout, 0);
                    }
                    assert(p < N_DNAs);
                    auto m = node_ptr(new node(K[p]));
                    n->add_children(m);
                    p++;
                    Q.push(m);
                    if (p > N_DNAs)
                    {
                        std::cout<<"Q.size() = "<<Q.size()<<" N_maxargs = "<<N_maxargs<<" a = "<<a<<" p = "<<p<<std::endl;
                    }
                }
            }
            return T;
        }
        void dump_tree(std::ostream&os, gene_experssion_program&GEP) const
        {
            auto root = to_tree(GEP);
            root->dump(os, 0);
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
        std::basic_string<DNA_encode> header_string()const
        {
            std::basic_ostringstream<DNA_encode> oss;
            for(int i=0;i<N_DNAs;i++)
                oss << i%10;
            return oss.str();
        }
        void dump(std::ostream&os, bool moreReadable=false)const
        {
            if(moreReadable)
            {
                os << header_string() << std::endl;
            }
            os << to_string() << std::endl;
        }
        
        void random_initialize(const functions_and_terminals_t&ft)
        {
            const DNA_encode *F_T = ft.F_T;
            int F_count = ft.F_count;
            int T_count = ft.T_count;
            
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

        void evolve_mutate(Real probability, const functions_and_terminals_t&ft)
        {
            const DNA_encode *F_T = ft.F_T;
            int F_count = ft.F_count;
            int T_count = ft.T_count;
            
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
        void reverse(int nStart, int nLength)
        {
            assert(nStart >= 0);
            assert(nStart < N_headers);// 倒串只作用到基因的头部
            assert(nLength >=2 );// 至少有两个才谈得倒串
            assert(nStart+nLength-1 < N_headers); // 倒串不可以超出头部范围
            for (int i=nStart; i<nStart+nLength/2; i++)
            {
                std::swap(DNAs[i], DNAs[nStart+nLength-(i-nStart)-1]);
            }
        }
        void evolve_reverse(Real probability) // 倒串
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nStart = std::rand()%N_headers;// 倒串只作用到基因的头部
                int nLength = 0;
                switch(N_headers-nStart-2)
                {
                    case -1:return;break;
                    case 0:nLength = 2;break;
                    case 1:nLength = std::rand()%2+2;break;
                    default:
                        nLength = std::rand()%(N_headers-nStart-2)+2;// 至少有两个DNA，才谈得上倒串
                }
                
                reverse(nStart, nLength);
            }
        }

        void insert_string(int nStart, int nLength, int nInsert)
        {
            assert(nInsert>=0);
            assert(nInsert<N_headers);
            if (nInsert >= nStart && nInsert<nStart+nLength)// 被插入串的位置在插串中间位置或者重合，意味着插入之后结果不变或者没法进行操作，所以就没有插入的必要了
                return;
            
            DNA bk_DNAs[N_DNAs];// 因为操作的是同一个序列，因此难免会重复，所以就先备份出来
            for(int i=0; i<N_DNAs; i++)
                bk_DNAs[i] = DNAs[i];
            
            if (nInsert < nStart)
            {
                for (int i=nInsert; i+nLength<N_headers; i++)// 被移动到的位置不可以超过头部位置
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
            else if(nInsert > nStart+nLength)
            {
                if (nInsert+nLength<=N_headers)
                {
                    for (int i=nStart;i<nStart+nLength && i<N_headers;i++)
                        DNAs[i] = bk_DNAs[nStart+i-nInsert];
                }
            }
            
//            for (int i=nInsert; i<N_headers; i++)// 插入位置的nLength长度的DNA后移，超出头部长度的直接丢弃
//            {
//                if (i>=nStart && i<nStart+nLength)// 被选中的插串所在位置的内容不需要向后移动
//                    continue;
//                DNAs[i+nLength] = bk_DNAs[i];
//            }
//            for (int i=nInsert; i<nInsert+nLength && i<N_headers; i++)// 将插串插入到指定位置，超出头部长度的直接丢弃
//            {
//                DNAs[i] = bk_DNAs[nStart+i-nInsert];
//            }
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
        
        void single_crossover(int nCrossOver, gene&another)
        {
            crossover_single(nCrossOver, DNAs, DNAs+N_DNAs, another.DNAs);
        }
        void evolve_single_crossover(Real probability, gene&another)
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nCrossOver = std::rand()%(N_DNAs-1)+1;//单点交叉，交叉点不能是第一个
                single_crossover(nCrossOver, another);
            }
        }
        void double_crossover(int nCrossOver, int nLength, gene&another)
        {
            crossover_double(nCrossOver, nLength, DNAs, DNAs+N_DNAs, another.DNAs);
        }
        void evolve_double_crossover(Real probability, gene&another)
        {
            Real p = static_cast<Real>(std::rand())/static_cast<Real>(RAND_MAX);
            if (p <= probability)
            {
                int nCrossOver = std::rand()%(N_DNAs-1-1)+1;//两点交叉，交叉点不能是第一个和最后一个
                int nLength = std::rand()%(N_DNAs);
                double_crossover(nCrossOver, nLength, another);
            }
        }
    };
    
    class chromosome
    {
        gene genes[N_genes];
    public:
        void dump_tree(std::ostream&os, gene_experssion_program&GEP) const
        {
            for (auto&gene:genes)
            {
                auto root = gene.to_tree(GEP);
                root->dump(os, 0);
            }
        }
        void dump(std::ostream&os, bool moreReadable=false)const
        {
            if(moreReadable)
            {
                for (auto&gene:genes)
                    os << gene.header_string();
                os << std::endl;
            }
            for (auto&gene:genes)
                os << gene.to_string();
            os << std::endl;
        }
        void random_initialize(functions_and_terminals_t&ft)
        {
            for(auto&gene:genes)
                gene.random_initialize(ft);
        }
        Real eval(gene_experssion_program&GEP) const
        {
            Real result = 0.0;
            for(auto&gene:genes)
                result += gene.eval(GEP);
            return result;
        }

        void evolve_mutate(Real probability, functions_and_terminals_t&ft)
        {
            for(auto&g:genes)
                g.evolve_mutate(probability, ft);
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
        void evolve_root_insert_string(Real probability, const std::function<bool(DNA_encode)>&is_function)// 根插串
        {
            for(auto&g:genes)
                g.evolve_root_insert_string(probability, is_function);
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
                int nCrossOver = std::rand()%N_genes;
                crossover_single(nCrossOver, genes, genes+N_genes, another.genes);
                //std::swap(genes[nCrossOver], another.genes[nCrossOver]);
            }
        }
    };
    
    class node:public with_function, public std::enable_shared_from_this<node>
    {
        friend gene;
        std::weak_ptr<node> parent;
        size_t _size = 0;
        node_ptr children[N_maxargs];
    public:
        size_t size_of_children_recursive()
        {
            size_t nSize = size_of_children();
            for (size_t i=0; i<size_of_children(); i++)
                nSize += children[i]->size_of_children_recursive();
            return nSize;
        }
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
            Real valuesOfChildren[N_maxargs];
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
    
    std::function<Real(const Unit&)> lambda_fitness;// 适应度函数
    
    std::function<void(const std::function<void(Unit&unit, Real fitness)>&)> inner_lambda_foreach_unit;// 遍历所有的
    
    std::function<void(functions_and_terminals_t&ft)> inner_lambda_random_initialize;// 随机初始化
    std::function<std::pair<Real, Unit>()> inner_lambda_fitness_compute; // 适应度计算
    std::function<void()> inner_lambda_selection_roulette_wheel; // 轮盘赌
    std::function<void(Real probability, functions_and_terminals_t&ft)> inner_lambda_evolve_mutation;// 标准变异过程
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
            std::default_random_engine random;
            Local()
            {
                units = __units_back;
                //random.seed(static_cast<unsigned int>(time(nullptr)));
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
        
        inner_lambda_foreach_unit = [local](const std::function<void(Unit&unit, Real fitness)>&f)
        {
            for (int i=0; i<N_units; i++)
                f(local->units[i], local->fitnesses[i]);
        };
        
        inner_lambda_random_initialize = [local](functions_and_terminals_t&ft)
        {
            for(int i = 0; i<N_units; i++)
            {
                local->units[i].random_initialize(ft);
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
            selection_roulette_wheel(local->units, local->units+N_units,
                                     local->fitnesses, probabilityOfAccum,
                                     local->buffer_units(), local->random);
            local->swap_units();
        };
        
        inner_lambda_evolve_mutation = [local](Real probability, functions_and_terminals_t&ft)
        {
            for(int i=0; i<N_units; i++)
            {
                Unit&unit = local->units[i];
                unit.evolve_mutate(probability, ft);
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
        
        inner_lambda_evolve_gene_crossover = [local,crossover,this](Real probability)
        {
            crossover(probability, [this](Real probability, Unit&a, Unit&b){
                this->__evolve_gene_crossover(probability, a, b);
            });
        };
    }
    inline void __evolve_gene_crossover(Real probability, chromosome&a, chromosome&b)
    {
        a.evolve_gene_crossover(probability, b);
    }
    inline void __evolve_gene_crossover(Real probability, gene&a, gene&b) { }
};

/***************************************************
g++ -std=c++11 -DTEST_WITH_MAIN_FOR_GEP_HPP=1 -x c++ %
***************************************************/

#if TEST_WITH_MAIN_FOR_GEP_HPP
#include <map>
#include <set>
#include <iostream>
int main(int argc, const char*argv[])
{
    std::srand((unsigned int)time(nullptr));
    //std::srand(199999);
    
    
    typedef double Real;
    typedef char DNA_encode;

    
    const int a = 0;
    Real variables[1] = {0.0};
    
    gene_experssion_program_api<DNA_encode,Real>::operator_t ops[] = {
        {'+', 2, [](int argc, Real argv[]){return argv[0]+argv[1];}},
        {'-', 2, [](int argc, Real argv[]){return argv[0]-argv[1];}},
        {'*', 2, [](int argc, Real argv[]){return argv[0]*argv[1];}},
        {'/', 2, [](int argc, Real argv[]){return argv[0]/argv[1];}},
        {'a', 0, [&variables](int argc, Real argv[]){return variables[a];}},
    };
    
    const int N_ops = sizeof(ops)/sizeof(gene_experssion_program_api<DNA_encode,Real>::operator_t);
    
    std::map<DNA_encode, int> I;
    for (int i=0; i<N_ops; i++) {I[ops[i].DNA] = i;}
    
    auto is_terminal = [&I,ops](DNA_encode DNA){return 0 == ops[I[DNA]].argc && 0 != ops[I[DNA]].DNA;};
    auto is_function = [&I,ops](DNA_encode DNA){return ops[I[DNA]].argc  > 0 && 0 != ops[I[DNA]].DNA;};
    gene_experssion_program_api<DNA_encode,Real>::functions_and_terminals_t<N_ops> functions_and_terminals;
    functions_and_terminals.import(ops, is_function, is_terminal);
    
    {
        typedef gene_experssion_program<
        /*int N_units          */100,
        /*int N_genes          */3,
        /*int N_headers        */8,
        /*int N_maxargs        */2,
        /*int N_ops            */N_ops,
        /*typename T_DNA_encode*/DNA_encode,
        /*typename T_Real      */Real>GEP_t;
        
        typedef GEP_t::Unit Unit;
        
        auto GEP = GEP_t();
        
        GEP.lambda_arg_count = [&I,ops](DNA_encode DNA){return ops[I[DNA]].argc;};
        GEP.lambda_is_terminal = [&is_terminal](DNA_encode DNA){return is_terminal(DNA);};
        GEP.lambda_is_function = [&is_function](DNA_encode DNA){return is_function(DNA);};
        GEP.lambda_eval = [&I,ops](DNA_encode DNA, int argc, Real argv[]){ return ops[I[DNA]].eval(argc, argv); };
        
        {
            GEP_t::gene g1, g2;
            g1.random_initialize(functions_and_terminals);
            g2.random_initialize(functions_and_terminals);
            g1.dump(std::cout, true);
            g2.dump(std::cout);
        }
        
        {
            GEP_t::gene g("*a//+a+-aaaaaaaaa");
            auto root = g.to_tree(GEP);
            root->dump(std::cout, 0);
            variables[a] = 0.5;
            std::cout<<"g.eval(GEP) = "<<g.eval(GEP)<<std::endl;
        }
        {
            {
                GEP_t::gene g1("*a//+a+-aaaaaaaaa");
                GEP_t::gene g2("+a*a-*-/aaaaaaaaa");
                assert(g1.to_string() == "*a//+a+-aaaaaaaaa");
                assert(g2.to_string() == "+a*a-*-/aaaaaaaaa");
                g1.single_crossover(1, g2);
                assert(g1.to_string() == "*a*a-*-/aaaaaaaaa");
                assert(g2.to_string() == "+a//+a+-aaaaaaaaa");
            }
            {
                GEP_t::gene g1("*a//+a+-aaaaaaaaa");
                GEP_t::gene g2("+a*a-*-/aaaaaaaaa");
                assert(g1.to_string() == "*a//+a+-aaaaaaaaa");
                assert(g2.to_string() == "+a*a-*-/aaaaaaaaa");
                g1.single_crossover(5, g2);
                assert(g1.to_string() == "*a//+*-/aaaaaaaaa");
                assert(g2.to_string() == "+a*a-a+-aaaaaaaaa");
            }
        }
        
        {
            GEP_t::gene g1("*a//+a+-aaaaaaaaa");
            GEP_t::gene g2("+a*a-*-/aaaaaaaaa");
            assert(g1.to_string() == "*a//+a+-aaaaaaaaa");
            assert(g2.to_string() == "+a*a-*-/aaaaaaaaa");
            g1.double_crossover(2, 5, g2);
            assert(g1.to_string() == "*a*a-*--aaaaaaaaa");
            assert(g2.to_string() == "+a//+a+/aaaaaaaaa");
        }
        
        {
            GEP_t::gene g("*a//+a+-aaaaaaaaa");
            assert(g.to_string() == "*a//+a+-aaaaaaaaa");
            g.reverse(3, 5);
            assert(g.to_string() == "*a/-+a+/aaaaaaaaa");
        }
        
        {
            GEP_t::gene g("*a//+a+-aaaaaaaaa");
            assert(g.to_string() == "*a//+a+-aaaaaaaaa");
            g.insert_string(3, 8, 5);
            //                       01234567890123456
            assert(g.to_string() == "*a//+a+-aaaaaaaaa");
        }
        {
            GEP_t::gene g("*a//+a+-aaaaaaaaa");
            assert(g.to_string() == "*a//+a+-aaaaaaaaa");
            g.insert_string(3, 8, 2);
            //                       01234567890123456
            assert(g.to_string() == "*a/+a+-aaaaaaaaaa");
            //                       *a/+a+-aaaaaaaaaa
        }
        {
            GEP_t::gene g("*a//+a+-aaaaaaaaa");
            assert(g.to_string() == "*a//+a+-aaaaaaaaa");
            g.insert_string(3, 2, 7);
            //                       01234567890123456
            assert(g.to_string() == "*a//+a+-aaaaaaaaa");
            //                       *a/+a+-aaaaaaaaaa
        }
        {
            GEP_t::gene g("*a//+a+-aaaaaaaaa");
            assert(g.to_string() == "*a//+a+-aaaaaaaaa");
            g.insert_string(3, 2, 6);
            //                       01234567890123456
            //assert(g.to_string() == "*a/a+-/+aaaaaaaaa");
            //                       *a/+a+-aaaaaaaaaa
        }
        
        {
            GEP_t::gene g("*a//+a+-aaaaaaaaa");
            g.dump(std::cout, true);
            g.evolve_root_insert_string(2.0, is_function);
            g.dump(std::cout, true);
        }
    }
    
    // 开始进行完整的GEP运算
    {
        typedef gene_experssion_program<
        /*int N_units          */200,
        /*int N_genes          */4,
        /*int N_headers        */8,
        /*int N_maxargs        */2,
        /*int N_ops            */N_ops,
        /*typename T_DNA_encode*/DNA_encode,
        /*typename T_Real      */Real>GEP_t;
        
        typedef GEP_t::Unit Unit;
        
        auto GEP = GEP_t();
        
        GEP.lambda_arg_count = [&I,ops](DNA_encode DNA){return ops[I[DNA]].argc;};
        GEP.lambda_is_terminal = [&is_terminal](DNA_encode DNA){return is_terminal(DNA);};
        GEP.lambda_is_function = [&is_function](DNA_encode DNA){return is_function(DNA);};
        GEP.lambda_eval = [&I,ops](DNA_encode DNA, int argc, Real argv[]){ return ops[I[DNA]].eval(argc, argv); };
        auto y = [](Real a){ return a*a/2 + 3*a - 2/a;/* 目标方程 */ };
        std::function<bool(Real)> is_find_result_successfully;
//        GEP.lambda_fitness = [&GEP,&variables,&y](const Unit&unit)->Real
//        {
//            Real sum = 0.0;
//            Real x = -10.0;
//            for(int i=0; i<10; i++)
//            {
//                x += 1.0;
//                variables[a] = x;// 为了unit执行求值，需要先赋予变量值
//                Real yx = y(x);
//                Real ex = unit.eval(GEP);
//                Real dy = std::abs(yx - ex);
//                
//                if (std::isnan(dy))
//                    dy = 0;
//                if (std::isinf(dy))
//                    dy = 0;
//                
//                const Real M = 100;
//                assert(dy <= M);
//                
//                sum += M-dy;
//            }
//            return sum;
//        };
//        is_find_result_successfully = [](Real fitness)
//        {
//            return std::abs(fitness - 100*10)<1e-6;
//        };
        
        GEP.lambda_fitness = [&GEP,&variables,&y](const Unit&unit)->Real
        {
            Real sum = 0.0;
            int nCount = 0;
            for(Real x = -10.0; x<10.0; x += 1.0/20, nCount++)
            {
                variables[a] = x;// 为了unit执行求值，需要先赋予变量值
                Real yx = y(x);
                Real ex = unit.eval(GEP);
                
                if (std::isnan(ex) || std::isinf(ex))
                    return 0.0;
                
                Real dy = yx - ex;
                sum += dy*dy;
            }
            Real E = sum/nCount;
            
            return 1000*1/(1+E);
        };
        is_find_result_successfully = [](Real fitness)
        {
            return std::abs(fitness - 1000)<1e-6;
        };
        
        GEP.inner_lambda_random_initialize(functions_and_terminals);
        Real bestFitness = 0;
        Unit bestUnit;
        for (int i=0; i<2000; i++)
        {
            auto best = GEP.inner_lambda_fitness_compute();
            {
                Real fitness = best.first;
                auto unit = best.second;
                
                auto report = [&]()
                {
                    std::cout<<"~~~~~~~~~~~~~~~~~~~~ i = "<<i<<", best fitness = " << fitness <<" ~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;
                    unit.dump(std::cout, true);
                    unit.dump_tree(std::cout, GEP);
                };
                if (0 == i)
                {
                    bestFitness = fitness;
                    bestUnit = unit;
                    report();
                }
                else
                {
                    if (fitness > bestFitness)
                    {
                        bestFitness = fitness;
                        bestUnit = unit;
                        report();
                    }
                }
                if (0 == i%10) {
                    std::cout<<"i = "<<i<<std::endl;
                }
            }
            if (is_find_result_successfully(bestFitness))
            {
                std::cout<<"结果已经找到，计算结束!"<<std::endl;
                // 下面的算法确保输出不重复的结果
                std::set<Unit*> bestUnits;
                GEP.inner_lambda_foreach_unit([bestFitness,&GEP,&bestUnits](Unit&unit, Real fitness){
                    if (std::abs(fitness - bestFitness) < 1e-5)
                    {
                        bestUnits.insert(&unit);
                    }
                });
                for (auto&unit:bestUnits)
                {
                    unit->dump(std::cout, true);
                    unit->dump_tree(std::cout, GEP);
                }
                break;// 已经找到了结果
            }
            
            
            GEP.inner_lambda_selection_roulette_wheel();
            GEP.inner_lambda_evolve_mutation(0.044, functions_and_terminals);
            GEP.inner_lambda_evolve_reverse(0.1);
//            GEP.inner_lambda_evolve_insert_string(0.1);
//            GEP.inner_lambda_evolve_root_insert_string(0.1);
            GEP.inner_lambda_evolve_single_crossover(0.4);
            GEP.inner_lambda_evolve_double_crossover(0.2);
            GEP.inner_lambda_evolve_gene_crossover(0.1);
        }
    }
	return 0;
}

#endif// TEST_WITH_MAIN_FOR_GEP_HPP
