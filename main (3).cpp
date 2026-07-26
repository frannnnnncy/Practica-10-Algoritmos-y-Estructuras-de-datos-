// ============================================================
// Practica N.10 - Actividades 5 y 6 (C++17)
// Framework de verificacion (assert + generacion pseudoaleatoria)
// y fuzzing de 500 secuencias x 50 operaciones = 25000 verificaciones
// Universidad Nacional del Altiplano - Algoritmos y Estructuras de Datos
// Autora: Francy Jimena Ramos Vilca
// ============================================================
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>

// ------------------------------------------------------------
// MODELO ACADEMICO
// ------------------------------------------------------------
enum class EstadoAcademico { ACTIVO, RETIRADO };

struct Estudiante {
    int codigo;
    std::string nombres;
    std::string apellidos;
    double promedio;
    int creditos;
    EstadoAcademico estado;
    std::string periodo;
};

// ------------------------------------------------------------
// BST (ArbolAcademico)
// ------------------------------------------------------------
struct NodoBST {
    Estudiante dato;
    std::unique_ptr<NodoBST> izquierdo;
    std::unique_ptr<NodoBST> derecho;
    explicit NodoBST(Estudiante d) : dato(std::move(d)), izquierdo(nullptr), derecho(nullptr) {}
};

class ArbolAcademico {
public:
    std::unique_ptr<NodoBST> raiz;

    void insertar(const Estudiante& e) {
        raiz = insertarRec(std::move(raiz), e);
    }

    void eliminar(int codigo) {
        raiz = eliminarRec(std::move(raiz), codigo);
    }

    std::vector<Estudiante> inOrder() const {
        std::vector<Estudiante> resultado;
        inOrderRec(raiz.get(), resultado);
        return resultado;
    }

    int altura() const {
        return alturaRec(raiz.get());
    }

private:
    static std::unique_ptr<NodoBST> insertarRec(std::unique_ptr<NodoBST> nodo, const Estudiante& e) {
        if (!nodo) return std::make_unique<NodoBST>(e);
        if (e.codigo < nodo->dato.codigo)
            nodo->izquierdo = insertarRec(std::move(nodo->izquierdo), e);
        else if (e.codigo > nodo->dato.codigo)
            nodo->derecho = insertarRec(std::move(nodo->derecho), e);
        return nodo;
    }

    static NodoBST* minimo(NodoBST* nodo) {
        while (nodo->izquierdo) nodo = nodo->izquierdo.get();
        return nodo;
    }

    static std::unique_ptr<NodoBST> eliminarRec(std::unique_ptr<NodoBST> nodo, int codigo) {
        if (!nodo) return nullptr;
        if (codigo < nodo->dato.codigo) {
            nodo->izquierdo = eliminarRec(std::move(nodo->izquierdo), codigo);
        } else if (codigo > nodo->dato.codigo) {
            nodo->derecho = eliminarRec(std::move(nodo->derecho), codigo);
        } else {
            if (!nodo->izquierdo) return std::move(nodo->derecho);
            if (!nodo->derecho) return std::move(nodo->izquierdo);
            NodoBST* sucesor = minimo(nodo->derecho.get());
            nodo->dato = sucesor->dato;
            nodo->derecho = eliminarRec(std::move(nodo->derecho), sucesor->dato.codigo);
        }
        return nodo;
    }

    static void inOrderRec(const NodoBST* nodo, std::vector<Estudiante>& out) {
        if (!nodo) return;
        inOrderRec(nodo->izquierdo.get(), out);
        out.push_back(nodo->dato);
        inOrderRec(nodo->derecho.get(), out);
    }

    static int alturaRec(const NodoBST* nodo) {
        if (!nodo) return -1;
        return 1 + std::max(alturaRec(nodo->izquierdo.get()), alturaRec(nodo->derecho.get()));
    }
};

// ------------------------------------------------------------
// ACTIVIDAD 5: VERIFICADORES
// ------------------------------------------------------------
bool esBST(const NodoBST* n, long long mn = INT_MIN, long long mx = INT_MAX) {
    if (!n) return true;
    if (!(mn < n->dato.codigo && n->dato.codigo < mx)) return false;
    return esBST(n->izquierdo.get(), mn, n->dato.codigo) &&
           esBST(n->derecho.get(), n->dato.codigo, mx);
}

bool esInOrderOrdenado(const ArbolAcademico& a) {
    auto v = a.inOrder();
    return std::is_sorted(v.begin(), v.end(),
        [](const Estudiante& x, const Estudiante& y) { return x.codigo < y.codigo; });
}

int contarRecursivo(const NodoBST* n) {
    if (!n) return 0;
    return 1 + contarRecursivo(n->izquierdo.get()) + contarRecursivo(n->derecho.get());
}

bool cotaAlturaOK(int altura, int n) {
    if (n == 0) return true;
    int cota = static_cast<int>(std::ceil(std::log2(n + 1))) - 1;
    return altura >= cota;
}

struct ResultadoVerificacion {
    bool p1_inorder, p2_altura, p3_conteo, p4_bst;
    bool todasOK() const { return p1_inorder && p2_altura && p3_conteo && p4_bst; }
};

ResultadoVerificacion verificarPropiedades(const ArbolAcademico& a, int nEsperado) {
    ResultadoVerificacion r;
    r.p1_inorder = esInOrderOrdenado(a);
    r.p2_altura  = cotaAlturaOK(a.altura(), nEsperado);
    r.p3_conteo  = (contarRecursivo(a.raiz.get()) == nEsperado);
    r.p4_bst     = esBST(a.raiz.get());
    return r;
}

// ------------------------------------------------------------
// ACTIVIDAD 6: FUZZING (500 x 50 = 25000 verificaciones)
// ------------------------------------------------------------
struct ResultadoFuzz {
    int caso, op;
    std::string accion;
    int codigo;
};

std::vector<ResultadoFuzz> fuzzTest(int nCasos = 500, int nOps = 50, unsigned semilla = 42) {
    std::mt19937 rng(semilla);
    std::vector<ResultadoFuzz> fallos;
    long long fallosP1 = 0, fallosP2 = 0, fallosP3 = 0, fallosP4 = 0;

    for (int caso = 0; caso < nCasos; ++caso) {
        ArbolAcademico arbol;
        std::vector<int> activos;

        std::vector<int> pool(nOps * 3);
        for (int i = 0; i < nOps * 3; ++i) pool[i] = 20000000 + caso * 1000 + i;
        std::shuffle(pool.begin(), pool.end(), rng);
        int poolIdx = 0;

        std::uniform_int_distribution<int> dist3(0, 2); // 0,1 = insertar ; 2 = eliminar

        for (int op = 0; op < nOps; ++op) {
            bool insertarAccion = (dist3(rng) != 2) || activos.empty();
            int codigo;
            if (insertarAccion) {
                if (poolIdx >= (int)pool.size()) break;
                codigo = pool[poolIdx++];
                Estudiante e{codigo, "T", "T", 15.0, 100, EstadoAcademico::ACTIVO, "2024-I"};
                arbol.insertar(e);
                activos.push_back(codigo);
            } else {
                std::uniform_int_distribution<size_t> distIdx(0, activos.size() - 1);
                size_t idx = distIdx(rng);
                codigo = activos[idx];
                arbol.eliminar(codigo);
                activos.erase(activos.begin() + idx);
            }

            ResultadoVerificacion res = verificarPropiedades(arbol, (int)activos.size());
            if (!res.p1_inorder) fallosP1++;
            if (!res.p2_altura)  fallosP2++;
            if (!res.p3_conteo)  fallosP3++;
            if (!res.p4_bst)     fallosP4++;

            assert(res.p1_inorder && "P1 violada: inorder no ordenado");
            assert(res.p3_conteo  && "P3 violada: conteo incorrecto");
            assert(res.p4_bst     && "P4 violada: invariante BST rota");

            if (!res.todasOK()) {
                fallos.push_back({caso, op, insertarAccion ? "insertar" : "eliminar", codigo});
            }
        }
    }

    std::cout << "Verificaciones ejecutadas: " << (nCasos * nOps) << "\n";
    std::cout << "Fallos P1 (in-order): " << fallosP1 << "\n";
    std::cout << "Fallos P2 (altura): "    << fallosP2 << "\n";
    std::cout << "Fallos P3 (conteo): "    << fallosP3 << "\n";
    std::cout << "Fallos P4 (BST): "       << fallosP4 << "\n";

    return fallos;
}

int main() {
    std::cout << "=== Practica 10 - Actividad 5: verificadores unitarios ===\n";

    // Prueba unitaria rapida antes del fuzzing masivo
    ArbolAcademico prueba;
    std::vector<int> codigosPrueba = {2021055123, 2020034011, 2022078456, 2019012987, 2023099001};
    for (int c : codigosPrueba) {
        prueba.insertar(Estudiante{c, "T", "T", 15.0, 100, EstadoAcademico::ACTIVO, "2024-I"});
    }
    assert(esBST(prueba.raiz.get()));
    assert(esInOrderOrdenado(prueba));
    assert(contarRecursivo(prueba.raiz.get()) == (int)codigosPrueba.size());
    prueba.eliminar(2020034011); // elimina un nodo con posibles dos hijos segun forma del arbol
    assert(esBST(prueba.raiz.get()));
    assert(esInOrderOrdenado(prueba));
    std::cout << "Verificadores unitarios: assert() paso sin errores (P1, P3, P4 OK)\n\n";

    std::cout << "=== Practica 10 - Actividad 6: fuzzing 500 x 50 operaciones ===\n";
    auto fallos = fuzzTest(500, 50, 42);
    std::cout << "Fallos detectados (total): " << fallos.size() << "\n";
    if (fallos.empty()) {
        std::cout << "CERTIFICACION C++17 EXITOSA - P1, P2, P3, P4 verificadas en 25000 casos\n";
    } else {
        std::cout << "PRIMER FALLO -> caso=" << fallos[0].caso
                  << " op=" << fallos[0].op
                  << " accion=" << fallos[0].accion
                  << " codigo=" << fallos[0].codigo << "\n";
    }
    return 0;
}
