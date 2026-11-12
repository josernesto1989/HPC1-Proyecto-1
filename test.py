
file = '/Volumes/JE32GB/matriz_ordenada.txt'

def read_two_lines_from_file(file):
    sumador=0
    with open(file, 'r') as f:
    #     while True:
    #         line1 = f.readline().strip()
    #         sumador += len(line1.split())
    #         if not line1:
    #             break
        
    # return sumador
        return f.readline()[0:50]

print(read_two_lines_from_file(file))