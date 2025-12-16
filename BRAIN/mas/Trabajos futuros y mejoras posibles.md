<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

### Trabajos futuros y mejoras posibles

Una vez implementado el sistema actual, existen varias líneas claras de mejora y ampliación que podrían abordarse en trabajos futuros.

#### 1. Mejora del motor de audio y calidad DSP

- Sustituir los filtros y detectores de envolvente simplificados por versiones más avanzadas (por ejemplo, filtros IIR diseñados con bilinear transform o FIR dedicados en crossovers y sidechain) para mejorar la respuesta en frecuencia y la estabilidad numérica en todo el rango de sample rates.[^1]
- Integrar oversampling real (2x/4x con filtros de entrada/salida) en el LookaheadCompressorModule y en el MasteringLimiterModule para reducir aliasing en situaciones de alta compresión y soft clipping intenso, acercando el comportamiento a herramientas de mastering comerciales.[^2][^1]
- Ampliar la reverb por convolución con convolución particionada “real” en bloque (UPOLS/UPFFB) y soporte multicanal (estéreo/true stereo), optimizando el uso de CPU en proyectos grandes.[^1]


#### 2. Extensión del MIDI Brain y la capa rítmica

- Añadir soporte para patrones de arpegiador y secuenciador polimétricos y/o polirrítmicos (distintas longitudes de patrón por pista), permitiendo estructuras más complejas que el clásico patrón lineal uniforme.[^3]
- Implementar un sistema de escenas o presets de performance que guarde de forma conjunta el estado de MIDI Brain, arpegiador, secuenciador y parámetros de master, con cambios suaves entre escenas para uso en directo.[^3]
- Incluir algoritmos de probabilidad avanzada y randomización controlada (por ejemplo, probability per step, humanización de timing y velocity) para enriquecer el comportamiento del arpegiador y del secuenciador sin perder control sobre el resultado.[^3]


#### 3. Interfaz de usuario avanzada

- Incorporar vistas de análisis adicionales, como un analizador de espectro en tiempo real y medidores de loudness (LUFS) integrados en el MasterBusComponent, facilitando decisiones más informadas durante el mastering.[^4][^5]
- Añadir funciones de edición múltiple y operaciones “tipo DAW” en el SequencerGridComponent (copiar/pegar secciones, doblar/halvear longitud de patrón, randomización por rango), para acelerar el flujo de trabajo creativo.[^3]
- Implementar temas visuales (skins) y layouts configurables, de forma que el sistema pueda adaptarse a distintos contextos: directo, estudio, docencia o demostraciones técnicas.


#### 4. Integración externa y automatización

- Exponer el motor como plugin VST3/AU mediante JUCE, permitiendo utilizar el sistema dentro de DAWs estándar y aprovechar automatización de host, sincronía de transporte y ruteo avanzado de audio/MIDI.[^5]
- Diseñar una capa de automatización interna que permita grabar y reproducir cambios de parámetros (por ejemplo, movimientos de ModWheel, mezcla de multibanda, ceiling del limitador) en tiempo, funcionando como una pista de automatización simplificada.[^4]
- Añadir soporte para controladores hardware dedicados (MIDI controllers con pads y faders), con mapeos por defecto y plantillas, para mejorar la ergonomía en directo y en estudio.


#### 5. Robustez, pruebas y optimización

- Desarrollar una batería de tests automatizados para validar el comportamiento de los módulos DSP (por ejemplo, respuesta al impulso, verificación de que el limitador nunca supera el ceiling y de que el multibanda mantiene fase coherente en los puntos de cruce).[^1]
- Optimizar secciones críticas con SIMD (por ejemplo, procesamiento de bloques en la convolución y en el multibanda) y evaluar el impacto de diferentes tamaños de bloque en latencia y rendimiento.[^1]
- Implementar un sistema de logging y profiling interno que permita registrar uso de CPU, latencia y eventos MIDI/audio clave, facilitando el diagnóstico de problemas en configuraciones complejas.

Estas líneas de trabajo permitirían evolucionar el proyecto desde un prototipo avanzado y funcional hacia una herramienta de nivel casi comercial, reforzando tanto la calidad sonora como la flexibilidad de uso en entornos reales de producción y directo.[^2][^5][^4]

<div align="center">⁂</div>

[^1]: https://signalsmith-audio.co.uk/writing/2022/limiter/

[^2]: https://www.dsprelated.com/thread/12872/dsp-digital-complex-limiter-crest-factor-digital-audio-soft-clipping

[^3]: https://www.musicradar.com/tuition/tech/how-to-use-a-brickwall-limiter-in-a-mastering-chain-602356

[^4]: https://productionadvice.co.uk/brickwall-limiters/

[^5]: https://www.waves.com/which-mastering-limiter-should-i-use

