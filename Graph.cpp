#ifndef EDGES_CPP
#define EDGES_CPP
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

vector<int> help_index(int u, int v) {
    vector<int> index = vector<int> ();
    if (u > v) {
        swap(u, v);
    }
    index.push_back(u);
    index.push_back(v);
    return index;
}

struct TCP_index
{
    /* data */
    // 存储杜颖的
    int block_id;
    // 存储邻居集
    set<int> Neighbors;
    // 存储子图的边
    set<int> G_x;
    // 存储权重
    map<int, int> Weight;
    // 存储构成MST的边
    set<int> MST;

    int k_max;

    TCP_index(int block_id) {
        this->block_id = block_id;
        Neighbors = set<int> ();
        G_x = set<int> ();
        Weight = map<int, int> ();
        MST = set<int> ();
        k_max = 2;
    }

    void insert_neighbor(int neigh) {
        Neighbors.insert(neigh);
    }

    void erase_neighbor(int neigh) {
        Neighbors.erase(neigh);
    }

    void insert_edge(int edge_index) {
        G_x.insert(edge_index);
    }

    void erase_edge(int edge_index) {
        G_x.erase(edge_index);
    }

    void cal_weight(map<int, int> trussness) {
        for(set<int>::iterator i = G_x.begin(); i != G_x.end(); i++) {
            vector<int> temp_vertes = vector<int> ();
            temp_vertes = index2edges[*i]->get_vertex();
            Weight[*i] = get_triangle_weight(block_id, temp_vertes[0], temp_vertes[1], trussness);
            k_max = max(k_max, Weight[*i]);
        }
    }

    void create_MST() {
        // 先计算出Sk
        


        for(int k = k_max; k >= 2; k--) {

        }
    }

    void update_MST(set<int> insert_vertexs, set<int> insert_edges) {

    }

    void update_MST(int erase_vertex) {

    }

    void query_path(int u, int v, int &index, set<int> &path) {

    }
};

struct Edge
{
    /* data */
    int weight;
    vector<int> vertexs;

    Edge(int u, int v) {
        if (u > v) {
            swap(u, v);
        }
        vertexs.push_back(u);
        vertexs.push_back(v);
        weight = 1;
    }

    int increase() {
        weight++;
        // TODO 其余的操作
        return weight;
    }

    void reset() {
        weight = 1;
    }

    vector<int> get_vertex() {
        return vertexs;
    }
};

map<int, Edge *> index2edges;
    // 为了迅速找到两点的下标
map<vector<int>, int> vertexs2index;


int get_triangle_weight(int x, int y, int z, map<int, int> trussness) {
    return min( trussness[vertexs2index[help_index(x, y)]], \
    min( trussness[vertexs2index[help_index(y, z)]],\
    trussness[vertexs2index[help_index(x, z)]]));
}

struct Real_Graph
{
    map<int, TCP_index *> TCPs;
    map<int, int> trussness;
    set<int> Vertexs;
    set<int> Edges;
    queue<int> Edges_queue;
    int Real_Graph_size;
    bool dynamic;             // 用于区分动态与非动态
    Real_Graph(int graph_size=100) {
        TCPs = map<int, TCP_index *> ();
        trussness = map<int, int> ();
        Vertexs = set<int> ();
        Edges = set<int> ();
        Edges_queue = queue<int> ();
        Real_Graph_size = graph_size;
        dynamic = false;
    }

    // 插入边的时候构造TCPs，以及更新NB
    void insert_edges(int edge_index) {
        Edges.insert(edge_index);
        vector<int> temp_vertexs = index2edges[edge_index]->get_vertex();
        insert_vertex(temp_vertexs[0], temp_vertexs[1]);
        insert_vertex(temp_vertexs[1], temp_vertexs[0]);
    }

    void insert_vertex(int u, int v) {
        if (Vertexs.count(u) == 0) {
            Vertexs.insert(u);
            TCPs[u] = new TCP_index(u);
            TCPs[u]->Neighbors.insert(v);
        }
        else{
            TCPs[u]->Neighbors.insert(v);
        }
    }

    // 第一次创建G_x
    void cal_G_x() {
        for(set<int>::iterator i = Vertexs.begin(); i != Vertexs.end(); i++) {
            for(set<int>::iterator z = TCPs[*i]->Neighbors.begin(); z != TCPs[*i]->Neighbors.end(); z++) {
                for(set<int>::iterator y = TCPs[*i]->Neighbors.begin(); y != z; y++) {
                    if(vertexs2index.count(help_index(*y, *z)) == 1) {
                        TCPs[*i]->G_x.insert(vertexs2index[help_index(*y, *z)]);
                    }
                }
            }
        }
    }



    /* data */
};

struct Edges_Set
{
    // 记录顶点
    set<int> Vertexs;
    // 为了迅速找到下标对应的边
    


    /* data */
};

#endif