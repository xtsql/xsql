/**
 * @file xsql_compile.hpp
 * @author your name (you@domain.com)
 * @brief 编译接口，用此接口传入字符串即可进行编译得到控制命令和where执行器
 * @version 0.1
 * @date 2021-12-05
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once
#ifndef _xsql_compile_h_
#define _xsql_compile_h_

#include <string>
#include <vector>
#include "xsql_compile_higherdef.hpp"
#include "xsql_exec.hpp"

namespace cp
{
    /**
     * @brief SQL语句编译指令。
     *
     * @param str 待编译的字符串
     * @param result 编译结果。包含一系列SQL执行控制语句与进行where条件运行的执行器，传入空对象即可。如果编译失败，该对象也会被清空。
     * @param str_len 要编译的字符串长度（不包括末尾\0），如果传入0则为自动获取
     * @return int64_t
     */
    err_code analysis(const char *str, std::vector<sql_program_t> &result, int str_len = 0);

    /**
     * @brief 向控制台输出某一指令的信息。
     * 
     * @param cmd 
     */
    void show_sql_cmd(const sql_cmd_t& cmd);
}

#endif //!_xsql_compile_h_