#include <stdio.h>
#include <vector>
#include <string>

#include "xsql_dbm.hpp"
#include"xsql_compile.hpp"
#include"xsql_connect.hpp"


int main()
{
    /**
     * @brief 储存用户输入
     *
     */
    std::string userInput;

    /**
     * @brief 储存编译层编译结果
     *
     */
    std::vector<cp::sql_program_t> do_queue;
    char letter;
    cp::err_code err_code;//编译的返回结果
    bool flagdouble = true;//分号是否在双引号外
    bool flagsingle = true;//分号是否在单引号外
    bool flagcontinue = false;//语句是否结束
    std::string::iterator it;//迭代器用于查找特定字符
    printf("xsql>");
    while(letter = getchar()){
        if(letter == '\n'){
            if(flagcontinue){
                printf("  -->");
                continue;
            }else{
                printf("xsql>");
            }
        }
        else if(letter == '\"'){//双引号
            userInput+=letter;
            it=userInput.end()-1;
            if(*it=='\\'){
                continue;
            }else{
                flagdouble = !flagdouble;
            }
        }
        else if(letter == '\''){//单引号
            userInput+=letter;
            it=userInput.end()-1;
            if(*it=='\\'){
                continue;
            }else{
                flagsingle = !flagsingle;
            }
        }
        else if(letter == ';'&&flagdouble&&flagsingle){
            flagcontinue = false;
            userInput+=letter;
            if(userInput=="quit;"){
                printf("感谢使用xsql！\n");
                break;
            }
            err_code = cp::analysis(userInput.c_str(),do_queue,0);
            if(err_code==cp::success){
                std::vector<expl::err_code> err = expl::explain_queue(do_queue[0]);
                //输出运行结果
                for(auto& item:err){
                    switch (item)
                    {
                    case expl::SUCCESS:
                        std::cout<<"SUCCESS!"<<std::endl;
                        break;
                    
                    default:
                        std::cout<<"ERROR!"<<std::endl;
                        break;
                    }                 
                }
                userInput.clear();
                do_queue.clear();
            }
            else if(do_queue.size()==0){//无效输入
                std::cout<<"无效输入，请检查语法是否有误!"<<std::endl;
                userInput.clear();
                do_queue.clear();
                continue;
            }
            else{//抛出异常
                std::cout<<"error!"<<std::endl;
                userInput.clear();
                do_queue.clear();
            }
        }else{
            userInput+=letter;
            flagcontinue = true;
        }
    }
    return 0;
}

