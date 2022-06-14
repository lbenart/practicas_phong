todo: skel

LDLIBS=-lGL -lGLEW -lglfw -lm -lstdc++ -ldl -lstdc++fs

skel: textfile.o

clean:
	rm -f *.o *~

cleanall: clean
	rm -f skel