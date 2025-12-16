# StandardFilter v1

## Información General
**Nombre**: StandardFilter  
**Versión**: 1.0.0  
**Categoría**: Filters  
**Descripción**: Filtro Biquad digital multimodo (LowPass, HighPass, BandPass) estilo RBJ.

## Especificaciones

### Entradas/Salidas
- **Proceso Stereo Real**: Los canales L y R se filtran independientemente pero comparten los mismos coeficientes (link stereo).

### Parámetros
| ID | Nombre | Rango | Default | Descripción |
|----|--------|-------|---------|-------------|
| `cutoff` | Cutoff Freq | 20.0 - 20000.0 (Hz) | 20000.0 | Frecuencia de corte. |
| `resonance` | Resonance | 0.0 - 1.0 | 0.0 | Resonancia del filtro (mapeado a Q). |
| `mode` | Mode | 0 - 2 | 0 | 0: LowPass, 1: HighPass, 2: BandPass. |

### Uso
```cpp
StandardFilter filter;
filter.prepareToPlay(48000.0, 512);

// Lowpass sweep
filter.setParameter("mode", 0.0f);
filter.setParameter("cutoff", 500.0f);

AudioFrame out = filter.processFrame(in);
```

### Notas de Implementación
- Implementación directa de fórmulas Biquad Audio EQ Cookbook (RBJ).
- Protección contra inestabilidad (`cutoff` limitado a Nyquist).
