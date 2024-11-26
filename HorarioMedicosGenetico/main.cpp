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

int calcularFitnessHorario(vector<int> cromo,map<int, vector<int>> preferencias) {
    int fitness=0;
    
    for (int i = 0; i < cromo.size(); i += 8) {
        int idMedico = cromo[i]; // ID del médico
        const auto& horario = vector<int>(cromo.begin() + i + 1, cromo.begin() + i + 8); // Horario del médico

        // Verificar si el médico tiene preferencias definidas
        if (preferencias.find(idMedico) != preferencias.end()) {
            const auto& pref = preferencias.at(idMedico);

            // Evaluar el horario comparándolo con las preferencias
            for (int dia = 0; dia < horario.size(); ++dia) {
                // Si el turno asignado coincide con la preferencia, sumar fitness
                if (horario[dia] == pref[dia]) {
                    fitness += 4; // Coincidencia exacta
                }
            }
        }
    }
    
    
    
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

void muestrapoblacion(vector<vector<int>>poblacion,map<int, vector<int>> preferencias) {

    for (int i = 0; i < poblacion.size(); i++) {
        for (int j = 0; j < poblacion[i].size(); j++) {;
            if (j % 8 == 0) cout << "Medico";
            cout << poblacion[i][j] << "  ";
        }
        cout<<"fit: "<<calcularFitnessHorario(poblacion[i],preferencias);
        cout << endl;
    }
}

int main(int argc, char** argv) {

    int cont = 0;
    vector<vector<int>> poblacion;
    map<int, vector<int>> preferencias;
    // Médico 0: No trabajar lunes, martes, miércoles; jueves mañana, viernes tarde, sábado noche, domingo tarde
    preferencias[0] = {0, 0, 0, 1, 2, 3, 2};
    // Médico 1: Prefiere trabajar lunes y martes por la mañana, miércoles tarde, jueves tarde, viernes noche, no sábado ni domingo
    preferencias[1] = {1, 1, 2, 2, 3, 0, 0};
    // Médico 2: Prefiere no trabajar lunes, martes noche, miércoles mañana, jueves no trabaja, viernes tarde, sábado noche, domingo mañana
    preferencias[2] = {0, 3, 1, 0, 2, 3, 1};
    // Médico 3: Prefiere no trabajar lunes, martes y miércoles; jueves noche, viernes mañana, sábado tarde, domingo noche
    preferencias[3] = {0, 0, 0, 3, 1, 2, 3};
    
    
    generapoblacioninicial(poblacion);


    muestrapoblacion(poblacion,preferencias);

    return 0;
}

