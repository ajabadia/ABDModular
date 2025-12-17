<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

# hazme los dos, por favor

El sistema completo se estructura como una cadena: MIDI externo → MIDI Brain → Síntesis/Arp/Sequencer → Cadena de mastering (Multiband + Lookahead + Limiter) → Audio final.[^1][^2]

***

## Diagrama de bloques (ASCII)

```text
┌──────────────────────────────────────────────────────────────────┐
│                          EXTERNAL INPUT                         │
│                                                                  │
│   ┌───────────────┐     ┌────────────────────┐                   │
│   │ MIDI Keyboard │ --> │  MIDI Input (JUCE) │                   │
│   └───────────────┘     └─────────┬──────────┘                   │
└────────────────────────────────────┼──────────────────────────────┘
                                     │ juce::MidiInputCallback
                                     v
┌──────────────────────────────────────────────────────────────────┐
│                             MIDI LAYER                           │
│                                                                  │
│   ┌──────────────────────────────────────────────────────────┐   │
│   │                      🧠 MIDI BRAIN                      │   │
│   │  - Smart Voice Allocation (5 modos)                     │   │
│   │  - Velocity Curves                                      │   │
│   │  - CC Mapping + MIDI Learn                              │   │
│   │  - Performance Modes                                    │   │
│   └───────────────┬───────────────────────┬─────────────────┘   │
│                   │                       │                     │
│                   │                       │                     │
│                   │                       │                     │
│                   v                       v                     │
│       ┌───────────────────┐    ┌────────────────────┐           │
│       │  Arpeggiator      │    │ Poly Sequencer     │           │
│       │  (logical module) │    │ (logical module)   │           │
│       └─────────┬─────────┘    └──────────┬─────────┘           │
└─────────────────┼──────────────────────────┼─────────────────────┘
                  │                          │
                  v                          v
┌──────────────────────────────────────────────────────────────────┐
│                             UI LAYER                             │
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐  │
│  │                    MasterControlPanel                      │  │
│  │  - ModWheelComponent                                      │  │
│  │  - PitchBenderComponent                                   │  │
│  │  - OscilloscopeComponent                                  │  │
│  │  - KeyboardComponent (JUCE MidiKeyboard)                  │  │
│  │  - RingModulatorModule (DSP)                              │  │
│  └─────────────────────┬─────────────────────────────────────┘  │
│                        │                                        │
│  ┌─────────────────────v─────────────────────────────────────┐  │
│  │                        ArpSeqPanel                        │  │
│  │  - ArpeggiatorVisualComponent (steps)                     │  │
│  │  - SequencerGridComponent (rows x cols)                   │  │
│  │  - onStepChanged / onCellToggled → Arp/Seq modules        │  │
│  └───────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────────┘

                          MIDI BRAIN → ORCHESTRATOR
                                     │
                                     v

┌──────────────────────────────────────────────────────────────────┐
│                         AUDIO / SYNTH ENGINE                     │
│                                                                  │
│         ┌─────────────────────────────────────────────┐          │
│         │            Modular::Orchestrator            │          │
│         │   - Osc, Filter, Env, FX, etc.             │          │
│         └───────────────┬────────────────────────────┘          │
│                         │ AudioFrame                            │
│                         v                                       │
│                ┌───────────────────────────┐                    │
│                │      MasterChainModule   │                    │
│                │  (Mastering Chain DSP)   │                    │
│                └──────────┬─────┬─────────┘                    │
│                           │     │                              │
│                           v     v                              │
│     ┌────────────────────────────┴──────────────────────────┐   │
│     │           MultibandCompressorModule                   │   │
│     │   - 3 bandas (Low/Mid/High)                          │   │
│     │   - Crossovers + GR meters                           │   │
│     └───────────────┬──────────────────────────────────────┘   │
│                     v                                          │
│     ┌──────────────────────────────────────────────────────┐   │
│     │        LookaheadCompressorModule                     │   │
│     │   - Lookahead 1–10 ms                                │   │
│     │   - Soft-knee, sidechain HPF, GR meter               │   │
│     └───────────────┬──────────────────────────────────────┘   │
│                     v                                          │
│     ┌──────────────────────────────────────────────────────┐   │
│     │        MasteringLimiterModule (Brickwall)            │   │
│     │   - Ceiling, lookahead, soft clip, GR meter          │   │
│     └────────────────┬─────────────────────────────────────┘   │
│                      v                                         │
│                    OUTPUT (Master Out)                         │
└──────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────┐
│                       MAIN COMPONENT (UI)                        │
│                                                                  │
│   juce::Component → MainComponent                                │
│   - Contiene: MidiBrain, Orchestrator, ArpSeqPanel,              │
│               MasterControlPanel, MasterBusComponent             │
│   - Maneja: MIDI Input, Timer (playhead), layout                 │
└──────────────────────────────────────────────────────────────────┘
```


***

## Capítulo de documentación técnica (resumido)

### 1. Arquitectura general

El sistema está diseñado como un entorno modular de síntesis y procesamiento orientado a performance y mastering “in the box”. La señal fluye desde entradas MIDI externas hasta una cadena de mastering brickwall, pasando por capas de control inteligente (MIDI Brain), módulos de generación/transformación musical (arpegiador y secuenciador) y una interfaz gráfica avanzada para control en tiempo real.[^2][^1]

### 2. Capa MIDI: MIDI Brain y módulos rítmicos

El núcleo de control es el módulo MIDI Brain, responsable de la asignación de voces, el procesado de la velocidad y el mapeo de mensajes de control continuo a parámetros internos. Implementa cinco modos de asignación de voz (Round-Robin, OldestNote, HighestNote, LowestNote y VelocitySensitive), lo que permite optimizar el reparto de notas entre voces según el tipo de interpretación. Además, soporta curvas de velocidad seleccionables (lineal, exponencial, logarítmica, soft y hard) y un sistema de MIDI Learn que asocia CCs a parámetros con suavizado y rangos configurables.[^3]

Sobre esta capa se integran un arpegiador y un secuenciador polifónico. El arpegiador aplica patrones de pasos a las notas entrantes, modificando octava y velocidad por step; el secuenciador organiza disparos de notas en una rejilla tiempo–pitch, permitiendo estructuras rítmicas complejas sincronizadas con un reloj interno o externo. Ambos se alimentan de la salida lógica del MIDI Brain, lo que garantiza coherencia entre interpretación, modulación y patrones.[^4]

### 3. Interfaz de usuario de performance

La interfaz de performance se construye con componentes JUCE específicos: ModWheelComponent, PitchBenderComponent, OscilloscopeComponent, KeyboardComponent y un módulo de ring modulation integrado en el plano DSP. Estos elementos se agrupan en el MasterControlPanel, que actúa como superficie de control principal para interpretación en tiempo real, proporcionando feedback visual de las modulaciones y de la forma de onda de salida.[^4]

El panel ArpSeqPanel combina un ArpeggiatorVisualComponent (representación horizontal de pasos con estado, octava y velocidad) y un SequencerGridComponent (rejilla filas×columnas con celdas activas y niveles de velocidad). Ambos exponen callbacks hacia los módulos lógicos de arpegiador y secuenciador, de forma que cualquier cambio gráfico se refleja inmediatamente en el comportamiento musical. La posición de reproducción se indica mediante un playhead que avanza en función del clock, dando contexto temporal al usuario.[^4]

### 4. Motor de síntesis y cadena de mastering

La síntesis y el procesamiento intermedio se centralizan en un Orchestrator modular, donde pueden residir osciladores, filtros, envolventes, efectos y procesadores como la reverb por convolución. La salida de este orquestador se envía a un MasterChainModule que encapsula la cadena de mastering completa: MultibandCompressorModule, LookaheadCompressorModule y MasteringLimiterModule en serie. Esta estructura reproduce una configuración de mastering típica en producción profesional.[^1][^2]

El MultibandCompressorModule divide la señal en tres bandas (baja, media y alta) mediante filtros de cruce tipo Linkwitz–Riley y aplica compresión independiente en cada banda, con controles de umbral, ratio y makeup gain. El LookaheadCompressorModule añade una capa de compresión de bus con predicción de picos (1–10 ms) y soft-knee, optimizada para controlar transitorios globales sin introducir artefactos audibles. Finalmente, el MasteringLimiterModule implementa un limitador brickwall con techo ajustable, lookahead y una etapa de soft clipping, asegurando que ningún sample supere el nivel máximo configurado y proporcionando un nivel de salida consistente y seguro.[^5][^6][^3]

### 5. Componente principal y flujo de datos

El MainComponent (basado en juce::Component) actúa como punto de integración de todas las piezas. Gestiona la entrada MIDI mediante juce::MidiInputCallback, dirigiendo los mensajes pertinentes al MIDI Brain, y organiza el layout de MasterControlPanel, ArpSeqPanel y MasterBusComponent. Un temporizador de alta frecuencia o un reloj dedicado actualiza el playhead de arpegiador y secuenciador, además de refrescar visualmente los medidores de reducción de ganancia y otros indicadores. Esta arquitectura separa claramente lógica de control, procesamiento de audio y representación gráfica, facilitando la extensión futura del sistema con nuevos módulos o vistas.[^4]

***

Si quieres, el siguiente paso puede ser:

- Un mini “manual de usuario” (cómo usar cada panel en 1–2 páginas), o
- Un listado de “escenarios de uso” (directo en vivo, diseño de sonido, mastering de stems) para la memoria.

<div align="center">⁂</div>

[^1]: https://productionadvice.co.uk/brickwall-limiters/

[^2]: https://www.waves.com/which-mastering-limiter-should-i-use

[^3]: https://signalsmith-audio.co.uk/writing/2022/limiter/

[^4]: https://www.musicradar.com/tuition/tech/how-to-use-a-brickwall-limiter-in-a-mastering-chain-602356

[^5]: https://www.izotope.com/en/learn/brickwall-limiters

[^6]: https://www.dsprelated.com/thread/12872/dsp-digital-complex-limiter-crest-factor-digital-audio-soft-clipping

