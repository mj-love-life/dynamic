#ifndef TRIANGLE_CONNECTIVITY_PRESERVED_INDEX
#define TRIANGLE_CONNECTIVITY_PRESERVED_INDEX
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <stack>
#include <algorithm>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <set>
#include <deque>
#include <ctime>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/vector_of.hpp>
#include <boost/bimap/tags/tagged.hpp>

using namespace boost::bimaps;
using namespace boost;
using namespace std;


// bimap 仅仅支持只读的结构
// 改成bimap双向map
// block info to unique id
extern bimap<string, int> block_info_id;
bimap<string, int> block_info_id = bimap<string, int> ();

// 用于双向存储
// edge vertex to edge unique id
extern bimap<vector<int>, int> Appear_Edge_id;
bimap<vector<int>, int> Appear_Edge_id =bimap<vector<int>, int> ();


// 用于记录所有的Block的hash_id
extern int block_count;
extern int global_k_max;
int global_k_max = 0;
int block_count = 0;
extern int edge_threshold;
extern double time_threshold;
// 用于记录边的结构

// 全局变量
extern map<int, int> Weight;
map<int, int> Weight = map<int, int> ();


// 时间复杂度：O(m + n)
set<int> get_map_key_intersection(map<int, int> a, map<int, int> b) {
    set<int> result = set<int> ();
    map<int, int>::iterator i = a.begin();
    map<int, int>::iterator j = b.begin();
    for(; i != a.end() && j != b.end();  ) {
        if (i->first == j->first) {
            result.insert(i->first);
            i++;
            j++;
        }
        else if (i->first < j->first) {
            i++;
        }
        else {
            j++;
        }
    }
    return result;
}

set<int> get_neighbor_set(set<int> u_nb, set<int> v_nb) {
    set<int> inter_section = set<int> ();
    set_intersection(u_nb.begin(), u_nb.end(), v_nb.begin(), v_nb.end(), inserter(inter_section, inter_section.begin()));
    return inter_section;
}


// ascending
bool ascending_cmp(const pair<int, set<int> > &p1, const pair<int, set<int>> &p2) {
	return p1.second.size()< p2.second.size();
}

bool ascending_cmp2(const pair<int, int> &p1, const pair<int, int> &p2) {
    return p1.second< p2.second;
}

bool descending_cmp(const pair<int, int> &p1, const pair<int, int> &p2) {
    return p1.second > p2.second;
}

vector<int> get_edge_help(int u, int v) {
    if(u > v) {
        swap(u, v);
    }
    vector<int> result = {u, v};
    return result;
}

vector<pair<int, int> > sort_map(map<int, int> NBs, bool ascending=false) {
    vector<pair<int, int> > result(NBs.begin(), NBs.end());
    if(ascending) {
        sort(result.begin(), result.end(), ascending_cmp2);
    }
    else {
        sort(result.begin(), result.end(), descending_cmp);
    }
    return result;
}


struct TCP_index {
    int unique_id;
    set<int> NBs;
    int k_max;

    // 子图的边
    map<int, int> G_x;
    set<int> MST;

    TCP_index(int unique_id) {
        this->unique_id = unique_id;
        NBs = set<int> ();
        k_max = 0;
        G_x = map<int, int> ();
        MST = set<int> ();
    }

    void get_k_value(int k) {
        k_max = max(k_max, k);
        global_k_max = max(global_k_max, k_max);
    }

    void insert_neighbor(int nb) {
        NBs.insert(nb);
    }

    set<int> getNB() {
        return NBs;
    }

    void insert_G_x(int edge_index, int w) {
        G_x[edge_index] = w;
        k_max = max(k_max, w);
        global_k_max = max(global_k_max, k_max);
    }

    vector<pair<int, int> > get_descending_G_x() {
        vector<pair<int, int> > descending_G_x(G_x.begin(), G_x.end());
        sort(descending_G_x.begin(), descending_G_x.end(), descending_cmp);
        return descending_G_x;
    }

    // 查询两个点是否在一个集合中
    void query_in_MST() {
        // TODO
    }

    void display() {
        cout << "----------------------------------------" << endl;
        cout << "Block_id is " << unique_id << endl;
        cout << "Neighbors have : ";
        for(set<int>::iterator i = NBs.begin(); i != NBs.end(); i++) {
            cout << *i << " ";
        }
        cout << endl << "k_max is : " << k_max << endl;
    }
};


struct Real_Graph {
    map<int, int> Real_Edges_Trussness;
    map<int, TCP_index *> Real_Vertexs;
    set<int> Used_Edges;
    
    Real_Graph() {
        Real_Edges_Trussness = map<int,int> ();
        Real_Vertexs = map<int, TCP_index *> ();
        Used_Edges = set<int> ();
    }

    void insert(vector<int> edge) {
        int edge_index = Appear_Edge_id.left.find(edge)->second;
        if (Used_Edges.count(edge_index) == 0) {
            Used_Edges.insert(edge_index);
            this->insert_vertex(edge[0], edge[1]);
            this->insert_vertex(edge[1], edge[0]);
        }
    }

    void insert_vertex(int u, int v) {
        if (Real_Vertexs.count(u) == 0) {
            Real_Vertexs[u] = new TCP_index(u);
        }
        Real_Vertexs[u]->insert_neighbor(v);        
    }

    deque<pair<int, set<int> > > get_sup(int & max_size) {
        map<int, set<int> > result = map<int, set<int> >();
        max_size = 0;
        for(set<int>::iterator i = Used_Edges.begin(); i != Used_Edges.end(); i++) {
            vector<int> Edge_index = Appear_Edge_id.right.find(*i)->second;
            result[*i] = get_neighbor_set(Real_Vertexs[Edge_index[0]]->NBs, Real_Vertexs[Edge_index[1]]->NBs);
            max_size = max(max_size, int(result[*i].size()));
        }
        deque<pair<int, set<int> > > temp_result(result.begin(), result.end());
        // TODO 暂时使用快排
        sort(temp_result.begin(), temp_result.end(), ascending_cmp);
        return temp_result;
    }

    // TODO 待改进
    void decrese_one(int edge_index,  deque<pair<int, set<int> > >& sups, int vertex) {
        int i = 0;
        for(; i != sups.size(); i++) {
            if (sups[i].first == edge_index) {
                sups[i].second.erase(vertex);
                break;
            }
        }
        int now_index = i;
        int now_size = sups[i].second.size();
        i--;
        for(; i >= 0; i--) {
            if(sups[i].second.size() == now_size && i != now_index) {
                swap(sups[i+1], sups[now_index]);
            }
        }
    }

    void display(deque<pair<int, set<int> > > sups) {
        for(int i = 0; i < sups.size(); i++) {
            cout << "(" << sups[i].first << ", " << sups[i].second.size() <<")";
        }
        cout << endl;
    }

    void truss_decomposition() {
        clock_t startTime = clock();
        int max_size = 0;
        deque<pair<int, set<int> > > sups = get_sup(max_size);
        set<int> Used_Edges_temp = Used_Edges;
        int k = 2;
        while(Used_Edges_temp.size() > 0) {
            // display(sups);
            while(sups.size() > 0 && sups[0].second.size() <= (k - 2)) {
                // cout << "hhh" << endl;
                pair<int, set<int> > lowest_e = sups[0];
                sups.pop_front();
                int u = Appear_Edge_id.right.find(lowest_e.first)->second[0];
                int v = Appear_Edge_id.right.find(lowest_e.first)->second[1];
                if (Real_Vertexs[u]->getNB().size() > Real_Vertexs[v]->getNB().size()) {
                    swap(u, v);
                }
                for(set<int>::iterator i = lowest_e.second.begin(); i != lowest_e.second.end(); i++) {
                    decrese_one(Appear_Edge_id.left.find(get_edge_help(*i, u))->second, sups, v);
                    decrese_one(Appear_Edge_id.left.find(get_edge_help(*i, v))->second, sups, u);
                }
                Real_Edges_Trussness[lowest_e.first] = k;
                //Real_Vertexs[u]->get_k_value(k);
                //Real_Vertexs[v]->get_k_value(k);
                Used_Edges_temp.erase(lowest_e.first);
            }
            k = k + 1;
        }
        cout << "The truss decomposition time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }

    int get_triangle_w(int x, int y, int z) {
        return min( Real_Edges_Trussness[Appear_Edge_id.left.find(get_edge_help(x, y))->second], \
        min( Real_Edges_Trussness[Appear_Edge_id.left.find(get_edge_help(z, y))->second],\
        Real_Edges_Trussness[Appear_Edge_id.left.find(get_edge_help(x, z))->second]));
    }

    // 完成每个点的Gx以及kmax的计算
    void cal_G_x() {
        for(map<int, TCP_index *>::iterator i = Real_Vertexs.begin(); i != Real_Vertexs.end(); i++) {
            for(set<int>::iterator z = i->second->getNB().begin(); z != i->second->getNB().end(); z++) {
                for(set<int>::iterator y = i->second->getNB().begin(); y != z; y++) {
                    if(Real_Edges_Trussness.count(Appear_Edge_id.left.find(get_edge_help(*y, *z))->second) == 1) {
                        int y_z_index = Real_Edges_Trussness[Appear_Edge_id.left.find(get_edge_help(*y, *z))->second];
                        int w_temp = get_triangle_w(i->first, *y, *z);
                        i->second->insert_G_x(y_z_index, w_temp);
                    }
                }
            }
        }
    }

    void tcp_index_construction() {
        this->truss_decomposition();
        this->cal_G_x();
        // 计算生成最小生成树
        for(map<int, TCP_index *>::iterator i = Real_Vertexs.begin(); i != Real_Vertexs.end(); i++) {
            vector<pair<int, int> > descending_G_x = i->second->get_descending_G_x();
            int temp_index = 0;
            for(int k = i->second->k_max; k >= 2; k--) {
                for(;descending_G_x[temp_index].second == k; temp_index++) {
                    // TODO
                }
            }
        }
    }


    void display() {
        for(map<int, TCP_index *>::iterator i = Real_Vertexs.begin(); i != Real_Vertexs.end(); i++) {
            i->second->display();
        }
    }
};


struct Appear_Graph {
    Real_Graph * real_graph;

    Appear_Graph() {
        real_graph = new Real_Graph();
    }

    // 插入一条新出现的边
    void insert(int a, int b) {
        vector<int> index = get_edge_help(a, b);
        if(Appear_Edge_id.left.count(index) == 0) {
            // 此处使下标从1开始，因为在query的过程中需要进行逆向的操作
            int edge_index = Appear_Edge_id.size() + 1;
            Appear_Edge_id.left.insert(make_pair(index, edge_index));
            Weight[edge_index] = 1;
        }
        else {
            Weight[Appear_Edge_id.left.find(index)->second]++;
        }
        if(Weight[Appear_Edge_id.left.find(index)->second] >= edge_threshold) {
            real_graph->insert(index);
        }

    }

    void display_detail(set<int> results) {
        
    }

};
#endif