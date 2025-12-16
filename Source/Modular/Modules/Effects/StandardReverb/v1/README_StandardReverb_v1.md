# StandardReverb v1

## Información General
**Nombre**: StandardReverb  
**Versión**: 1.0.0  
**Categoría**: Effects  
**Descripción**: Reverb algorítmica clásica tipo Schroeder. Usa 4 filtros Comb en paralelo seguidos de 2 filtros All-Pass en serie.

## Especificaciones

### Parámetros
| ID | Nombre | Rango | Default | Descripción |
|----|--------|-------|---------|-------------|
| `mix` | Mix | 0.0 - 1.0 | 0.3 | Mezcla Dry/Wet. |
| `size` | Room Size | 0.5 - 2.0 | 1.0 | Factor de escala para los tiempos de retardo (re-init required si cambia drásticamente, actualmente no realtime). |

### Notas de Implementación
- Implementación Mono-to-Stereo (la salida es mono duplicada, podría mejorarse con decorrelación en fases posteriores).
- Tiempos de retardo basados en números primos para minimizar resonancias metálicas (Schroeder constants).
