#ifndef GRAFO_H_
#define GRAFO_H_

#include "ej.h"

class Grafo{
    public:
        Grafo();
        Grafo(int cantNodos);
        Grafo& operator=(const Grafo& otro);

        vector<Nodo>& nodos();
        Nodo& nodo(int id);
        float distancia(const Nodo& n1, const Nodo& n2);

        void asignarDistancia(const Nodo& n1, const Nodo& n2, float distancia);
        void asignarNodo(const Nodo& n);

    private: 
        vector<Nodo> _nodos;
        vector<vector<float>> _distancias;
};

Grafo::Grafo(){

}

Grafo::Grafo(int cantNodos){
    _distancias.resize(cantNodos, vector<float>(cantNodos));
}

Grafo& Grafo::operator=(const Grafo& otro){
    _nodos = otro._nodos;
    _distancias = otro._distancias;
    return *this;
}

vector<Nodo>& Grafo::nodos(){
    return _nodos;
}

Nodo& Grafo::nodo(int id){
    return nodos()[id - 1];
}

float Grafo::distancia(const Nodo& n1, const Nodo& n2){
    return _distancias[n1.id - 1][n2.id - 1];
}

void Grafo::asignarDistancia(const Nodo& n1, const Nodo& n2, float distancia){
    _distancias[n1.id - 1][n2.id - 1] = distancia;
}

void Grafo::asignarNodo(const Nodo& n){
    assert(n.id == nodos().size() + 1);

    nodos().push_back(n);
}

#endif