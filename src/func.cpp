#include <ctype.h>
#include <iostream>
#include <string>
#include "sevenday.h"
#include "strHandle.hpp"

using namespace dolores::matchers;
using namespace rapidjson;


dolores_on_message("绑定账号", startswith(".ebind")) {
    try {
        vector<string> para = strhandle::rm_preflix(current.event.message, 6);
        if (para.size() < 2) throw "🆖输入格式不正确，请重试！\r\n使用方法：phy.doeca.cc";
        sdschool::api api(para[0], para[1]);

        //将账号信息存储到本地
        Document doc;
        Pointer("/account").Set(doc, para[0].c_str());
        Pointer("/password").Set(doc, para[1].c_str());
        StringBuffer str_buff;
        Writer<StringBuffer> writer(str_buff);
        doc.Accept(writer);
        strhandle::str2file(str_buff.GetString(), strhandle::user_dir(current) + "account.json");
        current.send("🤩绑定成功！");
    } catch (string info) {
        current.send(info);
    } catch (const char* info) {
        current.send(info);
    } catch (...) {
        cq::send_private_message(
            1124468334, "严重错误\r\nid : " + to_string(current.event.user_id) + "\r\nmsg : " + current.event.message);
        current.send("😓发生了严重的错误，请稍后再使用吧");
        std::cerr << "发生了错误\r\n";
    }
}

dolores_on_message("查询列表", startswith(".elist")) {
    try {
        vector<string> para = strhandle::rm_preflix(current.event.message, 6);
        if (para.size() == 0) para.push_back("1");
        Document d;
        string fs = strhandle::file2str(strhandle::user_dir(current) + "account.json");
        cout << fs << endl;
        d.Parse(fs.c_str());

        sdschool::api api(d["account"].GetString(), d["password"].GetString());
        current.send(api.getExamList(strhandle::str2int(para[0])));
    } catch (string info) {
        current.send(info);
    } catch (const char* info) {
        current.send(info);
    } catch (...) {
        cq::send_private_message(
            1124468334, "严重错误\r\nid : " + to_string(current.event.user_id) + "\r\nmsg : " + current.event.message);
        current.send("😓发生了严重的错误，请稍后再使用吧");
        std::cerr << "发生了错误\r\n";
    }
}

dolores_on_message("查询成绩", startswith(".egrades")) {
    try {
        vector<string> para = strhandle::rm_preflix(current.event.message, 8);
        if (para.size() == 0) para.push_back("1");
        Document d;
        string fs = strhandle::file2str(strhandle::user_dir(current) + "account.json");
        cout << fs << endl;
        d.Parse(fs.c_str());

        sdschool::api api(d["account"].GetString(), d["password"].GetString());
        current.send("👌稍等，正在查询");
        current.send(api.examGrades(strhandle::str2int(para[0])));
    } catch (string info) {
        current.send(info);
    } catch (const char* info) {
        current.send(info);
    } catch (...) {
        cq::send_private_message(1124468334, "严重错误\r\nid : " + to_string(current.event.user_id)+"\r\nmsg : "+current.event.message);
        current.send("😓发生了严重的错误，请稍后再使用吧");
        std::cerr << "发生了错误\r\n";
    }
}

dolores_on_message("查询错题", startswith(".einfo")) {
    try {
        vector<string> para = strhandle::rm_preflix(current.event.message, 6);
        if (para.size() < 2) throw "🆖输入格式不正确，请重试！\r\n正确格式 .einfo 序号 科目\r\n详细使用方法：phy.doeca.cc";
        Document d;
        string fs = strhandle::file2str(strhandle::user_dir(current) + "account.json");
        cout << fs << endl;
        d.Parse(fs.c_str());

        sdschool::api api(d["account"].GetString(), d["password"].GetString());
        current.send("😤稍等，正在查询");
        current.send(api.examWrongAnswer(strhandle::str2int(para[0]), para[1]));

    } catch (string info) {
        current.send(info);
    } catch (const char* info) {
        current.send(info);
    } catch (...) {
        cq::send_private_message(
            1124468334, "严重错误\r\nid : " + to_string(current.event.user_id) + "\r\nmsg : " + current.event.message);
        current.send("😓发生了严重的错误，请稍后再使用吧");
        std::cerr << "发生了错误\r\n";
    }
}

dolores_on_message("查询答题卡", startswith(".ecard")) {
    try {
        vector<string> para = strhandle::rm_preflix(current.event.message, 6);
        if (para.size() < 2)
            throw "🆖输入格式不正确，请重试！\r\n正确格式 .ecard 序号 科目\r\n详细使用方法：phy.doeca.cc";
        Document d;
        string fs = strhandle::file2str(strhandle::user_dir(current) + "account.json");
        cout << fs << endl;
        d.Parse(fs.c_str());

        sdschool::api api(d["account"].GetString(), d["password"].GetString());
        current.send("😤稍等，正在查询");
        string path = api.examAnswerCard(strhandle::str2int(para[0]), para[1]);
        current.send(cq::message::MessageSegment::image(path));

        remove((utils::ansi(cq::dir::root("data", "image") + path)).c_str());
    } catch (string info) {
        current.send(info);
    } catch (const char* info) {
        current.send(info);
    } catch (...) {
        cq::send_private_message(
            1124468334, "严重错误\r\nid : " + to_string(current.event.user_id) + "\r\nmsg : " + current.event.message);
        current.send("😓发生了严重的错误，请稍后再使用吧");
        std::cerr << "发生了错误\r\n";
    }
}