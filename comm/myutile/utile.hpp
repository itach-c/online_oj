#pragma once
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <atomic>
#include "timestamp.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "log.hpp"
#include <fstream>
#include <vector>
#include <string.h>

namespace ns_utile
{
    const std::string tmpPath = "./tmp/";
    class pathUtile
    {
    public:
        static std::string AddSuffix(const std::string &filename, const std::string &suffix)
        {
            std::string path_name = tmpPath;
            path_name += filename;
            path_name += suffix;
            return path_name;
        }
        // 构建一个源文件路径加后缀
        // 编译时需要的文件
        static std::string buildSrc(const std::string &filename)
        {
            return AddSuffix(filename, ".cpp");
        }
        static std::string buildExe(const std::string &filename)
        {
            return AddSuffix(filename, ".exe");
        }

        static std::string buildCompilerError(const std::string &filename)
        {
            return AddSuffix(filename, ".compiler_err");
        }

        // 运行时需要的文件

        static std::string StdIn(const std::string &filename)
        {
            return AddSuffix(filename, ".stdin");
        }
        static std::string StdOut(const std::string &filename)
        {
            return AddSuffix(filename, ".stdout");
        }
        static std::string StdError(const std::string &filename)
        {
            return AddSuffix(filename, ".stderr");
        }
    };
    class FileUtile
    {

    public:
        static bool isFileExists(const std::string &path_name)
        {
            struct stat st;

            int n = stat(path_name.c_str(), &st);

            if (n == 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        static std::string unique_file_name()
        {

            // do;

            static std::atomic_uint id(0);
            std::string now = std::to_string(time(NULL));
            id++;

            return now + "_" + std::to_string(id);
        }
        static bool WriteFile(const std::string &file_name, const std::string &code)
        {
          
            int fd = open(file_name.c_str(), O_RDWR | O_CREAT, 0644);
            if (fd < 0)
            {
                LOG_ERROR("打开文件失败,errno: %d error message : %s", errno, strerror(errno));
                return false;
            }
            int n = write(fd, code.c_str(), code.size());

            ::close(fd);

            return true;
        }
        static bool ReadFile(const std::string &file_name, std::string *out)
        {
            std::ifstream file(file_name, std::ios::binary); // 替换为你的图片路径
            if (!file)
            {
                LOG_ERROR("无法打开文件 file_name : %s", file_name.c_str());
                return false;
            }

            // 读取文件内容
            std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            std::string data(buffer.begin(), buffer.end());
            *out = data;
            return true;
        }
    };

    class StringUtile
    {

    public:
        static void splitString(const std::string &str, const std::string &sep, std::vector<std::string> *out)
        {
            size_t start = 0;
            size_t end = str.find(sep);

            while (end != std::string::npos)
            {
                // 提取子字符串并忽略空字符串
                std::string token = str.substr(start, end - start);
                if (!token.empty())
                {
                    out->push_back(token);
                }

                start = end + sep.length(); // 更新起始位置
                end = str.find(sep, start); // 查找下一个分隔符
            }

            // 处理最后一个子字符串
            std::string token = str.substr(start);
            if (!token.empty())
            {
                out->push_back(token);
            }
        }
    };


};
