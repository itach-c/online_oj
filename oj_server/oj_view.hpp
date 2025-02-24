#pragma once
#include <vector>
#include <iostream>
#include "../comm/myutile/utile.hpp"
#include "question.hpp"
#include <ctemplate/template.h>
namespace ns_view
{
    using namespace ns_model;
    using namespace ns_utile;
    const std::string template_question_html_path = "./template_html/";
    class View
    {

    public:
        void all_expand_html(std::string *html, std::vector<struct question> &all)
        {
            // 需要显示的内容: 题目编号、题目标题、题目难度
            // 推荐使用表格显示
        

            std::string src_html = template_question_html_path + "all_questions.html";

            ctemplate::TemplateDictionary root("all_questions");
            for (const auto &q : all)
            {

                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("question_list");

                sub->SetValue("number", q.qid);
                sub->SetValue("title", q.title);
                sub->SetValue("star", q.level);
                sub->SetValue("cpu_limit", std::to_string(q.cpu_limit));
                sub->SetValue("mem_limit", std::to_string(q.mem_limit));
            }

            // 3.获取被渲染的网页对象
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);

            if (nullptr == tpl)
            {
                LOG_ERROR("渲染失败可能是路径不正确");
            }
            tpl->Expand(html, &root);
        }

        void oneExpandHtml(std::string *html, const question &q)
        {

            // 1. 形成路径
            std::string src_html = template_question_html_path + "one_question.html";

            // 2. 形成数字典
            ctemplate::TemplateDictionary root("one_question");
            root.SetValue("number", q.qid);
            root.SetValue("title", q.title);
            root.SetValue("star", q.level);
            root.SetValue("desc", q.desc);
            root.SetValue("pre_code", q.header);

            // 3. 获取被渲染的html
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);

            // 4. 开始完成渲染功能
            tpl->Expand(html, &root);
        }
    };
};