#include <chrono>
#include "crow.h"
#include "fetch_all_movies.h"
#include "Movie.h"
#include "merge_sort.h"
#include "heap_sort.h"
using namespace std;

int main() {
  crow::SimpleApp app;
  vector<Movie*> movies = get_all_movies("../movies.json");
  vector<pair<float, Movie*>> sortable_movies;

  CROW_ROUTE(app, "/")([]() {
    crow::mustache::set_base("../templates/");
    return crow::mustache::load_text("index.html");
  });

  CROW_ROUTE(app, "/css/styles.css")([]() {
    crow::mustache::set_base("../templates/");
    return crow::mustache::load_text("css/styles.css");
  });

  CROW_ROUTE(app, "/images/dropdown_page.png")([]() {
    crow::response res;
    res.set_static_file_info_unsafe("../templates/images/dropdown_page.png");
    return res;
  });

  CROW_ROUTE(app, "/images/result_page.png")([]() {
    crow::response res;
    res.set_static_file_info_unsafe("../templates/images/result_page.png");
    return res;
  });

  CROW_ROUTE(app, "/js/script.js")([]() {
    crow::mustache::set_base("../templates/");
    return crow::mustache::load_text("js/script.js");
  });

  CROW_ROUTE(app, "/fetch-all-movies")([]() {
    fetch_all_movies_by_year("../movies.json", 1870, 2026);
    return "Done! Check movies.json for the results.";
  });

  CROW_ROUTE(app, "/submit").methods("POST"_method)([&movies, &sortable_movies](const crow::request& req) {
    auto body = crow::json::load(req.body);

    auto genres = body["genres"];
    vector<int> genres_list;
    for (int i = 0; i < genres.size(); i++) {
      genres_list.push_back(genres[i].i());
    }

    int year = body["year"].i();
    float popularity = body["popularity"].d();
    float rating = body["rating"].d();
    int votes = body["votes"].i();

    Movie user_query("", "", "", genres_list, year, popularity, rating, votes);
    vector<pair<float, Movie*>> merge_sortable_movies, heap_sortable_movies;

    for (Movie* movie : movies) {
      float similarity = movie->similarity_to(user_query);
      merge_sortable_movies.push_back({similarity, movie});
      heap_sortable_movies.push_back({similarity, movie});
    }

    auto merge_sort_start = chrono::steady_clock::now();
    mergeSort(merge_sortable_movies, 0, (int)merge_sortable_movies.size() - 1);
    auto merge_sort_end = chrono::steady_clock::now();
    auto merge_sort_diff = chrono::duration<double, milli>(merge_sort_end - merge_sort_start).count();
    cout << "Merge sort took " << merge_sort_diff << " ms" << endl;

    auto heap_sort_start = chrono::steady_clock::now();
    heapSort(heap_sortable_movies);
    auto heap_sort_end = chrono::steady_clock::now();
    auto heap_sort_diff = chrono::duration<double, milli>(heap_sort_end - heap_sort_start).count();
    cout << "Heap sort took " << heap_sort_diff << " ms" << endl;

    // verify that both sorting algorithms produced the same results
    if (merge_sortable_movies.size() != heap_sortable_movies.size()) {
      return crow::response("Error: Sorting algorithms produced different results!");
    }

    for (int i = 0; i < (int)merge_sortable_movies.size(); i++) {
      if (merge_sortable_movies[i].first != heap_sortable_movies[i].first) {
        return crow::response("Error: Sorting algorithms produced different results!");;
      }
    }

    sortable_movies = merge_sortable_movies;

    crow::json::wvalue res;
    for (int i = 0; i < 25; i++) {
      res["movies"][i]["title"] = sortable_movies[i].second->get_title();
      res["movies"][i]["release_year"] = sortable_movies[i].second->get_release_year();
      res["movies"][i]["overview"] = sortable_movies[i].second->get_overview();
      res["movies"][i]["poster_path"] = sortable_movies[i].second->get_poster_path();
      res["movies"][i]["similarity"] = sortable_movies[i].first;

      vector<int> genres = sortable_movies[i].second->get_genres();
      for (int j = 0; j < (int)genres.size(); j++) {
        res["movies"][i]["genres"][j] = genres[j];
      }
    }

    res["merge_sort_time_ms"] = chrono::duration<double, milli>(merge_sort_diff).count();
    res["heap_sort_time_ms"] = chrono::duration<double, milli>(heap_sort_diff).count();
    res["total"] = sortable_movies.size();
    return crow::response(res);
  });


  CROW_ROUTE(app, "/page-change").methods("POST"_method)([&sortable_movies](const crow::request& req) {
    auto page_count = crow::json::load(req.body);  // parse JSON
    int page = page_count["page"].i();
    int start = (page - 1) * 25;
    int end;

    if (start + 25 > sortable_movies.size() - 1) {
      end = sortable_movies.size();
    } else {
      end = start + 25;
    }

    crow::json::wvalue res;
    for (int i = start; i < end; i++) {
      int j = i - start;
      res["movies"][j]["title"] = sortable_movies[i].second->get_title();
      res["movies"][j]["release_year"] = sortable_movies[i].second->get_release_year();
      res["movies"][j]["overview"] = sortable_movies[i].second->get_overview();
      res["movies"][j]["poster_path"] = sortable_movies[i].second->get_poster_path();
      res["movies"][j]["similarity"] = sortable_movies[i].first;

      vector<int> genres = sortable_movies[i].second->get_genres();
      for (int k = 0; k < (int)genres.size(); k++) {
        res["movies"][j]["genres"][k] = genres[k];
      }
    }

    return crow::response(res);
  });

  app.port(8080).multithreaded().run();
}