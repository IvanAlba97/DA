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

// COGER DEL CAPUS (PREGUNTAS FRECUENTES) LA FUNCIÓN QUE DADA UNA CELDA DEVUELVE LA POSICIÓN SU CENTRO Y PEGARLA AQUÍ.

// Devuelve la posición en el mapa del centro de la celda (i,j)
Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight) {
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0);
}

// Asigna valores a las celdas para colocar la primera defensa
float cellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
	, float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    float totalDistance = 0;
    List<Object*>::iterator itObstacle = obstacles.begin();
    while(itObstacle != obstacles.end()) {
        totalDistance += _distance(cellCenterToPosition(row, col, cellWidth, cellHeight), (*itObstacle)->position);
    }
	return totalDistance;
}

// Asigna valores a las celdas para colocar el resto de defensas
float cellValueRest(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
	, float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    List<Defense*>::iterator itDefense = defenses.begin();  // El primer elemento de la lista de defensas es el centro de extracción
	return _distance(cellCenterToPosition(row, col, cellWidth, cellHeight), (*itDefense)->position);
}

// CREAR AQUÍ LA FUNCIÓN DE FACTIBILIDAD
// BOOL FACTIBLE(ROW, COL, DEFENSA_A_COLOCAR, MAPWIDTH, MAPHEIGHT, OBSTACLES, DEFENSES, CELLWIDTH, CELLHEIGHT) {
    // COMPROBAR SI SE SALE DEL MAPA
    // VECTOR3 POSICION_CENTRAL = CELLCENTERTOPOSITION(ROW, COL)
    // RADIO_DEFENSA = DEFENSA->RADIO
    // EL SIGUIENTE SI() ES PARA COMPROBAR QUE LA DEFENSA NO SE SALE DEL MAPA:
    // SI(POSICION_CENTRAL.X + RADIO_DEFENSA > MAPWIDTH || POSICION_CENTRAL.X - RADIO_DEFENSA < 0 || LO MISMO POR ARRIBA || LO MISMO POR ABAJO) {
        // FACTIBLE = FALSO
    // }
    // COMPROBAR QUE NO CHOQUE CON NINGUN OBSTACULO
    // RETURN FACTIBLE
// }

bool factibility(Defense* defense, int row, int col, std::list<Object*> obstacles, std::list<Defense*> defenses, int cellWidth, int cellHeight, float mapWidth, float mapHeight) {
    bool factible = true;
    Vector3 centralPosition = cellCenterToPosition(row, col, cellWidth, cellHeight);
    float defenseRadio = defense->radio;
    // Comprobamos que la defensa no se sale del mapa
    if(centralPosition.x + defenseRadio > mapWidth || centralPosition.x - defenseRadio < 0 || 
        centralPosition.y + defenseRadio > mapHeight || centralPosition.y - defenseRadio < 0) {
        factible = false;
    } else {
        // Comprobamos que la defensa no choca con nunguna defensa ya colocada
        List<Defense*>::iterator itDefense = defenses.begin();
        while(itDefense != defenses.end() && factible) {
            if(defense->radio + (*itDefense)->radio > _distance(defense->position, (*itDefense)->position)) {
                factible = false;
            }
        }
        // Comprobamos que la defensa no choca con ningún obstáculo
        List<Object*>::iterator itObstacle = obstacles.begin();
        while(itObstacle != obstacles.end() && factible) {
            if(defense->radio + (*itObstacle)->radio > _distance(defense->position, (*itObstacle)->position)) {
                factible = false;
            }
        }
    }

    return false;
}

void DEF_LIB_EXPORTED placeDefenses(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses) {
        
                // ELIMINAR EL CONTENIDO DE ESTA FUNCIÓN Y HACERLA YO CORRECTAMENTE.
                // CREAR UNA ESTRUCTURA PARA ALMACENAR LOS VALORES DE LAS CELDAS
                // RECORRER LAS CELDAS LLAMANDO A CELLVALUE(...) Y ALMACENAR EL VALOR QUE DEVUELVA EN LA ESTRUCTURA ANTERIOR
                // PARA LA DEFENSA PRINCIPAL:
                    // MIENTRAS NO TENGA POSICION LA DEFENSA PRINCIPAL:
                        // SELECCIONAR LA CELDA MÁS PROMETEDORA
                        // IF(FACTIBLE(...)) LA CELDA CANDIDATA PARA COLOCAR LA DEFENSA PRINCIPAL
                            // COLOCARLA
                    // FIN MIENTRAS
                // RECORRER LAS CELDAS LLAMANDO A CELLVALUEREST(...) Y ALMACENAR EL VALOR QUE DEVUELVA EN LA ESTRUCTURA ANTERIOR
                // PARA LEL RESTO DE DEFENSAS:
                    // MIENTRAS HAYA DEFENSAS SIN POSICIÓN:
                        // SELECCIONAR LA CELDA MÁS PROMETEDORA
                        // IF(FACTIBLE(...)) LA CELDA CANDIDATA PARA COLOCAR LA DEFENSA PRINCIPAL
                            // COLOCARLA
                            // SACAR LA SIGUIENTE DEFENSA
                    // FIN MIENTRAS

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 

    int maxAttemps = 1000;
    List<Defense*>::iterator currentDefense = defenses.begin();
    while(currentDefense != defenses.end() && maxAttemps > 0) {

        (*currentDefense)->position.x = ((int)(_RAND2(nCellsWidth))) * cellWidth + cellWidth * 0.5f;
        (*currentDefense)->position.y = ((int)(_RAND2(nCellsHeight))) * cellHeight + cellHeight * 0.5f;
        (*currentDefense)->position.z = 0; 
        ++currentDefense;
    }

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
