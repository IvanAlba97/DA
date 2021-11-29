// ###### Config options ################



// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include "cronometro.h"

using namespace Asedio; 

// Devuelve la posición en el mapa del centro de la celda (i,j)
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
    // Comprobamos que la defensa no se sale del mapa
    if(centralPosition.x + defenseRadio > mapWidth || centralPosition.x - defenseRadio < 0 || 
        centralPosition.y + defenseRadio > mapHeight || centralPosition.y - defenseRadio < 0) {
        factible = false;
    }
    // Comprobamos que la defensa no choca con nunguna defensa ya colocada
    List<Defense*>::iterator itDefense = defenses.begin();
    while(itDefense != defenses.end() && factible) {
        if(defense->radio + (*itDefense)->radio > _distance(centralPosition, (*itDefense)->position)) {
            factible = false;
        }
        itDefense++;
    }
    // Comprobamos que la defensa no choca con ningún obstáculo
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

Cell selection(std::list<Cell> cellList, int tipo) { 
    Cell aux;
    switch(tipo) {
        case 0:     // Sin preordenación
            aux = cellList.front();
            for(List<Cell>::iterator itCells = cellList.begin(); itCells != cellList.end(); itCells++) {
                if(itCells->getValue() < aux.getValue()) {
                    aux = (*itCells);
                }
            }
            break;
        case 1:     // Ordenación por fusión

            break;
        case 2:     // Ordenación rápida

            break;
        case 3:     // Ordenación por montículo
        
            break;
    }
    return aux;
}

void DEF_LIB_EXPORTED placeDefenses_SPO(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses, float cellWidth, float cellHeight) {
    float values[nCellsHeight][nCellsWidth];    // Matriz que almacena los valores de cada celda
    List<Cell> cellList;
    // Rellenamos la matriz de valores para colocar las defensas
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            values[i][j] = defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
        }
    }
    // Agrupamos las celdas en una lista
    List<Cell>::iterator itCells = cellList.begin();
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            Cell c(i, j, values[i][j]);
            cellList.insert(itCells, c);
            itCells++;
        }
    }
    // Colocamos las defensas
    List<Defense*>::iterator itDefense = defenses.begin();
    itDefense = defenses.begin();
    bool placed;
    while(itDefense != defenses.end()) {    // Mientras haya defensas sin colocar...
        placed = false;
        while(!placed) {
            // Seleccionamos la celda más prometedora
            Cell promisingCell = selection(cellList, 0);  // promisingCell guarda la celda más prometedora
            if(factibility(*itDefense, promisingCell.getRow(), promisingCell.getCol(), obstacles, defenses, cellWidth, cellHeight, mapWidth, mapHeight)) {
                // Colocamos la defensa en el centro de la celda más prometedora
                (*itDefense)->position = cellCenterToPosition(promisingCell.getRow(), promisingCell.getCol(), cellWidth, cellHeight);
                placed = true;
            }
            // Sacamos de la lista la celda procesada
            cellList.pop_front();
        }
        itDefense++;
    }
}

void DEF_LIB_EXPORTED placeDefenses_OF(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses, float cellWidth, float cellHeight) {

}

void DEF_LIB_EXPORTED placeDefenses_OR(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses, float cellWidth, float cellHeight) {

}

void DEF_LIB_EXPORTED placeDefenses_OM(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses, float cellWidth, float cellHeight) {

}

void DEF_LIB_EXPORTED placeDefenses3(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses, float cellWidth, float cellHeight) {

    

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
 
    // SIN PREORDENACIÓN

    int maxAttemps = 100;
	cronometro c1;
    long int r1 = 0;
    c1.activar();
    do {	
		List<Defense*>::iterator currentDefense = defenses.begin();
		while(currentDefense != defenses.end() && maxAttemps > 0) {
			placeDefenses_SPO(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, cellWidth, cellHeight);
		}
        maxAttemps--;
		r1++;
    } while(c1.tiempo() < 1.0);
    c1.parar();
    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c1.tiempo() / r1 << '\t' << c1.tiempo() / r1 << '\t' << c1.tiempo() / r1 << '\t' << c1.tiempo() / r1 << std::endl;

    // ORDENACIÓN POR FUSIÓN

    cronometro c2;
    long int r2 = 0;
    c2.activar();
    do {	
		List<Defense*>::iterator currentDefense = defenses.begin();
		while(currentDefense != defenses.end() && maxAttemps > 0) {
			placeDefenses_OF(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, cellWidth, cellHeight);
		}
        maxAttemps--;
		r2++;
    } while(c2.tiempo() < 1.0);
    c2.parar();
    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c2.tiempo() / r2 << '\t' << c2.tiempo() / r2 << '\t' << c2.tiempo() / r2 << '\t' << c2.tiempo() / r2 << std::endl;

    // ORDENACIÓN RÁPIDA

    cronometro c3;
    long int r3 = 0;
    c3.activar();
    do {	
		List<Defense*>::iterator currentDefense = defenses.begin();
		while(currentDefense != defenses.end() && maxAttemps > 0) {
			placeDefenses_OR(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, cellWidth, cellHeight);
		}
        maxAttemps--;
		r3++;
    } while(c3.tiempo() < 1.0);
    c3.parar();
    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c3.tiempo() / r3 << '\t' << c3.tiempo() / r3 << '\t' << c3.tiempo() / r3 << '\t' << c3.tiempo() / r3 << std::endl;

    // ORDENACIÓN POR MONTÍCULOS

    cronometro c4;
    long int r4 = 0;
    c4.activar();
    do {	
		List<Defense*>::iterator currentDefense = defenses.begin();
		while(currentDefense != defenses.end() && maxAttemps > 0) {
			placeDefenses_OM(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, cellWidth, cellHeight);
		}
        maxAttemps--;
		r4++;
    } while(c4.tiempo() < 1.0);
    c4.parar();
    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c4.tiempo() / r4 << '\t' << c4.tiempo() / r4 << '\t' << c4.tiempo() / r4 << '\t' << c4.tiempo() / r4 << std::endl;

}
