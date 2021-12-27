#pragma once
#define __xsql_dbm_catch_print(func_name) \
    catch ( const xsql_io_exception & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：文件操作错误！\n"); \
    } \
    catch ( const std::filesystem::filesystem_error & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：文件操作错误！\n"); \
    } \
    catch ( const std::bad_alloc & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：内存不足！\n"); \
    } \
    catch ( const xsql_exception_database & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：格式错误，数据库可能已经损坏！\n"); \
    } \
    catch ( const xsql_exception_end_of_file & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：未预期的文件结尾，数据库可能已经损坏！\n"); \
    } \
    catch ( const xsql_exception_file_not_exist & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：未预期的文件不存在，数据库可能已经损坏！\n"); \
    } \
    catch ( ... ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：未知异常！\n"); \
    }

#define __xsql_dbm_handle_exception_last(func_name) \
    catch ( const xsql_io_exception & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：文件操作错误！\n"); \
        return ERR_FILE; \
    } \
    catch ( const std::filesystem::filesystem_error & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：文件操作错误！\n"); \
        return ERR_FILE; \
    } \
    catch ( const std::bad_alloc & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：内存不足！\n"); \
        return ERR_MEMORY_SPACE; \
    } \
    catch ( const xsql_exception_database & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：格式错误，数据库可能已经损坏！\n"); \
        return ERR_DATABASE; \
    } \
    catch ( const xsql_exception_end_of_file & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：未预期的文件结尾，数据库可能已经损坏！\n"); \
        return ERR_DATABASE; \
    } \
    catch ( const xsql_exception_file_not_exist & ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：未预期的文件不存在，数据库可能已经损坏！\n"); \
        return ERR_DATABASE; \
    } \
    catch ( ... ) \
    { \
        fprintf(stderr, "异常：在函数 " #func_name "()：未知异常！\n"); \
        throw; \
    }
// 适用于外部有__xsql_dbm_handle_exception_last包裹的情况
#define __xsql_dbm_try_delete_in(func_name, path, fail) \
    try \
    { \
        if ( __xsql_filesystem_exists(path) ) \
        { \
            __xsql_filesystem_remove_all(path); \
        } \
    } \
    catch ( const xsql_io_exception & ) \
    { \
        { \
            fail \
        } \
        try \
        { \
            __xsql_filesystem_remove_all(path); \
        } \
        catch ( ... ) \
        { \
            fprintf(stderr, "异常：在函数" #func_name "()文件操作错误！\n"); \
            throw; \
        } \
        throw; \
    } \
    catch ( const std::filesystem::filesystem_error & ) \
    { \
        { \
            fail \
        } \
        try \
        { \
            __xsql_filesystem_remove_all(path); \
        } \
        catch ( ... ) \
        { \
            fprintf(stderr, "异常：在函数" #func_name "()文件操作错误！\n"); \
            throw; \
        } \
        throw; \
    } \
    catch ( const std::bad_alloc & ) \
    { \
        { \
            fail \
        } \
        try \
        { \
            __xsql_filesystem_remove_all(path); \
        } \
        catch ( ... ) \
        { \
            fprintf(stderr, "异常：在函数" #func_name "()内存不足！\n"); \
            throw; \
        } \
        throw; \
    } \
    catch ( ... ) \
    { \
        { \
            fail \
        } \
        try \
        { \
            __xsql_filesystem_remove_all(path); \
        } \
        catch ( ... ) \
        { \
            fprintf(stderr, "异常：在函数" #func_name "()未知异常！\n"); \
            throw; \
        } \
        throw; \
    }

#define __xsql_dbm_try_delete_in_helper(func_name, path, release) \
    catch ( const xsql_io_exception & ) \
    { \
        {release}; \
        __xsql_dbm_try_delete_in(func_name, path, fprintf(stderr, "异常：在函数" #func_name "()：文件操作异常！\n");) throw; \
    } \
    catch ( const std::filesystem::filesystem_error & ) \
    { \
        {release}; \
        __xsql_dbm_try_delete_in(func_name, path, fprintf(stderr, "异常：在函数" #func_name "()：文件操作异常！\n");) throw; \
    } \
    catch ( const std::bad_alloc & ) \
    { \
        {release}; \
        __xsql_dbm_try_delete_in(func_name, path, fprintf(stderr, "异常：在函数" #func_name "()：内存不足！\n");) throw; \
    } \
    catch ( ... ) \
    { \
        {release}; \
        __xsql_dbm_try_delete_in(func_name, path, fprintf(stderr, "异常：在函数" #func_name "()：其他异常！\n");) throw; \
    }

#define __xsql_dbm_print_exception(exc_str) \
    do \
    { \
        fprintf(stderr, "异常：在" __xsql_dbm_func_name "；" exc_str "！\n"); \
    } while ( 0 )
