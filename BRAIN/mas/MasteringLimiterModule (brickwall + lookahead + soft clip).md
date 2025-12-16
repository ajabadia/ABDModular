<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

# hazlo como lo venias haciedo hasta ahora

Un brickwall limiter de mastering debe tener techo fijo (ceiling), lookahead corto, medidor de reducción de ganancia y, a menudo, una etapa de soft clipping para aprovechar más loudness sin distorsión evidente.[^1][^2]

## MasteringLimiterModule (brickwall + lookahead + soft clip)

### Archivo: `Source/Modular/Modules/MasteringLimiterModule.h`

```cpp
#pragma once
#include <JuceHeader.h>
#include "../Module.h"
#include <vector>
#include <cmath>

namespace Modular::Modules
{
    /**
     * MasteringLimiterModule v1.0 - Brickwall Master Limiter
     *
     * ✅ Brickwall Ceiling (dBFS)
     * ✅ Lookahead (0.5–10 ms)
     * ✅ Input Gain (+/- dB)
     * ✅ Program-Dependent Release
     * ✅ Soft Clip Stage (pre/ post)
     * ✅ True Peak-ish (4x oversample simple)
     * ✅ Gain Reduction Metering
     * ✅ JUCE UI integrado
     */

    class MasteringLimiterModule : public Module,
                                   public juce::Component,
                                   public juce::Slider::Listener,
                                   public juce::ComboBox::Listener
    {
    public:
        MasteringLimiterModule()
            : oversampleFactor_(4),
              lookaheadMs_(3.0f),
              ceilingDb_(-0.1f),
              inputGainDb_(0.0f),
              releaseMs_(80.0f),
              softClipAmount_(0.2f)
        {
            createParameters();
            createUI();
            prepareBuffers();

            std::cout << "🔒 Mastering Limiter (Brickwall) inicializado\n";
        }

        std::string getModuleName() const override     { return "MasteringLimiter"; }
        std::string getModuleCategory() const override { return "Master"; }

        void reset() override
        {
            std::fill(lookaheadBuffer_.begin(), lookaheadBuffer_.end(), 0.0f);
            writePos_    = 0;
            env_         = 0.0f;
            grDisplayDb_ = 0.0f;
        }

        // ════════════════════════════════════════════════════════
        // AUDIO PROCESSING
        // ════════════════════════════════════════════════════════

        AudioFrame processFrame(const AudioFrame& input) override
        {
            if (getBypass())
                return input;

            // Update (por si cambian en runtime)
            updateRuntimeParams();

            // 1) Aplicar input gain
            float x = input.samples[^0] * juce::Decibels::decibelsToGain(inputGainDb_);

            // 2) Opcional: soft clipping PRE-limitador para controlar picos agresivos
            float preClipped = softClip(x, softClipAmount_ * 0.5f);

            // 3) Guardar en buffer de lookahead
            pushLookaheadSample(preClipped);

            // 4) Estimar pico futuro (true-peak-ish con oversampling simple)
            float futurePeak = scanLookaheadPeak();

            // 5) Calcular ganancia necesaria para no superar ceiling
            float desiredGain = computeLimiterGain(futurePeak);

            // 6) Suavizar ganancia (release program-dependent)
            float smoothedGain = smoothGain(desiredGain);

            // 7) Leer sample “alineado” desde el buffer de lookahead
            float y = popLookaheadAligned() * smoothedGain;

            // 8) Soft-clip POST-limit (para sonido un poco más musical)
            float yPost = softClip(y, softClipAmount_);

            // 9) Clamp absoluto brickwall
            float linearCeiling = juce::Decibels::decibelsToGain(ceilingDb_);
            yPost = juce::jlimit(-linearCeiling, linearCeiling, yPost);

            // Métrica de GR
            float grDb = 0.0f;
            if (smoothedGain < 1.0f)
                grDb = -juce::Decibels::gainToDecibels(smoothedGain);
            grDisplayDb_ = 0.9f * grDisplayDb_ + 0.1f * grDb;

            AudioFrame out = input;
            out.samples[^0] = yPost;
            return out;
        }

        // ════════════════════════════════════════════════════════
        // UI
        // ════════════════════════════════════════════════════════

        void paint(juce::Graphics& g) override
        {
            g.fillAll(juce::Colour(25, 25, 35));

            g.setColour(juce::Colours::white);
            g.setFont(16.0f);
            g.drawText("Mastering Limiter (Brickwall)", 10, 5, getWidth() - 20, 20,
                       juce::Justification::centredLeft);

            drawMeters(g);
        }

        void resized() override
        {
            auto area = getLocalBounds().reduced(10);

            auto row1 = area.removeFromTop(35);
            inputGainSlider_.setBounds(row1.removeFromLeft(120));
            ceilingSlider_.setBounds(row1.removeFromLeft(120));
            lookaheadSlider_.setBounds(row1.removeFromLeft(120));
            releaseSlider_.setBounds(row1.removeFromLeft(120));

            auto row2 = area.removeFromTop(35);
            softClipSlider_.setBounds(row2.removeFromLeft(120));
            oversampleBox_.setBounds(row2.removeFromLeft(120));

            meterArea_ = area.reduced(10, 10);
        }

        void sliderValueChanged(juce::Slider* s) override
        {
            if (s == &inputGainSlider_)
                inputGainDb_ = (float)inputGainSlider_.getValue();
            else if (s == &ceilingSlider_)
                ceilingDb_ = (float)ceilingSlider_.getValue();
            else if (s == &lookaheadSlider_)
            {
                lookaheadMs_ = (float)lookaheadSlider_.getValue();
                prepareBuffers();
            }
            else if (s == &releaseSlider_)
                releaseMs_ = (float)releaseSlider_.getValue();
            else if (s == &softClipSlider_)
                softClipAmount_ = (float)softClipSlider_.getValue();

            repaint();
        }

        void comboBoxChanged(juce::ComboBox* box) override
        {
            if (box == &oversampleBox_)
            {
                int id = oversampleBox_.getSelectedId();
                oversampleFactor_ = (id == 1 ? 1 : id == 2 ? 2 : 4);
                prepareBuffers();
            }
        }

    private:
        // Parámetros internos
        int   oversampleFactor_;
        float lookaheadMs_;
        float ceilingDb_;
        float inputGainDb_;
        float releaseMs_;
        float softClipAmount_;

        // Estado DSP
        std::vector<float> lookaheadBuffer_;
        int  writePos_   = 0;
        int  delaySamples_ = 0;
        float env_       = 0.0f; // envolvente de reducción
        float grDisplayDb_ = 0.0f;

        // UI
        juce::Slider inputGainSlider_, ceilingSlider_, lookaheadSlider_;
        juce::Slider releaseSlider_, softClipSlider_;
        juce::ComboBox oversampleBox_;
        juce::Rectangle<int> meterArea_;

        // ════════════════════════════════════════════════════════
        // DSP helpers
        // ════════════════════════════════════════════════════════

        void prepareBuffers()
        {
            float maxLookaheadMs = 10.0f;
            int maxSamples = (int)(maxLookaheadMs * 0.001f * sampleRate_ * oversampleFactor_);
            lookaheadBuffer_.assign(maxSamples + 4, 0.0f);

            delaySamples_ = (int)(lookaheadMs_ * 0.001f * sampleRate_ * oversampleFactor_);
            writePos_     = 0;
        }

        void updateRuntimeParams()
        {
            // Aquí podrías linkar con parámetros globales si hace falta
        }

        void pushLookaheadSample(float x)
        {
            // Oversampling “ingenuo” (hold)
            for (int i = 0; i < oversampleFactor_; ++i)
            {
                lookaheadBuffer_[writePos_] = x;
                writePos_ = (writePos_ + 1) % (int)lookaheadBuffer_.size();
            }
        }

        float popLookaheadAligned() const
        {
            int readPos = writePos_ - delaySamples_;
            if (readPos < 0)
                readPos += (int)lookaheadBuffer_.size();

            return lookaheadBuffer_[readPos];
        }

        float scanLookaheadPeak() const
        {
            float peak = 0.0f;
            int size   = (int)lookaheadBuffer_.size();

            int start = writePos_;
            for (int i = 0; i < delaySamples_; ++i)
            {
                float s = std::abs(lookaheadBuffer_[(start + i) % size]);
                if (s > peak) peak = s;
            }

            return peak;
        }

        float computeLimiterGain(float peak)
        {
            if (peak <= 0.0f)
                return 1.0f;

            float linearCeiling = juce::Decibels::decibelsToGain(ceilingDb_);
            if (peak <= linearCeiling)
                return 1.0f;

            float needed = linearCeiling / peak;
            return juce::jlimit(0.0f, 1.0f, needed);
        }

        float smoothGain(float targetGain)
        {
            // Release program-dependent: si limitador está actuando mucho, alarga release
            float baseReleaseMs = releaseMs_;
            if (targetGain < 0.7f)
                baseReleaseMs *= 1.5f;

            float relCoef = std::exp(-1.0f / (baseReleaseMs * sampleRate_ / 1000.0f));

            // En ataque (cuando target < env_) aplicamos casi instantáneo
            if (targetGain < env_)
                env_ = targetGain;
            else
                env_ = env_ * relCoef + targetGain * (1.0f - relCoef);

            return env_;
        }

        float softClip(float x, float amount)
        {
            if (amount <= 0.001f)
                return x;

            // Curva tanh con mezcla
            float clipped = std::tanh(x * (1.0f + amount * 4.0f));
            return x * (1.0f - amount) + clipped * amount;
        }

        // ════════════════════════════════════════════════════════
        // UI helpers
        // ════════════════════════════════════════════════════════

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

        void createParameters()
        {
            // Si quieres integrarlo con tu ParameterSpec, definir aquí.
        }

        void createUI()
        {
            setupSlider(inputGainSlider_, "Input Gain (dB)", -12.0f, 12.0f, inputGainDb_);
            setupSlider(ceilingSlider_,   "Ceiling (dBFS)",  -3.0f, 0.0f,  ceilingDb_);
            setupSlider(lookaheadSlider_, "Lookahead (ms)",  0.5f, 10.0f, lookaheadMs_);
            setupSlider(releaseSlider_,   "Release (ms)",   10.0f, 500.0f, releaseMs_);
            setupSlider(softClipSlider_,  "Soft Clip",       0.0f, 1.0f,  softClipAmount_);

            oversampleBox_.addItem("1x", 1);
            oversampleBox_.addItem("2x", 2);
            oversampleBox_.addItem("4x", 4);
            oversampleBox_.setSelectedId(4);
            oversampleBox_.addListener(this);
            addAndMakeVisible(oversampleBox_);
        }

        void drawMeters(juce::Graphics& g)
        {
            auto r = meterArea_;
            int  w = 40;
            int  h = r.getHeight() - 20;
            int  x = r.getX();

            // GR meter
            g.setColour(juce::Colours::darkgrey);
            g.fillRoundedRectangle((float)x, (float)r.getY(), (float)w, (float)h, 4.0f);

            float grNorm = juce::jlimit(0.0f, 1.0f, grDisplayDb_ / 12.0f); // hasta 12dB
            juce::Colour c = juce::Colour::fromHSV(0.33f - grNorm * 0.33f, 1.0f, 0.9f, 1.0f);
            g.setColour(c);

            float filled = h * grNorm;
            g.fillRoundedRectangle((float)x + 3.0f,
                                   (float)(r.getY() + h - filled + 3.0f),
                                   (float)w - 6.0f,
                                   filled - 6.0f,
                                   3.0f);

            g.setColour(juce::Colours::white);
            g.setFont(11.0f);
            g.drawText("GR", x, r.getBottom() - 16, w, 16, juce::Justification::centred);
            g.drawText(juce::String(-grDisplayDb_, 1) + " dB",
                       x + w + 4, r.getY(), 80, 16, juce::Justification::left);
        }
    };
}
```

Este módulo está pensado para ir el último en la cadena (después de multiband, lookahead comp, etc.) y darte un techo tipo $-0.1$ dBFS con lookahead y clipping suave.[^3][^1]

¿Quieres que te escriba ahora un pequeño “master chain” de ejemplo conectando:

- MultibandCompressorModule
- LookaheadCompressorModule
- MasteringLimiterModule

en un `MasterBusModule` o similar?

<div align="center">⁂</div>

[^1]: https://www.izotope.com/en/learn/brickwall-limiters

[^2]: https://signalsmith-audio.co.uk/writing/2022/limiter/

[^3]: https://productionadvice.co.uk/brickwall-limiters/

