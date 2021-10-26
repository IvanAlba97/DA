// ###### Config options ################

//#define PRINT_DEFENSE_STRATEGY 1    // generate map images

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_DEFENSE_STRATEGY
#include "ppm.h"
#endif

#ifdef CUSTOM_RAND_GENERATOR
RAND_TYPE SimpleRandomGenerator::a;
#endif

using namespace Asedio;

// COGER DEL CAMPUS (PREGUNTAS FRECUENTES) LA FUNCIÓN QUE DADA UNA CELDA DEVUELVE LA POSICIÓN SU CENTRO Y PEGARLA AQUÍ.
// make doc y make pack para entregar la práctica

// Devuelve la posición en el mapa del centro de la celda (i,j)
Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight) {
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0);
}

// Asigna valores a las celdas para colocar la primera defensa
float cellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
	, float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    //std::cout << "Entra en la función cellValue" << std::endl;
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    
    /* float totalDistance = 0;
    List<Object*>::iterator itObstacle = obstacles.begin();
    while(itObstacle != obstacles.end()) {
        totalDistance += _distance(cellCenterToPosition(row, col, cellWidth, cellHeight), (*itObstacle)->position);
        itObstacle++;
    }
    //std::cout << "Sale de la función cellValue" << std::endl;
	return totalDistance; */

    float middleWidth = mapWidth / 2;
    float middleHeight = mapHeight / 2;
    return _distance(Vector3(middleHeight, middleWidth, 0), cellCenterToPosition(row, col, cellWidth, cellHeight));
 
}

// Asigna valores a las celdas para colocar el resto de defensas
float cellValueRest(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
	, float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    //std::cout << "Entra en la función cellValueRest" << std::endl;
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    List<Defense*>::iterator itDefense = defenses.begin();  // El primer elemento de la lista de defensas es el centro de extracción
    //std::cout << "Sale de la función cellValueRest" << std::endl;
	return _distance(cellCenterToPosition(row, col, cellWidth, cellHeight), (*itDefense)->position);
}

bool factibility(Defense* defense, int row, int col, std::list<Object*> obstacles, std::list<Defense*> defenses, float cellWidth, float cellHeight, float mapWidth, float mapHeight) {
    //std::cout << "Entra en la función factibility" << std::endl;
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
    //std::cout << "Sale de la función factibility" << std::endl;
    return factible;
}

class Cell {    // Usado en placeDefenses()
    public:
        Cell(int row, int col, float value) : row_(row), col_(col), value_(value) {}
        int row() { return row_; }
        int col() { return col_; }
        float value() { return value_; }
    private:
        int row_, col_;
        float value_;
};

void DEF_LIB_EXPORTED placeDefenses(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses) {
    //std::cout << "Entra en la función placeDefenses" << std::endl;
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
    float values[nCellsHeight][nCellsWidth];    // Matriz que almacena los valores de cada celda
    List<Cell> cellList;
    // Rellenamos la matriz de valores para colocar la primera defensa (Centro de extracción de minerales)
    //std::cout << "Rellenando la matriz de valores" << std::endl;
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            values[i][j] = cellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
        }
    }
    // Agrupamos las celdas en una lista
    //std::cout << "Agrupando las celdas en una lista" << std::endl;
    List<Cell>::iterator itCells = cellList.begin();
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            Cell c(i, j, values[i][j]);
            cellList.insert(itCells, c);
            itCells++;
        }
    }
    // Ordenamos la lista de menor a mayor valor
    //std::cout << "Ordenano la lista" << std::endl;
    cellList.sort([](Cell& c1, Cell& c2) { return c1.value() < c2.value(); });

    // Colocamos la primera defensa
    std::cout <<"Colocando la primera defensa" << std::endl;
    List<Defense*>::iterator itDefense = defenses.begin();
    bool exit = false;
    while(!exit) { // Mientras no esté colocada la primera defensa...
        // Seleccionamos la celda más prometedora, que es el primer elemento de cellList
        List<Cell>::iterator promisingCell = cellList.begin();  // promisingCell guarda la celda más prometedora
        // Si es factible...
        if(factibility(*itDefense, promisingCell->row(), promisingCell->col(), obstacles, defenses, cellWidth, cellHeight, mapWidth, mapHeight)) {
            // Asignamos a la posición de la defensa la posición del centro de la celda más prometedora
            (*itDefense)->position = cellCenterToPosition(promisingCell->row(), promisingCell->col(), cellWidth, cellHeight);
            std::cout  << "Primera defensa colocada en: " << "row " << promisingCell->row() << " col " << promisingCell->col() << std::endl;
            exit = true;
        }
        // Sacamos de la lista la celda procesada
        cellList.pop_front();
    }
    // Rellenamos la matriz de valores para colocar el resto de defensas
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            values[i][j] = cellValueRest(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
        }
    }
    // Vaciamos la lista
    while(!cellList.empty()) { cellList.pop_front(); }
    // Agrupamos las celdas en la lista anterior
    itCells = cellList.begin();
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            Cell c(i, j, values[i][j]);
            cellList.insert(itCells, c);
            itCells++;
        }
    }
    // Ordenamos la lista de menor a mayor valor
    cellList.sort([](Cell& c1, Cell& c2) { return c1.value() < c2.value(); });
    // Colocamos el resto de defensas
    std::cout << "Colocando el resto de defensas" << std::endl;
    itDefense = defenses.begin();
    itDefense++;    // Empezamos a colocar la segunda defensa, puesto que la primera es el centro de extracción
    while(itDefense != defenses.end()) {    // Mientras haya defensas sin colocar...
        exit = false;
        while(!exit) {
            // Seleccionamos la celda más prometedora, que es el primer elemento de cellList
            List<Cell>::iterator promisingCell = cellList.begin();  // promisingCell guarda la celda más prometedora
            if(factibility(*itDefense, promisingCell->row(), promisingCell->col(), obstacles, defenses, cellWidth, cellHeight, mapWidth, mapHeight)) {
                // Colocamos la defensa en el centro de la celda más prometedora
                (*itDefense)->position = cellCenterToPosition(promisingCell->row(), promisingCell->col(), cellWidth, cellHeight);
                std::cout  << "Defensa colocada en: " << "row " << promisingCell->row() << " col " << promisingCell->col() << std::endl;
                exit = true;
            }
            // Sacamos de la lista la celda procesada
            cellList.pop_front();
        }
        itDefense++;
    }
    std::cout << "Sale de la función placeDefenses" << std::endl;

#ifdef PRINT_DEFENSE_STRATEGY

    float** cellValues = new float* [nCellsHeight]; 
    for(int i = 0; i < nCellsHeight; ++i) {
       cellValues[i] = new float[nCellsWidth];
       for(int j = 0; j < nCellsWidth; ++j) {
           cellValues[i][j] = ((int)(cellValue(i, j))) % 256;
       }
    }
    dPrintMap("strategy.ppm", nCellsHeight, nCellsWidth, cellHeight, cellWidth, freeCells
                         , cellValues, std::list<Defense*>(), true);

    for(int i = 0; i < nCellsHeight ; ++i)
        delete [] cellValues[i];
	delete [] cellValues;
	cellValues = NULL;

#endif
}
