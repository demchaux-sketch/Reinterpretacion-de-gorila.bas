DOCUMENTACIÓN DEL PROYECTO 

GORILLA.BAS REINTERPRETADO EN C++ 

Alumno: DEMIAN CHAUX MINCK 

Curso: Programacion-1 Ing. Informatica 

Fecha: 28/04/2026 

1\. DESCRIPCIÓN GENERAL 

Este proyecto es una reinterpretación en C++ del juego clásico Gorilla.bas. El juego original consistía en dos personajes ubicados sobre edificios que se lanzaban proyectiles por turnos, modificando ángulo y fuerza del disparo. 

En esta versión se desarrolló una interfaz gráfica estilo pixel art usando WinAPI/GDI, que son herramientas propias de Windows para crear ventanas, botones, cajas de texto, dibujos y animaciones. 

El juego permite: 

\- Jugar de a dos jugadores. 

\- Jugar contra una computadora. 

\- Ingresar nombres de jugadores. 

\- Ver una pantalla de reglas. 

\- Lanzar bananas con angulo y fuerza. 

\- Tener viento que modifica la trayectoria. 

\- Romper edificios cuando la banana impacta. 

\- Sumar puntos cuando la banana golpea al rival. 

\- Ganar la partida al llegar a 3 puntos. 

\- Ver una pantalla final de victoria. 

2\. LIBRERÍAS UTILIZADAS 

\#include \<windows.h\> 

Permite usar WinAPI. Se usa para crear la ventana, botones, cajas de texto, timers, sonidos con Beep, dibujo con GDI y manejo de eventos. 

\#include \<algorithm\> 

Se usa para funciones como min y max, que limitan valores como angulo y fuerza. 

\#include \<cmath\> 

Se usa para operaciones matemáticas como sin, cos, sqrt y round. Son necesarias para calcular la trayectoria del proyectil. 

\#include \<cstdlib\> 

Se usa para rand, srand y atoi.  
\#include \<ctime\> 

Se usa para time, que ayuda a inicializar numeros aleatorios. 

\#include \<string\> 

Permite usar string para guardar nombres y mensajes. 

\#include \<vector\> 

Permite usar vectores para guardar edificios y cráteres. 

3\. CONSTANTES PRINCIPALES 

VENTANA\_ANCHO y VENTANA\_ALTO: 

Definen el tamano de la ventana grafica. 

PANEL\_ALTO: 

Define la altura del panel superior donde aparecen mensajes, viento y marcador. 

SUELO\_Y: 

Indica la posición vertical del suelo. 

GRAVEDAD: 

Representa la fuerza de gravedad que hace caer la banana. 

PI: 

Se usa para convertir angulos de grados a radianes. 

4\. ESTRUCTURAS DEL PROGRAMA 

struct Edificio 

Guarda la posición, ancho, alto y color de cada edificio. 

struct Crater 

Representa una zona rota de un edificio. Guarda posición y radio del daño. 

struct Explosion 

Controla la animación cuando una banana impacta contra un edificio. 

struct ImpactoMono 

Controla la animación cuando una banana golpea a un jugador. 

struct Lanzamiento 

Controla la animación previa al disparo de la banana. 

struct Jugador 

Guarda nombre, posición y color del jugador.  
struct Proyectil 

Guarda posición actual, posición anterior, velocidad y si la banana está activa. La posición anterior se usa para detectar impactos aunque la banana se mueva rápido. 

struct Juego 

Es la estructura principal. Guarda todo el estado del juego: edificios, jugadores, puntajes, turno, viento, animaciones, modo de juego, pantalla actual y mensajes. 

5\. VARIABLES GLOBALES IMPORTANTES 

Juego juego; 

Es la variable principal que contiene todo el estado del juego. 

HWND etiquetaAngulo, entradaAngulo, botonDisparar, etc. 

Son controles gráficos de la ventana. HWND es un identificador que usa Windows para manejar botones, cajas de texto y etiquetas. 

6\. FUNCIONES DE UTILIDAD 

numeroAleatorio() 

Devuelve un numero aleatorio entre un minimo y un maximo. Se usa para generar edificios, viento y posiciones. 

escribirTexto() 

Cambia el texto de una caja o control de Windows. 

leerNumero() 

Lee un numero desde una caja de texto. Si el usuario no escribe nada valido, usa un valor por defecto. 

leerTexto() 

Lee el nombre de un jugador. Si está vacío, usa un nombre por defecto. 

mostrarControl() 

Muestra u oculta botones y cajas de texto. 

ocultarTodosLosControles() 

Oculta todos los controles para cambiar entre menús, juego, reglas y final. 

detenerTimers() 

Detiene todos los timers activos. Esto evita que queden animaciones viejas funcionando cuando se cambia de pantalla o se reinicia la ronda. 

sonidoDisparo(), sonidoExplosion(), sonidoPunto(), sonidoVictoria() Usan Beep para crear sonidos retro simples.  
7\. LOGICA DEL ESCENARIO 

crearCiudad() 

Genera edificios aleatorios con diferentes altos, anchos y colores. 

alturaEdificioEnX() 

Devuelve la altura del edificio que existe en una posicion horizontal determinada. 

puntoTieneEdificio() 

Determina si un punto pertenece a un edificio. Tambien revisa si ese punto no está dentro de un cráter. 

estaDentroDeCrater() 

Verifica si un punto está dentro de una zona destruida. 

ubicarJugadorSobreCiudad() 

Coloca al jugador sobre la parte superior de un edificio. 

8\. LOGICA DE RONDAS Y PARTIDA 

nuevaRonda() 

Crea una ciudad nueva, reinicia proyectil, explosiones y animaciones, coloca a los jugadores y prepara el turno inicial. 

comenzarJuego() 

Se ejecuta al elegir "Dos jugadores" o "Contra PC". Lee nombres, reinicia puntajes y empieza una nueva partida. 

volverAlMenu() 

Regresa al menú principal y detiene timers o animaciones pendientes. 

mostrarReglas() 

Cambia la pantalla actual para mostrar las reglas. 

pasarTurnoDespuesDeFallo() 

Cambia el turno cuando la banana falla o impacta contra un edificio. 

terminarDisparo() 

Se usa cuando la banana golpea a un jugador. Suma un punto al atacante, activa la animacion de impacto y verifica si alguien llego a 3 puntos. 

9\. FISICA DEL DISPARO 

El disparo se calcula usando:  
\- Angulo. 

\- Fuerza. 

\- Gravedad. 

\- Viento. 

\- Tiempo. 

La funcion lanzarProyectil() convierte el ángulo a radianes y calcula la velocidad horizontal y vertical usando coseno y seno. 

En actualizarFisica(), la banana se mueve en cada frame: 

\- La velocidad horizontal cambia por el viento. 

\- La velocidad vertical cambia por la gravedad. 

\- La posicion se actualiza sumando velocidad por tiempo. 

Si la banana sale de la pantalla, el disparo termina. 

Si toca un edificio, se genera una explosion. 

Si toca al rival, se suma punto. 

10\. DETECCION DE IMPACTOS 

proyectilTocaJugador() 

Detecta si la banana golpea al jugador rival. No usa solo un punto, sino una zona rectangular amplia que cubre el sprite pixel art del monito. 

segmentoTocaRectangulo() 

Revisa la línea entre la posición anterior y la posición actual de la banana. Esto evita que una banana rápida atraviese al jugador sin detectar impacto. 

proyectilTocaEdificio() 

Verifica si la banana impactó contra una parte sólida de un edificio. 

11\. DESTRUCCION DE EDIFICIOS 

iniciarExplosionEdificio() 

Activa una explosion pixel art cuando la banana golpea un edificio. 

actualizarExplosion() 

Aumenta el radio de la explosion hasta llegar al tamaño final. Cuando termina, guarda un cráter en el vector de cráteres. 

Los cráteres hacen que esa zona del edificio quede visualmente rota y también deje de contar como una pared sólida. 

12\. COMPUTADORA  
calcularDisparoComputadora() 

Prueba diferentes combinaciones de ángulo y fuerza para encontrar un disparo que se acerque al jugador humano. 

Luego agrega un pequeño margen aleatorio para que la computadora no sea perfecta. 

dispararComputadora() 

Ejecuta el disparo automático cuando es turno de la computadora. 

13\. DIBUJO E INTERFAZ GRAFICA 

El juego usa GDI para dibujar: 

\- Fondo pixel art. 

\- Edificios. 

\- Monitos. 

\- Banana. 

\- Explosiones. 

\- Cráteres. 

\- Pantalla de menú. 

\- Pantalla de reglas. 

\- Pantalla final. 

dibujarJuego() 

Decide que pantalla dibujar según el estado actual: 

\- Menú. 

\- Reglas. 

\- Partida. 

\- Pantalla final. 

dibujarJugador() 

Dibuja el monito en estilo pixel art usando rectangulos. 

dibujarBanana() 

Dibuja la banana como proyectil pixel art. 

dibujarCiudad() 

Dibuja edificios, ventanas, crateres y explosiones. 

dibujarPantallaFinal() 

Muestra el ganador de la partida y el resultado final. 

14\. EVENTOS DE VENTANA  
procedimientoVentana() 

Es la funcion central de eventos de WinAPI. 

Maneja: 

\- WM\_CREATE: crea botones, cajas de texto y controles. 

\- WM\_COMMAND: responde a clicks en botones. 

\- WM\_TIMER: actualiza fisica y animaciones. 

\- WM\_PAINT: redibuja la pantalla. 

\- WM\_DESTROY: cierra el programa. 

Para evitar titilado, el dibujo se hace con doble buffer: 

primero se dibuja en memoria y luego se copia todo junto a la ventana con BitBlt. 

15\. MODIFICACIONES MANUALES REALIZADAS 

Durante el desarrollo se agregaron varias mejoras: 

\- Interfaz grafica con WinAPI. 

\- Estilo pixel art. 

\- Jugadores dibujados como monitos. 

\- Banana como proyectil. 

\- Menu con modos de juego. 

\- Pantalla de reglas. 

\- Nombres personalizados. 

\- Modo contra computadora. 

\- Puntaje acumulado. 

\- Victoria al llegar a 3 puntos. 

\- Pantalla final. 

\- Sonidos retro. 

\- Animacion de lanzamiento. 

\- Animacion de impacto contra jugador. 

\- Explosion y destruccion de edificios. 

\- Correccion de bugs de timers y colisiones. 

16\. COMO COMPILAR 

Si el archivo se llama gorila.bas.cpp: 

g++ \-Wall \-Wextra gorila.bas.cpp \-o gorila.bas.exe \-mwindows \-lgdi32 \-luser32 Si el archivo se llama gorila.bas: 

g++ \-x c++ \-Wall \-Wextra gorila.bas \-o gorila.bas.exe \-mwindows \-lgdi32 \-luser32  
17\. COMO ENTREGAR O COMPARTIR 

Para entregar el trabajo se pueden subir: 

\- gorila.bas.cpp: codigo fuente. 

\- gorila.bas.exe: ejecutable. 

\- documentacion\_gorila\_bas.txt: este archivo de explicacion. 

Para compartir solo el juego con otra persona, alcanza con enviar el archivo: gorila.bas.exe 

18\. CONCLUSION 

Este proyecto demuestra como un juego clásico de artillería por turnos puede re interpretarse en C++ usando estructuras, funciones, vectores, calculos fisicos, eventos gráficos, timers, animaciones y detección de colisiones. 

El código no solo busca que el juego funcione, sino también que pueda analizarse y explicarse por partes: interfaz, física, lógica de turnos, IA, dibujo y estado general de la partida.