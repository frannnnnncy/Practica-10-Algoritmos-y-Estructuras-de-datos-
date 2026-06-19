import random
import math
from dataclasses import dataclass
from enum import Enum


class EstadoAcademico(Enum):
    ACTIVO = 1
    RETIRADO = 2


@dataclass
class Estudiante:
    codigo: int
    nombres: str
    apellidos: str
    promedio: float
    creditos: int
    estado: EstadoAcademico
    periodo: str


class NodoBST:
    __slots__ = ("dato", "izquierdo", "derecho")

    def __init__(self, dato):
        self.dato = dato
        self.izquierdo = None
        self.derecho = None


class ArbolAcademico:
    def __init__(self):
        self._raiz = None

    def insertar(self, e: Estudiante):
        self._raiz = self._insertar_rec(self._raiz, e)

    def _insertar_rec(self, nodo, e):
        if nodo is None:
            return NodoBST(e)
        if e.codigo < nodo.dato.codigo:
            nodo.izquierdo = self._insertar_rec(nodo.izquierdo, e)
        elif e.codigo > nodo.dato.codigo:
            nodo.derecho = self._insertar_rec(nodo.derecho, e)
        return nodo

    def eliminar(self, codigo):
        self._raiz = self._eliminar_rec(self._raiz, codigo)

    def _eliminar_rec(self, nodo, codigo):
        if nodo is None:
            return None
        if codigo < nodo.dato.codigo:
            nodo.izquierdo = self._eliminar_rec(nodo.izquierdo, codigo)
        elif codigo > nodo.dato.codigo:
            nodo.derecho = self._eliminar_rec(nodo.derecho, codigo)
        else:
            if nodo.izquierdo is None:
                return nodo.derecho
            if nodo.derecho is None:
                return nodo.izquierdo
            sucesor = nodo.derecho
            while sucesor.izquierdo is not None:
                sucesor = sucesor.izquierdo
            nodo.dato = sucesor.dato
            nodo.derecho = self._eliminar_rec(nodo.derecho, sucesor.dato.codigo)
        return nodo

    def in_order(self):
        resultado = []
        self._in_order_rec(self._raiz, resultado)
        return resultado

    def _in_order_rec(self, nodo, resultado):
        if nodo is not None:
            self._in_order_rec(nodo.izquierdo, resultado)
            resultado.append(nodo.dato)
            self._in_order_rec(nodo.derecho, resultado)

    def altura(self):
        return self._altura_rec(self._raiz)

    def _altura_rec(self, nodo):
        if nodo is None:
            return -1
        return 1 + max(self._altura_rec(nodo.izquierdo), self._altura_rec(nodo.derecho))


# ---------- Verificadores (Actividad 3) ----------

def es_bst(nodo, minimo=float('-inf'), maximo=float('inf')) -> bool:
    if nodo is None:
        return True
    if not (minimo < nodo.dato.codigo < maximo):
        return False
    return (es_bst(nodo.izquierdo, minimo, nodo.dato.codigo) and
            es_bst(nodo.derecho, nodo.dato.codigo, maximo))


def es_inorder_ordenado(arbol) -> bool:
    codigos = [e.codigo for e in arbol.in_order()]
    return codigos == sorted(codigos)


def contar_recursivo(nodo) -> int:
    if nodo is None:
        return 0
    return 1 + contar_recursivo(nodo.izquierdo) + contar_recursivo(nodo.derecho)


def cota_altura_cumplida(arbol, n) -> bool:
    if n == 0:
        return True
    cota = math.ceil(math.log2(n + 1)) - 1
    return arbol.altura() >= cota


def verificar_propiedades(arbol, n_esperado) -> dict:
    return {
        'P1_inorder_ordenado': es_inorder_ordenado(arbol),
        'P2_cota_altura': cota_altura_cumplida(arbol, n_esperado),
        'P3_conteo_correcto': contar_recursivo(arbol._raiz) == n_esperado,
        'P4_es_bst': es_bst(arbol._raiz),
    }


# ---------- Fuzzing (Actividad 4) ----------

def fuzz_test(n_casos=500, n_operaciones=50, semilla=42):
    random.seed(semilla)
    fallos = []
    total_verificaciones = 0
    for caso in range(n_casos):
        arbol = ArbolAcademico()
        codigos_activos = set()
        codigos_pool = list(range(20_000_000, 20_000_000 + n_operaciones * 3))
        random.shuffle(codigos_pool)
        for op in range(n_operaciones):
            accion = random.choice(['insertar', 'insertar', 'eliminar'])
            if accion == 'insertar' or not codigos_activos:
                cod = codigos_pool.pop()
                e = Estudiante(cod, 'T', 'T', 15.0, 100, EstadoAcademico.ACTIVO, '2024-I')
                arbol.insertar(e)
                codigos_activos.add(cod)
            else:
                cod = random.choice(list(codigos_activos))
                arbol.eliminar(cod)
                codigos_activos.remove(cod)
            total_verificaciones += 1
            resultados = verificar_propiedades(arbol, len(codigos_activos))
            if not all(resultados.values()):
                fallos.append((caso, op, accion, cod, resultados))
    return fallos, total_verificaciones


if __name__ == "__main__":
    fallos, total = fuzz_test(n_casos=500, n_operaciones=50, semilla=42)
    print(f"Verificaciones ejecutadas: {total}")
    print(f"Fallos detectados: {len(fallos)}")
    if fallos:
        print("PRIMER FALLO:", fallos[0])
    else:
        print("TODAS LAS PROPIEDADES VERIFICADAS - P1,P2,P3,P4 certificadas")

    # Conteo por propiedad para la tabla comparativa
    fallos_por_propiedad = {'P1_inorder_ordenado': 0, 'P2_cota_altura': 0,
                             'P3_conteo_correcto': 0, 'P4_es_bst': 0}
    random.seed(42)
    fallos2, _ = fuzz_test(n_casos=500, n_operaciones=50, semilla=42)
    for (_, _, _, _, resultados) in fallos2:
        for prop, ok in resultados.items():
            if not ok:
                fallos_por_propiedad[prop] += 1
    print("Fallos por propiedad:", fallos_por_propiedad)
