#include "xsql_dbm.hpp"

#include "xsql_dbm_private.hpp"
#include "xsql_io.hpp"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

std::string Dbm::database_used;

const std::filesystem::path Dbm::data_path([]() -> std::filesystem::path
    {
        try
        {
            const std::wstring temp = (boost::dll::program_location().parent_path() / "data").wstring();
            const std::filesystem::path data_path = [&temp]() -> std::filesystem::path
            {
                if ( sizeof(wchar_t) == sizeof(char32_t) )
                {
                    return std::filesystem::path(std::u32string(temp.begin(), temp.end()));
                }
                else if ( sizeof(wchar_t) == sizeof(char16_t) )
                {
                    return std::filesystem::path(std::u16string(temp.begin(), temp.end()));
                }
                else
                {
                    fprintf(stderr, "异常：在初始化；未知的wchar_t大小！\n");
                    exit(-1);
                }
            }();
            if ( !__xsql_filesystem_exists(data_path) )
            {
                printf("第一次运行程序，新建data文件夹。。。\n");
                __xsql_filesystem_create_directory(data_path);
            }
            return data_path;
        }
        catch ( const xsql_io_exception & )
        {
            fprintf(stderr, "异常：在初始化；文件操作失败！\n");
            exit(-1);
        }
        catch ( ... )
        {
            fprintf(stderr, "异常：在初始化；\n");
            throw;
        }
    }());

#define __xsql_dbm_func_name "Dbm::use_database"
int64_t Dbm::use_database(const std::string &db)
try
{
    if ( !__xsql_filesystem_exists(data_path / db) )
    {
        __xsql_dbm_print_exception("不存在该数据库");
        return ERR_NOT_EXIST;
    }
    database_used = db;
    return 0;
}
__xsql_dbm_handle_exception_last();

const std::string &Dbm::get_useing_database()
{
    return database_used;
}

Table::Table(const uint64_t size)
try : size(size)
{
}
__xsql_dbm_catch_print(Table::Table);

int64_t Dbm::show_databases(std::vector<std::string> *names)
try
{
    std::filesystem::directory_iterator dir_itr(data_path), end_iter;
    names->clear();
    try
    {
        while ( dir_itr != end_iter )
        {
            names->emplace_back(dir_itr->path().filename().string());
            __xsql_std_filesystem_directory_iterator_increment(dir_itr);
        }
    }
    catch ( ... )
    {
        names->clear();
        throw;
    }
    return 0;
}
__xsql_dbm_handle_exception_last(Dbm::show_databases);

int64_t Dbm::create_database(const std::string &name)
try
{
    if ( name.empty() )
    {
        return ERR_ARG;
    }
    if ( __xsql_filesystem_exists(data_path / name) )
    {
        fprintf(stderr, "数据库已存在！\n");
        return ERR_ALREADY_EXIST;
    }
    try
    {
        __xsql_filesystem_create_directory(data_path / name);
    }
    __xsql_dbm_try_delete_in_helper(Dbm::create_database, data_path / name, );
    return 0;
}
__xsql_dbm_handle_exception_last(Dbm::create_database);

#define __xsql_dbm_func_name "Dbm::create_table"
int64_t Dbm::create_table(const std::string &db_i_name, const std::string &table_name, const std::vector<std::string> &column_name_list, const std::vector<Type> &type_list, const uint64_t primary_key)
try
{
    const uint64_t column_num = type_list.size();
    if ( table_name.empty() || column_num == 0 || column_name_list.size() != type_list.size() || primary_key >= type_list.size() )
    {
        return ERR_ARG;
    }
    std::string db_name;
    if ( db_i_name.empty() )
    {
        if ( database_used.empty() )
        {
            __xsql_dbm_print_exception("没有选择数据库");
            return ERR_ARG;
        }
        db_name = database_used;
    }
    else
    {
        db_name = db_i_name;
    }

    if ( !__xsql_filesystem_exists(data_path / db_name) )
    {
        fprintf(stderr, "数据库不存在！\n");
        return ERR_NOT_EXIST;
    }

    const std::filesystem::path table_path = data_path / db_name / table_name;
    if ( __xsql_filesystem_exists(table_path) )
    {
        fprintf(stderr, "表已存在！\n");
        return ERR_ALREADY_EXIST;
    }

    std::remove_const_t<decltype(type_list[0].size)> *const column_size_list =
        (std::remove_const_t<decltype(type_list[0].size)> *)
            malloc(column_num * sizeof(type_list[0].size));
    if ( column_size_list == NULL )
    {
        fprintf(stderr, "在函数Dbm::create_table()：malloc失败！\n");
        return ERR_MEMORY_SPACE;
    }
    {
        uint64_t i = column_num - 1;
        while ( 1 )
        {
            switch ( type_list[i].type )
            {
                case INT:
                    column_size_list[i] = INT_SIZE;
                    break;
                case LONG:
                    column_size_list[i] = LONG_SIZE;
                    break;
                case FLOAT:
                    column_size_list[i] = FLOAT_SIZE;
                    break;
                case DOUBLE:
                    column_size_list[i] = DOUBLE_SIZE;
                    break;
                case BOOLEAN:
                    column_size_list[i] = BOOLEAN_SIZE;
                    break;
                case CHAR:
                    column_size_list[i] = type_list[i].size;
                    break;
                default:
                    free(column_size_list);
                    return ERR_ARG;
            }
            if ( i == 0 )
            {
                break;
            }
            i--;
        }
    }

    try
    {
        __xsql_filesystem_create_directory(table_path);
        {
            FILE *fp = __xsql_fopen((table_path / "bitmap").c_str(), __auto_wstr("wb"));
            __xsql_fclose(fp);
        }
        {
            FILE *fp = __xsql_fopen((table_path / "content").c_str(), __auto_wstr("wb"));
            __xsql_fclose(fp);
        }
    }
    __xsql_dbm_try_delete_in_helper(Dbm::create_table, table_path, free(column_size_list););


    {
        void *const buf = malloc(sizeof(uint64_t) + sizeof(uint64_t) + sizeof(primary_key) + sizeof(uint64_t) + (sizeof(Type::type) + sizeof(Type::size)) * column_num);
        if ( buf == NULL )
        {
            free(column_size_list);
            fprintf(stderr, "在函>数Dbm::create_table()：malloc失败！\n");
            __xsql_dbm_try_delete_in(Dbm::create_table, table_path, );
            throw xsql_io_exception();
        }
        void *buf_i = buf;
        // size
        *(uint64_t *)buf_i = 0;
        buf_i = (char *)buf_i + sizeof(uint64_t);

        // 容量
        *(uint64_t *)buf_i = 0;
        buf_i = (char *)buf_i + sizeof(uint64_t);

        // 主码
        *(std::remove_const_t<decltype(primary_key)> *)buf_i = primary_key;
        buf_i = (char *)buf_i + sizeof(primary_key);

        // 列数
        *(uint64_t *)buf_i = column_num;
        buf_i = (char *)buf_i + sizeof(uint64_t);

        try
        {
            // 每列的信息
            for ( uint64_t i = 0; i < column_num; i++ )
            {
                *(std::remove_const_t<decltype(type_list[i].type)> *)buf_i = type_list[i].type;
                buf_i = (char *)buf_i + sizeof(type_list[i].type);
                *(std::remove_const_t<decltype(type_list[i].size)> *)buf_i = column_size_list[i];
                buf_i = (char *)buf_i + sizeof(type_list[i].size);
                FILE *fp = __xsql_fopen((table_path / ((std::string) "column" + std::to_string(i) + "_name")).c_str(), __auto_wstr("w"));
                if ( (uint64_t)fprintf(fp, "%s", column_name_list[i].c_str()) < column_name_list[i].size() )
                {
                    __xsql_fclose(fp);
                    throw xsql_io_exception();
                }
                __xsql_fclose(fp);
            }
        }
        __xsql_dbm_try_delete_in_helper(Dbm::create_table, table_path, free(buf); free(column_size_list););

        free(column_size_list);

        try
        {
            FILE* const fp = __xsql_fopen((table_path / "header").c_str(), __auto_wstr("wb"));
            try
            {
                __xsql_fwrite(buf, sizeof(uint64_t) + sizeof(uint64_t) + sizeof(primary_key) + sizeof(uint64_t) + (sizeof(Type::type) + sizeof(Type::size)) * column_num, 1, fp);
            }
            catch ( ... )
            {
                __xsql_dbm_print_exception("");
                __xsql_fclose(fp);
                throw;
            }
            __xsql_fclose(fp);
        }
        __xsql_dbm_try_delete_in_helper(Dbm::create_table, table_path, free(buf););


        free(buf);
    }
    return 0;
}
__xsql_dbm_handle_exception_last(Dbm::create_table);

#define __xsql_dbm_func_name "Dbm::drop_table"
int64_t Dbm::drop_table(const std::string &db_i_name, const std::string &table_name)
try
{
    if ( table_name.empty() )
    {
        fprintf(stderr, "异常：在Dbm::drop_table()输入参数错误！\n");
        return ERR_ARG;
    }
    std::string db_name;
    if ( db_i_name.empty() )
    {
        if ( database_used.empty() )
        {
            __xsql_dbm_print_exception("没有选择数据库");
            return ERR_ARG;
        }
        db_name = database_used;
    }
    else
    {
        db_name = db_i_name;
    }

    if ( !__xsql_filesystem_exists(data_path / db_name) )
    {
        fprintf(stderr, "异常：在Dbm::drop_table()数据库不存在！\n");
        return ERR_NOT_EXIST;
    }
    const std::filesystem::path table_path = data_path / db_name / table_name;
    if ( !__xsql_filesystem_exists(table_path) )
    {
        fprintf(stderr, "异常：在Dbm::drop_table()表不存在！\n");
        return ERR_NOT_EXIST;
    }
    if ( __xsql_filesystem_exists(table_path / u8"lock") )
    {
        fprintf(stderr, "异常：在Dbm::drop_table()资源正忙！\n");
        return ERR_BUSY;
    }
    __xsql_filesystem_remove_all(table_path);
    return 0;
}
__xsql_dbm_handle_exception_last(Dbm::drop_table);

#define __xsql_dbm_func_name "Dbm::show_tables"
int64_t Dbm::show_tables(const std::string &database_in_name, std::vector<std::string> *names)
try
{
    const std::string *database_name_temp;
    //const std::string &database_name = [](const std::string &database_in_name, const std::string &database_used, const std::filesystem::path &data_path) -> const std::string &
    {
        if ( !database_in_name.empty() )
        {
            if ( !__xsql_filesystem_exists(data_path / database_in_name) )
            {
                __xsql_dbm_print_exception("数据库不存在");
                return ERR_NOT_EXIST;
                //throw xsql_exception_not_exist();
            }
            database_name_temp = &database_in_name;
            //return database_in_name;
        }
        else
        {
            if ( database_used.empty() )
            {
                __xsql_dbm_print_exception("没有指定数据库");
                return ERR_ARG;
                //throw xsql_exception_args();
            }
            database_name_temp = &database_used;
            //return database_used;
        }
    } //(database_in_name, database_used, data_path);
    const std::string &database_name = *database_name_temp;
    std::filesystem::directory_iterator dir_itr(data_path / database_name);
    std::filesystem::directory_iterator end_iter;
    names->clear();
    try
    {
        while ( dir_itr != end_iter )
        {
            names->emplace_back(dir_itr->path().filename().string());
            __xsql_std_filesystem_directory_iterator_increment(dir_itr);
        }
    }
    catch ( ... )
    {
        names->clear();
        throw;
    }
    return 0;
}
__xsql_dbm_handle_exception_last(Dbm::drop_table);
#undef __xsql_dbm_func_name

int64_t Dbm::drop_database(const std::string &db_name)
try
{
    if ( db_name.empty() )
    {
        fprintf(stderr, "异常：在Dbm::drop_database()参数错误！\n");
        return ERR_ARG;
    }
    if ( !__xsql_filesystem_exists(data_path / db_name) )
    {
        fprintf(stderr, "异常：在Dbm::drop_database()数据库不存在！\n");
        return ERR_NOT_EXIST;
    }
    {
        std::filesystem::directory_iterator dir_itr(data_path / db_name);
        std::filesystem::directory_iterator end_iter;
        while ( dir_itr != end_iter )
        {
            if ( __xsql_filesystem_exists(dir_itr->path() / u8"lock") )
            {
                fprintf(stderr, "异常：在Dbm::drop_database()表正忙！\n");
                return ERR_BUSY;
            }
            ++dir_itr;
        }
    }
    __xsql_filesystem_remove_all(data_path / db_name);
    if ( db_name == database_used )
    {
        database_used.clear();
    }
    return 0;
}
__xsql_dbm_handle_exception_last(Dbm::drop_database);
