#include <iostream>
#include <vector>
#include <string>
#include "xsql_compile.hpp"
using namespace std;
using namespace cp;
using namespace exec;

int main()
{
	vector<sql_program_t> t;
	analysis("create database db1;create table db1.t2(a int,b long,c float,d double,e bool,f boolean,g char(20));drop database db2;drop table t1; drop table db2.t1;show tables;show databases;use db5;", t);
	return 0;
}