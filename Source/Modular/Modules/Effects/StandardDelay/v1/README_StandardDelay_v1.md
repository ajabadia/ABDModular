# StandardDelay v1

## Información General
**Nombre**: StandardDelay  
**Versión**: 1.0.0  
**Categoría**: Effects  
**Descripción**: Delay digital estéreo básico con feedback (Feedback Delay).

## Especificaciones

### Parámetros
| ID | Nombre | Rango | Default | Descripción |
|----|--------|-------|---------|-------------|
| `time` | Delay Time | 0.0 - 2.0 (s) | 0.5 | Tiempo de retardo. |
| `feedback` | Feedback | 0.0 - 1.0 | 0.3 | Cantidad de señal reinyectada. |
| `mix` | Dry/Wet | 0.0 - 1.0 | 0.5 | Balance entre señal limpia y procesada. |

### Buffers
- Tamaño máximo hardcodeado a 2 segundos + blocksize.
- Soporta soft-clipping en el loop de feedback para evitar explosiones de volumen.
