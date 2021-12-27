/**
 * @file xsql_graSem_anls.hpp
 * @author your name (you@domain.com)
 * @brief 本文件提供语法及语义分析能力。可以直接由token串得到 sql_cmds_t 及 exec_t。
 * @version 0.1
 * @date 2021-12-06
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once
#ifndef _xsql_graSem_anls_hpp_
#define _xsql_graSem_anls_hpp_

#include <string>
#include <vector>
#include "xsql_compile_higherdef.hpp"

namespace cp
{

    /**
     * @brief 根据词法分析得到的字符串和字符映射表，处理并得到SQL程序
     * 
     * @param code_list token 串表
     * @param rst_map 与token 串对应的字符映射表
     * @param result 识别结果。如果识别失败，该表仍会被清空；如果识别成功，会写入可执行的语句
     * （每一条可执行语句包含一组 sql_cmd 及0至多个 exec_t）
     * @return err_code 返回识别结果。成功时为 err_code::success 。
     */
    err_code grasem_analysis(
        const std::vector<cp::token_t> &code_list,
        const std::vector<std::string> &rst_map,
        std::vector<sql_program_t> &result);

} // namespace cp

#endif // !_xsql_graSem_anls_hpp_