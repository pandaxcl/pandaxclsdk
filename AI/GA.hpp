// 基因算法的通用算法列举在此文件中，将会在GEP和ANN里面使用；）
// 2015年8月3日 熊春雷(pandaxcl@gmail.com)


template<typename ForwardIterator>
void crossover_single(typename std::iterator_traits<ForwardIterator>::difference_type nCrossOver,
                      ForwardIterator begin1, ForwardIterator end1, ForwardIterator begin2,
                      std::function<void(typename std::iterator_traits<ForwardIterator>::reference, typename std::iterator_traits<ForwardIterator>::reference)>swap)
{
    typedef typename std::iterator_traits<ForwardIterator>::difference_type difference_type;
    ForwardIterator it1 = begin1;
    ForwardIterator it2 = begin2;
    std::advance(it1, nCrossOver);
    std::advance(it2, nCrossOver);
    for (;it1 != end1;it1++, it2++)
        swap(*it1, *it2);
}

template<typename ForwardIterator>
void crossover_single(typename std::iterator_traits<ForwardIterator>::difference_type nCrossOver,
                      ForwardIterator begin1, ForwardIterator end1, ForwardIterator begin2)
{
    typedef typename std::iterator_traits<ForwardIterator>::reference reference;
    crossover_single(nCrossOver, begin1, end1, begin2, [](reference A, reference B){ std::swap(A, B); });
}

template<typename ForwardIterator>
void crossover_double(typename std::iterator_traits<ForwardIterator>::difference_type nCrossOver,
                      typename std::iterator_traits<ForwardIterator>::difference_type nLength,
                      ForwardIterator begin1, ForwardIterator end1, ForwardIterator begin2,
                      std::function<void(typename std::iterator_traits<ForwardIterator>::reference, typename std::iterator_traits<ForwardIterator>::reference)>swap)
{
    typedef typename std::iterator_traits<ForwardIterator>::difference_type difference_type;
    ForwardIterator it1 = begin1;ForwardIterator it1_end = begin1;
    ForwardIterator it2 = begin2;
    std::advance(it1, nCrossOver);std::advance(it1_end, nCrossOver+nLength);
    std::advance(it2, nCrossOver);
    for(;it1 != it1_end && it1 != end1; it1++, it2++)
        swap(*it1, *it2);
}

template<typename ForwardIterator>
void crossover_double(typename std::iterator_traits<ForwardIterator>::difference_type nCrossOver,
                      typename std::iterator_traits<ForwardIterator>::difference_type nLength,
                      ForwardIterator begin1, ForwardIterator end1, ForwardIterator begin2)
{
    typedef typename std::iterator_traits<ForwardIterator>::reference reference;
    crossover_double(nCrossOver, nLength, begin1, end1, begin2, [](reference A, reference B){ std::swap(A, B); });
}


template<typename RealForwardIterator>
void selection_roulette_wheel_accumulate(RealForwardIterator beginFitness, RealForwardIterator endFitness, RealForwardIterator beginAccumProbility)
{
    typedef typename std::iterator_traits<RealForwardIterator>::difference_type difference_type;
    typedef typename std::iterator_traits<RealForwardIterator>::value_type Real;
    Real totalFitness = std::accumulate(beginFitness, endFitness, Real(0));
    
    Real probabilityAccum = 0.0;
    for (RealForwardIterator it=beginFitness, itAccum=beginAccumProbility; it!=endFitness; it++,itAccum++)
    {
        Real p = *it/totalFitness;
        probabilityAccum += p;
        *itAccum = probabilityAccum;
    }
}

template<typename UnitForwardIterator, typename RealForwardIterator, typename RandomGenerator=std::default_random_engine>
void selection_roulette_wheel_select(UnitForwardIterator beginUnits, UnitForwardIterator endUnits, size_t n, UnitForwardIterator beginNextUnits,
                                     RealForwardIterator beginAccumProbility, RandomGenerator&randomGenerator)
{
    typedef typename std::iterator_traits<RealForwardIterator>::value_type Real;
    UnitForwardIterator endNextUnits = beginNextUnits; std::advance(endNextUnits, n);
    for (UnitForwardIterator i=beginNextUnits;i!=endNextUnits; /* i++ */)
    {
        Real p = static_cast<Real>(randomGenerator())/RandomGenerator::max();
        RealForwardIterator itAccum=beginAccumProbility;
        for (UnitForwardIterator j=beginUnits;j!=endUnits;j++,itAccum++)
        {
            if(p <= *itAccum)
            {
                *i = *j;
                i++;// 确保选择万无一失，不会因为计算精度而造成选择上的遗漏
                break;
            }
        }
    }
}

template<typename UnitForwardIterator, typename RealForwardIterator, typename RandomGenerator=std::default_random_engine>
void selection_roulette_wheel(UnitForwardIterator beginUnits, UnitForwardIterator endUnits, UnitForwardIterator beginNextUnits,
                              RealForwardIterator beginFitness, RealForwardIterator beginAccumProbility, RandomGenerator&randomGenerator)
{
    typedef typename std::iterator_traits<UnitForwardIterator>::difference_type difference_type;
    difference_type N_units = std::distance(beginUnits, endUnits);
    RealForwardIterator endFitness = beginFitness; std::advance(endFitness, N_units);
    selection_roulette_wheel_accumulate(beginFitness, endFitness, beginAccumProbility);
    selection_roulette_wheel_select(beginUnits, endUnits, N_units, beginNextUnits, beginAccumProbility, randomGenerator);
}