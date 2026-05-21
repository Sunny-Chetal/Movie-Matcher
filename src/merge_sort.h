#pragma once
#include <vector>
#include "Movie.h"

void merge(std::vector<std::pair<float, Movie*>> &movies, int start, int middle, int end);
void mergeSort(std::vector<std::pair<float, Movie*>> &movies, int start, int end);