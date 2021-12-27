/**
 * @file xsql_mor_anls.hpp
 * @author your name (you@domain.com)
 * @brief 这一文件用于进行词法分析，将得到一个序列及一个字符映射表
 * @version 0.1
 * @date 2021-12-03
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once
#ifndef _xsql_mor_anls_hpp_
#define _xsql_mor_anls_hpp_

#include <string>
#include <vector>
#include "xsql_compile_typedef.hpp"
#include "xsql_keywords.hpp"

namespace cp
{
    /**
     * @brief 进行词法分析，将得到一个序列及一个字符映射表
     *
     * @param str 要分析的字符串
     * @param code_list 分析结果的Token串列表。分析失败不会导致清空
     * @param rst_map 分析结果的字符串映射表。分析失败不会导致清空。表中有效元素从1开始（0号为空串）
     * @param str_len 要编译的字符串长度（不包括末尾\0）
     * @return cp::err_code 函数执行情况
     */
    err_code mor_analysis(const char *str, std::vector<cp::token_t> &code_list, std::vector<std::string> &rst_map, int str_len);
}
#endif //!_xsql_mor_anls_hpp_