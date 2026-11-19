import numpy as np

nombre_archivo = "matriz_ordenada.bin"
FILAS_ESPERADAS = 20000
COLS_ESPERADAS = 20000
TOTAL_ESPERADO = FILAS_ESPERADAS * COLS_ESPERADAS
DTYPE = np.float32
CHUNK_ELEMENTOS = 10_000_000
BYTES_CHUNK = CHUNK_ELEMENTOS * np.dtype(DTYPE).itemsize

last_value = None
cantidad_elementos = 0
is_ok = True

with open(nombre_archivo, "rb") as f:
    while True:
        data = f.read(BYTES_CHUNK)
        if not data:
            break
        arr = np.frombuffer(data, dtype=DTYPE)
        if arr.size == 0:
            continue

        # Orden dentro del chunk
        diffs = np.diff(arr)
        bad = np.where(diffs < 0)[0]
        if bad.size > 0:
            i = int(bad[0])
            print(f"Desorden en elemento {cantidad_elementos + i}: "
                  f"{arr[i+1]} < {arr[i]}")
            is_ok = False
            break

        # Continuidad entre chunks
        if last_value is not None and arr[0] < last_value:
            print(f"Desorden entre chunks: {arr[0]} < {last_value}")
            is_ok = False
            break

        last_value = arr[-1]
        cantidad_elementos += arr.size

if is_ok:
    print("La matriz esta ordenada correctamente.")
    if cantidad_elementos == TOTAL_ESPERADO:
        print(f"Cantidad de elementos: {cantidad_elementos} (correcta)")
    else:
        print(f"ATENCION: se esperaban {TOTAL_ESPERADO}, "
              f"se encontraron {cantidad_elementos}")
