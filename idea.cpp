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
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <query graph> <data graph> " << std::endl;
        return 1;
    }
    //-------------------input element----------------------------------
    read_file(query_graph,&node_num_query,&edge_num_query,temp_list_query);
    read_file(data_graph,&node_num_data,&edge_num_data,temp_list_data);
    
    std::cout<<"node_num_query: "<<node_num_query<<endl;
    std::cout<<"edge_num_query: "<<edge_num_query<<endl;
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
    auto beg_t = std::chrono::high_resolution_clock::now();
    //turn into CSR format
    std::vector<int> offset_array_query(node_num_query + 1, 0);
    std::vector<int> list_array_query(2*edge_num_query, 0); //undirect graph, so 2 times
    std::vector<int> offset_array_data(node_num_data + 1, 0);
    std::vector<int> list_array_data(2*edge_num_data, 0); //undirect graph

    transefer_list2CSR(temp_list_query,list_array_query,offset_array_query);
    temp_list_query.clear();
    temp_list_query.shrink_to_fit();
    transefer_list2CSR(temp_list_data,list_array_data,offset_array_data);
    temp_list_data.clear();
    temp_list_data.shrink_to_fit();

    // cout<<"offset_array_query: ";
    // for(auto &i:offset_array_query){
    //     cout<<i<<" ";
    // }
    // cout<<endl;
    // cout<<"list_array_query: ";
    // for(auto &i:list_array_query){
    //     cout<<i<<" ";
    // }
    // cout<<endl;

    // cout<<"offset_array_data: ";
    // for(auto &i:offset_array_data){
    //     cout<<i<<" ";
    // }
    // cout<<endl;
    // cout<<"list_array_data: ";
    // for(auto &i:list_array_data){
    //     cout<<i<<" ";
    // }
    // cout<<endl;
    

    //query partition(k-truss)
    vector<int> k_truss(2*edge_num_query,-1);
    vector<int> sup(2*edge_num_query,-1);
    int min_sup=INT32_MAX;
    //initialize support
    for(auto i=0;i<node_num_query;i++){
        for(auto j=offset_array_query[i];j<offset_array_query[i+1];j++){
            if(i<list_array_query[j]){ //compute edge support

                int node1=offset_array_query[i],node2=offset_array_query[list_array_query[j]];//two pointers MNN
                int sup_temp=0;
                while(node1<offset_array_query[i+1] && node2<offset_array_query[list_array_query[j]+1]){
                    if(list_array_query[node1]==list_array_query[node2]){ //found MNN
                        node1++;
                        node2++;
                        sup_temp++;
                    }else if(list_array_query[node1]<list_array_query[node2]){
                        node1++;
                    }else{
                        node2++;
                    }
                }
                sup[j]=sup_temp;

                for(auto insert=offset_array_query[list_array_query[j]];
                    insert<offset_array_query[list_array_query[j]+1];
                    insert++){
                    if(list_array_query[insert]==i){
                        sup[insert]=sup_temp;
                        break;
                    }
                }    
            }
        }
    }

    auto has_minus_one = [&]() -> bool {
        return std::find(k_truss.begin(), k_truss.end(), -1) != k_truss.end();
    };

    while (has_minus_one()){
        
    }




    auto end_t = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_t - beg_t;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds." << std::endl;

    // std::cout << "sup: ";
    // for (const auto& s : sup) {
    //     std::cout << s << " ";
    // }
    // std::cout << std::endl;

    return 0;
}
