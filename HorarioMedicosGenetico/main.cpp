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
#include <map>
#include <chrono>
#include <random>
#include <climits>
#include <algorithm>
#include <cassert>
using namespace std;
#define NIND 15
#define Tseleccion 0.3
#define Tcomplemeto 0.4
#define Pcasamiento 0.5
#define NITERACIONES 2500
#define Tmutacion 0.8
#define NDias 7
#define NTurnos 4
#define NMedicos 4
#define HorarioPerfecto 4
#define PenalizacionNocheManana 2
#define vi vector<int>
#define vvi vector<vi>
#define endl '\n'

map <int, vi> preferencias;

/*  TURNOS = 3;  // Ejemplo: 0 - no viene, 1 - mañana, 2 - tarde, 3 - noche 
    DIAS = 7;    // Ejemplo: 0 - lunes, 1 - martes, ..., 6 - domingo
    MEDICOS = 5; // Número de médicos */
int64_t seed = std::chrono::steady_clock::now().time_since_epoch().count();
std::mt19937_64 gen(seed);

long Random (long l = 1, long r = INT_MAX) {
    return std::uniform_int_distribution<long> (l, r) (gen);
}

void cargarDatos () {
    // Médico 0: No trabajar lunes, martes, miércoles; jueves mañana, viernes tarde, sábado noche, domingo tarde
    preferencias[0] = {0, 3, 0, 1, 2, 3, 2};
    // Médico 1: Prefiere trabajar lunes y martes por la mañana, miércoles tarde, jueves tarde, viernes noche, no sábado ni domingo
    preferencias[1] = {1, 1, 2, 2, 3, 0, 0};
    // Médico 2: Prefiere no trabajar lunes, martes noche, miércoles mañana, jueves no trabaja, viernes tarde, sábado noche, domingo mañana
    preferencias[2] = {2, 3, 2, 0, 2, 3, 1};
    // Médico 3: Prefiere no trabajar lunes, martes y miércoles; jueves noche, viernes mañana, sábado tarde, domingo noche
    preferencias[3] = {3, 0, 1, 3, 3, 2, 3};
}

void generaHorario(vi &vaux, int n) {
    for (int i = 0; i < n; i++) 
        vaux.push_back(Random (0, NTurnos - 1));
}

void generaPoblacionInicial (vvi &poblacion) {
    for (int cont = 0; cont < NIND; cont++) {
        vi vaux;
        for (int i = 0; i < NMedicos; i++) 
            generaHorario(vaux, NDias);
        poblacion.push_back(vaux);
    }
}


int calculaFitness (vi cromo) {
    int fitness = 0;
    int idMedico = 0;
    for (int i = NDias; i < cromo.size(); i += NDias) {
        const auto& horario = vi (cromo.begin() + i + 1, cromo.begin() + i + 8); // Horario del médico

        // Verificar si el médico tiene preferencias definidas
        if (preferencias.find(idMedico) != preferencias.end()) {
            const auto& pref = preferencias.at(idMedico);
            // Evaluar el horario comparándolo con las preferencias
            for (int dia = 0; dia < horario.size(); ++dia) 
                // Si el turno asignado coincide con la preferencia, sumar fitness
                if (horario[dia] == pref[dia]) 
                    fitness += HorarioPerfecto; // Coincidencia exacta define
        }
        idMedico++;
    }

    // Penalización por turnos consecutivos de noche y mañana
    for (int dia = 0; dia < cromo.size(); dia++) {
        if (dia + 1 < cromo.size() && (dia + 1) % NDias != 0 && dia != 0 && cromo[dia] == 3 && cromo[dia + 1] == 1) 
                fitness -= PenalizacionNocheManana; // Penalización más alta factor de k;
    }
    return fitness;
}


void muestraPoblacion (vvi poblacion) {
    for (int i = 0; i < poblacion.size(); i++) {
        for (int j = 0; j < poblacion[i].size(); j++) {
            if (j % NDias == 0) cout << "Medico  ->  ";
            cout << poblacion[i][j] << "  ";
        }
        cout << "fit: " << calculaFitness (poblacion[i]) << endl;
    }
}


void calculaSupervivencia (vvi poblacion, vi &supervivencia) {
    int suma = 0;
    for (int i = 0; i < poblacion.size(); i++)
        suma += calculaFitness (poblacion[i]);
    for (int i = 0; i < poblacion.size(); i++) {
        int fit = round(100 * (double) calculaFitness (poblacion[i]) / suma);
        supervivencia.push_back(fit);
    }
}


void cargaRuleta (vi supervivencia, vi &ruleta) {
    int ind = 0;
    for (int i = 0; i < supervivencia.size(); i++)
        for (int j = 0; j < supervivencia[i]; j++)
            ruleta[ind++] = i;
}


void seleccion (vvi &padres, vvi poblacion) {
    vi ruleta (1000, -1);
    vi supervivencia;
    calculaSupervivencia (poblacion, supervivencia);
    cargaRuleta (supervivencia, ruleta);
    int nseleccionados = poblacion.size() * Tseleccion;
    for (int i = 0; i < nseleccionados; i++) {
        int ind = Random (0, 99);
        if (ruleta[ind] > -1)
            padres.push_back(poblacion[ruleta[ind]]);
    }
}


void generaHijo (vi padre, vi madre, vi &hijo) {
    int pos = round(padre.size() * Pcasamiento);
    for (int i = 0; i < pos; i++)
        hijo.push_back(padre[i]);
    for (int i = pos; i < madre.size(); i++)
        hijo.push_back(madre[i]);
}


void casamiento (vvi padres, vvi &poblacion) {
    for (int i = 0; i < padres.size(); i++)
        for (int j = 0; j < padres.size(); j++)
            if (i != j) {
                vi cromo;
                generaHijo (padres[i], padres[j], cromo);
                poblacion.push_back(cromo);
            }
}



void inversion(vvi &poblacion, vvi padres) {
    for (int i = 0; i < padres.size(); i++) {
        reverse (padres[i].begin(), padres[i].end());
        poblacion.push_back (padres[i]);
    }
}


void complemento(vvi &poblacion, vvi padres){
    for (int i = 0; i < padres.size() * Tcomplemeto; i++){
        for (int j = 0; j < padres[i].size(); j++){
            padres[i][j] = abs(padres[i][j] - (NTurnos-1));
        }
        poblacion.push_back(padres[i]);
    }    
}


void mutacion(vvi padres, vvi &poblacion) {
    int nmuta = round(padres.size() * Tmutacion);
    for (int cont = 0; cont < nmuta; cont ++) {
        for (int i = 0; i < NMedicos; ++i) {
            int pos = Random (0, NDias - 1);
            int pos1 = Random (0, NDias - 1);
            padres[cont][i * NDias + pos] = Random (0, NTurnos - 1);
            padres[cont][i * NDias + pos1] = Random (0, NTurnos - 1);
        }
        poblacion.push_back(padres[cont]);
    }
}

void eliminaRepetidos (vvi &poblacion){
    map <vi, int> m;
    for (int i = 0; i < poblacion.size(); i++) m[poblacion[i]]++;
    poblacion.clear();
    for (auto it : m) poblacion.push_back(it.first);
}

bool compara(vi a, vi b) {
    return calculaFitness(a) > calculaFitness(b);
}


void generarPoblacion (vvi &poblacion) {
    for (int j = 0; j < 15; j++) {
        vi vaux;
        for (int i = 0; i < NDias; i++) 
            vaux.push_back(Random (0, NTurnos - 1));
        poblacion.push_back (vaux);
    }
    sort(poblacion.begin(), poblacion.end(), compara);
    poblacion.erase(poblacion.begin() + NIND, poblacion.end());
}

void muestraMejor (vvi poblacion) {
    cout << endl << "La mejor solucion es: " << calculaFitness(poblacion[0]) << endl;
    for (int i = 0; i < poblacion[0].size(); i++) 
        cout << poblacion[0][i] << "  ";
    cout << endl;
}

void solve () {
    cargarDatos ();
    vvi poblacion;
    generaPoblacionInicial (poblacion);
    muestraPoblacion (poblacion);

    for (int cont = 0; cont < NITERACIONES; cont++) {
        vvi padres;
        seleccion (padres, poblacion);
        casamiento (padres, poblacion);
        inversion (poblacion, padres);
        complemento (poblacion,padres);
        mutacion (poblacion, padres);

        eliminaRepetidos (poblacion);
        generarPoblacion(poblacion);

        muestraPoblacion (poblacion);
        muestraMejor (poblacion);
        cout << cont << endl;
    }
}

int main () {
    //cin.tie(0)->sync_with_stdio(0);
    solve ();
    return 0;
}