#pragma once
#include "xsql_error.hpp"
#include "xsql_io.hpp"

#include <filesystem>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

enum Data_type
{
    // 物理大小：5字节
    // 0字节代表is null，1-4字节为 int32_t
    INT,

#define INT_SIZE 4

    // 物理大小：9字节
    // 0字节代表is null，1-8字节为 int64_t
    LONG,

#define LONG_SIZE 8

    // 物理大小：5字节
    // 0字节代表is null，1-4字节为 float
    FLOAT,

#define FLOAT_SIZE 4

    // 物理大小：9字节
    // 0字节代表is null，1-8字节为 double
    DOUBLE,

#define DOUBLE_SIZE 8

    // 物理大小：对于 CHAR(n) ： 2+n字节
    // 0字节代表is null，1-(n+1)字节存放字符串，结尾包含'\0'
    CHAR,

    // 大小：2字节
    // 0字节代表is null，1字节代表真假
    BOOLEAN,

#define BOOLEAN_SIZE 1

    // 暂时删除
    // 大小：8字节
    // 0字节代表is null
    // 1-2字节：uint16_t 代表年份；3字节：uint8_t 代表月份；4字节：uint8_t 代表日
    // 5字节：uint8_t 代表小时；6字节：uint8_t 代表分钟；7字节：uint8_t 代表秒
    //DATETIME
};

// 代表一个Cell的类型，由这个结构体组成一个列表/数组我们称之为类型列表
struct Type
{
    Data_type type;
    // 此参数目前只在type为CHAR的时候有效，代表CHAR(n)的n
    uint64_t size;
};

// 可以储存表中一个Cell的值的最小数据结构
struct Value
{
    // is null
    bool null;
    // 对于CHAR这个值是 char *
    // 对于其他，这个值就是他的值
    uint64_t value;
};

class Table;
class Disk_table;
class Memory_table;

class Dbm
{
public:
    // 创建&&删除数据库
    // 返回值，>=0表示成功，为成功码，<0表示失败，为失败码，之后的函数相同
    static int64_t create_database(const std::string &name);
    static int64_t drop_database(const std::string &name);
    static int64_t show_databases(std::vector<std::string> *names);
    static int64_t use_database(const std::string &name);
    static const std::string &get_useing_database();

    // 如果传入数据库名字为空，则默认使用using_database
    static int64_t create_table(const std::string &database_name, const std::string &table_name, const std::vector<std::string> &column_name_list, const std::vector<Type> &type_list, const uint64_t primary_key);
    // 如果传入数据库名字为空，则默认使用using_database
    static int64_t drop_table(const std::string &database_name, const std::string &table_name);
    // 如果传入数据库名字为空，则默认使用using_database
    static int64_t show_tables(const std::string &database_name, std::vector<std::string> *names);

private:
    Dbm();
    const static std::filesystem::path data_path;
    static std::string database_used;
    friend class Disk_table;
};

class Tuple;


class Table
{
public:
    virtual void insert(const std::vector<Tuple> &x) = 0;
    virtual void insert(const Tuple &x) = 0;
    uint64_t get_size()
    {
        return size;
    }
    virtual void set_i_begin() = 0;
    virtual void inc_i() = 0;
    // 对于CHAR型数据需要自行malloc到value.value上
    virtual void read_i(const uint64_t pos_in_tuple, Value *const value) = 0;
    virtual void write_i(const uint64_t pos_in_tuple, const Value &value) = 0;
    virtual void remove_i() = 0;
    virtual bool i_is_end() = 0;
    virtual const std::string &get_table_name() = 0;
    virtual const std::string &get_column_name(uint64_t pos) = 0;
    virtual const std::vector<std::string> &get_column_name() = 0;
    virtual uint64_t get_size(const uint64_t pos) = 0;
    virtual uint64_t get_real_size(const uint64_t pos) = 0;
    virtual const std::vector<Type> &get_type_list() const = 0;
    virtual ~Table()
    {
    }
protected:
    Table(const uint64_t size);
    uint64_t size;
private:
    Table(const Table&);
};

class Memory_table: public Table
{
public:
    Memory_table(const std::vector<Type> &type_list);
    void insert(const std::vector<Tuple> &x);
    void insert(const Tuple &x);
    void set_table_name(const std::string &name);
    void set_column_name(const std::vector<std::string> &column_name_list);
    void set_column_name(uint64_t pos, const std::string &column_name);
    void set_i_begin();
    void inc_i();
    void remove_i();
    bool i_is_end();
    // 对于CHAR型数据需要自行malloc到value.value上
    void read_i(const uint64_t pos_in_tuple, Value *const value);
    void write_i(const uint64_t pos_in_tuple, const Value &value);
    const std::string &get_table_name();
    const std::string &get_column_name(uint64_t pos);
    const std::vector<std::string> &get_column_name();
    uint64_t get_size(const uint64_t pos);
    uint64_t get_real_size(const uint64_t pos);
    const std::vector<Type> &get_type_list() const;
    ~Memory_table();
private:
    const std::vector<Type> type_list;
    std::string name;
    std::vector<std::string> column_name_list;
    std::vector<std::vector<struct Value>> content;
    std::vector<std::vector<struct Value>>::iterator i;
private:
    Memory_table(const Memory_table&);
};

class Disk_table: public Table
{
private:
    struct offset_size_t
    {
        uint64_t offset;
        uint64_t size;
    };
    struct struct_for_construct
    {
        const std::string &database_name;
        const std::string &name;
        const std::filesystem::path table_path;
        std::vector<offset_size_t> offset_size_list;
        std::vector<Type> type_list;
        std::vector<std::string> column_name_list;
        uint64_t size;
        uint64_t reserve;
        uint64_t primary_key;
        uint64_t tuple_size;
        FILE *bit_fp;
        FILE *main_fp;
    };
public:
    static struct struct_for_construct construct_helper(const std::string &database_name, const std::string &table_name);
    Disk_table(const struct struct_for_construct &);
    ~Disk_table();
    void insert(const std::vector<Tuple> &x);
    void insert(const Tuple &x);
    void set_i_begin();
    void inc_i();
    void read_i(const uint64_t pos_in_tuple, Value *const value);
    void write_i(const uint64_t pos_in_tuple, const Value &value);
    void remove_i();
    const std::string &get_table_name();
    const std::string &get_database_name();
    const std::string &get_column_name(uint64_t pos);
    const std::vector<std::string> &get_column_name();
    uint64_t get_size(const uint64_t pos);
    uint64_t get_real_size(const uint64_t pos);
    bool i_is_end()
    {
        return f_pos == reserve;
    }
    const std::vector<Type> &get_type_list() const;

private:
    static void get_column_name_list(const std::filesystem::path &table_path, const uint64_t num, std::vector<std::string> *column_name_list);
    bool find_next_empty();
    void insert_cur(const Tuple &);
    const std::string database_name;
    const std::string name;
    const std::vector<offset_size_t> offset_size_list;
    const std::vector<Type> type_list;
    const std::vector<std::string> column_name_list;
    const uint64_t primary_key;
    const uint64_t tuple_size;
    uint64_t f_pos;
    uint64_t reserve;
    FILE *const bit_fp;
    FILE *const main_fp;

private:
    Disk_table(const Disk_table&);
};

class Tuple
{
public:
    Tuple(const Table &table);
    void set_value(const uint64_t i, const struct Value value);
    void unset(const uint64_t i);
    const std::vector<Type> &get_type_list() const;
private:
    struct Cell
    {
        bool valid;
        struct Value struct_value;
    };
    int64_t err_code = 0;
    const std::vector<Type> type_list;
    std::vector<Cell> cell_list;
    friend class Disk_table;
    friend class Memory_table;
};
