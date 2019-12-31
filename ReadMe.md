- 环境要求：
    - 需要boost库:1.67.0
    - g++，gcc version:7.4.0 （只需要支持c++11即可:4.8.4的可以跑）
    - 单核单线程，所以对cpu没有特别需求
- 运行指令：
    - 编译：g++ train_and_test.cpp -o train_and_test -std=c++11
    - 运行：

```shell
./train_and_test 训练文件名 动态测试文件 块信息文件 所有块查询结果文件 逻辑/物理处理标示位（Y代表逻辑）时间间隔 边阈值 图密度 是否查询所有的点（Y代表是） 块标示的位数 对应的块标示的每个位的位置


example：

训练文件名：file2.txt（放在data文件夹下）
动态测试文件名：file1.txt（放在data文件夹下）
块信息文件：block_info.txt（生成于info文件夹）
所有块查询结果文件：result.txt（生成于result文件夹）
逻辑/物理处理标示位：逻辑处理（Y）
时间间隔：3个逻辑块
边阈值：3
图密度：1
是否查询所有的点：是（Y）

其中一条trace为：
100000000000003 100000004 W 100000000000000 0 0 90000000003 20000000005 89997 1 1 7/3 1204_00:21:23.952710 0 1 903 0

其中100000000000000 0 0 90000000003 20000000005分别为deviceid、segmentindex、extentid、devoffset、segoffset，若认为这是块的标示，那么 块标示的位数 为5，而 对应的块标示的每个位的位置 为 3，4,5,6,7（从0开始数起）

那么运行指令如下：

./train_and_test  file2.txt file1.txt block_info.txt result.txt Y 3 3 1 Y 5 3 4 5 6 7
```

- **如果需要计算acc情况，可以将train_and_test.cpp line16注释掉，并将line17-324的注释去掉后再次运行**
- 主要的变量：
  - dynamic_or_static（train_and_test.cpp line30）：用于控制动态与静态
  - graph（train_and_test.cpp line19）：存储整个图信息
    - 有关操作：
      -  void insert(int a, int b) （Triangle_Connectivity_Preserved_Index.hpp line1654）：用于插入边，动静态皆可
    - 包含的变量：real_graph（存储真实图信息），主要操作如下：
      - graph.real_graph->tcp_index_construction（Triangle_Connectivity_Preserved_Index.hpp line768）：用于从静态图开始构建TCP索引结构，包含k-truss分解的过程
      - graph.real_graph->query_processing（Triangle_Connectivity_Preserved_Index.hpp line808）：用于查询指定点的ktruss结构

