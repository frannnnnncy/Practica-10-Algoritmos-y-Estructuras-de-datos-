# Práctica N.° 10 — Inducción Estructural en Árboles Binarios de Búsqueda

Verificación formal del índice de matrículas SIGA-UNAP mediante inducción estructural, property-based testing y fuzzing, en Python y C++17.

**Curso:** Algoritmos y Estructuras de Datos — SIS210
**Escuela:** Ingeniería de Sistemas — Universidad Nacional del Altiplano (Puno)
**Docente:** Dr. Aldo Hernán Zanabria Gálvez
**Autora:** Francy

## Descripción

Este repositorio contiene el desarrollo completo de la Práctica 10, que aplica el principio de **inducción estructural** para demostrar formalmente cinco propiedades de los árboles binarios de búsqueda (BST), y certifica esas propiedades de forma automatizada sobre una implementación del índice de matrículas del SIGA-UNAP, tanto en Python como en C++17.

Propiedades demostradas y verificadas:

| # | Propiedad | Enunciado |
|---|-----------|-----------|
| P1 | Orden in-order | El recorrido in-order de un BST produce una lista ordenada ascendente |
| P2 | Cota de altura | h(T) ≥ ⌈log2(n+1)⌉ − 1 |
| P3 | Conteo de nodos | \|T\| = \|T_L\| + \|T_R\| + 1 |
| P4 | Invariante en inserción | insertar(T, k) preserva la invariante BST |
| P5 | Invariante en eliminación | eliminar(T, k) preserva la invariante BST (incluye el caso de dos hijos) |

## Estructura del repositorio

```
.
├── README.md
├── Practica10_PorActividad.docx        # Informe completo (demostraciones, frameworks, reflexión, investigación)
├── Actividad3y4_framework.py           # Verificadores + fuzzing en Python (Actividades 3 y 4)
├── Actividad5y6_framework.cpp          # Verificadores + fuzzing en C++17 (Actividades 5 y 6)
└── TrabajoInvestigacion_hypothesis_test_p1.py   # Test de Hypothesis (@given, st.lists) para P1
```

## Requisitos

- Python 3.11+
- g++ con soporte para C++17
- (Opcional, para el test de Hypothesis) `pip install hypothesis pytest`

## Cómo ejecutar

### Python — Actividades 3 y 4

```bash
python3 Actividad3y4_framework.py
```

Salida esperada:

```
Verificaciones ejecutadas: 25000
Fallos detectados: 0
TODAS LAS PROPIEDADES VERIFICADAS - P1,P2,P3,P4 certificadas
```

### C++17 — Actividades 5 y 6

```bash
g++ -std=c++17 -O2 -Wall -o framework Actividad5y6_framework.cpp
./framework
```

Salida esperada:

```
Verificaciones ejecutadas: 25000
Fallos detectados: 0
CERTIFICACION C++ EXITOSA - P1,P2,P3,P4 verificadas
```

### Trabajo de investigación — Test de Hypothesis (P1)

El archivo `TrabajoInvestigacion_hypothesis_test_p1.py` debe estar en la misma carpeta que `Actividad3y4_framework.py`, ya que importa el BST y el verificador de P1 desde ese módulo.

```bash
pip install hypothesis pytest
python -m pytest TrabajoInvestigacion_hypothesis_test_p1.py -v
```

Salida esperada:

```
hypothesis_test_p1.py::test_p1_inorder_ordenado PASSED   [100%]
1 passed in 0.93s
```

## Resultados obtenidos

| Propiedad | Casos | Fallos Python | Fallos C++ | Certificada |
|-----------|-------|----------------|------------|-------------|
| P1 — in-order ordenado | 25,000 | 0 | 0 | Sí |
| P2 — cota de altura | 25,000 | 0 | 0 | Sí |
| P3 — conteo correcto | 25,000 | 0 | 0 | Sí |
| P4 — invariante BST | 25,000 | 0 | 0 | Sí |

Ambas implementaciones (Python y C++17) certifican las mismas cuatro propiedades sobre los mismos 25,000 casos de fuzzing, sin fallos registrados.

## Demostraciones formales

Las demostraciones por inducción estructural de P1, P3, P4 y P5 (casos base, hipótesis inductiva y paso inductivo) se encuentran desarrolladas en `Practica10_PorActividad.docx`, junto con las preguntas de reflexión y el trabajo de investigación sobre Hypothesis, Coq e Isabelle/HOL.

## Referencias

- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2022). *Introduction to algorithms* (4th ed.). MIT Press.
- Claessen, K., & Hughes, J. (2000). QuickCheck: A lightweight tool for random testing of Haskell programs. *ACM SIGPLAN Notices, 35*(9), 268–279. https://doi.org/10.1145/351240.351266
- MacIver, D. R., & Hatfield-Dodds, Z. (2019). Hypothesis: A new approach to property-based testing. *Journal of Open Source Software, 4*(43), 1891. https://doi.org/10.21105/joss.01891
- Bertot, Y., & Castéran, P. (2004). *Interactive theorem proving and program development: Coq'Art*. Springer. https://doi.org/10.1007/978-3-662-07964-5
- Nipkow, T., Paulson, L. C., & Wenzel, M. (2002). *Isabelle/HOL: A proof assistant for higher-order logic*. Springer. https://doi.org/10.1007/3-540-45949-9
- Winskel, G. (1993). *The formal semantics of programming languages: An introduction*. MIT Press.
