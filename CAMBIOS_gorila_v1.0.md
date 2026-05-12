# Cambios en gorila_v1.0

## Archivo renombrado

- El codigo fuente principal paso de `TP_GorillaBas_Chaux_Demian.cpp` a `gorila_v1.0.cpp`.

## Controles nuevos

- Flecha izquierda / `A`: baja el angulo del disparo.
- Flecha derecha / `D`: sube el angulo del disparo.
- Flecha arriba / `W`: sube la fuerza.
- Flecha abajo / `S`: baja la fuerza.
- `Espacio` o `Enter`: dispara la banana.
- Mantener `Shift` aumenta el paso de ajuste para cambiar los valores mas rapido.

## Trayectoria previa

- Se agrego una linea punteada antes de disparar.
- La trayectoria usa el mismo viento, gravedad y fuerza del disparo real para mostrar una referencia aproximada de donde va a caer la banana.
- La vista previa desaparece durante el vuelo, la animacion de lanzamiento, explosiones, menus y turnos de la computadora.

## Fisica mejorada

- Se centralizo el calculo de origen, velocidad inicial, gravedad y viento para que la IA, la trayectoria previa y el disparo real usen la misma base.
- El proyectil ahora avanza con subpasos de fisica, lo que reduce saltos grandes entre frames.
- Las colisiones contra edificios revisan el segmento recorrido por la banana, no solo el punto final, para evitar que atraviese bordes o edificios delgados.
- La IA tambien considera choques contra edificios al estimar sus disparos.

## Interfaz

- Las cajas de texto de angulo y fuerza ya no se muestran durante la partida.
- Los valores actuales se ven como etiquetas en el panel superior.
- El menu y la pantalla de reglas fueron actualizados para explicar los controles nuevos.
