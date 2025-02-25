#include "graph.hpp"
#include <fstream>

int main(){
    Graph Graph("u.info","u.data","u.item");
    Graph.print_graph();

    clock_t start,end;
    double time1,time2,time3,time4,time5;

    if (std::ifstream("recommendations.txt").good()) {
        std::ofstream output_file("recommendations.txt");
        if (!output_file){
            cout<<"File deletion error!\n";
            return 1;
        }
    }

    int user=50;
    cout<<"Enter a user id (0-"<<Graph.num_users<<"): ";
    cin>>user;

    start = clock();
    Graph.print_recommendation(1,user);
    end = clock();
    time1 = static_cast<double> (end-start) / CLOCKS_PER_SEC;

    start = clock();
    Graph.print_recommendation(2,user);
    end = clock();
    time2 = static_cast<double> (end-start) / CLOCKS_PER_SEC;

    start = clock();
    Graph.print_recommendation(3,user);
    end = clock();
    time3 = static_cast<double> (end-start) / CLOCKS_PER_SEC;

    start = clock();
    Graph.print_recommendation(4,user);
    end = clock();
    time4 = static_cast<double> (end-start) / CLOCKS_PER_SEC;

    start = clock();
    Graph.print_recommendation(5,user);
    end = clock();
    time5 = static_cast<double> (end-start) / CLOCKS_PER_SEC;

    int column_width=40;
    printf("\n");
    printf("%-*s | Process Time\n", 
    column_width, "Algorithm");
    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    printf("%-*s | %lfs\n", column_width,"1-Random Recommendation",time1);
    printf("%-*s | %lfs\n", column_width,"2-Highest Rating Recommendation",time2);
    printf("%-*s | %lfs\n", column_width,"3-Most Similar User Recommendation",time3);
    printf("%-*s | %lfs\n", column_width,"4-Shortest Weighted Path Recommendation",time4);
    printf("%-*s | %lfs\n", column_width,"5-Genre Based Recommendation",time5);
    printf("\n");
    return 0;
}