#include "fetch_all_movies.h"
#include <asio.hpp>
#include <iostream>
#include <unordered_set>
using namespace std;

const string TMDB_HOST = "api.themoviedb.org";
const string API_KEY = "e34623f0fa8066716d7141c6f2248fc0";

// HTTP request helper
string get_tmdb_data(const string& url) {
  asio::io_context io_context;
  asio::ip::tcp::resolver resolver(io_context);
  asio::ip::tcp::socket socket(io_context);
  asio::connect(socket, resolver.resolve(TMDB_HOST, "80"));

  string request = "GET " + url + " HTTP/1.1\r\n"
								 + "Host: " + TMDB_HOST + "\r\n"
								 + "Accept: application/json\r\n"
								 + "Connection: close\r\n\r\n";
  asio::write(socket, asio::buffer(request));

  string response;
  asio::error_code ec;
  char buffer[4096];

  while (true) {
    int bytes_read = socket.read_some(asio::buffer(buffer), ec);
    if (bytes_read > 0) response.append(buffer, bytes_read);
    if (ec == asio::error::eof || ec) break;
  }

  int index = response.find("\r\n\r\n");
  return (index != string::npos) ? response.substr(index + 4) : response;
}

// TMDB metadata helpers
vector<int> fetch_genres() {
  string json = get_tmdb_data("/3/genre/movie/list?api_key=" + API_KEY);

  vector<int> genres;
  int search = 0;

  while (true) {
    int index = json.find("\"id\":", search);
    if (index == string::npos) break;

    genres.push_back(stoi(json.substr(index + 5)));
    search = index + 6;
  }

  cout << "Fetched " << genres.size() << " genres" << endl;
  return genres;
}

vector<string> fetch_languages() {
  string json = get_tmdb_data("/3/configuration/languages?api_key=" + API_KEY);

  vector<string> languages;
  int search = 0;

  while (true) {
    int index = json.find("\"iso_639_1\":\"", search);
    if (index == string::npos) break;

    int start = index + 13;
    int end = json.find("\"", start);
    string lang = json.substr(start, end - start);

    if (!lang.empty()) languages.push_back(lang);
    search = index + 14;
  }

  cout << "Fetched " << languages.size() << " languages" << endl;
  return languages;
}

// JSON parsing helpers
int parse_total_pages(const string& json) {
  int index = json.find("\"total_pages\":");

  if (index == string::npos) return 1;
  return min(stoi(json.substr(index + 14)), 500);
}

int write_movies(const string& json, ofstream& out, int total_written, unordered_set<int>& seen_ids) {
  int start = json.find("\"results\":[");
  int end = json.rfind("]");
  if (start == string::npos || end == string::npos) return 0;

  string fetched_movies = json.substr(start + 11, end - start - 11);
  int count = 0;
  int depth = 0;
  int obj_start = -1;

  bool in_string = false;
  for (int i = 0; i < (int)fetched_movies.size(); i++) {
    char c = fetched_movies[i];

    if (in_string) {
      if (c == '\\') {
        i++;
      } else if (c == '"') {
        in_string = false;
      }

      continue;
    }

    if (c == '"') {
      in_string = true;
      continue;
    }

    if (c == '{') {
      if (depth == 0) {
        obj_start = i;
        depth++;
      }
    } else if (c == '}' && depth == 1 && obj_start != -1) {
      depth--;

      string obj = fetched_movies.substr(obj_start, i - obj_start + 1);
      int id_pos = obj.find("\"id\":");
      int id = stoi(obj.substr(id_pos + 5));
      if (seen_ids.count(id)) {
        obj_start = -1;
        continue;
      }
      seen_ids.insert(id);

      if (total_written + count > 0) {
        out << "," << endl;
      }
      out << obj;

      count++;
      obj_start = -1;
    }
  }

  return count;
}

// fetch all movies
void fetch_chunk(int year, int genre_id, const string& lang, ofstream& out, int& total_written, int& total_requests, unordered_set<int>& seen_ids) {
  string base = "/3/discover/movie?api_key=" + API_KEY
							+ "&primary_release_year=" + to_string(year)
							+ "&with_genres=" + to_string(genre_id)
							+ (lang.empty() ? "" : "&with_original_language=" + lang)
							+ "&sort_by=popularity.desc&page=";

  string first_page = get_tmdb_data(base + "1");
  total_requests++;

  int total_pages = parse_total_pages(first_page);
  if (total_pages == 0) return;

  if (total_pages == 500) {
    cerr << "WARNING:"
				 << " year=" << year
				 << " genre=" << genre_id
         << " lang=" << (lang.empty() ? "any" : lang)
         << " hit 500 page cap" << endl;
  }

  cout << "  -->"
			 << " year=" << year
			 << " genre=" << genre_id
       << " lang=" << (lang.empty() ? "any" : lang)
       << ": " << total_pages << " pages" << endl;

  for (int page = 1; page <= total_pages; page++) {
    string json = (page == 1) ? first_page : get_tmdb_data(base + to_string(page));
    if (page > 1) total_requests++;

    int written = write_movies(json, out, total_written, seen_ids);
    total_written += written;
  }
}

void fetch_all_movies_by_year(const string& out_filepath, int start_year, int end_year) {
  ofstream out(out_filepath);
  if (!out.is_open()) {
    cerr << "Failed to open output file: " << out_filepath << endl;
    return;
  }

  out << "[" << endl;

  vector<int> genres = fetch_genres();
  vector<string> languages = fetch_languages();
  cout << "Using "
			 << genres.size() << " genres and "
       << languages.size() << " languages" << endl;

  int total_written  = 0;
  int total_requests = 0;
  unordered_set<int> seen_ids;

  for (int year = start_year; year <= end_year; year++) {
    cout << "Year " << year << endl;

    for (int& genre_id : genres) {
      if (year >= 2020) {
				// for years 2020 and later, fetch movies for each language
        for (string& lang : languages) {
          fetch_chunk(year, genre_id, lang, out, total_written, total_requests, seen_ids);
        }
      } else {
				// for years before 2020, fetch movies without specifying a language
        fetch_chunk(year, genre_id, "", out, total_written, total_requests, seen_ids);
      }
    }

    cout << "Year " << year << " done. requests=" << total_requests << " movies=" << total_written << endl;
  }

  out << endl << "]";
  cout << "Done. " << total_requests << " requests, " << total_written << " movies written to " << out_filepath << endl;
}

vector<Movie*> get_all_movies(const string& filepath) {
  ifstream in(filepath);
  if (!in.is_open()) {
    cerr << "Failed to open file: " << filepath << endl;
    return {};
  }

  vector<Movie*> movies;
  string line;

  while (getline(in, line)) {
    if (line.empty() || line == "[" || line == "]") continue;

    if (line.back() == ',') line.pop_back();

    movies.push_back(new Movie(line));
  }

  cout << "Loaded " << movies.size() << " movies from " << filepath << endl;
  return movies;
}