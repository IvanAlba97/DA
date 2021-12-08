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

// De las diapositivas
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

// De las diapositivas
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

//  De las diapositivas
int pivote(std::vector<Cell>& cellVector, int i, int j) {
    int p = i;
    Cell x = cellVector[i];
    for(int k = i + 1; k <= j; k++) {
        if(cellVector[k].getValue() <= x.getValue()) {
            p++;
            Cell aux = cellVector[p];
            cellVector[p] = cellVector[k];
            cellVector[k] = aux;
        }
    }
    cellVector[i] = cellVector[p];
    cellVector[p] = x;
    return p;
}

// De las diapositivas
void fastSort(std::vector<Cell>& cellVector, int i, int j) {
    int n = j - i + 1;
    int umbral = 2;
    if(n <= umbral) {
        insertionSort(cellVector, i, j);
    } else {
        int p = pivote(cellVector, i, j);
        fastSort(cellVector, i, p - 1);
        fastSort(cellVector, p + 1, j);
    }
}

void swap(Cell a, Cell b) {
    Cell aux = a;
    a = b;
    b = aux;
}

// De https://www.delftstack.com/es/tutorial/algorithm/heap-sort/
void heapify(std::vector<Cell>& cellVector, int n, int i) {
    int pariente = i;
    int hijoIzq = 2 * i + 1;
    int hijoDer = 2 * i + 2;
    if(hijoIzq < n && cellVector[hijoIzq].getValue() > cellVector[pariente].getValue()) {
        pariente = hijoIzq;
    }
    if(hijoDer < n && cellVector[hijoDer].getValue() > cellVector[pariente].getValue()) {
        pariente = hijoDer;
    }
    if(pariente != i) {
        swap(cellVector[i], cellVector[pariente]);
        heapify(cellVector, n, pariente);
    }
}

// De https://www.delftstack.com/es/tutorial/algorithm/heap-sort/
void heapSort(std::vector<Cell>& cellVector, int n) {
    for(int i = n / 2 - 1; i >= 0; i--) {
        heapify(cellVector, n, i);
    }
    for(int i = n - 1; i >= 0; i--) {
        swap(cellVector[0], cellVector[i]);
        heapify(cellVector, i, 0);
    }
}

Cell selection(std::vector<Cell> cellVector, int tipo) { 
    Cell aux;
    switch(tipo) {
        case 0:     // Sin preordenación
            aux = cellVector.front();
            for(std::vector<Cell>::iterator itCells = cellVector.begin(); itCells != cellVector.end(); itCells++) {
                if(itCells->getValue() < aux.getValue()) {
                    aux = (*itCells);
                }
            }
            break;
        case 1:     // Ordenación por fusión
            aux = cellVector.front();
            break;
        case 2:     // Ordenación rápida
            aux = cellVector.front();
            break;
        case 3:     // Ordenación por montículo
            aux = cellVector.front();
            break;
    }
    return aux;
}

void placeDefenses_SPO(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses, float cellWidth, float cellHeight) {
    /* float values[nCellsHeight][nCellsWidth];    // Matriz que almacena los valores de cada celda
    std::vector<Cell> cellVector;
    // Rellenamos la matriz de valores para colocar las defensas
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            values[i][j] = defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
        }
    }
    // Agrupamos las celdas en un vector
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            Cell c(i, j, values[i][j]);
            cellVector.push_back(c);
        }
    }
    // Colocamos las defensas
    List<Defense*>::iterator itDefense = defenses.begin();
    bool placed;
    while(itDefense != defenses.end()) {    // Mientras haya defensas sin colocar...
        placed = false;
        while(!placed) {
            // Seleccionamos la celda más prometedora
            Cell promisingCell = selection(cellVector, 0);  // promisingCell guarda la celda más prometedora
            if(factibility(*itDefense, promisingCell.getRow(), promisingCell.getCol(), obstacles, defenses, cellWidth, cellHeight, mapWidth, mapHeight)) {
                // Colocamos la defensa en el centro de la celda más prometedora
                (*itDefense)->position = cellCenterToPosition(promisingCell.getRow(), promisingCell.getCol(), cellWidth, cellHeight);
                placed = true;
            }
            // Sacamos del vector la celda procesada
            cellVector.erase(//posicion de promisingCell dentro del vector cellVector);
        }
        itDefense++;
    } */
}

void placeDefenses_OF(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses, float cellWidth, float cellHeight) {
    std::vector<Cell> cellVector;
    // Agrupamos las celdas en un vector
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            Cell c(i, j, defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses));
            cellVector.push_back(c);
        }
    }
    // Ordenamos el vector con el algoritmo de ordenación por fusión
    fusionSort(cellVector, 0, cellVector.size()-1);
    // Colocamos las defensas
    List<Defense*>::iterator itDefense = defenses.begin();
    bool placed;
    while(itDefense != defenses.end()) {    // Mientras haya defensas sin colocar...
        placed = false;
        while(!placed) {
            // Seleccionamos la celda más prometedora
            Cell promisingCell = selection(cellVector, 1);  // promisingCell guarda la celda más prometedora
            if(factibility(*itDefense, promisingCell.getRow(), promisingCell.getCol(), obstacles, defenses, cellWidth, cellHeight, mapWidth, mapHeight)) {
                // Colocamos la defensa en el centro de la celda más prometedora
                (*itDefense)->position = cellCenterToPosition(promisingCell.getRow(), promisingCell.getCol(), cellWidth, cellHeight);
                placed = true;
            }
            // Sacamos del vector la celda procesada ( la primera del vector )
            cellVector.erase(cellVector.begin());
        }
        itDefense++;
    }
}

void placeDefenses_OR(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses, float cellWidth, float cellHeight) {
    std::vector<Cell> cellVector;
    // Agrupamos las celdas en un vector
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            Cell c(i, j, defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses));
            cellVector.push_back(c);
        }
    }
    // Ordenamos el vector con el algoritmo de ordenación rápida
    fastSort(cellVector, 0, cellVector.size()-1);
    // Colocamos las defensas
    List<Defense*>::iterator itDefense = defenses.begin();
    bool placed;
    while(itDefense != defenses.end()) {    // Mientras haya defensas sin colocar...
        placed = false;
        while(!placed) {
            // Seleccionamos la celda más prometedora
            Cell promisingCell = selection(cellVector, 2);  // promisingCell guarda la celda más prometedora
            if(factibility(*itDefense, promisingCell.getRow(), promisingCell.getCol(), obstacles, defenses, cellWidth, cellHeight, mapWidth, mapHeight)) {
                // Colocamos la defensa en el centro de la celda más prometedora
                (*itDefense)->position = cellCenterToPosition(promisingCell.getRow(), promisingCell.getCol(), cellWidth, cellHeight);
                placed = true;
            }
            // Sacamos del vector la celda procesada ( la primera del vector )
            cellVector.erase(cellVector.begin());
        }
        itDefense++;
    }
}

void placeDefenses_OM(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses, float cellWidth, float cellHeight) {
    std::vector<Cell> cellVector;
    // Agrupamos las celdas en un vector
    for(int i = 0; i < nCellsHeight; i++) {
        for(int j = 0; j < nCellsWidth; j++) {
            Cell c(i, j, defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses));
            cellVector.push_back(c);
        }
    }
    // Ordenamos el vector con el algoritmo de ordenación por montículos
    heapSort(cellVector, cellVector.size());
    // Colocamos las defensas
    List<Defense*>::iterator itDefense = defenses.begin();
    bool placed;
    while(itDefense != defenses.end()) {    // Mientras haya defensas sin colocar...
        placed = false;
        while(!placed) {
            // Seleccionamos la celda más prometedora
            Cell promisingCell = selection(cellVector, 2);  // promisingCell guarda la celda más prometedora
            if(factibility(*itDefense, promisingCell.getRow(), promisingCell.getCol(), obstacles, defenses, cellWidth, cellHeight, mapWidth, mapHeight)) {
                // Colocamos la defensa en el centro de la celda más prometedora
                (*itDefense)->position = cellCenterToPosition(promisingCell.getRow(), promisingCell.getCol(), cellWidth, cellHeight);
                placed = true;
            }
            // Sacamos del vector la celda procesada ( la primera del vector )
            cellVector.erase(cellVector.begin());
        }
        itDefense++;
    }
}

void DEF_LIB_EXPORTED placeDefenses3(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) {

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    float E_ABS = 0.01;
    float E_REL = 0.001;

    // SIN PREORDENACIÓN

/*     
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
    } while(c1.tiempo() < E_ABS/E_REL+E_ABS);
    c1.parar();
    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c1.tiempo() / r1 << '\t' << c1.tiempo() / r1 << '\t' << c1.tiempo() / r1 << '\t' << c1.tiempo() / r1 << std::endl;
 */
    // ORDENACIÓN POR FUSIÓN
    // std::cout << "ORDENACION POR FUSION" << std::endl;
    cronometro c2;
    long int r2 = 0;
    c2.activar();
    do {	
		List<Defense*>::iterator currentDefense = defenses.begin();
		while(currentDefense != defenses.end()) {
			placeDefenses_OF(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, cellWidth, cellHeight);
            currentDefense++;
		}
		r2++;
    } while(c2.tiempo() < E_ABS/E_REL+E_ABS);
    c2.parar();
    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c2.tiempo() / r2 << std::endl;

    // ORDENACIÓN RÁPIDA
    // std::cout << "ORDENACION RAPIDA" << std::endl;
    cronometro c3;
    long int r3 = 0;
    c3.activar();
    do {	
		List<Defense*>::iterator currentDefense = defenses.begin();
		while(currentDefense != defenses.end()) {
			placeDefenses_OR(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, cellWidth, cellHeight);
            currentDefense++;
		}
		r3++;
    } while(c3.tiempo() < E_ABS/E_REL+E_ABS);
    c3.parar();
    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c3.tiempo() / r3 << std::endl;

    // ORDENACIÓN POR MONTÍCULOS
    // std::cout << "ORDENACION POR MONTICULOS" << std::endl;
    cronometro c4;
    long int r4 = 0;
    c4.activar();
    do {	
		List<Defense*>::iterator currentDefense = defenses.begin();
		while(currentDefense != defenses.end()) {
			placeDefenses_OM(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, cellWidth, cellHeight);
            currentDefense++;
		}   
		r4++;
    } while(c4.tiempo() < E_ABS/E_REL+E_ABS);
    c4.parar();
    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c4.tiempo() / r4 << std::endl;

}
