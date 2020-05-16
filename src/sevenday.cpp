#include "sevenday.h"

void sdschool::api::claim() {
    auto r = cpr::Post(cpr::Url{"https://szone-api.7net.cc/userinfo/subscribe"},
                       cpr::Payload{{"serviceKey", "652653c6-41f6-404d-a5c2-515a3bba266d"}},
                       cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});
}


void sdschool::api::initStudentInfo() {
    auto r = cpr::Get(cpr::Url{"https://szone-api.7net.cc/userinfo/GetStudentInfo"},
                       cpr::Header{{"Token", sdschool::api::token}, {"Version", "2.4.1"}});

}


string sdschool::api::login(string account, string password, int &success) {
    auto r = cpr::Post(cpr::Url{"https://szone-api.7net.cc/login"},
                       cpr::Payload{{"password", password}, {"userCode", account}},
                       cpr::Header{{"Version","2.4.1"}});
    if (r.status_code != 200) return "❎因不明原因登陆失败，请稍后再试";

    rapidjson::Document d;
    d.Parse(r.text.data());

    if (!(d.HasMember("data")))
        return ((string)"❎") + (d.HasMember("message") ? d["message"].GetString() : "因不明原因登陆失败") + "，请稍后再试";

    success = 1;
    sdschool::api::token = d["data"].GetObject()["token"].GetString();

    return "🧐绑定成功！";
}
