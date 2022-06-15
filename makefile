todo: skel

LDLIBS=-lGL -lGLEW -lglfw -lm 

skel: textfile.o 

clean:
	rm -f *.o *~

cleanall: clean
	rm -f skel
