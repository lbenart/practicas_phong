# Repositorio para Práctica OpenGL: Iluminación con Phong

Este repositorio contiene el desarrollo de los ejercicios de la práctica OpenGL Iluminación con Phong de la asignatura IGM, MUEI 2021-2022. Estudiante Laura Ben Artiles.

Está organizado de la siguiente forma:

* Rama <b>feature/lighting</b>.
Contiene los archivos para dar respuesta a la primera parte de esta tarea: con una sola fuente de luz y un cubo con un único material homogéneo aplicar Phong shading básico.

* Rama <b>feature/secondCube</b>.
Contiene los archivos para dar respuesta a la segunda parte de esta tarea: añadir un segundo cubo rotando, más pequeño, y un segundo punto de luz. (Esta funcionalidad fue la última que se desarrolló por eso los cubos en este punto ya cuentan con mapa difuso y mapa especular.)

* Rama <b>feature/diffuse</b>.
Contiene los archivos para dar respuesta a la tercera parte de esta tarea: añadir un mapa difuso al cubo para el cálculo de Phong.

* Rama <b>feature/specular</b>.
Contiene los archivos para dar respuesta a la cuarta parte de esta tarea: añadir también un mapa especular, que permita distintos comportamientos de la componente especular de la luz en función del punto iluminado del cubo.

* Rama <b>main</b>. Contiene un merge desde la rama feature/specular que es la más adelantada.


En cada rama hay una carpeta que contiene capturas con los ejemplos de renders obtenidos para cada caso.

## Compilación

Cada rama contiene un archivo makefile apropiado, por tanto basta con ejecutar el comando:

```console
    $ make
```

## Ejecución
```console
    $ ./skel
```
