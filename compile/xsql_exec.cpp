/**
 * @file xsql_exec.cpp
 * @author your name (you@domain.com)
 * @brief
 * 本文件作为xsql_exec.hpp的实现，实现一个判断器/执行器的的运行
 * @version 0.9
 * @date 2021-12-04
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <string>
// using std::string;
#include "malloc.h"
#include <assert.h>
#include "xsql_exec.hpp"
#include "xsql_commandEXEC.hpp"

/**
 * @brief SQL 中关于NULL的规则：
 * 布尔表达式中除非被吸收可得到true/false，得到unknown
 * 行内数值运算涉及 NULL 全得到 NULL
 * 对 NULL 进行比较得到 unknown，unknown用于输出时得到false
 */

/**
 * @brief 指示符号的运算扩展，使用指针。op1与op2直接使用 val_t* 即可
 * 但没有NULL型支持，不支持string，这里只是提供一种便捷写法
 */
#define PTYPE_OP(cmd, type, op1, op2) ((*((type *)(op1->pdata)))cmd(*((type *)(op2->pdata))))

/**
 * @brief 指示符号的运算扩展，不使用指针。op1与op2使用 val_t
 * 但没有NULL型支持，不支持string，这里只是提供一种便捷写法
 */
#define TYPE_OP(cmd, type, op1, op2) ((*((type *)(op1.pdata)))cmd(*((type *)(op2.pdata))))

/**
 * @brief 数据格式转换宏，不能用于转布尔/字符串
 *
 */
#define CH_TYPE(old_type, op1, new_type, f, wt_flag)                                     \
    {                                                                                    \
        val_t temp;                                                                      \
        assert(this->__get(op1, &temp) == 0);                                            \
        if (temp.is_null)                                                                \
            assert(this->__set(f, val_t(), wt_flag) == 0);                               \
        else                                                                             \
        {                                                                                \
            new_type i = (new_type)(*((old_type *)temp.pdata));                          \
            assert(this->__set(f, val_t(sizeof(new_type), (void *)(&i)), wt_flag) == 0); \
        }                                                                                \
    }

/**
 * @brief 数据格式转换宏，用于转字符串
 *
 */
#define CH_2STR(old_type, op1, f, wt_flag)                                                \
    {                                                                                     \
        val_t temp;                                                                       \
        assert(this->__get(op1, &temp) == 0);                                             \
        if (temp.is_null)                                                                 \
            assert(this->__set(f, val_t(), wt_flag) == 0);                                \
        else                                                                              \
        {                                                                                 \
            std::string i = std::to_string(*((old_type *)temp.pdata));                    \
            assert(this->__set(f, val_t(i.length(), (void *)(i.c_str())), wt_flag) == 0); \
        }                                                                                 \
    }

/**
 * @brief 数据格式转换宏，用于转布尔
 *
 */
#define CH_2BOOL(old_type, op1, f, wt_flag)                         \
    {                                                               \
        val_t temp;                                                 \
        assert(this->__get(op1, &temp) == 0);                       \
        if (temp.is_null)                                           \
            assert(this->__set(f, val_t(), wt_flag) == 0);          \
        else                                                        \
        {                                                           \
            if ((*((old_type *)temp.pdata)) == 0)                   \
                assert(this->__set(f, val_t(false), wt_flag) == 0); \
            else                                                    \
                assert(this->__set(f, val_t(true), wt_flag) == 0);  \
        }                                                           \
    }

/**
 * @brief 将表达式封装为一个对象的快捷操作，无法封装NULL
 *
 */
#define CH2VAR_T(name, mytypename, size_, myexpression) \
    exec::val_t name;                                   \
    name.is_null = false;                               \
    name.size = size_;                                  \
    name.pdata = malloc(size_);                         \
    {                                                   \
        mytypename t = myexpression;                    \
        memcpy(name.pdata, (void *)(&t), size_);        \
    }

/**
 * @brief 指示符号的运算扩展（不含比较），不使用指针。op1与op2使用 val_t
 * 有NULL型支持，不支持string
 */
#define FULL_OP(cmd, type, wt_flag)                                               \
    {                                                                             \
        val_t temp1, temp2;                                                       \
        assert(this->__get(cur->op1, &temp1) == 0);                               \
        assert(this->__get(cur->op2, &temp2) == 0);                               \
        if (temp1.is_null || temp2.is_null)                                       \
        {                                                                         \
            val_t temp;                                                           \
            assert(this->__set(cur->f, temp, wt_flag) == 0);                      \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            CH2VAR_T(temp, type, sizeof(type), TYPE_OP(cmd, type, temp1, temp2)); \
            assert(this->__set(cur->f, temp, wt_flag) == 0);                      \
        }                                                                         \
    }

/**
 * @brief 指示符号的比较运算扩展，不使用指针。op1与op2使用 val_t
 * 有NULL型支持，不支持string
 */
#define FULL_CMP(cmd, type, wt_flag)                                                                           \
    {                                                                                                          \
        val_t temp1, temp2;                                                                                    \
        assert(this->__get(cur->op1, &temp1) == 0);                                                            \
        assert(this->__get(cur->op2, &temp2) == 0);                                                            \
        if (temp1.is_null || temp2.is_null)                                                                    \
        {                                                                                                      \
            assert(this->__set(cur->f, val_t(), wt_flag) == 0);                                                \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
            assert(this->__set(cur->f, val_t((*(type *)temp1.pdata)cmd(*(type *)temp2.pdata)), wt_flag) == 0); \
        }                                                                                                      \
    }
/**
 * @brief 指示符号的比较运算扩展，不使用指针。op1与op2使用 val_t
 * 有NULL型支持，不支持string
 */
#define FULL_CMP(cmd, type, wt_flag)                                                                           \
    {                                                                                                          \
        val_t temp1, temp2;                                                                                    \
        assert(this->__get(cur->op1, &temp1) == 0);                                                            \
        assert(this->__get(cur->op2, &temp2) == 0);                                                            \
        if (temp1.is_null || temp2.is_null)                                                                    \
        {                                                                                                      \
            assert(this->__set(cur->f, val_t(), wt_flag) == 0);                                                \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
            assert(this->__set(cur->f, val_t((*(type *)temp1.pdata)cmd(*(type *)temp2.pdata)), wt_flag) == 0); \
        }                                                                                                      \
    }
/**
 * @brief 将一个类型为 t的变量转为可被变量表存储的val_t值
 *
 * @tparam t 类型参数
 * @param val 要存储的值
 * @return exec::val_t 转换后的结构体
 */
template <typename t>
exec::val_t __create(t val)
{
    exec::val_t ret;
    ret.size = sizeof(t);
    ret.is_null = false;
    ret.pdata = malloc(ret.size);
    *((t *)ret.pdata) = val;
    return ret;
}

exec::exec_t::exec_t(const program_t &cmds,
                     const index_t variable_num,
                     const data_table_t &constant_table,
                     const std::vector<int> *const str_len_limit)
{
    this->_reinit(cmds, variable_num, constant_table, str_len_limit);
}
exec::exec_t::exec_t()
{
    this->exit();
}

exec::exec_t::exec_t(const exec_t &old)
{
    this->exit();
    if (old.__constance_len)
    {
        const data_table_t::const_iterator first = old.__var_table.begin();
        const data_table_t::const_iterator second = first + (old.__constance_len);
        this->_reinit(
            old.__prog,
            old.__var_table.size() - old.__constance_len,
            data_table_t(first, second),
            &old.__str_len_limit);
    }
    else
        this->_reinit(
            old.__prog,
            old.__var_table.size(),
            data_table_t(),
            &old.__str_len_limit);
}
exec::exec_t &exec::exec_t::operator=(const exec_t &old)
{
    this->exit();
    if (old.__constance_len)
    {
        const data_table_t::const_iterator first = old.__var_table.begin();
        const data_table_t::const_iterator second = first + (old.__constance_len);
        this->_reinit(
            old.__prog,
            old.__var_table.size() - old.__constance_len,
            data_table_t(first, second),
            &old.__str_len_limit);
    }
    else
        this->_reinit(
            old.__prog,
            old.__var_table.size(),
            data_table_t(),
            &old.__str_len_limit);
    return *this;
}

exec::exec_t::~exec_t()
{
    this->exit();
}
bool exec::exec_t::run(Table *data, bool *is_written)
{
    if (this->__state != __state_t::ok)
        return false;
    this->__cur_data_ptr = data;
    int pc = -1;
    while (true)
    {
        ++pc;
        if (pc >= this->__prog.size())
            return false;
        cmd_t *cur = &(this->__prog[pc]);
        switch (cur->cmd)
        {
        // 空指令
        case code_exec_t::HLT:
            break;
        // 类型转换
        case code_exec_t::I2I64:
        {
            CH_TYPE(int, cur->op1, int64_t, cur->f, is_written);
            break;
        }
        case code_exec_t::I2F:
        {
            CH_TYPE(int, cur->op1, float, cur->f, is_written);
            break;
        }
        case code_exec_t::I2LF:
        {
            CH_TYPE(int, cur->op1, double, cur->f, is_written);
            break;
        }
        case code_exec_t::I2S:
        {
            CH_2STR(int, cur->op1, cur->f, is_written);
            break;
        }
        case code_exec_t::I2B:
        {
            CH_2BOOL(int, cur->op1, cur->f, is_written);
            break;
        }
        case code_exec_t::I642I:
        {
            CH_TYPE(int64_t, cur->op1, int, cur->f, is_written);
            break;
        }
        case code_exec_t::I642F:
        {
            CH_TYPE(int64_t, cur->op1, float, cur->f, is_written);
            break;
        }
        case code_exec_t::I642LF:
        {
            CH_TYPE(int64_t, cur->op1, double, cur->f, is_written);
            break;
        }
        case code_exec_t::I642S:
        {
            CH_2STR(int64_t, cur->op1, cur->f, is_written);
            break;
        }
        case code_exec_t::I642B:
        {
            CH_2BOOL(int64_t, cur->op1, cur->f, is_written);
            break;
        }
        case code_exec_t::F2I:
        {
            CH_TYPE(float, cur->op1, int, cur->f, is_written);
            break;
        }
        case code_exec_t::F2I64:
        {
            CH_TYPE(float, cur->op1, int64_t, cur->f, is_written);
            break;
        }
        case code_exec_t::F2LF:
        {
            CH_TYPE(float, cur->op1, double, cur->f, is_written);
            break;
        }
        case code_exec_t::F2S:
        {
            CH_2STR(float, cur->op1, cur->f, is_written);
            break;
        }
        case code_exec_t::F2B:
        {
            CH_2BOOL(float, cur->op1, cur->f, is_written);
            break;
        }
        case code_exec_t::LF2I:
        {
            CH_TYPE(double, cur->op1, int, cur->f, is_written);
            break;
        }
        case code_exec_t::LF2I64:
        {
            CH_TYPE(double, cur->op1, int64_t, cur->f, is_written);
            break;
        }
        case code_exec_t::LF2F:
        {
            CH_TYPE(double, cur->op1, float, cur->f, is_written);
            break;
        }
        case code_exec_t::LF2S:
        {
            CH_2STR(double, cur->op1, cur->f, is_written);
            break;
        }
        case code_exec_t::LF2B:
        {
            CH_2BOOL(double, cur->op1, cur->f, is_written);
            break;
        }
        // 加法
        case code_exec_t::ADDI:
        {
            FULL_OP(+, int, is_written);
            break;
        }
        case code_exec_t::ADDI64:
        {
            FULL_OP(+, int64_t, is_written);
            break;
        }
        case code_exec_t::ADDF:
        {
            FULL_OP(+, float, is_written);
            break;
        }
        case code_exec_t::ADDLF:
        {
            FULL_OP(+, double, is_written);
            break;
        }
        case code_exec_t::ADDS:
        {
            val_t temp1, temp2;
            assert(this->__get(cur->op1, &temp1) == 0);
            assert(this->__get(cur->op2, &temp2) == 0);
            val_t temp;
            if (!(temp1.is_null || temp2.is_null))
            {
                temp.is_null = false;
                std::string ts = std::string((char *)temp1.pdata) + std::string((char *)temp2.pdata);
                temp.size = ts.length() + 1;
                temp.pdata = malloc(temp.size);
                memcpy(temp.pdata, (void *)(ts.c_str()), temp.size);
            }
            assert(this->__set(cur->f, temp, is_written) == 0);
            break;
        }
        case code_exec_t::SUBI:
        {
            FULL_OP(-, int, is_written);
            break;
        }
        case code_exec_t::SUBI64:
        {
            FULL_OP(-, int64_t, is_written);
            break;
        }
        case code_exec_t::SUBF:
        {
            FULL_OP(-, float, is_written);
            break;
        }
        case code_exec_t::SUBLF:
        {
            FULL_OP(-, double, is_written);
            break;
        }
        case code_exec_t::MTPI:
        {
            FULL_OP(*, int, is_written);
            break;
        }
        case code_exec_t::MTPI64:
        {
            FULL_OP(*, int64_t, is_written);
            break;
        }
        case code_exec_t::MTPF:
        {
            FULL_OP(*, float, is_written);
            break;
        }
        case code_exec_t::MTPLF:
        {
            FULL_OP(*, double, is_written);
            break;
        }
        case code_exec_t::DIVI:
        {
            FULL_OP(/, int, is_written);
            break;
        }
        case code_exec_t::DIVI64:
        {
            FULL_OP(/, int64_t, is_written);
            break;
        }
        case code_exec_t::DIVF:
        {
            FULL_OP(/, float, is_written);
            break;
        }
        case code_exec_t::DIVLF:
        {
            FULL_OP(/, double, is_written);
            break;
        }
        case code_exec_t::EQI:
        {
            FULL_CMP(==, int, is_written);
            break;
        }
        case code_exec_t::EQI64:
        {
            FULL_CMP(==, int64_t, is_written);
            break;
        }
        case code_exec_t::EQF:
        {
            FULL_CMP(==, float, is_written);
            break;
        }
        case code_exec_t::EQLF:
        {
            FULL_CMP(==, double, is_written);
            break;
        }
        case code_exec_t::EQS:
        {
            val_t temp1, temp2;
            assert(this->__get(cur->op1, &temp1) == 0);
            assert(this->__get(cur->op2, &temp2) == 0);
            if (temp1.is_null || temp2.is_null)
            {
                val_t temp;
                assert(this->__set(cur->f, temp, is_written) == 0);
            }
            {
                bool ts = std::string((char *)temp1.pdata) == std::string((char *)temp2.pdata);
                val_t temp(ts);
                assert(this->__set(cur->f, temp, is_written) == 0);
            }
            break;
        }
        case code_exec_t::EQB:
        {
            FULL_CMP(==, bool, is_written);
            break;
        }
        case code_exec_t::LTI:
        {
            FULL_CMP(>, int, is_written);
            break;
        }
        case code_exec_t::LTI64:
        {
            FULL_CMP(>, int64_t, is_written);
            break;
        }
        case code_exec_t::LTF:
        {
            FULL_CMP(>, float, is_written);
            break;
        }
        case code_exec_t::LTLF:
        {
            FULL_CMP(>, double, is_written);
            break;
        }
        case code_exec_t::STI:
        {
            FULL_CMP(<, int, is_written);
            break;
        }
        case code_exec_t::STI64:
        {
            FULL_CMP(<, int64_t, is_written);
            break;
        }
        case code_exec_t::STF:
        {
            FULL_CMP(<, float, is_written);
            break;
        }
        case code_exec_t::STLF:
        {
            FULL_CMP(<, double, is_written);
            break;
        }
        case code_exec_t::NOT:
        {
            val_t temp;
            assert(this->__get(cur->op1, &temp) == 0);
            if (temp.is_null)
                assert(this->__set(cur->f, val_t(), is_written) == 0);
            else
            {
                if (*((bool *)temp.pdata))
                    assert(this->__set(cur->f, val_t(false), is_written) == 0);
                else
                    assert(this->__set(cur->f, val_t(true), is_written) == 0);
            }
            break;
        }
        case code_exec_t::AND:
        {
            val_t temp1, temp2;
            assert(this->__get(cur->op1, &temp1) == 0);
            assert(this->__get(cur->op2, &temp2) == 0);
            CH2VAR_T(temp, bool, sizeof(bool), TYPE_OP(&&, bool, temp1, temp2));
            // 如果temp为 NULL 说明至少存在一个 NULL 操作数，检查是否
            // temp1 为 false 或 temp2 为 false，可合并为false
            if ((temp.is_null) && ((!(temp1.is_null || (*((bool *)temp1.pdata)))) || (!(temp2.is_null || (*((bool *)temp2.pdata))))))
                assert(this->__set(cur->f, val_t(false), is_written) == 0);
            else
                assert(this->__set(cur->f, temp, is_written) == 0);
            break;
        }
        case code_exec_t::OR:
        {
            val_t temp1, temp2;
            assert(this->__get(cur->op1, &temp1) == 0);
            assert(this->__get(cur->op2, &temp2) == 0);
            CH2VAR_T(temp, bool, sizeof(bool), TYPE_OP(&&, bool, temp1, temp2));
            // 如果temp为 NULL 说明至少存在一个 NULL 操作数，检查是否
            // temp1 为 true 或 temp2 为 true，可合并为true
            if ((temp.is_null) &&
                ((((!temp1.is_null) && (*((bool *)temp1.pdata)))) || (((!temp2.is_null) && (*((bool *)temp2.pdata))))))
                assert(this->__set(cur->f, val_t(true), is_written) == 0);
            else
                assert(this->__set(cur->f, temp, is_written) == 0);
            break;
        }
        case code_exec_t::JT:
        {
            val_t temp;
            assert(this->__get(cur->op1, &temp) == 0);
            if (temp.is_null)
                break;
            else if (*((bool *)(temp.pdata)))
                pc = pc + cur->f - 1;
            break;
        }
        case code_exec_t::JF:
        {
            val_t temp;
            assert(this->__get(cur->op1, &temp) == 0);
            if (temp.is_null)
                break;
            else if (!*((bool *)(temp.pdata)))
                pc = pc + cur->f - 1;
            break;
        }
        case code_exec_t::J:
        {
            pc = pc + cur->f - 1;
            break;
        }
        case code_exec_t::RTT:
        {
            this->__cur_data_ptr = NULL;
            return true;
        }
        break;
        case code_exec_t::RTF:
        {
            this->__cur_data_ptr = NULL;
            return false;
        }
        break;
        case code_exec_t::RTIF:
        {
            val_t temp;
            assert(this->__get(cur->op1, &temp) == 0);
            if (temp.is_null)
            {
                this->__cur_data_ptr = NULL;
                return false;
            }
            else
            {
                this->__cur_data_ptr = NULL;
                return *((bool *)(temp.pdata));
            }
            break;
        }
        case code_exec_t::IFN:
        {
            val_t temp;
            assert(this->__get(cur->op1, &temp) == 0);
            if (temp.is_null)
                assert(this->__set(cur->f, val_t(true), is_written) == 0);
            else
                assert(this->__set(cur->f, val_t(true), is_written) == 0);
            break;
        }
        case code_exec_t::CPY:
        {
            val_t temp;
            assert(this->__get(cur->op1, &temp) == 0);
            assert(this->__set(cur->f, temp, is_written) == 0);
            break;
        }
        case code_exec_t::SETB:
        {
            assert((cur->op1) >= 0);
            this->__base_addr = cur->op1;
            break;
        }
        default:
            assert("not runnable code" && 0);
            break;
        }
    }
}

void exec::exec_t::exit()
{
    // 修改状态量
    this->__state = __state_t::zombie;
    __base_addr = 0;
    this->__cur_data_ptr = NULL;
    this->__constance_len = 0;
    // 清理代码空间
    this->__prog.clear();
    // 深度清理（通过swap交换空间）
    {
        program_t temp;
        temp.swap(this->__prog);
    }
    // 变量、常量空间清理比较麻烦
    // 首先释放所有malloc
    {
        int len = this->__var_table.size();
        for (int i = 0; i < len; ++i)
        {
            if (this->__var_table[i].pdata)
            {
                free(this->__var_table[i].pdata);
                this->__var_table[i].pdata = NULL;
            }
        }
    }
    // 同样，进行清空和深度清理
    this->__var_table.clear();
    {
        data_table_t temp;
        temp.swap(this->__var_table);
    }
}

bool exec::exec_t::is_ok()
{
    return this->__state == __state_t::ok;
}

void exec::exec_t::_reinit(const program_t &cmds,
                           const index_t variable_num,
                           const data_table_t &constant_table,
                           const std::vector<int> *const str_len_limit)
{
    // 先进行资源释放
    this->exit();
    // 设置为初始化中
    this->__state = __state_t::init;
    __base_addr = 0;
    this->__cur_data_ptr = NULL;
    this->__prog.clear();
    this->__prog = cmds;
    // 拷贝状态
    this->__constance_len = constant_table.size();
    int len = variable_num + this->__constance_len;
    this->__var_table.resize(len);
    {
        // 拷贝常量表至对象内
        for (int i = 0; i < this->__constance_len; ++i)
        {
            this->__var_table[i].is_null = constant_table[i].is_null;
            this->__var_table[i].size = constant_table[i].size;
            if (this->__var_table[i].is_null)
                this->__var_table[i].pdata = NULL;
            else
            {
                // 拷贝变量内容
                this->__var_table[i].pdata = malloc(this->__var_table[i].size);
                char *to = (char *)this->__var_table[i].pdata,
                     *from = (char *)constant_table[i].pdata;
                for (int j = 0; j < this->__var_table[i].size; ++j)
                    to[j] = from[j];
            }
        }
        // 初始化变量空间
        for (int i = this->__constance_len; i < len; ++i)
        {
            this->__var_table[i].is_null = true;
            this->__var_table[i].size = 0;
            this->__var_table[i].pdata = NULL;
        }
    }
    // 复制字符串保护信息
    if (str_len_limit)
    {
        this->__str_len_limit = *str_len_limit;
    }

    this->__state = __state_t::ok;
}

int64_t exec::exec_t::__get(index_t index, val_t *val)
{
    // 访问 Tuple 空间
    if (index >= 0)
    {
        // 释放目标结构的内存
        if (val->pdata)
            free(val->pdata);
        uint64_t sz = this->__cur_data_ptr->get_real_size(index);
        void *tempp = malloc(sz);
        val->pdata = NULL;
        val->size = 0;
        val->is_null = true;
        ::Value tempv{.value = (uint64_t)tempp};
        this->__cur_data_ptr->read_i(index, &tempv);
        // 判 NULL
        if (tempv.null)
        {
            free(tempp);
        }
        else
        {
            val->pdata = tempp;
            val->size = sz;
            val->is_null = false;
            // 如果.value不同了说明复制的是常规类型，此时tempp为空需要写入
            if (tempv.value != (uint64_t)tempp)
            {
                memcpy(tempp, &tempv.value, sz);
            }
        }
        return 0;
    }
    // 访问变量空间
    else
    {
        // 越界检查
        index = -1 - index;
        index += this->__base_addr;
        if (index >= this->__var_table.size())
            return -1;
        // 无复制写回
        *val = this->__var_table[index];
        return 0;
    }
}
int64_t exec::exec_t::__set(index_t index, const val_t& val, bool *written_flag)
{
    // 访问 Tuple 空间
    if (index >= 0)
    {
        ::Value tempv;
        if(val.is_null || (val.pdata == NULL))
            tempv.null = true;
        else{
            // 如果val大于8必定为char，否则需要读取类型
            bool converse = false;
            if(val.size<8)
            {
                Type t = this->__cur_data_ptr->get_type_list()[index];
                if(t.type != ::Data_type::CHAR)
                    converse = true;
            }
            if(converse)
                memcpy(&tempv.value,val.pdata,val.size);
            else
                tempv.value = (uint64_t)val.pdata;
        }
        this->__cur_data_ptr->write_i(index,tempv);
        // 更新写入标记
        if (written_flag)
            *written_flag = true;
        return 0;
    }
    // 访问变量空间
    else
    {
        // 越界检查
        index = -1 - index;
        index += this->__base_addr;
        if (index >= this->__var_table.size())
            return -1;
        // 复制
        this->__var_table[index].is_null = val.is_null;
        // NULL型简单处理
        if (val.is_null)
        {
            if (this->__var_table[index].pdata)
                free(this->__var_table[index].pdata);
            this->__var_table[index].pdata = NULL;
            this->__var_table[index].size = 0;
        }
        // 非 NULL 需要考虑PDATA情况
        else
        {
            // 判断内存大小是否兼容（新 <= 旧），是则复制即可，否则触发free - malloc
            if (this->__var_table[index].size < val.size)
            {
                if (this->__var_table[index].pdata)
                    free(this->__var_table[index].pdata);
                this->__var_table[index].pdata = malloc(val.size);
            }
            this->__var_table[index].size = val.size;
            // 拷贝变量内容
            char *to = (char *)this->__var_table[index].pdata,
                 *from = (char *)val.pdata;
            for (int j = 0; j < val.size; ++j)
                to[j] = from[j];
        }
        return 0;
    }
}