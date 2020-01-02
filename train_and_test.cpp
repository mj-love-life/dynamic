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
// 通过控制dynamic_or_static进行控制动态与静态
int dynamic_or_static = 0;


// 为了训练与动态插入产生的变量
int sign_num = 0;
// 训练文件
ifstream train_file;
vector<int> sign_bits = vector<int> ();

void display(set<int> results) {
    for(set<int>::iterator i = results.begin(); i != results.end(); i++) {
        cout << *i << " ";
    }
    cout << endl;
}

// 用于预处理每一条数据并返回vector<string> [unique_id, time]
pair<int, double> pre_deal2(vector<string> arrs) {
    vector<string> unique_vec;
    for(int i = 0; i < sign_num; i++) {
        unique_vec.push_back(arrs[sign_bits[i]]);
    }
    string info = vec2str(unique_vec);
    if(block_info_id.left.count(info) == 0) {
        unique_block_info_file << block_count << endl;
        unique_block_info_file << info << endl << endl;
        block_info_id.left.insert(make_pair(info, block_count));
        block_count++;
    }
    // 时间暂时忽略了
    return make_pair(block_info_id.left.find(info)->second, get_time(arrs[12].substr(8)));
}

// 使用真实时间间隔
void physics_deal_file(ifstream &read_file) {
    cout << "deal................." << endl;
    clock_t startTime = clock();
    string request_stream;
    vector<string> info;
    // int count = 0;
    while(getline(read_file, request_stream)) {
        // count++;
        // if(count % 100 == 0) {
        //     cout << "Deal with " << count << " line" << endl;
        // }
        // TODO 切割符号
        if(request_stream.size() == 0) {
            break;
        }
        split(info, request_stream, is_any_of(" "), token_compress_on);
        pair<int, double> temp= pre_deal2(info);
        // 插入点集中
        int block_id = temp.first;
        double block_time = temp.second;
        vertexs_set.insert(block_id);
        //说明队列中没有其余的元素，也就是最开始的时候，只有一个点，也没有边产生
        if(request_queue.size() == 0){
            request_queue.push_back(block_id);
            time_queue.push_back(block_time);
        }
        else{
            // 遍历去除时间间隔不满足要求的
            while(request_queue.size() > 0 && abs(time_queue.front() - block_time) > time_threshold){
                request_queue.pop_front();
                time_queue.pop_front();
            }
            // 剩余满足要求的，其中一种情况是没有点剩下了
            request_queue.push_back(block_id);
            time_queue.push_back(block_time);
            if(request_queue.size() != 1) {
                for(int i = 0; i < request_queue.size() - 1; i++) {
                    if(request_queue[i] == block_id){
                        continue;
                    }
                    graph.insert(request_queue[i], block_id);
                }
            }
        }
    }
    cout << "The deal file time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
}

// 使用逻辑时间作为间隔
void logistic_deal_file(ifstream& read_file) {
    cout << "deal................." << endl;
    clock_t startTime = clock();
    string request_stream;
    vector<string> info;
    // int count = 0;
    while(getline(read_file, request_stream)) {
        // count++;
        // if(count % 100 == 0) {
        //     cout << "Deal with " << count << " line" << endl;
        // }
        // TODO 切割符号
        if(request_stream.size() == 0) {
            break;
        }
        split(info, request_stream, is_any_of(" "), token_compress_on);
        pair<int, double> temp= pre_deal2(info);
        // 插入点集中
        int block_id = temp.first;
        vertexs_set.insert(block_id);
        //说明队列中没有其余的元素，也就是最开始的时候，只有一个点，也没有边产生
        if(request_queue.size() == 0){
            request_queue.push_back(block_id);
        }
        else{
            // 遍历去除时间间隔不满足要求的
            if (request_queue.size() > time_threshold) {
                request_queue.pop_front();
            }
            request_queue.push_back(block_id);
            if(request_queue.size() != 1) {
                for(int i = 0; i < request_queue.size() - 1; i++) {
                    if(request_queue[i] == block_id){
                        continue;
                    }
                    graph.insert(request_queue[i], block_id);
                }
            }
        }
    }
    cout << "The deal file time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
}

// 将query的数据写入文件中
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
    split(info, request_stream, is_any_of(" "), token_compress_on);
    pair<int, double> temp= pre_deal2(info);
    return temp.first;
}

// TODO 训练函数与测试函数
// 训练集的函数

// 计算准确率的函数
int not_in_real_count = 0;
int in_real_not_pred_count = 0;
int in_real_in_pred_count = 0;
set<int> pred = set<int> ();

// 用于逻辑处理的计算准确率相关的信息（时间耗费比较大，因为需要对每条trace进行一次query）
void logistic_deal_file_train(ifstream& read_file) {
    cout << "deal................." << endl;
    clock_t startTime = clock();
    string request_stream;
    vector<string> info;
    // int count = 0;
    int line_count = 0;
    while(getline(read_file, request_stream)) {
        line_count++;
        // count++;
        // if(count % 100 == 0) {
        //     cout << "Deal with " << count << " line" << endl;
        // }
        // TODO 切割符号
        if(request_stream.size() == 0) {
            break;
        }
        split(info, request_stream, is_any_of(" "), token_compress_on);
        pair<int, double> temp= pre_deal2(info);

        // 计算准确率
        if (graph.real_graph->Real_Vertexs.count(temp.first) == 0) {
            not_in_real_count++;
        }
        else {
            if (pred.count(temp.first) == 1) {
                in_real_in_pred_count++;
            }
            else {
                in_real_not_pred_count++;
            }
            set<int> temp_set = graph.real_graph->query_processing(temp.first);
            if(pred.size() > 200) {
                pred = set<int> ();
            }
            set_union(pred.begin(), pred.end(), temp_set.begin(), temp_set.end(), inserter(pred, pred.begin()));
        }
        if(line_count % 10000 == 0) {
            cout << "---------------" << line_count  << "---------------" << endl;
            cout << "pred size is " << pred.size() << endl;
            cout << "not_in_real_count is: " << not_in_real_count << endl;
            cout << "in_real_not_pred_count is: " << in_real_not_pred_count << endl;
            cout << "in_real_in_pred_count is: " << in_real_in_pred_count << endl;
            cout << "The calculate accurate time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
        }


        // 插入点集中
        int block_id = temp.first;
        vertexs_set.insert(block_id);
        //说明队列中没有其余的元素，也就是最开始的时候，只有一个点，也没有边产生
        if(request_queue.size() == 0){
            request_queue.push_back(block_id);
        }
        else{
            // 遍历去除时间间隔不满足要求的
            if (request_queue.size() > time_threshold) {
                request_queue.pop_front();
            }
            request_queue.push_back(block_id);
            if(request_queue.size() != 1) {
                for(int i = 0; i < request_queue.size() - 1; i++) {
                    if(request_queue[i] == block_id){
                        continue;
                    }
                    graph.insert(request_queue[i], block_id);
                }
            }
        }
    }
    cout << "The deal file time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
}

int main(int argc, char **argv) {
    if (argc <= 11) {
        cout << "The argc is not enough" << endl;
        return 0;
    }
    cout << argv[1] << endl;
    // 此处添加训练文件夹

    /* 输入字符串格式:
    训练文件，测试文件，块信息文件，查询结果文件，逻辑/物理处理标示位(Y代表逻辑)，时间间隔，权重大小，密度，是否查询所有点(Y代表是)，块标示的位数，后续跟着块标示的每个位
    */
    train_file.open("./data/" + string(argv[1]));
    read_file.open("./data/" + string(argv[2]));
    unique_block_info_file.open("./info/" + string(argv[3]));
    result_file.open("./result/" + string(argv[4]));
    logistic_or_physics = string(argv[5]) == "Y";
    time_threshold = strtod(argv[6], nullptr);
    weight_threshold = strtol(argv[7], nullptr, 10);
    alpha = strtod(argv[8], nullptr);
    query_all = string (argv[9]) == "Y";
    // start_attr = strtol(argv[9], nullptr, 10);
    // end_attr = strtol(argv[10], nullptr, 10);
    sign_num = strtol(argv[10], nullptr, 10);
    for(int i = 0; i < sign_num; i++) {
        sign_bits.push_back(strtol(argv[11 + i], nullptr, 10));
    }
    if(train_file.is_open() == false) {
        cout << "训练文件没有成功打开" << endl;
        return 0;
    }
    // 静态训练
    cout << "---------------next is training---------------" << endl;
    // 设置dynamic_or_static为1用于静态训练
    dynamic_or_static = 1;
    if (logistic_or_physics) {
        logistic_deal_file(train_file);
    }
    else {
        physics_deal_file(train_file);
    }
    // 输出相关信息
    cout << "Appear Block number is " << block_info_id.size() << endl;
    cout << "Appear Edges number is " << Appear_Edge_id.size() << endl;
    cout << "global_vertex num is : " << graph.real_graph->Real_Vertexs.size() << endl;
    cout << "global edge num is : " << graph.real_graph->Used_Edges.size() << endl;
    // 构建TCP索引
    graph.real_graph->tcp_index_construction();
    cout << "global_k_max is : " <<  global_k_max << endl;
    cout << "global_update time: " << total_update_time << "s" << endl;
    if(read_file.is_open() == false) {
        cout << "文件没有成功打开" << endl;
        return 0;
    }
    cout << "---------------next is dynamic---------------" << endl;
    // 设置dynamic_or_static为0用于动态训练，也就是动态插入
    dynamic_or_static = 0;
    // continue_insert 用于标示是否持续测试
    int continue_insert = 1;
    // 
    while(continue_insert) {
        if (logistic_or_physics) {
            logistic_deal_file(read_file);
            // logistic_deal_file_train(read_file); // 用于获取准确率
            // cout << endl;
            // cout << "累积数量如下：" << endl;
            // cout << "pred size is " << pred.size() << endl;
            // cout << "not_in_real_count is: " << not_in_real_count << endl;
            // cout << "in_real_not_pred_count is: " << in_real_not_pred_count << endl;
            // cout << "in_real_in_pred_count is: " << in_real_in_pred_count << endl;
            // cout << endl;
        }
        else {
            physics_deal_file(read_file);
        }
        // 输出相关信息
        cout << "Appear Block number is " << block_info_id.size() << endl;
        cout << "Appear Edges number is " << Appear_Edge_id.size() << endl;
        cout << "global_vertex num is : " << graph.real_graph->Real_Vertexs.size() << endl;
        cout << "global edge num is : " << graph.real_graph->Used_Edges.size() << endl;
        cout << "global_k_max is : " <<  global_k_max << endl;
        cout << "global_update time: " << total_update_time <<  "s" << endl;
        cout << "Do you want to continue?" << endl;
        cin >> continue_insert;
        read_file.close();

        if(continue_insert == 0) break;
        else {
            cout << "Please input the file you want to deal: ";
            string file_name;
            cin >> file_name;
            read_file.open("./data/" + file_name);
            while(read_file.is_open() == false) {
                cout << "File not exist. Please input the file you want to deal again: ";
                cin >> file_name;
                read_file.open("./data/" + file_name);
            }
        }
    }
    write_query_file();
    unique_block_info_file.close();
    result_file.close();
    return 0;
}