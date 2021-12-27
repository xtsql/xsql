/**
 * @file xsql_keywords.hpp
 * @author your name (you@domain.com)
 * @brief 编译所指导的字母类关键字，已按ascii值升序排列
 * @version 0.1
 * @date 2021-12-04
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once
#ifndef _xsql_keywords_hpp_
#define _xsql_keywords_hpp_

#include <unordered_map>
#include "xsql_compile_typedef.hpp"

#define IN(a, left, right) ((a >= left) && (a <= right))

// 字符类型
// 0 空格
// 1 数字
// 2 符号
// 3 字母
// 4 下划线
#define CHAR_TYPE(a) ((a <= 32) ? 0 : (IN(a, 48, 57) ? 1 : ((IN(a, 65, 90) || IN(a, 97, 122) || (a > 127)) ? 3 : ((a == '_') ? 4 : 2))))

namespace cp
{
    /**
     * @brief 字母型保留字到关键字id的映射表。
     * 如果不是关键字，则得到 kid_t::_not_found
     *
     * @param str 字符串指针地址
     * @param len 字符串长度（不包括\0）
     * @return kid_t 查询到的id
     */
    cp::kid_t is_key(const char *str, const int len);

    void keywords_init();
}
#endif //!_xsql_keywords_hpp_