# Guía de Estructura y Versionado (Library Philosophy)

Este documento define la normativa ESTRICTA para la organización del código fuente en el proyecto modular. El objetivo es crear una **Biblioteca de Componentes Versionados** que permita la evolución segura y la reutilización.

## 1. Jerarquía de Directorios

Cada componente debe residir en su propia carpeta, anidado por categoría, nombre y **versión**. No se permiten archivos sueltos en las carpetas de categoría.

### Patrón
`Source/Modular/[Category]/[ComponentName]/[Version]/`

### Ejemplo
```text
Source/Modular/
├── Modules/
│   ├── Oscillators/                <-- Categoría plural
│   │   └── StandardOscillator/     <-- Nombre del Componente (PascalCase)
│   │       └── v1/                 <-- Versión (siempre 'v' + número)
│   │           ├── StandardOscillator.h
│   │           └── README_StandardOscillator_v1.md
│   │
│   └── Envelopes/
│       └── ADSREnvelope/
│           └── v1/
│               ├── ADSREnvelope.h
│               └── README_ADSREnvelope_v1.md
```

## 2. Reglas de Componentes

### 2.1. Contenido de la Carpeta de Versión
Cada carpeta `vX` debe ser **autocontenida** en la medida de lo posible y debe incluir obligatoriamente:
1.  **Código Fuente (.h / .cpp)**: La implementación del módulo.
2.  **Documentación (README_Name_vX.md)**: Hoja de especificaciones.

### 2.2. El README del Componente
Debe seguir la plantilla estándar:
- **Información General**: Nombre, versión, categoría.
- **Especificaciones**:
    - Entradas y Salidas de audio.
    - Mensajes de Control soportados.
- **Parámetros**: Tabla detallada con ID, Rango, Default y Descripción.
- **Uso**: Snippet de código C++ de ejemplo.
- **Historial**: Cambios respecto a versiones anteriores.

## 3. Flujo de Trabajo

### Crear un Nuevo Componente
1.  Decidir Categoría (Oscillators, Filters, Mixers, etc.).
2.  Elegir un nombre descriptivo (ej: `MoogLadderFilter` en vez de `Filter`).
3.  Crear directorio `v1`.
4.  Implementar código y README.
5.  **Registrar**: Añadir entrada en `DOCS/Module_Registry.md`.

### Actualizar un Componente (Breaking Changes)
Si un cambio rompe la compatibilidad (ej: cambia el nombre de parámetros o la firma de métodos):
1.  **NO modificar la carpeta `v1`**.
2.  Crear una carpeta `v2` paralela.
3.  Copiar, modificar y evolucionar en `v2`.
4.  Los instrumentos existentes seguirán importando `v1` hasta que se migren explícitamente.

## 4. Namespaces
El namespace C++ debe reflejar la estructura física pero **no incluir la versión** para facilitar el uso (la versión se selecciona mediante el `#include` del archivo correcto).

- Correcto: `Modular::Modules::StandardOscillator`
- Incorrecto: `Modular::Modules::StandardOscillatorV1` (salvo colisión estricta necesaria)
## 5. Core vs Modules
- **Core**: Tipos fundamentales (`AudioFrame`), Interfaces (`Module`) y Utilidades lógicas (`VoiceManager`). Siguen la misma estructura de versionado (`Core/Foundation/v1`).
- **Modules**: Procesadores de audio concretos.
- **Instruments**: Orquestadores que ensamblan módulos. También deben versionarse (`Instruments/CZ101/v1`).

## 6. Contexto de Ejecución (Environment Context)
Es CRÍTICO que los módulos sean agnósticos del entorno de hardware. **NUNCA** asumir valores fijos para:

### 6.1. Sample Rate & Block Size
- Deben inyectarse dinámicamente vía `prepareToPlay(sampleRate, maxBlockSize)`.
- **Sample Rate**: Usado para cálculos de tiempo/frecuencia. Default safe: `0.0`.
- **Block Size**: El tamaño máximo de buffer que el host enviará. Los módulos deben reservar memoria base a esto si usan buffers internos.
- Los módulos deben recalcular sus coeficientes inmediatamente si estos valores cambian entre llamadas a `prepareToPlay`.

### 6.2. Resolución y Canales
- **Precisión**: El procesamiento interno es siempre **32-bit Float** (`float` en C++). No gestionar bit-depth (16/24 bit) dentro de los módulos DSP; eso es responsabilidad de la capa de E/S del Host.
- **Canales**: Definido por `AudioFrame` (actualmente Stereo fijo). Si esto cambia, se hará cambiando la definición de `AudioFrame`, no módulo por módulo.
