#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <new>
#include <pthread.h>
#include <unistd.h>

const int ROWS = 20000;
const int COLS = 20000;
const long long TOTAL_ELEMENTS = (long long)ROWS * COLS;

const char *filename = "matriz_a.bin";
const char *output_filename = "matriz_ordenada.bin";

using namespace std;
using Valor = float;  // precision simple

void printProgress(const char* element, size_t cantidad)
{
    int porcentaje = (int)((cantidad * 100ULL) / (unsigned long long)TOTAL_ELEMENTS);
    if (porcentaje > 100) porcentaje = 100;
    cout << "\r" << element << "(" << porcentaje << "%)[";
    for (int i = 0; i < 50; ++i)
        cout << (i < (porcentaje / 2) ? "X" : "-");
    cout << "]" << flush;
}

void ordenarTozos(vector<Valor> &data, size_t start, size_t end)
{
    sort(data.begin() + start, data.begin() + end);
}

struct ParametrosOrdenamiento
{
    vector<Valor> *data;
    size_t start;
    size_t end;
};

void *ordenarTozosPthread(void *argumentos)
{
    ParametrosOrdenamiento *parametros =
        static_cast<ParametrosOrdenamiento *>(argumentos);
    ordenarTozos(*parametros->data, parametros->start, parametros->end);
    return NULL;
}

void mezclarTozos(vector<Valor> &data, size_t cantidad_threads, size_t trozo_size)
{
    for (size_t step = 1; step < cantidad_threads; step *= 2)
    {
        for (size_t i = 0; i < cantidad_threads - step; i += 2 * step)
        {
            size_t left = i * trozo_size;
            size_t mid = min((i + step) * trozo_size, (size_t)TOTAL_ELEMENTS);
            size_t right = min((i + 2 * step) * trozo_size, (size_t)TOTAL_ELEMENTS);
            if (mid < right)
                inplace_merge(data.begin() + left, data.begin() + mid,
                              data.begin() + right);
        }
    }
}

int main()
{
    const streamsize bytes_needed =
        (streamsize)TOTAL_ELEMENTS * (streamsize)sizeof(Valor);

    cout << "Elementos: " << TOTAL_ELEMENTS
         << " | bytes por elemento: " << sizeof(Valor)
         << " | archivo: " << (bytes_needed / (1024.0*1024.0*1024.0)) << " GB"
         << endl;

    //Lectura binaria
    cout << "Leyendo " << filename << "..." << endl;
    ifstream inFile(filename, ios::binary);
    if (!inFile)
    {
        cerr << "Error abriendo el archivo " << filename << "!" << endl;
        return 1;
    }

    vector<Valor> matrixData;
    try
    {
        matrixData.resize((size_t)TOTAL_ELEMENTS);
    }
    catch (const bad_alloc &)
    {
        cerr << "No hay memoria suficiente para reservar "
             << TOTAL_ELEMENTS << " elementos." << endl;
        return 1;
    }

    inFile.read(reinterpret_cast<char*>(matrixData.data()), bytes_needed);
    streamsize leidos = inFile.gcount();
    inFile.close();

    if (leidos != bytes_needed)
    {
        cerr << "Lectura incompleta: " << leidos << " de "
             << bytes_needed << " bytes." << endl;
        return 1;
    }

    //Ordenamiento paralelo por bloques
    long procesadores = sysconf(_SC_NPROCESSORS_ONLN);
    size_t cantidad_threads = procesadores > 0 ? (size_t)procesadores : 8;
    cout << "Ordenando con " << cantidad_threads << " hilos..." << endl;

    auto start_time = chrono::high_resolution_clock::now();

    vector<pthread_t> threads(cantidad_threads);
    vector<ParametrosOrdenamiento> parametros(cantidad_threads);
    size_t trozo_size = TOTAL_ELEMENTS / cantidad_threads;
    if (trozo_size == 0) trozo_size = 1;

    for (size_t i = 0; i < cantidad_threads; ++i)
    {
        size_t start = i * trozo_size;
        size_t end = (i == cantidad_threads - 1)
                         ? (size_t)TOTAL_ELEMENTS
                         : start + trozo_size;
        if (start > (size_t)TOTAL_ELEMENTS) start = (size_t)TOTAL_ELEMENTS;
        if (end > (size_t)TOTAL_ELEMENTS) end = (size_t)TOTAL_ELEMENTS;
        parametros[i] = {&matrixData, start, end};

        int rc = pthread_create(&threads[i], NULL, ordenarTozosPthread,
                                &parametros[i]);
        if (rc != 0)
        {
            cerr << "Error creando el hilo " << i << ": " << rc << endl;
            for (size_t j = 0; j < i; ++j)
                pthread_join(threads[j], NULL);
            return 1;
        }
    }

    bool join_error = false;
    for (size_t i = 0; i < cantidad_threads; ++i)
    {
        int rc = pthread_join(threads[i], NULL);
        if (rc != 0)
        {
            cerr << "Error uniendo el hilo " << i << ": " << rc << endl;
            join_error = true;
        }
    }
    if (join_error) return 1;

    //Mezcla
    cout << "Mezclando trozos..." << endl;
    mezclarTozos(matrixData, cantidad_threads, trozo_size);

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end_time - start_time;
    cout << "Tiempo de ordenamiento: " << diff.count() << " segundos" << endl;

    //Escritura binaria
    cout << "Escribiendo en " << output_filename << "..." << endl;
    ofstream outFile(output_filename, ios::binary);
    if (!outFile)
    {
        cerr << "Error abriendo el archivo de salida "
             << output_filename << "!" << endl;
        return 1;
    }

    outFile.write(reinterpret_cast<const char*>(matrixData.data()), bytes_needed);
    if (!outFile)
    {
        cerr << "Error escribiendo en " << output_filename << endl;
        outFile.close();
        return 1;
    }
    outFile.close();

    cout << "Done." << endl;
    return 0;
}