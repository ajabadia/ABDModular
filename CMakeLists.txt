cmake_minimum_required(VERSION 3.15)
project(ModularSuite)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# JUCE Integration
# -----------------------------------------------------------------------------
# Check common locations
if(EXISTS "C:/JUCE/CMakeLists.txt")
    add_subdirectory("C:/JUCE" build_juce)
elseif(EXISTS "D:/JUCE/CMakeLists.txt")
     add_subdirectory("D:/JUCE" build_juce)
endif()

# -----------------------------------------------------------------------------
# Sources (Shared)
# -----------------------------------------------------------------------------
set(MODULAR_SOURCES
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

# -----------------------------------------------------------------------------
# Target: ModularTest (Console)
# -----------------------------------------------------------------------------
add_executable(ModularTest 
    Source/Tests/ModularTest.cpp
    ${MODULAR_SOURCES}
)
target_include_directories(ModularTest PRIVATE Source)

# -----------------------------------------------------------------------------
# Target: ModularSandbox (GUI)
# -----------------------------------------------------------------------------
if(TARGET juce::juce_gui_basics)
    message(STATUS "JUCE found - building ModularSandbox")
    
    juce_add_gui_app(ModularSandbox
        PRODUCT_NAME "ModularSandbox"
        VERSION "1.0.0"
    )
    
    target_sources(ModularSandbox PRIVATE 
        Source/Apps/ModularSandbox/Main.cpp
        ${MODULAR_SOURCES}
    )
    
    target_link_libraries(ModularSandbox PRIVATE
        juce::juce_gui_basics
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_graphics
        juce::juce_data_structures
        juce::juce_events
    )
    
    target_include_directories(ModularSandbox PRIVATE Source)
else()
    message(WARNING "JUCE not found at C:/JUCE or D:/JUCE. ModularSandbox will not be built.")
endif()
