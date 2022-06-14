todo: spinningcube_withlight spinningcube_withlight0

LDLIBS=-lGL -lGLEW -lglfw -lm 

spinningcube_withlight: spinningcube_withlight.o spinningcube_withlight0.o textfile.o 

clean:
	rm -f *.o *~

cleanall: clean
	rm -f spinningcube_withlight
