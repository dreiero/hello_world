#!/bin/bash

# This short script is for creating new databases, users and tables

dbname=""       # add name for your database
user=""         # add your username
pass=""         # add your password
table=""        # add name for your table

# if you want to add a user for server-like interaction with your database 
# (e.g. with ssh) you can replace all 'localhost' with '%'

mysql -u root -p <<query 

CREATE DATABASE $dbname;
CREATE USER '$user'@'localhost' IDENTIFIED BY '$pass';
GRANT ALL PRIVILEGES ON $dbname.* TO '$user'@'localhost';
USE $dbname;
CREATE TABLE $table(x0 double, x1 double, T double); 
exit

query

