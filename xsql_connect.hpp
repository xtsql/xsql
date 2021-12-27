/*
 * @Author: your name
 * @Date: 2021-12-09 04:33:15
 * @LastEditTime: 2021-12-09 10:05:26
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \xsql\xsql_connect.hpp
 */
/**
 * @file xsql_connect.hpp
 * @author your name (you@domain.com)
 * @brief
 * 本文件定义了一个解释器，可以用于读取用户输入，也可用于处理SQL队列。
 * @version 0.9
 * @date 2021-12-04
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#ifndef _xsql_connect_hpp_
#define _xsql_connect_hpp_

#include<iostream>
#include<string>
#include<vector>
#include"xsql_compile_typedef.hpp"
#include"xsql_compile.hpp"
#include"xsql_dbm.hpp"

using cp::sql_cmd_t;

namespace expl
{
    /**
     * @brief 错误码
     *
     */
    enum err_code{
        SUCCESS,
        EXCEED,
        ILLEGAL,
        FAILED
    };


    /**
     * @brief 调用用户输入
     * @param input 用来储存用户输入的字符串
     *
     */
    void accept_userinput();

    /**
     * @brief 解析命令
     * @param queue 用来传入编译得到的指令
     * 
     */
    std::vector<err_code> explain_queue(cp::sql_program_t queue);

    /**
     * @brief 创建数据库
     *
     */
    err_code create_db(cp::sql_cmd_t cmd);

    /**
     * @brief 删除数据库
     *
     */
    err_code drop_db(cp::sql_cmd_t cmd);


    /**
     * @brief 创建数据表
     *
     */
    err_code create_t(cp::sql_cmd_t cmd);

    /**
     * @brief 展示数据库
     *
     */
    err_code show_db();

    /**
     * @brief 展示数据表
     *
     */ 
    err_code show_t();

    /**
     * @brief 切换数据库
     * 
     */
    err_code use_db(cp::sql_cmd_t cmd);

    /**
     * @brief 删除数据表
     *
     */
    err_code drop_t(cp::sql_cmd_t cmd);

    /**
     * @brief 插入元组
     *
     */
    err_code insert_turple(cp::sql_cmd_t cmd);

    /**
     * @brief 选择元组
     *
     */
    err_code select_turple(cp::sql_cmd_t cmd);
    /**
     * @brief 删除元组
     *
     */
    err_code delete_turple(cp::sql_cmd_t cmd);
}

#endif //!_xsql_connect_h_
