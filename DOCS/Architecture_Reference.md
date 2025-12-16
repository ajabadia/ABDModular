# Referencia de Arquitectura Técnica

Este documento define los contratos técnicos del sistema modular. Para las reglas de organización de archivos, ver [Structure_Guidelines.md](Structure_Guidelines.md).

## 1. Estructuras de Datos (Common/Types.h)

### AudioFrame
Contenedor para un "instante" de audio (o un bloque, dependiendo de la optimización). Para el diseño modular sample-by-sample (estilo VCV Rack o modular clásico), procesamos frames.
```cpp
struct AudioFrame {
    static constexpr int CHANNELS = 2;
    float samples[CHANNELS] = { 0.0f, 0.0f };
    
    // Helpers
    void clear() { samples[0] = 0.0f; samples[1] = 0.0f; }
    void add(const AudioFrame& other) { ... }
    void multiply(float gain) { ... }
};
```

### ControlMessage
Sistema de eventos para evitar llamadas directas acopladas.
```cpp
struct ControlMessage {
    enum class Type { NoteOn, NoteOff, GateOpen, GateClose, ParameterChange, Modulate };
    Type type;
    int channel = 0;
    int note = 0;
    float value = 0.0f; // Velocity, CC value, etc.
};
```

## 2. Clase Base (Modular/Module.h)

Todas las piezas del sistema deben heredar de esta clase.

```cpp
class Module {
public:
    virtual ~Module() = default;

    // Configuración inicial
    virtual void prepareToPlay(double sampleRate, int maxBlockSize) {
        sampleRate_ = sampleRate;
        maxBlockSize_ = maxBlockSize;
        // Subclasses use these to init buffers/coefficients
    }

    // Proceso de audio principal
    virtual AudioFrame process(const AudioFrame& input) = 0;

    // Manejo de eventos
    virtual void handleMessage(const ControlMessage& message) { }

    // Parámetros genéricos (para automatización/UI)
    virtual void setParameter(const std::string& paramId, float value) {
        parameters_[paramId] = value;
    }
    virtual float getParameter(const std::string& paramId) const {
        // ... return value ...
    }

protected:
    double sampleRate_ = 44100.0;
    std::unordered_map<std::string, float> parameters_;
};
```

## 3. El Orquestador (Modular/Orchestrator.h)

El director de orquesta. No procesa audio per se, sino que dirige el flujo entre módulos y gestiona voces.

```cpp
class Orchestrator : public AudioSource { // JUCE AudioSource
public:
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override {
        // 1. Procesar eventos MIDI
        // 2. Por cada sample en el buffer:
        //    a. Limpiar frame acumulador
        //    b. Iterar voces activas
        //    c. Procesar cadena de módulos de la voz (Osc -> Filter -> Amp)
        //    d. Sumar al acumulador
        //    e. Escribir en buffer
    }

    virtual void noteOn(int note, float velocity) = 0;
    virtual void noteOff(int note) = 0;
};
```

## 4. Convenciones de Nombres
- **Namespaces**: `Modular::Core`, `Modular::Modules`, `Modular::Instruments`.
- **Archivos**: `Source/Modular/Module.h`, `Source/Modular/Modules/OscillatorModule.h`.

## 5. Gestión de Polifonía
El `VoiceManager` no es un módulo de audio, es una utilidad lógica.
- Mantiene una lista de "Voces" (indices).
- Asigna notas MIDI a índices de voz disponibles.
- Gestiona "voice stealing" (robar voz si todas están ocupadas).
