#include "movie.hpp"

Movie::Movie(int id,const string& title, const vector<int>& genres) 
: id(id), title(title), genres(genres), final_rating(0.0), degree(0), total_rating(0){}


void Movie::rating_calc(int rating){
    total_rating += rating;
    degree++;
    if(degree>0){
        final_rating = total_rating / degree;
    }
}

const string& Movie::get_title() const {
        return title;
    }

int Movie::get_id(){
    return id;
}

float Movie::get_final_rating(){
    return final_rating;
}