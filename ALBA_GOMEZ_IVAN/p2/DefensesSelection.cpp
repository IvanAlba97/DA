// ###### Config options ################


// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

using namespace Asedio;

// Clase auxiliar donde se almacena el identificador, el coste y el valor que yo considere para cada defensa
class Aux {
    public:
        Aux(int _id, float _cost, float _value) : id(_id), cost(_cost), value(_value) {}
        void setId(int _id) { id = _id; }
        void setCost(int _cost) { cost = _cost; }
        void setValue(float _value) { value = _value; }
        int getId() { return id; }
        int getCost() { return cost; }
        float getValue() { return value; }
    private:
        int id, cost;
        float value;
};

// Función que devuelve el valor de una defensa según mi criterio
float defenseValue(std::list<Defense*>::iterator it) {

        /**
         * Mi criterio de importancia (0-100%):
         * range -------------------- 5%
         * dispersion --------------- 15%
         * damage ------------------- 30%
         * attacksPerSecond  -------- 20%
         * health ------------------- 30%
         */

    return (  0.05 * (*it)->range 
            + 0.15 * (*it)->dispersion 
            + 0.3 * (*it)->damage 
            + 0.2 * (*it)->attacksPerSecond 
            + 0.3 * (*it)->health
            ) / (*it)->cost;
}

// Función que resuelve la tabla de subproblemas resueltos
void mochila(float** tsp, unsigned int ases, std::list<Aux> myDefenses) {
    std::list<Aux>::iterator it = myDefenses.begin();
    for(int j = 0; j <= ases && it != myDefenses.end(); j++) {
        if(j < it->getCost()) tsp[0][j] = 0;
        else tsp[0][j] = it->getValue();
        it++;
    }
    it = myDefenses.begin(); it++;
    for(int i = 1; i <= myDefenses.size() && it != myDefenses.end(); i++) {
        for(int j = 0; j <= ases; j++) {
            if(j < it->getCost()) tsp[i][j] = tsp[i-1][j];
            else tsp[i][j] = std::max(tsp[i-1][j], tsp[i-1][j-it->getCost()] + it->getValue());
        }
        it++;
    }
}

// Función que almacena los ids de las defensas seleccionadas en selectedIDs
void selectIds(float** tsp, unsigned int ases, std::list<Aux> myDefenses, std::list<int> &selectedIDs) {
    std::list<Aux>::iterator it = myDefenses.end()--;   // end() devuelve la siguiente posición a la última, por eso el --
    int j = ases;
    for(int i = myDefenses.size()-1; i > 0; i--, it--) {
        if(tsp[i][j] != tsp[i-1][j]) {
            selectedIDs.push_back(it->getId());
            //std::cout<<it->getId()<<std::endl;
            j = j - it->getCost();
        }
    }
    if(tsp[0][j] != 0) {
        selectedIDs.push_back(myDefenses.begin()->getId());
    }
}

void DEF_LIB_EXPORTED selectDefenses(std::list<Defense*> defenses, unsigned int ases, std::list<int> &selectedIDs
            , float mapWidth, float mapHeight, std::list<Object*> obstacles) {
    // Como la primera defensa es el centro de extracción, hay que comprarla siempre, por lo tanto...
    std::list<Defense*>::iterator it = defenses.begin();// Como la primera defensa es el centro de extracción, hay que comprarla
    selectedIDs.push_back((*it)->id);
    ases -= (*it)->cost;
    defenses.erase(it);
    // Creamos la estructura para gestionar la tabla de subproblemas resueltos
    float** tsp = new float*[defenses.size()];
    for(size_t i = 0; i < defenses.size(); ++i) tsp[i] = new float[ases+1];
    // Creamos una lista de defensas (Lista de Aux)
    std::list<Aux> myDefenses;
    // Le damos valores
    it = defenses.begin();
    for(it = defenses.begin(); it != defenses.end(); it++) {
        myDefenses.push_back(Aux((*it)->id, (*it)->cost, defenseValue(it)));
    }
    // Rellenamos la tabla usando el algoritmo de la mochila discreta
    mochila(tsp, ases, myDefenses);
    // Añadimos a selectedIDs los ids de las defensas
    selectIds(tsp, ases, myDefenses, selectedIDs);
    // Para imprimir la lista selectedIDs
    //for( auto item : selectedIDs ) std::cout << item << std::endl;
}
