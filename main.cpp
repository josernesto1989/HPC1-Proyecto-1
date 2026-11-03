#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <pthread.h>
#include <unistd.h>


const int ROWS = 10000;
const int COLS = 10000;
const long long TOTAL_ELEMENTS = (long long)ROWS * COLS;
const char * filename = "./matriz/numeros.txt";
const char * output_filename = "./matriz/matriz_ordenada.txt";
using namespace std;

// Funcion para ordenar un trozo de datos por un hilo
void ordenarTozos(vector<int>& data, size_t start, size_t end) {
    sort(data.begin() + start, data.begin() + end);
}

struct ParametrosOrdenamiento {
    vector<int>* data;
    size_t start;
    size_t end;
};

void* ordenarTozosPthread(void* argumentos) {
    ParametrosOrdenamiento* parametros =
        static_cast<ParametrosOrdenamiento*>(argumentos);
    ordenarTozos(*parametros->data, parametros->start, parametros->end);
    return NULL;
}

// Function to merge sorted trozos sequentially
void mezclarTozos(vector<int>& data, size_t cantidad_threads, size_t trozo_size) {
    for (size_t step = 1; step < cantidad_threads; step *= 2) {
        for (size_t i = 0; i < cantidad_threads - step; i += 2 * step) {
            size_t left = i * trozo_size;
            size_t mid = min((i + step) * trozo_size, (size_t)TOTAL_ELEMENTS);
            size_t right = min((i + 2 * step) * trozo_size, (size_t)TOTAL_ELEMENTS);
            
            if (mid < right) {
                inplace_merge(data.begin() + left, data.begin() + mid, data.begin() + right);
            }
        }
    }
}

int main() {
    // 1. Optimizar operaciones entrada salida
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    vector<int> matrixData;
    matrixData.reserve(TOTAL_ELEMENTS);

    // Leyendo el archivo de entrada
    cout << "Leyendo..." << endl;
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error abriendo el archivo " << filename << "!" << endl;
        return 1;
    }

    int value;
    while (inFile >> value) {
        matrixData.push_back(value);
    }
    inFile.close();

    if (matrixData.size() != TOTAL_ELEMENTS) {
        cerr << "La matriz no tiene el tamaño correcto 10000x10000 . Tiene "<<
        matrixData.size() << " elementos." << endl;
    }

    // Manejando los hilos
    long procesadores_disponibles = sysconf(_SC_NPROCESSORS_ONLN);
    size_t cantidad_threads = procesadores_disponibles > 0
        ? static_cast<size_t>(procesadores_disponibles)
        : 8; // Fallback
    
    cout << "Ordenando con " << cantidad_threads << " hilos..." << endl;
    
    vector<pthread_t> threads(cantidad_threads);
    vector<ParametrosOrdenamiento> parametros(cantidad_threads);
    size_t trozo_size = TOTAL_ELEMENTS / cantidad_threads;

    auto start_time = chrono::high_resolution_clock::now();

    // Mandando a cada hilo a ordenar su trozo
    for (size_t i = 0; i < cantidad_threads; ++i) {
        size_t start = i * trozo_size;
        size_t end = (i == cantidad_threads - 1) ? TOTAL_ELEMENTS : start + trozo_size;
        parametros[i] = {&matrixData, start, end};

        int resultado = pthread_create(
            &threads[i], NULL, ordenarTozosPthread, &parametros[i]);
        if (resultado != 0) {
            cerr << "Error creando el hilo " << i << ": " << resultado << endl;
            for (size_t j = 0; j < i; ++j) {
                pthread_join(threads[j], NULL);
            }
            return 1;
        }
    }

    // Unir hilos
    for (size_t i = 0; i < cantidad_threads; ++i) {
        int resultado = pthread_join(threads[i], NULL);
        if (resultado != 0) {
            cerr << "Error uniendo el hilo " << i << ": " << resultado << endl;
            return 1;
        }
    }

    // Mezclar los trozos ordenados
    cout << "Mezclando trozos..." << endl;
    mezclarTozos(matrixData, cantidad_threads, trozo_size);

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end_time - start_time;
    cout << "Tiempo de ordenamiento: " << diff.count() << " segundos" << endl;

    // Escribir resultado en el archivo de salida
    cout << "Escribiendo en el archivo de salida..." << endl;
    ofstream outFile(output_filename);
    for (size_t i = 0; i < TOTAL_ELEMENTS; ++i) {
        outFile << matrixData[i] << " ";
        // Adicionar un salto de línea después de cada fila
        if ((i + 1) % COLS == 0) {
            outFile << "\n";
        }
    }
    outFile.close();

    cout << "Done." << endl;
    return 0;
}