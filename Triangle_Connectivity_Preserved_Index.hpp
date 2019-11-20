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
#include <queue>

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
    int k_max;

    // 子图的边
    map<int, int> G_x;
    set<int> MST;
    // 并查集
    map<int, int> vertex2union;
    map<int, set<pair<int, int>, greater<pair<int, int> > > > NBs;
    TCP_index(int unique_id) {
        this->unique_id = unique_id;
        NBs = map<int, set<pair<int, int>, greater<pair<int, int> > > > ();
        k_max = 0;
        G_x = map<int, int> ();
        MST = set<int> ();
        vertex2union = map<int, int> ();
    }

    void get_k_value(int k) {
        k_max = max(k_max, k);
        global_k_max = max(global_k_max, k_max);
    }

    void insert_neighbor(int nb) {
        NBs[nb] = set<pair<int, int>, greater<pair<int, int> > > ();
    }

    map<int, set<pair<int, int>, greater<pair<int, int> > > > getNB() {
        return NBs;
    }

    set<int> get_NB_set() {
        set<int> result = set<int> ();
        for(map<int, set<pair<int, int>, greater<pair<int, int> > > >::iterator i = NBs.begin(); i != NBs.end(); i++) {
            result.insert(i->first);
        }
        return result;
    }

    vector<pair<int, int> > get_sortNB(map<int, int> Real_Edges_Trussness) {
        map<int, int> result = map<int, int> ();
        for(map<int, set<pair<int, int>, greater<pair<int, int> > > >::iterator i = NBs.begin(); i != NBs.end(); i++) {
            int edge_index = Appear_Edge_id.left.find(get_edge_help(i->first, unique_id))->second;
            result[i->first] = Real_Edges_Trussness[edge_index];
        }
        return sort_map(result, false);
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

    bool query_in_one_union(int u, int v) {
        if (vertex2union.count(u) == 0 || vertex2union.count(v) == 0) {
            if (vertex2union.count(u) == 0) vertex2union[u] = u;
            if (vertex2union.count(v) == 0) vertex2union[v] = v;
            return false;
        }
        return find_union_index(u) == find_union_index(v);
    }

    int find_union_index(int u) {
        return vertex2union[u] == u ? u : find_union_index(vertex2union[u]);
    }

    void union_two_vertex(int u, int v) {
        vertex2union[find_union_index(u)] = find_union_index(v);
    }

    void insert_MST_static(int edge_index) {
        MST.insert(edge_index);
        int u = Appear_Edge_id.right.find(edge_index)->second[0];
        int v = Appear_Edge_id.right.find(edge_index)->second[1];
        NBs[u].insert(make_pair(G_x[edge_index], v));
        NBs[v].insert(make_pair(G_x[edge_index], u));
    }

    set<int> get_MST() {
        return MST;
    }

    set<int> compute_Vk(int y, int k) {
        // 使用bfs来产生
        set<int> result = set<int> ();
        result.insert(y);
        queue<int> index_temp = queue<int> ();
        for(set<pair<int, int>, greater<pair<int, int> > >::iterator i = NBs[y].begin(); i != NBs[y].end() && i->first >= k; i++) {
            result.insert(i->second);
            index_temp.push(i->second);
        }
        while(index_temp.size() > 0) {
            int u = index_temp.front();
            index_temp.pop();
            for(set<pair<int, int>, greater<pair<int, int> > >::iterator i = NBs[u].begin(); i != NBs[u].end() && i->first >= k; i++) {
                if(result.count(i->second) == 0) {
                    result.insert(i->second);
                    index_temp.push(i->second);
                }
            }
        }
        return result;
    }

    // 查询两个点是否在一个集合中
    void query_in_MST() {
        // TODO 
    }

    void display() {
        cout << "----------------------------------------" << endl;
        cout << "Block_id is " << unique_id << endl;
        cout << "Neighbors have : " << endl;;
        for(map<int, set<pair<int, int>, greater<pair<int, int> > > >::iterator i = NBs.begin(); i != NBs.end(); i++) {
            cout << i->first << " ";
            for(set<pair<int, int>, greater<pair<int, int> > >::iterator j = i->second.begin(); j != i->second.end(); j++) {
                cout << "[" << j->first << ", " << j->second << "], ";
            }
            cout << endl;
        }
        cout << "k_max is : " << k_max << endl;
        cout << "Gx is as followed" << endl;
        for(map<int, int>::iterator i = G_x.begin(); i != G_x.end(); i++) {
            cout << "index is " << i->first << " : (" << Appear_Edge_id.right.find(i->first)->second[0] << ", " << Appear_Edge_id.right.find(i->first)->second[1] << "), the weight is " << i->second << endl;
        }
        cout << "vertex2union is as followed " << endl;
        for(map<int, int>::iterator i = vertex2union.begin(); i != vertex2union.end(); i++) {
            cout << i->first << " " << i->second << endl;
        }
        cout << "MST is as followed" << endl;
        for(set<int>::iterator i = MST.begin(); i != MST.end(); i++) {
            cout << *i << " ";
        }
        cout << endl;
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
            result[*i] = get_neighbor_set(Real_Vertexs[Edge_index[0]]->get_NB_set(), Real_Vertexs[Edge_index[1]]->get_NB_set());
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

    void tcp_index_construction() {
        this->truss_decomposition();
        // this->cal_G_x();
        clock_t startTime = clock();
        for(map<int, TCP_index *>::iterator i = Real_Vertexs.begin(); i != Real_Vertexs.end(); i++) {
            // 完成每个点的Gx以及kmax的计算
            map<int, set<pair<int, int>, greater<pair<int, int> > > > NBs = i->second->getNB();
            for(map<int, set<pair<int, int>, greater<pair<int, int> > > >::iterator z = NBs.begin(); z != NBs.end(); z++) {
                for(map<int, set<pair<int, int>, greater<pair<int, int> > > >::iterator y = z; y != NBs.end(); y++) {
                    if(y == z) continue;
                    if(Real_Edges_Trussness.count(Appear_Edge_id.left.find(get_edge_help(y->first, z->first))->second) == 1) {
                        int y_z_index = Appear_Edge_id.left.find(get_edge_help(y->first, z->first))->second;
                        int w_temp = get_triangle_w(i->first, y->first, z->first);
                        i->second->insert_G_x(y_z_index, w_temp);
                    }
                }
            }
            // 计算生成最小生成树
            // 此处相当于获得Sk
            vector<pair<int, int> > descending_G_x = i->second->get_descending_G_x();
            int G_x_size = descending_G_x.size();
            int temp_index = 0;
            for(int k = i->second->k_max; k >= 2; k--) {
                for(;temp_index < G_x_size && descending_G_x[temp_index].second == k; temp_index++) {
                    int y = Appear_Edge_id.right.find(descending_G_x[temp_index].first)->second[0];
                    int z = Appear_Edge_id.right.find(descending_G_x[temp_index].first)->second[1];
                    if (! i->second->query_in_one_union(y, z)){
                        i->second->union_two_vertex(y, z);
                        i->second->insert_MST_static(descending_G_x[temp_index].first);
                    }
                }
                // 如果边的数目等于点的数目-1，则可以终止程序
                if(i->second->get_MST().size() == NBs.size() - 1) {
                    break;
                } 
            }
        }
        cout << "The TCP index construction time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }

    set<int> query_processing(int Vq, int k=0) {
        if( Real_Vertexs.count(Vq) == 0) {
            cout << "this Vertex dose not exist in the real graph" << endl;
            return set<int>();
        }
        clock_t startTime = clock();
        set<int> result = set<int> ();
        if (k == 0) {
            k = Real_Vertexs[Vq]->k_max;
        }
        set<int> visited = set<int> ();
        vector<pair<int, int> > Vq_Neighbor = Real_Vertexs[Vq]->get_sortNB(Real_Edges_Trussness);
        for(int i = 0; i < Vq_Neighbor.size(); i++) {
            if (Vq_Neighbor[i].second < k) {
                break;
            }
            bool flag = (Vq > Vq_Neighbor[i].first);
            int vq_u = Appear_Edge_id.left.find(get_edge_help(Vq, Vq_Neighbor[i].first))->second;
            int temp_count = flag? visited.count(-1 * vq_u) : visited.count(vq_u);
            if (Vq_Neighbor[i].second >= k && temp_count == 0) {
                stack<int> Q = stack<int> ();
                flag ? Q.push(-1 * vq_u): Q.push(vq_u);
                while(Q.size() > 0) {
                    int x_y = Q.top();
                    Q.pop();
                    if(visited.count(x_y) == 0) {
                        int x, y;
                        if(x_y < 0) {
                            x_y *= (-1);
                            vector<int> temp = Appear_Edge_id.right.find(x_y)->second;
                            x = temp[1];
                            y = temp[0];
                        }
                        else {
                            vector<int> temp = Appear_Edge_id.right.find(x_y)->second;
                            x = temp[0];
                            y = temp[1];
                        }
                        set<int> Vk_x_y = Real_Vertexs[x]->compute_Vk(y, k);
                        for(set<int>::iterator z = Vk_x_y.begin(); z != Vk_x_y.end(); z++) {
                            int x_z = Appear_Edge_id.left.find(get_edge_help(x, *z))->second;
                            if(x > *z) {
                                x_z *= -1;
                            }
                            visited.insert(x_z);
                            result.insert(x);
                            result.insert(*z);
                            x_z *= -1;
                            if (visited.count(x_z) == 0) {
                                visited.insert(x_z);
                            }
                        }
                    }
                }
            }
        }
        cout << "The query processing time is : " << (double) (clock() - startTime) / (1.0 * CLOCKS_PER_SEC) << "s" << endl;
        return result;
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