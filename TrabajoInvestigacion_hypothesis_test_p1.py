"""
Test de Hypothesis para verificar P1 (orden in-order) del BST de la
Práctica 09 / 10, usando @given y st.lists para generar los casos
automáticamente en lugar del fuzzing manual de la Actividad 4.

Ejecutar con:
    pip install hypothesis
    pytest hypothesis_test_p1.py -v
o directamente:
    python hypothesis_test_p1.py
"""

from hypothesis import given, settings, strategies as st

from framework import ArbolAcademico, Estudiante, EstadoAcademico, es_inorder_ordenado


def construir_arbol(codigos):
    """Construye un ArbolAcademico insertando una lista de códigos únicos."""
    arbol = ArbolAcademico()
    for cod in codigos:
        e = Estudiante(cod, "T", "T", 15.0, 100, EstadoAcademico.ACTIVO, "2024-I")
        arbol.insertar(e)
    return arbol


@given(st.lists(st.integers(min_value=1, max_value=10_000), unique=True))
@settings(max_examples=200)
def test_p1_inorder_ordenado(codigos):
    """
    Para cualquier lista de códigos únicos generada por Hypothesis,
    el recorrido in-order del BST resultante debe estar ordenado
    de forma ascendente (Teorema P1).
    """
    arbol = construir_arbol(codigos)
    assert es_inorder_ordenado(arbol)


if __name__ == "__main__":
    test_p1_inorder_ordenado()
    print("TEST DE HYPOTHESIS PARA P1: TODOS LOS CASOS GENERADOS PASARON (sin contraejemplos)")
