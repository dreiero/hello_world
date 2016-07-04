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


// your task is to modify this class as needed
// please keep in mind to write a full documentation


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
        T retrieve( std::vector<T> const & vector)
            {
               // throw std::out_of_range( "value not found in db!" );
               std::fstream fout; 
               fout.open("befehle.sql", std::fstream::out | std::fstream::trunc); // opens the file befehle.sql to write, deletes old text
               fout << "USE beleg;\n";
               fout << "SELECT * FROM datei WHERE";
               
               if (vector.size() == 2)
               {
               fout << " x = " << vector[0];
               fout << " AND y = " << vector[1];
               fout << ";\nexit\n";   
               } 
               // add else/else if/switch for n input values
              
               fout.close(); // closes filestream to befehle.sql
//_______________________________________________________________________________
               system("mysql -u root -p beleg < befehle.sql > mysql-out.txt");
//_______________________________________________________________________________
               std::fstream fin;
               fin.open ("mysql-out.txt", std::fstream::in);
               std::string str;
               int i = 0;
              
               while(i<2)
               {
                  std::getline(fin, str);
                  i++;
               }
              
               i=0;
               char *ptrchange;
               char *ptrsave = strdup(str.c_str());
               ptrchange = strtok(ptrsave, "\t");
              
               while (ptrchange != NULL)
               {
                   ptrsave = ptrchange;
                   ptrchange = strtok(NULL, "\t");
                   i++;
               }
              
               fin.close();
//_______________________________________________________________________________
               if(i==0)
                   throw std::out_of_range( "value not found in db!");
               else
                  // std::string str(ptrchange);
                   return std::stod( ptrsave );
            }
        
        void add( std::vector<T> const & vector, T const & result)
            {
               std::fstream fout; 
               fout.open("befehle.sql", std::fstream::out | std::fstream::trunc); // opens the file befehle.sql to write, deletes old text
               fout << "USE beleg;\nINSERT INTO datei VALUES ('" << vector[0];
               for (int i=1; i < vector.size(); i++)
               {
                   fout << "','" << vector[i];
               }
               fout << "','" << result;
               fout << "');\nexit;\n";
               fout.close();
            
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

//for (int i=0; i<argc; i++)
//{
//    std::cout << *argv; 
//    std::cout << "\n";
//    argv ++;
//};
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
