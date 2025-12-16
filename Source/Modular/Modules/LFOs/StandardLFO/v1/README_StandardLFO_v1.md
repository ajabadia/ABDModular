# StandardLFO v1

## Información General
**Nombre**: StandardLFO  
**Versión**: 1.0.0  
**Categoría**: LFOs  
**Descripción**: Oscilador de baja frecuencia para modulación. Genera señales de control en el rango de audio (-1.0 a 1.0 * Depth).

## Especificaciones

### Entradas/Salidas
- **Salida**: Señal de control (DC) en ambos canales L/R. Ignora la entrada de audio.

### Parámetros
| ID | Nombre | Rango | Default | Descripción |
|----|--------|-------|---------|-------------|
| `frequency` | Frequency | 0.01 - 100.0 (Hz) | 1.0 | Velocidad de oscilación. |
| `depth` | Depth | 0.0 - 1.0 | 1.0 | Amplitud de la señal de salida. |
| `waveform` | Waveform | 0 - 3 | 0 | 0=Sine, 1=Triangle, 2=Saw, 3=Square. |

### Uso
Típicamente usado para modular parámetros de otros módulos. El host debe leer la salida del buffer del LFO y asignarla a `filter.setParameter("cutoff", base + lfoOut * amount)`.
