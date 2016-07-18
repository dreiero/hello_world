#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <string.h>
#include "fancyFunction.h"
#include <typeinfo>
//
#include <sys/io.h>
#include <fstream>
#include <mysql.h>
#include <sstream>

/** @file */

/** @author Otto Dreier 
* @date 15.07.2016 */

/*!
 * \brief function to return the occuring error while working with MYSQL
 * @param[in] con MYSQL connection handle
 */
void errorfunction( MYSQL *con )
        {
            std::cerr << mysql_error( con );
            mysql_close( con );
            exit( 1 );
        }

/** @author Otto Dreier 
* @date 15.07.2016 */

/*!
 * \class Database
 * \brief class to operate with the database
 */
template<class T>
class Database
{
    private:
    public:
        
        /** \brief function that initializes the Database
         * @param[in] *x number of columns in the table
         * @return con: MYSQL connection handle
         */
        MYSQL *init(int *x)
        {
            // definition of database handle
            MYSQL *con = mysql_init( NULL );

            // error output, if definition fails 
            if ( con == NULL )
            {
                std::cerr << mysql_error( con );
                exit( 1 );
            }

            // connection to MYSQL, error output if connection fails 
            if ( mysql_real_connect( con, "localhost", "test", NULL, "beleg", 0, NULL, 0 ) == NULL )
            {
                std::cerr << mysql_error( con );
                mysql_close( con );
                exit( 1 );
            }

            // first MYSQL query; gets the number of columns in the database
            if ( mysql_query( con, "SELECT COUNT(*) FROM information_schema.columns WHERE table_schema = 'beleg' AND table_name = 'datei'" ) )
            {
                errorfunction( con );
            }

            // definition of query result handle 
            MYSQL_RES *result = mysql_store_result( con );
            
            // error output, if definition fails
            if ( result == NULL )
            {
                errorfunction( con );
            }
           
            // definition of columns in query result 
            int num_fields = mysql_num_fields( result );
           
            // definition of rows in query result 
            MYSQL_ROW row;
          
            // loops through all result rows, saves the value in the int-pointer x 
            while( ( row = mysql_fetch_row( result ) ) )
              {
                   *x = atof(row[num_fields-1]);
              }

            // frees the result handle
            mysql_free_result(result);

            // returns the connection handle
            return con;
        }

        /** \brief function that retrieves the data from the database
         * @param[in] vector vector of double values, user input
         * @param[in] con MYSQL connection handle 
         * @param[in] length number of columns in the table
         * @return value: saved T value (from database)
         */
        T retrieve( std::vector<T> const & vector, MYSQL *con, int length )
            {
              // definition of stringstream ss1 to save the MYSQL query the counting int variable and a tolerance  const double 
              std::stringstream ss1;
              int i = 0;
              const double tol = 0.001;

              //writes first input value in stringstream 
              ss1 << "SELECT * FROM datei WHERE x0 BETWEEN " << ( vector[0] - tol ) << " AND " << ( vector[0] + tol );
              
              // writes every input value and the according x-position in the stream 
              for ( i = 1; i < vector.size(); i++ )
              {
                  ss1 << " AND " << "x" << i << " BETWEEN " << ( vector[i] - tol ) << " AND " << ( vector[i] + tol );
              }

              // writes for every column without a value a 1 in the query stream
              for ( i = vector.size(); i < length-1; i++ )
              {
                  ss1 << " AND " << "x" << i << " = 1";
              }

              // creates a string with the same content as ss1, neccesary to convert stringstream to char 
              std::string str = ss1.str(); 
              
              // char with same content as string str
              const char * charsql = str.c_str();
              
              // second MYSQL query, searches the database for input values (with a given tolerance)
              if ( mysql_query( con, charsql ) )
              {
                  errorfunction( con );
              }

              // definition of query result handle 
              MYSQL_RES *result = mysql_store_result( con );

              // error output, if definition fails
              if ( result == NULL )
              {
                  errorfunction( con );
              }

              // definition of columns in query result 
              int num_fields = mysql_num_fields( result );

              // definition of rows in query result 
              MYSQL_ROW row;

              //definition of second stringstream 
              std::stringstream ss2;
              
              // setting counter to 0
              i=0;

              // loops through all result rows, counts fields and writes the last value in ss2 
              while( ( row = mysql_fetch_row( result ) ) )
              {
                  for (i = 0; i < num_fields; i++)
                  {}
                  ss2 << row[num_fields-1];
              }

              // error output if result contains no fields 
              // else the last value is passed to a string and returned
              // in both cases, the result is deleted
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

       /** \brief function that adds new data to the database
        * @param[in] vector vector of double values, user input
        * @param[in] result calculated value, from fancyFunction
        * @param[in] con MYSQL connection handle 
        * @param[in] lenght number of columns in the table
        */
        void add( std::vector<T> const & vector, T const & result,  MYSQL *con, int length )
            {
              // definition of stringstream ss1 to save the MYSQL query and the counting int variable 
              std::stringstream ss1;
              int i = 0;

              // writes MYSQL query plus first input value in stringstream
              ss1 << "INSERT INTO datei VALUES(" << vector[0];

              // writes every remaining input value in stream
              for (i = 1; i < vector.size(); i++)
              {
                  ss1 << ", " << vector[i];
              }

              // writes ones in stream for every remaining place in database, leaving space for one more value 
              for (i = vector.size(); i < length-1; i++)
              {
                  ss1 << ", 1";
              }

              // writes the calculated result in the stream 
              ss1 << ", " << result << ")";

              // creates a string with the same content as ss1, neccesary to convert stringstream to char 
              std::string str = ss1.str();
 
              // char with same content as string str
              const char * charsql = str.c_str();

              // third MYSQL query, adds input values and calculated result to the database 
              if ( mysql_query( con, charsql ) )
              {
                  errorfunction( con );
              }

            }
      /** \brief function that creates a .txt file for plotting
       * @param[in] con MYSQL connection handle
       */
        void dump( MYSQL *con )
            {
              // definition of filestream f
              std::fstream f;

              // open .txt file to write from filestream
              f.open ( "plot-daten.txt", std::fstream::out );
              
              // fourth MYSQL query, selects whole table
              if ( mysql_query( con, "SELECT * FROM datei" ) )
              {
                  errorfunction( con );
              }
              
              // definition of query result handle 
              MYSQL_RES *result = mysql_store_result( con );

              // error output, if definition fails
              if ( result == NULL )
              {
                  errorfunction( con );
              }

              // definition of columns in query result 
              int num_fields = mysql_num_fields( result );

              // definition of rows in query result 
              MYSQL_ROW row;
              
              // loops through all result rows, loops for every row trough the fields in the row, writes them in the filestream 
              while( ( row = mysql_fetch_row( result ) ) )
              {
                  for (int i = 0; i < num_fields; i++)
                  {
                      f << row[i] << "\t";
                  }
                  
                  // to seperate the rows a end-of-line character is written to the filestream
                  f << "\n";
              }
              
              // closes filestream
              f.close();

            };
};

int main( int argc, char ** argv )
{
    // transform strings passed to executable into vector<type>
    // any un-transformable type becomes 1.0
    auto values = transformStringsToValues<double>( argc, argv );

    // setup your database:
    Database<double> db;
    
    // definition of pointer and variable for the number of columns in the table
    int *lengthtable, length;
    
    // definition of the calculated or saved T value
    double result = 0;

    // lengthtable points at length
    lengthtable = & length;

    // the MYSQL connection handle is retrieved from the init function
    MYSQL *con = db.init( lengthtable );

    // definition of main stringstream
    std::stringstream ssmain;
  
    // in case the user gives no arguments, the programm will stop 
    if ( values.size() < 1 )
    {
        std::cout << "not enough arguments!\n";
        return 1;
    }

    // in case the user gives more arguments then the length of the table, the table is extended to fit all the arguments
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
  
    // the dump function is called to create a plottable .txt file
    db.dump( con );

    // the connection to MYSQL is closed
    mysql_close( con );
   
    // the resulting T value, retrieved from the database or newly calculated is written as output
    std::cout << result << std::endl;
    return 0;
}
