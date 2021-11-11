# SistemasOperativosPII
Segundo Proyecto del curso de Sistemas Operativos. Se simula la administración de memoria dinámica de un sistema operativo creando un espacio de memoria compartida y creando threads de forma aleatoria que solicitan parte de dicho espacio de memoria. Para la administración de memoria, se desarrollaron los algoritmos *first-fit*, *best-fit* y *worst-fit*.


## Instrucciones
### Compilar
Primero, compilar los distintos archivos utilizando el comando

`make`

Si se desea, se puede compilar individualmente cada uno de los programas con los siguientes comandos
* Initializer: `make init`
* Producer: `make prod`
* Spy: `make spy`
* Destroyer: `make dest`

### Correr

Luego, corremos el programa inicializador. Nótese que este programa espera el número de líneas como parámetro.

`./init.out <número de líneas>`

Una vez que se corre el inicializador, podemos correr el productor.

`./prod.out <número de líneas> <numero algoritmo> <id_shared_mem> <execId> <blockedId> <currentId>`

Mientras el productor de procesos se ejecuta, podemos correr el proceso espía para ver la ejecución de los procesos.

`./spy.out <número de líneas> <id_shared_mem> <id hilos en ejecucion shm> <id hilos bloqueados shm> <id hilo actual shm>`

### Finalizar

Para cerrar los recursos y la memoria compartida, así como todos los procesos, se corre el proceso finalizador.

`./dest.out <id_shared_mem> <id hilos en ejecucion shm> <id hilos bloqueados shm> <id hilo actual shm> <archivo> <id proceso prod> `
