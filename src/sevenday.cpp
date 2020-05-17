#include "sevenday.h"
#include <cqcppsdk/cqcppsdk.h>
#include <fstream>
using namespace rapidjson;

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
    auto paras = cpr::Parameters{{"uniqueKey", "652653c6-41f6-404d-a5c2-515a3bba266d"},
                                 {"start", "0"},
                                 {"rows", "100"},
                                 {"schoolGuid", schoolid}};
    string url = "https://szone-api.7net.cc/exam/getClaimExams?";
    url += paras.content;
    url += "&studentName=" + urlEncode(name);
    auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});

    if (r.status_code != 200) throw "🤒获取数据失败，请稍后再试";

    Document d;
    // cout << r.text << endl;
    d.Parse(r.text.c_str());
    if (!d.HasMember("data")) throw "😢出现未知错误，请稍后再试";

    const Value& arr = d["data"]["list"];
    for (SizeType i = 0; i < arr.Size(); i++) {
        sdschool::api::exam tmp(arr[i].GetObject()["examName"].GetString(), arr[i].GetObject()["examGuid"].GetString());
        sdschool::api::examList.push_back(tmp);
    }
}
//获取科目信息，防止提交错误的科目到服务器
bool sdschool::api::hasSubject(int id, string subject) {
    string url = "https://szone-api.7net.cc/report/getSubject?";
    auto paras = cpr::Parameters{{"examGuid", examList[id - 1].guid},
                                 {"scope", "1"},
                                 {"uniqueKey", "652653c6-41f6-404d-a5c2-515a3bba266d"},
                                 {"schoolGuid", schoolid}};
    url += paras.content;
    url += "&studentName=" + urlEncode(name);
    auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});
    Document d;
    d.Parse(r.text.c_str());
    if (r.status_code != 200 || !d.HasMember("data")) throw "🤒获取数据失败，请稍后再试";
    const Value& v = d["data"]["xaxis"];
    for (int i = 0; i < v.Size(); i++) {
        if (v[i].GetString() == subject) return true;
    }

    return false;
}
//根据参数获得该行数据
string sdschool::api::getCorrectAnswer(int id, string subject, string th) {
    string url = "https://szone-api.7net.cc/question/getinfo?";
    auto paras = cpr::Parameters{{"examGuid", examList[id - 1].guid},
                                 {"th", th},
                                 {"uniqueKey", "652653c6-41f6-404d-a5c2-515a3bba266d"},
                                 {"schoolGuid", schoolid}};
    url += paras.content;
    url += "&studentName=" + urlEncode(name) + "&subject=" + urlEncode(subject) + "&srcSubject=" + urlEncode(subject);
    auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});
    Document d;
    d.Parse(r.text.c_str());
    if (r.status_code != 200 || !d.HasMember("data")) throw "🤒获取数据失败，请稍后再试";
    ostringstream res;
    res << emoji(0) << th << "   ";

    res << (Pointer("/data/myAnswers/0/value").Get(d)->IsString()
                ? Pointer("/data/myAnswers/0/value").Get(d)->GetString()
                : "略")
        << "   ";

    res << (Pointer("/data/answers/0/value").Get(d)->IsString() ? Pointer("/data/answers/0/value").Get(d)->GetString()
                                                                : "略")
        << "   ";
    if (Pointer("/data/score").Get(d)->IsInt()) {
        res << Pointer("/data/score").Get(d)->GetInt();
    } else if (Pointer("/data/score").Get(d)->IsFloat()) {
        res << Pointer("/data/score").Get(d)->GetFloat();
    } else {
        res << "N";
    }
    res << "/";
    if (Pointer("/data/total").Get(d)->IsInt()) {
        res << Pointer("/data/total").Get(d)->GetInt();
    } else if (Pointer("/data/total").Get(d)->IsFloat()) {
        res << Pointer("/data/total").Get(d)->GetFloat();
    } else {
        res << "T";
    }
    return res.str();
}
// 内部tools Start
string sdschool::api::urlEncode(string& url) {
    string res;
    for (int i = 0; i < url.length(); i++) {
        char c = url[i];
        if (c == '_' || c == '-' || c == '~' || c == '.' || isalnum((unsigned char)c)) {
            res += c;
            continue;
        } else if (c == ' ') {
            res += '+';
            continue;
        } else {
            res += '%';
            res += toHex((int)((unsigned char)c));
        }
    }
    return res;
}
string sdschool::api::toHex(int num) {
    string HEX = "0123456789ABCDEF";
    if (num == 0) return "0";
    string result;
    int count = 0;
    while (num && count++ < 8) {
        result = HEX[(num & 0xf)] + result;
        num >>= 4;
    }
    return result;
}
string sdschool::api::emoji(int id) {
    vector<string> emojis = {
        "🍦",
        "🍋",
        "🍌",
        "🧀",
        "🌽",
        "🍮",
        "🍏",
        "🍵",
        "🍐",
        "🍈",
        "🍬",
        "🍭",
        "🍡",
        "🍙",
        "🍚",
        "🍛",
        "🍧",
        "🍰",
        "🍱",
        "🍥",
        "🍨",
        "☕",
        "🍿",

    };
    srand((unsigned int)time(0)+(unsigned int)rand());
    if (id > emojis.size() || id < 1) return emojis[(rand() % (emojis.size() - 1))];
    return emojis[id - 1];
}
// 内部tools End

//获取考试列表
string sdschool::api::getExamList(int page) {
    sdschool::api::initExamList();
    int total = sdschool::api::examList.size();
    string res = "💮考试列表，页码：" + to_string(page) + "/" + to_string((int)(total / 5) + (total % 5 == 0 ? 0 : 1));
    for (int i = (page - 1) * 5, j = 1; i < total && j <= 5; i++, j++) {
        res += "\r\n" + emoji(0) + to_string(i + 1) + ". " + sdschool::api::examList[i].name;
    }
    res += "\r\n🧐命令.egrades 序号 可用来查询成绩";
    return res;
}

//获取考试成绩
string sdschool::api::examGrades(int id) {
    initExamList();
    if (id > examList.size() || id < 1) throw "😷您输入的序号不正确哦";
    int student_a = 0, student_c = 0; //年级人数，班级人数
    for (int i = 1; i < 4; i++) {
        string url;
        switch (i) {
        case 1:
            url = "https://szone-api.7net.cc/report/getScore?";
            break;
        default:
            url = "https://szone-api.7net.cc/report/getPosition?";
        }

        auto paras = cpr::Parameters{{"examGuid", examList[id - 1].guid},
                                     {"scope", (i == 3 ? "1" : "2")},
                                     {"uniqueKey", "652653c6-41f6-404d-a5c2-515a3bba266d"},
                                     {"schoolGuid", schoolid}};
        url += paras.content;
        url += "&studentName=" + urlEncode(name);

        auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});

        if (r.status_code != 200) throw "🤒获取数据失败，请稍后再试";
        Document d;

        d.Parse(r.text.c_str());

        if (!d.HasMember("data")) throw "🤒获取数据失败，请稍后再试";

        if (i == 1) {
            const Value& v = d["data"]["scores"];
            if (!v.IsArray()) throw "🤒获取数据失败，请稍后再试";
            for (int j = 1; j < v.Size(); j++) {
                grades tmp;
                tmp.object_name = v[j][0].GetString();
                if (v[j][1].IsInt()) {
                    tmp.score = v[j][1].GetInt();
                } else if (v[j][1].IsFloat()) {
                    tmp.score = v[j][1].GetFloat();
                }
                gradesList.push_back(tmp);
            }
        } else {
            const Value& v = d["data"]["series"];
            if (!v.IsArray()) throw "🤒获取数据失败，请稍后再试";

            if (i == 2)
                student_a = v[1][0].GetInt() + v[2][0].GetInt();
            else
                student_c = v[1][0].GetInt() + v[2][0].GetInt();

            const Value& pos = v[0];
            for (int j = 0; j < pos.Size(); j++) {
                if (i == 2)
                    gradesList[j].pos_senior = student_a - pos[j].GetInt() + 1;
                else
                    gradesList[j].pos_class = student_c - pos[j].GetInt() + 1;
            }
        }
    }

    string res = emoji(0) + examList[id - 1].name + "\r\n"+emoji(0)+"科目   得分   年排   班排";

    for (int i = 0, j = 1; i < gradesList.size(); j++, i++) {
        ostringstream os;
        os << gradesList[i].score;

        string face;
        if (gradesList[i].pos_senior == 1)
            face = "🥇";
        else if (gradesList[i].pos_senior == 2)
            face = "🥈";
        else if (gradesList[i].pos_senior == 3)
            face = "🥉";
        else if (gradesList[i].pos_senior <= 50)
            face = "㊗";
        else if (gradesList[i].pos_senior >= 690)
            face = "⚰";
        else
            face = emoji(0);

        res += "\r\n" + face + gradesList[i].object_name + "   " + os.str() + "   "
               + to_string(gradesList[i].pos_senior) + "   " + to_string(gradesList[i].pos_class);
        os.clear();
    }
    res += "\r\n"+emoji(0)+"年级参考人数：" + to_string(student_a) + "人\r\n"+emoji(0)+"班级参考人数：" + to_string(student_c);
    return res;
}
//获取考试错题
string sdschool::api::examWrongAnswer(int id, string subject) {
    initExamList();
    if (id > examList.size() || id < 1) throw "😷您输入的序号不正确哦";
    if (!hasSubject(id, subject)) throw "😥没有找到" + subject + "科目的相关数据哦";

    string url = "https://szone-api.7net.cc/question/getThs?";
    auto paras = cpr::Parameters{{"examGuid", examList[id - 1].guid},
                                 {"uniqueKey", "652653c6-41f6-404d-a5c2-515a3bba266d"},
                                 {"schoolGuid", schoolid}};
    url += paras.content;
    url += "&studentName=" + urlEncode(name) + "&subject=" + urlEncode(subject) + "&srcSubject=" + urlEncode(subject);
    auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});
    Document d;
    d.Parse(r.text.c_str());
    if (r.status_code != 200 || !d.HasMember("data")) throw "🤒获取数据失败，请稍后再试";
    const Value& v = d["data"];
    ostringstream res;
    res << emoji(0) << examList[id - 1].name << endl;
    res << "⚔" << subject << "错题信息" << endl;
    res << emoji(0) << "题号 错答 准答 得分" << endl;
    for (int i = 0; i < v.Size(); i++) {
        if (v[i]["right"].IsFalse()) {
            res << getCorrectAnswer(id, subject, v[i]["th"].GetString()) << endl;
        }
    }
    res << emoji(0) << "命令 .ecard 序号 科目 可查询原答题卡";
    return res.str();
}
//获取答题卡
string sdschool::api::examAnswerCard(int id, string subject) {
    initExamList();
    if (id > examList.size() || id < 1) throw "😷您输入的序号不正确哦";
    if (!hasSubject(id, subject)) throw "😥没有找到" + subject + "科目的相关数据哦";
    string url = "https://szone-api.7net.cc/question/getAnswerCardUrl?";

    auto paras = cpr::Parameters{{"examGuid", examList[id - 1].guid},
                                 {"uniqueKey", "652653c6-41f6-404d-a5c2-515a3bba266d"},
                                 {"schoolGuid", schoolid}};

    url += paras.content;
    url += "&studentName=" + urlEncode(name) + "&srcSubject=" + urlEncode(subject);
    auto r = cpr::Get(cpr::Url{url}, cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});
    Document d;
    d.Parse(r.text.c_str());
    if (r.status_code != 200 || !d.HasMember("data")) throw "🤒获取数据失败，请稍后再试";

    url = d["data"].GetString();
    r = cpr::Get(cpr::Url{url});

    string path = cq::dir::root("data", "image") + r.header["x-oss-meta-file"];
    cout << path << endl;

    ofstream fp(cq::utils::ansi(path), ios::binary);
    if (!fp.good()) throw "🤒写入文件失败，请稍后再试";
    fp << r.text;
    fp.close();

    return r.header["x-oss-meta-file"];
}