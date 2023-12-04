PAV - P3: estimación de pitch
=============================

Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.

Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la estimación de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.

<img width="517" alt="Captura de pantalla 2023-12-04 012927" src="https://github.com/juangarub/P3/assets/84085478/b47da4ae-de7f-43d2-b124-054c71d01b35">


   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un segmento de
     unos 30 ms de un fonema sonoro y su periodo de pitch; y, en otro *subplot*, se vea con claridad la
	 autocorrelación de la señal y la posición del primer máximo secundario.

	 NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para
	 hacerlo. Se valorará la utilización de la biblioteca matplotlib de Python.

Este es el código Matlab que hemos utilizado para visualizar las gráficas de la señal:

<img width="556" alt="Captura de pantalla 2023-12-03 233218" src="https://github.com/juangarub/P3/assets/84085478/708d5214-f3d7-4c79-b837-3e8401e03291">

En esta, podemos encontrar una muestra de 30ms de la señal y su autocorrelación. Encontramos su frecuencia f0 mediante el periodo (9.02ms), lo cual nos da una frecuencia de 110Hz. Esta se corresponde con las 440 muestras del pico de la correlación para una señal con una frecuencia de muestreo de 48kHz.

<img width="856" alt="Captura de pantalla 2023-12-03 231216" src="https://github.com/juangarub/P3/assets/84085478/0cdb8d74-501e-42c3-8b43-547328250538">


   * Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.

     <img width="574" alt="Captura de pantalla 2023-12-04 013351" src="https://github.com/juangarub/P3/assets/84085478/4e4f0538-449e-4d32-b515-66719bce8d9c">


   * Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.

<img width="384" alt="Captura de pantalla 2023-12-04 035908" src="https://github.com/juangarub/P3/assets/84085478/c8d68c0e-a494-400c-b554-626aa6d04c17">

Se han establecido los valores adecuados para los parámetros en pitch_analyzer.h.

   * Puede serle útil seguir las instrucciones contenidas en el documento adjunto `código.pdf`.

- Una vez completados los puntos anteriores, dispondrá de una primera versión del estimador de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la estimación de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
		(r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
		autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).

		Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.

	    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
		en esta práctica es de 15 ms.

      - Use el estimador de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos estimadores.
     
		Aunque puede usar el propio Wavesurfer para obtener la representación, se valorará
	 	el uso de alternativas de mayor calidad (particularmente Python).

	<img width="883" alt="Captura de pantalla 2023-12-03 a las 22 41 51" src="https://github.com/juangarub/P3/assets/118903051/eacd16db-5f5d-4d9c-884d-9cb7c1a02d05">

En la gráfica superior podemos observar la señal de audio 'prueba.wav' junto con su potencia, pitch de wavesurfer y pitch obtenido por nosotros. Como se puede ver en los sonidos sonoros, tanto la potencia como el pitch tienen calores más elevados. Hemos tenido algunas complicaciones a la hora de representar el pitch creado por nosotros mismos

  
  * Optimice los parámetros de su sistema de estimación de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..

<img width="447" alt="Captura de pantalla 2023-12-04 034050" src="https://github.com/juangarub/P3/assets/84085478/364ff286-302a-4656-a68f-ce7be1a80eaf">

Este es el primer resultado de FScore conseguido.

En un intento de mejorarlo, hemos incorporado el center clipping como técnica de preprocesamiento y un filtro de mediana de tres elementos para el postprocesado.

En nuestro esfuerzo por aumentar la puntuación global, adoptamos una nueva aproximación para la función unvoiced y la enriquecimos con la tasa de cruces por cero. Para determinar si una sección de audio es sonora o sorda, hemos establecido cuatro criterios: el umbral basado en la potencia de la señal, el umbral basado en la autocorrelación normalizada de 1, el umbral basado en la autocorrelación en su punto máximo, y el umbral en los cruces por cero.

Así, si la potencia de la señal supera su umbral correspondiente, esto indica que la sección es sonora, dado que los sonidos sonoros poseen más energía que los sordos. Respecto a la autocorrelación normalizada, una autocorrelación cercana a cero apunta a una sección sorda. Si el máximo de la autocorrelación excede su umbral, sugiere periodicidad y, por ende, que la sección es sonora. Finalmente, en cuanto a los cruces por cero, un número bajo de estos sugiere que la sección es sonora, ya que los sonidos sonoros presentan una señal más constante y, por tanto, menos cruces por cero.

Este es el nuevo FScore, posterior a los ajustes realizados:

<img width="437" alt="Captura de pantalla 2023-12-04 033517" src="https://github.com/juangarub/P3/assets/84085478/0c0433eb-6b48-4a01-84ab-4ee1e89cb857">


Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  estimador a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del estimador. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la estimación de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.

<img width="638" alt="Captura de pantalla 2023-12-04 042938" src="https://github.com/juangarub/P3/assets/84085478/ab9cfb85-e603-4dad-98f0-ca78f580a223">


- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de estimación
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, diezmado, *center clipping*, etc.
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el estimador, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso](https://atenea.upc.edu/pluginfile.php/2908770/mod_resource/content/3/2b_PS%20Techniques.pdf)
  y en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al estimador. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.
   
Hemos elegido usar el center clipping como técnica de preprocesado y el filtro de mediana como técnica de postprocesado.

Este es el código de ambas con las explicaciones pertinentes:

<img width="312" alt="Captura de pantalla 2023-12-04 043505" src="https://github.com/juangarub/P3/assets/84085478/a9bb637a-df87-461f-867e-3ad202545d79">

Probamos con la aplicación de un límite específico, ajustando la señal hacia arriba o hacia abajo si supera un valor absoluto determinado. Luego, intentamos establecer un límite por debajo del cual, si la señal es inferior, la consideramos como cero. Observamos que el enfoque más simple es establecer un umbral único bajo el cual la señal se asume como cero. 

<img width="482" alt="Captura de pantalla 2023-12-04 043745" src="https://github.com/juangarub/P3/assets/84085478/c42f9292-18e6-4232-9e5a-75992b7f1de9">

Para el postprocesado, optamos por emplear un filtro de mediana con tres elementos. Este método nos ayuda a eliminar errores conocidos como dobles o de mitad. La implementación consiste en tomar una muestra, junto con la anterior y la siguiente, ordenarlas de menor a mayor y seleccionar la del medio. Este procedimiento se repite para todas las muestras de la señal. Dado que el pitch debe presentar variaciones sutiles, este postprocesado nos permite corregir cualquier cambio abrupto que surja.

Evaluación *ciega* del estimador
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
estimador con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
