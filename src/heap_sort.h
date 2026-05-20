//
// Created by Jessica Dippolito on 3/24/26.
//
#pragma once
#include <vector>
#include "Movie.h"

void heapifyDown(std::vector<std::pair<float, Movie*>> &movies, int n, int i);
void heapSort(std::vector<std::pair<float, Movie*>> &movies);


