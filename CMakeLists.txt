cmake_minimum_required(VERSION 3.15)
project(ModularTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    Source/Tests/ModularTest.cpp
    Source/Modular/Core/Foundation/v1/Types.h
    Source/Modular/Core/Foundation/v1/Module.h
    Source/Modular/Core/VoiceManager/v1/VoiceManager.h
    Source/Modular/Modules/Oscillators/StandardOscillator/v1/StandardOscillator.h
    Source/Modular/Modules/Envelopes/ADSREnvelope/v1/ADSREnvelope.h
    Source/Modular/Modules/Filters/StandardFilter/v1/StandardFilter.h
    Source/Modular/Modules/Mixers/SimpleMixer/v1/SimpleMixer.h
    Source/Modular/Modules/LFOs/StandardLFO/v1/StandardLFO.h
    Source/Modular/Modules/Sequencers/PolySequencer/v1/PolySequencer.h
    Source/Modular/Modules/Effects/StandardDelay/v1/StandardDelay.h
    Source/Modular/Modules/Effects/StandardChorus/v1/StandardChorus.h
    Source/Modular/Modules/Effects/StandardReverb/v1/StandardReverb.h
)

add_executable(ModularTest ${SOURCES})

# Include directories
target_include_directories(ModularTest PRIVATE Source)
