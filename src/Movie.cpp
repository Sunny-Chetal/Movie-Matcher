#include "Movie.h"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace std;

Movie::Movie(const string& json) {
  // parse title
  int index = json.find("\"original_title\":\"");
  if (index != string::npos) {
    int start = index + 18;
    int end = json.find("\",", start);
    title = json.substr(start, end - start);

    // unescape quotes
    int pos = title.find("\\\"");
    while (pos != string::npos) {
      title.replace(pos, 2, "\"");
      pos = title.find("\\\"", pos + 1);
    }
  }

  // parse overview
  index = json.find("\"overview\":\"");
  if (index != string::npos) {
    int start = index + 12;
    int end = json.find("\",", start);
    overview = json.substr(start, end - start);

    // unescape quotes
    int pos = overview.find("\\\"");
    while (pos != string::npos) {
      overview.replace(pos, 2, "\"");
      pos = overview.find("\\\"", pos + 1);
    }
  }

  // parse poster path
  index = json.find("\"poster_path\":\"");
  if (index != string::npos) {
    int start = index + 15;
    int end = json.find("\",", start);
    poster_path = json.substr(start, end - start);
  }

  int search = 0;

  // parse genres
  while (true) {
    int index = json.find("\"genre_ids\":[", search);
    if (index == string::npos) break;

    int start = index + 13;
    int end = json.find("]", start);

    string genres_str = json.substr(start, end - start);
    int pos = genres_str.find(",");

    while (pos != string::npos) {
      genres.push_back(stoi(genres_str.substr(0, pos)));
      genres_str.erase(0, pos + 1);

      pos = genres_str.find(",");
    }

    if (!genres_str.empty()) {
      genres.push_back(stoi(genres_str));
    }

    search = end + 1;
  }

  // parse release year
  index = json.find("\"release_date\":");
  if (index != string::npos) {
    int start = index + 16;
    int end = json.find("\"", start);
    string date_str = json.substr(start, end - start);
    if (date_str.size() >= 4) {
      release_year = stoi(date_str.substr(0, 4));
    }
  }

  // parse popularity
  index = json.find("\"popularity\":");
  if (index != string::npos) {
    popularity = stof(json.substr(index + 13));
  }

  // parse vote average
  index = json.find("\"vote_average\":");
  if (index != string::npos) {
    vote_average = stof(json.substr(index + 15));
  }

  // parse vote count
  index = json.find("\"vote_count\":");
  if (index != string::npos) {
    vote_count = stoi(json.substr(index + 13));
  }
}

Movie::Movie(const string title, const string overview, const string poster_path, const vector<int>& genres, int release_year, float popularity, float vote_average, int vote_count) {
  // for display
  this->title = title;
  this->overview = overview;
  this->poster_path = poster_path;

  // for similarity
  this->genres = genres;
  this->release_year = release_year;
  this->popularity = popularity;
  this->vote_average = vote_average;
  this->vote_count = vote_count;
}

float Movie::similarity_to(const Movie& query) const {
  // GENRE SCORE: intersection / union
  int intersection_size = 0;
  for (int i = 0; i < (int)query.genres.size(); i++) {
    if (find(genres.begin(), genres.end(), query.genres[i]) != genres.end()) {
      intersection_size++;
    }
  }

  // AuB = A + B - AnB
  int union_size = (int)genres.size() + (int)query.genres.size() - intersection_size;

  float genre_score = union_size > 0 ? (float)intersection_size / union_size : 0.0f;

  // YEAR SCORE: all movies within 10 years of query get a score > 0, linearly decreasing to 0 at 10+ years difference
  float year_diff = abs((float)(release_year - query.release_year));
  float year_score = max(0.0f, 1.0f - year_diff / 10.0f);

  // RATING SCORE: linearly decreasing from 1 at 0 apart to 0 at 10 apart
  float rating_score = max(0.0f, 1.0f - abs(vote_average - query.vote_average) / 10.0f);

  // POPULARITY SCORE: use log scale since popularity varies so widely; linearly decreasing from 1 at 0 apart to 0 at log10(1 + 472.8603) apart
  // 472.8603 is the largest popularity in the dataset
  float log_popularity = log10(1.0f + popularity);
  float log_popularity_query = log10(1.0f + query.popularity);
  float popularity_score = max(0.0f, 1.0f - abs(log_popularity - log_popularity_query) / log10(1.0f + 472.8603f));

  // VOTE COUNT SCORE: use log scale since vote count varies widely; linearly decreasing from 1 at 0 apart to 0 at log10(1 + 39135) apart
  // 39135 is the largest vote count in the dataset
  float log_vote_count = log10(1.0f + (float)vote_count);
  float log_vote_count_query = log10(1.0f + (float)query.vote_count);
  float vote_count_score = max(0.0f, 1.0f - abs(log_vote_count - log_vote_count_query) / log10(1.0f + 39135.0f));

  // arbitrary weights: genre is most important, then rating, then year, then popularity, then vote count
  return 0.35f * genre_score
       + 0.20f * year_score
       + 0.25f * rating_score
       + 0.12f * popularity_score
       + 0.08f * vote_count_score;
}

void Movie::print() const {
  cout << "Movie: release_year=" << release_year << ", popularity=" << popularity << ", rating=" << vote_average << ", votes=" << vote_count << ", genres=[";

  for (int i = 0; i < (int)genres.size(); i++) {
    cout << genres[i];

    if (i < (int)genres.size() - 1) {
      cout << ",";
    }
  }

  cout << "]" << endl;
}

string Movie::get_title() const {
  return title;
}

string Movie::get_overview() const {
  return overview;
}

string Movie::get_poster_path() const {
  return poster_path;
}

int Movie::get_release_year() const {
  return release_year;
}

vector<int> Movie::get_genres() const {
  return genres;
}