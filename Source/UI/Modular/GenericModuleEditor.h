#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Modular/Core/Foundation/v1/Module.h"
#include <vector>
#include <memory>

class GenericModuleEditor : public juce::Component, public juce::Timer
{
public:
    GenericModuleEditor(Modular::Core::Module* module)
        : module_(module)
    {
        if (module_)
        {
            const auto& metadata = module_->getParameterMetadata();
            for (const auto& meta : metadata)
            {
                auto* slider = new juce::Slider();
                slider->setRange(meta.minValue, meta.maxValue);
                slider->setValue(module_->getParameter(meta.id), juce::dontSendNotification);
                slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
                
                // Lambda to update module
                slider->onValueChange = [this, slider, id = meta.id]() {
                    if (module_) module_->setParameter(id, (float)slider->getValue());
                };
                
                addAndMakeVisible(slider);
                sliders_.add(slider);
                
                auto* label = new juce::Label();
                label->setText(meta.name, juce::dontSendNotification);
                label->setFont(14.0f);
                addAndMakeVisible(label);
                labels_.add(label);
                
                paramIds_.push_back(meta.id);
            }
        }
        
        setSize(300, 20 + sliders_.size() * 30);
        startTimerHz(30); // Refresh UI at 30Hz
    }
    
    ~GenericModuleEditor() override
    {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        g.setFont(16.0f);
        g.drawText(module_ ? module_->getName() : "No Module", 0, 0, getWidth(), 20, juce::Justification::centred);
        
        g.setColour(juce::Colours::grey);
        g.drawRect(getLocalBounds(), 1);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(5);
        area.removeFromTop(20); // Title
        
        for (int i = 0; i < sliders_.size(); ++i)
        {
            auto row = area.removeFromTop(25);
            labels_[i]->setBounds(row.removeFromLeft(100));
            sliders_[i]->setBounds(row);
            area.removeFromTop(5); // Gap
        }
    }
    
    void timerCallback() override
    {
        // One-way binding: If parameter changes externally (e.g. automation), update slider
        // This is tricky with onValueChange loop.
        // For now, simpler: Only update slider if user is NOT dragging it.
        // But Slider doesn't expose dragging state easily without listener.
        // Let's assume for this version that UI is the main controller.
        // Or check if value differs significantly.
        
        if (!module_) return;
        
        for (int i = 0; i < sliders_.size(); ++i)
        {
            auto* slider = sliders_[i];
            juce::String id = paramIds_[i];
            float val = module_->getParameter(id.toStdString());
            
            if (std::abs(slider->getValue() - val) > 0.001f && !slider->isMouseButtonDown())
            {
                slider->setValue(val, juce::dontSendNotification);
            }
        }
    }

private:
    Modular::Core::Module* module_;
    juce::OwnedArray<juce::Slider> sliders_;
    juce::OwnedArray<juce::Label> labels_;
    std::vector<std::string> paramIds_; // Store IDs to map index to ID
};
