all : base_opengl

base_opengl: base_opengl.o ppm.o kohonen.o
	gcc -g -Wall ppm.o base_opengl.o kohonen.o -o base_opengl -lGL -lGLU -lglut -lm 

ppm.o : ppm.c ppm.h
	gcc -g -Wall -c ppm.c -o ppm.o

base_opengl.o : base_opengl.c base_opengl.h
	gcc -g -Wall -c base_opengl.c -o base_opengl.o 

base_opengl.h :
	touch base_opengl.h

kohonen.o: kohonen.c kohonen.h
	gcc -g -Wall -c kohonen.c -o kohonen.o

kohonen.h:
	touch kohonen.h

ppm.h : 
	touch ppm.h

clean :
	rm -f base_opengl *.o
