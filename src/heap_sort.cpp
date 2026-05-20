//
// Created by Jessica Dippolito on 3/24/26.
//
#include "heap_sort.h"
#include <iostream>
#include <ostream>
#include <vector>

void heapifyDown(std::vector<std::pair<float, Movie*>> &movies, int n, int i) {
    //adapted from code given in the slides
    while (true) {
        int left = 2*i+1;
        int right = 2*i+2;
        int smallest = i;
        if (left < n && movies[left].first < movies[smallest].first) {
            smallest = left;
        }
        if (right < n && movies[right].first < movies[smallest].first) {
            smallest = right;
        }
        if (smallest != i) {
            std::swap(movies[i], movies[smallest]);
            i = smallest;
        }
        else {
            return;
        }
    }
}

void heapSort(std::vector<std::pair<float, Movie*>> &movies) {
    //adapted from pseudocode given in the slides
    //build min heap in place
    int index = movies.size()/2-1;
    for (int i = index; i >= 0; i--) {
        heapifyDown(movies, movies.size(), i);
    }

    //place elements in descending order and heapifyDown
    for (int i = movies.size()-1; i >= 0; i--) {
        std::pair<float, Movie*> temp = movies[i];
        movies[i] = movies[0];
        movies[0] = temp;
        heapifyDown(movies,i, 0);
    }
}
