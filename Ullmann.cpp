#include<iostream>
#include <algorithm>
#include"class.h"
using namespace std;

int main(int argc, char **argv){
    //read file list ->CSR
    vector<list<int>> temp_list_query;
    string query_graph = argv[1];
    int node_num =0, edge_num =0;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <query graph> <data graph> " << std::endl;
        return 1;
    }
    //-------------------input element----------------------------------
    read_file(query_graph,&node_num,&edge_num,temp_list_query);
    // read_file(data_graph,&node_num,&edge_num,temp_list_data);
    
    std::cout<<"node_num: "<<node_num<<endl;
    std::cout<<"edge_num: "<<edge_num<<endl;
    std::cout<<"temp list"<<endl;
    for(auto i=0;i<temp_list_query.size();i++){
        std::cout<<i<<": ";
        for(const auto& node : temp_list_query[i])
            cout<< node <<" ";
        std::cout<<endl;
    }
    
    //turn into CSR format
    std::vector<int> offset_array_query(node_num + 1, 0);
    std::vector<int> list_array_query(edge_num, 0);

    transefer_list2CSR(temp_list_query,list_array_query,offset_array_query);


    cout<<"done"<<endl;
    std::cout << "offset_array: ";
    for (const auto &i : offset_array_query)
        std::cout << i << " ";
    std::cout << std::endl;

    std::cout << "list_array: ";
    for (const auto &i : list_array_query)
        std::cout << i << " ";
    std::cout << std::endl;

    // temp_list_query.clear();
    // temp_list_query.shrink_to_fit();
    return 0;
}
