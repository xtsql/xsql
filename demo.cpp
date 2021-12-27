#include "xsql_dbm.hpp"

#include <stdio.h>
#include <string>
#include <vector>

void show_databases();
void show_tables(const std::string &database);

int main()
{
    Dbm::drop_database("temp");
    Dbm::create_database("temp");

    printf("+++++++++++++++++++>>\n");
    show_databases();
    printf("===================<<\n\n");
    Dbm::create_table("temp", "customer", {"id", "name", "email"}, {{LONG}, {CHAR, 50}, {CHAR, 50}}, 0);

    {
        Disk_table table(Disk_table::construct_helper("temp", "customer"));
        std::vector<Tuple> tuple_list(6, table);

        tuple_list[0].set_value(0, {false, 1});

        tuple_list[1].set_value(0, {false, 2});
        tuple_list[1].set_value(1, {false, (uint64_t) "li hua"});

        tuple_list[2].set_value(0, {false, 5});
        tuple_list[2].set_value(1, {false, (uint64_t) "xiao ming"});
        tuple_list[2].set_value(2, {false, (uint64_t) "123@aa.com"});

        //tuple_list[3].set_value(0, {false, 7});
        tuple_list[3].set_value(1, {false, (uint64_t) "所说的"});
        tuple_list[3].set_value(2, {false, (uint64_t) "123@aa.com"});

        //tuple_list[4].set_value(1, {false, (uint64_t)"所说的"});
        tuple_list[4].set_value(2, {false, (uint64_t) "123@aa.com"});

        table.insert(tuple_list);



        Value value0, value1, value2;
        value1.value = (uint64_t)malloc(51);
        value2.value = (uint64_t)malloc(51);
        table.set_i_begin();
        while ( !table.i_is_end() )
        {
            table.read_i(0, &value0);
            table.read_i(1, &value1);
            table.read_i(2, &value2);
            if ( value0.null )
            {
                printf("null ");
            }
            else
            {
                printf("%lld ", (int64_t)value0.value);
            }
            if ( value1.null )
            {
                printf("null ");
            }
            else
            {
                printf("%s ", (char *)value1.value);
            }
            if ( value2.null )
            {
                printf("null ");
            }
            else
            {
                printf("%s ", (char *)value2.value);
            }
            putchar('\n');
            table.inc_i();
        }
        free((void *)value1.value);
        free((void *)value2.value);
    }


    return 0;
}

void show_databases()
{
    std::vector<std::string> names;
    if ( Dbm::show_databases(&names) < 0 )
    {
        fprintf(stderr, "获取数据库失败！\n");
        return;
    }
    for ( auto &i: names )
    {
        printf("%s\n", i.c_str());
    }
}

void show_tables(const std::string &db)
{
    std::vector<std::string> names;
    if ( Dbm::show_tables(db, &names) < 0 )
    {
        fprintf(stderr, "获取表失败！\n");
        return;
    }
    for ( auto &i: names )
    {
        printf("%s\n", i.c_str());
    }
}
