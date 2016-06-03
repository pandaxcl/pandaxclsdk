

template<typename T>
class kvo_property
{
private:
    T m;
public:
    kvo_property(T m) :subject(this->m=m) { }
    rxcpp::subjects::behavior<T> subject;
    T&get() { return m; }
    void set(const T&m)
    {
        if (this->m != m)
        {
            this->m = m;
            this->subject.get_subscriber().on_next(this->m);
        }
    }

};

template<typename T>
class kvo_container
{
public:
    kvo_container()
    :subject_setting(container_t())
    ,subject_insertion(container_t())
    ,subject_removal(container_t()) { }
    
    typedef std::list<T> container_t;
    rxcpp::subjects::behavior<container_t> subject_setting;
    rxcpp::subjects::behavior<container_t> subject_insertion;
    rxcpp::subjects::behavior<container_t> subject_removal;
//    rxcpp::subjects::behavior<T> subject_replacement;
    container_t&get() { return this->m; }
    void set(container_t&&m)
    {
        if (this->m != m)
        {
            this->m = m;
            subject_setting.get_subscriber().on_next(this->m);
        }
    }
    void insert(std::initializer_list<T>&&v)
    {
        this->m.insert(this->m.end(), v);
        subject_insertion.get_subscriber().on_next(v);
    }
    void remove(std::initializer_list<T>&&v)
    {
        for (auto&&x:v)
            this->m.remove(x);
        subject_removal.get_subscriber().on_next(v);
    }
private:
    container_t m;
};
