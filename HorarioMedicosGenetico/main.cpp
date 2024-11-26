/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * File:   main.cpp
 * Author: VLADYMIR
 *
 * Created on 26 de noviembre de 2024, 12:23
 */

#include <iostream>
#include <vector>
#include <bitset>
#include <ctime>
#include <cmath>
#include <map>
#include <algorithm>
#define NIND 10
#define Tseleccion 0.3
#define Pcasamiento 0.5
#define NITERACIONES 1000
#define Tmutacion 0.5
#define NDias 7
#define NTurnos 4
#define NMedicos 4
using namespace std;

/*
TURNOS = 3;  // Ejemplo: 0 - no viene, 1 - mañana, 2 - tarde, 3 - noche 
DIAS = 7;    // Ejemplo: 0 - lunes, 1 - martes, ..., 6 - domingo
MEDICOS = 5; // Número de médicos
 
 */


void generaHorario(vector <int> &vaux, int n) {

    for (int i = 0; i < n; i++) {
        vaux.push_back(rand() % NTurnos);
    }
}

//, vector<vector<int>> Vrequisitos

int calcularFitnessHorario(vector<int> cromo) {
    int fitness=0;
    
    
    //Lunes Mañana Martes noche
    for (int dia = 1; dia < cromo.size(); ++dia) {
        if (dia % 8 != 0) {
            if (cromo[dia] == 1 && cromo[dia+1] == 3) {
                fitness += 2; 
            }
        }

    }
    // Penalización por turnos consecutivos de noche y mañana
    
    for (int dia = 1; dia < cromo.size(); ++dia) {
        if (dia % 8 != 0) {
            if (cromo[dia] == 3 && cromo[dia+1] == 1) {
                fitness -= 2; // Penalización más alta
            }
        }

    }
    return fitness;
}

void generapoblacioninicial(vector<vector<int>> &poblacion) {
    int cont = 0;

    while (cont < NIND) {
        vector <int> vaux;
        // Medido
        for (int i = 0; i < NMedicos; i++) {
            vaux.push_back(i);
            generaHorario(vaux, NDias);

        }
        poblacion.push_back(vaux);
        /* if (!aberracion(vaux, Vrequisitos)) {
            
             
         }*/
        cont++;
    }

}

void muestrapoblacion(vector<vector<int>>poblacion) {

    for (int i = 0; i < poblacion.size(); i++) {
        for (int j = 0; j < poblacion[i].size(); j++) {;
            if (j % 8 == 0) cout << "Medico";
            cout << poblacion[i][j] << "  ";
        }
        cout<<"fit: "<<calcularFitnessHorario(poblacion[i]);
        cout << endl;
    }
}

int main(int argc, char** argv) {

    int cont = 0;
    vector<vector<int>> poblacion;


    generapoblacioninicial(poblacion);


    muestrapoblacion(poblacion);

    return 0;
}

