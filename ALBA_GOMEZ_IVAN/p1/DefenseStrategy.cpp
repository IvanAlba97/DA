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

float cellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
	, float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
	return 0; // implemente aqui la funci�n que asigna valores a las celdas     PRIMERA DEFENSA
}

float cellValueRest(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
	, float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
	return 0; // implemente aqui la funci�n que asigna valores a las celdas     RESTO DE DEFENSAS (RODEANDO A LA PRIMERA DEFENSA)
}

// CREAR AQUÍ LA FUNCIÓN DE FACTIBILIDAD
// BOOL FACTIBLE(ROW, COL, DEFENSA_A_COLOCAR, MAPWIDTH, MAPHEIGHT, OBSTACLES, DEFENSES, CELLWIDTH, CELLHEIGHT) {
    // COMPROBAR SI SE SALE DEL MAPA
    // VECTOR3 POSICION_CENTRAL = CELLCENTERTOPOSITION(ROW, COL)
    // RADIO_DEFENSA = DEFENSA->RADIO
    // SI(POSICION_CENTRAL.X + RADIO_DEFENSA > MAPWIDTH || POSICION_CENTRAL.X - RADIO_DEFENSA < 0 || LO MISMO POR ARRIBA || LO MISMO POR ABAJO)
// }

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
