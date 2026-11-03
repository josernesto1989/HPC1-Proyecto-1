import random

def generar_matriz(filas, columnas, nombre_archivo):
    with open(nombre_archivo, 'w') as archivo:
        for _ in range(filas):
            fila = [str(random.randint(0, 1000000)) for _ in range(columnas)]
            archivo.write(' '.join(fila) + '\n')
generar_matriz(10000, 10000, './matriz/numeros.txt')