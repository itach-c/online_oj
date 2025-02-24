#pragma once

// 系统头文件
#include <iostream>
#include <vector>
#include <fstream>
#include <mutex>
#include <atomic>
#include <memory>
#include <arpa/inet.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <cstring>

// 第三方库头文件
#include "jsoncpp/json/json.h"
#include "../comm/httplib.h"

// 用户自定义的头文件
//#include "oj_model2.hpp"
#include "oj_model.hpp"  // 如果需要可以解除注释
#include "oj_view.hpp"
#include "../comm/myutile/utile.hpp"

namespace ns_control
{
    using namespace ns_model;

    using namespace ns_view;
    using namespace ns_utile;

    class Machine
    {
    public:
        Machine(std::string ip, uint16_t port)
            : ip_(ip), port_(port), load_(std::make_shared<std::atomic_int64_t>(0))
        {
        }
        ~Machine()
        {
        }
        void IncLoad()
        {
            (*load_)++;
        }
        void DecLoad()
        {
            (*load_)--;
        }
        std::string getIp() const { return ip_; }

        // 获取port_
        std::uint16_t getPort() const { return port_; }

        // 获取load_，返回值为 std::shared_ptr
        std::shared_ptr<std::atomic_int64_t> getPLoad() const { return load_; }

        // 获取load_的值，返回int64_t
        int64_t getLoadValue() const { return load_->load(); }

    private:
        std::string ip_;
        std::uint16_t port_;
        std::shared_ptr<std::atomic_int64_t> load_;
    };
    const std::string machine_conf = "./conf/server_machine.conf";
    class loadBlance
    {
    public:
        loadBlance()
        {
            loadConf();
        }

        void loadConf()
        {
            std::ifstream in(machine_conf);
            if (!in.is_open())
            {
                LOG_FATAL("加载主机序列失败");
            }
            std::string line;
            while (std::getline(in, line))
            {
                std::vector<std::string> v;
                StringUtile::splitString(line, ":", &v);
                if (v.size() != 2)
                {
                    LOG_ERROR("加载单个主机序列失败，请检查配置文件");
                }
                onlineMachines_.push_back(Machines_.size());
                Machines_.push_back(Machine(v[0], std::stoi(v[1])));
            }
        }
        // 第二个参数为输出型参数，是由于machine 里面有原子类型的指针，不想将机器来回拷贝,这里的机器理论上是不能拷贝的,
        // 所有直接返回机器的第一，要改变指针类型的值就需要传递二级指针
        bool smartChoose(int *id, Machine **m)
        {
            {
                std::unique_lock<std::mutex> lock(mtx_);
                int onlineNum = onlineMachines_.size();
                if (0 == onlineNum)
                {
                    LOG_FATAL("所有机器都无法提供编译服务,请立即检查");
                }
                *id = onlineMachines_[0];
                *m = &Machines_[*id];
                int64_t min_load = *(Machines_[onlineMachines_[0]].getPLoad());
                for (int i = 0; i < onlineMachines_.size(); i++)
                {
                    int64_t temp_load = *(Machines_[onlineMachines_[i]].getPLoad());
                    if (min_load < temp_load)
                    {
                        min_load = temp_load;
                        *id = onlineMachines_[i];
                        *m = &Machines_[*id];
                    }
                }

                return true;
            }
        }
        void showMachines()
        {
            mtx_.lock();
            std::cout << "在线主机: ";
            for (auto &m : onlineMachines_)
            {
                std::cout << m << " ";
            }
            std::cout << std::endl;

            std::cout << "离线主机: ";
            for (auto &m : offlineMachines_)
            {
                std::cout << m << " ";
            }
            std::cout << std::endl;
            mtx_.unlock();
        }
        void offlineMachine(int which)
        {
            auto it = onlineMachines_.begin();
            while (it != onlineMachines_.end())
            {
                if (*it == which)
                {
                    onlineMachines_.erase(it);
                    offlineMachines_.push_back(which);
                    break;
                }
                it++;
            }
        }
        void onlineMachine()
        { // 我们统一上线，后面统一解决
            mtx_.lock();
            onlineMachines_.insert(onlineMachines_.end(), offlineMachines_.begin(), offlineMachines_.end());
            offlineMachines_.erase(offlineMachines_.begin(), offlineMachines_.end());
            mtx_.unlock();
            LOG_INFO("所有主机都上线了");
        }

        ~loadBlance()
        {
        }

    private:
        std::vector<Machine> Machines_;    // 所有主机
        std::vector<int> onlineMachines_;  // 保存在线主机id
        std::vector<int> offlineMachines_; // 保存离线主机id
        std::mutex mtx_;
    };
    class Control
    {

    public:
        void RecoveryMachine()
        {
            load_blance_.onlineMachine();
        }
        bool Allquestions(std::string *html)
        {
            std::vector<question> all;
            if (model_.GetAllQuestions(&all))
            {
                // 将所有题目构建成网页返回
                std::sort(all.begin(), all.end(), [](question a, question b)
                          { return std::stoi(a.qid) < std::stoi(b.qid); });

                view_.all_expand_html(html, all);
                return true;
            }
            else
            {
                *html = "获取题目列表失败";
                return false;
            }
        }
        bool Onequestion(std::string &num, std::string *html)
        {

            question q;
            if (model_.GetOneQuestion(num, &q) == true)
            {

                // 构建html返回
                view_.oneExpandHtml(html, q);
                return true;
            }
            else
            {
                *html = "获取指定题目失败,题目编号:" + num;
                return false;
            }
        }
        void Judge(const std::string &num, const std::string &in_json, std::string *out_json)
        {
            // 1接受前端传来的代码
            question q;
            model_.GetOneQuestion(num, &q);
            Json::Value root;

            // 2 反序列化得到用户代码和用户输入
            Json::Reader reader;
            reader.parse(in_json, root);
            std::string code = root["code"].asString();
            std::string input = root["input"].asString();
            // 拼接测试用例形成完整编译代码
            Json::Value complieRoot;
            complieRoot["code"] = code + q.tail;
            complieRoot["input"] = input;
            complieRoot["cpu_limit"] = q.cpu_limit;
            complieRoot["mem_limit"] = q.mem_limit;
            Json::FastWriter writer;
            std::string sendToCompiler = writer.write(complieRoot);

            // 选择负载最低的主机
            // 一直请求直到全部主机挂掉

            while (true)
            {
                load_blance_.showMachines();
                int id = 0;
                Machine *m = nullptr;
                if (load_blance_.smartChoose(&id, &m))
                {
                    m->IncLoad();
                    LOG_INFO("向ip: %s port: %u 机器发起请求", m->getIp().c_str(), m->getPort());
                    httplib::Client cli(m->getIp(), m->getPort());

                    if (auto res = cli.Post("/compile_and_run", sendToCompiler, "application/json;charset=utf-8"))
                    {
                        res->status == 200;
                        *out_json = res->body;
                        m->DecLoad();
                        LOG_INFO("请求编译运行服务成功");
                        break;
                    }
                    else
                    {
                        LOG_INFO("ip %s port: %u 的机器可能已经离线", m->getIp().c_str(), m->getPort());
                        load_blance_.offlineMachine(id);
                        m->DecLoad();
                    }
                }

                // smartChoose 中对选择失败已经做了判断，如果没有主机可供选择，则直接会LOG_FATAL 这个宏里面会直接终止程序;
            }

            // 得到结果赋值给out_json;
        }

    private:
        Model model_;
        View view_;
        loadBlance load_blance_;
    };

};