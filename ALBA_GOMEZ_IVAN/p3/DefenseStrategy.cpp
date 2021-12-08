// ###### Config options ################



// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include "cronometro.h"

using namespace Asedio; 

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight) {
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0);
}        

float defaultCellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
    , float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    	
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;

    Vector3 cellPosition((col * cellWidth) + cellWidth * 0.5f, (row * cellHeight) + cellHeight * 0.5f, 0);
    	
    float val = 0;
    for (List<Object*>::iterator it=obstacles.begin(); it != obstacles.end(); ++it) {
	    val += _distance(cellPosition, (*it)->position);
    }

    return val;
}

bool factibility(Defense* defense, int row, int col, std::list<Object*> obstacles, std::list<Defense*> defenses, float cellWidth, float cellHeight, float mapWidth, float mapHeight) {
    bool factible = true;
    Vector3 centralPosition = cellCenterToPosition(row, col, cellWidth, cellHeight);
    float defenseRadio = defense->radio;
    if(centralPosition.x + defenseRadio > mapWidth || centralPosition.x - defenseRadio < 0 || 
        centralPosition.y + defenseRadio > mapHeight || centralPosition.y - defenseRadio < 0) {
        factible = false;
    }
    List<Defense*>::iterator itDefense = defenses.begin();
    while(itDefense != defenses.end() && factible) {
        if(defense->radio + (*itDefense)->radio > _distance(centralPosition, (*itDefense)->position)) {
            factible = false;
        }
        itDefense++;
    }
    List<Object*>::iterator itObstacle = obstacles.begin();
    while(itObstacle != obstacles.end() && factible) {
        if(defense->radio + (*itObstacle)->radio > _distance(centralPosition, (*itObstacle)->position)) {
            factible = false;
        }
        itObstacle++;
    }
    return factible;
}

class Cell {
    public:
        Cell(int row = 0, int col = 0, float value = 0) : row_(row), col_(col), value_(value) {}
        int getRow() { return row_; }
        int getCol() { return col_; }
        float getValue() { return value_; }
    private:
        int row_, col_;
        float value_;
};

void insertionSort(std::vector<Cell>& cellVector, int i, int j) {
    Cell cell;
    for (int l = i + 1; l <= j; l++) {
        cell = cellVector[l];
        int p = l - 1;
        while (p >= i && cellVector[p].getValue() < cell.getValue()) {
            cellVector[p+1] = cellVector[p];
            p--;
        }
        cellVector[p+1] = cell;
    }
}

void fusion(std::vector<Cell>& cellVector, int i, int k, int j) {
    int n = j - i + 1;
    int p = i;
    int q = k + 1;
    std::vector<Cell> w(n);
    for(int l = 0; l < n; l++) {
        if(p <= k && (q > j || cellVector[p].getValue() <= cellVector[q].getValue())) {
            w[l] = cellVector[p];
            p++;
        } else {
            w[l] = cellVector[q];
            q++;
        }
    }
    for(int l = 0; l < n; l++) { cellVector[i+l] = w[l]; }
}

void fusionSort(std::vector<Cell>& cellVector, int i, int j) {
    int n = j - i + 1;
    int umbral = 2;
    if(n <= umbral) {
        insertionSort(cellVector, i, j);
    } else {
        int k = i -1 + (n / 2);
        fusionSort(cellVector, i, k);
        fusionSort(cellVector, k + 1, j);
        fusion(cellVector, i, k, j);
    }
}

Cell selection(std::vector<Cell> cellVector, int tipo) { 
    return cellVector.front();
}

void DEF_LIB_EXPORTED placeDefenses3(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) {

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    std::vector<Cell> cellVector;
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            Cell c(i, j, defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses));
            cellVector.push_back(c);
        }
    }
    fusionSort(cellVector, 0, cellVector.size()-1);
    List<Defense*>::iterator itDefense = defenses.begin();
    bool placed;
    while(itDefense != defenses.end()) {
        placed = false;
        while(!placed) {
            Cell promisingCell = selection(cellVector, 1);
            if(factibility(*itDefense, promisingCell.getRow(), promisingCell.getCol(), obstacles, defenses, cellWidth, cellHeight, mapWidth, mapHeight)) {
                (*itDefense)->position = cellCenterToPosition(promisingCell.getRow(), promisingCell.getCol(), cellWidth, cellHeight);
                placed = true;
            }
            cellVector.erase(cellVector.begin());
        }
        itDefense++;
    }
}
