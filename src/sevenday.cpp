#include "sevenday.h"
using namespace rapidjson;

//订阅服务
void sdschool::api::claim() {
    auto r = cpr::Post(cpr::Url{"https://szone-api.7net.cc/userinfo/subscribe"},
                       cpr::Payload{{"serviceKey", "652653c6-41f6-404d-a5c2-515a3bba266d"}},
                       cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});
    if (r.status_code != 200) throw "🤒获取数据失败，请稍后再试";
}
//获取学生信息
void sdschool::api::initStudentInfo() {
    auto r = cpr::Get(cpr::Url{"https://szone-api.7net.cc/userinfo/GetStudentInfo"},
                      cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});
    if (r.status_code != 200) throw "🤒获取数据失败，请稍后再试";
    Document d;
    d.Parse(r.text.data());
    sdschool::api::schoolid = d["data"].GetObject()["schoolGuid"].GetString();
    sdschool::api::name = d["data"].GetObject()["studentName"].GetString();
    cout << sdschool::api::schoolid << " " << sdschool::api::name << endl;
}
//初始化考试列表
void sdschool::api::initExamList() {
    //cout << cpr::util::urlEncode() << " " << schoolid << endl;

    string tmp = "杨艺哲";
    for (int i = 0; i < tmp.length(); i++) {
        cout << (int)tmp[i] << endl;
    }
    auto paras = cpr::Parameters{{"uniqueKey", "652653c6-41f6-404d-a5c2-515a3bba266d"},
                                 {"start", "1"},
                                 {"rows", "10"},
                                 {"studentName",name.c_str()},
                                 {"schoolGuid", schoolid}};

    auto r = cpr::Get(cpr::Url{"https://szone-api.7net.cc/exam/getClaimExams"},
                      cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}},
                      paras);
    if (r.status_code != 200) throw "🤒获取数据失败，请稍后再试";
    cout << r.text << endl;

    Document d;
    // cout << r.text << endl;
    d.Parse(r.text.c_str());
    if (!d.HasMember("data")) throw "😢出现未知错误，请稍后再试";

    const Value& arr = d["data"];
    for (SizeType i = 0; i < arr.Size(); i++) {
        sdschool::api::exam tmp(arr[i].GetObject()["examName"].GetString(), arr[i].GetObject()["examGuid"].GetString());
        sdschool::api::examList.push_back(tmp);
    }
}

string sdschool::api::getExamList(int page) {
    sdschool::api::initExamList();
    int total = sdschool::api::examList.size();
    string res = "💮考试列表，页码：" + to_string(page) + "/" + to_string((int)(total / 5) + (total % 5 == 0 ? 0 : 1));

    for (int i = (page - 1) * 5, j = 1; i < total && j <= 5; i++, j++) {
        string emoji;
        switch (j) {
        case 1:
            emoji = "🛫";
            break;
        case 2:
            emoji = "🚝";
            break;
        case 3:
            emoji = "🚢";
            break;
        case 4:
            emoji = "🚁";
            break;
        case 5:
            emoji = "🌎";
            break;
        }
        res += "\r\n" + emoji + "." + to_string(i + 1) + ". " + sdschool::api::examList[i].name;
    }
    res += "\r\n🧐命令.egrades 序号 可用来查询成绩";
    return res;
}

//登陆并订阅服务，获取学生信息
sdschool::api::api(string account, string password) {
    auto r = cpr::Post(cpr::Url{"https://szone-api.7net.cc/login"},
                       cpr::Payload{{"password", password}, {"userCode", account}},
                       cpr::Header{{"Version", "2.4.1"}});
    if (r.status_code != 200) throw "❎因不明原因登陆失败，请稍后再试";

    Document d;
    d.Parse(r.text.data());
    if (!(d.HasMember("data")))
        throw((string) "❎") + (d.HasMember("message") ? d["message"].GetString() : "因不明原因登陆失败")
            + "，请稍后再试";
    sdschool::api::token = d["data"].GetObject()["token"].GetString();
    sdschool::api::claim();
    sdschool::api::initStudentInfo();
}