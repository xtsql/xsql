/**
 * @file xsql_exec.hpp
 * @author your name (you@domain.com)
 * @brief
 * 本文件定义了一个处理器，可以用于某一行是否被选取的判断，也可用于修改某一行内的值。
 * 不同行逻辑无关，但一个执行器通常和一个表绑定
 * @version 0.9
 * @date 2021-12-04
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#ifndef _exec_hpp_
#define _exec_hpp_

#include <vector>
#include "xsql_compile_typedef.hpp"
#include "xsql_dbm.hpp"
// 这个定义是分离测试的时候使用的。最终不使用此typedef
// typedef void Tuple;

namespace exec
{

    /**
     * @brief
     * 指令格式
     */
    struct cmd_t
    {
        int32_t cmd,
            op1,
            op2,
            f;

        cmd_t(int32_t cmd, int32_t op1, int32_t op2, int32_t f) : cmd(cmd), op1(op1), op2(op2), f(f) {}
    };
    /**
     * @brief
     * 变量、常量存储格式。
     * 由于SQL语法支持NULL格式，因此不可简单地使用 void*
     */
    struct val_t
    {
        bool is_null; /** @brief 指示此变量是否为空 */
        int32_t size; /** @brief 指示此变量占用的内存大小，便于alloc*/
        void *pdata;  /** @brief 变量所在内存指针 */

        val_t()
        {
            this->is_null = true;
            this->pdata = NULL;
            this->size = 0;
        }

        val_t(int32_t size, void *data)
        {
            this->is_null = false;
            this->size = size;
            this->pdata = malloc(size);
            memcpy(this->pdata, data, size);
        }

        val_t(const double data)
        {
            this->is_null = false;
            this->size = sizeof(double);
            this->pdata = malloc(this->size);
            memcpy(this->pdata, (void *)(&data), this->size);
        }

        val_t(const float data)
        {
            this->is_null = false;
            this->size = sizeof(float);
            this->pdata = malloc(this->size);
            memcpy(this->pdata, (void *)(&data), this->size);
        }

        val_t(const int data)
        {
            this->is_null = false;
            this->size = sizeof(int);
            this->pdata = malloc(this->size);
            memcpy(this->pdata, (void *)(&data), this->size);
        }

        val_t(const int64_t data)
        {
            this->is_null = false;
            this->size = sizeof(int64_t);
            this->pdata = malloc(this->size);
            memcpy(this->pdata, (void *)(&data), this->size);
        }

        val_t(bool val)
        {
            this->is_null = false;
            this->size = sizeof(bool);
            this->pdata = malloc(this->size);
            *((bool *)(this->pdata)) = val;
        }

        val_t(const val_t &old)
        {
            this->is_null = old.is_null;
            this->size = old.size;
            if (this->is_null)
                this->pdata = NULL;
            else
            {
                this->pdata = malloc(this->size);
                memcpy(this->pdata, old.pdata, this->size);
            }
        }

        ~val_t()
        {
            this->is_null = true;
            if (this->pdata)
            {
                free(this->pdata);
                this->pdata = NULL;
            }
        }

        val_t &operator=(const val_t &op2)
        {
            this->is_null = op2.is_null;
            this->size = op2.size;
            if ((!this->is_null) && (op2.pdata))
            {
                this->pdata = malloc(this->size);
                memcpy(this->pdata, op2.pdata, this->size);
            }
            else
                this->pdata = NULL;
            return *this;
        }
    };

    /**
     * @brief
     * 对vector的封装，用此方法传入脚本
     */
    typedef std::vector<cmd_t> program_t;

    /**
     * @brief
     * 对vector的封装，用此方法传入常量
     */
    typedef std::vector<val_t> data_table_t;

    /**
     * @brief 通用的执行器，创建时已写入指令序列、常量表，并初始化变量表（按所给出的变量表长度）
     * 指令执行过程中，变量表保持动态malloc状态（即，写入前malloc，返回/覆盖前free）
     * 基于此特性，允许外部代码变量序号几乎无限（取决于cmd_t中关于变量长度的限制）
     *
     * 理论上，程序存在三个地址空间：常量空间，变量空间，元组数据空间
     * 实际上，常量空间及变量空间合并，二者连续且变量空间位于高地址位。
     */
    class exec_t
    {
    public:
        /**
         * @brief 下标索引所支持的大小
         *
         */
        typedef int32_t index_t;

        /**
         * @brief Construct a new exec object
         *
         * @param cmds 执行所依赖的指令序列
         * @param variable_num 程序所会出现的最大变量数量
         * @param constant_table 程序执行所依赖的常量表。
         * 常量表下标为0开始即为有效常量，但cmds中首个常量序号应该从-1递减（即 -1-序号 == 下标）
         * @param str_len_limit 是一个数组，指示要处理的 Tuple 中各下标写入字符串时的保护长度。
         * 如果Tuple内不存在字符串或不会向Tuple产生写入，可以传入NULL即可。
         * 单独屏蔽某一下标的检查，将对应值置0即可
         */
        exec_t(const program_t &cmds,
               const index_t variable_num,
               const data_table_t &constant_table,
               const std::vector<int> *const str_len_limit = NULL);

        exec_t();
        exec_t(const exec_t &);
        exec_t &operator=(const exec_t &);
        ~exec_t();
        /**
         * @brief 根据data运行内部判别逻辑。
         *
         * @param data 要处理的Table。执行器支持对Table的写入，但只会读写当前迭代器指向的那一行。
         * @param is_written 如果不为 NULL，当对Tuple发生写入逻辑（尽管可能写入但不改变值）时将其置为 true
         * @return true 代码中出现 RETURN TRUE 逻辑时返回。
         * @return false 代码中出现 RETURN FALSE 逻辑时返回。对象不可运行也会得到false。
         */
        bool run(Table *data, bool *is_written = NULL);
        /**
         * @brief 释放本函数所使用的空间，此后无法再使用本对象
         */
        void exit();
        /**
         * @brief
         * 查看当前对象是否可运行
         * @return true 当前对象可运行
         * @return false 当前对象不可运行
         */
        bool is_ok();

        friend int64_t analysis(const char *str, cp::sql_cmds_t &rst_cmd, exec::exec_t &result_actuator);

    private:
        /**
         * @brief 当前对象状态
         *
         */
        enum __state_t
        {
            init,
            ok,
            zombie,
        } __state;
        /**
         * @brief
         * 程序表
         */
        program_t __prog;
        /**
         * @brief 变量及常量空间
         *
         */
        data_table_t __var_table;
        /**
         * @brief 常量数量，同时也是变量表起始地址
         *
         */
        int __constance_len;
        /**
         * @brief 字符串保护信息
         *
         */
        std::vector<int> __str_len_limit;

        /**
         * @brief 内存空间基地址。为 i 表示 地址 -x 映射到地址 -(x+i)处
         * 
         */
        int32_t __base_addr;

        /**
         * @brief 当前运行时的Tuple指针
         * 
         */
        Table* __cur_data_ptr;
        /**
         * @brief 初始化和重置使用的函数。此函数主要用于提供给友元函数compile重置此对象使用
         *
         * @param cmds
         * @param variable_num
         * @param constant_table
         * @param str_len_limit
         */
        void _reinit(const program_t &cmds,
                     const index_t variable_num,
                     const data_table_t &constant_table,
                     const std::vector<int> *const str_len_limit = NULL);

        /**
         * @brief 读数据的封装，根据下标判断应该从哪里取数据
         *
         * @param index 指令集中出现的读取下标
         * @param val 读到的数据。如果超出下标访问不会写入并通过返回值报错
         * @return int64_t 成功时得到0，失败得到-1
         */
        inline int64_t __get(index_t index, val_t *val);
        /**
         * @brief 写数据的封装，根据下标判断应该向哪里写数据
         *
         * @param index 指令集中出现的读取下标
         * @param val 要写入的数据。如果超出下标则不会有写入动作。
         * 注意，val->pdata所指向的内容不复制（即如果访问变量空间，可以对该空间写入），但size和is_null无法传导。
         * @param written_flag 如果不为 NULL，当对Tuple发生写入逻辑（尽管可能写入但不改变值）时将其置为 true
         * @return int64_t 成功时得到0，失败得到-1
         */
        inline int64_t __set(index_t index, const val_t& val, bool *written_flag = NULL);
    };

}

#endif //!_exec_hpp_