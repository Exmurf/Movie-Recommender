#include "graph.hpp"
#include <iostream>
#include <fstream> 
#include <sstream>
#include <unordered_set>
#include <algorithm> 
#include <iomanip>
#include <queue>
#include <vector>

#define MIN_RATING_COUNT 10 // Öneri algoritmaları için minimum rating kriteri
#define RECOMMEDNATION_COUNT 5 // Öneri sayısı

using namespace std;

//Graph constructor
Graph::Graph(const string& info_file_name, const string& data_file_name, const string& item_file_name){
        ifstream info_file(info_file_name);
        if(!info_file){
            cerr<<"Info dosyasi acilamadi!"<<endl;
            return;
        }
        if (!(info_file >> num_users)) {
        cerr << "Error reading number of users!" << endl;
        return;
        }
        // Ignore the word "users"
        string users_label;
        info_file >> users_label;

        // Read number of items
        if (!(info_file >> num_items)) {
            cerr << "Error reading number of items!" << endl;
            return;
        }
        // Ignore the word "items"
        string items_label;
        info_file >> items_label;
        info_file.close();

        user_adj_list.resize(num_users + 1);
        item_adj_list.resize(num_items + 1);

        load_item_file(item_file_name);
        load_data_file(data_file_name);

        srand(time(0));
    }

//Film verilerinin cekildigi fonksiyon
void Graph::load_item_file(const string& item_file_name){
    ifstream item_file(item_file_name);
    if(!item_file){
        cerr<<"Item file could not be opened!"<<endl;
        return;
    }

    string line;
    while(getline(item_file,line)){
        try {
            stringstream ss(line);
            
            int movie_id;
            string title, release_date, imdb_url;
            vector<int> genres(19);
            
            ss >> movie_id;
            ss.ignore();
            getline(ss, title, '|');
            getline(ss, release_date, '|');
            ss.ignore();
            getline(ss, imdb_url, '|');    

            for (int i = 0; i < 19; ++i) {
                ss >> genres[i];
                if(i<18) ss.ignore();
            }
            item_adj_list[movie_id] = Movie(movie_id,title, genres);
        } 
        catch (const exception& e) {
            cerr << "Error parsing item: " << e.what() << endl;
        }
    }
    item_file.close();
}

//Graph verilerinin cekildigi fonksiyon
void Graph::load_data_file(const string& data_file_name){
    ifstream data_file(data_file_name);
    if(!data_file){
        cerr<<"Data file could not be opened!"<<endl;
        return;
    }

    string line;
    while(getline(data_file,line)){
        stringstream ss(line);
        int user,item,rating,timestamp;
        ss >> user >> item >> rating >> timestamp;
        pair<int,float> movie_paired = make_pair(item,rating);
        Graph::add_edge(user,movie_paired);
    }
    data_file.close();
    
}

//Graph baglantilarini ekleyen fonksiyon
void Graph::add_edge(int user,pair<int,float> pair){
        user_adj_list[user].push_back(pair);
        item_adj_list[pair.first].user_list.push_back(user);
        item_adj_list[pair.first].rating_calc(pair.second);
        
}

//Kullanicinin daha once izlemedigi filmler arasindan tamamen rassal olarak
//film oneren algoritma.
int Graph::random_recommendation(int user){
    vector<bool> watched(num_items + 1, false);

    for(int item = 0; item < user_adj_list[user].size(); item++){
        watched[user_adj_list[user][item].first] = true;
    }

    vector<int> unwatched;
    for(int i=1; i<=num_items; i++){
        if(!watched[i]) unwatched.push_back(i);
    }

    if(unwatched.empty()) return -1;

    int rand_index = rand() % unwatched.size();
    return unwatched[rand_index];
}

//Yardimci fonksiyonlar
bool compare_by_rating(const pair<int, float>& a, const pair<int, float>& b) {
    return a.second > b.second;
}

bool compare_by_distance(const pair<int, float>& a, const pair<int, float>& b) {
    return a.second > b.second;
}

bool does_contain(const pair<int,float>& pair,int item){
    return pair.first == item;
}

//Kullanicinin daha once izlemedigi filmler arasindan
//ratingi en yuksek filmleri oneren algoritma.
vector<int> Graph::highest_rating_recommendation(int user){
    vector<bool> watched(num_items + 1, false);

    for(int item = 0; item < user_adj_list[user].size(); item++){
        watched[user_adj_list[user][item].first] = true;
    }

    vector< pair<int, float> > unwatched_movies;
    for (int i = 1; i <= num_items; i++) {
        if (!watched[i] && item_adj_list[i].degree >= MIN_RATING_COUNT) {
            unwatched_movies.push_back(make_pair(i, item_adj_list[i].final_rating));
        }
    }

    sort(unwatched_movies.begin(), unwatched_movies.end(),compare_by_rating);

    vector<int> top_movies;
    for (int i = 0; i < min(RECOMMEDNATION_COUNT, (int)unwatched_movies.size()); i++) {
        top_movies.push_back(unwatched_movies[i].first); 
    }

    return top_movies;
}

//Hedef kullanicinin en fazla ortak film izledigi kullanici ile benzerlik kurarak, bu
//kullanicinin en cok begendigi filmleri oneren bir algoritma.
pair<int,vector<int> > Graph::most_similar_user_recommendation(int user){
    int max_common_movies = 0;
    int most_similar_user = -1;

    unordered_set<int> target_user_movies;
    for (int i = 0; i < user_adj_list[user].size(); i++) {
        target_user_movies.insert(user_adj_list[user][i].first);
    }
    for(int other_user=1; other_user<=num_users;other_user++){
        if(other_user==user) continue;

        int common_movies = 0;
        for (size_t j = 0; j < user_adj_list[other_user].size(); ++j) {
            int current_movie_id = user_adj_list[other_user][j].first;
            if (target_user_movies.count(current_movie_id)) common_movies++;
        }
        if(common_movies>max_common_movies){
            max_common_movies = common_movies;
            most_similar_user = other_user;
        }
    }

    if(most_similar_user == -1)return make_pair(most_similar_user, vector<int>());

    vector< pair<int,float> > recommendation_list;
    for (int i = 0; i < user_adj_list[most_similar_user].size(); i++) {
        int movie_id = user_adj_list[most_similar_user][i].first;
        float rating = user_adj_list[most_similar_user][i].second;

        if (!target_user_movies.count(movie_id) && item_adj_list[movie_id].degree >= MIN_RATING_COUNT) {
            recommendation_list.push_back(make_pair(movie_id,rating));
        }
    }
    sort(recommendation_list.begin(),recommendation_list.end(),compare_by_rating);

    vector<int> top_list;
    for(int i=0;i<min((int)recommendation_list.size(),RECOMMEDNATION_COUNT);i++){
        top_list.push_back(recommendation_list[i].first);
    }
    return make_pair(most_similar_user,top_list);
}

//Hedef kullaniciya en yakin agirlikli uzakliga sahip filmleri oneren bir algoritma.
vector<pair<int, float>> Graph::shortest_weighted_path_recommendation(int user) {
    vector<float> movie_distances(num_items + 1, __FLT_MAX__); // Baslangicta tum filmler sonsuz mesafede
    vector<bool> watched_movies(num_items + 1, false);         
    vector<bool> visited(num_items + 1, false);             

    //Izlenen filmler isaretlenir
    for (int i = 0; i < user_adj_list[user].size(); i++) {
        int movie_id = user_adj_list[user][i].first;
        float given_rating = user_adj_list[user][i].second;
        watched_movies[movie_id] = true;
        movie_distances[movie_id] = given_rating; //Ilk mesafe olarak verilen rating atanır
    }

    // Dijkstra icin Priority queue yapisi
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<pair<float, int>>> pq;

    //Izlenen filmler baslangic olarak eklenir
    for (int i = 0; i < user_adj_list[user].size(); i++) {
        int movie_id = user_adj_list[user][i].first;
        float given_rating = user_adj_list[user][i].second;
        pq.push({given_rating, movie_id});
    }

    // Dijkstra 
    while (!pq.empty()) {
        float current_distance = pq.top().first;
        int current_movie_id = pq.top().second;
        pq.pop();

        // Eğer bu dugum zaten ziyaret edildiyse atlanir
        if (visited[current_movie_id]) continue;
        visited[current_movie_id] = true;

        // Bu filmi izleyen kullanicilar uzerinden ilerle
        for (int i = 0; i < item_adj_list[current_movie_id].user_list.size(); i++) {
            int user_id = item_adj_list[current_movie_id].user_list[i]; // Bu filme rating veren kullanıcı

            // Kullanıcının izledigi diğer filmleri kontrol et
            for (int j = 0; j < user_adj_list[user_id].size(); j++) {
                int next_movie_id = user_adj_list[user_id][j].first; 
                float next_movie_rating = user_adj_list[user_id][j].second;
                float new_distance = current_distance + next_movie_rating; // Yeni mesafeyi hesapla

                // Eğer yeni mesafe daha kısa ise
                if (new_distance < movie_distances[next_movie_id]) {
                    movie_distances[next_movie_id] = new_distance; // Mesafeyi güncelle
                    pq.push({new_distance, next_movie_id}); // Kuyruga yeni mesafeyi ekle
                }
            }
        }
    }

    // Izlenmemiş filmleri topla
    vector<pair<int, float>> unwatched_movies;
    for (int i = 1; i <= num_items; i++) {
        if (!watched_movies[i] && movie_distances[i] < __FLT_MAX__) {
            unwatched_movies.push_back({i, movie_distances[i]});
        }
    }

    // Mesafeye göre sıralama (küçükten büyüğe)
    sort(unwatched_movies.begin(), unwatched_movies.end(),
         [](const pair<int, float>& a, const pair<int, float>& b) { return a.second < b.second; });

    vector<pair<int, float>> top_list;
    for (int i = 0; i < min(RECOMMEDNATION_COUNT, (int)unwatched_movies.size()); i++) {
        top_list.push_back(unwatched_movies[i]);
    }

    return top_list; 
}

//Hedef kullanicinin en cok izledigi ture gore oneri yapan algoritma
pair<vector<int>, int> Graph::genre_based_recommendation(int user) {
    vector<int> genre_counts(19, 0); // 19 tür için bir sayaç (hangi tür kaç kez izlenmiş)

    // Kullanıcının izlediği filmleri analiz ederek tür sayacı oluştur
    for (int i = 0; i < user_adj_list[user].size(); i++) {
        int movie_id = user_adj_list[user][i].first;
        for (int j = 0; j < item_adj_list[movie_id].genres.size(); j++) {
            if (item_adj_list[movie_id].genres[j] == 1) {
                genre_counts[j]++;
            }
        }
    }

    // En çok izlenen türü bul
    int most_watched_genre = -1;
    int max_count = 0;
    for (int i = 0; i < genre_counts.size(); i++) {
        if (genre_counts[i] > max_count) {
            max_count = genre_counts[i];
            most_watched_genre = i; // Türün ID'si
        }
    }

    // İzlenmemiş filmleri bul ve sadece en çok izlenen türe ait olanları topla
    vector<pair<int, float>> suitable_movies; // {film_id, rating}
    if (most_watched_genre != -1) {
        for (int i = 1; i <= num_items; i++) {
            // Film izlenmemiş ve ilgili türe ait mi?
            if (find_if(user_adj_list[user].begin(), user_adj_list[user].end(),
                        [i](const pair<int, float>& p) { return p.first == i; }) == user_adj_list[user].end()) {
                // Eğer film türü doğruysa ve MIN_RATING_COUNT şartını sağlıyorsa
                if (item_adj_list[i].genres[most_watched_genre] == 1 &&
                    item_adj_list[i].degree >= MIN_RATING_COUNT) {
                    suitable_movies.push_back({i, item_adj_list[i].get_final_rating()});
                }
            }
        }
    }

    // Filmleri yüksekten düşüğe sıralama (rating'e göre)
    sort(suitable_movies.begin(), suitable_movies.end(),
         [](const pair<int, float>& a, const pair<int, float>& b) {
             return a.second > b.second;
         });

    // Öneri listesini oluştur
    vector<int> recommendations;
    for (int i = 0; i < min(RECOMMEDNATION_COUNT, (int)suitable_movies.size()); i++) {
        recommendations.push_back(suitable_movies[i].first);
    }

    return {recommendations, most_watched_genre}; // Öneri listesini ve en çok izlenen türü döndür
}

//Graph'in printlendigi dosyayi olusturan fonksiyon.
void Graph::print_graph() {
    ofstream output_file("graph.txt");
    if (!output_file) {
        cerr << "Output file could not be opened!" << endl;
        return;
    }

    output_file << "User-Item interactions: " << endl;
    for (int i = 1; i <= num_users; i++) {
        output_file << "User " << i << ": ";
        for (int j=0; j<user_adj_list[i].size();j++) {
            output_file << user_adj_list[i][j].first << " ";
        }
        output_file << endl;
    }
    output_file << endl;

    output_file << "Item-User interactions: " << endl;
    for (int i = 1; i <= num_items; i++) {
    output_file << "Item " << i << " (Rating: " 
                << fixed << setprecision(2) << item_adj_list[i].get_final_rating() << "/5) : ";
    for (int j = 0; j < item_adj_list[i].user_list.size(); j++) {
        output_file << item_adj_list[i].user_list[j] << " ";
    }
    output_file << endl;
    }
}

//Onerilerin printlendigi dosyayi olusturan/ekleme yapan fonksiyon.
void Graph::print_recommendation(int type, int user) {
    ofstream output_file("recommendations.txt", std::ios::app);
    if (!output_file) {
        cerr << "Output file could not be opened!" << endl;
        return;
    }

    switch (type) {
        case (1): // Oneri algoritmasi 1
        {
            output_file << endl;
            output_file << "Random unwatched movie recommendations for user " << user << ": " << endl;
            for (int i = 0; i < RECOMMEDNATION_COUNT; i++) {
                int rand_movie_index = random_recommendation(user);
                Movie recommended_movie = item_adj_list[rand_movie_index];
                output_file << i + 1 << " - "
                            << "(" << recommended_movie.get_id() << ") " << recommended_movie.get_title()
                            << " (Rating: " << fixed << setprecision(2) << recommended_movie.get_final_rating() << "/5)"
                            << endl;
            }
        }
            break;

        case (2): // Oneri algoritmasi 2
        {
            output_file << endl;
            output_file << "Highly rated unwatched movie recommendations for user " << user << ": " << endl;
            vector<int> recommendation_list = highest_rating_recommendation(user);
            for (int i = 0; i < recommendation_list.size(); i++) {
                Movie recommended_movie = item_adj_list[recommendation_list[i]];
                output_file << i + 1 << " - "
                            << "(" << recommended_movie.get_id() << ") " << recommended_movie.get_title()
                            << " (Rating: " << fixed << setprecision(2) << recommended_movie.get_final_rating() << "/5)"
                            << endl;
            }
        }
            break;

        case (3): // Oneri algoritmasi 3
        {
            pair<int, vector<int>> recommend_list = most_similar_user_recommendation(user);
            if (recommend_list.first != -1) {
                output_file << endl
                            << "User " << user << " has the most similarity with user " << recommend_list.first << "."
                            << endl;
                output_file << "Similar movies you might like:" << endl;
                for (int i = 0; i < recommend_list.second.size(); i++) {
                    Movie recommended_movie = item_adj_list[recommend_list.second[i]];
                    output_file << i + 1 << " - "
                                << "(" << recommended_movie.get_id() << ") " << recommended_movie.get_title()
                                << " (Rating: " << fixed << setprecision(2)
                                << recommended_movie.get_final_rating() << "/5)" << endl;
                }
            } else {
                output_file << endl << "No unwatched movies available for user " << user << endl;
            }
        }
            break;

        case (4): // Oneri algoritmasi 4
        {
            output_file << endl
                        << "Shortest weighted path recommendations for user " << user << ":" << endl;

            vector<pair<int, float>> recommendation_list = shortest_weighted_path_recommendation(user);
            for (int i = 0; i < recommendation_list.size(); i++) {
                Movie recommended_movie = item_adj_list[recommendation_list[i].first];
                output_file << i + 1 << " - "
                            << "(" << recommended_movie.get_id() << ")" << recommended_movie.get_title()
                            << " (Rating: " << fixed << setprecision(2)
                            << recommended_movie.get_final_rating() << "/5)" << "\n"
                            << " (Weighted Distance: " << fixed << setprecision(2)
                            << recommendation_list[i].second << ")" << endl;
            }
        }
            break;

        case (5): // Genre-based öneri algoritması
        {
            output_file << endl;
            output_file << "Genre-based movie recommendations for user " << user << ": " << endl;

            // Öneri listesini ve en çok izlenen tür ID'sini al
            auto [recommendation_list, most_watched_genre] = genre_based_recommendation(user);

            // En çok izlenen türün adını yazdır
            static const vector<string> genre_names = {
                "Unknown", "Action", "Adventure", "Animation", "Children's", "Comedy", "Crime", "Documentary",
                "Drama", "Fantasy", "Film-Noir", "Horror", "Musical", "Mystery", "Romance", "Sci-Fi", "Thriller",
                "War", "Western"};

            if (most_watched_genre != -1) {
                output_file << "Most watched genre: " << genre_names[most_watched_genre] << endl;
            } else {
                output_file << "No specific genre found." << endl;
            }

            // Önerilen filmleri yazdır
            for (int i = 0; i < recommendation_list.size(); i++) {
                Movie recommended_movie = item_adj_list[recommendation_list[i]];
                output_file << i + 1 << " - "
                            << "(" << recommended_movie.get_id() << ") " << recommended_movie.get_title()
                            << " (Rating: " << fixed << setprecision(2)
                            << recommended_movie.get_final_rating() << "/5)" << endl;
            }
        }
            break;
        default:
            output_file << "Invalid recommendation type!" << endl;
    }
}
