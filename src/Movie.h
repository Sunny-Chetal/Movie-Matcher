#pragma once
#include <string>
#include <vector>

class Movie {
  // for display
  std::string title;
  std::string overview;
  std::string poster_path;

  // for similarity
  std::vector<int> genres;
  int release_year;
  float popularity;
  float vote_average;
  int vote_count;

public:
  Movie(const std::string& json);
  Movie(const std::string title, const std::string overview, const std::string poster_path, const std::vector<int>& genres, int release_year, float popularity, float vote_average, int vote_count);

  // Returns a similarity score in [0, 1] against a query Movie.
  float similarity_to(const Movie& query) const;

  void print() const;

  std::string get_title() const;
  std::string get_overview() const;
  std::string get_poster_path() const;
  int get_release_year() const;
  std::vector<int> get_genres() const;
};
