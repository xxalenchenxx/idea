#include<iostream>
#include <algorithm>
#include"class.h"
// using namespace std;


struct IndexedList {
    int index;
    std::list<int> lst;
};

int main(int argc, char **argv){
    //read file list ->CSR
    vector<list<int>> temp_list_query,temp_list_data;
    string query_graph = argv[1];
    string data_graph = argv[2];
    int node_num_query =0, edge_num_query =0;
    int node_num_data =0, edge_num_data =0;
    // if (argc < 3) {
    //     std::cerr << "Usage: " << argv[0] << " <query graph> <data graph> " << std::endl;
    //     return 1;
    // }
    //-------------------input element----------------------------------
    read_file(query_graph,&node_num_query,&edge_num_query,temp_list_query);
    read_file(data_graph,&node_num_data,&edge_num_data,temp_list_data);
    
    // std::cout<<"node_num: "<<node_num_query<<endl;
    // std::cout<<"edge_num: "<<edge_num_query<<endl;
    // std::cout<<"temp list"<<endl;
    // for(auto i=0;i<temp_list_query.size();i++){
    //     std::cout<<i<<": ";
    //     for(const auto& node : temp_list_query[i])
    //         cout<< node <<" ";
    //     std::cout<<endl;
    // }
     
    // std::cout<<"node_num_data: "<<node_num_data<<endl;
    // std::cout<<"edge_num_data: "<<edge_num_data<<endl;
    // std::cout<<"temp list"<<endl;
    // for(auto i=0;i<temp_list_data.size();i++){
    //     std::cout<<i<<": ";
    //     for(const auto& node : temp_list_data[i])
    //         cout<< node <<" ";
    //     std::cout<<endl;
    // }

    //turn into CSR format
    // std::vector<int> offset_array_query(node_num_query + 1, 0);
    // std::vector<int> list_array_query(2*edge_num_query, 0); //undirect graph
    // std::vector<int> offset_array_data(node_num_data + 1, 0);
    // std::vector<int> list_array_data(2*edge_num_data, 0); //undirect graph

    // transefer_list2CSR(temp_list_query,list_array_query,offset_array_query);
    // temp_list_query.clear();
    // temp_list_query.shrink_to_fit();
    // transefer_list2CSR(temp_list_data,list_array_data,offset_array_data);
    // temp_list_data.clear();
    // temp_list_data.shrink_to_fit();

    // 創建包含索引和列表的結構
    std::vector<IndexedList> indexedList(node_num_query);
    std::vector<int> origin2sort(node_num_query);
    for (int i = 0; i < node_num_query; ++i) 
        indexedList[i] = {i, temp_list_query[i]};
    // 根據degree的大小對結構進行排序
    std::sort(indexedList.begin(), indexedList.end(), 
    [](const IndexedList& a, const IndexedList& b) {return a.lst.size() > b.lst.size();}
    );
    //因為有重新order，所以要有新的map
    for(int i = 0; i < node_num_query; ++i){
        origin2sort[i]=indexedList[i].index;
    }

    // 建立矩陣 A
    vector<vector<bool>> A(node_num_query,vector<bool>(node_num_query,0));
    for (int i = 0; i < node_num_query; ++i) 
        for (const auto& val : indexedList[i].lst)
            A[i][origin2sort[val]] = true;


    vector<vector<bool>> M(node_num_query,vector<bool>(node_num_data,0));  //query node*data graph node
    for(int q=0; q < node_num_query; ++q){
        for(int d=0; d < node_num_data; ++d){
            if(indexedList[q].lst.size()<=temp_list_data[d].size()){ //if data degree >= query degree
                M[q][d]=true;
            }
        }
    }

    vector<vector<bool>> B(node_num_data,vector<bool>(node_num_data,0));  //query node*data graph node
    for (int i = 0; i < node_num_data; ++i) 
        for (const auto& val : temp_list_data[i])
            B[i][val] = true;

    // Print B matrix
    std::cout << "B matrix:" << std::endl;
    for (int i = 0; i < node_num_data; ++i) {
        for (int j = 0; j < node_num_data; ++j) {
            std::cout << B[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
