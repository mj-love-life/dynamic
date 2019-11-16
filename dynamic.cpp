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
#include <sstream>
#include <fstream>
#include "Graph.cpp"
using namespace std;
using namespace boost;






// 存储出现过的边以及计数、顶点集、队列（长度与时间有关）、队列首部时间
Graph graph = Graph();
set<int> vertexs_set = set<int> ();
deque<int> request_queue = deque<int> ();
deque<double> time_queue = deque<double> ();
// 时间阈值
// double time_threshold = 1.1;
// double time_threshold = 0.00004;
// double time_threshold = 0.1;
// request_file2的时间控制

int main() {
    // 读取文件
    string request_stream;
    string file_name;
    cout << "which file do you want to deal with? \n";
    cin >> file_name;
    ifstream in(file_name + ".txt");
    if(in.is_open() == false) {
        cout << "文件没有成功打开" << endl;
        return 0;
    }

    vector<string> info;
    int count = 0;
    while(getline(in, request_stream)) {
        count++;
        if(count % 100 == 0) {
            cout << "Deal with " << count << " line" << endl;
        }
        if(count == 10000){
            break;
        }
        split(info, request_stream, is_any_of(" "));
        pair<int, double> temp= pre_deal(info);
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
                    // TODO 插入边
                    if(request_queue[i] == block_id){
                        continue;
                    }
                    graph.insert(request_queue[i], block_id);
                }
            }
            // 暂时使用graph来实现完整的ktruss
        }
        //TODO 核心代码在这里

        //TODO
        // 然后使用graph的insert函数生成对应的边的图，
        // 获取insert之后更新的图的边的次数
        // 如果超过阈值，则插入heap中，接下来继续更新


        // 只有在被删除的时候才会把次数变成0，否则继续加

    }
    graph.display();
    
    graph.Used_display();
    graph.NB_display();
    graph.k_truss_computation();
    graph.k_G_display();
    graph.t_e_display();
    int v_q, k;
    v_q = 1;
    k = 1;
    // 只有在输入查询点与k值为0的时候才退出程序

    graph.tcp_index_construction();
    graph.TCPs_display();
    // set<int> temp2 = graph.TCPs[2]->compute_Vk(1, 4, graph.Edges);
    // graph.TCPs[2]->display(graph.Edges);
    // for(set<int>::iterator i = temp2.begin(); i != temp2.end(); i++) {
    //     cout << *i << " ";
    // }
    // cout << endl;
    
    while(v_q != 0 || k != 0){
        cout << "which vertex do you want to query: " << endl;
        cin >> v_q;
        cout << "The k value you want to query: " << endl;
        cin >> k;
        cout << "----------------------K_TRUSS_INDEX-----------------" << endl;
        graph.display_query_result(v_q, k, graph.query_processing_using_k_truss_index(k, v_q));
        cout << "----------------------TCP_INDEX---------------------" << endl;
        graph.display_query_result(v_q, k, graph.query_processing_using_tcp_index(k, v_q));

    }

    cout << "Next is Lemma2 test" << endl;

    // int u, v;
    // while(cin >> u >> v) {
    //     cout << "----------------------TESTING FIND USING BFS-----------------" << endl;
    //     graph.TCPs[4]->display(graph.Edges);
    //     graph.find_path_using_bfs(4, u, v);
    // }
    
    int u, v;
    while(cin >> u >> v) {
        cout << "----------------------LINK " << u << " and " << v << " -----------------" << endl;
        display_vector(graph.compute_k12(u, v));
        graph.update_with_edge_insert(u, v);
        // TODO 下面这个函数有问题
        // graph.update_with_edge_erase(u, v);
    }
    

    in.close();
    // query vertex
    return 0;
}