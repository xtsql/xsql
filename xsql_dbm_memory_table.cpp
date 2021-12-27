#include "xsql_dbm.hpp"
#include "xsql_dbm_private.hpp"

#include <vector>

#define __xsql_dbm_func_name "Memory_table::Memory_table"
Memory_table::Memory_table(const std::vector<Type> &type_list):
    Table(0), type_list([&type_list]() -> std::vector<Type>
                  {
                      uint64_t column_num = type_list.size();
                      if ( column_num == 0 )
                      {
                          __xsql_dbm_print_exception("输入的type_list列数为0");
                          throw xsql_exception_args();
                      }
                      std::vector<Type> ret(column_num);
                      do
                      {
                          --column_num;
                          ret[column_num].type = type_list[column_num].type;
                          switch ( ret[column_num].type )
                          {
                              case INT:
                                  ret[column_num].size = INT_SIZE;
                                  break;
                              case LONG:
                                  ret[column_num].size = LONG_SIZE;
                                  break;
                              case FLOAT:
                                  ret[column_num].size = FLOAT_SIZE;
                                  break;
                              case DOUBLE:
                                  ret[column_num].size = DOUBLE_SIZE;
                                  break;
                              case BOOLEAN:
                                  ret[column_num].size = BOOLEAN_SIZE;
                                  break;
                              case CHAR:
                                  ret[column_num].size = type_list[column_num].size;
                                  break;
                              default:
                                  __xsql_dbm_print_exception("创建表不支持的变量类型");
                                  throw xsql_exception_args();
                          }
                      } while ( column_num != 0 );
                      return ret;
                  }()),
    column_name_list(type_list.size())
{
}
#undef __xsql_dbm_func_name

Memory_table::~Memory_table()
{
    std::vector<uint64_t> char_column;
    {
        uint64_t i=type_list.size()-1;
        char_column.reserve(i);
        while (1)
        {
            if (type_list[i].type==CHAR)
            {
                char_column.emplace_back(i);
            }
            if (i==0)
            {
                break;
            }
            --i;
        }
    }
    if ( char_column.size() != 0 )
    {
        for ( auto &tuple: content )
        {
            for ( auto &i: char_column )
            {
                free((void *)tuple[i].value);
            }
        }
    }
}

#define __xsql_dbm_func_name "Memory_table::insert"
void Memory_table::insert(const std::vector<Tuple> &tuple_list)
{
    const uint64_t column_num = type_list.size();
    for ( auto &tuple: tuple_list )
    {
        if ( tuple.cell_list.size() != column_num )
        {
            __xsql_dbm_print_exception("插入元组与表类型不符");
            throw xsql_exception_args();
        }
        content.emplace_back(column_num);
        auto &new_tuple = *content.rbegin();
        for ( uint64_t i = column_num; i != 0; )
        {
            --i;
            if ( type_list[i].type != tuple.type_list[i].type )
            {
                __xsql_dbm_print_exception("插入元组与表类型不符");
                throw xsql_exception_args();
            }
            if ( type_list[i].type == CHAR )
            {
                new_tuple[i].value = (uint64_t)malloc(type_list[i].size);
                if ((void*)new_tuple[i].value==NULL)
                {
                    __xsql_dbm_print_exception( "malloc失败，内存不足" );
                    throw xsql_exception_memory();
                }
            }
            if ( tuple.cell_list[i].valid )
            {
                new_tuple[i].null = tuple.cell_list[i].struct_value.null;
                if ( new_tuple[i].null == false )
                {
                    if ( type_list[i].type == CHAR )
                    {
                        sprintf((char *)new_tuple[i].value, "%.*s", (int)type_list[i].size, (const char *)(tuple.cell_list[i].struct_value.value));
                    }
                    else
                    {
                        new_tuple[i].value = tuple.cell_list[i].struct_value.value;
                    }
                }
            }
            else
            {
                new_tuple[i].null = true;
            }
        }
    }
}
#undef __xsql_dbm_func_name

#define __xsql_dbm_func_name "Memory_table::insert"
void Memory_table::insert(const Tuple &in)
{
    uint64_t i = type_list.size();
    if ( i != in.cell_list.size() )
    {
        __xsql_dbm_print_exception("插入元组与表类型不符");
        throw xsql_exception_args();
    }
    content.emplace_back(i);
    auto &new_tuple = *content.rbegin();
    do
    {
        --i;
        if ( type_list[i].type != in.type_list[i].type )
        {
            __xsql_dbm_print_exception("插入元组与表类型不符");
            throw xsql_exception_args();
        }
        if ( type_list[i].type == CHAR )
        {
            new_tuple[i].value = (uint64_t)malloc(type_list[i].size);
        }
        if ( in.cell_list[i].valid )
        {
            new_tuple[i].null = in.cell_list[i].struct_value.null;
            if ( new_tuple[i].null == false )
            {
                if ( type_list[i].type == CHAR )
                {
                    sprintf((char *)new_tuple[i].value, "%.*s", (int)type_list[i].size, (const char *)(in.cell_list[i].struct_value.value));
                }
                else
                {
                    new_tuple[i].value = in.cell_list[i].struct_value.value;
                }
            }
        }
        else
        {
            new_tuple[i].null = true;
        }
    } while ( i != 0 );
}
#undef __xsql_dbm_func_name

void Memory_table::set_table_name(const std::string &name)
{
    this->name=name;
}

void Memory_table::set_column_name(uint64_t i, const std::string& name)
{
    column_name_list[i]=name;
}

#define __xsql_dbm_func_name "Memory_table::set_column_name"
void Memory_table::set_column_name(const std::vector<std::string>& names)
{
    if (names.size()!=column_name_list.size())
    {
        __xsql_dbm_print_exception( "传入列名数和表列数不同" );
        throw xsql_exception_args();
    }
    column_name_list = names;
}
#undef __xsql_dbm_func_name

void Memory_table::set_i_begin()
{
    i=content.begin();
}

void Memory_table::inc_i()
{
    ++i;
}

void Memory_table::remove_i()
{
    i=content.erase(i);
}

bool Memory_table::i_is_end()
{
    return i==content.end();
}

#define __xsql_dbm_func_name "Memory_table:read_i"
void Memory_table::read_i(const uint64_t pos, struct Value *const struct_value)
{
    if ( pos > type_list.size() )
    {
        __xsql_dbm_print_exception("读取的列不存在");
        throw xsql_exception_args();
    }
    struct_value->null = (*i)[pos].null;
    if ( !struct_value->null )
    {
        if ( type_list[pos].type == CHAR )
        {
            strcpy((char *)struct_value->value, (const char *)(*i)[pos].value);
        }
        else
        {
            struct_value->value = (*i)[pos].value;
        }
    }
}
#undef __xsql_dbm_func_name

#define __xsql_dbm_func_name "Memory_table::write_i"
void Memory_table::write_i(const uint64_t pos, const struct Value &struct_value)
{
    if ( pos > type_list.size() )
    {
        __xsql_dbm_print_exception("列不存在");
        throw xsql_exception_args();
    }
    (*i)[pos].null=struct_value.null;
    if ( !(*i)[pos].null )
    {
        if ( type_list[pos].type == CHAR )
        {
            strcpy((char *)(*i)[pos].value ,(const char *)struct_value.value);
        }
        else
        {
            (*i)[pos].value= struct_value.value;
        }
    }
}
#undef __xsql_dbm_func_name

const std::vector<Type>& Memory_table::get_type_list() const
{
    return type_list;
}

const std::string &Memory_table::get_table_name()
{
    return name;
}

const std::string &Memory_table::get_column_name(uint64_t pos)
{
    return column_name_list[pos];
}

const std::vector<std::string> &Memory_table::get_column_name()
{
    return column_name_list;
}

#define __xsql_dbm_func_name "Memory_table::get_size"
uint64_t Memory_table::get_size(const uint64_t pos)
{
    if ( pos >= type_list.size() )
    {
        __xsql_dbm_print_exception("访问不存在的列");
        throw xsql_exception_args();
    }
    return type_list[pos].size;
}
#undef __xsql_dbm_func_name

#define __xsql_dbm_func_name "Memory_table::get_real_size"
uint64_t Memory_table::get_real_size(const uint64_t pos)
{
    if ( pos >= type_list.size() )
    {
        __xsql_dbm_print_exception("访问不存在的列");
        throw xsql_exception_args();
    }
    if ( type_list[pos].type == CHAR )
    {
        return type_list[pos].size + 1;
    }
    return type_list[pos].size;
}
#undef __xsql_dbm_func_name
