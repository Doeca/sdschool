#pragma once
#include <cpr/cpr.h>
#include <rapidJson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <dolores/dolores.hpp>
#include <iostream>

using namespace std;

namespace sdschool {

    class api {
    private:
        //私有结构
        class exam {
        public:
            string  name,guid;
            exam(string c_name, string c_guid) : name(c_name), guid(c_guid) {
            }
        };
        //私有成员
        string token, name, schoolid;
        vector<exam> examList;
        //私有方法
        void initExamList();
        void initStudentInfo();
        void claim();

    public:
        api(string account, string password);
        string getExamList(int page);
        string examGrades(int id);
        string examAnswerCard(int subject);
    };

} // namespace sdschool