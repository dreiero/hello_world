#include <vector>
#include <iostream>
#include <stdexcept>
#include "fancyFunction.h"

// your task is to modify this class as needed
// please keep in mind to write a full documentation
template<class T>
class Database
{
    private:
        /// ??
    public:
        T retrieve( std::vector<T> const & ){ throw std::out_of_range( "value not found in db!" ); }
        void add( std::vector<T> const &, T const & ){ } 
        void dump(){ };
        /// ??
};

int main( int argc, char ** argv )
{
    // transform strings passed to executable into vector<type>
    // any un-transformable type becomes 1.0
    auto values = transformStringsToValues<double>( argc, argv );

    // setup your database:
    Database<double> db;

    double result = 0;

    // check if arguments already in database
    // if yes return result from database
    // else calculate result
    try
    {
        result = db.retrieve( values );
    }
    catch( const std::out_of_range & err )
    {
        std::cerr << err.what() << std::endl;
        result = superFancyFunction( values );

        //save result to database
        db.add( values, result );
    }
    
    std::cout << result << std::endl;
    return 0;
}
