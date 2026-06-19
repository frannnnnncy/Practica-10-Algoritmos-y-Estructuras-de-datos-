#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <climits>
#include <cmath>
#include <algorithm>
#include <string>

enum class EstadoAcademico { ACTIVO, RETIRADO };

struct Estudiante {
    int codigo;
    std::string nombres;
    std::string apellidos;
    float promedio;
    int creditos;
    EstadoAcademico estado;
    std::string periodo;
};

struct NodoBST {
    Estudiante dato;
    std::unique_ptr<NodoBST> izquierdo;
    std::unique_ptr<NodoBST> derecho;
    explicit NodoBST(Estudiante d) : dato(std::move(d)) {}
};

class ArbolAcademico {
public:
    std::unique_ptr<NodoBST> raiz;

    void insertar(Estudiante e) { raiz = insertarRec(std::move(raiz), std::move(e)); }
    void eliminar(int codigo) { raiz = eliminarRec(std::move(raiz), codigo); }

    std::vector<Estudiante> inOrder() const {
        std::vector<Estudiante> resultado;
        inOrderRec(raiz.get(), resultado);
        return resultado;
    }

    int altura() const { return alturaRec(raiz.get()); }

private:
    static std::unique_ptr<NodoBST> insertarRec(std::unique_ptr<NodoBST> nodo, Estudiante e) {
        if (!nodo) { auto n = std::make_unique<NodoBST>(std::move(e)); return n; }
        if (e.codigo < nodo->dato.codigo) nodo->izquierdo = insertarRec(std::move(nodo->izquierdo), std::move(e));
        else if (e.codigo > nodo->dato.codigo) nodo->derecho = insertarRec(std::move(nodo->derecho), std::move(e));
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
            NodoBST* sucesor = nodo->derecho.get();
            while (sucesor->izquierdo) sucesor = sucesor->izquierdo.get();
            int codigoSucesor = sucesor->dato.codigo;
            nodo->dato = sucesor->dato;
            nodo->derecho = eliminarRec(std::move(nodo->derecho), codigoSucesor);
        }
        return nodo;
    }

    static void inOrderRec(const NodoBST* n, std::vector<Estudiante>& out) {
        if (!n) return;
        inOrderRec(n->izquierdo.get(), out);
        out.push_back(n->dato);
        inOrderRec(n->derecho.get(), out);
    }

    static int alturaRec(const NodoBST* n) {
        if (!n) return -1;
        return 1 + std::max(alturaRec(n->izquierdo.get()), alturaRec(n->derecho.get()));
    }
};

// ---------- Verificadores (Actividad 5) ----------

bool esBST(const NodoBST* n, int mn = INT_MIN, int mx = INT_MAX) {
    if (!n) return true;
    if (n->dato.codigo <= mn || n->dato.codigo >= mx) return false;
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
    int cota = (int)std::ceil(std::log2(n + 1)) - 1;
    return altura >= cota;
}

// ---------- Fuzzing (Actividad 6) ----------

struct ResultadoFuzz { int caso, op; std::string accion; int codigo;
    bool p1, p2, p3, p4; };

std::vector<ResultadoFuzz> fuzzTest(int nCasos, int nOps, int semilla, long long& totalVerif) {
    std::mt19937 rng(semilla);
    std::vector<ResultadoFuzz> fallos;
    totalVerif = 0;
    for (int caso = 0; caso < nCasos; caso++) {
        ArbolAcademico arbol;
        std::vector<int> activos;
        std::uniform_int_distribution<int> dist(0, 2); // 0,1 = insertar ; 2 = eliminar (2:1)
        for (int op = 0; op < nOps; op++) {
            bool insertar = (dist(rng) != 2) || activos.empty();
            int codigo;
            std::string accion;
            if (insertar) {
                codigo = 20000000 + caso * 1000 + op;
                arbol.insertar({codigo, "T", "T", 15.0f, 100, EstadoAcademico::ACTIVO, "2024-I"});
                activos.push_back(codigo);
                accion = "insertar";
            } else {
                int idx = rng() % activos.size();
                codigo = activos[idx];
                arbol.eliminar(codigo);
                activos.erase(activos.begin() + idx);
                accion = "eliminar";
            }
            totalVerif++;
            bool p1 = esInOrderOrdenado(arbol);
            bool p2 = cotaAlturaOK(arbol.altura(), (int)activos.size());
            bool p3 = contarRecursivo(arbol.raiz.get()) == (int)activos.size();
            bool p4 = esBST(arbol.raiz.get());
            if (!(p1 && p2 && p3 && p4))
                fallos.push_back({caso, op, accion, codigo, p1, p2, p3, p4});
        }
    }
    return fallos;
}

int main() {
    long long totalVerif = 0;
    auto fallos = fuzzTest(500, 50, 42, totalVerif);

    int fallosP1 = 0, fallosP2 = 0, fallosP3 = 0, fallosP4 = 0;
    for (const auto& f : fallos) {
        if (!f.p1) fallosP1++;
        if (!f.p2) fallosP2++;
        if (!f.p3) fallosP3++;
        if (!f.p4) fallosP4++;
    }

    std::cout << "Verificaciones ejecutadas: " << totalVerif << "\n";
    std::cout << "Fallos detectados: " << fallos.size() << "\n";
    std::cout << "Fallos P1 (in-order): " << fallosP1 << "\n";
    std::cout << "Fallos P2 (altura): " << fallosP2 << "\n";
    std::cout << "Fallos P3 (conteo): " << fallosP3 << "\n";
    std::cout << "Fallos P4 (BST): " << fallosP4 << "\n";
    if (fallos.empty())
        std::cout << "CERTIFICACION C++ EXITOSA - P1,P2,P3,P4 verificadas\n";
    return 0;
}
