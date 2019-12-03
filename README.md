# Proyecto_Operativos

## Resumen
  **Proyecto correspondiente al primer Parcial de la materia de Sistemas Operativos.**<br>
  **Autor:** Oswaldo Aguilar Mora   &nbsp; &nbsp; &nbsp; &nbsp; **Matrícula:** 201419079 <br>
  **Tutor:** Angel Lopez    &nbsp; &nbsp; &nbsp;&nbsp; **Paralelo #** 2<br>
  **Fecha:** 03/12/2019<br>
  
  ## Estructura
   + **Proyecto Parcial -Oswaldo Aguilar** -> Código
      - bin: Ejecutables
      - files: Archivos de prueba
      - include
      - obj
      - src: Archivos fuentes
        - shell.c -> Parte 1 del proyecto
        - twc.c  -> Parte 2 del proyecto
      - Makefile
      <br>
   + Diagramas:
      <br>
 
 ## Especificacion
   + **Shell** -> Validaciones necesarias por lo que permite las siguientes entradas.
     - exit
     - exit [anything]
     - cd
     - cd dir
     - cd .
     - cd ..
     - cd [dirs] -> Mesaje de error
     - [executable] [args]
     
     ![Shell_Captura](https://github.com/Oswagui/Proyecto_Operativos/blob/master/Diagramas%20y%20Documentaci%C3%B3n/shell.PNG)
     
   + **wtc** -> Validaciones necesarias por lo que permite las siguientes entradas. Salida formato ( archivo, lineas, palabras) en caso de ser más de un archivo.
     - wtc -l [file]
     - wtc -w [file]
     - wtc -w -l [file]
     - wtc -l [files]
     - wtc -w [files]
     - wtc -w -l [files]
     ![Shell_Captura](https://github.com/Oswagui/Proyecto_Operativos/blob/master/Diagramas%20y%20Documentaci%C3%B3n/twc.PNG)
     
    
 ## Estrategia
 ![Estrategia wtc](https://github.com/Oswagui/Proyecto_Operativos/blob/master/Diagramas%20y%20Documentaci%C3%B3n/Estrategia%20twc.PNG)
 
 
  ## Bugs
     - Ingreso vacio en shell de la parte 1.
     - En caso de multiples archivos en el twc, falla el segundo archivo por algun motivo con el numero de palabras. Cosas de memoria.
     - Ingresar obligatoriamente al menos una bandera en twc.
     
   ![Shell_Captura](https://github.com/Oswagui/Proyecto_Operativos/blob/master/Diagramas%20y%20Documentaci%C3%B3n/twc_bug.PNG)
   
   
   ## Discusión
     - Inicialmente pensaba hacer un hilo por archivo para resolver el problema.
     - Decidi cambiar ese enfoque ya que en el caso que solo ingrese un arhcivo seria igual que no usar hilos.
     - Mi algoritmo se basa en el tamaño de los archivos y los procesadores disponibles, de esta forma al archivo con más tamaño se le da más hilos para procesar su trabajo.
     - Almceno contadores por punteros, por archivos y por total, para el calculo.
     - Al dividir un archivo puebe existir un desfase en el conteo ya que una palabra puede ser cortada y contada dós veces por lo que se realizo un ajuste para estos casos.
     - No se que tan valido seá mi procedimiento ya que en la maquina virtual solo daba un procesador y cpu por lo que use una cosntante de 4 para simular el procesamiento y la division.
     - Resulta algo complicado el manejo de memoria y c onstantemente daba problemas que solucionar por la falta de experiencia, use un debugger para solucionar la mayoria de estos problemas.
     
   
