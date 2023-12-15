#pragma once

#include <JuceHeader.h>
#include "util/include/Constants.h"
#include "gui/include/Meter.h"
#include "gui/include/LabeledSlider.h"
#include "gui/include/Meter.h"
#include "gui/include/SCLookAndFeel.h"
#include "dsp/include/Compressor.h"
#include "dsp/include/LevelEnvelopeFollower.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::Button::Listener, juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button*) override;
    void timerCallback() override;

private:
    void initWidgets();
    void setGUIState(bool);

    void processBlock(juce::AudioBuffer<float>& buffer);

    void openButtonClicked();
    void saveButtonClicked();
    void processButtonClicked();

    // juce::AudioProcessorValueTreeState& valueTreeState;

    //LookAndFeel
    SCLookAndFeel scLaF;
    juce::Colour backgroundApp;

    //Widgets
    MeterBackground meterbg;
    Meter meter;

    LabeledSlider inGainLSlider;
    LabeledSlider makeupGainLSlider;
    LabeledSlider treshLSlider;
    LabeledSlider ratioLSlider;
    LabeledSlider kneeLSlider;
    LabeledSlider attackLSlider;
    LabeledSlider releaseLSlider;
    LabeledSlider mixLSlider;

    juce::TextButton lahButton;
    juce::TextButton autoAttackButton;
    juce::TextButton autoReleaseButton;
    juce::TextButton autoMakeupButton;
    juce::DrawableButton powerButton;
    juce::TextButton openButton;
    juce::TextButton saveButton;
    juce::TextButton processButton;
    juce::ComboBox selectComboBox;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lahAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoMakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
    
    // File
    std::unique_ptr<juce::FileChooser> chooser;
    juce::String outputFilePath;
    juce::String outputFileName;

    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> inputAudioBuffer;
    juce::AudioBuffer<float> outputAudioBuffer;
    juce::AudioBuffer<float> *fileBuffer;

    juce::File saveFile;
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatWriter> writer;

    int position;
    double sampleRate;
    int bitsPerSample;

    bool audioPause;

    //Processors
    juce::Atomic<float> currentInput;
    juce::Atomic<float> currentOutput;

    Compressor compressor;
    LevelEnvelopeFollower inLevelFollower;
    LevelEnvelopeFollower outLevelFollower;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
