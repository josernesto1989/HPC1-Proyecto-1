nombre_archivo = "/Volumes/JE32GB/matriz_ordenada.txt"
nombre_archivo_origen = "./matriz/matriz_a.txt"

last_value = None
is_ok = True
cantidad_elementos = 0
with open(nombre_archivo, "r") as archivo:
    for linea in archivo:
        fila = list(map(float, linea.split()))
        cantidad_elementos += len(fila)
        print(f"Elementos total: {cantidad_elementos}")
        if last_value is not None and fila[0] < last_value:
            print("La matriz no está ordenada correctamente.")
            is_ok = False
            break
if is_ok:
    print("La matriz está ordenada correctamente.")
    print(f"Cantidad de elementos: {cantidad_elementos}")
