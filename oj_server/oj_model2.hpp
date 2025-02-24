#pragma once
#include <iostream>
#include <unordered_map>
#include "../comm/myutile/log.hpp"
#include <vector>
#include <fstream>
#include "question.hpp"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

// #pragma once
// #include <string>

// struct question{

//       std::string qid; //题目编号
//       std::string title;//题目的标题
//       std::string level;//难度
//       int cpu_limit; //单位秒
//       int mem_limit; //单位kb
//       std::string desc;//题目描述
//       std::string header;//预留代码
//       std::string tile; //测试提交编译器的
//   };
namespace ns_model
{

    using namespace ns_log;

    class MySQLDB
    {
    public:
        MySQLDB()
        {
            try
            {
                // 获取 MySQL 驱动
                driver = sql::mysql::get_mysql_driver_instance();
            }
            catch (std::runtime_error &e)
            {
                std::cerr << "MySQL driver creation failed: " << e.what() << std::endl;
                exit(1);
            }
        }

        ~MySQLDB()
        {
            if (con)
            {
                delete con;
            }
        }

        bool connect(const std::string &host, const std::string &user, const std::string &pass, const std::string &db)
        {
            try
            {
                // 连接到 MySQL 数据库
                con = driver->connect(host, user, pass);
                con->setSchema(db); // 选择数据库
            }
            catch (sql::SQLException &e)
            {
                std::cerr << "Connection failed: " << e.what() << std::endl;
                return false;
            }
            return true;
        }

    public:
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;
    };

    const std::string oj_questions = "oj_question";
    const std::string host =  host + ":3306";
    const std::string user = "oj_client";
    const std::string pass = "123456";
    const std::string db = "oj";
    class Model
    {

    public:
      

        Model()
        {
            if (db_.connect(host, user, pass, db))
            {
                LOG_INFO("数据库连接成功");
            }
            else
            {
                LOG_FATAL("数据库连接失败");
            }
        }
        ~Model()
        {
        }
        bool QuerySql(const std::string &sql, std::vector<question> *out)
        {
            try
            {
                // 创建一个 Statement 对象
                sql::Statement *stmt = db_.con->createStatement();
                // 执行查询
                sql::ResultSet *res = stmt->executeQuery(sql);

                // 处理结果
                while (res->next())
                {
                    question q;
                    q.qid = res->getString("id");
                    q.title = res->getString("title");
                    q.level = res->getString("level");
                    q.header = res->getString("header");
                    q.tail = res->getString("tail");
                    q.cpu_limit = res->getInt("cpu_limit");
                    q.mem_limit = res->getInt("mem_limit");
                    q.desc = res->getString("desc");
                    out->push_back(q);
                }
                delete res;
                delete stmt;
                return true;
            }
            catch (sql::SQLException &e)
            {
                LOG_ERROR("SQL query execution failed: %s", e.what());
                return false;
            }
        }

        bool GetAllQuestions(std::vector<question> *out)
        {
            std::string sql = "select * from ";
            sql += oj_questions;
            return QuerySql(sql, out);
        }
        bool GetOneQuestion(const std::string &num, question *q)
        {
            std::string sql = "select * from ";
            sql += oj_questions;
            sql += " where id = ";
            sql += num;
            std::vector<question> res;
            if (QuerySql(sql, &res) && res.size() == 1)
            {
                *q = res[0];
                return true;
            }
            else
            {
                return false;
            }
        }

    private:
        std::unordered_map<std::string, question> questions_;
        MySQLDB db_;
    };
};
