flags = -Wno-deprecated -O2
comp = g++
prog  = OPHS

#El problema
config = ./Instancias/100-80-12-6.ophs
results = results.txt

cr = 0.3				#Tasa de cruzamiento
mr = 0.01				#Tasa de mutación
ps = 5					#Population size
max_evals=1e+4			#Maximo de iteraciones
eo = 1
semilla = 2

comp:$(prog)

exe:$(prog)
	#valgrind --leak-check=full --show-reachable=yes 
	./$(prog) $(config) $(results) ${eo} $(cr) $(mr) $(ps) $(max_evals) $(semilla)

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

