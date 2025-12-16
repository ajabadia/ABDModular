# ADSREnvelope v1

## Información General
**Nombre**: ADSREnvelope  
**Versión**: 1.0.0  
**Categoría**: Envelopes  
**Descripción**: Generador de envolvente estándar de 4 etapas (Attack, Decay, Sustain, Release). Funciona como un procesador de ganancia (VCA integrado) multiplicando la señal de entrada por el valor de la envolvente.

## Especificaciones

### Control Messages
Responde a mensajes de control para disparar las fases:
- `GateOpen`: Inicia la fase de Attack.
- `GateClose`: Inicia la fase de Release.

### Parámetros
| ID | Nombre | Rango | Default | Descripción |
|----|--------|-------|---------|-------------|
| `attack` | Attack Time | 0.0 - 10.0 (s) | 0.1 | Tiempo para ir de 0 a 1. |
| `decay` | Decay Time | 0.0 - 10.0 (s) | 0.1 | Tiempo para ir de 1 al nivel de Sustain. |
| `sustain` | Sustain Level | 0.0 - 1.0 | 0.8 | Nivel mantenido mientras Gate está abierto. |
| `release` | Release Time | 0.0 - 10.0 (s) | 0.2 | Tiempo para caer a 0 tras GateClose. |

### Uso (C++)
```cpp
Modular::Modules::ADSREnvelope env;
env.prepareToPlay(44100.0, 512);

// Trigger
ControlMessage msg { ControlMessage::Type::GateOpen };
env.handleMessage(msg);

// Process (Audio In * Envelope)
AudioFrame out = env.processFrame(in);
```

### Historial de Cambios
- **v1.0.0**: Implementación lineal básica de ADSR.
