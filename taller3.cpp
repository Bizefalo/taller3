#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <unordered_map>

class Transaccion {
public:
    int id;
    std::string cuentaOrigen;
    std::string cuentaDestino;
    double monto;
    std::string ubicacion;
    std::time_t fechaHora;

    Transaccion(int _id, std::string _cuentaOrigen, std::string _cuentaDestino, double _monto, std::string _ubicacion)
        : id(_id), cuentaOrigen(_cuentaOrigen), cuentaDestino(_cuentaDestino), monto(_monto), ubicacion(_ubicacion) {
        fechaHora = std::time(nullptr);
    }

    void mostrarTransaccion() {
        std::cout << "ID: " << id << ", Cuenta Origen: " << cuentaOrigen << ", Cuenta Destino: " << cuentaDestino 
                  << ", Monto: " << monto << ", Ubicacion: " << ubicacion << ", Fecha y Hora: " << std::ctime(&fechaHora);
    }
};


class NodoAVL {
public:
    Transaccion data;
    NodoAVL* izquierda;
    NodoAVL* derecha;
    int altura;

    NodoAVL(Transaccion _data) : data(_data), izquierda(nullptr), derecha(nullptr), altura(1) {}
};

class ArbolAVL {
private:
    NodoAVL* raiz;

    int altura(NodoAVL* N) {
        if (N == nullptr)
            return 0;
        return N->altura;
    }

    NodoAVL* rotacionDerecha(NodoAVL* y) {
        NodoAVL* x = y->izquierda;
        NodoAVL* T2 = x->derecha;

        x->derecha = y;
        y->izquierda = T2;

        y->altura = std::max(altura(y->izquierda), altura(y->derecha)) + 1;
        x->altura = std::max(altura(x->izquierda), altura(x->derecha)) + 1;

        return x;
    }

    NodoAVL* rotacionIzquierda(NodoAVL* x) {
        NodoAVL* y = x->derecha;
        NodoAVL* T2 = y->izquierda;

        y->izquierda = x;
        x->derecha = T2;

        x->altura = std::max(altura(x->izquierda), altura(x->derecha)) + 1;
        y->altura = std::max(altura(y->izquierda), altura(y->derecha)) + 1;

        return y;
    }

    int getBalance(NodoAVL* N) {
        if (N == nullptr)
            return 0;
        return altura(N->izquierda) - altura(N->derecha);
    }

    NodoAVL* insertar(NodoAVL* nodo, Transaccion data) {
        if (nodo == nullptr)
            return (new NodoAVL(data));

        if (data.id < nodo->data.id)
            nodo->izquierda = insertar(nodo->izquierda, data);
        else if (data.id > nodo->data.id)
            nodo->derecha = insertar(nodo->derecha, data);
        else
            return nodo;

        nodo->altura = 1 + std::max(altura(nodo->izquierda), altura(nodo->derecha));

        int balance = getBalance(nodo);

        if (balance > 1 && data.id < nodo->izquierda->data.id)
            return rotacionDerecha(nodo);

        if (balance < -1 && data.id > nodo->derecha->data.id)
            return rotacionIzquierda(nodo);

        if (balance > 1 && data.id > nodo->izquierda->data.id) {
            nodo->izquierda = rotacionIzquierda(nodo->izquierda);
            return rotacionDerecha(nodo);
        }

        if (balance < -1 && data.id < nodo->derecha->data.id) {
            nodo->derecha = rotacionDerecha(nodo->derecha);
            return rotacionIzquierda(nodo);
        }

        return nodo;
    }

    void buscarTransaccion(NodoAVL* nodo, int id) {
        if (nodo == nullptr) {
            std::cout << "Transaccion no encontrada." << std::endl;
            return;
        }

        if (id < nodo->data.id)
            buscarTransaccion(nodo->izquierda, id);
        else if (id > nodo->data.id)
            buscarTransaccion(nodo->derecha, id);
        else
            nodo->data.mostrarTransaccion();
    }

    void obtenerTransacciones(NodoAVL* nodo, std::vector<Transaccion>& transacciones) {
        if (nodo != nullptr) {
            obtenerTransacciones(nodo->izquierda, transacciones);
            transacciones.push_back(nodo->data);
            obtenerTransacciones(nodo->derecha, transacciones);
        }
    }

public:
    ArbolAVL() : raiz(nullptr) {}

    void insertar(Transaccion data) {
        raiz = insertar(raiz, data);
    }

    void buscarTransaccion(int id) {
        buscarTransaccion(raiz, id);
    }

    std::vector<Transaccion> obtenerTransacciones() {
        std::vector<Transaccion> transacciones;
        obtenerTransacciones(raiz, transacciones);
        return transacciones;
    }
};

class SistemaTransacciones {
private:
    ArbolAVL arbolTransacciones;

public:
    void registrarTransaccion(int id, std::string cuentaOrigen, std::string cuentaDestino, double monto, std::string ubicacion) {
        Transaccion nuevaTransaccion(id, cuentaOrigen, cuentaDestino, monto, ubicacion);
        arbolTransacciones.insertar(nuevaTransaccion);
        std::cout << "Transaccion registrada exitosamente." << std::endl;
    }

    void buscarTransaccion(int id) {
        arbolTransacciones.buscarTransaccion(id);
    }

    void detectarTransaccionesSospechosas(double montoLimite, int frecuenciaLimite, int tiempoLimite, std::vector<Transaccion>& transaccionesSospechosas) {
        std::vector<Transaccion> transacciones = arbolTransacciones.obtenerTransacciones();
        std::unordered_map<std::string, int> frecuenciaCuenta;

        for (const auto& transaccion : transacciones) {
            if (transaccion.monto > montoLimite) {
                transaccionesSospechosas.push_back(transaccion);
            }
            frecuenciaCuenta[transaccion.cuentaOrigen]++;
            if (frecuenciaCuenta[transaccion.cuentaOrigen] > frecuenciaLimite) {
                transaccionesSospechosas.push_back(transaccion);
            }
        }

        for (size_t i = 0; i < transacciones.size(); ++i) {
            for (size_t j = i + 1; j < transacciones.size(); ++j) {
                if (transacciones[i].cuentaOrigen == transacciones[j].cuentaOrigen && 
                    transacciones[i].ubicacion != transacciones[j].ubicacion && 
                    std::difftime(transacciones[j].fechaHora, transacciones[i].fechaHora) < tiempoLimite) {
                    transaccionesSospechosas.push_back(transacciones[j]);
                }
            }
        }
    }

    void generarReporte() {
        std::vector<Transaccion> transaccionesSospechosas;
        detectarTransaccionesSospechosas(1000.0, 5, 3600, transaccionesSospechosas);

        std::ofstream archivo("reporte_sospechosas.txt");
        if (archivo.is_open()) {
            archivo << "Reporte de Transacciones Sospechosas:\n\n";
            for (const auto& transaccion : transaccionesSospechosas) {
                archivo << "ID: " << transaccion.id << ", Cuenta Origen: " << transaccion.cuentaOrigen 
                        << ", Cuenta Destino: " << transaccion.cuentaDestino << ", Monto: " << transaccion.monto 
                        << ", Ubicacion: " << transaccion.ubicacion << ", Fecha y Hora: " << std::ctime(&transaccion.fechaHora);
            }
            archivo.close();
            std::cout << "Reporte generado exitosamente." << std::endl;
        } else {
            std::cout << "No se pudo abrir el archivo para escribir el reporte." << std::endl;
        }
    }
};

int main() {
    SistemaTransacciones sistema;

    sistema.registrarTransaccion(1, "A123", "B456", 1000.0, "New York");
    sistema.registrarTransaccion(2, "C789", "D012", 2000.0, "Los Angeles");
    sistema.registrarTransaccion(3, "E345", "F678", 500.0, "Chicago");
    sistema.registrarTransaccion(4, "A123", "G910", 3000.0, "New York");
    sistema.registrarTransaccion(5, "A123", "H112", 400.0, "New York");

   
    std::cout << "Buscando transaccion con ID 2:" << std::endl;
    sistema.buscarTransaccion(2
