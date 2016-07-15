#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <string.h>
#include "fancyFunction.h"
#include <typeinfo>

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
        
        /// \brief function that initializes the Database
        MYSQL *init(int *x)
        {
            /// definition of database handle
            MYSQL *con = mysql_init( NULL );

            /// error output, if definition fails 
            if ( con == NULL )
            {
                std::cerr << mysql_error( con );
                exit( 1 );
            }

            /// connection to MYSQL, error output if connection fails 
            if ( mysql_real_connect( con, "localhost", "test", NULL, "beleg", 0, NULL, 0 ) == NULL )
            {
                std::cerr << mysql_error( con );
                mysql_close( con );
                exit( 1 );
            }

            /// first MYSQL query; gets the number of columns in the database
            if ( mysql_query( con, "SELECT COUNT(*) FROM information_schema.columns WHERE table_schema = 'beleg' AND table_name = 'datei'" ) )
            {
                errorfunction( con );
            }

            /// definition of query result handle 
            MYSQL_RES *result = mysql_store_result( con );
            
            /// error output, if definition fails
            if ( result == NULL )
            {
                errorfunction( con );
            }
           
            /// definition of columns in query result 
            int num_fields = mysql_num_fields( result );
           
            /// definition of rows in query result 
            MYSQL_ROW row;
          
            /// loops through all result rows, saves the value in the int-pointer x 
            while( ( row = mysql_fetch_row( result ) ) )
              {
                   *x = atof(row[num_fields-1]);
              }

            /// frees the result handle
            mysql_free_result(result);

            /// returns the connection handle
            return con;
        }

        /// \brief function that retrieves the data from the database
        T retrieve( std::vector<T> const & vector, MYSQL *con, int length )
            {
              /// definition of stringstream ss1 to save the MYSQL query and the counting int variable  
              std::stringstream ss1;
              int i = 0;
              double tol = 0.001;
             
              ss1 << "SELECT * FROM datei WHERE x0 BETWEEN " << ( vector[0] - tol ) << " AND " << ( vector[0] + tol );
              
              /// writes every input value and the according x-position in the stream 
              for ( i = 1; i < vector.size(); i++ )
              {
                  ss1 << " AND " << "x" << i << " BETWEEN " << ( vector[i] - tol ) << " AND " << ( vector[i] + tol );
              }

              /// writes for every column without a value a 1 in the query stream
              for ( i = vector.size(); i < length-1; i++ )
              {
                  ss1 << " AND " << "x" << i << " = 1";
              }

              /// creates a string with the same content as ss1 
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
        
        void add( std::vector<T> const & vector, T const & result,  MYSQL *con, int length )
            {
              std::stringstream ss1;
              int i = 0;
              ss1 << "INSERT INTO datei VALUES(" << vector[0];
              for (i = 1; i < vector.size(); i++)
              {
                  ss1 << ", " << vector[i];
              }

              for (i = vector.size(); i < length-1; i++)
              {
                  ss1 << ", 1";
              }

              ss1 << ", " << result << ")";
              std::string str = ss1.str();
 
              const char * charsql = str.c_str();

              if ( mysql_query( con, charsql ) )
              {
                  errorfunction( con );
              }

            }

        void dump( MYSQL *con )
            {

 //             if ( mysql_query( con, "SELECT * FROM datei" ) )
  //            {
   //               errorfunction( con );
    //          }
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
        db.add( values, result, con, length );
    }
   
    db.dump( con );

    mysql_close( con );
    
    std::cout << result << std::endl;
    return 0;
}
