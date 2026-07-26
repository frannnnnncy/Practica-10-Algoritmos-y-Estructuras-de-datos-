from enum import Enum
import random
import math

# ==========================================================
# MODELO ACADEMICO
# ==========================================================

class EstadoAcademico(Enum):
    ACTIVO = "ACTIVO"
    RETIRADO = "RETIRADO"


class Estudiante:
    def __init__(self, codigo, nombres, apellidos,
                 promedio, creditos, estado, periodo):
        self.codigo = codigo
        self.nombres = nombres
        self.apellidos = apellidos
        self.promedio = promedio
        self.creditos = creditos
        self.estado = estado
        self.periodo = periodo

    def __repr__(self):
        return f"Estudiante({self.codigo})"


# ==========================================================
# BST
# ==========================================================

class NodoBST:
    def __init__(self, dato):
        self.dato = dato
        self.izquierdo = None
        self.derecho = None


class ArbolAcademico:

    def __init__(self):
        self._raiz = None

    def insertar(self, estudiante):
        def _insertar(nodo, estudiante):
            if nodo is None:
                return NodoBST(estudiante)
            if estudiante.codigo < nodo.dato.codigo:
                nodo.izquierdo = _insertar(nodo.izquierdo, estudiante)
            elif estudiante.codigo > nodo.dato.codigo:
                nodo.derecho = _insertar(nodo.derecho, estudiante)
            return nodo
        self._raiz = _insertar(self._raiz, estudiante)

    def eliminar(self, codigo):
        def minimo(nodo):
            while nodo.izquierdo:
                nodo = nodo.izquierdo
            return nodo

        def _eliminar(nodo, codigo):
            if nodo is None:
                return None
            if codigo < nodo.dato.codigo:
                nodo.izquierdo = _eliminar(nodo.izquierdo, codigo)
            elif codigo > nodo.dato.codigo:
                nodo.derecho = _eliminar(nodo.derecho, codigo)
            else:
                if nodo.izquierdo is None:
                    return nodo.derecho
                if nodo.derecho is None:
                    return nodo.izquierdo
                sucesor = minimo(nodo.derecho)
                nodo.dato = sucesor.dato
                nodo.derecho = _eliminar(nodo.derecho, sucesor.dato.codigo)
            return nodo
        self._raiz = _eliminar(self._raiz, codigo)

    def in_order(self):
        resultado = []
        def _rec(nodo):
            if nodo is None:
                return
            _rec(nodo.izquierdo)
            resultado.append(nodo.dato)
            _rec(nodo.derecho)
        _rec(self._raiz)
        return resultado

    def altura(self):
        def _altura(nodo):
            if nodo is None:
                return -1
            return 1 + max(_altura(nodo.izquierdo), _altura(nodo.derecho))
        return _altura(self._raiz)


# ==========================================================
# VERIFICADORES (ACTIVIDAD 3)
# ==========================================================

def es_bst(nodo, minimo=float('-inf'), maximo=float('inf')):
    if nodo is None:
        return True
    if not (minimo < nodo.dato.codigo < maximo):
        return False
    return (es_bst(nodo.izquierdo, minimo, nodo.dato.codigo) and
            es_bst(nodo.derecho, nodo.dato.codigo, maximo))


def es_inorder_ordenado(arbol):
    codigos = [e.codigo for e in arbol.in_order()]
    return codigos == sorted(codigos)


def contar_recursivo(nodo):
    if nodo is None:
        return 0
    return 1 + contar_recursivo(nodo.izquierdo) + contar_recursivo(nodo.derecho)


def cota_altura_cumplida(arbol, n):
    if n == 0:
        return True
    cota = math.ceil(math.log2(n + 1)) - 1
    return arbol.altura() >= cota


def verificar_propiedades(arbol, n_esperado):
    return {
        "P1_inorder_ordenado": es_inorder_ordenado(arbol),
        "P2_cota_altura": cota_altura_cumplida(arbol, n_esperado),
        "P3_conteo_correcto": contar_recursivo(arbol._raiz) == n_esperado,
        "P4_es_bst": es_bst(arbol._raiz),
    }


# ==========================================================
# FUZZING (ACTIVIDAD 4)
# ==========================================================

def fuzz_test(n_casos=500, n_operaciones=50, semilla=42):
    random.seed(semilla)
    fallos = []
    fallos_por_propiedad = {"P1_inorder_ordenado": 0, "P2_cota_altura": 0,
                             "P3_conteo_correcto": 0, "P4_es_bst": 0}

    for caso in range(n_casos):
        arbol = ArbolAcademico()
        codigos_activos = set()
        codigos_pool = list(range(20_000_000 + caso * 1000,
                                   20_000_000 + caso * 1000 + n_operaciones * 3))
        random.shuffle(codigos_pool)

        for op in range(n_operaciones):
            accion = random.choice(["insertar", "insertar", "eliminar"])

            if accion == "insertar" or not codigos_activos:
                if not codigos_pool:
                    break
                cod = codigos_pool.pop()
                e = Estudiante(cod, "T", "T", 15.0, 100, EstadoAcademico.ACTIVO, "2024-I")
                arbol.insertar(e)
                codigos_activos.add(cod)
            else:
                cod = random.choice(list(codigos_activos))
                arbol.eliminar(cod)
                codigos_activos.remove(cod)

            resultados = verificar_propiedades(arbol, len(codigos_activos))
            for k, v in resultados.items():
                if not v:
                    fallos_por_propiedad[k] += 1
            if not all(resultados.values()):
                fallos.append((caso, op, accion, cod, resultados))

    return fallos, fallos_por_propiedad


if __name__ == "__main__":
    print("=== Practica 10 - Actividad 3 y 4: Python 3.12 ===")
    fallos, fallos_por_propiedad = fuzz_test(n_casos=500, n_operaciones=50, semilla=42)
    print("Verificaciones ejecutadas: 500 secuencias x 50 operaciones = 25000")
    print(f"Fallos detectados: {len(fallos)}")
    print(f"Fallos por propiedad: {fallos_por_propiedad}")
    if fallos:
        print("PRIMER FALLO:", fallos[0])
    else:
        print("TODAS LAS PROPIEDADES VERIFICADAS - P1,P2,P3,P4 certificadas (sin contraejemplos)")
