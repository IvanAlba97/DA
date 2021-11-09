// ###### Config options ################


// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

using namespace Asedio;

void DEF_LIB_EXPORTED selectDefenses(std::list<Defense*> defenses, unsigned int ases, std::list<int> &selectedIDs
            , float mapWidth, float mapHeight, std::list<Object*> obstacles) {

/* std::list<Defense*>::iterator it = defenses.begin();
selectedIDs.push_back((*it)->id);
ases -= (*i)->cost;     DESCOMENTAR */
// CREAR LAS ESTRUCTURAS PARA GESTIONAR LA TABLA DE SUBPROBLEMAS RESUELTOS
// RELLENAR LA TSP (CODIFICAR EL ALGORITMO DE LA MOCHILA)
    // POR CADA DEFENSA CALCULAR SU valor()
// RECUPERAR LA SOLUCION Y GUARDAR LOS id EN selectedIDs

    unsigned int cost = 0;
    std::list<Defense*>::iterator it = defenses.begin();
    while(it != defenses.end()) {
        if(cost + (*it)->cost <= ases) {
            selectedIDs.push_back((*it)->id);
            cost += (*it)->cost;
        }
        ++it;
    }
}
