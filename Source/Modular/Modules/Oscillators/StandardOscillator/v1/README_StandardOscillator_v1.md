# StandardOscillator v1

## Información General
**Nombre**: StandardOscillator  
**Versión**: 1.0.0  
**Categoría**: Oscillators  
**Descripción**: Un oscilador de propósito general con formas de onda básicas (Sine, Triangle, Square, Sawtooth) y soporte para modulación de frecuencia básica.

## Especificaciones

### Entradas (Inputs)
- **AudioFrame Input**: Actualmente ignorado (o usado para futura FM).

### Salidas (Outputs)
- **AudioFrame Output**: Señal generada estéreo (mismo sample L/R). Rango -1.0 a 1.0.

### Parámetros
| ID | Nombre | Rango | Default | Descripción |
|----|--------|-------|---------|-------------|
| `frequency` | Frequency | 0.0 - 20000.0 (Hz) | 440.0 | Frecuencia base del oscilador. |
| `detune` | Detune | -12.0 - 12.0 (semitonos) | 0.0 | Afinación fina. |
| `waveform` | Waveform | 0 - 3 | 0 (Sine) | Selector de onda: 0=Sine, 1=Triangle, 2=Saw, 3=Square. |
| `level` | Level | 0.0 - 1.0 | 1.0 | Volumen de salida. |

### Uso (C++)
```cpp
Modular::Modules::StandardOscillator osc;
osc.prepareToPlay(44100.0, 512);

// Config
osc.setParameter("frequency", 220.0f);
osc.setParameter("waveform", 2.0f); // Saw

// Process
AudioFrame out = osc.processFrame(in);
```

### Historial de Cambios
- **v1.0.0**: Implementación inicial con 4 formas de onda básicas.
