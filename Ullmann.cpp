#include<iostream>
#include <algorithm>
#include"class.h"
// using namespace std;


struct IndexedList { //紀錄order by degree前後的index
    int index;
    std::list<int> lst;
};

vector<vector<bool>> matrix_multiply(vector<vector<bool>> A,vector<vector<bool>> B){
    int n = A.size();
    int m = B.size();
    int p = B[0].size();
    
    vector<vector<bool>> result(n, vector<bool>(p, false));
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < m; k++) {
                result[i][j] = result[i][j] || (A[i][k] && B[k][j]);
            }
        }
    }
    
    return result;
}

vector<vector<bool>> matrix_transpose(vector<vector<bool>> A){
    int n = A.size();
    int m = A[0].size();
    
    vector<vector<bool>> result(m, vector<bool>(n));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            result[j][i] = A[i][j];
        }
    }

    return result;
}

void DFS_ULLmann(vector<vector<bool>> M,vector<int> enumeration,int enumerate_it,vector<vector<bool>> A,vector<vector<bool>> B,vector<vector<int>> &output){
    if(enumerate_it==enumeration.size()){
        vector<vector<bool>> ans;
        vector<vector<bool>> M_pron(M.size(),vector<bool>(M[0].size(),0));
        // cout<<"enumeration:"<<endl;
        // for(const auto& i:enumeration){
        //     cout<<i<<" ";
        // }
        // cout<<endl;
        for(int i=0;i<enumeration.size();i++)
            M_pron[i][enumeration[i]]=true;
        
        ans=matrix_multiply(M_pron,matrix_transpose(matrix_multiply(M_pron,B))); //M'(M'B)T
        //cout<<ans.size()<<" , "<<ans[0].size()<<endl;
        // std::cout << "M matrix:" << std::endl;
        // for (int i = 0; i < M.size(); ++i) {
        //     for (int j = 0; j < M[0].size(); ++j) {
        //         std::cout << M[i][j] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        // std::cout << "B matrix:" << std::endl;
        // for (int i = 0; i < B.size(); ++i) {
        //     for (int j = 0; j < B[0].size(); ++j) {
        //         std::cout << B[i][j] << " ";
        //     }
        //     std::cout << std::endl;
        // }




        for(auto i = 0; i < A.size(); i++) {    //鑒察是否跟A一樣
            for(auto j = 0; j < A.size(); j++) {
                if(A[i][j]==1&&ans[i][j]==0){ //No match!!
                    cout<<"No match!!!"<<endl;
                    return;
                }
            }
        }
        output.push_back(enumeration);
        cout<<"find mapping!!!"<<endl;
        return;
    }
    cout<<"enumerate_it: "<<enumerate_it<<endl;
    for(int i=0;i<M[0].size();i++){
        if(M[enumerate_it][i]){
            vector<int> enumeration_temp=enumeration;
            vector<vector<bool>> M_temp=M;
            enumeration_temp[enumerate_it]=i;
            for(auto delete_it=0;delete_it<M.size();delete_it++) //delete map v to avoid repeated enumeration
                M_temp[delete_it][i]=false;
             DFS_ULLmann(M_temp,enumeration_temp,(enumerate_it+1),A,B,output);
        }
    }
    return;
}


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

    //time start here
    // 創建包含索引和列表的結構
    auto beg_t = std::chrono::high_resolution_clock::now();
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

    // 建立矩陣 M
    vector<vector<bool>> M(node_num_query,vector<bool>(node_num_data,0));  //query node*data graph node
    for(int q=0; q < node_num_query; ++q){
        for(int d=0; d < node_num_data; ++d){
            if(indexedList[q].lst.size()<=temp_list_data[d].size()){ //if data degree >= query degree
                M[q][d]=true;
            }
        }
    }

    // 建立矩陣 B
    vector<vector<bool>> B(node_num_data,vector<bool>(node_num_data,0));  //query node*data graph node
    for (int i = 0; i < node_num_data; ++i) 
        for (const auto& val : temp_list_data[i])
            B[i][val] = true;

    // Print B matrix
    // std::cout << "B matrix:" << std::endl;
    // for (int i = 0; i < node_num_data; ++i) {
    //     for (int j = 0; j < node_num_data; ++j) {
    //         std::cout << B[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    // std::cout << "A matrix:" << std::endl;
    // for (int i = 0; i < node_num_query; ++i) {
    //     for (int j = 0; j < node_num_query; ++j) {
    //         std::cout << A[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    // std::cout << " origin2sort:" << std::endl;
    // for (int j = 0; j < origin2sort.size(); ++j) {
    //     std::cout << " origin["<<j<<"]"<<origin2sort[j]<<endl;
    // }
    // std::cout << std::endl;
    

    vector<vector<int>> output;
    vector<int> enumeration(node_num_query,-1);
    int enumerate_it=0;
    DFS_ULLmann(M,enumeration,enumerate_it,A,B,output);
    vector<vector<int>> output2origin; //轉回 order
    //time stop here
    auto end_t = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_t - beg_t;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds." << std::endl;
    // std::cout << "output matrix:" << std::endl;
    // for (const auto& row : output) {
    //     for (const auto& val : row) {
    //         std::cout << val << " ";
    //     }
    //     std::cout << std::endl;
    // }

    for(auto& result : output) {
        vector<int> temp(node_num_query,-1);
        for(auto i=0;i<result.size();i++){
            temp[indexedList[i].index]=result[i];
        }
        output2origin.push_back(temp);
    }

    // std::cout << "output2origin matrix:" << std::endl;
    // for (const auto& row : output2origin) {
    //     for (const auto& val : row) {
    //         std::cout << val << " ";
    //     }
    //     std::cout << std::endl;
    // }

    int num_columns = output2origin[0].size();
    sort(output2origin.begin(), output2origin.end(), [num_columns](const vector<int>& a, const vector<int>& b) {
        for (int i = 0; i < num_columns; ++i) {
            if (a[i] != b[i]) {
                return a[i] < b[i];
            }
        }
        return false; // a 和 b 完全相同
    });

    size_t pos = query_graph.find_last_of("/\\");
    std::string filename1 = (pos == std::string::npos) ? query_graph : query_graph.substr(pos + 1);
    size_t pos1 = data_graph.find_last_of("/\\");
    std::string filename2 = (pos1 == std::string::npos) ? data_graph : data_graph.substr(pos1 + 1);
    //outputfile
    std::ofstream outputFile("dataset_ans/Q"+filename1+"_D"+filename2);
    for (const auto& row : output2origin) {
        for (const auto& val : row) {
            outputFile << val << " ";
        }
        outputFile << std::endl;
    }
    outputFile.close();
    std::cout << "end" << std::endl;


    return 0;
}
