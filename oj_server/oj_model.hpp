#pragma once
#include <iostream>
#include <unordered_map>
#include "../comm/myutile/log.hpp"
#include <vector>
#include "../comm/myutile/utile.hpp"
#include <fstream>
#include "question.hpp"

namespace ns_model
{
    using namespace ns_utile;
    using namespace ns_log;

    class Model
    {

    public:
        const std::string sep = " ";
        const std::string question_list_path = "./questions/questions.list";
        const std::string questions_path = "./questions/";

        Model()
        {
            // oj_server/questions
            LoadQuestionsList(question_list_path);
        }
        ~Model()
        {
        }
        bool LoadQuestionsList(const std::string &list_path)
        {

            std::ifstream in(list_path);
            if (!in.is_open())
            {
                LOG_FATAL("loadQuestion open file error errno :%d, message : %s", errno, strerror(errno));

                return false;
            }
            std::string line;
            while (std::getline(in, line))
            {
                question q;
                std::vector<std::string> v;
                StringUtile::splitString(line, sep, &v);
                if (v.size() != 5)
                {
                    LOG_ERROR("加载题目列表出错,题号:%s", q.qid.c_str());
                    continue;
                }
                q.qid = v[0];
                q.title = v[1];
                q.level = v[2];
                q.cpu_limit = std::stoi(v[3]);
                q.mem_limit = std::stoi(v[4]);
                std::string question_desc_path = questions_path + q.qid + "/desc.txt";
                std::string question_header_path = questions_path + q.qid + "/header.hpp";
                std::string question_tail_path = questions_path + q.qid + "/tail.cpp";

                FileUtile::ReadFile(question_desc_path, &q.desc);
                FileUtile::ReadFile(question_header_path, &q.header);
                FileUtile::ReadFile(question_tail_path, &q.tail);
                questions_.insert({q.qid, q});
            }
            in.close();
            LOG_INFO("加载题库成功.........");
            return true;
        }
        bool GetAllQuestions(std::vector<question> *out)
        {
            if (questions_.size() == 0)
            {
                LOG_INFO("用户获取所有题目失败");
                return false;
            }
            for (auto &q : questions_)
            {
                out->push_back(q.second);
            }
            return true;
        }
        bool GetOneQuestion(const std::string &num, question *q)
        {
            auto ret = questions_.find(num);
            if (ret == questions_.end())
            {
                LOG_INFO("用户获题目失败 题号:%s", num.c_str());

                return false;
            }
            *q = ret->second;
            return true;
        }

    private:
        std::unordered_map<std::string, question> questions_;
    };
};
