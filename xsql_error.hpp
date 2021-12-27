#pragma once
// 文件读写错误
#define ERR_FILE -1

// 参数错误：目标不存在(如select一张不存在的表)
#define ERR_NOT_EXIST -2

// 参数错误：目标已存在(如create一张已经存在的表)
#define ERR_ALREADY_EXIST -3

// 其他参数错误
#define ERR_ARG -4

// 内存不足
#define ERR_MEMORY_SPACE -5

// 磁盘空间不足
#define ERR_DISK_SPACE -6

// 资源正忙（如删除数据库，但是数据库中有表是打开的）
#define ERR_BUSY -7

// 其他(未知异常)
#define ERR_OTHER -8

// 数据库文件格式不正确，数据库可能已经损坏
#define ERR_DATABASE -9

class xsql_exception
{
};
class xsql_io_exception: public xsql_exception
{
};
class xsql_exception_fopen: public xsql_io_exception
{
};
class xsql_exception_fclose: public xsql_io_exception
{
};
class xsql_exception_fread: public xsql_io_exception
{
};
class xsql_exception_fwrite: public xsql_io_exception
{
};
class xsql_exception_fseek: public xsql_io_exception
{
};
class xsql_exception_ftell: public xsql_io_exception
{
};
class xsql_exception_fgets: public xsql_io_exception
{
};
class xsql_exception_std_filesystem_exists: public xsql_io_exception
{
};
class xsql_exception_std_filesystem_is_directory: public xsql_io_exception
{
};
class xsql_exception_std_filesystem_create_directory: public xsql_io_exception
{
};
class xsql_exception_std_filesystem_remove_all: public xsql_io_exception
{
};
class xsql_exception_std_filesystem_directory_iterator: public xsql_io_exception
{
};
class xsql_exception_std_filesystem_directory_iterator_increment: public xsql_io_exception
{
};
class xsql_exception_boost_dll_program_location: public xsql_io_exception
{
};
class xsql_exception_end_of_file: public xsql_exception
{
};
class xsql_exception_database: public xsql_exception
{
};
class xsql_exception_file_not_exist: public xsql_exception
{
};
class xsql_exception_fopen_directory: public xsql_exception
{
};
class xsql_exception_args: public xsql_exception
{
};
class xsql_exception_not_exist: public xsql_exception
{
};
class xsql_exception_busy: public xsql_exception
{
};
class xsql_exception_memory: public xsql_exception
{
};
