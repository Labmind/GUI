/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2012 Open Ephys

    ------------------------------------------------------------------

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

#include "ChannelSelector.h"
#include <math.h>

#include "../RecordNode.h"
#include "../AudioNode.h"
#include "../ProcessorGraph.h"

ChannelSelector::ChannelSelector(bool createButtons, Font& titleFont_) :
	isNotSink(createButtons), titleFont(titleFont_), offsetLR(0), offsetUD(0),
	moveRight(false), moveLeft(false), desiredOffset(0), paramsActive(true), paramsToggled(true),
	radioStatus(false), eventsOnly(false)
{

	// initialize buttons
	audioButton = new EditorButton("AUDIO", titleFont);
    audioButton->addListener(this);
    addAndMakeVisible(audioButton);
    if (!createButtons)
    	audioButton->setState(false);

 	recordButton = new EditorButton("REC", titleFont);
	recordButton->addListener(this);
	addAndMakeVisible(recordButton);
	 if (!createButtons)
    	recordButton->setState(false);

	paramsButton = new EditorButton("PARAM", titleFont);
	paramsButton->addListener(this);
	addAndMakeVisible(paramsButton);
	
	paramsButton->setToggleState(true, false);

	audioButtons.clear();
	recordButtons.clear();

	// set button layout parameters
	parameterOffset = 0;
	recordOffset = getDesiredWidth();
	audioOffset = getDesiredWidth()*2;

	parameterButtons.clear();

	allButton = new EditorButton("all", titleFont);
	allButton->addListener(this);
	addAndMakeVisible(allButton);

	noneButton = new EditorButton("none", titleFont);
	noneButton->addListener(this);
	addAndMakeVisible(noneButton);

}

ChannelSelector::~ChannelSelector()
{
	deleteAllChildren();

}

void ChannelSelector::paint(Graphics& g)
{
	ColourGradient grad1 = ColourGradient(Colours::black.withAlpha(0.8f),0.0,0.0,
										  Colours::black.withAlpha(0.1f),0.0,25.0f,
										  false);
	g.setGradientFill(grad1);
	g.fillRect(0, 15, getWidth(), getHeight()-30);

	ColourGradient grad2 = ColourGradient(Colours::black.withAlpha(0.2f),0.0,0.0,
										  Colours::black.withAlpha(0.0f),5.0f,0.0f,
										  false);
	g.setGradientFill(grad2);
	g.fillRect(0, 15, getWidth(), getHeight()-30);

	ColourGradient grad3 = ColourGradient(Colours::black.withAlpha(0.2f),(float) getDesiredWidth(),0.0,
										  Colours::black.withAlpha(0.0f),(float) getDesiredWidth()-5.0f,0.0f,
										  false);
	g.setGradientFill(grad3);
	g.fillRect(0, 15, getWidth(), getHeight()-30);
}


void ChannelSelector::setNumChannels(int numChans)
{

	int difference = numChans - parameterButtons.size();

	std::cout << difference << " buttons needed." << std::endl;

	if (difference > 0)
	{
		for (int n = 0; n < difference; n++)
		{
			addButton();
		}
	} else if (difference < 0)
	{
		for (int n = 0; n < -difference; n++)
		{
			removeButton();
		}
	}

	refreshButtonBoundaries();

}

void ChannelSelector::refreshButtonBoundaries()
{

	int nColumns = 8;
	int columnWidth = getDesiredWidth()/(nColumns + 1);
	int rowHeight = 14;
	int topOffset = 20;

	for (int i = 0; i < parameterButtons.size(); i++)
	{
		parameterButtons[i]->setBounds(columnWidth/2 + offsetLR +
									   columnWidth*((i)%nColumns),
									   floor((i)/nColumns)*rowHeight+offsetUD + topOffset,
									   columnWidth, rowHeight);

		if (isNotSink)
		{
			recordButtons[i]->setBounds(columnWidth/2 + offsetLR +
									   columnWidth*((i)%nColumns) - getDesiredWidth(),
									   floor((i)/nColumns)*rowHeight+offsetUD + topOffset,
									   columnWidth, rowHeight);
			audioButtons[i]->setBounds(columnWidth/2 + offsetLR +
									   columnWidth*((i)%nColumns) -
									   	getDesiredWidth()*2,
									   floor((i)/nColumns)*rowHeight+offsetUD + topOffset,
									   columnWidth, rowHeight);
		}

	}

	int w = getWidth()/3;
	int h = 15;

	audioButton->setBounds(0, 0, w, h);
	recordButton->setBounds(w, 0, w, h);
	paramsButton->setBounds(w*2, 0, w, h);

	allButton->setBounds(0, getHeight()-15, getWidth()/2, 15);
	noneButton->setBounds(getWidth()/2, getHeight()-15, getWidth()/2, 15);

}

void ChannelSelector::resized()
{
	refreshButtonBoundaries();
}

void ChannelSelector::timerCallback()
{

	//std::cout << desiredOffset - offsetLR << std::endl;

	if (offsetLR != desiredOffset)
	{
		if (desiredOffset - offsetLR > 0)
		{
			offsetLR += 25; // be careful what you set this value to!
							// if it's not a multiple of the desired
							// width, things could go badly!
		} else {
			offsetLR -= 25;
		}

	} else {
		stopTimer();
	}

	refreshButtonBoundaries();

}

void ChannelSelector::addButton()
{

	int size = parameterButtons.size();

	ChannelSelectorButton* b = new ChannelSelectorButton(size+1, PARAMETER, titleFont);
	parameterButtons.add(b);
	addAndMakeVisible(b);

	if (paramsToggled)
		b->setToggleState(true, false);
	else 
		b->setToggleState(false, false);

	if (!paramsActive)
		b->setActive(false);

	b->addListener(this);

	if (isNotSink)
	{
		ChannelSelectorButton* br = new ChannelSelectorButton(size+1, RECORD, titleFont);
		recordButtons.add(br);
		addAndMakeVisible(br);
		br->addListener(this);

		ChannelSelectorButton* ba = new ChannelSelectorButton(size+1, AUDIO, titleFont);
		audioButtons.add(ba);
		addAndMakeVisible(ba);
		ba->addListener(this);
	}
}

void ChannelSelector::removeButton()
{
	int size = parameterButtons.size();

	ChannelSelectorButton* b = parameterButtons.remove(size-1);
	removeChildComponent(b);
	deleteAndZero(b);

	if (isNotSink)
	{
		ChannelSelectorButton* br = recordButtons.remove(size-1);
		removeChildComponent(br);
		deleteAndZero(br);

		ChannelSelectorButton* ba = audioButtons.remove(size-1);
		removeChildComponent(ba);
		deleteAndZero(ba);
	}
}

Array<int> ChannelSelector::getActiveChannels()
{
	Array<int> a;

    if (!eventsOnly) {
	for (int i = 0; i < parameterButtons.size(); i++)
	{
		if (parameterButtons[i]->getToggleState())
			a.add(i);
	}
    } else {
        a.add(0);
    }

	return a;
}

void ChannelSelector::setActiveChannels(Array<int> a)
{

	std::cout << "Setting active channels!" << std::endl;

	for (int i = 0; i < parameterButtons.size(); i++)
	{
		parameterButtons[i]->setToggleState(false,false);
	}

	for (int i = 0; i < a.size(); i++)
	{
		parameterButtons[a[i]]->setToggleState(true,false);
	}
}

void ChannelSelector::inactivateButtons()
{

	paramsActive = false;

	for (int i = 0; i < parameterButtons.size(); i++)
	{
		parameterButtons[i]->setActive(false);
		parameterButtons[i]->repaint();
	}
}

void ChannelSelector::activateButtons()
{

	paramsActive = true;

	for (int i = 0; i < parameterButtons.size(); i++)
	{
		parameterButtons[i]->setActive(true);
		parameterButtons[i]->repaint();
	}

}

void ChannelSelector::setRadioStatus(bool radioOn)
{

	radioStatus = radioOn;

	for (int i = 0; i < parameterButtons.size(); i++)
	{
		if (radioOn) {
			parameterButtons[i]->setToggleState(false, false);
			parameterButtons[i]->setRadioGroupId(999);
		} else {
			parameterButtons[i]->setToggleState(false, false);
			parameterButtons[i]->setRadioGroupId(0);
		}
	}

}

bool ChannelSelector::getRecordStatus(int chan)
{

	if (chan >= 0 && chan < recordButtons.size())
		return recordButtons[chan]->getToggleState();
	else 
		return false;

}

bool ChannelSelector::getAudioStatus(int chan)
{

	if (chan >= 0 && chan < recordButtons.size())
		return audioButtons[chan]->getToggleState();
	else 
		return false;

}


int ChannelSelector::getDesiredWidth()
{
	return 150;
}

void ChannelSelector::buttonClicked(Button* button)
{
	//checkChannelSelectors();
	if (button == paramsButton)
	{
		// make sure param buttons are visible
		allButton->setState(true);
		desiredOffset = parameterOffset;
		startTimer(20);
		return;
	} else if (button == audioButton)
	{
		// make sure audio buttons are visible

		if (audioButton->getState())
		{
		allButton->setState(false);
			
		desiredOffset = audioOffset;
		startTimer(20);
		} else {
			paramsButton->setToggleState(true, false);
		}
		return;
	} else if (button == recordButton)
	{
		// make sure record buttons are visible;
		if (recordButton->getState())
			{
		allButton->setState(true);
		desiredOffset = recordOffset;
		startTimer(20);
		} else {
				paramsButton->setToggleState(true, false);
			}
		return;
	} 
	else if (button == allButton)	
	{
		// select all active buttons
		if (offsetLR == recordOffset)
		{
			
			
			for (int i = 0; i < recordButtons.size(); i++)
			{
				recordButtons[i]->setToggleState(true, true);
			}
			
		} else if (offsetLR == parameterOffset)
		{
			

			for (int i = 0; i < parameterButtons.size(); i++)
			{
				parameterButtons[i]->setToggleState(true, true);
			}
		} else if (offsetLR == audioOffset)
		{
			// do nothing--> button is disabled
		}
	} else if (button == noneButton)
	{
		// deselect all active buttons
		if (offsetLR == recordOffset)
		{
			for (int i = 0; i < recordButtons.size(); i++)
			{
				recordButtons[i]->setToggleState(false, true);
			}
		} else if (offsetLR == parameterOffset)
		{
			for (int i = 0; i < parameterButtons.size(); i++)
			{
				parameterButtons[i]->setToggleState(false, true);
			}
		} else if (offsetLR == audioOffset)
		{
			for (int i = 0; i < audioButtons.size(); i++)
			{
				audioButtons[i]->setToggleState(false, true);
			}
		}
	} else {

		ChannelSelectorButton* b = (ChannelSelectorButton*) button;

		if (b->getType() == AUDIO)
		{
			// get audio node, and inform it of the change
			GenericEditor* editor = (GenericEditor*) getParentComponent();

			int channelNum = editor->getStartChannel() + b->getChannel() - 1;
			bool status = b->getToggleState();

			std::cout << "Setting audio monitor for channel " << channelNum;

			if (status)
			{
				std::cout << " to true." << std::endl;
			} else {
				std::cout << " to false." << std::endl;
			}
			
			editor->getProcessorGraph()->
					getAudioNode()->
					setChannelStatus(channelNum, status);

		} else if (b->getType() == RECORD)
		{
			// get record node, and inform it of the change
			GenericEditor* editor = (GenericEditor*) getParentComponent();

			int channelNum = editor->getStartChannel() + b->getChannel() - 1;
			bool status = b->getToggleState();

			std::cout << "Setting record status for channel " << channelNum;

			if (status)
			{
				std::cout << " to true." << std::endl;
			} else {
				std::cout << " to false." << std::endl;
			}
			
			editor->getProcessorGraph()->
					getRecordNode()->
					setChannelStatus(channelNum, status);
					
		} else {
			// do nothing
			if (radioStatus) // if radio buttons are active
			{
				// send a message to parent
				GenericEditor* editor = (GenericEditor*) getParentComponent();
				editor->channelChanged(b->getChannel());
			}
		}

	}

}


///////////// BUTTONS //////////////////////


EditorButton::EditorButton(const String& name, Font& f) : Button(name) 
{

	isEnabled = true;

	buttonFont = f;
	buttonFont.setHeight(10);

	if (!getName().equalsIgnoreCase("all") && !getName().equalsIgnoreCase("none"))
	{
    	setRadioGroupId(999);
    	setClickingTogglesState(true);
    }

    selectedGrad = ColourGradient(Colour(240,179,12),0.0,0.0,
										 Colour(207,160,33),0.0, 20.0f,
										 false);
    selectedOverGrad = ColourGradient(Colour(209,162,33),0.0, 5.0f,
										 Colour(190,150,25),0.0, 0.0f,
										 false);
    neutralGrad = ColourGradient(Colour(220,220,220),0.0,0.0,
										 Colour(170,170,170),0.0, 20.0f,
										 false);
    neutralOverGrad = ColourGradient(Colour(180,180,180),0.0,5.0f,
										 Colour(150,150,150),0.0, 0.0,
										 false);


}

void EditorButton::resized()
{
    float radius = 5.0f;
    float width = (float) getWidth();
    float height = (float) getHeight();

    if (getName().equalsIgnoreCase("AUDIO"))
	{
		//outlinePath.startNewSubPath(0, height);
		outlinePath.lineTo(0, 0);//radius);
		//outlinePath.addArc(0, 0, radius*2, radius*2, 1.5*double_Pi, 2.0*double_Pi );

		outlinePath.lineTo(width, 0);//getHeight());

		outlinePath.lineTo(width, height);

		outlinePath.lineTo(0, height);
		//outlinePath.addArc(0, getHeight()-radius*2, radius*2, radius*2, double_Pi, 1.5*double_Pi);
		//outlinePath.lineTo(0, radius);
		outlinePath.closeSubPath();

	} else if (getName().equalsIgnoreCase("PARAM"))
	{
		//outlinePath.startNewSubPath(0, 0);

		outlinePath.lineTo(width, 0);
	
		//outlinePath.addArc(width-radius*2, 0, radius*2, radius*2, 0, 0.5*double_Pi);

		outlinePath.lineTo(getWidth(), height);

		//outlinePath.addArc(getWidth()-radius*2, getHeight()-radius*2, radius*2, radius*2, 0.5*double_Pi, double_Pi);

		outlinePath.lineTo(0, height);
		outlinePath.lineTo(0, 0);
		//outlinePath.closeSubPath();


	} else if (getName().equalsIgnoreCase("REC"))
	{

		outlinePath.addRectangle(0,0,getWidth(),getHeight());

	} else if (getName().equalsIgnoreCase("all"))
	{

		//outlinePath.startNewSubPath(0, 0);

		outlinePath.lineTo(width, 0);
	
		//outlinePath.addArc(width-radius*2, 0, radius*2, radius*2, 0, 0.5*double_Pi);

		outlinePath.lineTo(width, height);

		//outlinePath.addArc(getWidth()-radius*2, getHeight()-radius*2, radius*2, radius*2, 0.5*double_Pi, double_Pi);

		outlinePath.lineTo(0, height);
		//outlinePath.addArc(0, height-radius*2, radius*2, radius*2, double_Pi, 1.5*double_Pi);

		outlinePath.lineTo(0, 0);
		//outlinePath.closeSubPath();

	} else if (getName().equalsIgnoreCase("none"))
	{

		//outlinePath.startNewSubPath(0, 0);

		outlinePath.lineTo(width, 0);
	
		//outlinePath.addArc(width-radius*2, 0, radius*2, radius*2, 0, 0.5*double_Pi);

		outlinePath.lineTo(width, height);

		//outlinePath.addArc(width-radius*2, height-radius*2, radius*2, radius*2, 0.5*double_Pi, double_Pi);

		outlinePath.lineTo(0, height);

		outlinePath.lineTo(0, 0);
		//outlinePath.closeSubPath();

	}

}


void EditorButton::paintButton(Graphics &g, bool isMouseOver, bool isButtonDown)
{

	g.setColour(Colours::grey);
	g.fillPath(outlinePath);

	 if (getToggleState())
     {
     	if (isMouseOver && isEnabled)
     		g.setGradientFill(selectedOverGrad);
        else
        	g.setGradientFill(selectedGrad);
     } else {
         if (isMouseOver && isEnabled)
         	g.setGradientFill(neutralOverGrad);
        else
        	g.setGradientFill(neutralGrad);
     }

	AffineTransform a = AffineTransform::scale(0.98f, 0.94f, float(getWidth())/2.0f,
												float(getHeight())/2.0f);
	g.fillPath(outlinePath, a);

	buttonFont.setHeight(10.0f);
	int stringWidth = buttonFont.getStringWidth(getName());

	g.setFont(buttonFont);

	if (isEnabled)
		g.setColour(Colours::darkgrey);
	else
		g.setColour(Colours::lightgrey);

	g.drawSingleLineText(getName(), getWidth()/2 - stringWidth/2, 11);

}


ChannelSelectorButton::ChannelSelectorButton(int num_, int type_, Font& f) : Button("name") 
{
	isActive = true;
	num = num_;
	type = type_;

    setClickingTogglesState(true);

    buttonFont = f;
    buttonFont.setHeight(10);
}


void ChannelSelectorButton::paintButton(Graphics &g, bool isMouseOver, bool isButtonDown)
{
	if (isActive) {
		if (getToggleState() == true)
		    g.setColour(Colours::orange);
		else 
		    g.setColour(Colours::darkgrey);

		if (isMouseOver)
		    g.setColour(Colours::white);
	} else {
		if (getToggleState() == true)
			g.setColour(Colours::yellow);
		else
			g.setColour(Colours::lightgrey);
	}

    // g.fillRect(0,0,getWidth(),getHeight());

    g.setFont(buttonFont);

    // g.drawRect(0,0,getWidth(),getHeight(),1.0);

    g.drawText(String(num),0,0,getWidth(),getHeight(),Justification::centred,true);
}

void ChannelSelectorButton::setActive(bool t)
{
	isActive = t;
	setClickingTogglesState(t);
}