<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

# no, dame el código de Gate + Transient Shaper (final touch) tal y cómo venías haciendo

Un gate + transient shaper combinan detección de envolvente con umbral y curvas distintas para ataque y sustain, permitiendo cortar ruido de fondo y realzar o suavizar transitorios sin cambiar demasiado el RMS.[^1]

## Gate + Transient Shaper (JUCE + Modular)

### Archivo: `Source/Modular/Modules/GateTransientShaperModule.h`

```cpp
#pragma once
#include <JuceHeader.h>
#include "../Module.h"
#include <array>
#include <vector>
#include <cmath>

namespace Modular::Modules
{
    /**
     * GateTransientShaperModule v1.0
     *
     * ✅ Noise Gate con hysteresis
     * ✅ Transient Shaper (attack / sustain)
     * ✅ Envelope follower dual
     * ✅ Sidechain HPF básico
     * ✅ Hard/Soft mode
     * ✅ JUCE UI con metering
     */

    class GateTransientShaperModule : public Module,
                                      public juce::Component,
                                      public juce::Slider::Listener,
                                      public juce::ComboBox::Listener
    {
    public:
        struct EnvelopeState
        {
            float env = 0.0f;
            float attackMs = 1.0f;
            float releaseMs = 80.0f;
        };

        GateTransientShaperModule()
        {
            createParameters();
            createUI();
            std::cout << "🎯 Gate + Transient Shaper inicializado\n";
        }

        std::string getModuleName() const override     { return "GateTransientShaper"; }
        std::string getModuleCategory() const override { return "Effect"; }

        void reset() override
        {
            gateEnv_.env      = 0.0f;
            transEnv_.env     = 0.0f;
            lastInput_        = 0.0f;
            gainSmooth_       = 1.0f;
            gateOpen_         = false;
        }

        // ════════════════════════════════════════════════════════
        // AUDIO PROCESSING
        // ════════════════════════════════════════════════════════

        AudioFrame processFrame(const AudioFrame& input) override
        {
            if (getBypass())
                return input;

            float x = input.samples[^0];

            // Sidechain HPF simple (opcional, fijo ~80Hz)
            float sc = sidechainHPF(x);

            // Envelope para gate
            float gateEnv = updateEnvelope(gateEnv_, std::fabs(sc));
            // Envelope rápida para transients
            float transEnv = updateEnvelope(transEnv_, std::fabs(sc));

            // GATE: hysteresis
            float threshOpen  = juce::Decibels::decibelsToGain(gateThresholdDb_);
            float threshClose = juce::Decibels::decibelsToGain(gateThresholdDb_ - gateHysteresisDb_);

            if (!gateOpen_ && gateEnv > threshOpen)
                gateOpen_ = true;
            else if (gateOpen_ && gateEnv < threshClose)
                gateOpen_ = false;

            float gateTargetGain = gateOpen_ ? 1.0f : gateFloorGain_; // floor en -∞..0 dB

            // TRANSIENT SHAPER:
            // Separa ataque rápido y cuerpo (sustain) más lento
            float attackSignal  = x - lastInput_;           // componente de cambio rápido
            float sustainSignal = x - attackSignal;         // resto

            float attackGain  = 1.0f + attackAmount_;       // -1..+1
            float sustainGain = 1.0f + sustainAmount_;      // -1..+1

            float y = attackSignal * attackGain + sustainSignal * sustainGain;

            lastInput_ = x;

            // Mezcla con gate
            float targetGain = gateTargetGain;
            // Soft mode: algo de soft-knee sobre targetGain
            if (mode_ == Mode::Soft)
            {
                targetGain = std::tanh(targetGain * 1.5f);
            }

            // Smoothing de la ganancia global
            float smoothCoef = 0.005f;
            gainSmooth_ = gainSmooth_ + smoothCoef * (targetGain - gainSmooth_);

            float out = y * gainSmooth_;

            // Salvar valores para meter
            gateEnvDisplay_   = gateEnv;
            transEnvDisplay_  = transEnv;
            gateGainDisplay_  = gainSmooth_;

            AudioFrame result = input;
            result.samples[^0] = out;
            return result;
        }

        // ════════════════════════════════════════════════════════
        // UI
        // ════════════════════════════════════════════════════════

        void paint(juce::Graphics& g) override
        {
            g.fillAll(juce::Colour(30, 30, 40));

            g.setColour(juce::Colours::white);
            g.setFont(16.0f);
            g.drawText("Gate + Transient Shaper", 10, 5, getWidth() - 20, 20,
                       juce::Justification::centredLeft);

            drawMeters(g);
        }

        void resized() override
        {
            auto area = getLocalBounds().reduced(10);

            auto topRow = area.removeFromTop(40);
            threshSlider_.setBounds(topRow.removeFromLeft(120));
            floorSlider_.setBounds(topRow.removeFromLeft(120));
            hysteresisSlider_.setBounds(topRow.removeFromLeft(120));
            modeBox_.setBounds(topRow);

            auto midRow = area.removeFromTop(40);
            attackAmtSlider_.setBounds(midRow.removeFromLeft(120));
            sustainAmtSlider_.setBounds(midRow.removeFromLeft(120));
            transAttackSlider_.setBounds(midRow.removeFromLeft(120));
            transReleaseSlider_.setBounds(midRow);

            meterArea_ = area.reduced(0, 10);
        }

        void sliderValueChanged(juce::Slider* s) override
        {
            if (s == &threshSlider_)
                gateThresholdDb_ = (float)threshSlider_.getValue();
            else if (s == &floorSlider_)
                gateFloorGain_ = juce::Decibels::decibelsToGain((float)floorSlider_.getValue());
            else if (s == &hysteresisSlider_)
                gateHysteresisDb_ = (float)hysteresisSlider_.getValue();
            else if (s == &attackAmtSlider_)
                attackAmount_ = (float)attackAmtSlider_.getValue();
            else if (s == &sustainAmtSlider_)
                sustainAmount_ = (float)sustainAmtSlider_.getValue();
            else if (s == &transAttackSlider_)
                transEnv_.attackMs = (float)transAttackSlider_.getValue();
            else if (s == &transReleaseSlider_)
                transEnv_.releaseMs = (float)transReleaseSlider_.getValue();

            repaint();
        }

        void comboBoxChanged(juce::ComboBox* box) override
        {
            if (box == &modeBox_)
            {
                mode_ = (Mode)(modeBox_.getSelectedId() - 1);
            }
        }

    private:
        // Parámetros lógicos
        float gateThresholdDb_   = -40.0f;
        float gateHysteresisDb_  = 6.0f;
        float gateFloorGain_     = 0.0f;   // 0 → mute total, 1 → sin gate
        float attackAmount_      = 0.3f;   // -1..+1
        float sustainAmount_     = -0.2f;  // -1..+1

        EnvelopeState gateEnv_   { 0.0f, 5.0f, 100.0f };
        EnvelopeState transEnv_  { 0.0f, 1.0f, 50.0f };

        float lastInput_         = 0.0f;
        float gainSmooth_        = 1.0f;
        bool  gateOpen_          = true;

        // Sidechain HPF state
        float scHPState_         = 0.0f;

        // Meters
        float gateEnvDisplay_    = 0.0f;
        float transEnvDisplay_   = 0.0f;
        float gateGainDisplay_   = 1.0f;
        juce::Rectangle<int> meterArea_;

        enum class Mode { Hard = 0, Soft = 1 };
        Mode mode_ = Mode::Soft;

        // UI
        juce::Slider threshSlider_, floorSlider_, hysteresisSlider_;
        juce::Slider attackAmtSlider_, sustainAmtSlider_;
        juce::Slider transAttackSlider_, transReleaseSlider_;
        juce::ComboBox modeBox_;

        // ════════════════════════════════════════════════════════
        // DSP helpers
        // ════════════════════════════════════════════════════════

        float updateEnvelope(EnvelopeState& st, float inAbs)
        {
            float atkCoef = std::exp(-1.0f / (st.attackMs  * sampleRate_ / 1000.0f));
            float relCoef = std::exp(-1.0f / (st.releaseMs * sampleRate_ / 1000.0f));

            if (inAbs > st.env)
                st.env = atkCoef * st.env + (1.0f - atkCoef) * inAbs;
            else
                st.env = relCoef * st.env + (1.0f - relCoef) * inAbs;

            return st.env;
        }

        float sidechainHPF(float x)
        {
            float fc = 80.0f / sampleRate_;
            if (fc < 0.0005f) fc = 0.0005f;
            scHPState_ = scHPState_ + fc * (x - scHPState_);
            return x - scHPState_;
        }

        // ════════════════════════════════════════════════════════
        // UI helpers
        // ════════════════════════════════════════════════════════

        void createParameters()
        {
            // Si quieres engancharlo al sistema de parámetros global:
            // registerParameter(...) aquí
        }

        void setupSlider(juce::Slider& s, const juce::String& name,
                         float min, float max, float def)
        {
            s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            s.setRange(min, max, 0.1);
            s.setValue(def);
            s.setTooltip(name);
            s.addListener(this);
            addAndMakeVisible(s);
        }

        void createUI()
        {
            setupSlider(threshSlider_,     "Gate Threshold (dB)", -80.0f, 0.0f, gateThresholdDb_);
            setupSlider(floorSlider_,      "Gate Floor (dB)",    -80.0f, 0.0f, -80.0f);
            setupSlider(hysteresisSlider_, "Hysteresis (dB)",      0.0f, 24.0f, gateHysteresisDb_);

            setupSlider(attackAmtSlider_,  "Attack Amount",      -1.0f, 1.0f, attackAmount_);
            setupSlider(sustainAmtSlider_, "Sustain Amount",     -1.0f, 1.0f, sustainAmount_);

            setupSlider(transAttackSlider_,"Trans Attack (ms)",    0.1f, 50.0f, transEnv_.attackMs);
            setupSlider(transReleaseSlider_,"Trans Release (ms)",  5.0f, 300.0f, transEnv_.releaseMs);

            modeBox_.addItem("Hard", 1);
            modeBox_.addItem("Soft", 2);
            modeBox_.setSelectedId(2);
            modeBox_.addListener(this);
            addAndMakeVisible(modeBox_);
        }

        void drawMeters(juce::Graphics& g)
        {
            auto r = meterArea_;
            int w = 60;
            int h = r.getHeight() - 20;

            auto drawBar = [&](int x, float value, juce::Colour c, const juce::String& label)
            {
                value = juce::jlimit(0.0f, 1.0f, value);
                g.setColour(juce::Colours::darkgrey);
                g.fillRoundedRectangle((float)x, (float)r.getY(), (float)w, (float)h, 3.0f);

                g.setColour(c);
                float filled = h * value;
                g.fillRoundedRectangle((float)x + 2.0f,
                                       (float)(r.getY() + h - filled + 2.0f),
                                       (float)w - 4.0f,
                                       filled - 4.0f,
                                       3.0f);

                g.setColour(juce::Colours::white);
                g.setFont(11.0f);
                g.drawText(label, x, r.getBottom() - 16, w, 16, juce::Justification::centred);
            };

            // Env a dB aprox
            float gateEnvDb  = juce::Decibels::gainToDecibels(gateEnvDisplay_ + 1e-6f, -80.0f);
            float transEnvDb = juce::Decibels::gainToDecibels(transEnvDisplay_ + 1e-6f, -80.0f);
            float grDb       = juce::Decibels::gainToDecibels(gateGainDisplay_ + 1e-6f, -80.0f);

            drawBar(r.getX(),                (gateEnvDb  + 80.0f) / 80.0f,
                    juce::Colours::cyan,    "Gate Env");
            drawBar(r.getX() + w + 10,       (transEnvDb + 80.0f) / 80.0f,
                    juce::Colours::orange,  "Trans Env");
            drawBar(r.getX() + 2*(w + 10),   1.0f - (grDb + 80.0f) / 80.0f,
                    juce::Colours::red,     "Gain");
        }
    };
}
```

¿Te preparo ahora el **Mastering Limiter brickwall** en el mismo estilo (módulo + UI JUCE) o prefieres que lo integre directamente al final de la cadena con los otros módulos que ya tienes?

<div align="center">⁂</div>

[^1]: https://signalsmith-audio.co.uk/writing/2022/limiter/

