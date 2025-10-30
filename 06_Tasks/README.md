# Task Program

Con este programa aprenderemos el uso de las tareas dentro del ESP32 y como podemos ejecutar varias funciones en paralelo sin que sean afectadas por los retardos.

Para mayor profundidad en el tema, ver el video [aquí](https://www.youtube.com/watch?v=abwyjmfZ0mQ&list=PL-Hb9zZP9qC65SpXHnTAO0-qV6x5JxCMJ&index=12)


## Ejercicio 1

Crea un programa que tenga dos tareas: 
1. La primer tarea es leer el valor de un canal del ADC y almacenar la lectura en una variable global.  

2. La segunda tarea es controlar el sentido de giro y velocidad de un motor de CD siguiendo la siguiente regla:  
   - Si el valor del ADC está entre **0 y 2048**, el motor gira en **sentido horario** a una velocidad proporcional.  
   - Si el valor del ADC está entre **2049 y 4095**, el motor gira en **sentido antihorario** a una velocidad proporcional.
