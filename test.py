#Pruebas unitarias

import numpy as np

file = 'matriz_ordenada.bin'
COLS = 40000
DTYPE = np.float32

with open(file, 'rb') as f:
    data = f.read(2 * COLS * np.dtype(DTYPE).itemsize)

arr = np.frombuffer(data, dtype=DTYPE)
if arr.size != 2 * COLS:
    raise AssertionError(
        f"Se esperaban {2*COLS} valores, se leyeron {arr.size}"
    )

fila1 = arr[:COLS]
fila2 = arr[COLS:2*COLS]

print(f"Primeros 5 de fila 1: {fila1[:20]}")
print(f"Primeros 5 de fila 2: {fila2[:20]}")

if not np.all(np.diff(fila1) >= 0):
    raise AssertionError("Fila 1 desordenada")
if not np.all(np.diff(fila2) >= 0):
    raise AssertionError("Fila 2 desordenada")
if fila1[-1] > fila2[0]:
    raise AssertionError(
        f"Fila 2 no continua el orden: {fila1[-1]} > {fila2[0]}"
    )

print("Las dos primeras filas estan ordenadas y son continuas.")
