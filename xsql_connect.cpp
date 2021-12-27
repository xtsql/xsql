#include "xsql_connect.hpp"
#include<iostream>

std::vector<expl::err_code> expl::explain_queue(cp::sql_program_t queue){
    std::vector<err_code>res;
    err_code res_single;
    int exec_index = 0;
    for(auto& item:queue.sql_cmds){   
        switch (item.cmd)
        {
            case cp::CMD_CREATED:
                res_single = create_db(item);
                break;

            case cp::CMD_DROPD:
                res_single = drop_db(item);
                break;
            
            case cp::CMD_CREATET:
                res_single = create_t(item);
                break;

            case cp::CMD_DROPT:
                res_single = drop_t(item);
                break;

            case cp::CMD_RELEASE:
                delete (Table*)item.op1;
                res_single = expl::SUCCESS;
                break;

            case cp::CMD_INSERT:
                res_single = insert_turple(item);
                break;

            case cp::CMD_SELECT:
                res_single = select_turple(item);

            case cp::CMD_SHOWT:
                res_single = show_t();
                break;

            case cp::CMD_SHOWDB:
                res_single = show_db();
                break;
            
            case cp::CMD_USE:
                res_single = use_db(item);
                break;
            case cp::CMD_DELETE:
                res_single = delete_turple(item);
                break;
            default:
                break;
        }
        res.push_back(res_single);
    }
    return res;
};

expl::err_code expl::use_db(cp::sql_cmd_t cmd){
    int64_t err = Dbm::use_database(*(std::string*)cmd.op1);
    if(err==0){
        return expl::SUCCESS;
    }else{
        return expl::FAILED;
    }
}

expl::err_code expl::create_db(cp::sql_cmd_t cmd){
    Dbm::create_database(*(std::string*)cmd.op1);
    return expl::SUCCESS;
}

expl::err_code expl::drop_db(cp::sql_cmd_t cmd){
    Dbm::drop_database(*(std::string*)cmd.op1);
    return expl::SUCCESS;
}

expl::err_code expl::show_db(){
    std::vector<std::string> names;
    if (Dbm::show_databases(&names)<0)
    {
        fprintf(stderr, "获取数据库失败！\n");
        return expl::FAILED;
    }
    for(auto& i:names)
    {
        printf("%s\n", i.c_str());
    }
    return expl::SUCCESS;
}


expl::err_code expl::insert_turple(cp::sql_cmd_t cmd){
    // printf("kunkun:insert\n");
    std::string cur_d = Dbm::get_useing_database();
    Disk_table* table = (Disk_table*) cmd.op3;
    std::vector<Tuple>* tuple_list = (std::vector<Tuple>*)cmd.op2;
    //检查tuplelist
    // for(auto& item:*tuple_list){
    //     for(auto &item2: item.cell_list){
    //         if(item2.valid){
    //             printf("%llu\n",item2.struct_value.value);
    //         }else{
    //             printf("null\n");
    //         }
    //     }
    // }
    table->insert(*tuple_list);
    delete tuple_list;
    return expl::SUCCESS;
}

expl::err_code expl::create_t(cp::sql_cmd_t cmd){
    std::vector<std::string>col_names;
    std::vector<::Type>col_types;
    for(auto &item : *(std::vector<cp::colum_info_t>*)cmd.op2){
        col_names.push_back(item.col_name);
        col_types.push_back(item.col_type);
    }
    std::string cur_db = "";
    if(cmd.op3==NULL){
        cur_db=Dbm::get_useing_database();
    }else{
        cur_db = *(std::string*)cmd.op3;
    }
    // std::cout<<cur_db<<std::endl;
    int64_t err = Dbm::create_table(cur_db,*(std::string*)cmd.op1,col_names,col_types,0);
    if(err==0){
        return expl::SUCCESS;
    }else{
        return expl::FAILED;
    }
}

expl::err_code expl::drop_t(cp::sql_cmd_t cmd){
    std::string cur_db = "";
    if(cmd.op2==NULL){
        cur_db=Dbm::get_useing_database();
    }else{
        cur_db = *(std::string*)cmd.op2;
    }
    int64_t err = Dbm::drop_table(cur_db,*(std::string*)cmd.op1);
    if(err==0){
        return expl::SUCCESS;
    }else{
        return expl::FAILED;
    }
}

expl::err_code expl::show_t(){
    std::string cur_d = Dbm::get_useing_database();
    std::vector<std::string> names;
    if (Dbm::show_tables(cur_d, &names)<0)
    {
        fprintf(stderr, "获取表失败！\n");
        return expl::FAILED;
    }
    for(auto& i:names)
    {
        printf("%s\n", i.c_str());
    }
    return expl::SUCCESS;
}

expl::err_code expl::select_turple(cp::sql_cmd_t cmd){
    Disk_table* table = (Disk_table*) cmd.op1;
    std::vector<std::string>col_names = table->get_column_name();
    std::vector<Type>col_types = table->get_type_list();
    printf("| ");
    for(auto &item:*(std::vector<int64_t>*)cmd.op2){
        printf("%s\t",col_names[item].c_str());
    }
    printf("|\n");
    table->set_i_begin();
    while(!table->i_is_end()){
        printf("| ");
        Value value0;
        
        for(auto &item:*(std::vector<int64_t>*)cmd.op2){
            // printf("item: %d\n",item);
            if(col_types[item].type==CHAR){
                value0.value = (uint64_t)malloc(col_types[item].size);
                if(value0.value==NULL){
                    printf("malloc fail\n");
                }
            }
            table->read_i(item,&value0);
            switch (col_types[item].type)
            {
                case CHAR:
                    printf("%s\t",(char *)value0.value);
                    break;
                
                case INT:
                    printf("%d\t",*(int*)&value0.value);
                    break;
                
                case LONG:
                    printf("%lld\t",*(int64_t*)&value0.value);
                    break;
                
                case FLOAT:
                    printf("%f\t",*(float*)&value0.value);
                    break;
                
                case DOUBLE:
                    printf("%lf\t",*(double*)&value0.value);
                    break;
                
                case BOOLEAN:
                    bool temp;
                    temp = *(bool *)&value0.value;
                    std::cout<<temp<<'\t';
                    break;
                
                default:
                    printf("type error\n");
                    break;
            }
        }
        printf("|\n");
        table->inc_i();
    }
    delete (Table*)cmd.op1;
    delete (Table*)cmd.op3;
    return expl::SUCCESS;
}

expl::err_code expl::delete_turple(cp::sql_cmd_t cmd){
    //获取数据库名
    std::string cur_db = "";
    if(cmd.op2==NULL){
        cur_db=Dbm::get_useing_database();
    }else{
        cur_db = *(std::string*)cmd.op2;
    }
    printf("==========>into\n");
    Disk_table *table = new (Disk_table)(Disk_table::construct_helper(cur_db, *(std::string*)cmd.op1));
    printf("%p\n",table);
    printf("==========>construct failed\n");
    table->set_i_begin();
    printf("==========>set begin failed\n");
    while(!table->i_is_end()){
        printf("==========>not end\n");
        table->remove_i();
    }
    printf("==========>end\n");
    delete table;
    return expl::SUCCESS;
}
