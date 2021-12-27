/**
 * @file xsql_compile.cpp
 * @author your name (you@domain.com)
 * @brief 本文件实现了SQL语句的编译过程，包括词法、语法、语义分析，并最终生成指导查询器的SQL控制指令
 * @version 0.1
 * @date 2021-12-05
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "xsql_compile.hpp"
#include "xsql_mor_anls.hpp"
#include "xsql_graSem_anls.hpp"
#include "xsql_dbm.hpp"
#include <iostream>
using namespace std;

// #define _COMPILE_DEBUG_ 5

cp::err_code cp::analysis(const char *str, std::vector<sql_program_t> &result, int str_len)
{
    // 修正参数
    if (str_len == 0)
    {
        str_len = strlen(str);
    }
    if (strlen == 0)
        return err_code::no_str;
    std::vector<cp::token_t> code_list;
    std::vector<std::string> usr_strs;
    // 词法分析逻辑
    {
        // 复制待编译字符串
        // 同时将所有不可打印的 ASCII 字符替换为空格（大于127的区间不做替换）
#ifdef _COMPILE_DEBUG_
        printf("before:\n %s\n", str);
        // for(int i = 0;str[i]!=0;++i){
        //     printf("%c(%d)\n",str[i],str[i]);
        // }
#endif
        char *to_compile = new char[str_len + 1];
        for (int i = 0; i < str_len; ++i)
        {
            if (((str[i] < 32) && (str[i] > 0)) || str[i] == 127)
                to_compile[i] = ' ';
            else
                to_compile[i] = str[i];
        }
#ifdef _COMPILE_DEBUG_
        // 输出替换结果
        printf("after:\n ");
        printf(to_compile);
        printf("\n");
        // for(int i = 0;to_compile[i]!=0;++i){
        //     printf("%c(%d)\n",to_compile[i],to_compile[i]);
        // }
#endif
        err_code rst1 = mor_analysis(to_compile, code_list, usr_strs, str_len);
        delete[] to_compile;
        if (rst1 != err_code::success)
            return rst1;
#ifdef _COMPILE_DEBUG_
        // 输出分析结果
        for (int i = 0; i < code_list.size(); ++i)
        {
            printf("%-4d  %-4d", (int)(code_list[i].key_id), (code_list[i].index));
            if (code_list[i].index)
                cout << (usr_strs[code_list[i].index]);
            cout << endl;
        }
        for (int i = 0; i < usr_strs.size(); ++i)
        {
            printf("| %d\t %s\n", i, usr_strs[i].c_str());
        }
#endif
    }
    // 语法分析逻辑
    std::vector<sql_program_t> total_result;
    err_code sec_result = cp::grasem_analysis(code_list, usr_strs, total_result);
    if (sec_result == err_code::success)
    {
        result.clear();
        result = total_result;
#ifdef _COMPILE_DEBUG_
        // 调试用输出
        for (int i = 0; i < total_result.size(); ++i)
        {
            printf("\nprogram %d\n", i);
            for (int j = 0; j < total_result[i].sql_cmds.size(); ++j)
            {
                show_sql_cmd(total_result[i].sql_cmds[j]);
            }
        }
#endif
    }
    return sec_result;
}

void cp::show_sql_cmd(const sql_cmd_t &cmd)
{
    switch (cmd.cmd)
    {
    case code_sql_t::CMD_SELECT:
    {
        printf("SELECT\n");
        printf("%-15s from <table at0x%llX>\n", "", (int64_t)(cmd.op1));
        printf("%-15s  to  <table at0x%llX>\n", "", (int64_t)(cmd.op3));
        printf("%-15s columns     :", "");
        {
            vector<int64_t> &t = *(vector<int64_t> *)cmd.op2;
            for (int i = 0; i < t.size(); ++i)
            {
                printf("%lld ", t[i]);
            }
            printf("\n");
        }
        {
            printf("%-15s column name:", "");
            ::Table *tb = (::Table *)(cmd.op3);
            vector<string> s = tb->get_column_name();
            for (int i = 0; i < s.size(); ++i)
            {
                printf("%s ", s[i].c_str());
            }
            printf("\n");
        }
        break;
    }
    case code_sql_t::CMD_SELECTX:
    {
        printf("%-15s %-20s %-20s %-20s\n", "SELECTX", "<command not support>", "", "");
        break;
    }
    case code_sql_t::CMD_WHERE:
    {
        printf("%-15s <table at0x%08llX> <ecec id%-11d> <table at0x%08llX>\n", "WHERE", (int64_t)(cmd.op1), *(int*)(&(cmd.op2)), (int64_t)(cmd.op3));
        break;
    }
    case code_sql_t::CMD_CARTESIAN:
    {
        printf("%-15s <table at0x%08llX> <table at0x%08llX> <table at0x%08llX>\n", "CARTESIAN", (int64_t)(cmd.op1), (int64_t)(cmd.op2), (int64_t)(cmd.op3));
        break;
    }
    case code_sql_t::CMD_EXEC:
    {
        printf("%-15s <table at0x%08llX> <ecec id%-11d> <table at0x%08llX>\n", "EXEC", (int64_t)(cmd.op1), *(int*)(&(cmd.op2)), (int64_t)(cmd.op3));
        break;
    }
    case code_sql_t::CMD_RELEASE:
    {
        printf("%-15s <table at0x%08llX>\n", "RELEASE", (int64_t)(cmd.op1));
        break;
    }
    case code_sql_t::CMD_CREATET:
    {
        if (cmd.op3)
            printf("%-15s %-20s to %-20s\n", "CREATET", ((std::string *)(cmd.op1))->c_str(), ((std::string *)(cmd.op3))->c_str());
        else
            printf("%-15s %-20s\n", "CREATET", ((std::string *)(cmd.op1))->c_str());
        cp::colum_infos_t &infos = *(cp::colum_infos_t *)(cmd.op2);
        const char tname[][8]={"INT","LONG","FLOAT","DOUBLE","CHAR","BOOLEAN"};
        for (int i = 0; i < infos.size(); ++i)
        {
            printf("%10s%3d %-15s  %-8s  size=%d\n", "", i, infos[i].col_name.c_str(), tname[infos[i].col_type.type], infos[i].col_type.size);
        }
        break;
    }
    case code_sql_t::CMD_CREATED:
    {
        printf("%-15s %s\n", "CREATED", ((std::string *)(cmd.op1))->c_str());
        break;
    }
    case code_sql_t::CMD_INSERT:
    {
        printf("%-15s number:%-13lld <vector at0x%08llX> <table at0x%08llX>\n", "INSERT", (int64_t)(cmd.op1), (int64_t)(cmd.op2), (int64_t)(cmd.op3));
        break;
    }
    case code_sql_t::CMD_DROPT:
    {
        if (cmd.op2)
            printf("%-15s %-19s to %-18s\n", "DROPT", ((std::string *)(cmd.op1))->c_str(), ((std::string *)(cmd.op2))->c_str());
        else
            printf("%-15s %-20s\n", "DROPT", ((std::string *)(cmd.op1))->c_str());
        break;
    }
    case code_sql_t::CMD_DROPD:
    {
        printf("%-15s %s\n", "DROPD", ((std::string *)(cmd.op1))->c_str());
        break;
    }
    case code_sql_t::CMD_DELETE:
    {
        if (cmd.op2)
            printf("%-15s %-19s in %-18s <ecec id%-11lld>\n", "DELETE", ((std::string *)(cmd.op1))->c_str(), ((std::string *)(cmd.op2))->c_str(), (int64_t)(cmd.op3));
        else
            printf("%-15s %-20s <ecec id%-11lld>\n", "DELETE", ((std::string *)(cmd.op1))->c_str(), (int64_t)(cmd.op3));
        break;
    }
    case code_sql_t::CMD_USE:
    {
        printf("%-15s %s\n", "USE", ((std::string *)(cmd.op1))->c_str());
        break;
    }
    case code_sql_t::CMD_SHOWDB:
    {
        printf("SHOWDB\n");
        break;
    }
    case code_sql_t::CMD_SHOWT:
    {
        printf("SHOWT\n");
        break;
    }
    }
}