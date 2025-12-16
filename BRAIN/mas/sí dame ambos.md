<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

# sí dame ambos

A continuación van ambos: un mini manual de usuario práctico y una lista de escenarios de uso, listos para copiar en la memoria.

***

## Mini manual de usuario

### 1. Panel de Performance (MasterControlPanel)

El panel de performance agrupa los controles principales para tocar y modular el sinte en tiempo real: rueda de modulación, pitch bend, teclado virtual, osciloscopio y ring modulator. Permite tocar desde MIDI externo o desde el propio teclado gráfico, visualizar la forma de onda y aplicar modulaciones expresivas sobre el sonido en tiempo real.[^1]

Uso básico:

- Tocar notas en el teclado virtual dispara notas hacia el MIDI Brain, que las distribuye entre voces del sintetizador.
- La rueda de modulación ajusta parámetros asignados (por ejemplo, apertura de filtro o profundidad de vibrato) mediante el sistema de mapeo de CC del MIDI Brain.
- El pitch bender introduce variaciones de afinación temporales; su rango real se define en el motor de síntesis o en el MIDI Brain.[^2]

Controles:

- ModWheel: arrastrar verticalmente para variar de 0 a 127; su efecto depende de las asignaciones CC↔parámetro realizadas (MIDI Learn).
- PitchBender: arrastrar horizontalmente alrededor del centro para doblar o relajar el tono; se recomienda usarlo para glissandi lentos o efectos de “dive”.
- Osciloscopio: muestra la forma de onda de salida, permitiendo ver transitorios, nivel RMS y pico; útil para ajustar dinámica y ganancia antes del bus de mastering.[^2]


### 2. Arpegiador visual (ArpeggiatorVisualComponent)

El arpegiador visual representa cada step como una columna con estado on/off, octava relativa y nivel de velocidad. El patrón se aplica a las notas entrantes, generando secuencias rítmico-melódicas sincronizadas con el reloj.[^1]

Uso:

- Cada columna corresponde a un step del arpegiador; el playhead recorre estas columnas en bucle.
- Haciendo clic en la parte central/baja del step se ajusta la velocidad (altura de la barra).
- Haciendo clic en la zona superior se cambia la octava relativa del step (por ejemplo, -1, 0, +1), creando patrones ascendentes o descendentes.[^1]

Interacción:

- Click izquierdo: activa/ajusta el step (enable + velocity/octava según zona).
- Click derecho: habilita o deshabilita el step rápidamente.
- El indicador de playhead señala el step que se está ejecutando en ese momento, ayudando a sincronizar cambios con la reproducción.[^1]


### 3. Secuenciador por pasos (SequencerGridComponent)

El secuenciador por pasos ofrece una rejilla 2D donde las columnas representan instantes de tiempo y las filas suelen asociarse a notas o pistas. Se pueden activar celdas individuales con una determinada velocidad, construyendo patrones rítmicos o polifónicos.[^1]

Uso:

- Cada celda activa disparará una nota/evento cuando el playhead pase por su columna, en función de la fila asignada.
- Arrastrar verticalmente dentro de una celda ajusta la velocidad, aumentando o reduciendo su intensidad relativa.
- El playhead vertical muestra la columna en reproducción, facilitando cambios “al vuelo”.[^1]

Interacción:

- Click izquierdo: activa o desactiva una celda; al arrastrar se pueden encender varias seguidas.
- Click derecho: limpia la celda, devolviéndola a estado inactivo con velocidad por defecto.
- El número de filas y columnas se puede parametrizar según el diseño del módulo de secuenciador.[^1]


### 4. Cadena de mastering (MasterBusComponent)

La cadena de mastering agrupa el compresor multibanda, el compresor de bus con lookahead y el limitador brickwall. Su objetivo es controlar rango dinámico y nivel máximo de salida con calidad de mastering.[^3][^4]

Uso:

- MultibandCompressor: ajustar crossovers y umbrales por banda para equilibrar graves, medios y agudos sin destruir el carácter del sonido.
- LookaheadCompressor: usar ratios moderadas y tiempos de ataque/release suaves para controlar transitorios globales de la mezcla.
- MasteringLimiter: fijar un ceiling cercano a 0 dBFS (por ejemplo, -0.1 dB) y aumentar input gain/soft clip con cuidado para ganar loudness sin distorsión audible.[^5][^2]

Indicadores:

- Los medidores de reducción de ganancia por banda y de la cadena global muestran cuánta compresión/limitación se está aplicando, ayudando a evitar un exceso de procesamiento.[^4][^3]


### 5. Componente principal (MainComponent)

El MainComponent organiza la disposición de todos los paneles y gestiona la conexión con las entradas MIDI y el reloj interno. Es el punto de entrada de la aplicación y coordina tanto UX como flujo de audio/MIDI.[^1]

Funciones:

- Recepción de mensajes MIDI externos y reenvío al MIDI Brain.
- Avance del playhead del arpegiador y del secuenciador a través de un timer o reloj dedicado, manteniendo sincronía visual y musical.
- Distribución del espacio de pantalla entre panel de performance, panel de arpegiador/secuenciador y bus de mastering.[^1]

***

## Escenarios de uso (para la memoria)

### Escenario 1: Performance en directo con arpegiador

En este escenario se conecta un teclado MIDI físico al sistema y se utiliza el panel de performance junto con el arpegiador visual. El intérprete toca acordes o notas sueltas que el MIDI Brain organiza en voces, mientras el ArpeggiatorVisualComponent aplica un patrón de pasos con diferentes octavas y velocidades. Las variaciones en la rueda de modulación y el pitch bend se asignan a parámetros clave (filtro, vibrato, profundidad de efectos), permitiendo expresividad en tiempo real sin necesidad de interacción compleja con menús.[^2][^1]

La salida de audio pasa por el Orchestrator y, opcionalmente, por la cadena de mastering en modo ligero (ajustes conservadores de compresión y limitación) para asegurar un nivel estable en sistemas de PA o streaming. Este flujo convierte el sistema en un instrumento tocable, con un equilibrio entre control fino y feedback visual inmediato.[^3][^4]

### Escenario 2: Diseño de patrones rítmicos y melódicos

Aquí el foco está en el trabajo de programación de patrones antes de una actuación o grabación. El usuario emplea el SequencerGridComponent para construir patrones de varias compases, activando celdas específicas con distintas velocidades para crear acentos y síncopas. Paralelamente, el ArpeggiatorVisualComponent define la estructura melódica sobre la que se proyectan las notas entrantes (ya sea desde MIDI o desde un patrón base).[^1]

El sistema permite reproducir estos patrones en bucle, ajustar BPM mediante el reloj interno y refinar tanto la densidad rítmica como la articulación de cada evento. Una vez satisfecho con el resultado, el usuario puede grabar la salida de audio o exportar los patrones como base para producciones posteriores. La interfaz visual ayuda a detectar rápidamente huecos, repeticiones y zonas con exceso de densidad.[^1]

### Escenario 3: Cadena de mastering interna para stems o mezcla estéreo

En este caso, el sistema se utiliza como procesador de mastering ligero aplicado a la salida estéreo del Orchestrator o a stems internos. La señal se envía al MasterChainModule donde el MultibandCompressorModule equilibra el espectro, suavizando graves excesivos o realzando presencia en medios. A continuación, el LookaheadCompressorModule controla transitorios agresivos sin recorte brusco, gracias a su capacidad de predicción, y el MasteringLimiterModule garantiza que el nivel máximo no exceda el techo configurado, añadiendo un último nivel de protección y loudness.[^5][^3][^2]

Este flujo permite al usuario obtener mezclas listas para distribución o presentación sin depender de herramientas externas adicionales. Mediante los medidores de reducción de ganancia y el control de soft clipping, se busca un compromiso entre volumen percibido, impacto de transitorios y ausencia de distorsiones audibles, emulando prácticas habituales en cadenas de mastering profesionales.[^6][^5]

### Escenario 4: Entorno de experimentación y prototipado

Por último, el entorno también sirve como laboratorio para probar ideas de síntesis, control y dinámica. La estrecha integración entre MIDI Brain, UI gráfica y módulos DSP permite cambiar rápidamente la topología de patch, experimentar con diferentes curvas de velocidad, patrones de arpegiador y secuenciador, así como con configuraciones de mastering.[^2][^1]

El usuario puede, por ejemplo, probar un mismo patrón a través de configuraciones extremas de multibanda y limitador, observar en el osciloscopio cómo cambian los transitorios y evaluar el resultado auditivamente. Este tipo de flujo iterativo resulta especialmente útil en contextos académicos o de investigación, donde se comparan algoritmos y configuraciones de procesamiento de audio.[^6][^3]

***

Si quieres, el siguiente paso puede ser afinar el texto para encajar en el formato concreto de tu memoria (por ejemplo, secciones y longitud requeridas) o generar una breve sección de “trabajos futuros / mejoras posibles” para cerrar el documento.

<div align="center">⁂</div>

[^1]: https://www.musicradar.com/tuition/tech/how-to-use-a-brickwall-limiter-in-a-mastering-chain-602356

[^2]: https://signalsmith-audio.co.uk/writing/2022/limiter/

[^3]: https://productionadvice.co.uk/brickwall-limiters/

[^4]: https://www.waves.com/which-mastering-limiter-should-i-use

[^5]: https://www.izotope.com/en/learn/brickwall-limiters

[^6]: https://www.dsprelated.com/thread/12872/dsp-digital-complex-limiter-crest-factor-digital-audio-soft-clipping

