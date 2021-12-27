/**
 * @file xsql_compile_typedef.hpp
 * @author your name (you@domain.com)
 * @brief 存放编译过程中所共用的公共结构信息
 * @version 0.1
 * @date 2021-12-05
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once
#ifndef _xsql_compile_typedef_hpp_
#define _xsql_compile_typedef_hpp_

#include <vector>
#include <string>
#include "xsql_commandSQL.hpp"

// 当编译过程中遇到一个错误，是否尝试完成编译
// 如果为是，则会累计 err_detail_t 信息
// 如果为否，直接抛出 err_detail_t 异常（目前未实现）
#define TRY_CONTINUE_AFTER_ONE_ERROR false

namespace cp
{
    enum err_code
    {
        success = 0,     // 成功执行
        no_str,          // 没有字符串
        illegal_id,      // 非法标识符
        syntax_error,    // 语法错误
        no_such_table,   // 找不到表名
        no_such_column,  // 找不到列名
        repeated_colname,// 重复的列名（出现在insert和update的写入操作）
        unclear_colname, // 列名不清晰（可能存在多表同名）
        type_error,      // 列属性错误
        str_too_long,    // 字符串过长
        invalid_num,     // 非法数字串
        not_support,     // 不支持的操作
    };

    /**
     * @brief 详细编译错误结构体
     * @param code err_code错误码
     * @param info 详细错误信息
     */
    struct err_detail_t
    {
        err_code code;
        std::string info;

        err_detail_t(err_code code, std::string info = "no detail") : code(code), info(info) {}
    };

    /**
     * @brief sql语法中所有关键字的id都在这里定义（包括字母型，符号型等等）
     *
     */
    enum kid_t
    {
        //未找到对应的关键字
        _not_found = 0,
        // 字母型关键字
        Add,
        All,
        Alter,
        And,
        As,
        Asc,
        Avg,
        Between,
        Bool,
        By,
        Char,
        Count,
        Create,
        Database,  // drop/create database
        Databases, // show databases
        Delete,
        Desc,
        Distinct,
        Double,
        Drop,
        Except,
        Exists,
        False,
        Float,
        Foreign,
        From,
        Group,
        Having,
        In,
        Inner,
        Insert,
        Int,
        Intersect,
        Into,
        Join,
        Key,
        Long,
        Max,
        Min,
        Not,
        Null,
        On,
        Or,
        Order,
        Outer,
        Primary,
        Real,
        References,
        Select,
        Set,
        Show, // mysql 的 show 指令
        Sum,
        Table,
        Tables, // show tables
        True,
        Union,
        Update,
        Use, // 切换当前活动的数据库
        Using,
        Values,
        Where,
        // 符号型关键字
        //句末
        End,        // ;
                    // 比较
        Equal,      // =
        Larger,     // >
        Smaller,    // <
        No_larger,  // <=   !>
        No_Smaller, // >=   !<
        Not_equal,  // !=   <>

        // 运算
        opAdd, // +
        opSub, // -
        opDiv, // /

        // 格式
        Left_br,  // (
        Right_br, // )
        Star,     // *
        Dot,      // .
        Sep,      // ,

        // 自定义类型

        typeText,  // 'xxxx' "xxxx"
        typeWord,  // xxxx
        typeInt,   // 123 -123
        typeFloat, // 123.45 .45 123.
    };

    /**
     * @brief token串存放结构。index指示特殊元素（如用户定义变量）的存放位置。下标从0开始
     *
     */
    struct token_t
    {
        kid_t key_id;
        /**
         * @brief 指示特殊元素（如用户定义变量）的字符串存放位置。下标从0开始
         *
         */
        int32_t index;
        token_t(kid_t kid, int32_t index) : key_id(kid), index(index) {}
        token_t() : key_id(kid_t::_not_found), index(0) {}
    };

    /**
     * @brief
     * 存放SQL指令的结构体。通常每一条指令都会得到
     */
    struct sql_cmd_t
    {
        int32_t cmd;
        void *op1;
        void *op2;
        void *op3;

        sql_cmd_t(int32_t cmd, void *op1, void *op2, void *op3)
            : cmd(cmd), op1(op1), op2(op2), op3(op3) {}
        sql_cmd_t()
            : cmd(0), op1(NULL), op2(NULL), op3(NULL) {}
    };

    /**
     * @brief SQL指令序列。很可能无法单独使用（大多数情况需要配合一个或多个 exec_t（取决于存在多少个where表达式）使用
     *
     */
    typedef std::vector<sql_cmd_t> sql_cmds_t;
}

#endif //!_xsql_compile_typedef_hpp_