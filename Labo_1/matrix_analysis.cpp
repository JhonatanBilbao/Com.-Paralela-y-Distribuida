#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace std::chrono;

class MatrixMultiplier {
private:
    int size;
    vector<vector<double>> A, B, C;
    vector<double> x, y;

public:
    MatrixMultiplier(int n) : size(n) {
        A.resize(n, vector<double>(n));
        B.resize(n, vector<double>(n));
        C.resize(n, vector<double>(n));
        x.resize(n);
        y.resize(n);
        initializeMatrices();
    }

    void initializeMatrices() {
        srand(42); 
        
       
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                A[i][j] = (double)rand() / RAND_MAX;
                B[i][j] = (double)rand() / RAND_MAX;
            }
            x[i] = (double)rand() / RAND_MAX;
        }
    }

    void resetResult() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                C[i][j] = 0.0;
            }
            y[i] = 0.0;
        }
    }

 
    double firstPairLoops() {
        resetResult();
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                y[i] += A[i][j] * x[j];
            }
        }
        
        auto end = high_resolution_clock::now();
        return duration_cast<microseconds>(end - start).count() / 1000.0;
    }


    double secondPairLoops() {
        resetResult();
        auto start = high_resolution_clock::now();
        
        for (int j = 0; j < size; j++) {
            for (int i = 0; i < size; i++) {
                y[i] += A[i][j] * x[j];
            }
        }
        
        auto end = high_resolution_clock::now();
        return duration_cast<microseconds>(end - start).count() / 1000.0;
    }


    double classicMatrixMultiplication() {
        resetResult();
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < size; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        
        auto end = high_resolution_clock::now();
        return duration_cast<microseconds>(end - start).count() / 1000.0;
    }


    double blockedMatrixMultiplication(int blockSize = 64) {
        resetResult();
        auto start = high_resolution_clock::now();
        
        for (int ii = 0; ii < size; ii += blockSize) {
            for (int jj = 0; jj < size; jj += blockSize) {
                for (int kk = 0; kk < size; kk += blockSize) {
                    
                    int imax = min(ii + blockSize, size);
                    int jmax = min(jj + blockSize, size);
                    int kmax = min(kk + blockSize, size);
                    
                    for (int i = ii; i < imax; i++) {
                        for (int j = jj; j < jmax; j++) {
                            for (int k = kk; k < kmax; k++) {
                                C[i][j] += A[i][k] * B[k][j];
                            }
                        }
                    }
                }
            }
        }
        
        auto end = high_resolution_clock::now();
        return duration_cast<microseconds>(end - start).count() / 1000.0;
    }

    void printAnalysis(int testSize) {
        cout << "\n" << string(60, '=') << endl;
        cout << "ANALISIS DE RENDIMIENTO - Tamano: " << testSize << "x" << testSize << endl;
        cout << string(60, '=') << endl;
        
      
        double time1 = firstPairLoops();
        double time2 = secondPairLoops();
        double timeClassic = classicMatrixMultiplication();
        double timeBlocked = blockedMatrixMultiplication();
        
        cout << fixed << setprecision(3);
        cout << "\nTiempos de ejecucion (ms):" << endl;
        cout << "Primer par de bucles (por filas):   " << setw(8) << time1 << " ms" << endl;
        cout << "Segundo par de bucles (por cols):   " << setw(8) << time2 << " ms" << endl;
        cout << "Multiplicación clasica (3 bucles):  " << setw(8) << timeClassic << " ms" << endl;
        cout << "Multiplicación por bloques (6):     " << setw(8) << timeBlocked << " ms" << endl;
        
      
        cout << "\nAnalisis de eficiencia:" << endl;
        cout << "Diferencia bucles (columnas/filas): " << setw(6) << setprecision(2) 
             << (time2/time1) << "x" << endl;
        cout << "Mejora bloques vs clasica:          " << setw(6) << setprecision(2) 
             << (timeClassic/timeBlocked) << "x" << endl;
        
        
        long long operations = (long long)testSize * testSize * testSize;
        cout << "\nComplejidad algoritmica:" << endl;
        cout << "Operaciones teoricas: " << operations << " (O(n³))" << endl;
        cout << "FLOPS clasica: " << setw(8) << setprecision(1) 
             << (operations / (timeClassic * 1000)) << " MFLOPS" << endl;
        cout << "FLOPS bloques: " << setw(8) << setprecision(1) 
             << (operations / (timeBlocked * 1000)) << " MFLOPS" << endl;
    }

    void analyzeMemoryAccess() {
        cout << "\n" << string(60, '=') << endl;
        cout << "ANALISIS DE ACCESO A MEMORIA" << endl;
        cout << string(60, '=') << endl;
        
        cout << "\n1. PRIMER PAR DE BUCLEs (por filas):" << endl;
        cout << "   • Patrón: A[i][j] acceso secuencial por filas" << endl;
        cout << "   • Cache: Excelente localidad espacial" << endl;
        cout << "   • Misses: Mínimos, aprovecha lineas de cache" << endl;
        
        cout << "\n2. SEGUNDo PAR DE BUCLES (por columnas):" << endl;
        cout << "   • Patrón: A[i][j] acceso por columnas" << endl;
        cout << "   • Cache: Pobre localidad espacial" << endl;
        cout << "   • Misses: Muchos, cada acceso puede ser un miss" << endl;
        
        cout << "\n3. MULTIPLICACIoN CLASICa:" << endl;
        cout << "   • A[i][k]: Acceso por filas (buena localidad)" << endl;
        cout << "   • B[k][j]: Acceso por columnas (mala localidad)" << endl;
        cout << "   • C[i][j]: Reutilizacion en bucle interno" << endl;
        
        cout << "\n4. MULTIPLICACION POR BLOQUES:" << endl;
        cout << "   • Divide matrices en bloques que caben en cache" << endl;
        cout << "   • Mejora localidad temporal y espacial" << endl;
        cout << "   • Reduce significativamente cache misses" << endl;
    }
};

void printHeader() {
    cout << string(80, '=') << endl;
    cout << "    COMPARACION DE ALGORITMOS DE MULTIPLICACION DE MATRICES" << endl;
    cout << "                    Análisis de Rendimiento y Cache" << endl;
    cout << string(80, '=') << endl;
}

void printValgrindInstructions() {
    cout << "\n" << string(60, '=') << endl;
    cout << "INSTRUCCIONES PARA VALGRIND/KCACHEGRIND" << endl;
    cout << string(60, '=') << endl;
    
    cout << "\nPara analisis detallado con valgrind:" << endl;
    cout << "\n1. Compilar con informacion de debug:" << endl;
    cout << "   g++ -g -O2 programa.cpp -o matriz" << endl;
    
    cout << "\n2. Ejecutar con cachegrind:" << endl;
    cout << "   valgrind --tool=cachegrind ./matriz" << endl;
    
    cout << "\n3. Analizar resultados:" << endl;
    cout << "   cg_annotate cachegrind.out.XXXXX" << endl;
    
    cout << "\n4. Interfaz grfica (si disponible):" << endl;
    cout << "   kcachegrind cachegrind.out.XXXXX" << endl;
    
    cout << "\nMétricas importantes a observar:" << endl;
    cout << "I1 misses: Fallos en cache de instrucciones L1" << endl;
    cout << "D1 misses: Fallos en cache de datos L1" << endl;
    cout << "LL misses: Fallos en ultimo nivel de cache" << endl;
    cout << "Total refs: Referencias totales a memoria" << endl;
}

int main(int argc, char* argv[]) {
    printHeader();
    

    vector<int> sizes = {128, 256, 512, 1024};
    
    
    if (argc > 1) {
        int customSize = atoi(argv[1]);
        if (customSize > 0 && customSize <= 2048) {
            sizes = {customSize};
        }
    }
    
    cout << "\nEjecutando pruebas con tamanos: ";
    for (int size : sizes) {
        cout << size << " ";
    }
    cout << "\n";
    
   
    for (int size : sizes) {
        MatrixMultiplier multiplier(size);
        multiplier.printAnalysis(size);
        
        if (size <= 512) { 
            multiplier.analyzeMemoryAccess();
        }
    }
    
    printValgrindInstructions();
    
    cout << "\n" << string(80, '=') << endl;
    cout << "Analisis terminado. Para mas informacion, utilice kcachegrind/valgrind." << endl;
    cout << string(80, '=') << endl;
    
    return 0;
}

