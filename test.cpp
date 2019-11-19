#include <iostream>
#include <cstdio>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/vector_of.hpp>
#include <boost/bimap/tags/tagged.hpp>
#include <vector>
#include <algorithm>

using namespace boost::bimaps;
using namespace boost;
using namespace std;
// 此处使用双向容器


vector<int> vector_help(int u, int v) {
    if (u > v) {
        swap(u, v);
    }
    return vector<int>{u, v};
}

void display(vector<int> edge_vertex) {
    cout << "(" <<  edge_vertex[0] << " " << edge_vertex[1] << ")";
}

template< class MapType > void print_map(const MapType & m)

{
    typedef typename MapType::const_iterator const_iterator;
    for( const_iterator iter = m.begin(), iend = m.end(); iter != iend; ++iter ) {
        display(iter->first);
        std::cout << "-->" << iter->second << std::endl;
    }
}

int main() {
    int u, v;
    bimap<vector<int>, int> block_id;
    int count = 0;
    while(1) {
        cin >> u >> v;
        if (u == 0 && v == 0) {
            break;
        }
        cout << block_id.left.count(vector_help(u, v)) << endl;
        block_id.left.insert(make_pair(vector_help(u, v), count));
        count++;
    }
    display(block_id.right.find(1)->second);
    cout << endl;
    print_map(block_id.left);
}