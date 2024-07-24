#ifndef class_H
#define class_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <set>
#include <queue>
#include <map>
using namespace std;

//struct node
// typedef struct node{
//     int vertex=-1;
//     int sup=INT32_MAX;
//     int k=-1;
//     int lowerBound_k=INT32_MAX;
//     int upperBound_k=INT32_MAX;

// }NODE;





//class graph
class Graph{
    public:
        int m,n;
        vector<list<int>> adj;
        Graph(){};

        void addEdge(int u, int v){      
            adj[u].push_back(v); 
            adj[v].push_back(u);
            return; 
        }

        void removeEdge(int u, int v){
            adj[u].remove_if([v](int& n) { return n== v; });
            adj[v].remove_if([u](int& n) { return n == u; });
            return;
        }

        

};


bool read_file(string filename ,int *nodes,int *edges,vector<list<int>> &temp_list){
    ifstream myfile;string str;
    int source = 0;
    int target = 0;
    //open file
    myfile.open(filename);
    if(!myfile.is_open()){
        cout << "Cannot open input file :(" << endl;
        return false;
    }

    while(getline(myfile, str)){
        stringstream ss(str);
        string token;

        ss >> token;
        if(token == "Nodes"){ //Nodes
            ss >> (*nodes);
            temp_list.resize(*nodes);
            //cout << "Nodes: " << *nodes << endl;
        }
        else if(token == "Edges"){ //Edges
            ss >> (*edges);
            //cout << "Edges: " << *edges << endl;
        }
        else {
            source= stoi(token);
            ss >> target;
            //insert edge
            // int* s = new int;
            // int* t = new int; 
            // *s=source;
            // *t=target;
            temp_list[source].push_back(target); 
            temp_list[target].push_back(source);
        }    
    }
    
    return true;
}

void transefer_list2CSR(vector<list<int>> temp_list_data,vector<int> &list_array,vector<int> &offset_array){
    for (auto i = 0; i < temp_list_data.size(); i++) {
        int j = 0;
        for (auto it = temp_list_data[i].begin(); it != temp_list_data[i].end(); it++) {
            list_array[offset_array[i] + j] = *it;
            j++;
        }
        offset_array[i + 1] = j + offset_array[i];
    }
    //sort CSR array
    // #pragma omp parallel for
    for(int i=0;i<(offset_array.size()-1);i++){       
        std::sort(list_array.begin() + offset_array[i], list_array.begin() + offset_array[i + 1]);
    }
    return;
}

#endif