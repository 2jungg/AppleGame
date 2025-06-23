#include "tools.h"
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <emscripten/emscripten.h>

#define DEFUALT_TOP_K 70
double COEFF_HEURESTIC = 1.0;

struct GameState {
    AppleMap map;
    int currentScore = 0;
    std::vector<std::vector<int>> dragRecord;
    std::vector<std::vector<int>> possibleDrag;

    void updatePossibleDrag() {
        possibleDrag.clear();
        for (int i = 0; i < map.getHeight(); i++) {
            for (int j = 0; j < map.getWidth(); j++) {
                for (int dx = 0; i + dx < map.getHeight(); dx++) {
                    if (map.calSum(i, j, i + dx, j) > 10) break;
                    if (map.calSum(i, j, i + dx, j) == 0) continue;
                    for (int dy = 0; j + dy < map.getWidth(); dy++) {
                        if (map.calSum(i, j, i, j + dy) == 0) continue;
                        if (map.calSum(i, j, i + dx, j + dy) > 10) break;
                        if (map.calSum(i, j, i + dx, j + dy) == 10) {
                            possibleDrag.push_back({i, j, i + dx, j + dy});
                            break;
                        }
                    }
                }
            }
        }
    }

    double getHeuristic() const {
        return currentScore + COEFF_HEURESTIC * possibleDrag.size();
    }

    bool operator<(const GameState& other) const {
        return getHeuristic() < other.getHeuristic();
    }
};


class GamePlay {
private:
    int height;
    int width;
    double highestVal = 0;
    std::vector<std::vector<int>> highestDrag;
    AppleMap myMap;
    std::vector<std::vector<int>> dragRecord;
    std::vector<std::vector<std::vector<int>>> mapRecord;
    std::vector<std::vector<int>> possibleDrag;
public:
    GamePlay(int x = 10, int y = 17) : height(x), width(y), myMap(x, y) {
        updatePossibleDrag();
    }
    GamePlay(std::vector<std::vector<int>> map) : height(map.size()), width(map[0].size()), myMap(map) {
        updatePossibleDrag();
    }
    int loadMap(const std::string& filename) {
        std::vector<std::vector<int>> mapData;
        std::ifstream inputFile(filename);
        if (!inputFile.is_open()) {
            return -1;
        }
        else {
            std::string line;
            while (std::getline(inputFile, line)) {
                std::vector<int> row;
                for (const auto& ch : line) {
                    if (ch == ' ') continue;
                    row.push_back(ch - '0');
                }
                if (row.size() != width) {
                    return -1;
                }
                mapData.push_back(row);
            }
            myMap.setZero(0, 0, height - 1, width - 1);
            myMap.setMap(mapData, 0, 0, height - 1, width - 1);
            inputFile.close();
            updatePossibleDrag();
            return 0;
        }
    }
    void startGreedy() {
        int result = 0;
        while (!possibleDrag.empty()) {
            std::vector<int> drag = possibleDrag.front();
            int x1 = drag[0], y1 = drag[1], x2 = drag[2], y2 = drag[3];
            dragRecord.push_back(drag);
            mapRecord.push_back(myMap.getPartialMap(x1, y1, x2, y2));
            if (myMap.setZero(x1, y1, x2, y2) == 0) {
                result += (x2 - x1 + 1) * (y2 - y1 + 1);
                updatePossibleDrag();
            } else {
                myMap.setMap(mapRecord.back(), x1, y1, x2, y2);
                dragRecord.pop_back();
                mapRecord.pop_back();
            }
        }
        if (result > highestVal) {
            highestVal = result;
            highestDrag = dragRecord;
        }
    }
    void startDFS(int depth = 0) {
        if (possibleDrag.empty()) {
            if (depth > highestVal) {
                highestVal = depth;
                highestDrag = dragRecord;
            }
            return;
        }
        for (const auto& drag : possibleDrag) {
            int x1 = drag[0], y1 = drag[1], x2 = drag[2], y2 = drag[3];
            dragRecord.push_back(drag);
            mapRecord.push_back(myMap.getPartialMap(x1, y1, x2, y2));
            std::vector<std::vector<int>> rememberPossibleDrag = possibleDrag;
            if (myMap.setZero(x1, y1, x2, y2) == 0) {
                updatePossibleDrag();
                startDFS(depth + 1);
                myMap.setMap(mapRecord.back(), x1, y1, x2, y2);
                possibleDrag = rememberPossibleDrag;
                dragRecord.pop_back();
                mapRecord.pop_back();
            } else {
                myMap.setMap(mapRecord.back(), x1, y1, x2, y2);
                dragRecord.pop_back();
                mapRecord.pop_back();
            }
        }
        return;
    }
    void startBeamSearch(int beamWidth) {
        std::priority_queue<GameState> currentBeams;
        currentBeams.push({myMap, 0, {}, possibleDrag});
        while(currentBeams.size() > 0) {
            std::priority_queue<GameState> nextBeams;
            while(currentBeams.size() > 0) {
                GameState currentState = currentBeams.top();
                currentBeams.pop();
                if (currentState.possibleDrag.empty()) {
                    if (currentState.getHeuristic() > highestVal) {
                        highestVal = currentState.getHeuristic();
                        highestDrag = currentState.dragRecord;
                    }
                    continue;
                }
                
                for (const auto& drag : currentState.possibleDrag) {
                    int x1 = drag[0], y1 = drag[1], x2 = drag[2], y2 = drag[3];
                    GameState nextState = currentState;
                    nextState.dragRecord.push_back(drag);
                    nextState.map.setZero(x1, y1, x2, y2);
                    nextState.currentScore += (x2 - x1 + 1) * (y2 - y1 + 1);
                    nextState.updatePossibleDrag();
                    nextBeams.push(nextState);
                }
            }
            for (int i = 0; i < beamWidth && !nextBeams.empty(); i++) {
                GameState bestState = nextBeams.top();
                nextBeams.pop();
                currentBeams.push(bestState);
            }
        }
    }
    void updatePossibleDrag() {
        possibleDrag.clear();
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                for (int dx = 0; i + dx < height; dx++) {
                    if (myMap.calSum(i, j, i + dx, j) > 10) break;
                    if (myMap.calSum(i, j, i + dx, j) == 0) continue;
                    for (int dy = 0; j + dy < width; dy++) {
                        if (myMap.calSum(i, j, i, j + dy) == 0) continue;
                        if (myMap.calSum(i, j, i + dx, j + dy) > 10) break;
                        if (myMap.calSum(i, j, i + dx, j + dy) == 10) {
                            possibleDrag.push_back({i, j, i + dx, j + dy});
                            break;
                        }
                    }
                }
            }
        }
    }
    void displayMap() {
        myMap.displayMap();
    }
    void displayHighestDrag() {
        std::cout << "Highest Drag Operations: " << highestVal << std::endl;
        for (const auto& drag : highestDrag) {
            std::cout << "Drag from (" << drag[0] << ", " << drag[1] << ") to (" 
                      << drag[2] << ", " << drag[3] << ")" << std::endl;
        }
    }
    
    std::vector<std::vector<int>> getHighestDrag() const {
        return highestDrag;
    }

    void reset() {
        highestVal = 0;
        highestDrag.clear();
        dragRecord.clear();
        mapRecord.clear();
    }
};

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    GamePlay myPlay;
    int topK = DEFUALT_TOP_K;
    // 맵 로드 또는 랜덤 생성
    if (myPlay.loadMap("input.txt") == 0) {
        std::cout << "Map loaded successfully." << std::endl;
    } else {
        std::cout << "Map generated randomly." << std::endl;
    }
    myPlay.displayMap();
    if (argc > 1 && std::string(argv[1]) == "dfs") {
        std::cout << "Starting DFS Algorithm..." << std::endl;
        myPlay.startDFS();
    } else if (argc > 1 && std::string(argv[1]) == "beam") {
        std::cout << "Starting Beam Search Algorithm..." << std::endl;
        if (argc > 3) {
            topK = std::stoi(argv[2]);
            COEFF_HEURESTIC = std::stod(argv[3]);
        }
        myPlay.startBeamSearch(topK);
        // myPlay.displayHighestDrag();
        // myPlay.reset();
        // std::cout << "Starting Greedy Algorithm..." << std::endl;
        // myPlay.updatePossibleDrag();
        // myPlay.startGreedy();
    } else {
        std::cout << "Starting Greedy Algorithm..." << std::endl;
        myPlay.startGreedy();
    }
    myPlay.displayHighestDrag();
    return 0;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int* solveMap(const int map[]) {
        std::vector<std::vector<int>> inputMap;
        for (int i = 0; i < 10; i++) {
            std::vector<int> row;
            for (int j = 0; j < 17; j++) {
                row.push_back(map[i * 17 + j]);
            }
            inputMap.push_back(row);
        }

        GamePlay myPlay(inputMap);
        myPlay.startBeamSearch(DEFUALT_TOP_K);
        std::vector<std::vector<int>> result = myPlay.getHighestDrag();

        // 0번째 인덱스는 드래그 수
        int* output = (int*)malloc((result.size()+1) * 4 * sizeof(int));
        int index = 0;
        output[index++] = result.size();
        for (const auto& drag : result) {
            output[index++] = drag[0];
            output[index++] = drag[1];
            output[index++] = drag[2];
            output[index++] = drag[3];
        }
        return output;
    }
    
    EMSCRIPTEN_KEEPALIVE
    void freeMemory(int* ptr) {
        free(ptr);
    }
}