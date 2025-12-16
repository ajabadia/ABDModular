# Core Foundation v1

## Información General
**Nombre**: Modular Foundation  
**Versión**: 1.0.0  
**Categoría**: Core  
**Descripción**: Define los contratos base del sistema modular. Contiene las definiciones de tipos (`Types.h`) y la clase base de la que heredan todos los módulos (`Module.h`).

## Componentes

### AudioFrame
Estructura de datos para paso de mensajes de audio.
- Stereo (Hardcoded 2 channels por defecto).
- Operaciones vectoriales básicas (`add`, `multiply`).

### ControlMessage
Sistema de eventos unificado.
- Soporta: `NoteOn`, `NoteOff`, `GateOpen`, `GateClose`, `ParameterChange`.

### Module (Base Class)
Interfaz polimórfica que garantiza la interoperabilidad.
- `processFrame(AudioFrame)`: Proceso síncrono de audio.
- `handleMessage(ControlMessage)`: Proceso asíncrono de eventos.
- `setParameter(id, value)`: Sistema de automatización unificado.

### Historial de Cambios
- **v1.0.0**: Definición inicial de la arquitectura.
