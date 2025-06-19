#include "tools.h"
#include <iostream>
#include <random>
#include <chrono>
#include <vector>
AppleMap::AppleMap(int x, int y)
    : randomEngine(std::random_device{}()),
      distrib(1, 9)
{
    height = x;
    width = y;
    mapData.resize(height);
    for (int i = 0; i < height; ++i) {
        mapData[i].resize(width);
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            mapData[i][j] = distrib(randomEngine);
        }
    }
}

void AppleMap::displayMap() {
    std::cout << "\n--- Map Contents ---" << std::endl;
    for (const auto& row : mapData) {
        for (int cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "--------------------" << std::endl;
}

int AppleMap::getVal(int x, int y) {
    if (x >= height || y >= width || x < 0 || y < 0)
        return -1;
    else
        return mapData[x][y];
}

std::vector<std::vector<int>> AppleMap::getPartialMap(int x1, int y1, int x2, int y2) {
    if (x2 >= height || y2 >= width || x1 > x2 || y1 > y2 || x1 < 0 || y1 < 0)
        return {};
    std::vector<std::vector<int>> partialMap;
    partialMap.resize(x2 - x1 + 1);
    for (int i = x1; i <= x2; i++) {
        partialMap[i - x1].resize(y2 - y1 + 1);
        for (int j = y1; j <= y2; j++) {
            partialMap[i - x1][j - y1] = mapData[i][j];
        }
    }
    return partialMap;
}

int AppleMap::setZero(int x1, int y1, int x2, int y2) {
    if (x2 >= height || y2 >= width || x1 > x2 || y1 > y2 || x1 < 0 || y1 < 0)
        return -1;
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            mapData[i][j] = 0;
        }
    }
    return 0;
}

int AppleMap::setMap(std::vector<std::vector<int>> map, int x1, int y1, int x2, int y2) {
    if (x2 >= height || y2 >= width || x1 > x2 || y1 > y2 || x1 < 0 || y1 < 0)
        return -1;
    if (map.size() != (x2 - x1 + 1) || (map.empty() ? 0 : map[0].size()) != (y2 - y1 + 1))
        return -1;
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            mapData[i][j] = map[i - x1][j - y1];
        }
    }
    return 0;
}

int AppleMap::calSum(int x1, int y1, int x2, int y2) {
    int sum = 0;
    if (x1 < 0 || y1 < 0 || x2 >= height || y2 >= width || x1 > x2 || y1 > y2) {
        return 0;
    }
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            sum += getVal(i, j);
        }
    }
    return sum;
}