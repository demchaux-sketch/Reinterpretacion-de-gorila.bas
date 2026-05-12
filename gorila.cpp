#include <windows.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

using namespace std;

// Constantes generales de ventana, escenario y fisica.
const int VENTANA_ANCHO = 900;
const int VENTANA_ALTO = 620;
const int PANEL_ALTO = 82;
const int SUELO_Y = 560;
const double GRAVEDAD = 240.0;
const double PI = 3.14159265358979323846;

// Estructuras que guardan el estado del juego.
struct Edificio {
    int x;
    int ancho;
    int alto;
    COLORREF color;
};

struct Crater {
    int x;
    int y;
    int radio;
};

struct Explosion {
    bool activa;
    int x;
    int y;
    int radio;
    int radioFinal;
};

struct ImpactoMono {
    bool activo;
    int x;
    int y;
    int frame;
};

struct Lanzamiento {
    bool activo;
    int frame;
    int angulo;
    int fuerza;
};

struct Jugador {
    string nombre;
    int x;
    int y;
    COLORREF color;
};

struct Proyectil {
    double x;
    double y;
    double anteriorX;
    double anteriorY;
    double vx;
    double vy;
    bool activo;
};

struct Juego {
    vector<Edificio> edificios;
    vector<Crater> crateres;
    Jugador jugadores[2];
    Proyectil proyectil;
    Explosion explosion;
    ImpactoMono impactoMono;
    Lanzamiento lanzamiento;
    int turno;
    int viento;
    int puntos[2];
    int ganadorPartida;
    bool enMenu;
    bool contraComputadora;
    bool rondaTerminada;
    bool partidaTerminada;
    bool pantallaFinal;
    bool mostrandoReglas;
    string nombres[2];
    string mensaje;
};

Juego juego;

// Controles de la interfaz grafica WinAPI.
HWND etiquetaAngulo;
HWND etiquetaFuerza;
HWND entradaAngulo;
HWND entradaVelocidad;
HWND botonDisparar;
HWND botonReiniciar;
HWND botonDosJugadores;
HWND botonComputadora;
HWND etiquetaNombre1;
HWND etiquetaNombre2;
HWND entradaNombre1;
HWND entradaNombre2;
HWND botonRevancha;
HWND botonVolverMenu;
HWND botonReglas;
HWND botonVolverReglas;

// Utilidades generales de entrada, sonido y controles.
int numeroAleatorio(int minimo, int maximo) {
    return minimo + rand() % (maximo - minimo + 1);
}

void escribirTexto(HWND control, const string& texto) {
    SetWindowTextA(control, texto.c_str());
}

int leerNumero(HWND control, int valorPorDefecto) {
    char buffer[32];
    GetWindowTextA(control, buffer, sizeof(buffer));

    int valor = atoi(buffer);
    if (valor == 0 && buffer[0] != '0') {
        return valorPorDefecto;
    }

    return valor;
}

string leerTexto(HWND control, const string& valorPorDefecto) {
    char buffer[64];
    GetWindowTextA(control, buffer, sizeof(buffer));

    string texto = buffer;
    if (texto.empty()) {
        return valorPorDefecto;
    }

    return texto;
}

void mostrarControl(HWND control, bool visible) {
    ShowWindow(control, visible ? SW_SHOW : SW_HIDE);
}

void ocultarTodosLosControles() {
    mostrarControl(etiquetaAngulo, false);
    mostrarControl(etiquetaFuerza, false);
    mostrarControl(entradaAngulo, false);
    mostrarControl(entradaVelocidad, false);
    mostrarControl(botonDisparar, false);
    mostrarControl(botonReiniciar, false);
    mostrarControl(botonDosJugadores, false);
    mostrarControl(botonComputadora, false);
    mostrarControl(etiquetaNombre1, false);
    mostrarControl(etiquetaNombre2, false);
    mostrarControl(entradaNombre1, false);
    mostrarControl(entradaNombre2, false);
    mostrarControl(botonRevancha, false);
    mostrarControl(botonVolverMenu, false);
    mostrarControl(botonReglas, false);
    mostrarControl(botonVolverReglas, false);
}

void mostrarMenuControles() {
    ocultarTodosLosControles();
    mostrarControl(entradaNombre1, true);
    mostrarControl(entradaNombre2, true);
    mostrarControl(botonDosJugadores, true);
    mostrarControl(botonComputadora, true);
    mostrarControl(botonReglas, true);
}

void mostrarControlesReglas() {
    ocultarTodosLosControles();
    mostrarControl(botonVolverReglas, true);
}

void mostrarControlesJuego() {
    ocultarTodosLosControles();
    mostrarControl(etiquetaAngulo, true);
    mostrarControl(etiquetaFuerza, true);
    mostrarControl(entradaAngulo, true);
    mostrarControl(entradaVelocidad, true);
    mostrarControl(botonDisparar, true);
    mostrarControl(botonReiniciar, true);
}

void mostrarControlesFinal() {
    ocultarTodosLosControles();
    mostrarControl(botonRevancha, true);
    mostrarControl(botonVolverMenu, true);
}

void limpiarEntradasDisparo() {
    escribirTexto(entradaAngulo, "");
    escribirTexto(entradaVelocidad, "");
}

void detenerTimers(HWND ventana) {
    KillTimer(ventana, 1);
    KillTimer(ventana, 2);
    KillTimer(ventana, 3);
    KillTimer(ventana, 4);
    KillTimer(ventana, 5);
    KillTimer(ventana, 6);
    KillTimer(ventana, 7);
}

void sonidoDisparo() {
    Beep(880, 45);
    Beep(1175, 45);
}

void sonidoExplosion() {
    Beep(180, 70);
    Beep(120, 90);
}

void sonidoPunto() {
    Beep(660, 70);
    Beep(880, 70);
    Beep(1320, 90);
}

void sonidoVictoria() {
    Beep(784, 90);
    Beep(988, 90);
    Beep(1175, 130);
}

// Logica del escenario, turnos, disparos, impactos e IA.
int alturaEdificioEnX(const vector<Edificio>& edificios, int x) {
    for (size_t i = 0; i < edificios.size(); i++) {
        const Edificio& edificio = edificios[i];
        if (x >= edificio.x && x < edificio.x + edificio.ancho) {
            return edificio.alto;
        }
    }

    return 0;
}

bool estaDentroDeCrater(int x, int y, const Crater& crater) {
    int dx = x - crater.x;
    int dy = y - crater.y;
    return dx * dx + dy * dy <= crater.radio * crater.radio;
}

bool puntoTieneEdificio(int x, int y) {
    int altura = alturaEdificioEnX(juego.edificios, x);

    if (altura <= 0 || y < SUELO_Y - altura || y > SUELO_Y) {
        return false;
    }

    for (size_t i = 0; i < juego.crateres.size(); i++) {
        if (estaDentroDeCrater(x, y, juego.crateres[i])) {
            return false;
        }
    }

    return true;
}

void ubicarJugadorSobreCiudad(Jugador& jugador) {
    int altura = alturaEdificioEnX(juego.edificios, jugador.x);
    jugador.y = SUELO_Y - altura - 14;
}

void crearCiudad() {
    juego.edificios.clear();
    juego.crateres.clear();

    int x = 0;
    COLORREF colores[] = {
        RGB(58, 91, 133),
        RGB(82, 112, 122),
        RGB(119, 91, 96),
        RGB(70, 124, 104),
        RGB(98, 98, 137)
    };

    while (x < VENTANA_ANCHO) {
        Edificio edificio;
        edificio.x = x;
        edificio.ancho = numeroAleatorio(42, 72);
        edificio.alto = numeroAleatorio(120, 350);
        edificio.color = colores[numeroAleatorio(0, 4)];
        juego.edificios.push_back(edificio);
        x += edificio.ancho;
    }
}

void nuevaRonda() {
    crearCiudad();

    juego.turno = 0;
    juego.viento = numeroAleatorio(-35, 35);
    juego.proyectil = {0, 0, 0, 0, 0, 0, false};
    juego.explosion = {false, 0, 0, 0, 0};
    juego.impactoMono = {false, 0, 0, 0};
    juego.lanzamiento = {false, 0, 0, 0};
    juego.rondaTerminada = false;
    juego.pantallaFinal = false;
    juego.mostrandoReglas = false;

    juego.jugadores[0] = {juego.nombres[0], numeroAleatorio(70, 180), 0, RGB(245, 90, 82)};
    juego.jugadores[1] = {
        juego.nombres[1],
        numeroAleatorio(700, 830),
        0,
        RGB(95, 170, 255)
    };
    ubicarJugadorSobreCiudad(juego.jugadores[0]);
    ubicarJugadorSobreCiudad(juego.jugadores[1]);

    juego.mensaje = "Turno de " + juego.jugadores[0].nombre + ". Ingrese angulo y fuerza.";

    limpiarEntradasDisparo();
    EnableWindow(botonDisparar, TRUE);
}

void comenzarJuego(HWND ventana, bool contraComputadora) {
    detenerTimers(ventana);
    juego.enMenu = false;
    juego.contraComputadora = contraComputadora;
    juego.partidaTerminada = false;
    juego.pantallaFinal = false;
    juego.mostrandoReglas = false;
    juego.ganadorPartida = -1;
    juego.puntos[0] = 0;
    juego.puntos[1] = 0;
    juego.nombres[0] = leerTexto(entradaNombre1, "Jugador 1");
    juego.nombres[1] = contraComputadora ? "Computadora" : leerTexto(entradaNombre2, "Jugador 2");
    mostrarControlesJuego();
    nuevaRonda();
    InvalidateRect(ventana, NULL, FALSE);
}

void volverAlMenu(HWND ventana) {
    detenerTimers(ventana);
    juego.enMenu = true;
    juego.partidaTerminada = false;
    juego.pantallaFinal = false;
    juego.mostrandoReglas = false;
    juego.rondaTerminada = false;
    juego.proyectil.activo = false;
    juego.explosion.activa = false;
    juego.impactoMono.activo = false;
    juego.lanzamiento.activo = false;
    mostrarMenuControles();
    InvalidateRect(ventana, NULL, FALSE);
}

void mostrarReglas(HWND ventana) {
    detenerTimers(ventana);
    juego.enMenu = true;
    juego.mostrandoReglas = true;
    mostrarControlesReglas();
    InvalidateRect(ventana, NULL, FALSE);
}

bool puntoEnRectangulo(double x, double y, int izquierda, int arriba, int derecha, int abajo) {
    return x >= izquierda && x <= derecha && y >= arriba && y <= abajo;
}

bool segmentoTocaRectangulo(double x1, double y1, double x2, double y2,
                            int izquierda, int arriba, int derecha, int abajo) {
    for (int i = 0; i <= 24; i++) {
        double t = i / 24.0;
        double x = x1 + (x2 - x1) * t;
        double y = y1 + (y2 - y1) * t;

        if (puntoEnRectangulo(x, y, izquierda, arriba, derecha, abajo)) {
            return true;
        }
    }

    return false;
}

bool proyectilTocaJugador(const Jugador& jugador) {
    int spriteX = jugador.x - 28;
    int spriteY = jugador.y - 54;

    // El sprite pixel art ocupa cabeza, cuerpo, brazos, patas y cola.
    int izquierda = spriteX - 20;
    int arriba = spriteY - 10;
    int derecha = spriteX + 86;
    int abajo = spriteY + 96;

    // La banana no es un punto: se dibuja aproximadamente de x-16 a x+23.
    int margenBanana = 18;
    izquierda -= margenBanana;
    arriba -= margenBanana;
    derecha += margenBanana;
    abajo += margenBanana;

    return segmentoTocaRectangulo(
        juego.proyectil.anteriorX,
        juego.proyectil.anteriorY,
        juego.proyectil.x,
        juego.proyectil.y,
        izquierda,
        arriba,
        derecha,
        abajo
    );
}

bool proyectilTocaEdificio() {
    int x = static_cast<int>(round(juego.proyectil.x));
    int y = static_cast<int>(round(juego.proyectil.y));

    return puntoTieneEdificio(x, y);
}

void pasarTurnoDespuesDeFallo(HWND ventana) {
    int defensor = 1 - juego.turno;

    juego.turno = defensor;
    juego.mensaje = "Turno de " + juego.jugadores[juego.turno].nombre + ". Ajuste el disparo.";
    EnableWindow(botonDisparar, !(juego.contraComputadora && juego.turno == 1));

    if (juego.contraComputadora && juego.turno == 1) {
        juego.mensaje = "La computadora esta calculando su disparo...";
        SetTimer(ventana, 2, 900, NULL);
    }
}

void terminarDisparo(HWND ventana, bool acierto) {
    KillTimer(ventana, 1);
    KillTimer(ventana, 6);
    juego.proyectil.activo = false;
    juego.lanzamiento.activo = false;

    int atacante = juego.turno;

    if (acierto) {
        juego.puntos[atacante]++;
        juego.mensaje = juego.jugadores[atacante].nombre + " impacto al rival y gano la ronda.";
        juego.rondaTerminada = true;
        juego.impactoMono = {true, juego.jugadores[1 - atacante].x, juego.jugadores[1 - atacante].y - 18, 0};
        EnableWindow(botonDisparar, FALSE);
        sonidoPunto();
        SetTimer(ventana, 5, 60, NULL);

        if (juego.puntos[atacante] >= 3) {
            juego.partidaTerminada = true;
            juego.ganadorPartida = atacante;
            juego.mensaje = juego.jugadores[atacante].nombre + " gano la partida.";
            SetTimer(ventana, 4, 1700, NULL);
        } else {
            SetTimer(ventana, 4, 1400, NULL);
        }
    } else {
        pasarTurnoDespuesDeFallo(ventana);
    }
}

void iniciarExplosionEdificio(HWND ventana) {
    KillTimer(ventana, 1);
    KillTimer(ventana, 6);
    juego.proyectil.activo = false;
    juego.lanzamiento.activo = false;
    juego.explosion.activa = true;
    juego.explosion.x = static_cast<int>(round(juego.proyectil.x));
    juego.explosion.y = static_cast<int>(round(juego.proyectil.y));
    juego.explosion.radio = 4;
    juego.explosion.radioFinal = 34;
    juego.mensaje = "La banana rompio un edificio.";
    EnableWindow(botonDisparar, FALSE);
    sonidoExplosion();
    SetTimer(ventana, 3, 45, NULL);
}

void actualizarExplosion(HWND ventana) {
    if (!juego.explosion.activa) {
        return;
    }

    juego.explosion.radio += 5;

    if (juego.explosion.radio >= juego.explosion.radioFinal) {
        juego.explosion.activa = false;
        juego.crateres.push_back({juego.explosion.x, juego.explosion.y, juego.explosion.radioFinal});
        KillTimer(ventana, 3);
        pasarTurnoDespuesDeFallo(ventana);
    }
}

void actualizarImpactoMono(HWND ventana) {
    if (!juego.impactoMono.activo) {
        return;
    }

    juego.impactoMono.frame++;

    if (juego.impactoMono.frame > 18) {
        juego.impactoMono.activo = false;
        KillTimer(ventana, 5);
    }
}

void lanzarProyectil(HWND ventana, int angulo, int velocidad) {
    if (juego.proyectil.activo || juego.rondaTerminada || juego.enMenu || juego.pantallaFinal) {
        return;
    }

    KillTimer(ventana, 1);
    KillTimer(ventana, 6);

    Jugador& atacante = juego.jugadores[juego.turno];
    int direccion = (juego.turno == 0) ? 1 : -1;
    double radianes = angulo * PI / 180.0;
    double escala = 4.3;
    int frameFinal = 7;

    // El proyectil nace donde termina la animacion de lanzamiento para evitar saltos visuales.
    juego.proyectil.x = atacante.x + direccion * (28 + frameFinal * 5);
    juego.proyectil.y = atacante.y - 38 - frameFinal * 4;
    juego.proyectil.anteriorX = juego.proyectil.x;
    juego.proyectil.anteriorY = juego.proyectil.y;
    juego.proyectil.vx = direccion * velocidad * escala * cos(radianes);
    juego.proyectil.vy = -velocidad * escala * sin(radianes);
    juego.proyectil.activo = true;
    juego.lanzamiento.activo = false;
    juego.mensaje = "Banana en vuelo...";

    sonidoDisparo();
    SetTimer(ventana, 1, 16, NULL);
}

void iniciarDisparoConValores(HWND ventana, int angulo, int velocidad) {
    if (juego.proyectil.activo || juego.lanzamiento.activo || juego.rondaTerminada || juego.enMenu) {
        return;
    }

    angulo = max(0, min(90, angulo));
    velocidad = max(10, min(120, velocidad));

    juego.lanzamiento = {true, 0, angulo, velocidad};
    juego.mensaje = juego.jugadores[juego.turno].nombre + " prepara el lanzamiento...";
    EnableWindow(botonDisparar, FALSE);
    SetTimer(ventana, 6, 70, NULL);
}

void actualizarLanzamiento(HWND ventana) {
    if (!juego.lanzamiento.activo) {
        return;
    }

    juego.lanzamiento.frame++;

    if (juego.lanzamiento.frame >= 7) {
        int angulo = juego.lanzamiento.angulo;
        int fuerza = juego.lanzamiento.fuerza;
        juego.lanzamiento.activo = false;
        KillTimer(ventana, 6);
        lanzarProyectil(ventana, angulo, fuerza);
    }
}

void iniciarDisparo(HWND ventana) {
    if (juego.contraComputadora && juego.turno == 1) {
        return;
    }

    int angulo = leerNumero(entradaAngulo, 45);
    int velocidad = leerNumero(entradaVelocidad, 60);
    limpiarEntradasDisparo();
    iniciarDisparoConValores(ventana, angulo, velocidad);
}

double distanciaAlObjetivo(int atacante, int angulo, int velocidad) {
    int defensor = 1 - atacante;
    int direccion = (atacante == 0) ? 1 : -1;
    double radianes = angulo * PI / 180.0;
    double escala = 4.3;
    double x = juego.jugadores[atacante].x;
    double y = juego.jugadores[atacante].y - 10;
    double vx = direccion * velocidad * escala * cos(radianes);
    double vy = -velocidad * escala * sin(radianes);
    double mejor = 999999.0;

    for (int paso = 0; paso < 520; paso++) {
        double dt = 0.016;
        vx += juego.viento * dt;
        vy += GRAVEDAD * dt;
        x += vx * dt;
        y += vy * dt;

        double dx = x - juego.jugadores[defensor].x;
        double dy = y - juego.jugadores[defensor].y;
        mejor = min(mejor, sqrt(dx * dx + dy * dy));

        if (x < 0 || x > VENTANA_ANCHO || y > SUELO_Y) {
            break;
        }
    }

    return mejor;
}

void calcularDisparoComputadora(int& angulo, int& fuerza) {
    double mejorDistancia = 999999.0;
    angulo = 45;
    fuerza = 60;

    for (int a = 18; a <= 76; a += 2) {
        for (int f = 25; f <= 115; f += 3) {
            double distancia = distanciaAlObjetivo(1, a, f);
            if (distancia < mejorDistancia) {
                mejorDistancia = distancia;
                angulo = a;
                fuerza = f;
            }
        }
    }

    angulo = max(0, min(90, angulo + numeroAleatorio(-4, 4)));
    fuerza = max(10, min(120, fuerza + numeroAleatorio(-7, 7)));
}

void dispararComputadora(HWND ventana) {
    int angulo;
    int fuerza;
    calcularDisparoComputadora(angulo, fuerza);
    juego.mensaje = "Computadora dispara con angulo " + to_string(angulo) + " y fuerza " + to_string(fuerza) + ".";
    iniciarDisparoConValores(ventana, angulo, fuerza);
}

void actualizarFisica(HWND ventana) {
    if (!juego.proyectil.activo) {
        return;
    }

    double dt = 0.016;
    juego.proyectil.anteriorX = juego.proyectil.x;
    juego.proyectil.anteriorY = juego.proyectil.y;
    juego.proyectil.vx += juego.viento * dt;
    juego.proyectil.vy += GRAVEDAD * dt;
    juego.proyectil.x += juego.proyectil.vx * dt;
    juego.proyectil.y += juego.proyectil.vy * dt;

    int defensor = 1 - juego.turno;

    if (proyectilTocaJugador(juego.jugadores[defensor])) {
        terminarDisparo(ventana, true);
        return;
    }

    if (juego.proyectil.x < 0 || juego.proyectil.x > VENTANA_ANCHO || juego.proyectil.y > SUELO_Y) {
        terminarDisparo(ventana, false);
        return;
    }

    if (proyectilTocaEdificio()) {
        iniciarExplosionEdificio(ventana);
    }
}

// Funciones de dibujo pixel art.
void dibujarTexto(HDC hdc, int x, int y, const string& texto, COLORREF color = RGB(25, 31, 40)) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    TextOutA(hdc, x, y, texto.c_str(), static_cast<int>(texto.size()));
}

void rellenarRectangulo(HDC hdc, int izquierda, int arriba, int derecha, int abajo, COLORREF color) {
    HBRUSH pincel = CreateSolidBrush(color);
    RECT rect = {izquierda, arriba, derecha, abajo};
    FillRect(hdc, &rect, pincel);
    DeleteObject(pincel);
}

void dibujarBloque(HDC hdc, int x, int y, int tamano, COLORREF color) {
    rellenarRectangulo(hdc, x, y, x + tamano, y + tamano, color);
}

void dibujarRectPixel(HDC hdc, int x, int y, int ancho, int alto, COLORREF color) {
    rellenarRectangulo(hdc, x, y, x + ancho, y + alto, color);
}

COLORREF colorFondoEnY(int y) {
    if (y < PANEL_ALTO) {
        return RGB(31, 35, 52);
    }
    if (y < 200) {
        return RGB(95, 144, 181);
    }
    if (y < 365) {
        return RGB(73, 104, 143);
    }
    if (y < SUELO_Y) {
        return RGB(58, 75, 110);
    }

    return RGB(31, 47, 39);
}

void dibujarJugador(HDC hdc, const Jugador& jugador) {
    int x = jugador.x - 28;
    int y = jugador.y - 54;
    COLORREF borde = RGB(24, 18, 14);
    COLORREF sombra = RGB(78, 45, 22);
    COLORREF cuerpo = RGB(133, 82, 39);
    COLORREF piel = RGB(239, 183, 111);
    COLORREF pielSombra = RGB(213, 144, 78);
    COLORREF suelo = RGB(39, 44, 36);

    dibujarRectPixel(hdc, x + 9, y + 86, 38, 5, suelo);

    // Cola pixelada curvada hacia la derecha.
    dibujarRectPixel(hdc, x + 48, y + 56, 20, 7, borde);
    dibujarRectPixel(hdc, x + 63, y + 48, 8, 15, borde);
    dibujarRectPixel(hdc, x + 69, y + 38, 7, 16, borde);
    dibujarRectPixel(hdc, x + 74, y + 27, 6, 15, borde);
    dibujarRectPixel(hdc, x + 54, y + 58, 13, 4, cuerpo);
    dibujarRectPixel(hdc, x + 66, y + 49, 4, 12, cuerpo);
    dibujarRectPixel(hdc, x + 72, y + 39, 3, 13, cuerpo);

    // Orejas.
    dibujarRectPixel(hdc, x - 2, y + 32, 12, 18, borde);
    dibujarRectPixel(hdc, x + 46, y + 32, 12, 18, borde);
    dibujarRectPixel(hdc, x + 1, y + 36, 9, 10, cuerpo);
    dibujarRectPixel(hdc, x + 46, y + 36, 9, 10, cuerpo);

    // Cabeza grande con silueta escalonada.
    dibujarRectPixel(hdc, x + 18, y + 2, 20, 6, borde);
    dibujarRectPixel(hdc, x + 12, y + 8, 32, 7, borde);
    dibujarRectPixel(hdc, x + 8, y + 15, 40, 10, borde);
    dibujarRectPixel(hdc, x + 5, y + 25, 46, 28, borde);
    dibujarRectPixel(hdc, x + 10, y + 13, 36, 11, cuerpo);
    dibujarRectPixel(hdc, x + 8, y + 24, 40, 27, cuerpo);
    dibujarRectPixel(hdc, x + 15, y + 0, 24, 7, cuerpo);
    dibujarRectPixel(hdc, x + 22, y - 5, 11, 5, borde);

    // Cara clara.
    dibujarRectPixel(hdc, x + 15, y + 21, 26, 22, piel);
    dibujarRectPixel(hdc, x + 12, y + 29, 32, 17, piel);
    dibujarRectPixel(hdc, x + 16, y + 43, 24, 8, piel);
    dibujarRectPixel(hdc, x + 36, y + 25, 5, 18, pielSombra);

    // Ojos, nariz y boca.
    dibujarRectPixel(hdc, x + 20, y + 31, 6, 6, borde);
    dibujarRectPixel(hdc, x + 33, y + 31, 6, 6, borde);
    dibujarRectPixel(hdc, x + 24, y + 28, 3, 3, RGB(255, 234, 183));
    dibujarRectPixel(hdc, x + 37, y + 28, 3, 3, RGB(255, 234, 183));
    dibujarRectPixel(hdc, x + 28, y + 41, 4, 4, borde);
    dibujarRectPixel(hdc, x + 28, y + 51, 8, 4, borde);

    // Cuerpo, brazos y piernas.
    dibujarRectPixel(hdc, x + 13, y + 54, 31, 28, borde);
    dibujarRectPixel(hdc, x + 17, y + 56, 23, 25, cuerpo);
    dibujarRectPixel(hdc, x + 22, y + 60, 14, 17, piel);
    dibujarRectPixel(hdc, x + 37, y + 59, 5, 19, sombra);
    dibujarRectPixel(hdc, x + 6, y + 58, 10, 22, borde);
    dibujarRectPixel(hdc, x + 42, y + 58, 10, 22, borde);
    dibujarRectPixel(hdc, x + 9, y + 61, 7, 15, cuerpo);
    dibujarRectPixel(hdc, x + 42, y + 61, 7, 15, cuerpo);
    dibujarRectPixel(hdc, x + 4, y + 78, 12, 8, piel);
    dibujarRectPixel(hdc, x + 42, y + 78, 12, 8, piel);
    dibujarRectPixel(hdc, x + 17, y + 80, 10, 10, borde);
    dibujarRectPixel(hdc, x + 32, y + 80, 10, 10, borde);
    dibujarRectPixel(hdc, x + 19, y + 80, 7, 7, cuerpo);
    dibujarRectPixel(hdc, x + 33, y + 80, 7, 7, cuerpo);

    dibujarTexto(hdc, jugador.x - 30, jugador.y - 64, jugador.nombre, RGB(240, 232, 190));
}

void dibujarBanana(HDC hdc, int x, int y) {
    COLORREF borde = RGB(112, 78, 18);
    COLORREF amarillo = RGB(255, 219, 67);
    COLORREF brillo = RGB(255, 242, 130);

    dibujarRectPixel(hdc, x - 16, y + 4, 6, 5, borde);
    dibujarRectPixel(hdc, x - 10, y, 8, 5, amarillo);
    dibujarRectPixel(hdc, x - 2, y - 4, 14, 5, amarillo);
    dibujarRectPixel(hdc, x + 10, y - 1, 8, 5, amarillo);
    dibujarRectPixel(hdc, x + 18, y - 5, 5, 5, borde);
    dibujarRectPixel(hdc, x - 4, y - 2, 10, 2, brillo);
}

void dibujarImpactoMono(HDC hdc) {
    if (!juego.impactoMono.activo) {
        return;
    }

    int x = juego.impactoMono.x;
    int y = juego.impactoMono.y;
    int frame = juego.impactoMono.frame;
    int radio = 10 + frame * 3;
    COLORREF amarillo = RGB(255, 236, 94);
    COLORREF naranja = RGB(238, 116, 55);
    COLORREF blanco = RGB(255, 248, 202);

    dibujarRectPixel(hdc, x - radio, y - 4, radio * 2, 8, amarillo);
    dibujarRectPixel(hdc, x - 4, y - radio, 8, radio * 2, amarillo);
    dibujarRectPixel(hdc, x - radio / 2, y - radio / 2, radio, radio, naranja);

    dibujarRectPixel(hdc, x - 8, y - 8, 16, 16, blanco);
    dibujarRectPixel(hdc, x - radio - 10, y - radio / 2, 8, 8, amarillo);
    dibujarRectPixel(hdc, x + radio + 2, y - radio / 3, 8, 8, amarillo);
    dibujarRectPixel(hdc, x - radio / 3, y - radio - 10, 8, 8, blanco);
    dibujarRectPixel(hdc, x + radio / 2, y + radio + 2, 8, 8, naranja);
}

void dibujarAnimacionLanzamiento(HDC hdc) {
    if (!juego.lanzamiento.activo) {
        return;
    }

    const Jugador& jugador = juego.jugadores[juego.turno];
    int direccion = (juego.turno == 0) ? 1 : -1;
    int frame = juego.lanzamiento.frame;
    int frameVisual = min(frame, 6);
    int x = jugador.x + direccion * (28 + frameVisual * 5);
    int y = jugador.y - 38 - frameVisual * 4;

    if (direccion == 1) {
        dibujarRectPixel(hdc, jugador.x + 18, jugador.y - 30, 24, 8, RGB(24, 18, 14));
    } else {
        dibujarRectPixel(hdc, jugador.x - 42, jugador.y - 30, 24, 8, RGB(24, 18, 14));
    }
    dibujarBanana(hdc, x, y);
}

void dibujarCiudad(HDC hdc) {
    for (size_t i = 0; i < juego.edificios.size(); i++) {
        const Edificio& edificio = juego.edificios[i];
        int arriba = SUELO_Y - edificio.alto;

        dibujarRectPixel(hdc, edificio.x, arriba, edificio.ancho - 1, edificio.alto, RGB(27, 31, 47));
        dibujarRectPixel(hdc, edificio.x + 4, arriba + 4, edificio.ancho - 8, edificio.alto - 4, edificio.color);
        dibujarRectPixel(hdc, edificio.x + 4, arriba + 4, edificio.ancho - 8, 8, RGB(157, 181, 196));

        for (int y = arriba + 22; y < SUELO_Y - 16; y += 26) {
            for (int x = edificio.x + 12; x < edificio.x + edificio.ancho - 14; x += 22) {
                COLORREF ventana = ((x + y) % 3 == 0) ? RGB(255, 220, 105) : RGB(44, 54, 76);
                dibujarRectPixel(hdc, x, y, 9, 12, ventana);
            }
        }
    }

    vector<Crater> crateresVisibles = juego.crateres;
    if (juego.explosion.activa) {
        crateresVisibles.push_back({juego.explosion.x, juego.explosion.y, juego.explosion.radio});
    }

    for (size_t i = 0; i < crateresVisibles.size(); i++) {
        const Crater& crater = crateresVisibles[i];

        for (int y = crater.y - crater.radio; y <= crater.y + crater.radio; y += 6) {
            for (int x = crater.x - crater.radio; x <= crater.x + crater.radio; x += 6) {
                if (estaDentroDeCrater(x, y, crater)) {
                    dibujarRectPixel(hdc, x, y, 6, 6, colorFondoEnY(y));
                }
            }
        }

        dibujarRectPixel(hdc, crater.x - crater.radio, crater.y, 8, 6, RGB(22, 24, 30));
        dibujarRectPixel(hdc, crater.x + crater.radio - 8, crater.y - 4, 8, 6, RGB(22, 24, 30));
        dibujarRectPixel(hdc, crater.x - 6, crater.y + crater.radio - 8, 12, 6, RGB(22, 24, 30));
    }

    if (juego.explosion.activa) {
        int r = juego.explosion.radio;
        int x = juego.explosion.x;
        int y = juego.explosion.y;
        dibujarRectPixel(hdc, x - r, y - 4, r * 2, 8, RGB(255, 236, 94));
        dibujarRectPixel(hdc, x - 4, y - r, 8, r * 2, RGB(255, 236, 94));
        dibujarRectPixel(hdc, x - r / 2, y - r / 2, r, r, RGB(237, 97, 55));
    }
}

void dibujarFondoPixelArt(HDC hdc) {
    rellenarRectangulo(hdc, 0, 0, VENTANA_ANCHO, PANEL_ALTO, RGB(31, 35, 52));
    rellenarRectangulo(hdc, 0, PANEL_ALTO, VENTANA_ANCHO, SUELO_Y, RGB(95, 144, 181));
    rellenarRectangulo(hdc, 0, 200, VENTANA_ANCHO, SUELO_Y, RGB(73, 104, 143));
    rellenarRectangulo(hdc, 0, 365, VENTANA_ANCHO, SUELO_Y, RGB(58, 75, 110));
    rellenarRectangulo(hdc, 0, SUELO_Y, VENTANA_ANCHO, VENTANA_ALTO, RGB(31, 47, 39));

    for (int x = 0; x < VENTANA_ANCHO; x += 32) {
        dibujarRectPixel(hdc, x, SUELO_Y, 16, 12, RGB(48, 74, 50));
        dibujarRectPixel(hdc, x + 16, SUELO_Y + 12, 16, 12, RGB(38, 61, 43));
    }

    dibujarRectPixel(hdc, 760, 108, 56, 56, RGB(255, 220, 87));
    dibujarRectPixel(hdc, 748, 124, 12, 24, RGB(255, 220, 87));
    dibujarRectPixel(hdc, 816, 124, 12, 24, RGB(255, 220, 87));
    dibujarRectPixel(hdc, 776, 96, 24, 12, RGB(255, 220, 87));
    dibujarRectPixel(hdc, 778, 120, 10, 10, RGB(255, 243, 143));

    dibujarRectPixel(hdc, 90, 128, 48, 12, RGB(218, 232, 226));
    dibujarRectPixel(hdc, 110, 116, 44, 12, RGB(218, 232, 226));
    dibujarRectPixel(hdc, 565, 168, 64, 12, RGB(202, 220, 222));
    dibujarRectPixel(hdc, 590, 156, 48, 12, RGB(202, 220, 222));
}

void dibujarMenu(HDC hdc) {
    dibujarFondoPixelArt(hdc);

    dibujarRectPixel(hdc, 190, 122, 520, 328, RGB(24, 28, 43));
    dibujarRectPixel(hdc, 202, 134, 496, 304, RGB(43, 50, 76));
    dibujarRectPixel(hdc, 235, 164, 430, 8, RGB(239, 190, 83));
    dibujarTexto(hdc, 295, 190, "GORILLA.BAS", RGB(255, 234, 161));
    dibujarTexto(hdc, 260, 225, "Reinterpretacion en C++ - Pixel Art", RGB(210, 226, 216));
    dibujarTexto(hdc, 250, 255, "Ingrese nombres y elija un modo", RGB(255, 234, 161));
    dibujarTexto(hdc, 250, 285, "Primero en llegar a 3 puntos gana la partida.", RGB(210, 226, 216));
    dibujarTexto(hdc, 250, 310, "Use angulo y fuerza para lanzar bananas.", RGB(210, 226, 216));
    dibujarTexto(hdc, 312, 330, "Nombres de los jugadores", RGB(255, 234, 161));

    Jugador monoMenu = {"", 450, 500, RGB(245, 90, 82)};
    dibujarJugador(hdc, monoMenu);
    dibujarBanana(hdc, 535, 430);
}

void dibujarReglas(HDC hdc) {
    dibujarFondoPixelArt(hdc);

    dibujarRectPixel(hdc, 170, 105, 560, 370, RGB(24, 28, 43));
    dibujarRectPixel(hdc, 184, 119, 532, 342, RGB(43, 50, 76));
    dibujarRectPixel(hdc, 225, 148, 450, 8, RGB(239, 190, 83));

    dibujarTexto(hdc, 390, 175, "REGLAS", RGB(255, 234, 161));
    dibujarTexto(hdc, 245, 220, "1. Cada jugador dispara una banana por turno.", RGB(210, 226, 216));
    dibujarTexto(hdc, 245, 250, "2. Ingrese angulo y fuerza antes de disparar.", RGB(210, 226, 216));
    dibujarTexto(hdc, 245, 280, "3. El viento modifica la trayectoria.", RGB(210, 226, 216));
    dibujarTexto(hdc, 245, 310, "4. Si la banana golpea al rival, suma 1 punto.", RGB(210, 226, 216));
    dibujarTexto(hdc, 245, 340, "5. Si golpea un edificio, rompe esa zona.", RGB(210, 226, 216));
    dibujarTexto(hdc, 245, 370, "6. Gana la partida quien llega primero a 3 puntos.", RGB(210, 226, 216));

    dibujarBanana(hdc, 450, 425);
}

void dibujarPantallaFinal(HDC hdc) {
    dibujarFondoPixelArt(hdc);
    dibujarRectPixel(hdc, 190, 145, 520, 300, RGB(24, 28, 43));
    dibujarRectPixel(hdc, 204, 159, 492, 272, RGB(50, 55, 83));
    dibujarRectPixel(hdc, 250, 185, 400, 10, RGB(255, 220, 87));

    string ganador = juego.jugadores[juego.ganadorPartida].nombre;
    dibujarTexto(hdc, 310, 225, "VICTORIA FINAL", RGB(255, 234, 161));
    dibujarTexto(hdc, 285, 265, ganador + " gano la partida.", RGB(210, 226, 216));
    dibujarTexto(hdc, 323, 300, "Resultado: " + to_string(juego.puntos[0]) + " - " + to_string(juego.puntos[1]),
                 RGB(255, 234, 161));

    int pulso = (GetTickCount() / 120) % 8;
    dibujarRectPixel(hdc, 428 - pulso * 3, 345, 44 + pulso * 6, 8, RGB(255, 236, 94));
    dibujarRectPixel(hdc, 446, 327 - pulso * 3, 8, 44 + pulso * 6, RGB(255, 236, 94));

    Jugador monoGanador = {"", 450, 505, juego.jugadores[juego.ganadorPartida].color};
    dibujarJugador(hdc, monoGanador);
}

void dibujarJuego(HDC hdc) {
    if (juego.enMenu) {
        if (juego.mostrandoReglas) {
            dibujarReglas(hdc);
            return;
        }

        dibujarMenu(hdc);
        return;
    }

    if (juego.pantallaFinal && juego.ganadorPartida >= 0) {
        dibujarPantallaFinal(hdc);
        return;
    }

    dibujarFondoPixelArt(hdc);
    dibujarCiudad(hdc);

    dibujarJugador(hdc, juego.jugadores[0]);
    dibujarJugador(hdc, juego.jugadores[1]);
    dibujarAnimacionLanzamiento(hdc);
    dibujarImpactoMono(hdc);

    if (juego.proyectil.activo) {
        int x = static_cast<int>(round(juego.proyectil.x));
        int y = static_cast<int>(round(juego.proyectil.y));
        dibujarBanana(hdc, x, y);
    }

    string viento = "Viento: " + to_string(juego.viento);
    if (juego.viento > 0) {
        viento += " hacia la derecha";
    } else if (juego.viento < 0) {
        viento += " hacia la izquierda";
    } else {
        viento += " sin viento";
    }

    dibujarTexto(hdc, 22, 16, "GORILLA.BAS reinterpretado en C++ - Pixel Art", RGB(244, 232, 184));
    dibujarTexto(hdc, 22, 45, juego.mensaje, RGB(198, 220, 210));
    dibujarTexto(hdc, 530, 45, viento, RGB(198, 220, 210));
    dibujarTexto(hdc, 340, 16,
                 juego.jugadores[0].nombre + " " + to_string(juego.puntos[0]) + " - " +
                 to_string(juego.puntos[1]) + " " + juego.jugadores[1].nombre,
                 RGB(255, 234, 161));
}

// Procedimiento principal de la ventana: recibe botones, timers y repintado.
LRESULT CALLBACK procedimientoVentana(HWND ventana, UINT mensaje, WPARAM wParam, LPARAM lParam) {
    switch (mensaje) {
        case WM_CREATE:
            etiquetaAngulo = CreateWindowA("STATIC", "Angulo", WS_VISIBLE | WS_CHILD, 545, 15, 55, 22,
                                           ventana, NULL, NULL, NULL);
            entradaAngulo = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                          600, 12, 50, 25, ventana, NULL, NULL, NULL);

            etiquetaFuerza = CreateWindowA("STATIC", "Fuerza", WS_VISIBLE | WS_CHILD, 660, 15, 70, 22,
                                           ventana, NULL, NULL, NULL);
            entradaVelocidad = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                             735, 12, 50, 25, ventana, NULL, NULL, NULL);

            botonDisparar = CreateWindowA("BUTTON", "Disparar", WS_VISIBLE | WS_CHILD,
                                          795, 10, 85, 28, ventana, reinterpret_cast<HMENU>(1), NULL, NULL);
            botonReiniciar = CreateWindowA("BUTTON", "Nueva ronda", WS_VISIBLE | WS_CHILD,
                                           795, 45, 85, 28, ventana, reinterpret_cast<HMENU>(2), NULL, NULL);

            botonDosJugadores = CreateWindowA("BUTTON", "Dos jugadores", WS_VISIBLE | WS_CHILD,
                                              218, 426, 145, 34, ventana, reinterpret_cast<HMENU>(10), NULL, NULL);
            botonComputadora = CreateWindowA("BUTTON", "Contra PC", WS_VISIBLE | WS_CHILD,
                                             378, 426, 145, 34, ventana, reinterpret_cast<HMENU>(11), NULL, NULL);
            botonReglas = CreateWindowA("BUTTON", "Reglas", WS_VISIBLE | WS_CHILD,
                                        538, 426, 145, 34, ventana, reinterpret_cast<HMENU>(12), NULL, NULL);

            etiquetaNombre1 = CreateWindowA("STATIC", "Jugador 1", WS_VISIBLE | WS_CHILD,
                                            285, 342, 80, 22, ventana, NULL, NULL, NULL);
            entradaNombre1 = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                           290, 350, 320, 25, ventana, NULL, NULL, NULL);
            etiquetaNombre2 = CreateWindowA("STATIC", "Jugador 2", WS_VISIBLE | WS_CHILD,
                                            285, 374, 80, 22, ventana, NULL, NULL, NULL);
            entradaNombre2 = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                           290, 380, 320, 25, ventana, NULL, NULL, NULL);

            botonRevancha = CreateWindowA("BUTTON", "Volver a jugar", WS_VISIBLE | WS_CHILD,
                                          315, 370, 130, 34, ventana, reinterpret_cast<HMENU>(20), NULL, NULL);
            botonVolverMenu = CreateWindowA("BUTTON", "Menu principal", WS_VISIBLE | WS_CHILD,
                                            455, 370, 130, 34, ventana, reinterpret_cast<HMENU>(21), NULL, NULL);
            botonVolverReglas = CreateWindowA("BUTTON", "Volver", WS_VISIBLE | WS_CHILD,
                                              385, 500, 130, 34, ventana, reinterpret_cast<HMENU>(22), NULL, NULL);

            juego.enMenu = true;
            juego.contraComputadora = false;
            juego.rondaTerminada = false;
            juego.partidaTerminada = false;
            juego.pantallaFinal = false;
            juego.mostrandoReglas = false;
            juego.ganadorPartida = -1;
            mostrarMenuControles();
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                iniciarDisparo(ventana);
            } else if (LOWORD(wParam) == 2) {
                detenerTimers(ventana);
                nuevaRonda();
            } else if (LOWORD(wParam) == 10) {
                comenzarJuego(ventana, false);
            } else if (LOWORD(wParam) == 11) {
                comenzarJuego(ventana, true);
            } else if (LOWORD(wParam) == 12) {
                mostrarReglas(ventana);
            } else if (LOWORD(wParam) == 20) {
                comenzarJuego(ventana, juego.contraComputadora);
            } else if (LOWORD(wParam) == 21) {
                volverAlMenu(ventana);
            } else if (LOWORD(wParam) == 22) {
                volverAlMenu(ventana);
            }
            InvalidateRect(ventana, NULL, FALSE);
            return 0;

        case WM_TIMER:
            if (wParam == 1) {
                actualizarFisica(ventana);
            } else if (wParam == 2) {
                KillTimer(ventana, 2);
                dispararComputadora(ventana);
            } else if (wParam == 3) {
                actualizarExplosion(ventana);
            } else if (wParam == 4) {
                KillTimer(ventana, 4);
                KillTimer(ventana, 5);
                if (juego.partidaTerminada) {
                    juego.pantallaFinal = true;
                    sonidoVictoria();
                    mostrarControlesFinal();
                    SetTimer(ventana, 7, 120, NULL);
                } else {
                    nuevaRonda();
                }
            } else if (wParam == 5) {
                actualizarImpactoMono(ventana);
            } else if (wParam == 6) {
                actualizarLanzamiento(ventana);
            } else if (wParam == 7) {
                // Solo fuerza el repintado para la animacion de la pantalla final.
            }
            InvalidateRect(ventana, NULL, FALSE);
            return 0;

        case WM_ERASEBKGND:
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(ventana, &ps);
            HDC hdcMemoria = CreateCompatibleDC(hdc);
            HBITMAP bitmapMemoria = CreateCompatibleBitmap(hdc, VENTANA_ANCHO, VENTANA_ALTO);
            HBITMAP bitmapAnterior = static_cast<HBITMAP>(SelectObject(hdcMemoria, bitmapMemoria));

            dibujarJuego(hdcMemoria);
            BitBlt(hdc, 0, 0, VENTANA_ANCHO, VENTANA_ALTO, hdcMemoria, 0, 0, SRCCOPY);

            SelectObject(hdcMemoria, bitmapAnterior);
            DeleteObject(bitmapMemoria);
            DeleteDC(hdcMemoria);
            EndPaint(ventana, &ps);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcA(ventana, mensaje, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int mostrar) {
    srand(static_cast<unsigned int>(time(0)));

    WNDCLASSA clase = {};
    clase.lpfnWndProc = procedimientoVentana;
    clase.hInstance = instancia;
    clase.lpszClassName = "GorillaCppVentana";
    clase.hbrBackground = NULL;
    clase.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassA(&clase);

    HWND ventana = CreateWindowA(
        clase.lpszClassName,
        "Gorilla.bas reinterpretado en C++",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        VENTANA_ANCHO,
        VENTANA_ALTO,
        NULL,
        NULL,
        instancia,
        NULL
    );

    ShowWindow(ventana, mostrar);
    UpdateWindow(ventana);

    MSG mensaje;
    while (GetMessageA(&mensaje, NULL, 0, 0)) {
        TranslateMessage(&mensaje);
        DispatchMessageA(&mensaje);
    }

    return 0;
}