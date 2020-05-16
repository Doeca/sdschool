
#include <rapidJson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/stringbuffer.h>
#include <dolores/dolores.hpp>
#include <iostream>

#include "sevenday.h"
#include "strHandle.hpp"

using namespace dolores::matchers;
using namespace rapidjson;

dolores_on_message("跟我读", command("repeat", {"/", "."})) {
    current.send(current.command_argument());
}
dolores_on_message("绑定账号", startswith(".bind")) {
    std::vector<std::string> para = strhandle::rm_preflix(current.event.message, 5);
    if (para.size() < 2) {
        current.send("🆖输入格式不正确，请重试！\r\n使用方法：phy.doeca.cc");
        return;
    }
    sdschool::api api;
    int flag = 0;
    string res = api.login(para[0], para[1], flag);
    cout << res;
    if (flag == 0) {
        current.send(res);
        return;
    }

    Document doc;
    Pointer("/account").Set(doc, para[0].c_str());
    Pointer("/password").Set(doc, para[1].c_str());

    FILE* fp = fopen((strhandle::user_dir(current) + "account.json").c_str(), "wb");
    StringBuffer str_buff;
    char writeBuffer[4096];
    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

    Writer<FileWriteStream> writer(os);
    doc.Accept(writer);
    fclose(fp);

    current.send(res);
}
