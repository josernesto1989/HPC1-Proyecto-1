"""
Convierte un archivo de texto de numeros separados por espacios/saltos
de linea a un archivo binario plano (sin cabecera).

Uso:
    python txt_a_bin.py entrada.txt salida.bin [float32|float64]

Por defecto usa float32 (4 bytes por elemento).
"""
import sys
import numpy as np


def txt_a_bin(txt_path, bin_path, dtype=np.float32,
              buffer_tokens=10_000_000, verbose_every=100_000_000):
    dt = np.dtype(dtype)
    total = 0
    buffer = []

    with open(txt_path, 'r') as f_in, open(bin_path, 'wb') as f_out:
        for linea in f_in:
            tokens = linea.split()
            if not tokens:
                continue
            buffer.extend(tokens)

            if len(buffer) >= buffer_tokens:
                arr = np.array(buffer, dtype=dt)
                arr.tofile(f_out)
                total += arr.size
                buffer.clear()
                if total % verbose_every < buffer_tokens:
                    print(f"Convertidos: {total:,} elementos")

        if buffer:
            arr = np.array(buffer, dtype=dt)
            arr.tofile(f_out)
            total += arr.size

    print(f"Total convertidos: {total:,} elementos -> {bin_path}")
    return total


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)
    entrada = sys.argv[1]
    salida = sys.argv[2]
    tipo = sys.argv[3] if len(sys.argv) > 3 else "float32"
    dtype = np.float32 if tipo == "float32" else np.float64
    txt_a_bin(entrada, salida, dtype=dtype)
