all: npshell

npshell: npshell.o 
	g++ -o npshell npshell.o 

npshell.o: npshell.cpp
	g++ -c npshell.cpp

clean:
	rm npshell npshell.o
