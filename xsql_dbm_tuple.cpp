#include "xsql_dbm.hpp"

//mark
Tuple::Tuple(const Table &table):
    type_list(table.get_type_list()),
    cell_list(type_list.size(), {false})
{
}

void Tuple::unset(const uint64_t i)
{
    cell_list[i].valid = false;
}

void Tuple::set_value(const uint64_t i, const struct Value struct_value)
{
    cell_list[i].valid = true;
    cell_list[i].struct_value.null = struct_value.null;
    if ( struct_value.null == 0 )
    {
        cell_list[i].struct_value.value = struct_value.value;
    }
}

const std::vector<Type> &Tuple::get_type_list() const
{
    return type_list;
}
