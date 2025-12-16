# PolySequencer v1

## Información General
**Nombre**: PolySequencer  
**Versión**: 1.0.0  
**Categoría**: Sequencers  
**Descripción**: Secuenciador polifónico multitrack. Permite N pistas con conteo de pasos independiente.

## Especificaciones

### Parámetros
| ID | Nombre | Rango | Default | Descripción |
|----|--------|-------|---------|-------------|
| `tempo` | Tempo (BPM) | 30.0 - 300.0 | 120.0 | Velocidad de reproducción. |
| `on` | Transport | 0.0 - 1.0 | 0.0 | 0 = Stop, 1 = Play. |
| `mode` | Play Mode | 0 - 2 | 0 | 0=Fwd, 1=Back, 2=Random. |

### API C++
El módulo expone métodos para programación directa (pensado para ser usado por `GenericModuleEditor` o código host):
- `addTrack(name)`
- `setNote(track, step, note, vel)`
- `getCurrentNotes()`: Devuelve las notas activas en el paso actual.

### Notas
- La sincronización se basa en el conteo de muestras en `processFrame` usando el `sampleRate` inyectado.
- Resolución por defecto: semicorcheas (1/16th notes).
