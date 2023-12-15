#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
: backgroundApp(juce::Colour(Constants::Colors::bg_App)), inGainLSlider(this), makeupGainLSlider(this), treshLSlider(this), ratioLSlider(this), kneeLSlider(this), attackLSlider(this),
  releaseLSlider(this), mixLSlider(this), 
  powerButton("powerButton", juce::DrawableButton::ButtonStyle::ImageOnButtonBackground)
{
    setLookAndFeel(&scLaF);
    initWidgets();
    setSize(800, 600);

    startTimerHz(60);

    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    setLookAndFeel(nullptr);
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto numInputChannels = fileBuffer->getNumChannels();
    auto numOutputChannels = bufferToFill.buffer->getNumChannels();

    auto outputSamplesRemaining = bufferToFill.numSamples;
    auto outputSamplesOffset = bufferToFill.startSample;

    while (audioPause && outputSamplesRemaining > 0)
    {
        auto bufferSamplesRemaining = fileBuffer->getNumSamples() - position;
        auto samplesThisTime = juce::jmin(outputSamplesRemaining, bufferSamplesRemaining);

        for (auto channel = 0; channel < numOutputChannels; ++channel)
        {
            bufferToFill.buffer->copyFrom(channel, outputSamplesOffset, *fileBuffer, channel % numInputChannels, position, samplesThisTime);
        }

        outputSamplesRemaining -= samplesThisTime;
        outputSamplesOffset += samplesThisTime;
        position += samplesThisTime;

        if (position == fileBuffer->getNumSamples())
            position = 0;
    }
}

void MainComponent::releaseResources()
{
    inputAudioBuffer.setSize(0, 0);
    outputAudioBuffer.setSize(0, 0);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(Constants::Margins::big);

    const auto headerHeight = area.getHeight() / 10;
    const auto btnAreaWidth = area.getWidth() / 5;
    const auto btnBotHeight = area.getHeight() / 3;

    auto header = area.removeFromTop(headerHeight).reduced(Constants::Margins::small);
    auto lBtnArea = area.removeFromLeft(btnAreaWidth).reduced(Constants::Margins::small);
    auto rBtnArea = area.removeFromRight(btnAreaWidth).reduced(Constants::Margins::small);
    auto botBtnArea = area.removeFromBottom(btnBotHeight).reduced(Constants::Margins::medium);

    const juce::FlexItem::Margin knobMargin = juce::FlexItem::Margin(Constants::Margins::medium);
    const juce::FlexItem::Margin knobMarginSmall = juce::FlexItem::Margin(Constants::Margins::medium);
    const juce::FlexItem::Margin buttonMargin = juce::FlexItem::Margin(Constants::Margins::small, Constants::Margins::big,
        Constants::Margins::small,
        Constants::Margins::big);

    juce::FlexBox headerBox;
    headerBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    headerBox.flexDirection = juce::FlexBox::Direction::row;
    headerBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    //headerBox.items.add(juce::FlexItem(lahButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(juce::FlexItem(openButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(juce::FlexItem(processButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(juce::FlexItem(saveButton).withFlex(2).withMargin(buttonMargin));
    //headerBox.items.add(juce::FlexItem(autoAttackButton).withFlex(2).withMargin(buttonMargin));
    //headerBox.items.add(juce::FlexItem(autoReleaseButton).withFlex(2).withMargin(buttonMargin));
    //headerBox.items.add(juce::FlexItem(autoMakeupButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(juce::FlexItem(powerButton).withFlex(1).withMargin(buttonMargin));
    headerBox.performLayout(header.toFloat());

    juce::FlexBox leftBtnBox;
    leftBtnBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    leftBtnBox.flexDirection = juce::FlexBox::Direction::column;
    leftBtnBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    leftBtnBox.items.add(juce::FlexItem(attackLSlider).withFlex(1).withMargin(knobMarginSmall));
    leftBtnBox.items.add(juce::FlexItem(releaseLSlider).withFlex(1).withMargin(knobMarginSmall));
    leftBtnBox.items.add(juce::FlexItem(inGainLSlider).withFlex(1).withMargin(knobMarginSmall));
    leftBtnBox.performLayout(lBtnArea.toFloat());

    juce::FlexBox rightBtnBox;
    rightBtnBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    rightBtnBox.flexDirection = juce::FlexBox::Direction::column;
    rightBtnBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    rightBtnBox.items.add(juce::FlexItem(kneeLSlider).withFlex(1).withMargin(knobMarginSmall));
    rightBtnBox.items.add(juce::FlexItem(ratioLSlider).withFlex(1).withMargin(knobMarginSmall));
    rightBtnBox.items.add(juce::FlexItem(mixLSlider).withFlex(1).withMargin(knobMarginSmall));
    rightBtnBox.performLayout(rBtnArea.toFloat());

    juce::FlexBox botBtnBox;
    botBtnBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    botBtnBox.flexDirection = juce::FlexBox::Direction::row;
    botBtnBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    botBtnBox.items.add(juce::FlexItem(treshLSlider).withFlex(1).withMargin(knobMargin));
    botBtnBox.items.add(juce::FlexItem(makeupGainLSlider).withFlex(1).withMargin(knobMargin));
    botBtnBox.performLayout(botBtnArea.toFloat());

    juce::FlexBox meterBox;
    meterBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    meterBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    meterBox.items.add(juce::FlexItem(meter).withFlex(1).withMargin(Constants::Margins::big));
    meterBox.performLayout(area.toFloat());
}

void MainComponent::buttonClicked(juce::Button* b)
{
    // if (b == &autoAttackButton)attackLSlider.setEnabled(!attackLSlider.isEnabled());
    // if (b == &autoReleaseButton)releaseLSlider.setEnabled(!releaseLSlider.isEnabled());
    if (b == &powerButton) setGUIState(powerButton.getToggleState());
    if (b == &openButton) openButtonClicked();
    if (b == &saveButton) saveButtonClicked();
    if (b == &processButton) processButtonClicked();
}

void MainComponent::timerCallback()
{
    int m = meter.getMode();
    switch (m)
    {
    case Meter::Mode::IN:
        meter.update(currentInput.get());
        fileBuffer = &inputAudioBuffer;
        break;
    case Meter::Mode::OUT:
        meter.update(currentOutput.get());
        fileBuffer = &outputAudioBuffer;
        break;
    default:
        break;
    }
}

void MainComponent::processBlock(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = 2;
    const auto totalNumOutputChannels = 2;
    const auto numSamples = buffer.getNumSamples();

    // Clear buffer
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //Update input peak metering
    inLevelFollower.updatePeak(buffer.getArrayOfReadPointers(), totalNumInputChannels, numSamples);
    currentInput.set(juce::Decibels::gainToDecibels(inLevelFollower.getPeak()));

    // Do compressor processing
    compressor.process(buffer);

    // Update output peak metering
    outLevelFollower.updatePeak(buffer.getArrayOfReadPointers(), totalNumInputChannels, numSamples);
    currentOutput = juce::Decibels::gainToDecibels(outLevelFollower.getPeak());
}

void MainComponent::initWidgets()
{
    addAndMakeVisible(inGainLSlider);
    inGainLSlider.setLabelText("Input Gain");
    inGainLSlider.setRange(0.0, 100.0);
    inGainLSlider.setValue(0.5);

    addAndMakeVisible(makeupGainLSlider);
    makeupGainLSlider.setLabelText("Makeup");
    makeupGainLSlider.setRange(-12.0, 24.0);
    makeupGainLSlider.setValue(0.0);

    addAndMakeVisible(treshLSlider);
    treshLSlider.setLabelText("Threshold");
    treshLSlider.setRange(-100.0, 0.0);
    treshLSlider.setValue(-20.0);

    addAndMakeVisible(ratioLSlider);
    ratioLSlider.setLabelText("Ratio");
    ratioLSlider.setRange(1.0, 100.0);
    ratioLSlider.setValue(2.0);
    ratioLSlider.setTextValueSuffix("");

    addAndMakeVisible(kneeLSlider);
    kneeLSlider.setLabelText("Knee");
    kneeLSlider.setRange(1.0, 100.0);
    kneeLSlider.setValue(6.0);

    addAndMakeVisible(attackLSlider);
    attackLSlider.setLabelText("Attack");
    attackLSlider.setRange(0.001, 500.0);
    attackLSlider.setValue(0.01);
    attackLSlider.setTextValueSuffix("ms");

    addAndMakeVisible(releaseLSlider);
    releaseLSlider.setLabelText("Release");
    releaseLSlider.setRange(0.001, 500.0);
    releaseLSlider.setValue(0.14);
    releaseLSlider.setTextValueSuffix("ms");

    addAndMakeVisible(mixLSlider);
    mixLSlider.setLabelText("Mix");
    mixLSlider.setRange(1.0, 100.0);
    mixLSlider.setValue(1.0);
    mixLSlider.setTextValueSuffix(" ");

    addAndMakeVisible(lahButton);
    lahButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    lahButton.setButtonText("LookAhead");
    lahButton.setClickingTogglesState(true);
    lahButton.setToggleState(false, juce::dontSendNotification);

    addAndMakeVisible(autoAttackButton);
    autoAttackButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    autoAttackButton.setButtonText("AutoAttack");
    autoAttackButton.setClickingTogglesState(true);
    autoAttackButton.setToggleState(false, juce::dontSendNotification);
    autoAttackButton.addListener(this);

    addAndMakeVisible(autoReleaseButton);
    autoReleaseButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    autoReleaseButton.setButtonText("AutoRelease");
    autoReleaseButton.setClickingTogglesState(true);
    autoReleaseButton.setToggleState(false, juce::dontSendNotification);
    autoReleaseButton.addListener(this);

    addAndMakeVisible(autoMakeupButton);
    autoMakeupButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    autoMakeupButton.setButtonText("Makeup");
    autoMakeupButton.setClickingTogglesState(true);
    autoMakeupButton.setToggleState(false, juce::dontSendNotification);
    autoMakeupButton.addListener(this);
    
    addAndMakeVisible(powerButton);
    powerButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    powerButton.setImages(
        juce::Drawable::createFromImageData(BinaryData::power_white_svg, BinaryData::power_white_svgSize).get());
    powerButton.setClickingTogglesState(true);
    powerButton.setToggleState(true, juce::dontSendNotification);
    powerButton.addListener(this);

    addAndMakeVisible(openButton);
    openButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    openButton.setButtonText("File Open");
    openButton.setToggleState(false, juce::dontSendNotification);
    openButton.addListener(this);

    addAndMakeVisible(saveButton);
    saveButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    saveButton.setButtonText("File Save");
    saveButton.setToggleState(false, juce::dontSendNotification);
    saveButton.addListener(this);
    saveButton.setEnabled(false);

    addAndMakeVisible(processButton);
    processButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    processButton.setButtonText("Process");
    processButton.setToggleState(false, juce::dontSendNotification);
    processButton.addListener(this);
    processButton.setEnabled(false);

    addAndMakeVisible(meter);
    meter.setMode(Meter::Mode::GR);
}

void MainComponent::setGUIState(bool powerState)
{
    audioPause = powerState;

    inGainLSlider.setEnabled(powerState);
    treshLSlider.setEnabled(powerState);
    ratioLSlider.setEnabled(powerState);
    kneeLSlider.setEnabled(powerState);
    makeupGainLSlider.setEnabled(powerState);
    mixLSlider.setEnabled(powerState);
    meter.setEnabled(powerState);
    meter.setGUIEnabled(powerState);
    lahButton.setEnabled(powerState);
    autoMakeupButton.setEnabled(powerState);

    autoAttackButton.setEnabled(powerState);
    autoReleaseButton.setEnabled(powerState);

    if (!powerState)
    {
        attackLSlider.setEnabled(powerState);
        releaseLSlider.setEnabled(powerState);
    }
    else
    {
        attackLSlider.setEnabled(!autoAttackButton.getToggleState());
        releaseLSlider.setEnabled(!autoReleaseButton.getToggleState());
    }
}

void MainComponent::openButtonClicked()
{
    shutdownAudio();                                                                        

    chooser = std::make_unique<juce::FileChooser>("Select a Wave file...", juce::File{}, "*.wav");
    
    auto chooserFlags = juce::FileBrowserComponent::openMode
                      | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            outputFilePath = file.getFullPathName().upToLastOccurrenceOf("\\", true, true);
            outputFileName = file.getFileNameWithoutExtension();

            if (file == juce::File{})
                return;

            std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

            if (reader.get() != nullptr)
            {
                sampleRate = reader->sampleRate;
                bitsPerSample = reader->bitsPerSample;
                inputAudioBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
                reader->read(&inputAudioBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
                setAudioChannels(0, (int)reader->numChannels);
                position = 0;
                
                inLevelFollower.prepare(sampleRate);
                inLevelFollower.setPeakDecay(0.3f);

                inLevelFollower.updatePeak(inputAudioBuffer.getArrayOfReadPointers(), reader->numChannels, reader->lengthInSamples);
                currentInput.set(juce::Decibels::gainToDecibels(inLevelFollower.getPeak()));

                fileBuffer = &inputAudioBuffer;
                
                outputAudioBuffer = inputAudioBuffer;
            }
        }
    );

    processButton.setEnabled(true);
    saveButton.setEnabled(true);
    openButton.setEnabled(false);    // BUG: Open twice program existed after failed at reading audio file.
}

void MainComponent::saveButtonClicked()
{
    juce::String outputFileFullPath = outputFilePath + outputFileName + "_output.wav";
    saveFile = juce::File(outputFileFullPath);
    saveFile.deleteFile();
    saveFile.create();

    writer.reset(format.createWriterFor(new juce::FileOutputStream(saveFile), sampleRate,
        outputAudioBuffer.getNumChannels(), bitsPerSample, {}, 0));

    if (writer != nullptr)
    {
        writer->writeFromAudioSampleBuffer(outputAudioBuffer, 0, outputAudioBuffer.getNumSamples());
        writer->flush();

        saveFile.~File();

        juce::MessageBoxOptions options = juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::InfoIcon)
            .withTitle("File Save")
            .withMessage(juce::String("Save Success\nFull Path\n" + outputFileFullPath))
            .withButton("OK");

        juce::AlertWindow::showAsync(options, [this](int param) {});
    }
}

void MainComponent::processButtonClicked()
{
    outputAudioBuffer = inputAudioBuffer;
    compressor.prepare({ sampleRate, static_cast<unsigned int>(outputAudioBuffer.getNumSamples()), 2});
    inLevelFollower.prepare(sampleRate);
    outLevelFollower.prepare(sampleRate);
    inLevelFollower.setPeakDecay(0.3f);
    outLevelFollower.setPeakDecay(0.3f);

    compressor.setInput(inGainLSlider.getValue());
    compressor.setThreshold(treshLSlider.getValue());
    compressor.setRatio(ratioLSlider.getValue());
    compressor.setKnee(kneeLSlider.getValue());
    compressor.setAttack(attackLSlider.getValue());
    compressor.setRelease(releaseLSlider.getValue());
    compressor.setMakeup(makeupGainLSlider.getValue());
    compressor.setMix(mixLSlider.getValue());

    processBlock(outputAudioBuffer);
}