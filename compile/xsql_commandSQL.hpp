/**
 * @brief
 * 本文件定义了SQL操作指令相关的内容。包括指令码及指令结构体
 */

#pragma once
#ifndef _xsql_commandSQL_hpp_
#define _xsql_commandSQL_hpp_

namespace cp
{
    enum code_sql_t
    {
        no_op,

        // op1 : Table* 数据来源的表对象指针
        // op2 : std::vector<int64_t>*型，指示被选中的下标数组（按选中顺序输出，如[3,0,1]则选中0,1,3列但按3,0,1的顺序输出）
        // op3 : Table* 数据输出的目标表对象指针
        CMD_SELECT, // 无聚集选择 (select指令)

        // 不被使用
        // op1 : Table* 数据来源的表对象指针
        // op2 : ????
        // ???
        CMD_SELECTX, // 带聚集（GROUP BY或avg类函数）选择 (select指令)

        // op1 : Table* 数据来源的表对象指针
        // op2 : int 型，函数序号
        // op3 : Table* 数据输出的目标表对象指针
        CMD_WHERE, // where指令

        // op1 : Table*  数据来源的表 1 对象指针
        // op2 : Table*  数据来源的表 2 对象指针
        // op3 : Table* 数据输出的目标表对象指针
        CMD_CARTESIAN, // 表连接指令 op1 X op2

        // op1 : Table* 数据来源的表对象指针
        // op2 : int 型，函数序号
        // op3 : Table* 数据输出的目标表对象指针
        CMD_EXEC, // 行内数据执行指令，可以用于UPDATE

        // op1 : Table* 要释放的表对象指针
        // op2 : 无
        // op3 : 无
        CMD_RELEASE, // 释放临时表指令

        // op1 : std::string* 要创建的表名
        // op2 : cp::colum_info_t* 按顺序表示每一列的属性
        // op3 : std::string* 目标数据库名称
        CMD_CREATET,   // 创建表指令

        // op1 : std::string* 要创建的数据库名
        // op2 : 无
        // op3 : 无
        CMD_CREATED,   // 创建数据库指令

        // op1 : int64_t 要插入的数据量
        // op2 : int 函数序号
        // op3 : Table* 数据输出的目标表对象指针
        CMD_INSERT,   //插入指令 
        
        // op1 : std::string* 要删除的表名
        // op2 : std::string* 目标数据库名称
        // op3 : 无
        CMD_DROPT,   // 删除表指令
        
        // op1 : std::string* 要删除的数据库名称
        // op2 : 无
        // op3 : 无
        CMD_DROPD,   // 删除数据库指令
        
        // op1 : std::string* 所在表名
        // op2 : std::string* 表所在数据库名称
        // op3 : int 函数序号
        CMD_DELETE,   // 删除数据指令，根据f每次运行的返回值决定这一行是否删除

        // op1 : std::string* 目标数据库名称
        // op2 : 无
        // op3 : 无
        CMD_USE,   // 切换当前活跃的数据库

        // op1 : 无
        // op2 : 无
        // op3 : 无
        CMD_SHOWDB,   // 显示当前保存的数据库

        // op1 : 无
        // op2 : 无
        // op3 : 无
        CMD_SHOWT,   // 显示当前活动数据库内的所有表

        // // op1 : 
        // // op2 : 
        // // op3 : 
        // CMD_,   // 
    };
} // namespace cp

#endif //!_xsql_commandSQL_hpp_