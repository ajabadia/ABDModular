# VoiceManager v1

## Información General
**Nombre**: VoiceManager  
**Versión**: 1.0.0  
**Categoría**: Core  
**Descripción**: Utilidad lógica para la gestión de polifonía. No procesa audio, solo gestiona la asignación de notas MIDI a índices de voz virtuales.

## Funcionalidad

### Voice Allocation
- **Round Robin**: Intenta usar siempre una voz libre diferente para evitar cortar colas de release natural.
- **Voice Stealing (LRU)**: Si no hay voces libres, roba la voz más antigua (Least Recently Used).

### Interfaz Pública
- `findVoiceIndex(note, velocity)`: Devuelve el index (0..N-1) para tocar una nota.
- `noteOff(note)`: Libera la voz asociada a una nota MIDI.

### Uso
```cpp
VoiceManager vm(16); // 16 voces

// Note On
int voiceIdx = vm.findVoiceIndex(60, 0.8f);
mySynthesizer.voices[voiceIdx].trigger();

// Note Off
int voiceIdx = vm.noteOff(60);
if (voiceIdx >= 0) mySynthesizer.voices[voiceIdx].release();
```

### Historial de Cambios
- **v1.0.0**: Gestión básica de polifonía y stealing por antigüedad.
