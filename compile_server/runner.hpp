#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "../comm/myutile/log.hpp"
#include "../comm/myutile/utile.hpp"
#include <sys/wait.h>
 #include <sys/time.h>
#include <sys/resource.h>


namespace ns_runner
{
using namespace ns_utile;
using namespace ns_log;
 
    class Runner
    {

    public:
        Runner()
        {
        }

        ~Runner()
        {
        }
    public:


        //需要出题者传入 最大内存限制 和 cpu占用限制
     static int Run(const std::string& filename,int cpu_limit,int mem_limit  )
     {
        
            //是否正确跑完就行，测试通不通过由上层判断

            //程序启动默认打开
            //标准输入  
            //标准输出 和测试用例对比
            //标准错误 返回错误
            std::string execute = pathUtile::buildExe(filename);
           
            std::string _stdin  = pathUtile::StdIn(filename);  
            std::string _stdout  = pathUtile::StdOut(filename);
            std::string _stderror =  pathUtile::StdError(filename);
            umask(0);
            int infd = open(_stdin.c_str(),O_CREAT | O_RDONLY ,0644);
            int outfd = open(_stdout.c_str(),O_CREAT | O_WRONLY ,0644);
            int errorfd = open(_stderror.c_str(),O_CREAT | O_WRONLY ,0644);
            if(infd < 0 || outfd < 0 || errorfd < 0 )
            {
                LOG_ERROR("运行所需文件打开失败 errno: %d error message : %s" ,errno,strerror(errno));
                return -1;
            }
        pid_t pid = fork();
        if(pid < 0)
        {
            // false

            LOG_ERROR("创建子进程失败 errno: %d error message %s", errno, strerror(errno));
            ::close(infd);
            ::close(outfd);
            ::close(errorfd);
            return -2;
        }
        else if (pid == 0)
        {

            //
            SetProcLimit(cpu_limit,mem_limit + 20000);
   
    
            dup2(infd, 0);
            dup2(outfd, 1);
            dup2(errorfd, 2);
   
            int ret = execlp(execute.c_str(),execute.c_str(),nullptr);
            exit(1);
        }
        else
        {

            ::close(infd);
            ::close(outfd);
            ::close(errorfd);
            int status = 0;
            int rid = waitpid(pid,&status,0);
            LOG_INFO(" run done status code : %d", status & 0x7F);
            return status & 0x7F; 
        }


        return 0;


     }

    private:

        static void SetProcLimit(int cpu_limit,int mem_limit)
    {
        struct rlimit cpu_rlimit;
        struct rlimit mem_rlimit;
        cpu_rlimit.rlim_max = RLIM_INFINITY;
        cpu_rlimit.rlim_cur = cpu_limit;
        mem_rlimit.rlim_max = RLIM_INFINITY;
        mem_rlimit.rlim_cur = mem_limit * 1024;
        setrlimit(RLIMIT_CPU,&cpu_rlimit);
        setrlimit(RLIMIT_AS,&mem_rlimit);

    }
    };


};
