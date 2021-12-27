/**
 * @file xsql_commandEXEC.hpp
 * @author your name (you@domain.com)
 * @brief 本文件作为exec所依赖的代码，提供关于执行所需要的虚拟指令的定义
 * 所有指令定义为四元数格式(cmd, op1, op2, f)，但在指令名称上即对数据类型进行区别
 * 如果为二元运算，使用op1, op2
 * 如果为一元运算，使用op1
 * 存在少部分指令无需依赖op与f
 *
 * @version 0.9
 * @date 2021-12-06
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once
#ifndef _xsql_commandEXEC_hpp_
#define _xsql_commandEXEC_hpp_

namespace exec
{
    /**
     * @brief exec所使用的指令码（除编译器外，外部程序通常无需访问）
     * 根据四元数 (cmd,op1,op2,f)的描述，指令的通常格式是：
     * f := op1 cmd op2
     * 这里的op1与op2均指示为下标。非负时表示从Tuple下标索引，为负时表示从程序自身的常量、变量表中索引。
     */
    enum code_exec_t
    {
        HLT, // 空指令

        // 常规一元类型转换系列，只使用 OP1
        I2I64, // INT -> INT64
        I2F,   // INT -> float
        I2LF,  // INT -> double
        I2S,   // INT -> string
        I2B,   // INT -> BOOL

        I642I,  // INT64 -> INT
        I642F,  // INT64 -> float
        I642LF, // INT64 -> double
        I642S,  // INT64 -> string
        I642B,  // INT64 -> BOOL

        F2I,   // FLOAT -> INT
        F2I64, // FLOAT -> INT64
        F2LF,  // FLOAT -> double
        F2S,   // FLOAT -> string
        F2B,   // FLOAT -> BOOL

        LF2I,   // double -> INT
        LF2I64, // double -> INT64
        LF2F,   // double -> float
        LF2S,   // double -> string
        LF2B,   // double -> BOOL

        // 二元数字运算系列
        ADDI,   // INT32 加法
        ADDI64, // INT64 加法
        ADDF,   // FLOAT 加法
        ADDLF,  // DOUBLE 加法
        ADDS,   // STRING 加法
        // bool不支持加法

        SUBI,   // INT32 减法
        SUBI64, // INT64 减法
        SUBF,   // FLOAT 减法
        SUBLF,  // DOUBLE 减法
        // string, bool 不支持减法

        MTPI,   // INT32 乘法
        MTPI64, // INT64 乘法
        MTPF,   // FLOAT 乘法
        MTPLF,  // DOUBLE 乘法
        // string, bool 不支持乘法

        DIVI,   // INT32 除法
        DIVI64, // INT64 除法
        DIVF,   // FLOAT 除法
        DIVLF,  // DOUBLE 除法
        // string, bool 不支持除法

        // 布尔运算系列
        EQI,   // INT ==
        EQI64, // INT64 ==
        EQF,   // float ==
        EQLF,  // double ==
        EQS,   // string ==
        EQB,   // bool ==

        LTI,   // INT >
        LTI64, // INT64 >
        LTF,   // float >
        LTLF,  // double >
        // string, bool 不支持大小于

        STI,   // INT <
        STI64, // INT64 <
        STF,   // float <
        STLF,  // double <
        // string, bool 不支持大小于

        // <= >= != 使用NOT实现

        NOT, // !OP1
        AND, // OP1 && OP2
        OR,  // OP1 !! OP2
        // XOR, // OP1 ^ OP2

        // 比较及跳转系列
        // JEZ,        // OP1 = 0则跳转至序号f
        // JNZ,        // OP1 != 0则跳转至序号f
        JT, // OP1 == TRUE 则跳转至序号 pc + f（为 NULL/UNKNOWN 不跳转）
        JF, // OP1 == FALSE 则跳转至序号 pc + f，可能会很少使用（为 NULL/UNKNOWN 不跳转）
        // JLT,        // JUMP TO f IF OP1 > OP2
        // JNL,        // JUMP TO f IF OP1 <= OP2
        // JST,        // JUMP TO f IF OP1 < OP2
        // JNS,        // JUMP TO f IF OP1 >= OP2
        // JEQ,        // JUMP TO f IF OP1 == OP2
        // JNE,        // JUMP TO f IF OP1 != OP2

        // 无条件跳转系列
        J,    // JUMP TO pc + f
        RTT,  // RETURN TRUE
        RTF,  // RETURN FALSE
        RTIF, // RETURN TRUE IF OP1(bool type) IS TRUE

        // SQL语法涉及的特殊操作
        IFN, // f := OP1 IS NULL
        // ABS,        // F := ABS(OP1)

        // 其他操作
        CPY,   // COPY OP1 TO F
        SETB,  // SET BASE = OP1 ，设置内存空间的地址偏移（OP1为立即数）
    };
}

#endif //!_xsql_commandEXEC_hpp_