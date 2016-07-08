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

        MYSQL *init(int *x)
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

            if ( mysql_query( con, "SELECT COUNT(*) FROM information_schema.columns WHERE table_schema = 'beleg' AND table_name = 'datei'" ) )
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
           
            while( ( row = mysql_fetch_row( result ) ) )
              {
                   *x = atof(row[num_fields-1]);
              }
            mysql_free_result(result);
            return con;
        }

        T retrieve( std::vector<T> const & vector, MYSQL *con, int length )
            {
              std::stringstream ss1;
              int i = 0;
              ss1 << "SELECT * FROM datei WHERE " << "x0" << " = " << vector[0];
              
              for ( i = 1; i < vector.size(); i++ )
              {
                  ss1 << " AND " << "x" << i << " = " << vector[i];
              }
              for ( i = vector.size(); i < length-1; i++ )
              {
                  ss1 << " AND " << "x" << i << " = 1";
              }
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
              
              std::stringstream ss2;
              i=0;
              while( ( row = mysql_fetch_row( result ) ) )
              {
                  for (i = 0; i < num_fields; i++)
                  {}
                  ss2 << row[num_fields-1];
              }

              if( i == 0 )
              {
                  mysql_free_result(result);
                  throw std::out_of_range( "value not found in db!" );
              }
              else
              {
                   mysql_free_result(result);
                  std::string value = ss2.str();
                  return std::stod( value );
              }
            }
        
        void add( std::vector<T> const & vector, T const & result,  MYSQL *con )
            {
              std::stringstream ss1;
              int i = 0;
              ss1 << "INSERT INTO datei VALUES(" << vector[0];
              for (i = 1; i < vector.size(); i++)
              {
                  ss1 << ", " << vector[i];
              }
              ss1 << ", " << result << ")";
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
   
    int *lengthtabel, length;
    double result = 0;
    lengthtabel = & length;
    MYSQL *con = db.init( lengthtabel );
    std::stringstream ssmain;
   
    if ( values.size() < 1 )
    {
        std::cout << "not enough arguments!\n";
        return 1;
    }
    else if (length-1 < values.size())
    {
        for ( int i = 0; i <= values.size()-length; i++)
        {
            ssmain << "ALTER TABLE datei ADD x" << (length-1+i) << " DOUBLE DEFAULT 1 AFTER x" << (length-2+i) << ";"; 
        } 
        std::string str = ssmain.str();
        const char * charsql = str.c_str();
        if ( mysql_query( con, charsql ) )
        {
            errorfunction( con );
        }
    }

    // check if arguments already in database
    // if yes return result from database
    // else calculate result
    try
    {
        result = db.retrieve( values, con, length );
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
