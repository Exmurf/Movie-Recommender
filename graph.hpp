#include <vector>
#include <iostream>
#include "movie.hpp"
using namespace std;

class Graph
{
public:
    int num_users,num_items;
    vector< vector< pair<int,float> > > user_adj_list; //User adjecency list
    vector<Movie> item_adj_list; //Item adjecency list
public:
    Graph(const string& info_file_name, const string& data_file_name, const string& item_file_name); //Constructor

    void add_edge(int user,pair<int,float> pair);

    void print_graph(); // Output dosyasi olusturma fonksiyonu

    void load_data_file(const string& data_file_name);

    void load_item_file(const string& item_file_name);

    int random_recommendation(int user); // Oneri algoritması 1

    vector<int> highest_rating_recommendation(int user); // Oneri algoritması 2

    pair<int,vector<int> >  most_similar_user_recommendation(int user); // Oneri algoritması 3

    vector<pair<int,float> > shortest_weighted_path_recommendation(int user); // Oneri algoritması 4

    pair<vector<int>, int> genre_based_recommendation(int user); // Oneri algoritmasi 5

    void print_recommendation(int type,int user);

    ~Graph(){}; //Destructor
};
