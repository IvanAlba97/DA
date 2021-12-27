// ###### Config options ################

#define PRINT_PATHS 1

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_PATHS
#include "ppm.h"
#endif

using namespace Asedio;

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight){ 
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0); 
}

bool comp(AStarNode* n1, AStarNode* n2) {
    return n1->F > n2->F;
}

// Se llama al principio del programa y cuando algún UCO destruye alguna defensa
// Esta función asigna un valor a una celda en función de si tiene defensas cerca o no
void DEF_LIB_EXPORTED calculateAdditionalCost(float** additionalCost
                   , int cellsWidth, int cellsHeight, float mapWidth, float mapHeight
                   , List<Object*> obstacles, List<Defense*> defenses) {

    float cellWidth = mapWidth / cellsWidth;
    float cellHeight = mapHeight / cellsHeight;

    List<Defense*>::iterator itDefense = defenses.begin(); itDefense++;
    List<Object*>::iterator itObstacle = obstacles.begin();

    for(int i = 0; i < cellWidth; i++) {
        for(int j = 0; j < cellHeight; j++) {
            Vector3 cellPosition = cellCenterToPosition(i, j, cellWidth, cellHeight);
            float cost = 1000;
            while(itDefense != defenses.end()) {
                if(_distance(cellPosition, (*itDefense)->position) < 10) {
                    cost *= 2;
                } else {
                    cost /= 2;
                }
                itDefense++;
            }
            while(itObstacle != obstacles.end()) {
                if(_distance(cellPosition, (*itObstacle)->position) < 10) {
                    cost *= 2;
                } else {
                    cost /= 2;
                }
                itObstacle++;
            }
            additionalCost[i][j] = cost;
        }
    }
}

// Se llama cuando se crea el UCO y cuando algún UCO destruye alguna defensa
// Esta función guarda en path el camino que va siguiendo el UCO
void DEF_LIB_EXPORTED calculatePath(AStarNode* originNode, AStarNode* targetNode
                   , int cellsWidth, int cellsHeight, float mapWidth, float mapHeight
                   , float** additionalCost, std::list<Vector3> &path) {
    
    // ALGORITMO A*
    
    std::vector<AStarNode*> opened, closed;
    std::make_heap(opened.begin(), opened.end());
    AStarNode* cur = originNode;
    cur->G = 0;
	cur->H = additionalCost[(int)(cur->position.y / cellsHeight)][(int)(cur->position.x / cellsWidth)];
	cur->parent = NULL;
	cur->F = cur->G + cur->H;
    opened.push_back(cur);
    std::push_heap(opened.begin(), opened.end(), comp);
    bool found = false;
    while(!false && !opened.empty()) {
        std::pop_heap(opened.begin(), opened.end(), comp);
        cur = opened.back();
        opened.pop_back();
        closed.push_back(cur);
        if(cur == targetNode) {
            found = true;
        } else {
            for(std::list<AStarNode*>::iterator j = cur->adjacents.begin(); j != cur->adjacents.end(); j++) {
                if(std::find(closed.begin(), closed.end(), *j) == closed.end()) {
                    if(std::find(opened.begin(), opened.end(), *j) == opened.end()) {
                        (*j)->parent = cur;
                        (*j)->G = cur->G + _distance(cur->position, (*j)->position);
                        (*j)->H = additionalCost[(int)((*j)->position.y / cellsHeight)][(int)((*j)->position.x / cellsWidth)];
                        (*j)->F = (*j)->G + (*j)->H;
                        opened.push_back(*j);
                        std::push_heap(opened.begin(), opened.end(), comp);
                    } else {
                        float d = _distance(cur->position, (*j)->position);
                        if((*j)->G > cur->G + d) {
                            (*j)->parent = cur;
							(*j)->G = cur->G + d;
							(*j)->F = (*j)->G + (*j)->H;
                            std::make_heap(opened.begin(), opened.end());
                        }
                    }
                }
            }
        }
    }

    // RECUPERACIÓN DEL CAMINO

    cur = targetNode;
    path.push_front(targetNode->position);
    while(cur->parent != originNode && cur->parent != nullptr) {
        cur = cur->parent;
        path.push_front(cur->position);
    }
}


/**
 * Para uso de montículos me he inspirado de las siguentes páginas web:
 * https://en.cppreference.com/w/cpp/algorithm
 * https://www.geeksforgeeks.org/heap-using-stl-c/
 */