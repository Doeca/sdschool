#include <string>
#include <vector>
#include <iostream>
#include <dolores/dolores.hpp>
#include <cqcppsdk/cqcppsdk.h>

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
                if (tmp != "")
                {
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

    string user_dir(dolores::Current<MessageEvent> &current) {
        string path = dir::app() + "\\" + to_string(current.event.user_id) + "\\";
        dir::create_dir_if_not_exists(path);
        return path;
    }
} // namespace strhandle