#include <vector>

template<class T>
struct Product
{
    Product() : product_(1.0) {}
    void operator()( T const & val ) { product_ *= val; }
    T product_;
};

template<class T>
T superFancyFunction( std::vector<T> const & arguments )
{
    Product<T> product = for_each( arguments.begin(), arguments.end(), Product<T>() );
    return product.product_; 
}

inline void doTransformation( std::vector<std::string> const & arguments, std::vector<int> & values )
{
    std::transform( arguments.begin(), arguments.end(), values.begin(), 
            []( std::string const & arg )
            { 
                try
                { 
                    return std::stoi( arg ); 
                }
                catch(...)
                { 
                    return 1; 
                }
            } 
    );
}

inline void doTransformation( std::vector<std::string> const & arguments, std::vector<double> & values )
{
    std::transform( arguments.begin(), arguments.end(), values.begin(), 
            []( std::string const & arg )
            { 
                try
                { 
                    return std::stod( arg ); 
                }
                catch(...)
                { 
                    return 1.0; 
                }
            } 
    );
}

template<class T>
std::vector<T> transformStringsToValues( int argc, char ** argv )
{
    // the following two lines contain wild +1 and -1 numbers for excluding the first argument (the executable name)
    std::vector<std::string> arguments( argv + 1, argv + argc );
    std::vector<T> values(argc - 1);
    doTransformation( arguments, values );
    return values;
}

