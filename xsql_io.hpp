#pragma once
#include "xsql_error.hpp"

#include <boost/dll.hpp>
#include <boost/filesystem.hpp>
#include <filesystem>
#include <stdio.h>

#ifdef __xsql_windows_
    #define fopen(a, b) _wfopen((a), (b))
    #define __auto_wstr(a) L##a
#else
    #define __auto_wstr(a) a
#endif

static inline bool __xsql_filesystem_exists(const std::filesystem::path &path);
static inline bool __xsql_filesystem_is_directory(const std::filesystem::path &path);

#ifdef __xsql_windows_
static inline FILE *__xsql_fopen(const wchar_t *a, const wchar_t *b)
#else
static inline FILE *__xsql_fopen(const char *a, const char *b)
#endif
{
    if ( __xsql_filesystem_is_directory(a) )
    {
        fprintf(stderr, "异常：使用fopen打开一个目录！\n");
        throw xsql_exception_fopen_directory();
    }
    FILE *fp = fopen(a, b);
    if ( fp == NULL )
    {
#ifdef __xsql_windows_
        if ( !__xsql_filesystem_exists((std::wstring)a) )
#else
        if ( !__xsql_filesystem_exists((std::string)a) )
#endif
        {
            fprintf(stderr, "fopen不存在的文件！\n");
            throw xsql_exception_file_not_exist();
        }
        fprintf(stderr, "fopen出错！\n");
        throw xsql_exception_fopen();
    }
    return fp;
}

#define __xsql_fseek(a, b, c) \
    do \
    { \
        if ( fseek(a, b, c) != 0 ) \
        { \
            fprintf(stderr, "fseek出错！\n"); \
            throw xsql_exception_fseek(); \
        } \
    } while ( 0 )

static inline long int __xsql_ftell(FILE *a)
{
    long int temp_ret = ftell(a);
    if ( temp_ret == -1 )
    {
        fprintf(stderr, "ftell出错！\n");
        throw xsql_exception_ftell();
    }
    return temp_ret;
}

#define __xsql_fread(a, b, c, fp) \
    do \
    { \
        size_t __xsql__count = c; \
        FILE *__xsql_fp = fp; \
        if ( fread(a, b, __xsql__count, __xsql_fp) != __xsql__count ) \
        { \
            if ( ferror(__xsql_fp) == 0 && feof(__xsql_fp) == 1 ) \
            { \
                fprintf(stderr, "异常：在__xsql_fread；达到文件末尾\n"); \
                throw xsql_exception_end_of_file(); \
            } \
            fprintf(stderr, "异常：在__xsql_fread；文件操作出错\n"); \
            throw xsql_exception_fread(); \
        } \
    } while ( 0 )

#define __xsql_fwrite(a, b, c, d) \
    do \
    { \
        size_t __xsql__count = c; \
        FILE *const __xsql_fp = d; \
        if ( fwrite(a, b, __xsql__count, __xsql_fp) != __xsql__count ) \
        { \
            fprintf(stderr, "异常：在__xsql_fwrite；文件操作出错\n"); \
            throw xsql_exception_fwrite(); \
        } \
        try \
        { \
            __xsql_fseek(__xsql_fp, 0, SEEK_CUR); \
        } \
        catch ( ... ) \
        { \
            fprintf(stderr, "异常：在__xsql_fwrite；\n"); \
            throw; \
        } \
    } while ( 0 )

#define __xsql_fclose(a) \
    do \
    { \
        if ( fclose(a) != 0 ) \
        { \
            fprintf(stderr, "fclose出错！\n"); \
            throw xsql_exception_fclose(); \
        } \
    } while ( 0 )

#define __xsql_fgets(str, n, stream) \
    do \
    { \
        FILE *__xsql_fp = stream; \
        if ( fgets(str, n, __xsql_fp) == NULL ) \
        { \
            if ( ferror(__xsql_fp) != 0 ) \
            { \
                fprintf(stderr, "异常：在__xsql_fgets；文件操作出错\n"); \
                throw xsql_exception_fgets(); \
            } \
            fprintf(stderr, "异常：在__xsql_fgets；达到文件末尾\n"); \
            throw xsql_exception_end_of_file(); \
        } \
    } while ( 0 )

static inline bool __xsql_filesystem_exists(const std::filesystem::path &a)
try
{
    return std::filesystem::exists(a);
}
catch ( const std::filesystem::filesystem_error & )
{
    fprintf(stderr, "std::filesystem::exists错误！\n");
    throw xsql_exception_std_filesystem_exists();
}
catch ( ... )
{
    fprintf(stderr, "std::filesystem::exists错误！其他异常！\n");
    throw;
}

static inline bool __xsql_filesystem_is_directory(const std::filesystem::path &a)
try
{
    return std::filesystem::is_directory(a);
}
catch ( const std::filesystem::filesystem_error & )
{
    fprintf(stderr, "std::filesystem::is_directory错误！\n");
    throw xsql_exception_std_filesystem_is_directory();
}
catch ( ... )
{
    fprintf(stderr, "std::filesystem::is_directory错误！其他异常！\n");
    throw;
}

#define __xsql_filesystem_create_directory(a) \
    do \
    { \
        try \
        { \
            std::filesystem::create_directory(a); \
        } \
        catch ( const std::filesystem::filesystem_error & ) \
        { \
            fprintf(stderr, "std::filesystem::create_directory出错！\n"); \
            throw xsql_exception_std_filesystem_exists(); \
        } \
        catch ( ... ) \
        { \
            fprintf(stderr, "std::filesystem::create_directory出错！\n"); \
            throw; \
        } \
    } while ( 0 )

#define __xsql_filesystem_remove_all(a) \
    do \
    { \
        try \
        { \
            std::filesystem::remove_all(a); \
        } \
        catch ( const std::filesystem::filesystem_error & ) \
        { \
            fprintf(stderr, "std::filesystem::remove_all出错！\n"); \
            throw xsql_exception_std_filesystem_remove_all(); \
        } \
        catch ( ... ) \
        { \
            fprintf(stderr, "std::filesystem::remove_all出错！\n"); \
            throw; \
        } \
    } while ( 0 )

static inline std::filesystem::directory_iterator &__xsql_std_filesystem_directory_iterator_increment(std::filesystem::directory_iterator &a)
try
{
    std::filesystem::directory_iterator &__xsql_ret = ++a;
    return __xsql_ret;
}
catch ( const std::filesystem::filesystem_error &x )
{
    fprintf(stderr, "异常：std::filesystem::directory_iterator::自增出错！\n");
    throw xsql_exception_std_filesystem_directory_iterator_increment();
}
catch ( ... )
{
    fprintf(stderr, "异常：std::filesystem::directory_iterator::自增出错！其他异常！\n");
    throw;
}
