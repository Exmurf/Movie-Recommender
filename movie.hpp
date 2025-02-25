#include <vector>
#include <iostream>
using namespace std;

class Movie
{
public:
    vector<int> user_list;

    float total_rating;
    float final_rating;

    int id;
    int degree;
    string title;
    vector<int> genres;

public:
    Movie() : title(""),genres(),final_rating(0.0), total_rating(0.0), degree(0){}; //Default Constructor
    Movie(int id,const string& title, const vector<int>& genres); //Normal Constructor

    const string& get_title() const; //Getter method
    float get_final_rating(); //Getter method
    int get_id(); //Getter method
    
    void rating_calc(int rating);

    ~Movie(){};//Destructor
};