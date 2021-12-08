
/**
 * Pruebas de caja negra.
 * Para estas pruebas se ha utilizado un vector de enteros, 
 * en vez de un vector de Cell.
 */

#include <vector>
#include <random>
#include <iostream>

void insertionSort(std::vector<int>& cellVector, int i, int j) {
    int cell;
    for (int l = i + 1; l <= j; l++) {
        cell = cellVector[l];
        int p = l - 1;
        while (p >= i && cellVector[p] < cell) {
            cellVector[p+1] = cellVector[p];
            p--;
        }
        cellVector[p+1] = cell;
    }
}

// ORDENACIÓN POR FUSIÓN

void fusion(std::vector<int>& cellVector, int i, int k, int j) {
    int n = j - i + 1;
    int p = i;
    int q = k + 1;
    std::vector<int> w(n);
    for(int l = 0; l < n; l++) {
        if(p <= k && (q > j || cellVector[p] <= cellVector[q])) {
            w[l] = cellVector[p];
            p++;
        } else {
            w[l] = cellVector[q];
            q++;
        }
    }
    for(int l = 0; l < n; l++) { cellVector[i+l] = w[l]; }
}

void fusionSort(std::vector<int>& cellVector, int i, int j) {
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

// ORDENACIÓN RÁPIDA

int pivote(std::vector<int>& cellVector, int i, int j) {
    int p = 1;
    int x = cellVector[i];
    for(int k = i + 1; k < j; k++) {
        if(cellVector[k] <= x) {
            p++;
            int aux = cellVector[p];
            cellVector[p] = cellVector[k];
            cellVector[k] = aux;
        }
    }
    cellVector[i] = cellVector[p];
    cellVector[p] = x;
    return x;
}

void fastSort(std::vector<int>& cellVector, int i, int j) {
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

// ORDENACIÓN POR MONTÍCULOS

void swap(int a, int b) {
    int aux = a;
    a = b;
    b = aux;
}

void heapify(std::vector<int>& cellVector, int n, int i) {
    int pariente = i;
    int hijoIzq = 2 * i + 1;
    int hijoDer = 2 * i + 2;
    if(hijoIzq < n && cellVector[hijoIzq] > cellVector[pariente]) {
        pariente = hijoIzq;
    }
    if(hijoDer < n && cellVector[hijoDer] > cellVector[pariente]) {
        pariente = hijoDer;
    }
    if(pariente != i) {
        swap(cellVector[i], cellVector[pariente]);
        heapify(cellVector, n, pariente);
    }
}

void heapSort(std::vector<int>& cellVector, int n) {
    for(int i = n / 2 - 1; i >= 0; i--) {
        heapify(cellVector, n, i);
    }
    for(int i = n - 1; i >= 0; i--) {
        swap(cellVector[0], cellVector[i]);
        heapify(cellVector, i, 0);
    }
}

// IMPRIMIR

void imprimir(std::vector<int>& v) {
    for(int i = 0; i < v.size(); i++) {
        std::cout << v[i] + " " << std::endl;
    }
}

// MAIN

int main() {

    int N = 5;

    std::vector<int> v1;
    std::vector<int> v2;
    std::vector<int> v3;

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            v1.push_back(rand() % 100);     // rand de 0 a 99
            v2.push_back(rand() % 100);     // rand de 0 a 99
            v3.push_back(rand() % 100);     // rand de 0 a 99
        }
    }

    std::cout << "VECTOR V1" << std::endl;
    imprimir(v1);
    fusionSort(v1, 0, v1.size() - 1);
    imprimir(v1);

    std::cout << "VECTOR V2" << std::endl;
    imprimir(v2);
    fastSort(v2, 0, v2.size() - 1);
    imprimir(v2);

    std::cout << "VECTOR V3" << std::endl;
    imprimir(v3);
    heapSort(v3, v3.size());
    imprimir(v3);
}