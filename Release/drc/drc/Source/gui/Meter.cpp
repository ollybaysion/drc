/*
Copyright (C) 2020 Phillip Lamp
This file is part of the CTAGDRC project <https://github.com/p-hlp/CTAGDRC>.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "include/Meter.h"
#include "../Source/MainComponent.h"
#include "../util/include/Constants.h"
#include <string>


Meter::Meter()
{
    //Init vars
    startAngle = static_cast<float>(5.0f / 3.0f * juce::MathConstants<float>::pi);
    endAngle = static_cast<float>(7.0f / 3.0f * juce::MathConstants<float>::pi);
    meterBg.prepare(startAngle, endAngle);
    meterBg.setMode(GR);
    needle.prepare(startAngle, endAngle);
    needle.setMode(GR);
    addAndMakeVisible(modeBox);
    modeBox.addItem("Input", 1);
    modeBox.addItem("Output", 2);
    // modeBox.addItem("Gain Reduction", 3);
    modeBox.setJustificationType(juce::Justification::centred);
    modeBox.setSelectedId(1);
    modeBox.onChange = [this] { modeBoxChanged(); };
    addAndMakeVisible(modeBox);
    meterLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(meterLabel);
    addAndMakeVisible(meterBg);
    addAndMakeVisible(needle);

    backgroundDarkGrey = juce::Colour(Constants::Colors::bg_DarkGrey);
}

void Meter::paint(juce::Graphics& g)
{
    g.setColour(backgroundDarkGrey);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 3);
}

void Meter::resized()
{
    auto bounds = getLocalBounds();
    auto boxHeight = (bounds.getHeight() / 10) * 2;
    auto boxArea = bounds.removeFromTop(boxHeight);
    const juce::FlexItem::Margin comboMargin = juce::FlexItem::Margin(6.0f, 16.0f, 6.0f, 16.0f);

    juce::FlexBox mBox;
    mBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    mBox.flexDirection = juce::FlexBox::Direction::column;
    mBox.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    mBox.items.add(juce::FlexItem(modeBox).withFlex(1).withMargin(comboMargin));
    mBox.items.add(juce::FlexItem(meterLabel).withFlex(1).withMargin(comboMargin));
    mBox.performLayout(boxArea.toFloat());

    meterBg.setBounds(bounds);
    needle.setBounds(bounds);
}

void Meter::setMode(int m)
{
    meterBg.setMode(m);
    needle.setMode(m);
    meterMode = m;
}

void Meter::modeBoxChanged()
{
    int m = modeBox.getSelectedId();
    setMode(m);
}

void Meter::update(const float& val)
{
    char s[20];
    std::sprintf(s, "%.3f dBFS", val);
    if (val != valueInDecibel)
    {
        needle.update(val);
        meterLabel.setText(juce::String(s), juce::dontSendNotification);
    }
}

int Meter::getMode()
{
    return meterMode;
}

float Meter::getValue()
{
    return valueInDecibel;
}

void Meter::setGUIEnabled(bool state)
{
    needle.setNeedleEnabled(state);
    meterBg.setIndicatorEnabled(state);
}
