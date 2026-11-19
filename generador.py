import os
import sys
import numpy as np


def generar_matriz_bin(filas, columnas, nombre_archivo,
                       dtype=np.float32, chunk_rows=1000):
    directorio = os.path.dirname(nombre_archivo)
    if directorio:
        os.makedirs(directorio, exist_ok=True)
    dt = np.dtype(dtype)

    with open(nombre_archivo, 'wb') as f:
        for start in range(0, filas, chunk_rows):
            end = min(start + chunk_rows, filas)
            # Generar como int64 para no perder precision y luego convertir
            block = np.random.randint(
                0, 1_000_001, size=(end - start, columnas), dtype=np.int64
            ).astype(dt)
            block.tofile(f)


if __name__ == "__main__":
    filas = int(sys.argv[1]) if len(sys.argv) > 1 else 1000
    cols = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
    ruta = sys.argv[3] if len(sys.argv) > 3 else 'matriz_a.bin'
    tipo = sys.argv[4] if len(sys.argv) > 4 else 'float32'
    dtype = np.float32 if tipo == 'float32' else np.float64

    print(f"Generando {filas}x{cols} {tipo} en {ruta}...")
    generar_matriz_bin(filas, cols, ruta, dtype=dtype)
    print("Done.")
