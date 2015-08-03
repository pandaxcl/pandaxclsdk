// 基因算法的通用算法列举在此文件中，将会在GEP和ANN里面使用；）
// 2015年8月3日 熊春雷(pandaxcl@gmail.com)


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