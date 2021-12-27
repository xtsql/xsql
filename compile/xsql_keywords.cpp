/**
 * @file xsql_keywords.cpp
 * @author your name (you@domain.com)
 * @brief 实现map的初始化
 * @version 0.1
 * @date 2021-12-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "xsql_keywords.hpp"
// #include <pair>
using std::pair;
using std::string;
using cp::kid_t;

typedef std::unordered_map<std::string,kid_t> keymap;
typedef std::unordered_map<std::string,kid_t>::iterator keymap_it;

keymap key_id_map;

typedef std::pair<std::string,kid_t> mypair_t;

void cp::keywords_init(){
    if(key_id_map.size())
    return;
    key_id_map.insert(mypair_t(string("add"),kid_t::Add));
    key_id_map.insert(mypair_t(string("all"),kid_t::All));
    key_id_map.insert(mypair_t(string("alter"),kid_t::Alter));
    key_id_map.insert(mypair_t(string("and"),kid_t::And));
    key_id_map.insert(mypair_t(string("as"),kid_t::As));
    key_id_map.insert(mypair_t(string("asc"),kid_t::Asc));
    key_id_map.insert(mypair_t(string("avg"),kid_t::Avg));
    key_id_map.insert(mypair_t(string("between"),kid_t::Between));
    key_id_map.insert(mypair_t(string("bool"),kid_t::Bool));
    key_id_map.insert(mypair_t(string("boolean"),kid_t::Bool));
    key_id_map.insert(mypair_t(string("by"),kid_t::By));
    key_id_map.insert(mypair_t(string("char"),kid_t::Char));
    key_id_map.insert(mypair_t(string("count"),kid_t::Count));
    key_id_map.insert(mypair_t(string("create"),kid_t::Create));
    key_id_map.insert(mypair_t(string("database"),kid_t::Database));
    key_id_map.insert(mypair_t(string("databases"),kid_t::Databases));
    key_id_map.insert(mypair_t(string("delete"),kid_t::Delete));
    key_id_map.insert(mypair_t(string("desc"),kid_t::Desc));
    key_id_map.insert(mypair_t(string("distinct"),kid_t::Distinct));
    key_id_map.insert(mypair_t(string("double"),kid_t::Double));
    key_id_map.insert(mypair_t(string("drop"),kid_t::Drop));
    key_id_map.insert(mypair_t(string("except"),kid_t::Except));
    key_id_map.insert(mypair_t(string("exists"),kid_t::Exists));
    key_id_map.insert(mypair_t(string("false"),kid_t::False));
    key_id_map.insert(mypair_t(string("float"),kid_t::Float));
    key_id_map.insert(mypair_t(string("foreign"),kid_t::Foreign));
    key_id_map.insert(mypair_t(string("from"),kid_t::From));
    key_id_map.insert(mypair_t(string("group"),kid_t::Group));
    key_id_map.insert(mypair_t(string("having"),kid_t::Having));
    key_id_map.insert(mypair_t(string("in"),kid_t::In));
    key_id_map.insert(mypair_t(string("inner"),kid_t::Inner));
    key_id_map.insert(mypair_t(string("insert"),kid_t::Insert));
    key_id_map.insert(mypair_t(string("int"),kid_t::Int));
    key_id_map.insert(mypair_t(string("intersect"),kid_t::Intersect));
    key_id_map.insert(mypair_t(string("into"),kid_t::Into));
    key_id_map.insert(mypair_t(string("join"),kid_t::Join));
    key_id_map.insert(mypair_t(string("key"),kid_t::Key));
    key_id_map.insert(mypair_t(string("long"),kid_t::Long));
    key_id_map.insert(mypair_t(string("max"),kid_t::Max));
    key_id_map.insert(mypair_t(string("min"),kid_t::Min));
    key_id_map.insert(mypair_t(string("not"),kid_t::Not));
    key_id_map.insert(mypair_t(string("null"),kid_t::Null));
    key_id_map.insert(mypair_t(string("on"),kid_t::On));
    key_id_map.insert(mypair_t(string("or"),kid_t::Or));
    key_id_map.insert(mypair_t(string("order"),kid_t::Order));
    key_id_map.insert(mypair_t(string("outer"),kid_t::Outer));
    key_id_map.insert(mypair_t(string("primary"),kid_t::Primary));
    key_id_map.insert(mypair_t(string("real"),kid_t::Real));
    key_id_map.insert(mypair_t(string("references"),kid_t::References));
    key_id_map.insert(mypair_t(string("select"),kid_t::Select));
    key_id_map.insert(mypair_t(string("set"),kid_t::Set));
    key_id_map.insert(mypair_t(string("show"),kid_t::Show));
    key_id_map.insert(mypair_t(string("sum"),kid_t::Sum));
    key_id_map.insert(mypair_t(string("table"),kid_t::Table));
    key_id_map.insert(mypair_t(string("tables"),kid_t::Tables));
    key_id_map.insert(mypair_t(string("true"),kid_t::True));
    key_id_map.insert(mypair_t(string("union"),kid_t::Union));
    key_id_map.insert(mypair_t(string("update"),kid_t::Update));
    key_id_map.insert(mypair_t(string("use"),kid_t::Use));
    key_id_map.insert(mypair_t(string("using"),kid_t::Using));
    key_id_map.insert(mypair_t(string("values"),kid_t::Values));
    key_id_map.insert(mypair_t(string("where"),kid_t::Where));
}

kid_t cp::is_key(const char* str, const int len)
{
    if(key_id_map.empty())
        keywords_init();
    const static char diff = 'a' - 'A';
    string t(str, len);
    for (int i = 0; i < len; ++i)
        if (IN(t[i], 'A', 'Z'))
            t[i] += diff;
    keymap_it it = key_id_map.find(t);
    if (it == key_id_map.end())
        return kid_t::_not_found;
    return it->second;
}