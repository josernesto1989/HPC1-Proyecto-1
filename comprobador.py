nombre_archivo = './matriz/numeros.txt'
last_value = None
is_ok = True
with open(nombre_archivo, 'r') as archivo:
    for linea in archivo:
        fila = list(map(int, linea.split()))
        if last_value is not None and fila[0] < last_value:
            print("La matriz no está ordenada correctamente.")
            is_ok = False
            break
if is_ok:
    print("La matriz está ordenada correctamente.")
