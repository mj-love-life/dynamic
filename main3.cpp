#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <time.h>
#include <cstdlib>
#include <queue>
#include <map>
#include <deque>
#include <set>
#include "utils.hpp"
#include "Triangle_Connectivity_Preserved_Index.hpp"
#include "res.hpp"
using namespace std;
using namespace boost;


// 存储出现过的边以及计数、顶点集、队列（长度与时间有关）、队列首部时间
Appear_Graph graph = Appear_Graph();
set<int> vertexs_set = set<int> ();
deque<int> request_queue = deque<int> ();
deque<double> time_queue = deque<double> ();

ifstream read_file;
ofstream unique_block_info_file, result_file;
double time_threshold, alpha;
bool logistic_or_physics, query_all;
int start_attr, end_attr;
int dynamic_or_static = 0;
void display(set<int> results) {
    for(set<int>::iterator i = results.begin(); i != results.end(); i++) {
        cout << *i << " ";
    }
    cout << endl;
}



void write_query_file() {
    cout << "query................" << endl;
    clock_t startTime = clock();
    for(map<int, TCP_index *>::iterator i = graph.real_graph->Real_Vertexs.begin(); i != graph.real_graph->Real_Vertexs.end(); i++) {
        result_file << i->first << endl;
        result_file << i->second->k_max << endl;
        set<int> result = graph.real_graph->query_processing(i->first);
        for(set<int>::iterator j = result.begin(); j != result.end(); j++) {
            result_file << *j << " ";
        }
        result_file << endl << endl;
    }
    cout << "The query and write time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
}

int get_id() {
    string request_stream;
    getline(cin, request_stream);
    vector<string> info;
    split(info, request_stream, is_any_of(" "));
    pair<int, double> temp= pre_deal(info);
    return temp.first;
}



void deal_file(int start, int end) {
    cout << "deal................." << endl;
    clock_t startTime = clock();
    string request_stream;
    vector<string> info;
    // int count = 0;
    int node1, node2;
    // 有待改成全局变量方便进行调整
    for(int i = start; i < end; i++) {
        read_file >> node1 >> node2;
        graph.insert(node1, node2);
    }


    // while(getline(read_file, request_stream)) {
    //     // count++;
    //     // if(count % 100 == 0) {
    //     //     cout << "Deal with " << count << " line" << endl;
    //     // }
    //     split(info, request_stream, is_any_of(" "));
    //     cout << request_stream<< endl;
    //     cout << info[0] << " " << info[1] << endl;
    //     node1 = stoi(info[0]);
    //     node2 = stoi(info[1]);
    //     graph.insert(node1, node2);
    // }
    cout << "The deal file time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
}

int main(int argc, char **argv) {
    if (argc != 11) {
        cout << "The argc is not enough" << endl;
        return 0;
    }
    cout << argv[1] << endl;
    read_file.open("./data/" + string(argv[1]));
    unique_block_info_file.open("./info/" + string(argv[2]));
    result_file.open("./result/" + string(argv[3]));
    logistic_or_physics = string(argv[4]) == "Y";
    time_threshold = strtod(argv[5], nullptr);
    weight_threshold = strtol(argv[6], nullptr, 10);
    alpha = strtod(argv[7], nullptr);
    query_all = string (argv[8]) == "Y";
    start_attr = strtol(argv[9], nullptr, 10);
    end_attr = strtol(argv[10], nullptr, 10);
    if(read_file.is_open() == false) {
        cout << "文件没有成功打开" << endl;
        return 0;
    }
    dynamic_or_static = 1;
    deal_file(0, 1000);
    // 该步骤包含的函数已经基本优化到单线程以及该结构下极限
    graph.real_graph->tcp_index_construction();
    cout << "global_vertex num is : " << graph.real_graph->Real_Vertexs.size() << endl;
    cout << "global edge num is : " << graph.real_graph->Used_Edges.size() << endl;
    cout << "global_k_max is : " <<  global_k_max << endl;
    dynamic_or_static = 0;
    deal_file(1000, 3387388);
    cout << "global_vertex num is : " << graph.real_graph->Real_Vertexs.size() << endl;
    cout << "global edge num is : " << graph.real_graph->Used_Edges.size() << endl;
    cout << "global_k_max is : " <<  global_k_max << endl;
    write_query_file();
    // read_file.close();
    // read_file.open("./data/" + string(argv[1]));
    read_file.close();
    unique_block_info_file.close();
    result_file.close();
    return 0;
}