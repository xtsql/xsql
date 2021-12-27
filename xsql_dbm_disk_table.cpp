#include "xsql_dbm.hpp"
#include "xsql_dbm_private.hpp"

#define __xsql_dbm_func_name "Disk_table::Disk_table_construct_helper"
struct Disk_table::struct_for_construct Disk_table::construct_helper(const std::string &db_in, const std::string &table_name)
{
    if ( table_name.empty() )
    {
        __xsql_dbm_print_exception("参数错误");
        throw xsql_exception_args();
    }
    //const std::string *db_temp;
    const std::string &db = [](const std::filesystem::path &data_path, const std::string &db_in, const std::string &database_used) -> const std::string &
    {
        if ( !db_in.empty() )
        {
            if ( !__xsql_filesystem_exists(data_path / db_in) )
            {
                __xsql_dbm_print_exception("数据库不存在");
                //return ERR_NOT_EXIST;
                throw xsql_exception_not_exist();
            }
            //database_name_temp = &db_in;
            return db_in;
        }
        else
        {
            if ( database_used.empty() )
            {
                __xsql_dbm_print_exception("没有指定数据库");
                //return ERR_ARG;
                throw xsql_exception_args();
            }
            //database_name_temp = &database_used;
            return database_used;
        }
    }(Dbm::data_path, db_in, Dbm::database_used);
    //const std::string &database_name = *database_name_temp;
    Disk_table::struct_for_construct ret = {db, table_name, Dbm::data_path / db / table_name};
    if ( !__xsql_filesystem_exists(ret.table_path) )
    {
        __xsql_dbm_print_exception("表不存在");
        throw xsql_exception_not_exist();
    }
    if ( __xsql_filesystem_exists(ret.table_path / u8"lock") )
    {
        __xsql_dbm_print_exception("表正忙(该表已经创建了对象)！");
        throw xsql_exception_busy();
    }
    {
        FILE *fp = __xsql_fopen((ret.table_path / u8"header").c_str(), __auto_wstr("rb"));
        uint64_t column_num;
        try
        {
            __xsql_fread(&ret.size, sizeof(size), 1, fp);
            __xsql_fread(&ret.reserve, sizeof(ret.reserve), 1, fp);
            __xsql_fread(&ret.primary_key, sizeof(primary_key), 1, fp);
            __xsql_fread(&column_num, sizeof(column_num), 1, fp);
            if ( column_num == 0 )
            {
                throw xsql_exception_database();
            }
            ret.type_list.resize(column_num);
            ret.offset_size_list.resize(column_num + 1);
            {
                uint64_t sum = 0;
                uint64_t i = 0;
                for ( ; i < column_num; i++ )
                {
                    __xsql_fread(&ret.type_list[i].type, sizeof(Type::type), 1, fp);
                    __xsql_fread(&ret.type_list[i].size, sizeof(Type::size), 1, fp);
                    ret.offset_size_list[i].size = ret.type_list[i].size;
                    if ( ret.type_list[i].type == CHAR )
                    {
                        ++ret.offset_size_list[i].size;
                    }
                    ret.offset_size_list[i].offset = sum;
                    sum += ret.offset_size_list[i].size + 1;
                }
                ret.tuple_size = ret.offset_size_list[i].offset = sum;
            }
        }
        catch ( ... )
        {
            __xsql_dbm_print_exception("文件IO错误");
            __xsql_fclose(fp);
            throw;
        }
        __xsql_fclose(fp);
        get_column_name_list(ret.table_path, column_num, &ret.column_name_list);
    }
    ret.bit_fp = __xsql_fopen((ret.table_path / u8"bitmap").c_str(), __auto_wstr("rb+"));
    try
    {
        ret.main_fp = __xsql_fopen((ret.table_path / u8"content").c_str(), __auto_wstr("rb+"));
    }
    catch ( ... )
    {
        __xsql_fclose(ret.bit_fp);
        throw;
    }
    return ret;
}

const std::vector<Type>& Disk_table::get_type_list() const
{
    return type_list;
}

//mark
void Disk_table::get_column_name_list(const std::filesystem::path &table_path, const uint64_t num, std::vector<std::string> *column_name_list)
{
    column_name_list->clear();
    for (uint64_t i=0; i<num; ++i)
    {
        FILE *fp = __xsql_fopen((table_path / ((std::string) "column" + std::to_string(i) + "_name")).c_str(), __auto_wstr("r"));
        // mark
        __xsql_fseek(fp, 0, SEEK_END);
        long int size = __xsql_ftell(fp);
        rewind(fp);
        if ( size < 10240 )
        {
            char temp[10240];
            __xsql_fgets(temp, sizeof(temp), fp);
            column_name_list->emplace_back(temp);
        }
        else
        {
            column_name_list->emplace_back();
            (*column_name_list)[i].reserve(size);
            while ( size > 0 )
            {
                char temp[10241];
                __xsql_fgets(temp, sizeof(temp), fp);
                (*column_name_list)[i] += temp;
                size -= 10240;
            }
        }
        __xsql_fclose(fp);
    }
}


Disk_table::Disk_table(const struct struct_for_construct &x)
try : Table(x.size), database_name(std::move(x.database_name)), name(std::move(x.name)), offset_size_list(std::move(x.offset_size_list)), type_list(std::move(x.type_list)), column_name_list(std::move(x.column_name_list)), primary_key(x.primary_key), tuple_size(x.tuple_size), reserve(x.reserve), bit_fp(x.bit_fp), main_fp(x.main_fp)
{
    {
        FILE *fp;
        try
        {
            fp = __xsql_fopen((x.table_path / u8"lock").c_str(), __auto_wstr("wb"));
        }
        catch ( ... )
        {
            __xsql_filesystem_remove_all(x.table_path / u8"lock");
            throw;
        }
        __xsql_fclose(fp);
    }
    f_pos = 0;
}
catch ( ... )
{
    __xsql_fclose(x.main_fp);
    __xsql_fclose(x.bit_fp);
    throw;
}

void Disk_table::set_i_begin()
{
    f_pos = 0;
    rewind(bit_fp);
    rewind(main_fp);
    while ( 1 )
    {
        if ( f_pos == reserve )
        {
            return;
        }
        uint8_t temp;
        __xsql_fread(&temp, 1, 1, bit_fp);
        if ( temp == 1 )
        {
            __xsql_fseek(bit_fp, -1, SEEK_CUR);
            return;
        }
        f_pos++;
    }
}
// mark
void Disk_table::inc_i()
{
    __xsql_fseek(bit_fp, 1, SEEK_CUR);
    f_pos++;
    while ( 1 )
    {
        if ( f_pos == reserve )
        {
            return;
        }
        uint8_t temp;
        __xsql_fread(&temp, 1, 1, bit_fp);
        if ( temp == 1 )
        {
            __xsql_fseek(bit_fp, -1, SEEK_CUR);
            break;
        }
        f_pos++;
    }
}

bool Disk_table::find_next_empty()
{
    while ( f_pos != reserve )
    {
        uint8_t bit;
        __xsql_fread(&bit, 1, 1, bit_fp);
        if ( bit == 0 )
        {
            __xsql_fseek(bit_fp, -1, SEEK_CUR);
            return true;
        }
        ++f_pos;
    }
    return false;
}

void Disk_table::insert_cur(const Tuple &tuple)
{
    __xsql_fseek(main_fp, f_pos * tuple_size, SEEK_SET);
    for ( uint64_t i = 0; i < type_list.size(); i++ )
    {
        if ( tuple.cell_list[i].valid )
        {
            __xsql_fwrite(&tuple.cell_list[i].struct_value.null, 1, 1, main_fp);
            if ( tuple.cell_list[i].struct_value.null == 1 )
            {
                __xsql_fseek(main_fp, offset_size_list[i].size, SEEK_CUR);
            }
            else
            {
                if ( type_list[i].type == CHAR )
                {
                    int ret = fprintf(main_fp, "%.*s%c", (int)(offset_size_list[i].size - 1), (const char *)tuple.cell_list[i].struct_value.value, '\0');
                    __xsql_fseek(main_fp, offset_size_list[i].size - ret, SEEK_CUR);
                }
                else
                {
                    __xsql_fwrite(&tuple.cell_list[i].struct_value.value, offset_size_list[i].size, 1, main_fp);
                }
            }
        }
        else
        {
            uint8_t null = 1;
            __xsql_fwrite(&null, 1, 1, main_fp);
            __xsql_fseek(main_fp, offset_size_list[i].size, SEEK_CUR);
        }
    }
    {
        uint8_t bit = 1;
        __xsql_fwrite(&bit, 1, 1, bit_fp);
    }
    ++f_pos;
}

void Disk_table::insert(const std::vector<Tuple> &tuple_list)
{
    if ( tuple_list.size() == 0 )
    {
        return;
    }
    rewind(bit_fp);
    f_pos = 0;
    size += tuple_list.size();
    auto i = tuple_list.cbegin();
    while ( find_next_empty() )
    {
        insert_cur(*i);
        ++i;
        if ( i == tuple_list.cend() )
        {
            return;
        }
    }
    while ( i != tuple_list.cend() )
    {
        insert_cur(*i);
        ++i;
        ++reserve;
    }
}

void Disk_table::insert(const Tuple &tuple)
{
    rewind(bit_fp);
    f_pos = 0;
    if ( !find_next_empty() )
    {
        ++reserve;
    }
    insert_cur(tuple);
    ++size;
    return;
}

Disk_table::~Disk_table()
{
    __xsql_filesystem_remove_all(Dbm::data_path / database_name / name / u8"lock");
    FILE *fp = __xsql_fopen((Dbm::data_path / database_name / name / "header").c_str(), __auto_wstr("rb+"));
    __xsql_fwrite(&size, sizeof(size), 1, fp);
    __xsql_fwrite(&reserve, sizeof(reserve), 1, fp);
    __xsql_fclose(fp);
    __xsql_fclose(main_fp);
    __xsql_fclose(bit_fp);
}

void Disk_table::read_i(const uint64_t i, Value *const value)
{
    __xsql_fseek(main_fp, f_pos * tuple_size + offset_size_list[i].offset, SEEK_SET);
    __xsql_fread(&value->null, 1, 1, main_fp);
    if ( value->null == 0 )
    {
        if ( type_list[i].type == CHAR )
        {
            __xsql_fgets((char *)value->value, offset_size_list[i].size, main_fp);
        }
        else
        {
            __xsql_fread(&value->value, offset_size_list[i].size, 1, main_fp);
        }
    }
}

void Disk_table::write_i(const uint64_t i, const Value &struct_value)
{
    __xsql_fseek(main_fp, f_pos * tuple_size + offset_size_list[i].offset, SEEK_SET);
    __xsql_fwrite(&struct_value.null, 1, 1, main_fp);
    if ( struct_value.null == 0 )
    {
        if ( type_list[i].type == CHAR )
        {
            // mark
            fprintf(main_fp, "%.*s%c", (int)(offset_size_list[i].size - 1), (const char *)struct_value.value, '\0');
        }
        else
        {
            __xsql_fwrite(&struct_value.value, offset_size_list[i].size, 1, main_fp);
        }
    }
}

void Disk_table::remove_i()
{
    {
        uint8_t bit = 0;
        __xsql_fwrite(&bit, 1, 1, bit_fp);
    }
    ++f_pos;
    while ( 1 )
    {
        if ( f_pos == reserve )
        {
            return;
        }
        uint8_t temp;
        __xsql_fread(&temp, 1, 1, bit_fp);
        if ( temp == 1 )
        {
            __xsql_fseek(bit_fp, -1, SEEK_CUR);
            break;
        }
        ++f_pos;
    }
}
const std::string &Disk_table::get_table_name()
{
    return name;
}
const std::string &Disk_table::get_database_name()
{
    return database_name;
}
const std::string &Disk_table::get_column_name(uint64_t pos)
{
    return column_name_list[pos];
}
const std::vector<std::string> &Disk_table::get_column_name()
{
    return column_name_list;
}

#define __xsql_dbm_func_name "Disk_table::get_size"
uint64_t Disk_table::get_size(const uint64_t pos)
{
    if ( pos >= type_list.size() )
    {
        __xsql_dbm_print_exception("访问不存在的列");
        throw xsql_exception_args();
    }
    if ( type_list[pos].type == CHAR )
    {
        return offset_size_list[pos].size - 1;
    }
    return offset_size_list[pos].size;
}
#undef __xsql_dbm_func_name

#define __xsql_dbm_func_name "Disk_table::get_real_size"
uint64_t Disk_table::get_real_size(const uint64_t pos)
{
    if ( pos >= type_list.size() )
    {
        __xsql_dbm_print_exception("访问不存在的列");
        throw xsql_exception_args();
    }
    return offset_size_list[pos].size;
}
#undef __xsql_dbm_func_name
