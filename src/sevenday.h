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
        class grades {
        public:
            string object_name;
            float score;
            int pos_senior, pos_class;
            grades() {
                score = pos_senior = pos_class = 0;
            }
        };

        //私有成员
        string token, name, schoolid;
        vector<exam> examList;
        vector<grades> gradesList;
        //私有方法
        bool hasSubject(int id,string subject);
        void initExamList();
        void initStudentInfo();
        void claim();
        string getCorrectAnswer(int id, string subject, string th);
        string urlEncode(string &url);
        string toHex(int num);
        string emoji(int id);
    public:
        api(string account, string password);
        string getExamList(int page);
        string examGrades(int id);
        string examWrongAnswer(int id,string subject);
        string examAnswerCard(int id,string  subject);
    };

} // namespace sdschool