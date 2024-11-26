flags = -Wno-deprecated -O2
comp = g++
prog  = OPHS

#El problema
config = ./Instancias/100-240-15-10.ophs

cr = 0.6				#Tasa de cruzamiento
mr = 0.01				#Tasa de mutación
ps = 30					#Population size
max_evals=1e+4			#Maximo de iteraciones
eo = 0					#Se aplica o no elitismo
so = 2					#Operador de selección
semilla = 27112024		#Manejar aleatoriedad
kTournament = 5			#Tamaño para k tournament

comp:$(prog)

exe:$(prog)
	#valgrind --leak-check=full --show-reachable=yes 
	./$(prog) $(config) ${so} ${eo} $(cr) $(mr) $(ps) $(max_evals) $(semilla) ${kTournament}

$(prog): main.o
	$(comp) main.o -o $(prog) $(flags)

main.o: main.cpp includes.h
	$(comp) -c main.cpp $(flags)

clean:
	rm -f *~
	rm -f *.o
	rm -f *.txt
	rm -f *.ps
	rm -f $(prog)
	rm -f core*

