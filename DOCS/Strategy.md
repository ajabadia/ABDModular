# Estrategia de Desarrollo: Infraestructura Modular

**Rol**: Senior JUCE Developer  
**Objetivo**: Crear un ecosistema de síntesis modular escalable, mantenible y profesional.

## Filosofía General
En lugar de construir sintetizadores monolíticos (como el CZ-101 original), construiremos "ladrillos" (Módulos) independientes y estandarizados. Estos ladrillos se ensamblarán usando "Orquestadores" para crear instrumentos complejos.

### Principios Clave
1.  **Aislamiento**: Un `OscillatorModule` no sabe que es parte de un CZ-101 o un Minimoog. Solo genera audio.
2.  **Estandarización**: Todos los módulos heredan de `Module` y se comunican vía `AudioFrame` (audio) y `ControlMessage` (eventos).
3.  **Composición sobre Herencia**: Un sintetizador es una colección de módulos conectados, no una clase gigante.
4.  **Data-Driven**: La configuración de los instrumentos debe poder definirse en tiempo de ejecución (presets, wiring).

## Plan de Fases

### Fase 1: Los Cimientos (The Core)
*Objetivo: Definir el lenguaje común que hablarán todos los módulos.*
- **Estructuras de Datos**: `AudioFrame` (buffer estéreo), `ControlMessage` (eventos).
- **Clase Base**: `Module.h` (interface polimórfica: `processFrame`, `handleMessage`, `setParameter`).
- **Orquestador Base**: `Orchestrator.h` (gestor de voces, midi y ruteo de audio).
- **Voice Manager**: Lógica de asignación de polifonía agnóstica del instrumento.

### Fase 2: Ladrillos Básicos (Basic Bricks)
*Objetivo: Crear los componentes mínimos para hacer sonido.*
- **OscillatorModule**: Generador de ondas básico (Sine, Saw, Squ, Tri).
- **FilterModule**: Filtro multimodo (LP, HP, etc.).
- **EnvelopeModule**: Generador de contornos ADSR.
- **VCA / Mixer**: Control de ganancia y mezcla de señales.
- **LFOModule**: Modulación de baja frecuencia.

### Fase 3: El Primer Ensamble (The Pilot)
*Objetivo: Validar la arquitectura con un instrumento simple.*
- **MinimoogOrchestrator (Proof of Concept)**: Un sintetizador sustractivo simple (3 Osc -> Filter -> VCA) para probar que los ladrillos encajan.
- *Nota*: Es más fácil validar con un diseño simple (Minimoog) que con el complejo CZ-101 (Phase Distortion).

### Fase 4: Reconstrucción del CZ-101
*Objetivo: Portar la lógica "Phase Distortion" existente al nuevo sistema.*
- Crear `PhaseDistortionOscillatorModule` adaptado del código `TOTALCODE`.
- Crear `CZ101Orchestrator` que recablee estos módulos específicos.
- Migrar el Editor UI para usar componentes genéricos.

### Fase 5: Expansión (The Vision)
*Objetivo: Infraestructura avanzada.*
- Secuenciadores (Poly, Euclidean, Probability).
- Efectos (Reverb, Delay).
- MIDI Brain.

## Estrategia de "Entrada a Mitad de Proyecto"
Dado que el contexto se saturará:
1.  **Documentación Viva**: Mantendremos `DOCS/Architecture_Reference.md` actualizado con los contratos de las clases.
2.  **Micro-Tareas**: No intentaremos "hacer todo el sistema". Cada ticket/tarea será "Crear Oscilador" o "Crear Filtro".
3.  **Tests Unitarios**: Cada módulo debe tener su propio test para asegurar que funciona aislado antes de intentar conectarlo.

---
**Siguiente Paso Inmediato**: Definir la arquitectura técnica en `Module.h` y `AudioFrame`.
