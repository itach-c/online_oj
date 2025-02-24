#pragma once
#include "runner.hpp"
#include "jsoncpp/json/json.h"
#include "../comm/myutile/log.hpp"
#include "../comm/myutile/utile.hpp"
#include "compiler.hpp"
#include "runner.hpp"

namespace ns_compile_run
{
    using namespace ns_utile;
    using namespace ns_compile;
    using namespace ns_runner;
    using namespace ns_log;
    class CompileAndRun
    {

    public:
        /*** **************************
         * 输入:
         * .code  用户代码
         * .input 用户自己的输入先不处理
         * .cpulimit
         * .memlimit
         *
         * 输出
         * status 状态码
         * reson 请求原因
         * 选填字段
         * stdout 运行结果
         * stderror 程序完成的错误结果
         *
         *
         */
        static bool removeTempFile(const std::string &file_name)
        {

            std::string path = pathUtile::AddSuffix(file_name, "*");
            int pid = fork();
            if (pid == 0)
            {
                // child
                // 直接传rm -rf 通配符不会被解释，通配符需要再shell中解释，因此execlp 调用shell传入参数
                execlp("/bin/sh", "sh", "-c", ("rm -rf " + path).c_str(), nullptr);
            }
            else if (pid < 0)
            {
                // fork() error
                LOG_ERROR("remove fork error errno : %d ,error message %s", errno, strerror(errno));
            }
            else
            {

                pid_t rid = waitpid(pid, nullptr, 0);
            }
            return true;
        }
        static std::string CodetoDesc(int code, const std::string &file_name)
        {
            std::cout << "code :" << code << std::endl;

            switch (code)
            {
            case 0:
            {
                return "compile and run success";
            }
            case -1:
            {
                return "the code is empty";
            }
            case -2:
            {
                return "unknown";
            }
            case -3:
            {
                std::string compile_err;
                FileUtile::ReadFile(file_name, &compile_err);
                return compile_err;
            }
            case SIGXCPU:
            {
                return "cpu limit";
            }
            case SIGABRT:
            {
                return "mem limit";
            }
            default:
            {
                return "unknown";
            }
            }
        }
        static void start(std::string &json_in, std::string *json_out)
        {

            Json::Value root;
            Json::Reader reader;
            reader.parse(json_in, root);
            std::string code = root["code"].asString();
            std::string input = root["input"].asString();
            int cpu_limt = root["cpu_limit"].asInt();
            int mem_limt = root["mem_limit"].asInt();

            Json::Value OutValue;
            int status_code = 0;
            // 同时会有很多请求 形成唯一文件名
            std::string file_name = FileUtile::unique_file_name();
            if (code.size() == 0)
            {
                // 提交代码为空
                status_code = -1;
            }
            else
            {

                // 生成编译源文件
                if (FileUtile::WriteFile(pathUtile::buildSrc(file_name), code) == true)
                {

                    if (Compiler::compile(file_name) == true)
                    {
                        int n = Runner::Run(file_name, cpu_limt, mem_limt);
                        if (n == 0)
                        {
                            // run success
                            status_code = 0;
                        }
                        else if (n > 0)
                        {
                            // 信号杀死
                            status_code = n;
                        }
                        else if (n < 0)
                        {
                            // run内部出错未知错误
                            status_code = -2;
                        }
                    }
                    else
                    {
                        // 编译出错
                        status_code = -3;
                    }
                }
                else
                {
                    // 生成源文件出错
                    status_code = -2;
                }
            }
            OutValue["status"] = status_code;
            OutValue["reason"] = CodetoDesc(status_code, pathUtile::buildCompilerError(file_name));
            if (status_code == 0)
            {
                std::string std_out;
                FileUtile::ReadFile(pathUtile::StdOut(file_name), &std_out);
                OutValue["stdout"] = std_out;

                std::string std_err;
                FileUtile::ReadFile(pathUtile::StdError(file_name), &std_err);
                OutValue["stderror"] = std_err;
               
            }
            Json::StyledWriter writer;
            *json_out = writer.write(OutValue);
            removeTempFile(file_name);
        }
    };

}