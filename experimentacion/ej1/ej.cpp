#include <iostream>
#include <vector>
#include "../../implementacion/ej1/backtracking.h"
#include <fstream>
#include <chrono>

using namespace std;

#define CANT_REPETICIONES 30
#define RELOJ chrono::high_resolution_clock::now

int main(int argc, char* argv[]) {

	//Parseo la entrada
	int n;
	cin >> n;
	int m;
	cin >> m;
	int mochila;
	cin >> mochila;
	// int dificultad;
	// cin >> dificultad;

	vector<pos> gimnasios(n);
	vector<int> gimnasiosPoder(n);
	vector<pos> paradas(m);

	for(int i = 0; i < n; i++) {
		cin >> gimnasios[i].first;
		cin >> gimnasios[i].second;
		cin >> gimnasiosPoder[i];
	}
	for(int i = 0; i < m; i++) {
		cin >> paradas[i].first;
		cin >> paradas[i].second;
	}


	// queue<int> solucion;


	ofstream salida;
	salida.open("salida.csv", std::ios_base::app);
	double cantCiclosTotal = 0;
	float distancia = 0;
	for (int i = 0; i < CANT_REPETICIONES; ++i){

		auto inicio = RELOJ();

		backtracking bt(gimnasios, gimnasiosPoder, paradas, mochila);
		distancia = bt.correr_backtracking();

		auto fin = RELOJ();

		// while(!solucion.empty()) {
		// 	solucion.pop();
		// }

		cantCiclosTotal = cantCiclosTotal + (double) chrono::duration_cast<std::chrono::nanoseconds>(fin-inicio).count();
	}

	salida << n << ',' << m << ',' << mochila << ',' << distancia << ',' << cantCiclosTotal / (double) CANT_REPETICIONES << endl;

	salida.close();

	return 0;
}