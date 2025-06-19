#include "tools.h"

class GamePlay {
private:
    int height;
    int width;
    int highestVal = 0;
    std::vector<std::vector<int>> highestDrag;
    AppleMap myMap;
    std::vector<std::vector<int>> dragRecord;
    std::vector<std::vector<std::vector<int>>> mapRecord;
    std::vector<std::vector<int>> possibleDrag;
public:
    GamePlay(int x = 10, int y = 17) : height(x), width(y), myMap(x, y) {
        updatePossibleDrag();
    }
    void startGreedy() {
        int result = 0;
        dragRecord.clear();
        mapRecord.clear();
        highestVal = 0;
        updatePossibleDrag();
        while (!possibleDrag.empty()) {
            std::vector<int> drag = possibleDrag.front();
            int x1 = drag[0], y1 = drag[1], x2 = drag[2], y2 = drag[3];
            dragRecord.push_back(drag);
            mapRecord.push_back(myMap.getPartialMap(x1, y1, x2, y2));
            if (myMap.setZero(x1, y1, x2, y2) == 0) {
                result++;
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
            std::cout << "DFS Depth: " << depth << ", Drag: (" << x1 << ", " << y1 << ") to (" 
                      << x2 << ", " << y2 << ")" << std::endl;
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
    void updatePossibleDrag() {
        possibleDrag.clear();
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                for (int dx = 0; i + dx < height; dx++) {
                    if (myMap.calSum(i, j, i + dx, j) > 10) {
                        break;
                    }
                    for (int dy = 0; j + dy < width; dy++) {
                        if (myMap.calSum(i, j, i + dx, j + dy) == 10) {
                            possibleDrag.push_back({i, j, i + dx, j + dy});
                            break;
                        }
                        else if (myMap.calSum(i, j, i + dx, j + dy) > 10) {
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
    void displayHigestDrag() {
        std::cout << "Highest Drag Operations: " << highestVal << std::endl;
        for (const auto& drag : highestDrag) {
            std::cout << "Drag from (" << drag[0] << ", " << drag[1] << ") to (" 
                      << drag[2] << ", " << drag[3] << ")" << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    GamePlay myPlay;
    if (argc > 1) {
        myPlay = GamePlay(std::atoi(argv[1]), std::atoi(argv[2]));
    }
    myPlay.displayMap();
    if (argc > 3 && std::string(argv[3]) == "dfs") {
        myPlay.startDFS();
    } else {
        myPlay.startGreedy();
    }
    myPlay.displayHigestDrag();
    return 0;
}