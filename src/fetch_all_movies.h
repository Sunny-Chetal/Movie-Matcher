#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include "Movie.h"

// HTTP helper
std::string get_tmdb_data(const std::string& url);

// JSON parsing helpers
int parse_total_pages(const std::string& json);
int write_movies(const std::string& json, std::ofstream& out, int total_written, std::unordered_set<int>& seen_ids);

// TMDB metadata helpers
std::vector<int> fetch_genres();
std::vector<std::string> fetch_languages();

// fetch all movies
void fetch_chunk(int year, int genre_id, const std::string& lang, std::ofstream& out, int& total_written, int& total_requests, std::unordered_set<int>& seen_ids);
void fetch_all_movies_by_year(const std::string& out_filepath, int start_year, int end_year);

// get movie objects
std::vector<Movie*> get_all_movies(const std::string& filepath);