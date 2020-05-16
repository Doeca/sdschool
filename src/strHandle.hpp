#include <cqcppsdk/cqcppsdk.h>
#include <dolores/dolores.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace cq;

namespace strhandle {

    //去除命令前缀并以空格分割文本，返回数组
    vector<string> rm_preflix(string raw, int length) {
        string no_preflix = raw.substr(length);
        string tmp = "";
        vector<string> res;
        for (int i = 0; i < no_preflix.size(); i++) {
            if (no_preflix[i] == ' ') {
                if (tmp != "") {
                    res.push_back(tmp);
                    tmp = "";
                }
            } else {
                tmp += no_preflix[i];
            }
        }

        if (tmp != "") res.push_back(tmp);
        return res;
    }

    //根据用户QQ号创建并返回目录path
    string user_dir(dolores::Current<MessageEvent> &current) {
        string path = dir::app() + to_string(current.event.user_id) + '\\';
        dir::create_dir_if_not_exists(path);
        return path;
    }
    //将数据写到文件中
    void str2file(string data, string path) {
        ofstream fp;
        fp.open(path);
        fp << data;
        fp.close();
    }

    string file2str(string path) {
        cout << path << endl;
        ifstream fp(cq::utils::ansi(path));
        if (!fp) {
            throw "🤔您还没有绑定七天学堂账号哦，请发送 .ebind 账号 密码";
        }
        ostringstream tmp;
        tmp << fp.rdbuf();
        return tmp.str();
    }
} // namespace strhandle