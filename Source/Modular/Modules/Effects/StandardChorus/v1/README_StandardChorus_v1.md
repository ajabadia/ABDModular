# StandardChorus v1

## Información General
**Nombre**: StandardChorus  
**Versión**: 1.0.0  
**Categoría**: Effects  
**Descripción**: Efecto de modulación estéreo. Usa un LFO interno para modular el tiempo de retardo, creando sensación de grosor y movimiento.

## Especificaciones

### Entradas/Salidas
- **Stereo Spread**: El canal derecho usa la fase invertida del LFO para crear una imagen estéreo amplia.

### Parámetros
| ID | Nombre | Rango | Default | Descripción |
|----|--------|-------|---------|-------------|
| `rate` | Rate | 0.1 - 10.0 (Hz) | 0.5 | Velocidad de modulación. |
| `depth` | Depth | 0.0 - 0.02 (s) | 0.005 | Profundidad de modulación (en segundos). |
| `delay` | Pre-Delay | 0.0 - 0.05 (s) | 0.020 | Tiempo base de retardo. |
| `mix` | Mix | 0.0 - 1.0 | 0.5 | Mezcla Dry/Wet. |
| `feedback` | Feedback | 0.0 - 0.9 | 0.2 | Realimentación (Flanger-like effects si delay es corto). |

### Detalles Técnicos
- Buffer circular de 0.1s.
- Interpolación lineal para lectura del buffer.
