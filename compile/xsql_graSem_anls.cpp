/**
 * @file xsql_graSem_anls.cpp
 * @author your name (you@domain.com)
 * @brief 本文件实现语法及语义分析能力。可以直接由token串得到 sql_cmds_t 及 exec_t。
 * @version 0.1
 * @date 2021-12-06
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "xsql_graSem_anls.hpp"
#include "xsql_commandEXEC.hpp"
// #define _GS_DEBUG_ // 调试信息输出开关

/**
 * @brief 首个关键字为 CREATE ，进入 CREATE 语句分析环节
 *
 * @param global_code_list 传入 grasem_analysis 的 Token 串表
 * @param global_str_map 传入 grasem_analysis 的 字符串映射表
 * @param pos_play_back 从 grasem_analysis 获取的识别进度，识别结束（无论成功或失败）会得到新的识别起点位置
 * @param result 编译后的 SQL 程序
 * @return cp::err_code 错误码
 */
cp::err_code analysis_CREATE(
    const std::vector<cp::token_t> &global_code_list,
    const std::vector<std::string> &global_str_map,
    int64_t &pos_play_back,
    cp::sql_program_t &result)
{
    // 初始化
    result.execs.clear();
    result.sql_cmds.clear();
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Create)
        return cp::err_code::syntax_error;
    ++pos_play_back;

    // 判断是 table 还是 database
    // 第二位置只允许 database/table 关键字
    cp::sql_cmd_t this_cmd(cp::code_sql_t::no_op, NULL, NULL, NULL);
    switch (global_code_list[pos_play_back].key_id)
    {
    case cp::kid_t::Database:
        this_cmd.cmd = cp::code_sql_t::CMD_CREATED;
        break;
    case cp::kid_t::Table:
        this_cmd.cmd = cp::code_sql_t::CMD_CREATET;
        break;
    default:
        return cp::err_code::syntax_error;
        break;
    }
    ++pos_play_back;

    // 第三位置为 typeWord
    if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        return cp::err_code::syntax_error;
    // 复制字符串
    std::string *str1 = new std::string(global_str_map[global_code_list[pos_play_back].index]);
    // 查看第四位置
    ++pos_play_back;

    // 如果第四位置为分号则需要是 CREATED，为 . 则需要 this_cmd.cmd 是 CREATET
    // 是CREATET 的时候还需要 (
    switch (global_code_list[pos_play_back].key_id)
    {
        // ; 是 CREATED 则命令结束，否则报错
    case cp::kid_t::End:
    {
        if (this_cmd.cmd == cp::code_sql_t::CMD_CREATED)
        {
            // 保存命令
            ++pos_play_back;
            this_cmd.op1 = (void *)str1;
            result.sql_cmds.push_back(this_cmd);
            return cp::err_code::success;
        }
        else
        {
            delete str1;
            return cp::err_code::syntax_error;
        }
        break;
    }
    // . 可能是 create table db.t
    case cp::kid_t::Dot:
    {
        if (this_cmd.cmd != cp::code_sql_t::CMD_CREATET)
        {
            delete str1;
            return cp::err_code::syntax_error;
        }
        else
        {
            // 往下查看
            ++pos_play_back;
            // 期望找到 typeWord 作为表名
            if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
            {
                delete str1;
                return cp::err_code::syntax_error;
            }
            // str1 是库名，创建 str2 保存表名并写入指令中
            this_cmd.op3 = (void *)str1;
            std::string *str2 = new std::string(global_str_map[global_code_list[pos_play_back].index]);
            this_cmd.op1 = (void *)str2;
            // 查看左括号 (
            ++pos_play_back;
            if (global_code_list[pos_play_back].key_id != cp::kid_t::Left_br)
            {
                delete str1;
                delete str2;
                return cp::err_code::syntax_error;
            }
            else
                break;
        }
        break;
    }
    case cp::kid_t::Left_br:
        // 检查应该是 CREATET
        if (this_cmd.cmd != cp::code_sql_t::CMD_CREATET)
        {
            delete str1;
            return cp::err_code::syntax_error;
        }
        else
        {
            // str1 是 表名
            this_cmd.op1 = (void *)str1;
            break;
        }
    default:
        delete str1;
        return cp::err_code::syntax_error;
        break;
    }
    // 当前位置为列声明的左括号处，移进一位准备开始while循环
    ++pos_play_back;
    // 建立列对象准备存放
    cp::colum_infos_t *cols = new cp::colum_infos_t;
    this_cmd.op2 = cols;

    bool loop_condition = true, failed = false;
    while (loop_condition)
    {
        /* typeWord 类型关键字 【char型额外的 ( typeInt ) 】*/
        cp::colum_info_t single_col_info;

        // 检查列名合法
        if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        {
            loop_condition = false;
            failed = true;
            break;
        }
        // 当前位置为 列名 已被确认，复制，前进
        single_col_info.col_name = global_str_map[global_code_list[pos_play_back].index];
        ++pos_play_back;

        // 检查属性名称
        switch (global_code_list[pos_play_back].key_id)
        {
            // 几种基本类型比较简单，映射即可
        case cp::kid_t::Int:
            single_col_info.col_type.type = ::Data_type::INT;
            break;
        case cp::kid_t::Long:
            single_col_info.col_type.type = ::Data_type::LONG;
            break;
        case cp::kid_t::Float:
            single_col_info.col_type.type = ::Data_type::FLOAT;
            break;
        case cp::kid_t::Double:
            single_col_info.col_type.type = ::Data_type::DOUBLE;
            break;
        case cp::kid_t::Bool:
            single_col_info.col_type.type = ::Data_type::BOOLEAN;
            break;
            // CHAR 需要关心大小
        case cp::kid_t::Char:
        {
            single_col_info.col_type.type = ::Data_type::CHAR;
            // 准备识别 ( typeInt ) 结构，并让识别结束的光标位于 ) 处
            ++pos_play_back;
            if (global_code_list[pos_play_back].key_id != cp::kid_t::Left_br)
            {
                loop_condition = false;
                failed = true;
                break;
            }
            ++pos_play_back;
            if (global_code_list[pos_play_back].key_id != cp::kid_t::typeInt)
            {
                loop_condition = false;
                failed = true;
                break;
            }
            ++pos_play_back;
            if (global_code_list[pos_play_back].key_id != cp::kid_t::Right_br)
            {
                loop_condition = false;
                failed = true;
                break;
            }
            // 验证通过，读取光标左侧的 token (typeInt) 的字符串作为数据大小
            int64_t temp = atoll(global_str_map[global_code_list[pos_play_back - 1].index].c_str());
            if (temp <= 0)
            {
                loop_condition = false;
                failed = true;
                break;
            }
            else
                single_col_info.col_type.size = (uint64_t)temp;
            break;
        }
        default:
        {
            loop_condition = false;
            failed = true;
            break;
        }
        }
        // 到这里循环为假说明前面出错，退出到统一的错误处理
        if (!loop_condition)
            break;
        // 下一位应该是 , (继续) 或 ) (结束)。识别到这两个则光标前进，否则引导到出错
        ++pos_play_back;
        switch (global_code_list[pos_play_back].key_id)
        {
        case cp::kid_t::Right_br:
            loop_condition = false;
        case cp::kid_t::Sep:
            ++pos_play_back;
            cols->push_back(single_col_info);
            break;
            // 同样引导到出错处理
        default:
        {
            loop_condition = false;
            failed = true;
            break;
        }
        }
    } // 读取列属性 结束
    // 如果成功的话，此时的光标应该读取到 ; ，否则都处于错误处理流程
    if (failed || (global_code_list[pos_play_back].key_id != cp::kid_t::End))
    {
        // 统一的错误处理，释放指针
        delete ((std::string *)(this_cmd.op1));
        delete ((cp::colum_infos_t *)(this_cmd.op2));
        if (this_cmd.op3 != NULL)
            delete ((std::string *)(this_cmd.op3));
        return cp::err_code::syntax_error;
    }
    // 光标移进
    ++pos_play_back;
    // 命令写回
    result.sql_cmds.push_back(this_cmd);
    return cp::err_code::success;
}
/**
 * @brief 首个关键字为 DROP ，进入 DROP 语句分析环节
 *
 * @param global_code_list 传入 grasem_analysis 的 Token 串表
 * @param global_str_map 传入 grasem_analysis 的 字符串映射表
 * @param pos_play_back 从 grasem_analysis 获取的识别进度，识别结束（无论成功或失败）会得到新的识别起点位置
 * @param result 编译后的 SQL 程序
 * @return cp::err_code 错误码
 */
cp::err_code analysis_DROP(
    const std::vector<cp::token_t> &global_code_list,
    const std::vector<std::string> &global_str_map,
    int64_t &pos_play_back,
    cp::sql_program_t &result)
{
    // 支持：
    // drop database xxx
    // drop table ttt
    // drop table xxx.ttt

    // 初始化
    result.execs.clear();
    result.sql_cmds.clear();
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Drop)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 第二位置只允许 database/table 关键字
    cp::code_sql_t cs = cp::code_sql_t::no_op;
    switch (global_code_list[pos_play_back].key_id)
    {
    case cp::kid_t::Database:
        cs = cp::code_sql_t::CMD_DROPD;
        break;
    case cp::kid_t::Table:
        cs = cp::code_sql_t::CMD_DROPT;
        break;
    default:
        return cp::err_code::syntax_error;
        break;
    }
    ++pos_play_back;
    // 第三位置为 typeWord
    if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        return cp::err_code::syntax_error;
    // 复制字符串
    std::string *str1 = new std::string(global_str_map[global_code_list[pos_play_back].index]);
    // 查看第四位置
    ++pos_play_back;
    // 如果第四位置为分号则成功，为 . 则需要 cs 是dropt
    switch (global_code_list[pos_play_back].key_id)
    {
        // ; 命令结束
    case cp::kid_t::End:
    {
        // 保存命令
        ++pos_play_back;
        cp::sql_cmd_t cmd(cs, (void *)str1, NULL, NULL);
        result.sql_cmds.push_back(cmd);
        return cp::err_code::success;
        break;
    }
    // . 可能是 drop table db.t
    case cp::kid_t::Dot:
    {
        if (cs != cp::code_sql_t::CMD_DROPT)
        {
            delete str1;
            return cp::err_code::syntax_error;
        }
        else
        {
            // 往下查看
            ++pos_play_back;
            // 期望找到 typeWord 作为表名
            if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
            {
                delete str1;
                return cp::err_code::syntax_error;
            }
            // 查看分号
            ++pos_play_back;
            if (global_code_list[pos_play_back].key_id != cp::kid_t::End)
            {
                delete str1;
                return cp::err_code::syntax_error;
            }
            // 复制字符串
            std::string *str2 = new std::string(global_str_map[global_code_list[pos_play_back - 1].index]);
            // 保存命令
            cp::sql_cmd_t cmd(cs, (void *)str2, (void *)str1, NULL);
            result.sql_cmds.push_back(cmd);
            ++pos_play_back;
            return cp::err_code::success;
        }
        break;
    }
    default:
        delete str1;
        return cp::err_code::syntax_error;
        break;
    }
}

// cp::err_code analysis_subWHERE(
//     const std::vector<cp::token_t> &global_code_list,
//     const std::vector<std::string> &global_str_map,
//     const std::vector<std::string[2]> &db_table_name,
//     int64_t &pos_play_back,
//     exec::program_t &result)
// {
// }

/**
 * @brief 首个关键字为 SELECT ，进入 SELECT 语句分析环节
 *
 * @param global_code_list 传入 grasem_analysis 的 Token 串表
 * @param global_str_map 传入 grasem_analysis 的 字符串映射表
 * @param pos_play_back 从 grasem_analysis 获取的识别进度，识别结束（无论成功或失败）会得到新的识别起点位置
 * @param result 编译后的 SQL 程序
 * @return cp::err_code 错误码
 */
cp::err_code analysis_SELECT(
    const std::vector<cp::token_t> &global_code_list,
    const std::vector<std::string> &global_str_map,
    int64_t &pos_play_back,
    cp::sql_program_t &result)
{
    // 初始化
    result.execs.clear();
    result.sql_cmds.clear();
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Select)
        return cp::err_code::syntax_error;
    ++pos_play_back;

    // 记录被选中的列名
    struct selected_col
    {
        // std::string table_name;
        std::string origin_name;
        std::string renamed;
        int64_t origin_index;
        bool is_all;
        ::Type origin_type;
    };
    std::vector<selected_col> cols;
    while (true)
    {
        switch (global_code_list[pos_play_back].key_id)
        {
        case cp::kid_t::Star:
        {
            selected_col t = {.is_all = true};
            cols.push_back(t);
            break;
        }
        case cp::kid_t::typeWord:
        {
            selected_col t = {.is_all = false};
            t.origin_name = global_str_map[global_code_list[pos_play_back].index];
            // 如果下一位是 as 或者 typeWord 说明启用重命名
            bool rename = false;
            if (global_code_list[pos_play_back + 1].key_id == cp::kid_t::As)
            {
                ++pos_play_back;
                rename = true;
            }
            if (global_code_list[pos_play_back + 1].key_id == cp::kid_t::typeWord)
            {
                ++pos_play_back;
                t.renamed = global_str_map[global_code_list[pos_play_back].index];
            }
            // 如果出现 as 但是没有出现 typeWord
            else
            {
                if (rename)
                    return cp::err_code::syntax_error;
            }
            cols.push_back(t);
            break;
        }
        default:
            return cp::err_code::syntax_error;
            break;
        }
        ++pos_play_back;
        // 检查是不是 ,
        if (global_code_list[pos_play_back].key_id == cp::kid_t::Sep)
        {
            ++pos_play_back;
        }
        else
            break;
    }
    // 检查 from 和表名
    if (global_code_list[pos_play_back].key_id != cp::kid_t::From)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 库名 和 表名
    std::string db_name = "", tb_name = "";
    // dbname.tbname 或 tbname
    if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        return cp::err_code::syntax_error;
    tb_name = global_str_map[global_code_list[pos_play_back].index];
    ++pos_play_back;
    // 判 d.t逻辑
    if (global_code_list[pos_play_back].key_id == cp::kid_t::Dot)
    {
        ++pos_play_back;
        if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
            return cp::err_code::syntax_error;
        db_name = tb_name;
        tb_name = global_str_map[global_code_list[pos_play_back].index];
        ++pos_play_back;
    }
    // 判句末
    if (global_code_list[pos_play_back].key_id != cp::kid_t::End)
        return cp::err_code::syntax_error;
    // 开始检查列信息
    std::vector<std::string> actual_names;
    std::vector<::Type> actual_types;
    {
        ::Disk_table target_table(::Disk_table::construct_helper(db_name, tb_name));
        actual_names = target_table.get_column_name();
        actual_types = target_table.get_type_list();
    }
    // 输出信息
    std::vector<selected_col> output_cols;
    for (uint32_t i = 0; i < cols.size(); ++i)
    {
        if (cols[i].is_all)
        {
            for (uint32_t j = 0; j < actual_names.size(); ++j)
            {
                selected_col t;
                t.is_all = false;
                t.origin_name = actual_names[j];
                t.origin_type = actual_types[j];
                t.origin_index = j;
                t.renamed = actual_names[j];
                output_cols.push_back(t);
            }
        }
        else
        {
            bool failed = true;
            for (uint32_t j = 0; j < actual_names.size(); ++j)
            {
                if (cols[i].origin_name == actual_names[j])
                {
                    failed = false;
                    selected_col t;
                    t.is_all = false;
                    t.origin_name = actual_names[j];
                    t.origin_type = actual_types[j];
                    t.origin_index = j;
                    if (cols[i].renamed.empty())
                        t.renamed = actual_names[j];
                    else
                        t.renamed = cols[i].renamed;
                    output_cols.push_back(t);
                }
            }
            if (false)
                return cp::err_code::no_such_column;
        }
    }
    // 输出信息创建完成，开始准备输出表
    ::Memory_table *poutput = NULL;
    std::vector<int64_t> *pindexs = new std::vector<int64_t>;
    {
        std::vector<::Type> typelist;
        std::vector<std::string> namelist;
        for (uint32_t i = 0; i < output_cols.size(); ++i)
        {
            typelist.push_back(output_cols[i].origin_type);
            namelist.push_back(output_cols[i].renamed);
            pindexs->push_back(output_cols[i].origin_index);
        }
        poutput = new ::Memory_table(typelist);
        poutput->set_column_name(namelist);
    }
    // 创建对输入表的引用
    ::Disk_table *ptarget_table = new (::Disk_table)(::Disk_table::construct_helper(db_name, tb_name));
    result.sql_cmds.push_back(
        cp::sql_cmd_t(cp::code_sql_t::CMD_SELECT, (void *)ptarget_table, (void *)pindexs, (void *)poutput));
    return cp::err_code::success;
}
/**
 * @brief 首个关键字为 INSERT ，进入 INSERT 语句分析环节
 *
 * @param global_code_list 传入 grasem_analysis 的 Token 串表
 * @param global_str_map 传入 grasem_analysis 的 字符串映射表
 * @param pos_play_back 从 grasem_analysis 获取的识别进度，识别结束（无论成功或失败）会得到新的识别起点位置
 * @param result 编译后的 SQL 程序
 * @return cp::err_code 错误码
 */
cp::err_code analysis_INSERT(
    const std::vector<cp::token_t> &global_code_list,
    const std::vector<std::string> &global_str_map,
    int64_t &pos_play_back,
    cp::sql_program_t &result)
{
    // 初始化
    result.execs.clear();
    result.sql_cmds.clear();
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Insert)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 第二位置只允许 INTO 关键字
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Into)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 第 3 位置只允许 typeWord 型
    if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        return cp::err_code::syntax_error;
    // 保存表名称，这里同样也支持 dbname.tablename语法
    std::string table_name, db_name;
    table_name = global_str_map[global_code_list[pos_play_back].index];
    ++pos_play_back;
    // 看是否是 . typeWord 是的话改变表名与数据库名
    if (global_code_list[pos_play_back].key_id == cp::kid_t::Dot)
    {
        ++pos_play_back;
        // 现在只允许 typeWord 型
        if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
            return cp::err_code::syntax_error;
        // 改变字符串位置
        db_name = table_name;
        table_name = global_str_map[global_code_list[pos_play_back].index];
        ++pos_play_back;
    }
    // 存放可能存在的 ( ... ) 信息作为重映射参考。该 vector 在生成 pos_map_input2table 数组后失效
    std::vector<std::string> usr_input_namelist;
    // 查看是否是 (
    if (global_code_list[pos_play_back].key_id == cp::kid_t::Left_br)
    {
        ++pos_play_back;
        while (true)
        {
            // 现在只允许 typeWord 型
            if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
                return cp::err_code::syntax_error;
            usr_input_namelist.push_back(global_str_map[global_code_list[pos_play_back].index]);
            ++pos_play_back;
            // 如果是 ) 则跳出，是 , 则继续，其余报错
            if (global_code_list[pos_play_back].key_id == cp::kid_t::Right_br)
            {
                ++pos_play_back;
                break;
            }
            else if (global_code_list[pos_play_back].key_id == cp::kid_t::Sep)
            {
                ++pos_play_back;
                continue;
            }
            else
                return cp::err_code::syntax_error;
        }
    }
    // 接入DBM查找此表是否存在
    {
        std::vector<std::string> had_tables;
        Dbm::show_tables(db_name, &had_tables);
        bool not_found = true;
        for (int i = 0; i < had_tables.size(); ++i)
            if (had_tables[i] == table_name)
            {
                not_found = false;
                break;
            }
        if (not_found)
            return cp::err_code::no_such_table;
    }
#ifdef _GS_DEBUG_
    for (int i = 0; i < usr_input_namelist.size(); ++i)
        printf("%s ", usr_input_namelist[i].c_str());
    printf("\n");
#endif
    // 存放位置重映射信息,第k个元素的值为v表示原出现在第k个的元素该写入位置v
    std::vector<uint64_t> pos_map_input2table;
    // 存放表的真实数据类型信息
    std::vector<::Type> table_type_info;
    //在这里接入DBM获取表头信息，检验各名称是否存在并得到格式化要素
    {
        // 先导出表内真实列名
        ::Disk_table target_table(::Disk_table::construct_helper(db_name, table_name));
        std::vector<std::string> col_names_in_table = target_table.get_column_name();
        // 导出类型信息
        table_type_info = target_table.get_type_list();
        // 如果映射表为空，说明没有指定顺序，创建默认映射
        if (usr_input_namelist.empty())
        {
            for (uint32_t j = 0; j < col_names_in_table.size(); ++j)
                pos_map_input2table.push_back(j);
        }
        // 检查映射表
        else
        {
            for (uint32_t i = 0; i < usr_input_namelist.size(); ++i)
            {
                bool nfinded = true;
                for (uint32_t j = 0; j < col_names_in_table.size(); ++j)
                {
                    if (usr_input_namelist[i] == col_names_in_table[j])
                    {
                        nfinded = false;
                        pos_map_input2table.push_back(j);
                        break;
                    }
                }
                if (nfinded)
                    return cp::err_code::no_such_column;
            }
            // 在pos_map 内查重
            for (int i = 0; i < pos_map_input2table.size(); ++i)
                for (int j = i + 1; j < pos_map_input2table.size(); ++j)
                    if (pos_map_input2table[i] == pos_map_input2table[j])
                        return cp::err_code::repeated_colname;
        }
    }
#ifdef _GS_DEBUG_
    for (int i = 0; i < pos_map_input2table.size(); ++i)
        printf("%lld ", pos_map_input2table[i]);
    printf("\n");
#endif

    // 查看接下来只能是 values 了
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Values)
        return cp::err_code::syntax_error;
    ++pos_play_back;

    std::vector<::Tuple> all_vals;
    // 在这里读取values内的一条或多条 ( ... ) 并按表头信息格式化及重排
    {
        ::Disk_table target_table(::Disk_table::construct_helper(db_name, table_name));
        while (true)
        {
            // 外层循环，每层产生一条 line_t
            if (global_code_list[pos_play_back].key_id != cp::kid_t::Left_br)
                return cp::err_code::syntax_error;
            ++pos_play_back;
            ::Tuple one_line_of_data(target_table);
            // 内层循环，每次读一条，循环退出时产生一行
            for (int inline_index = 0;; ++inline_index)
            {
                // 如果有用户定义的映射信息，检查与映射信息是否吻合(是否超长度)
                if (usr_input_namelist.size())
                {
                    if (inline_index >= usr_input_namelist.size())
                    {
                        return cp::err_code::syntax_error;
                    }
                }
                // 根据目标列的不同检查属性
                cp::kid_t cur = global_code_list[pos_play_back].key_id;
                switch (table_type_info[pos_map_input2table[inline_index]].type)
                {
                case ::Data_type::BOOLEAN:
                {
                    if (cur == cp::kid_t::True)
                    {
                        ::Value t = {.null = false, .value = (uint64_t) true};
                        one_line_of_data.set_value(pos_map_input2table[inline_index], t);
                        break;
                    }
                    else if (cur == cp::kid_t::False)
                    {
                        ::Value t = {.null = false, .value = (uint64_t) false};
                        one_line_of_data.set_value(pos_map_input2table[inline_index], t);
                        break;
                    }
                    else
                        return cp::err_code::type_error;
                    break;
                }
                case ::Data_type::CHAR:
                {
                    if (cur != cp::kid_t::typeText)
                        return cp::err_code::type_error;
                    if (global_str_map[global_code_list[pos_play_back].index].size() > table_type_info[pos_map_input2table[inline_index]].size)
                        return cp::err_code::str_too_long;
                    else
                    {
                        ::Value t = {.null = false, .value = 0ull};
                        uint64_t size = global_str_map[global_code_list[pos_play_back].index].size() + 1;
                        t.value = (uint64_t)malloc(size);
                        memcpy((void *)(t.value), (void *)global_str_map[global_code_list[pos_play_back].index].c_str(), size);
                        one_line_of_data.set_value(pos_map_input2table[inline_index], t);
                    }
                    break;
                }
                case ::Data_type::DOUBLE:
                {
                    bool flip = false;
                    if (cur == cp::kid_t::opSub)
                    {
                        flip = true;
                        ++pos_play_back;
                        cur = global_code_list[pos_play_back].key_id;
                    }
                    if ((cur == cp::kid_t::typeInt) || (cur == cp::kid_t::typeFloat))
                    {
                        double d = 0;
                        try
                        {
                            d = std::stod(global_str_map[global_code_list[pos_play_back].index]);
                        }
                        catch (std::invalid_argument &)
                        {
                            return cp::err_code::invalid_num;
                        }
                        catch (std::out_of_range &)
                        {
                            return cp::err_code::invalid_num;
                        }
                        if (flip)
                            d = -d;
                        uint64_t t = 0;
                        memcpy((void *)(&t), (void *)&d, sizeof(double));
                        one_line_of_data.set_value(pos_map_input2table[inline_index], ::Value{.null = false, .value = t});
                        break;
                    }
                    else
                    {
                        return cp::err_code::type_error;
                        break;
                    }
                }
                case ::Data_type::FLOAT:
                {
                    bool flip = false;
                    if (cur == cp::kid_t::opSub)
                    {
                        flip = true;
                        ++pos_play_back;
                        cur = global_code_list[pos_play_back].key_id;
                    }
                    if ((cur == cp::kid_t::typeInt) || (cur == cp::kid_t::typeFloat))
                    {
                        float d = 0;
                        try
                        {
                            d = std::stof(global_str_map[global_code_list[pos_play_back].index]);
                        }
                        catch (std::invalid_argument &)
                        {
                            return cp::err_code::invalid_num;
                        }
                        catch (std::out_of_range &)
                        {
                            return cp::err_code::invalid_num;
                        }
                        if (flip)
                            d = -d;
                        uint64_t t = 0;
                        memcpy((void *)(&t), (void *)&d, sizeof(float));
                        one_line_of_data.set_value(pos_map_input2table[inline_index], ::Value{.null = false, .value = t});
                        break;
                    }
                    else
                    {
                        return cp::err_code::type_error;
                        break;
                    }
                }
                case ::Data_type::INT:
                {
                    bool flip = false;
                    if (cur == cp::kid_t::opSub)
                    {
                        flip = true;
                        ++pos_play_back;
                        cur = global_code_list[pos_play_back].key_id;
                    }
                    if (cur == cp::kid_t::typeInt)
                    {
                        int d = 0;
                        try
                        {
                            d = std::stoi(global_str_map[global_code_list[pos_play_back].index]);
                        }
                        catch (std::invalid_argument &)
                        {
                            return cp::err_code::invalid_num;
                        }
                        catch (std::out_of_range &)
                        {
                            return cp::err_code::invalid_num;
                        }
                        if (flip)
                            d = -d;
                        uint64_t t = 0;
                        memcpy((void *)(&t), (void *)&d, sizeof(int));
                        one_line_of_data.set_value(pos_map_input2table[inline_index], ::Value{.null = false, .value = t});
                        break;
                    }
                    else
                    {
                        return cp::err_code::type_error;
                        break;
                    }
                }
                case ::Data_type::LONG:
                {
                    bool flip = false;
                    if (cur == cp::kid_t::opSub)
                    {
                        flip = true;
                        ++pos_play_back;
                        cur = global_code_list[pos_play_back].key_id;
                    }
                    if (cur == cp::kid_t::typeInt)
                    {
                        int64_t d = 0;
                        try
                        {
                            d = std::stoll(global_str_map[global_code_list[pos_play_back].index]);
                        }
                        catch (std::invalid_argument &)
                        {
                            return cp::err_code::invalid_num;
                        }
                        catch (std::out_of_range &)
                        {
                            return cp::err_code::invalid_num;
                        }
                        if (flip)
                            d = -d;
                        one_line_of_data.set_value(pos_map_input2table[inline_index], ::Value{.null = false, .value = (uint64_t)d});
                        break;
                    }
                    else
                    {
                        return cp::err_code::type_error;
                        break;
                    }
                }
                default:
                    return cp::err_code::not_support;
                    break;
                }
                // 检查该结束还是继续
                ++pos_play_back;
                if (global_code_list[pos_play_back].key_id == cp::kid_t::Sep)
                {
                    ++pos_play_back;
                    continue;
                }
                else if (global_code_list[pos_play_back].key_id == cp::kid_t::Right_br)
                {
                    ++pos_play_back;
                    break;
                }
                else
                    return cp::err_code::syntax_error;
            }
            all_vals.push_back(one_line_of_data);
            // 检查是 , 还是 ;
            if (global_code_list[pos_play_back].key_id == cp::kid_t::Sep)
            {
                ++pos_play_back;
                continue;
            }
            else if (global_code_list[pos_play_back].key_id == cp::kid_t::End)
            {
                ++pos_play_back;
                break;
            }
            else
                return cp::err_code::syntax_error;
        }
    }
    // 在这里生成并写回控制指令
    ::Disk_table *ptarget_table = new (::Disk_table)(::Disk_table::construct_helper(db_name, table_name));
    result.sql_cmds.push_back(
        cp::sql_cmd_t(cp::code_sql_t::CMD_INSERT, (void *)(all_vals.size()), (void *)(new std::vector<Tuple>(all_vals)), (void *)ptarget_table));
    result.sql_cmds.push_back(
        cp::sql_cmd_t(cp::code_sql_t::CMD_RELEASE, (void *)ptarget_table, 0, 0));
    return cp::err_code::success;
}

/**
 * @brief 首个关键字为 UPDATE ，进入 UPDATE 语句分析环节
 *
 * @param global_code_list 传入 grasem_analysis 的 Token 串表
 * @param global_str_map 传入 grasem_analysis 的 字符串映射表
 * @param pos_play_back 从 grasem_analysis 获取的识别进度，识别结束（无论成功或失败）会得到新的识别起点位置
 * @param result 编译后的 SQL 程序
 * @return cp::err_code 错误码
 */
cp::err_code analysis_UPDATE(
    const std::vector<cp::token_t> &global_code_list,
    const std::vector<std::string> &global_str_map,
    int64_t &pos_play_back,
    cp::sql_program_t &result)
{
    // 初始化
    result.execs.clear();
    result.sql_cmds.clear();
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Update)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 库名 和 表名
    std::string db_name = "", tb_name = "";
    // dbname.tbname 或 tbname
    if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        return cp::err_code::syntax_error;
    tb_name = global_str_map[global_code_list[pos_play_back].index];
    ++pos_play_back;
    // 判 d.t逻辑
    if (global_code_list[pos_play_back].key_id == cp::kid_t::Dot)
    {
        ++pos_play_back;
        if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
            return cp::err_code::syntax_error;
        db_name = tb_name;
        tb_name = global_str_map[global_code_list[pos_play_back].index];
        ++pos_play_back;
    }
    // set 关键字
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Set)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    //读 name = val 序列，暂不支持运算
    struct update_info_t
    {
        uint64_t index;
        ::Type origin_type;
        std::string col_name;
        cp::token_t val;
    };
    // 待更新的信息整理
    std::vector<update_info_t> sets;
    while (true)
    {
        if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
            return cp::err_code::syntax_error;
        update_info_t one = {.col_name = global_str_map[global_code_list[pos_play_back].index]};
        ++pos_play_back;
        // 查重复列名
        for (uint32_t i = 0; i < sets.size(); ++i)
        {
            if (sets[i].col_name == one.col_name)
                return cp::err_code::repeated_colname;
        }
        cp::kid_t cur = global_code_list[pos_play_back].key_id;
        if ((cur == cp::kid_t::typeFloat) || (cur == cp::kid_t::typeInt) || (cur == cp::kid_t::typeText))
        {
            one.val.key_id = cur;
            one.val.index = global_code_list[pos_play_back].index;
        }
        else
            return cp::err_code::syntax_error;
        sets.push_back(one);
        ++pos_play_back;
        // 看结束或继续
        if (global_code_list[pos_play_back].key_id == cp::kid_t::Sep)
        {
            ++pos_play_back;
            continue;
        }
        else
            break;
    }
    // 接入DBM查找此表是否存在
    {
        std::vector<std::string> had_tables;
        Dbm::show_tables(db_name, &had_tables);
        bool not_found = true;
        for (int i = 0; i < had_tables.size(); ++i)
            if (had_tables[i] == tb_name)
            {
                not_found = false;
                break;
            }
        if (not_found)
            return cp::err_code::no_such_table;
    }
    {
        // 先导出 pos_map_input2table
        ::Disk_table target_table(::Disk_table::construct_helper(db_name, tb_name));
        std::vector<std::string> col_names_in_table = target_table.get_column_name();
        // 导出类型信息
        std::vector<Type> types = target_table.get_type_list();
        // 对sets中的值添加类型和下标信息
        for (int i = 0; i < sets.size(); ++i)
        {
            bool not_found = true;
            for (int j = 0; j < col_names_in_table.size(); ++j)
            {
                if (col_names_in_table[j] == sets[i].col_name)
                {
                    not_found = false;
                    sets[i].origin_type = types[j];
                    sets[i].index = j;
                    break;
                }
            }
            if (not_found)
                return cp::err_code::no_such_column;
        }
    }
    // 根据索引，类型信息，检查并生成 exec 所用的值
    exec::data_table_t consts;
    exec::program_t program;
    for (int i = 0; i < sets.size(); ++i)
    {
        // 根据目标列的不同检查属性
        cp::token_t cur_val = sets[i].val;
        cp::kid_t cur = cur_val.key_id;
        uint64_t cur_index = sets[i].index;
        switch (sets[i].origin_type.type)
        {
        case ::Data_type::BOOLEAN:
        {
            if (cur == cp::kid_t::True)
            {
                exec::val_t t(true);
                consts.push_back(t);
                program.push_back(exec::cmd_t(exec::code_exec_t::CPY, -consts.size(), 0, (int)cur_index));
                break;
            }
            else if (cur == cp::kid_t::False)
            {
                exec::val_t t(false);
                consts.push_back(t);
                program.push_back(exec::cmd_t(exec::code_exec_t::CPY, -consts.size(), 0, (int)cur_index));
                break;
            }
            else
                return cp::err_code::type_error;
            break;
        }
        case ::Data_type::CHAR:
        {
            if (cur != cp::kid_t::typeText)
                return cp::err_code::type_error;
            if (global_str_map[cur_val.index].size() > sets[i].origin_type.size)
                return cp::err_code::str_too_long;
            else
            {
                exec::val_t t(global_str_map[cur_val.index].size() + 1, (void *)(global_str_map[cur_val.index].c_str()));
                consts.push_back(t);
                program.push_back(exec::cmd_t(exec::code_exec_t::CPY, -consts.size(), 0, (int)cur_index));
            }
            break;
        }
        case ::Data_type::DOUBLE:
        {
            if ((cur == cp::kid_t::typeInt) || (cur == cp::kid_t::typeFloat))
            {
                double d = 0;
                try
                {
                    d = std::stod(global_str_map[cur_val.index]);
                }
                catch (std::invalid_argument &)
                {
                    return cp::err_code::invalid_num;
                }
                catch (std::out_of_range &)
                {
                    return cp::err_code::invalid_num;
                }
                exec::val_t t(d);
                consts.push_back(t);
                program.push_back(exec::cmd_t(exec::code_exec_t::CPY, -consts.size(), 0, (int)cur_index));
                break;
            }
            else
            {
                return cp::err_code::type_error;
                break;
            }
        }
        case ::Data_type::FLOAT:
        {
            if ((cur == cp::kid_t::typeInt) || (cur == cp::kid_t::typeFloat))
            {
                float d = 0;
                try
                {
                    d = std::stof(global_str_map[cur_val.index]);
                }
                catch (std::invalid_argument &)
                {
                    return cp::err_code::invalid_num;
                }
                catch (std::out_of_range &)
                {
                    return cp::err_code::invalid_num;
                }
                exec::val_t t(d);
                consts.push_back(t);
                program.push_back(exec::cmd_t(exec::code_exec_t::CPY, -consts.size(), 0, (int)cur_index));
                break;
            }
            else
            {
                return cp::err_code::type_error;
                break;
            }
        }
        case ::Data_type::INT:
        {
            if (cur == cp::kid_t::typeInt)
            {
                int d = 0;
                try
                {
                    d = std::stoi(global_str_map[cur_val.index]);
                }
                catch (std::invalid_argument &)
                {
                    return cp::err_code::invalid_num;
                }
                catch (std::out_of_range &)
                {
                    return cp::err_code::invalid_num;
                }
                exec::val_t t(d);
                consts.push_back(t);
                program.push_back(exec::cmd_t(exec::code_exec_t::CPY, -consts.size(), 0, (int)cur_index));
                break;
            }
            else
            {
                return cp::err_code::type_error;
                break;
            }
        }
        case ::Data_type::LONG:
        {
            if (cur == cp::kid_t::typeInt)
            {
                int64_t d = 0;
                try
                {
                    d = std::stoll(global_str_map[cur_val.index]);
                }
                catch (std::invalid_argument &)
                {
                    return cp::err_code::invalid_num;
                }
                catch (std::out_of_range &)
                {
                    return cp::err_code::invalid_num;
                }
                exec::val_t t(d);
                consts.push_back(t);
                program.push_back(exec::cmd_t(exec::code_exec_t::CPY, -consts.size(), 0, (int)cur_index));
                break;
            }
            else
            {
                return cp::err_code::type_error;
                break;
            }
        }
        default:
            return cp::err_code::not_support;
            break;
        }
    }
    // 在这里检查where或结束
    bool no_where = false;
    if (global_code_list[pos_play_back].key_id == cp::kid_t::Where)
    {
        ++pos_play_back;
        return cp::err_code::not_support;
    }
    if (global_code_list[pos_play_back].key_id != cp::kid_t::End)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 写回控制指令
    result.execs.push_back(exec::exec_t(program, 0, consts));

    ::Disk_table *ptarget_table = new (::Disk_table)(::Disk_table::construct_helper(db_name, tb_name));
    result.sql_cmds.push_back(
        cp::sql_cmd_t(cp::code_sql_t::CMD_EXEC, (void *)ptarget_table, (void *)(result.execs.size() - 1), 0ull));
    result.sql_cmds.push_back(
        cp::sql_cmd_t(cp::code_sql_t::CMD_RELEASE, (void *)ptarget_table, 0, 0));
    return cp::err_code::success;
}
/**
 * @brief 首个关键字为 DELETE ，进入 DELETE 语句分析环节。目前的where只支持最多 a op b
 *
 * @param global_code_list 传入 grasem_analysis 的 Token 串表
 * @param global_str_map 传入 grasem_analysis 的 字符串映射表
 * @param pos_play_back 从 grasem_analysis 获取的识别进度，识别结束（无论成功或失败）会得到新的识别起点位置
 * @param result 编译后的 SQL 程序
 * @return cp::err_code 错误码
 */
cp::err_code analysis_DELETE(
    const std::vector<cp::token_t> &global_code_list,
    const std::vector<std::string> &global_str_map,
    int64_t &pos_play_back,
    cp::sql_program_t &result)
{
    // 初始化
    result.execs.clear();
    result.sql_cmds.clear();
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Delete)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 第二位置只允许 from 关键字
    if (global_code_list[pos_play_back].key_id != cp::kid_t::From)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 库名 和 表名
    std::string db_name = "", tb_name = "";
    // dbname.tbname 或 tbname
    if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        return cp::err_code::syntax_error;
    tb_name = global_str_map[global_code_list[pos_play_back].index];
    ++pos_play_back;
    // 判 d.t逻辑
    if (global_code_list[pos_play_back].key_id == cp::kid_t::Dot)
    {
        ++pos_play_back;
        if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
            return cp::err_code::syntax_error;
        db_name = tb_name;
        tb_name = global_str_map[global_code_list[pos_play_back].index];
        ++pos_play_back;
    }
    // where
    exec::program_t program;
    bool no_where = false;
    if (global_code_list[pos_play_back].key_id == cp::kid_t::Where)
    {
        return cp::err_code::not_support;
        //! DELETE - where
        // // WHERE 暂不支持
        // ++pos_play_back;
        // if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        //     return cp::err_code::syntax_error;
        // std::string col_name1 = global_str_map[global_code_list[pos_play_back].index];
        // ++pos_play_back;
        // if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        //     return cp::err_code::syntax_error;
        // switch (global_code_list[pos_play_back].key_id)
        // {
        // case cp::kid_t::True:
        // default:
        //     return cp::err_code::not_support;
        //     break;
        // }
    }
    if (global_code_list[pos_play_back].key_id == cp::kid_t::End)
    {
        // 无where，添加永true逻辑
        if (no_where)
            program = {exec::cmd_t(exec::code_exec_t::RTT, 0, 0, 0)};
    }
    else
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 写回控制指令
    result.execs.push_back(exec::exec_t(program, 0, exec::data_table_t()));
    std::string *ptn = new std::string(tb_name), *pdn = NULL;
    if (db_name.size())
        pdn = new std::string(db_name);
    result.sql_cmds.push_back(
        cp::sql_cmd_t(cp::code_sql_t::CMD_DELETE, (void *)ptn, (void *)pdn, (void *)(result.execs.size() - 1)));
    return cp::err_code::success;
}
/**
 * @brief 首个关键字为 SHOW ，进入 SHOW DB/TABLE 语句分析环节
 *
 * @param global_code_list 传入 grasem_analysis 的 Token 串表
 * @param global_str_map 传入 grasem_analysis 的 字符串映射表
 * @param pos_play_back 从 grasem_analysis 获取的识别进度，识别结束（无论成功或失败）会得到新的识别起点位置
 * @param result 编译后的 SQL 程序
 * @return cp::err_code 错误码
 */
cp::err_code analysis_SHOW(
    const std::vector<cp::token_t> &global_code_list,
    const std::vector<std::string> &global_str_map,
    int64_t &pos_play_back,
    cp::sql_program_t &result)
{
    // 初始化
    result.execs.clear();
    result.sql_cmds.clear();
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Show)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 第二位置只允许 databases/tables 关键字
    cp::code_sql_t cs = cp::code_sql_t::no_op;
    switch (global_code_list[pos_play_back].key_id)
    {
    case cp::kid_t::Databases:
        cs = cp::code_sql_t::CMD_SHOWDB;
        break;
    case cp::kid_t::Tables:
        cs = cp::code_sql_t::CMD_SHOWT;
        break;
    default:
        return cp::err_code::syntax_error;
        break;
    }
    ++pos_play_back;
    // 如果第三位置为分号则成功
    if (global_code_list[pos_play_back].key_id != cp::kid_t::End)
        return cp::err_code::syntax_error;
    // 保存命令
    cp::sql_cmd_t cmd(cs, NULL, NULL, NULL);
    result.sql_cmds.push_back(cmd);
    ++pos_play_back;
    return cp::err_code::success;
}
/**
 * @brief 首个关键字为 use ，进入 use 语句分析环节
 *
 * @param global_code_list 传入 grasem_analysis 的 Token 串表
 * @param global_str_map 传入 grasem_analysis 的 字符串映射表
 * @param pos_play_back 从 grasem_analysis 获取的识别进度，识别结束（无论成功或失败）会得到新的识别起点位置
 * @param result 编译后的 SQL 程序
 * @return cp::err_code 错误码
 */
cp::err_code analysis_USE(
    const std::vector<cp::token_t> &global_code_list,
    const std::vector<std::string> &global_str_map,
    int64_t &pos_play_back,
    cp::sql_program_t &result)
{
    // 初始化
    result.execs.clear();
    result.sql_cmds.clear();
    if (global_code_list[pos_play_back].key_id != cp::kid_t::Use)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 第二位置只允许 word 型关键字
    if (global_code_list[pos_play_back].key_id != cp::kid_t::typeWord)
        return cp::err_code::syntax_error;
    ++pos_play_back;
    // 如果第三位置为分号则成功
    if (global_code_list[pos_play_back].key_id != cp::kid_t::End)
        return cp::err_code::syntax_error;
    // 复制字符串
    std::string *tgt_name = new std::string(global_str_map[global_code_list[pos_play_back - 1].index]);
    // 保存命令
    cp::sql_cmd_t cmd(cp::code_sql_t::CMD_USE, (void *)tgt_name, NULL, NULL);
    result.sql_cmds.push_back(cmd);
    ++pos_play_back;
    return cp::err_code::success;
}

cp::err_code cp::grasem_analysis(
    const std::vector<cp::token_t> &code_list,
    const std::vector<std::string> &str_map,
    std::vector<sql_program_t> &result)
{
    int64_t pos_token = 0;
    // 当token串还没见底，根据光标位置将分析任务下发给各个语句的分析器。
    while (pos_token < code_list.size())
    {
        sql_program_t single_prog;
        int64_t new_pos = pos_token;
        err_code single_err = err_code::type_error;
        switch (code_list[pos_token].key_id)
        {
        case kid_t::Create:
            single_err = analysis_CREATE(code_list, str_map, new_pos, single_prog);
            break;
        case kid_t::Drop:
            single_err = analysis_DROP(code_list, str_map, new_pos, single_prog);
            break;
        case kid_t::Select:
            single_err = analysis_SELECT(code_list, str_map, new_pos, single_prog);
            break;
        case kid_t::Insert:
            single_err = analysis_INSERT(code_list, str_map, new_pos, single_prog);
            break;
        case kid_t::Update:
            single_err = analysis_UPDATE(code_list, str_map, new_pos, single_prog);
            break;
        case kid_t::Delete:
            single_err = analysis_DELETE(code_list, str_map, new_pos, single_prog);
            break;
        case kid_t::Show:
            single_err = analysis_SHOW(code_list, str_map, new_pos, single_prog);
            break;
        case kid_t::Use:
            single_err = analysis_USE(code_list, str_map, new_pos, single_prog);
            break;
        case kid_t::End:
            ++pos_token;
            continue;
        default:
        {
#if TRY_CONTINUE_AFTER_ONE_ERROR
#else
            return err_code::not_support;
#endif
            break;
        }
        }
        if (new_pos > pos_token)
        {
            pos_token = new_pos;
            // 编译成功推入
            if (single_err == err_code::success)
                result.push_back(single_prog);
            // 编译失败根据宏决定
            else
            {
#if TRY_CONTINUE_AFTER_ONE_ERROR
#else
                return single_err;
#endif
            }
        }
        else
            return err_code::syntax_error;
    }
    return err_code::success;
}