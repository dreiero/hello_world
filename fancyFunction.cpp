#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <string.h>
#include "fancyFunction.h"
#include <typeinfo>

//#include <stdlib.h>
#include <sys/io.h>
#include <fstream>
#include <mysql.h>
#include <sstream>

// your task is to modify this class as needed
// please keep in mind to write a full documentation

void errorfunction( MYSQL *con )
        {
            std::cerr << mysql_error( con );
            mysql_close( con );
            exit( 1 );
        }

/*!
 * \class Database
 * \brief class to operate with the database
 */
template<class T>
class Database
{
    private:
        /// ??
    public:

        MYSQL *init()
        {
            MYSQL *con = mysql_init( NULL );

            if ( con == NULL )
            {
                std::cerr << mysql_error( con );
                exit( 1 );
            }

            if ( mysql_real_connect( con, "localhost", "test", NULL, "beleg", 0, NULL, 0 ) == NULL )
            {
                std::cerr << mysql_error( con );
                mysql_close( con );
                exit( 1 );
            }

            return con;
        }

        T retrieve( std::vector<T> const & vector, MYSQL *con )
            {
              std::stringstream ss1;
              ss1 << "SELECT * FROM datei WHERE x = " << vector[0];
              ss1 << " And y = ";
              ss1 << vector[1];
              std::string str = ss1.str(); 

              const char * charsql = str.c_str();

              if ( mysql_query( con, charsql ) )
              {
                  errorfunction( con );
              }

              MYSQL_RES *result = mysql_store_result( con );

              if ( result == NULL )
              {
                  errorfunction( con );
              }

              int num_fields = mysql_num_fields( result );

              MYSQL_ROW row;
              
              int i = 0;

              std::stringstream ss2;

              while( ( row = mysql_fetch_row( result ) ) )
              {
                  for (i = 0; i<num_fields; i++)
                  {}
                  ss2 << row[num_fields-1];
              }

              mysql_free_result(result);
              
              if(i == 0)
              {
                  throw std::out_of_range( "value not found in db!" );
              }
              else
              {
                  std::string value = ss2.str();
                  return std::stod( value );
              }
            }
        
        void add( std::vector<T> const & vector, T const & result,  MYSQL *con )
            {
              std::stringstream ss1;
              ss1 << "INSERT INTO datei VALUES( " << vector[0] << "," << vector [1] << "," << result << ")";
              std::string str = ss1.str();
 
              const char * charsql = str.c_str();

              if ( mysql_query( con, charsql ) )
              {
                  errorfunction( con );
              }

            }

        void dump()
            {
            
            
            };
};

int main( int argc, char ** argv )
{
    // transform strings passed to executable into vector<type>
    // any un-transformable type becomes 1.0

     auto values = transformStringsToValues<double>( argc, argv );

    // setup your database:
    Database<double> db;
    
    MYSQL *con = db.init();
    
    double result = 0;
    

    // check if arguments already in database
    // if yes return result from database
    // else calculate result
    try
    {
        result = db.retrieve( values, con );
    }
    catch( const std::out_of_range & err )
    {
        std::cerr << err.what() << std::endl;
        result = superFancyFunction( values );

        //save result to database
        db.add( values, result, con );
    }
    
    mysql_close( con );
    
    std::cout << result << std::endl;
    return 0;
}
