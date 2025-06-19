#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <random>
#include <chrono>
#include <vector>

class AppleMap {
private:
    std::vector<std::vector<int>> mapData;
    std::mt19937 randomEngine;
    std::uniform_int_distribution<int> distrib;
    int height;
    int width;

public:
    AppleMap(int x = 10, int y = 17); 
    void displayMap();
    int getVal(int x, int y);
    int setZero(int x1, int y1, int x2, int y2);
    int setMap(std::vector<std::vector<int>> map, int x1, int y1, int x2, int y2);
    int calSum(int x1, int y1, int x2, int y2);
    std::vector<std::vector<int>> getPartialMap(int x1, int y1, int x2, int y2);
};

#endif