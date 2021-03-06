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

#ifndef __VISUALIZEREDITOR_H_17E6D78C__
#define __VISUALIZEREDITOR_H_17E6D78C__

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "GenericEditor.h"
#include "../../UI/UIComponent.h"
#include "../../UI/DataViewport.h"
#include "../Visualization/DataWindow.h"
#include "../Visualization/Visualizer.h"

/**
  
  Base class for creating editors with visualizers.

  @see GenericEditor, Visualizer

*/

class DataWindow;
class Visualizer;

class SelectorButton : public Button
{
	public:
		SelectorButton(const String& name);
		~SelectorButton();	
	private:
		void paintButton(Graphics& g, bool isMouseOver, bool isButtonDown);
};

class VisualizerEditor : public GenericEditor
{
public:
	VisualizerEditor (GenericProcessor*, int);
	VisualizerEditor (GenericProcessor*);
	~VisualizerEditor();

	void buttonEvent (Button* button);
	virtual void buttonCallback(Button* button) {}

	virtual Visualizer* createNewCanvas() = 0;

	virtual void enable();
	virtual void disable();

	void editorWasClicked();

	void updateVisualizer();

	ScopedPointer<DataWindow> dataWindow;
	ScopedPointer<Visualizer> canvas;

	String tabText;

private:	

	void initializeSelectors();
	bool isPlaying;

	SelectorButton* windowSelector;
	SelectorButton* tabSelector;

	int tabIndex;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualizerEditor);

};



#endif  // __VISUALIZEREDITOR_H_17E6D78C__
