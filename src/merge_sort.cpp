//
// Created by Sanaa Chetal on 3/19/26.
//
#include "merge_sort.h"
using namespace std;

void merge(vector<pair<float, Movie*>>& movies, int start, int middle, int end) {
    //Adapted from code found in lecture slides
    int first = middle - start + 1;
    int last = end - middle;
    vector<pair<float, Movie*>> left, right;
    left.reserve(first);
    right.reserve(last);
    for (int i = 0; i < first; i++) {
        left.push_back(movies[start + i]);
    }
    for (int j = 0; j < last; j++) {
        right.push_back(movies[middle + j + 1]);
    }

    int i = 0, j = 0;
    int k = start;
    while (i < left.size() && j < right.size()) {
        if (left[i].first >= right[j].first) {
            movies[k] = left[i];
            i++;
        }
        else {
            movies[k] = right[j];
            j++;
        }
        k++;
    }
    while (i < left.size()) {
        movies[k] = left[i];
        i++;
        k++;
    }
    while (j < right.size()) {
        movies[k] = right[j];
        j++;
        k++;
    }
}

void mergeSort(vector<pair<float, Movie*>>& movies, int start, int end) {
    //adapted from psuedocode found in lecture slides
    if (start >= end) {
        return;
    }

    int mid = (start + end) / 2;
    mergeSort(movies, start, mid);
    mergeSort(movies, mid + 1, end);
    merge(movies, start, mid, end);
}