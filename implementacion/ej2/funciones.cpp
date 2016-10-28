#include "funciones.h"

int idx_actual;
int mochila = 0;
int gym_no_recorridos;
int paradas_no_recorridas;
int cant_gym;
int cant_paradas;


void solHeuristicaGolosa(unsigned int mochila_size, vector<struct gym>& gimnasios, vector<struct parada>& paradas, 
							vector<vector<float>>& matriz_dist)
{
	// Caso: No hay gimnasios => Gane
	// Caso: Si la cantidad de paradas*3 < suma total de pociones para ganarle a todos los gimasios => -1
	// Caso: (No hay pokeparadas o mochila_size == 0) y al menos un gimnasio tiene p>0 => -1
	// Caso: (No hay pokeparadas o mochila_size == 0) y los gimnasios tienen p=0 => Gane
	// Si es uno de estos casos lo soluciona, imprime y termina.
	if(solucionCasosParticulares(mochila_size, gimnasios, paradas, matriz_dist))
		return;


	// Busco la mejor solución comenzando por cada parada y cada una de estas soluciones la guardo en el vector soluciones.
	struct solucion mejor_sol;
	mejor_sol.d = -1;

	for(int i = gimnasios.size(); i < paradas.size() + gimnasios.size(); i++){
		struct solucion sol_nueva;
		sol_nueva.d = 0;
		solucionCasoGeneral(i, sol_nueva, mochila_size, gimnasios, paradas, matriz_dist);

		if((mejor_sol.d == -1 && sol_nueva.d != -1) || 
			(sol_nueva.d != -1 && sol_nueva.d < mejor_sol.d))
		{
			mejor_sol = sol_nueva;
		}
	}

	imprimirSolucion(mejor_sol);

	return;
}

/************************************************************************/

bool solucionCasosParticulares(unsigned int mochila_size, vector<struct gym> gimnasios, 
								vector<struct parada> paradas, vector<vector<float>>& matriz_dist)
{
	// Caso: No hay gimnasios => Gane
	if(gimnasios.size() == 0){
		cout << "0 0";
		return true;
	}

	int suma_total_pociones = 0;
	for(vector<gym>::iterator it = gimnasios.begin(); it != gimnasios.end(); it++){
		suma_total_pociones += (*it).p;
	}

	// Caso: Si la cantidad de paradas*3 < suma total de pociones para ganarle a todos los gimasios => -1
	// Caso: (No hay pokeparadas o mochila_size == 0) y al menos un gimnasio tiene p>0 => -1
	if((suma_total_pociones > paradas.size()*3) || 
	   ((paradas.size() == 0 || mochila_size == 0) && suma_total_pociones > 0))
	{
		cout << "-1";
		return true;
	}

	// Caso: No hay pokeparadas y los gimnasios tienen p=0 => Gane
	// Estoy devolviendo una solución lineal, o sea, por como ingresan
	int d = 0;
	if((paradas.size() == 0 || mochila_size == 0) && suma_total_pociones == 0){
		struct solucion mejor_sol;
		mejor_sol.d = -1;

		for(int i = 0; i < gimnasios.size(); i++){
			struct solucion sol_nueva;
			sol_nueva.d = 0;
			solucionCasoGeneral(i, sol_nueva, mochila_size, gimnasios, paradas, matriz_dist);

			if((mejor_sol.d == -1 && sol_nueva.d != -1) || 
				(sol_nueva.d != -1 && sol_nueva.d < mejor_sol.d))
			{
				mejor_sol = sol_nueva;
			}
		}

		imprimirSolucion(mejor_sol);

		return true;
	}

	// No es caso particular
	return false;
}

/************************************************************************/

void solucionCasoGeneral(int idx_comienzo, struct solucion& sol, unsigned int mochila_size, vector<struct gym> gimnasios, 
						 vector<struct parada> paradas, vector<vector<float>>& matriz_dist){

	// Si puedo ganarle a algún gym voy al de menor distancia.

	// Inicializo variables globales
	gym_no_recorridos = gimnasios.size();
	paradas_no_recorridas = paradas.size();
	cant_gym = gimnasios.size();
	cant_paradas = paradas.size();

	// Si puedo ganarle a un gym, voy y le gano, sino voy a la pokeparada mas cercana a recargar
	// Si no hay mas pokeparadas y si gyms -> -1
	// Si hay pokeparadas y no gyms -> gane

	// Comienzo dependiendo de que es idx_comienzo, si es < que gimnasios.size() entonces es el caso particular
	// sino es el caso general y comienzo desde una poke parada.
	idx_actual = idx_comienzo;
	sol.camino.push(idx_actual);
	
	if(idx_comienzo < gimnasios.size()){
		gimnasios[idx_comienzo].visitado = true;
		mochila = 0;	
		gym_no_recorridos--;
	} else {
		// idx_comienzo aqui es indice de paradas + gimnasios.size, entonces para conocer el indice real lo tengo que restar
		paradas[idx_comienzo - gimnasios.size()].visitado = true;
		mochila = 3;
		paradas_no_recorridas--;
	}


	while(gym_no_recorridos > 0){
		// Puedo ganarle al gimnasio con menos pociones?
			// Si y le gano-> pierdo las pociones requeridas, restar gym_no_recorridos, agregar a la cola, sumar distancia.
		if(leGanoAAlgunGym(gimnasios)){
			leGanoAlGymMasCercano(sol, gimnasios, matriz_dist);
			gym_no_recorridos--;

		} else if(mochila == mochila_size || paradas_no_recorridas == 0){
			// No le gano a ningún gym y la mochila está llena => -1
			// No le gana a ningún gym y no hay mas paradas
			break;

		} else {
			voyParadaMasCercana(mochila_size, sol, paradas, matriz_dist);
			paradas_no_recorridas--;
		}
	}

	// Si no recorrio todos los gimnasios entonces no hay solucion
	if(gym_no_recorridos > 0)
		sol.d = -1;

	return;
}

/************************************************************************/

bool leGanoAAlgunGym(vector<struct gym>& gimnasios){
	bool le_gano = false;
	for(int i = 0; i < gimnasios.size(); i++){
		if(!gimnasios[i].visitado && mochila >= gimnasios[i].p){ // No fue visitado y las pociones de la mochila son las necesarias
			le_gano = true;
			break;
		}
	}
	return le_gano;
}

/************************************************************************/

void leGanoAlGymMasCercano(struct solucion& sol, vector<struct gym>& gimnasios, vector<vector<float>>& matriz_dist){
	// Busco al gym que le gano con menor cant de pociones
	int idx_gym = -1;
	int dist;
	for(int i = 0; i < cant_gym; i++){
		if(idx_gym == -1){
			if(!gimnasios[i].visitado && mochila >= gimnasios[i].p){
				idx_gym = i;
				dist = matriz_dist[idx_actual][i];
			}
		} else {
			if(!gimnasios[i].visitado && 
				mochila >= gimnasios[i].p && 
				matriz_dist[idx_actual][i] < dist)
			{
				idx_gym = i;
				dist = matriz_dist[idx_actual][i];
			}
		}
	}

	mochila -= gimnasios[idx_gym].p;
	sol.d += sqrt(matriz_dist[idx_actual][idx_gym]);
	sol.camino.push(idx_gym);
	gimnasios[idx_gym].visitado = true;
	idx_actual = idx_gym;

	return;
}

/************************************************************************/

void voyParadaMasCercana(int mochila_size, struct solucion& sol, vector<struct parada>& paradas, vector<vector<float>>& matriz_dist){
	// Busco parada mas cercana
	int idx_parada = -1;
	for(int i = 0; i < cant_paradas; i++){
		if(idx_parada == -1){
			if(!paradas[i].visitado)
				idx_parada = i;
		} else {
			if(!paradas[i].visitado && matriz_dist[idx_actual][i+cant_gym] < matriz_dist[idx_actual][idx_parada+cant_gym])
				idx_parada = i;
		}
	}

	mochila = (mochila+3) > mochila_size ? mochila_size : (mochila+3);
	sol.d += sqrt(matriz_dist[idx_actual][idx_parada]);
	sol.camino.push(idx_parada + cant_gym);
	idx_actual = idx_parada + cant_gym;
	paradas[idx_parada].visitado = true;

	return;
}

/************************************************************************/

void imprimirSolucion(struct solucion& sol){
	// Imprimo soluciones
	cout << sol.d;
	if(sol.d == -1)
		return;

	cout << " " << sol.camino.size();
	imprimirCola(sol.camino);

	return;
}

/************************************************************************/

void imprimirCola(queue<int>& cola){
	// b == true => paradas  || b == false => gimnasios
	int nodo;
	int size = cola.size();
	for(int i = 0; i < size; i++){
		nodo = cola.front();
		cola.pop();
		cout << " " << nodo + 1;
	}
	return;
}

/************************************************************************/

void imprimirMatriz(vector<vector<float>>& matriz_dist){
	cout << endl << "Matriz" << endl;
	for(int i = 0; i < matriz_dist.size(); i++){
		for(int j = 0; j < matriz_dist.size(); j++){
			cout << "(" << matriz_dist[i][j] << ")";

		}
		cout << endl;
	}
	cout << endl;
	return;
}

/************************************************************************/

void imprimirVector(vector<struct gym>& gimnasios){
	cout << endl << "Gimnasios" << endl;
	for(int i = 0; i < cant_gym; i++){
		cout << gimnasios[i].x << " " << gimnasios[i].y << " " << gimnasios[i].p << " " << gimnasios[i].visitado << endl;
	}
	return;
}

/************************************************************************/

void imprimirVector(vector<struct parada>& paradas){
	cout << endl << "Paradas" << endl;
	for(int i = 0; i < cant_paradas; i++){
		cout << paradas[i].x << " " << paradas[i].y << " " << paradas[i].visitado << endl;
	}
	return;
}

/************************************************************************/

float distancia2(const nodo &n1, const nodo &n2){
	float x = n2.x - n1.x;
	float y = n2.y - n1.y;

	return (x*x + y*y);
}
