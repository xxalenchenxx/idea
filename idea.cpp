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
                if(sup_temp<min_sup){
                    min_sup=sup_temp;
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

    auto find_truss = [&]() -> bool {   //check all edge have truss or not
        return std::find(k_truss.begin(), k_truss.end(), -1) != k_truss.end();
    };
    auto edge_smaller_than_min_sup = [&]() -> int {
        auto it = std::find_if(sup.begin(), sup.end(), [&](int value) {
            return value <= min_sup;
        });
        if (it != sup.end()) {
            return std::distance(sup.begin(), it);
        } else {
            return -1; // 表示未找到
        }
    };

    while (find_truss()){ //find truss
        std::cout<<"---------------k="<<min_sup+2<<"-----------------"<<endl;
        int position;
        while((position=edge_smaller_than_min_sup())!=-1){ //position=j in list query_array
            int id=0;//i
            for(int id1=0;id1<node_num_query;id1++){ //find offset
                if(position<offset_array_query[id1+1]){
                    id=id1;
                    break;
                }
            }
            
            //input truss index
            // setting sup to MAX means edge dissapear
            int k=(min_sup+2);
            k_truss[position] = k;
            sup[position]=INT32_MAX;
            for(auto insert=offset_array_query[list_array_query[position]];
                insert<offset_array_query[list_array_query[position]+1];
                insert++){
                if(list_array_query[insert]==id){
                    k_truss[insert]=k;
                    sup[insert]=INT32_MAX;
                    break;
                }
            }
            //after delete minus effect edge 1
            int node1=offset_array_query[id],node2=offset_array_query[list_array_query[position]];//two pointers MNN
            while(node1<offset_array_query[id+1] && node2<offset_array_query[list_array_query[position]+1]){
                if(list_array_query[node1]==list_array_query[node2]){ //found MNN to minus support
                    sup[node1]--;
                    sup[node2]--;
                    node1++;
                    node2++;
                }else if(list_array_query[node1]<list_array_query[node2]){
                    node1++;
                }else{
                    node2++;
                }
            }
        }
        min_sup++;
    }
    //avg truss to spilte query graph
    vector<int> max_truss_node(node_num_query,0);
    double avg_truss=2.5; //only delete 2.5 truss edges
    for(auto i=0;i<node_num_query;i++){
        int max_truss=0;
        for(auto j=offset_array_query[i];j<offset_array_query[i+1];j++){
            max_truss=max(k_truss[j],max_truss);
        }
        max_truss_node[i]=max_truss;
    }
    
    vector<bool> dissapear_node(node_num_query,false); //紀錄之後再列舉的點
    vector<bool> connect_edge(k_truss.size(),false); //紀錄消失的edge
    for(int i=0;i<node_num_query;i++){   //
        if(max_truss_node[i]<avg_truss){
            dissapear_node[i]=true;
        }
    }

    for(int i=0;i<connect_edge.size();i++){   //
        if(k_truss[i]<avg_truss){
            connect_edge[i]=true;
        }
    }



    // // Print dissapear_node
    std::cout << "dissapear_node: ";
    for (const auto& node : dissapear_node) {
        std::cout << node << " ";
    }
    std::cout << std::endl;

    // // Print connect_edge
    std::cout << "connect_edge: ";
    for (const auto& edge : connect_edge) {
        std::cout << edge << " ";
    }
    std::cout << std::endl;

    // record component by BFS
    vector<bool> visited(node_num_query,false);
    queue<int>q;
    vector<vector<int>> conponents;
    for(int start=0;start<node_num_query;start++){
        if(!visited[start]){
            vector<int> conponent;
            q.push(start);
            visited[start] = true;
            while(!q.empty()){
                int travesal_point=q.front();
                conponent.push_back(travesal_point);
                q.pop();
                for(auto j=offset_array_query[travesal_point];j<offset_array_query[travesal_point+1];j++){
                    if(!visited[list_array_query[j]] && !connect_edge[j]){
                        visited[list_array_query[j]]=true;
                        q.push(list_array_query[j]);
                    }
                }
            }
            conponent.shrink_to_fit();
            conponents.push_back(conponent);
        }
    }
    conponents.shrink_to_fit();
    std::sort(conponents.begin(),conponents.end(),[](const std::vector<int>&a,std::vector<int>& b) {return a.size()>b.size(); });
    std::cout << "Components: " << std::endl;
    for (const auto& component : conponents) {
        for (const auto& node : component) {
            std::cout << node << " ";
        }
        std::cout << std::endl;
    }

    int small_enumeration_threshold=0; //component裡先列舉的
    for(int i=0; i<conponents.size();i++){
        if(max_truss_node [conponents[i][0]]<avg_truss){
            small_enumeration_threshold=i;
            break;
        }
    }

    //method 1 先用這個
    //決定前small_enumeration_threshold個component的order by (backtrack) 1.degree大
    //method 2
    //利用相互automorphism，決定優先列舉誰
    vector<vector<int>> enumeration_order_first(small_enumeration_threshold);
    priority_queue<pair<int,int>> pq;
    for(int start=0;start<small_enumeration_threshold;start++){
        visited.assign(node_num_query, false);
        vector<int> order;
        int max_degree_id=-1;
        for(int i=0;i<conponents[start].size();i++){    //degree max
            int max_value=-1;
            if((offset_array_query[conponents[start][i]+1]-offset_array_query[conponents[start][i]])>max_value){
                max_value=(offset_array_query[conponents[start][i]+1]-offset_array_query[conponents[start][i]]);
                max_degree_id=conponents[start][i];
            }
        }
        pq.push({max_truss_node[max_degree_id],max_degree_id}); //root node to backtrack
        visited[max_degree_id]=true;
        
        while(!pq.empty()){  //pick bigger truss node as priority
            auto point=pq.top();
            int travesal_point=point.second;
            order.push_back(travesal_point);
            pq.pop();
            for(auto j=offset_array_query[travesal_point];j<offset_array_query[travesal_point+1];j++){
                if(!visited[list_array_query[j]] && !connect_edge[j]){
                    visited[list_array_query[j]]=true;
                    pq.push({max_truss_node[list_array_query[j]],list_array_query[j]});
                }
            }
        }
        order.shrink_to_fit();
        enumeration_order_first[start]=order;
    }

    // // Print enumeration_order_first
    // std::cout << "Enumeration Order First: "<<enumeration_order_first.size() << std::endl;

    // for (const auto& order1 : enumeration_order_first) {
    //     for (const auto& node : order1) {
    //         std::cout << node << " ";
    //     }
    //     std::cout << std::endl;
    // }

    //sequential BFS
    int i1,j1;
    cout<<"parallel\n";
    #pragma omp parallel for
    for(i1 = 0; i1 < 3; ++ i1 )
        for(j1 = 0; j1 < 3; ++ j1 )
            cout<<i1<<","<<j1<<endl;

    auto end_t = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_t - beg_t;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds." << std::endl;

    std::cout << "max_truss: ";
    for (const auto& s : max_truss_node) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    // std::cout << "sup: ";
    // for (const auto& s : sup) {
    //     std::cout << s << " ";
    // }
    // std::cout << std::endl;
    std::cout << "k-truss: ";
    for (const auto& s : k_truss) {
        std::cout << s << " ";
    }
    std::cout << std::endl;

    return 0;
}
