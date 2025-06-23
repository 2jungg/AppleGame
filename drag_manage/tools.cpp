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
    sumTable = SumTable(height, width);
    for (int i = 0; i < height; ++i) {
        mapData[i].resize(width);
        for (int j = 0; j < width; ++j) {
            mapData[i][j] = distrib(randomEngine);
            sumTable.update(i, j, mapData[i][j]);
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
            if (mapData[i][j] == 0) continue;
            sumTable.update(i, j, -mapData[i][j]);
            mapData[i][j] = 0;
        }
    }
    return 0;
}

int AppleMap::setMap(std::vector<std::vector<int>> map, int x1, int y1, int x2, int y2) {
    if (x2 >= height || y2 >= width || x1 > x2 || y1 > y2 || x1 < 0 || y1 < 0)
        return -1;
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            if (map[i - x1][j - y1] == 0) continue;
            mapData[i][j] = map[i - x1][j - y1];
            sumTable.update(i, j, mapData[i][j]);
        }
    }
    return 0;
}

int AppleMap::calSum(int x1, int y1, int x2, int y2) {
    if (x1 < 0 || y1 < 0 || x2 >= height || y2 >= width || x1 > x2 || y1 > y2) {
        return 0;
    }
    return sumTable.calSum(x1, y1, x2, y2);;
}

SumTable::SumTable(int x, int y)
    : height(x), width(y) {
    fenwickTree.resize(height + 1);
    for (int i = 0; i <= height; ++i) {
        fenwickTree[i].resize(width + 1, 0);
    }
}

void SumTable::update(int x, int y, int val) {
    int i = x + 1;
    while (i <= height) {
        int j = y + 1;
        while (j <= width) {
            fenwickTree[i][j] += val;
            j += (j & -j);
        }
        i += (i & -i);
    }
}

int SumTable::query(int x, int y) {
    int sum = 0;
    int i = x + 1;
    while (i > 0) {
        int j = y + 1;
        while (j > 0) {
            sum += fenwickTree[i][j];
            j -= (j & -j);
        }
        i -= (i & -i);
    }
    return sum;
}

int SumTable::calSum(int x1, int y1, int x2, int y2) {
    if (x1 > x2 || y1 > y2 || x1 < 0 || y1 < 0 || x2 > height || y2 > width) {
        return 0;
    }
    return query(x2, y2) - query(x1 - 1, y2) - query(x2, y1 - 1) + query(x1 - 1, y1 - 1);
}

void SumTable::displayTree() {
    std::cout << "\n--- Fenwick Tree Contents ---" << std::endl;
    for (int i = 1; i <= height; ++i) {
        for (int j = 1; j <= width; ++j) {
            std::cout << fenwickTree[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------" << std::endl;
}