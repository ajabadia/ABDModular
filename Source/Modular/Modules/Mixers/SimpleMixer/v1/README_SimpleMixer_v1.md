# SimpleMixer v1

## Información General
**Nombre**: SimpleMixer  
**Versión**: 1.0.0  
**Categoría**: Mixers  
**Descripción**: Módulo de control de ganancia (VCA). En la arquitectura de cadena serial actual, actúa como control de volumen maestro o atenuador.

## Especificaciones

### Parámetros
| ID | Nombre | Rango | Default | Descripción |
|----|--------|-------|---------|-------------|
| `master` | Master Gain | 0.0 - 2.0 | 1.0 | Ganancia linear aplicada a la señal estéreo. |

### Notas
- En futuras versiones, este módulo podría aceptar entradas side-chain o múltiples buffers si la arquitectura base se expande para soportar puertos auxiliares.
