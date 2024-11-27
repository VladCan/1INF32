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
#define NIND 15
#define Tseleccion 0.3
#define Tcomplemeto 0.4
#define Pcasamiento 0.5
#define NITERACIONES 5000
#define Tmutacion 0.8
#define NDias 7
#define NTurnos 4
#define NMedicos 4
#define HorarioPerfecto 4
#define PenalizacionNocheManana 2
using namespace std;

/*
TURNOS = 3;  // Ejemplo: 0 - no viene, 1 - mañana, 2 - tarde, 3 - noche 
DIAS = 7;    // Ejemplo: 0 - lunes, 1 - martes, ..., 6 - domingo
MEDICOS = 5; // Número de médicos
 
 */

map<int, vector<int>> Gpreferencias;

void generaHorario(vector <int> &vaux, int n) {

    for (int i = 0; i < n; i++) {
        vaux.push_back(rand() % NTurnos);
    }
}

//, vector<vector<int>> Vrequisitos

int calculafitness(vector<int> cromo, map<int, vector<int>> preferencias) {
    int fitness = 0;
    int idMedico = 0;
    for (int i = NDias; i < cromo.size(); i += NDias) {
        const auto& horario = vector<int>(cromo.begin() + i + 1, cromo.begin() + i + 8); // Horario del médico

        // Verificar si el médico tiene preferencias definidas
        if (preferencias.find(idMedico) != preferencias.end()) {
            const auto& pref = preferencias.at(idMedico);

            // Evaluar el horario comparándolo con las preferencias
            for (int dia = 0; dia < horario.size(); ++dia) {
                // Si el turno asignado coincide con la preferencia, sumar fitness
                if (horario[dia] == pref[dia]) {
                    fitness += HorarioPerfecto; // Coincidencia exacta     define
                }
            }
        }
        idMedico++;
    }



    // Penalización por turnos consecutivos de noche y mañana

    for (int dia = 0; dia < cromo.size(); dia++) {
        if ((dia + 1) % NDias != 0 and dia != 0) {
            if (cromo[dia] == 3 && cromo[dia + 1] == 1) {
                fitness -= PenalizacionNocheManana; // Penalización más alta   factor de k;
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
            generaHorario(vaux, NDias);
        }
        poblacion.push_back(vaux);

        cont++;
    }

}

void muestrapoblacion(vector<vector<int>>poblacion, map<int, vector<int>> preferencias) {

    for (int i = 0; i < poblacion.size(); i++) {
        for (int j = 0; j < poblacion[i].size(); j++) {
            ;
            if (j % NDias == 0) cout << "Medico  ->  ";
            cout << poblacion[i][j] << "  ";
        }
        cout << "fit: " << calculafitness(poblacion[i], preferencias);
        cout << endl;
    }
}

void calculasupervivencia(vector<vector<int>>poblacion, vector<int>&supervivencia, map<int, vector<int>> preferencias) {
    int suma = 0;
    for (int i = 0; i < poblacion.size(); i++)
        suma += calculafitness(poblacion[i], preferencias);
    for (int i = 0; i < poblacion.size(); i++) {
        int fit = round(100 * (double) calculafitness(poblacion[i], preferencias) / suma);
        supervivencia.push_back(fit);
    }
}

void cargaruleta(vector<int>supervivencia, int *ruleta) {
    int ind = 0;
    for (int i = 0; i < supervivencia.size(); i++)
        for (int j = 0; j < supervivencia[i]; j++)
            ruleta[ind++] = i;
}

void seleccion(vector<vector<int>> &padres, vector<vector<int>>poblacion, map<int, vector<int>> preferencias) {


    int ruleta[100]{-1};
    vector<int>supervivencia;
    calculasupervivencia(poblacion, supervivencia, preferencias);
    cargaruleta(supervivencia, ruleta);
    int nseleccionados = poblacion.size() * Tseleccion;
    for (int i = 0; i < nseleccionados; i++) {
        int ind = rand() % 100;
        if (ruleta[ind]>-1)
            padres.push_back(poblacion[ruleta[ind]]);

    }
}

void generahijo(vector<int>padre, vector<int>madre,
        vector<int>&hijo) {
    int pos = round(padre.size() * Pcasamiento);

    for (int i = 0; i < pos; i++)
        hijo.push_back(padre[i]);
    for (int i = pos; i < madre.size(); i++)
        hijo.push_back(madre[i]);
}

void casamiento(vector<vector<int>>padres, vector<vector<int>>&poblacion, map<int, vector<int>> preferencias) {
    for (int i = 0; i < padres.size(); i++)
        for (int j = 0; j < padres.size(); j++)
            if (i != j) {
                vector<int>cromo;
                generahijo(padres[i], padres[j], cromo);
                poblacion.push_back(cromo);
            }
}

void inversion(vector<vector<int>>&poblacion, vector<vector<int>>padres) {
    //
    vector<int> cromo;
    for (int i = 0; i < padres.size(); i++) {
        vector<int> cromo = padres[i];
        reverse(cromo.begin(), cromo.end());
        poblacion.push_back(cromo);
        cromo.clear();
    }

}

void complemento(vector<vector<int>>&poblacion,
        vector<vector<int>>padres){
    
    
    
    for(int i=0;i<padres.size()*Tcomplemeto;i++){
        for(int j=0;j<padres[i].size();j++){
                padres[i][j]=abs(padres[i][j]-3);
        }
        poblacion.push_back(padres[i]);
    }
    
}

void mutacion(vector<vector<int>>padres, vector<vector<int>>&poblacion, map<int, vector<int>> preferencias) {
    int cont = 0;
    int nmuta = round(padres.size() * Tmutacion);
    while (cont < nmuta) {
        for (int i = 0; i < NMedicos; ++i) {
                int pos = rand() % NDias;
                int pos1 = rand() % NDias;
                //int pos2 = rand() % NDias;
                padres[cont][i * NDias + pos] = rand() % NTurnos;
                padres[cont][i * NDias + pos1] = rand() % NTurnos;
                //padres[cont][i * NDias + pos2] = rand() % NTurnos;
        }

        poblacion.push_back(padres[cont]);
        cont++;
    }
}

bool compara(vector<int>a, vector<int>b) {


    //cout<<"Compara"<<endl;

    int suma = 0, sumb = 0;
    
    suma+=(calculafitness(a,Gpreferencias)*a.size());
            
    sumb += (calculafitness(b,Gpreferencias)*b.size());
     
/*
    for (int i = 0; i < a.size(); i++)
        suma += calculafitness(a, Gpreferencias);
    for (int i = 0; i < b.size(); i++)
        sumb += calculafitness(b, Gpreferencias);

*/
    return suma>sumb;
}

void generarpoblacion(vector<vector<int>> &poblacion, map<int, vector<int>> preferencias) {

    //cout<<"Antes del Sort"<<endl;
    sort(poblacion.begin(), poblacion.end(), compara);
    //cout<<"Despues del Sort"<<endl;
    poblacion.erase(poblacion.begin() + NIND, poblacion.end());

}

int muestramejor(vector<vector<int>> poblacion, map<int, vector<int>> preferencias) {
    int mejor = 0;
    for (int i = 0; i < poblacion.size(); i++)
        if (calculafitness(poblacion[mejor], preferencias) < calculafitness(poblacion[i], preferencias))
            mejor = i;

    cout << endl << "La mejor solucion es:" << calculafitness(poblacion[mejor], preferencias) << endl;
    for (int i = 0; i < poblacion[mejor].size(); i++) {
        cout << poblacion[mejor][i] << "  ";
    }
    cout << endl;
    return calculafitness(poblacion[mejor], preferencias);
}

int main(int argc, char** argv) {
    int cont = 0;
    srand(time(0));
    vector<vector<int>> poblacion;
    map<int, vector<int>> preferencias;
    // Médico 0: No trabajar lunes, martes, miércoles; jueves mañana, viernes tarde, sábado noche, domingo tarde
    preferencias[0] = {0, 3, 0, 1, 2, 3, 2};
    // Médico 1: Prefiere trabajar lunes y martes por la mañana, miércoles tarde, jueves tarde, viernes noche, no sábado ni domingo
    preferencias[1] = {1, 1, 2, 2, 3, 0, 0};
    // Médico 2: Prefiere no trabajar lunes, martes noche, miércoles mañana, jueves no trabaja, viernes tarde, sábado noche, domingo mañana
    preferencias[2] = {2, 3, 2, 0, 2, 3, 1};
    // Médico 3: Prefiere no trabajar lunes, martes y miércoles; jueves noche, viernes mañana, sábado tarde, domingo noche
    preferencias[3] = {3, 0, 1, 3, 3, 2, 3};
    generapoblacioninicial(poblacion);
    muestrapoblacion(poblacion, preferencias);
    Gpreferencias = preferencias;

    while (true) {
        vector<vector<int>> padres;
        seleccion(padres, poblacion, preferencias);
        casamiento(padres, poblacion, preferencias);
        cout << endl;
        //cout<<"Mutacion"<<cont<<endl;
        inversion(poblacion, padres);
        complemento(poblacion,padres);
        mutacion(poblacion, padres, preferencias);
        
        //invercion
        //cout<<"GeneraPoblacion"<<cont<<endl;
        generarpoblacion(poblacion, preferencias);
        //muestrapoblacion(poblacion,preferencias);

        //cout<<"Merjor"<<cont<<endl;
        muestramejor(poblacion, preferencias);
        cout<<cont++<<endl;
        if (cont == NITERACIONES) break;
    }

    return 0;
}

