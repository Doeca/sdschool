#pragma once
#include <cpr/cpr.h>
#include <iostream>
#include <rapidJson/document.h>

using namespace std;

namespace sdschool {

    class api {

    private:
        //私有结构
        class exam {
        public:
            string name,guid;
        };
        //私有成员
        string token, name, schoolid;
        vector<exam> examList;
        //私有方法
        void initExamList();
        void initStudentInfo();
        void claim();

    public:
        string login(string account, string password, int &success);
        string getExamList();
        string examGrades(int id);
        string examAnswerCard(int subject);
    };

} // namespace sdschool