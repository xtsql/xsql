/**
 * @file xsql_mor_anls.cpp
 * @author your name (you@domain.com)
 * @brief 实现了词法分析功能
 * @version 0.1
 * @date 2021-12-05
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <unordered_map>
#include "xsql_keywords.hpp"
#include "xsql_mor_anls.hpp"

using cp::err_code;
using cp::kid_t;
using cp::token_t;
using std::string;
using std::unordered_map;

typedef unordered_map<string, int32_t> strid_map;
typedef std::pair<string, int32_t> strid_map_pair;
typedef strid_map::iterator strid_map_it;

// std::vector<cp::err_detail_t> all_error_happened;

void error_handle(cp::err_detail_t info)
{
#if TRY_CONTINUE_AFTER_ONE_ERROR
    // all_error_happened.push_back(info);
#else
    throw info;
#endif // TRY_CONTINUE_AFTER_ONE_ERROR
}

void error_handle(cp::err_code code, std::string info = "No detail")
{
    cp::err_detail_t detail(code, info);
#if TRY_CONTINUE_AFTER_ONE_ERROR
    // all_error_happened.push_back(detail);
#else
    throw info;
#endif // TRY_CONTINUE_AFTER_ONE_ERROR
}

/**
 * @brief 检查 val 是否在字符串 cp 中，是则返回true
 *
 * @param val 要检查的字符
 * @param cp 目标字符串
 * @return true 字符串中找到此字符
 * @return false 字符串中找不到此字符
 */
bool str_in(const char val, const char *cp)
{
    while (*cp != 0)
    {
        if ((*cp) == val)
            return true;
        ++cp;
    }
    return false;
}

/**
 * @brief 判断接下来的一个字符串是否是保留字或标识符
 *
 * @param str_start 该字符串起始位置
 * @param len_play_back 成功时返回被识别到的字符串长度，失败时返回错误发生时已移进距离
 * @param str_play_back 成功时返回被识别到的字符串，失败时返回空串
 * @return kid_t 成功时返回识别成功的类，失败时返回kid_t::_not_found
 */
kid_t mor_is_id(const char *str_start, int32_t *len_play_back, string *str_play_back)
{
    bool need_to_check = (*str_start) != '_', conti = true;
    *len_play_back = 1;
    *str_play_back = *str_start;
    ++str_start;
    while (conti)
    {
        int next_type = CHAR_TYPE((*str_start));
        switch (next_type)
        {
            // 空格和其他符号都认为是变量名结束
        case 0:
        case 2:
            conti = false;
            break;
            // 下划线和数字出现说明不再是保留字
        case 1:
        case 4:
            need_to_check = false;
            // 统一处理
        case 3:
            *str_play_back += *str_start;
            ++str_start;
            ++(*len_play_back);
            break;
        default:
        {
            error_handle(cp::err_code::illegal_id, std::string("cannot identify character ") + *str_start);
            break;
        }
        }
    }
    // 得出结果，检查保留字
    if (need_to_check)
    {
        kid_t rst = cp::is_key(str_play_back->c_str(), str_play_back->length());
        if (rst != kid_t::_not_found)
        {
            str_play_back->clear();
            return rst;
        }
    }
    // 不是保留字，全员转小写
    const static char diff = 'a' - 'A';
    for (int i = 0; i < str_play_back->length(); ++i)
        if (IN((*str_play_back)[i], 'A', 'Z'))
            (*str_play_back)[i] += diff;
    return kid_t::typeWord;
}

/**
 * @brief 判断接下来的一个字符串是否是符号。注意，负数的负号也会被识别为减号，在后续文法识别过程中消除
 *
 * @param str_start 该字符串起始位置
 * @param len_play_back 成功时返回被识别到的字符串长度，失败时返回错误发生时已移进距离
 * @return kid_t 成功时返回识别成功的类，失败时返回kid_t::_not_found
 */
kid_t mor_is_symbol(const char *str_start, int32_t *len_play_back)
{
    // 必单的符号共有 ()+-*/.,=;
    // 必双的符号有 != !> !<
    // 可单可双  的就是 < <= <>    > >=
    *len_play_back = 1;
    switch (*str_start)
    {
    case '(':
        return kid_t::Left_br;
        break;
    case ')':
        return kid_t::Right_br;
        break;
    case '+':
        return kid_t::opAdd;
        break;
    case '-':
        return kid_t::opSub;
        break;
    case '*':
        return kid_t::Star;
        break;
    case '/':
        return kid_t::opDiv;
        break;
    case '.':
        return kid_t::Dot;
        break;
    case ',':
        return kid_t::Sep;
        break;
    case '=':
        return kid_t::Equal;
        break;
    case ';':
        return kid_t::End;
        break;
    case '!':
    {
        ++str_start;
        switch (*str_start)
        {
        case '=':
            *len_play_back = 2;
            return kid_t::Not_equal;
            break;
        case '<':
            *len_play_back = 2;
            return kid_t::No_Smaller;
            break;
        case '>':
            *len_play_back = 2;
            return kid_t::No_larger;
            break;
        default:
        {
            error_handle(cp::err_code::illegal_id, " ! cannot exist alone");
            break;
        }
        }
        break;
    }
    case '<':
    {
        ++str_start;
        switch (*str_start)
        {
        case '=':
            *len_play_back = 2;
            return kid_t::No_larger;
            break;
        case '>':
            *len_play_back = 2;
            return kid_t::Not_equal;
            break;
        default:
            return kid_t::Smaller;
            break;
        }
        break;
    }
    case '>':
    {
        ++str_start;
        if ((*str_start) == '=')
        {
            *len_play_back = 2;
            return kid_t::No_Smaller;
        }
        else
            return kid_t::Larger;
        break;
    }
    default:
    {
        error_handle(cp::err_code::illegal_id, std::string("illegal symbol ") + *str_start);
        break;
    }
    }
    return kid_t::_not_found;
}

/**
 * @brief 以引号扩出的字符的方式读取，同时会进行转码。请确保开始字符是 ' 或 "
 *
 * @param str_start 该字符串起始位置
 * @param len_play_back 成功时返回被识源字符串指针的移动距离（源串长度），失败时返回错误发生时已移进距离
 * @param str_play_back 成功时返回被识别到的字符串，失败时返回空串
 * @return kid_t 成功时返回识别成功的类（kid_t::typeText），失败时返回kid_t::_not_found
 */
kid_t mor_read_text(const char *str_start, int32_t *len_play_back, string *str_play_back)
{
    char start_with = *str_start;
    *len_play_back = 1;
    *str_play_back = "";
    ++str_start;
    if (str_in(start_with, "'\""))
    {
        while ((*str_start) != start_with)
        {
            // 译码
            if ((*str_start) == '\\')
            {
                switch (*(str_start + 1))
                {
                case '\\':
                    *str_play_back += '\\';
                    break;
                case '\'':
                    *str_play_back += '\'';
                    break;
                case '\"':
                    *str_play_back += '\"';
                    break;
                case 'n':
                    *str_play_back += '\n';
                    break;
                case 't':
                    *str_play_back += '\t';
                    break;
                case 'r':
                    *str_play_back += '\r';
                    break;
                default:
                    str_play_back->append(str_start, 2);
                    break;
                }
                *len_play_back += 2;
                str_start += 2;
                continue;
            }
            // 无需译码
            *str_play_back += *str_start;
            ++str_start;
            ++(*len_play_back);
        }
        // 读取结束
        ++(*len_play_back);
        return kid_t::typeText;
    }
    else
        return kid_t::_not_found;
}
/**
 * @brief 判断接下来的一个字符串是否是整型或浮点。注意，负数的负号也会被识别为减号，在后续文法识别过程中消除
 *
 * @param str_start 该字符串起始位置
 * @param len_play_back 成功时返回被识源字符串指针的移动距离（源串长度），失败时返回错误发生时已移进距离
 * @param str_play_back 成功时返回被识别到的字符串，失败时返回空串
 * @return kid_t 成功时返回识别成功的类，失败时返回kid_t::_not_found
 */
kid_t mor_is_number(const char *str_start, int32_t *len_play_back, string *str_play_back)
{
    *len_play_back = 0;
    *str_play_back = "";
    // 允许有数位空白
    while (*str_start == ' ')
    {
        ++str_start;
        ++(*len_play_back);
    }
    // 存在数字标记
    bool has_num = false;
    // 整数部分判别
    while (IN((*str_start), '0', '9'))
    {
        *str_play_back += *str_start;
        ++str_start;
        ++(*len_play_back);
        has_num = true;
    }
    // 小数点标记
    bool pointed = false;
    if (*str_start == '.')
    {
        ++str_start;
        ++(*len_play_back);
        *str_play_back += '.';
        pointed = true;
        // 小数部分判别
        while (IN((*str_start), '0', '9'))
        {
            *str_play_back += *str_start;
            ++str_start;
            ++(*len_play_back);
            has_num = true;
        }
    }
    // 以上便完成了字符的判定。检查是否有数字被录入，如果没有则应当报错
    if (!has_num)
    {
        str_play_back->clear();
        return kid_t::_not_found;
    }
    // 检查后跟字符是否为空格或合适的运算符
    if (str_in(*str_start, " +-*/=!<>),"))
    {
        if (pointed)
            return kid_t::typeFloat;
        else
            return kid_t::typeInt;
    }
    str_play_back->clear();
    return kid_t::_not_found;
}

err_code cp::mor_analysis(const char *str, std::vector<cp::token_t> &code_list, std::vector<std::string> &rst_map, int str_len)
{
    if (str_len == 0)
        return err_code::no_str;
    // 创建临时变量
    std::vector<cp::token_t> _temp_list;
    // std::list<cp::token_t> _temp_list;
    strid_map _usr_strs;
    _usr_strs.insert(strid_map_pair("", 0));
    // 准备遍历
    int position = 0;
    while (position < str_len)
    {
        switch (CHAR_TYPE(str[position]))
        {
        // 空格
        case 0:
            ++position;
            break;
        // 数字
        case 1:
        {
            int sub_strlen = 0;
            string sts;
            kid_t rst = mor_is_number(str + position, &sub_strlen, &sts);
            position += sub_strlen;
            // 识别成功
            if (kid_t::_not_found != rst)
            {
                // 创建token串
                token_t cur(rst, -1);
                // 查询是否已存在
                strid_map_it it = _usr_strs.find(sts);
                if (it != _usr_strs.end())
                {
                    // 已存在，使用现有作为常数
                    cur.index = it->second;
                }
                else
                {
                    // 不存在，存入并作为新的
                    int32_t index = _usr_strs.size();
                    _usr_strs.insert(strid_map_pair(sts, index));
                    cur.index = index;
                }
                _temp_list.push_back(cur);
            }
            // 识别失败，属于非法标识符
            else
            {
                error_handle(cp::err_code::illegal_id, "illegal number");
            }
            break;
        }
        // 符号
        // ' 和 " 会进入字符串判定
        // 将 . 优先重定向到数字判定，如果数字判定失败再作为字符处理
        case 2:
        {
            int sub_strlen = 0;
            if (str[position] == '.')
            {
                string sts;
                kid_t rst = mor_is_number(str + position, &sub_strlen, &sts);
                // 识别成功
                if (kid_t::_not_found != rst)
                {
                    position += sub_strlen;
                    // 创建token串
                    token_t cur(rst, -1);
                    // 查询是否已存在
                    strid_map_it it = _usr_strs.find(sts);
                    if (it != _usr_strs.end())
                    {
                        // 已存在，使用现有作为常数
                        cur.index = it->second;
                    }
                    else
                    {
                        // 不存在，存入并作为新的
                        int32_t index = _usr_strs.size();
                        _usr_strs.insert(strid_map_pair(sts, index));
                        cur.index = index;
                    }
                    _temp_list.push_back(cur);
                    break;
                }
                // 识别失败，按其他符号的模式处理
            }
            // 进入字符串判定模式
            if (str_in(str[position], "'\""))
            {
                string sbs;
                kid_t rst = mor_read_text(str + position, &sub_strlen, &sbs);
                position += sub_strlen;
                // 识别成功
                if (kid_t::_not_found != rst)
                {
                    // 创建token串
                    token_t cur(rst, -1);
                    // 查询是否已存在
                    strid_map_it it = _usr_strs.find(sbs);
                    if (it != _usr_strs.end())
                    {
                        // 已存在，使用现有作为常数
                        cur.index = it->second;
                    }
                    else
                    {
                        // 不存在，存入并作为新的
                        int32_t index = _usr_strs.size();
                        _usr_strs.insert(strid_map_pair(sbs, index));
                        cur.index = index;
                    }
                    _temp_list.push_back(cur);
                }
                // 识别失败，属于非法标识符
                else
                {
                    error_handle(cp::err_code::illegal_id, "illegal symbol");
                }
                break;
            }
            // 其他常规符号
            kid_t rst = mor_is_symbol(str + position, &sub_strlen);
            position += sub_strlen;
            // 识别成功
            if (kid_t::_not_found != rst)
            {
                // 创建token串
                token_t cur(rst, 0);
                _temp_list.push_back(cur);
            }
            else
            {
                error_handle(cp::err_code::illegal_id, "illegal symbol");
            }
            break;
        }
        // 字母，重定向到关键字/标识符判定
        case 3:
        // 下划线，重定向到标识符判定
        case 4:
        {
            int sub_strlen = 0;
            string sts;
            kid_t rst = mor_is_id(str + position, &sub_strlen, &sts);
            position += sub_strlen;
            // 识别成功
            if (kid_t::_not_found != rst)
            {
                // 是保留字
                if (rst != kid_t::typeWord)
                    _temp_list.push_back(token_t(rst, 0));
                // 是标识符
                else
                {
                    // 创建token串
                    token_t cur(rst, -1);
                    // 查询是否已存在
                    strid_map_it it = _usr_strs.find(sts);
                    if (it != _usr_strs.end())
                    {
                        // 已存在，使用现有作为常数
                        cur.index = it->second;
                    }
                    else
                    {
                        // 不存在，存入并作为新的
                        int32_t index = _usr_strs.size();
                        _usr_strs.insert(strid_map_pair(sts, index));
                        cur.index = index;
                    }
                    _temp_list.push_back(cur);
                }
            }
            // 识别失败，属于非法标识符
            else
            {
                error_handle(cp::err_code::illegal_id, "illegal identifier");
            }
            break;
        }
        default:
                error_handle(cp::err_code::illegal_id, "unexpected char");
            break;
        }
    }
    // 遍历结束，清空目标
    code_list.clear();
    rst_map.clear();
    // 写回目标
    code_list = _temp_list;
    rst_map.resize(_usr_strs.size());
    for (strid_map_it it = _usr_strs.begin(); it != _usr_strs.end(); ++it)
        rst_map[it->second] = it->first;
    return err_code::success;
}