/**
 * @file xsql_compile_higherdef.hpp
 * @author your name (you@domain.com)
 * @brief 存放一些高级结构，在编译器顶层接口及语法、语义分析阶段使用
 * @version 0.1
 * @date 2021-12-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef _xsql_compile_higherdef_hpp_
#define _xsql_compile_higherdef_hpp_

#include "xsql_exec.hpp"
#include "xsql_compile_typedef.hpp"
#include "xsql_dbm.hpp"

namespace cp
{
    typedef std::vector<exec::exec_t> execs_t;

    /**
     * @brief SQL 程序。每个 sql_program_t 对应一条可被执行的 SQL 语句。其中：
     * @param sql_cmds SQL 指令序列
     * @param execs 运行 sql_cmds 所需要的 exec_t（可能为0个到多个）
     */
    struct sql_program_t
    {
        sql_cmds_t sql_cmds;
        execs_t execs;
    };

    struct colum_info_t
    {
        std::string table_name = ""; // 表名，只在 WHERE 语句内有效，可不关心此选项
        std::string col_name = "";   // 列名
        ::Type col_type = {.type = ::Data_type::BOOLEAN, .size = 0};
    };

    typedef std::vector<colum_info_t> colum_infos_t;
} // namespace cp

#endif // !_xsql_compile_higherdef_hpp_