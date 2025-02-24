#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include "sys/wait.h"
#include "../comm/myutile/utile.hpp"
// 只进行代码编译
// 远端提交代码，编译器编译代码
namespace ns_compile
{

      using namespace ns_utile;
      using namespace ns_log;
    class Compiler
    {
    public:
        Compiler()
        {
        }

        ~Compiler()
        {
        }
        // 成功true 失败false ，参数编译文件名
        static bool compile(const std::string &file_name)
        {
            std::cout << file_name.c_str() << std::endl;
            pid_t pid = fork();

            if (pid == 0)
            {
              
                int errorfd = ::open(pathUtile::buildCompilerError(file_name).c_str(),O_CREAT | O_WRONLY,0644 );
                if(errorfd < 0)
                {
                    LOG_ERROR("没有形成errorfd");
                
                    exit(1);
                }
                dup2(errorfd,2);
                // 子进程,调用编译器完成编译
                int ret = execlp("g++", "g++","-o"
                        ,pathUtile::buildExe(file_name).c_str()
                            , pathUtile::buildSrc(file_name).c_str()
                                    , "-std=c++11", nullptr);
                LOG_ERROR("g++编译器启动失败 可能是参数错误");


                exit(2);
            }
            else if (pid < 0)
            {
                // fork error
                LOG_ERROR("fork error errno:%d error message: %s",errno,strerror(errno));
                return false;
            }
            else
            {
                // 父进程

                waitpid(pid, nullptr, 0);
                // 编译是否成功

                if(FileUtile::isFileExists(pathUtile::buildExe(file_name)))
                {
                    LOG_INFO("%s :编译成功",pathUtile::buildExe(file_name).c_str())
                    return true;
                }
            }
            LOG_ERROR("没有形成可执行文件");
            return false;
        }

    private:
    };

};