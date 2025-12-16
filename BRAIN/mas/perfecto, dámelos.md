<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

# perfecto, dámelos

Se puede montar un master bus modular donde la señal pase en serie por MultibandCompressor, LookaheadCompressor y MasteringLimiter, actuando como cadena de mastering al final del patch.[^1][^2]

***

## 🧱 MasterChainModule (modular DSP)

### Archivo: `Source/Modular/Modules/MasterChainModule.h`

```cpp
#pragma once
#include "../Module.h"
#include "MultibandCompressorModule.h"
#include "LookaheadCompressorModule.h"
#include "MasteringLimiterModule.h"
#include <memory>

namespace Modular::Modules
{
    /**
     * MasterChainModule v1.0
     *
     * Routing:
     *    INPUT → MultibandCompressor → LookaheadCompressor → MasteringLimiter → OUTPUT
     */

    class MasterChainModule : public Module
    {
    public:
        MasterChainModule()
        {
            multiband_  = std::make_unique<MultibandCompressorModule>();
            lookahead_  = std::make_unique<LookaheadCompressorModule>();
            limiter_    = std::make_unique<MasteringLimiterModule>();

            std::cout << "🎛️ MasterChainModule inicializado\n";
        }

        std::string getModuleName() const override     { return "MasterChain"; }
        std::string getModuleCategory() const override { return "Master"; }

        void prepare(double sampleRate, int /*blockSize*/) override
        {
            Module::prepare(sampleRate, 0);
            multiband_->prepare(sampleRate, 0);
            lookahead_->prepare(sampleRate, 0);
            limiter_->prepare(sampleRate, 0);
        }

        void reset() override
        {
            multiband_->reset();
            lookahead_->reset();
            limiter_->reset();
        }

        AudioFrame processFrame(const AudioFrame& in) override
        {
            AudioFrame x = in;
            x  = multiband_->processFrame(x);
            x  = lookahead_->processFrame(x);
            x  = limiter_->processFrame(x);
            return x;
        }

        MultibandCompressorModule&   getMultiband()  { return *multiband_; }
        LookaheadCompressorModule&   getLookahead()  { return *lookahead_; }
        MasteringLimiterModule&      getLimiter()    { return *limiter_; }

    private:
        std::unique_ptr<MultibandCompressorModule> multiband_;
        std::unique_ptr<LookaheadCompressorModule> lookahead_;
        std::unique_ptr<MasteringLimiterModule>    limiter_;
    };
}
```


***

## 🎚️ MasterBusComponent (UI para la cadena)

### Archivo: `Source/UI/MasterBusComponent.h`

```cpp
#pragma once
#include <JuceHeader.h>
#include "../Modular/Modules/MasterChainModule.h"

class MasterBusComponent : public juce::Component
{
public:
    explicit MasterBusComponent(Modular::Modules::MasterChainModule& chain)
        : chain_(chain),
          multiband_(chain.getMultiband()),
          lookahead_(chain.getLookahead()),
          limiter_(chain.getLimiter())
    {
        addAndMakeVisible(multiband_);
        addAndMakeVisible(lookahead_);
        addAndMakeVisible(limiter_);

        setSize(900, 320);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(10);
        auto w = r.getWidth() / 3;

        multiband_.setBounds(r.removeFromLeft(w).reduced(5));
        lookahead_.setBounds(r.removeFromLeft(w).reduced(5));
        limiter_.setBounds(r.reduced(5));
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(12, 12, 18));
        g.setColour(juce::Colours::white);
        g.setFont(16.0f);
        g.drawText("MASTER BUS", 10, 5, getWidth()-20, 20, juce::Justification::centredTop);
    }

private:
    Modular::Modules::MasterChainModule&        chain_;
    Modular::Modules::MultibandCompressorModule& multiband_;
    Modular::Modules::LookaheadCompressorModule& lookahead_;
    Modular::Modules::MasteringLimiterModule&    limiter_;
};
```


***

## 🧩 Integración en ventana principal

En tu `MainComponent` / `MainEditor` solo necesitas algo así (esquema):

```cpp
class MainComponent : public juce::Component
{
public:
    MainComponent()
        : masterChain_(),
          masterBus_(masterChain_)
    {
        addAndMakeVisible(masterBus_);
        // addAndMakeVisible(arpSeqPanel_);
        // addAndMakeVisible(masterControlPanel_);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(10);
        masterBus_.setBounds(r.removeFromBottom(320));
        // arpSeqPanel_.setBounds(r.removeFromTop(r.getHeight()/2));
        // masterControlPanel_.setBounds(r);
    }

private:
    Modular::Modules::MasterChainModule masterChain_;
    MasterBusComponent                   masterBus_;
    // ArpSeqPanel                        arpSeqPanel_;
    // MasterControlPanel                 masterControlPanel_;
};
```


***

Si quieres, el siguiente paso puede ser:

- A) Esqueleto de `MainComponent` completo (MIDI Brain + ArpSeqPanel + MasterBusComponent)
- B) Integrar callbacks reales (MIDI, clock, playhead) entre `MidiBrain`, `ArpSeqPanel` y `SequencerModule`
- C) Escribir la documentación técnica del sistema (para la memoria del proyecto)

<div align="center">⁂</div>

[^1]: https://productionadvice.co.uk/brickwall-limiters/

[^2]: https://www.waves.com/which-mastering-limiter-should-i-use

