#include <ctype.h>
#include <iostream>
#include <string>
#include "Encoder.h"
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
        current.send("绑定成功！");
    } catch (string info) {
        current.send(info);
    } catch (const char* info) {
        current.send(info);
    } catch (...) {
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
        current.send(api.getExamList(stoi(para[0])));

    } catch (string info) {
        current.send(info);
    } catch (const char* info) {
        current.send(info);
    } catch (...) {
        std::cerr << "发生了错误\r\n";
    }
}

dolores_on_message("测试", startswith("test")) {
    Encoder encoder;
    cout << encoder.UTF8UrlEncode("C++中实现类似php的UTF8和UrlEncode函数") << endl;
}