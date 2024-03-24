/**
 * @file
 * The <C>FluidSynthPlugin_Editor</C> module implements the editor
 * for the processor of MIDI events from the DAW via JUCE to the
 * FluidSynth library.
 *
 * @author Dr. Thomas Tensi
 * @date   2022-04
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "Logging.h"
#include "Percentage.h"
#include "FluidSynthPlugin_Editor.h"

/*-----------------------*/

using BaseTypes::Primitives::Percentage;
using Main::FluidSynthPlugin::FluidSynthPlugin_Editor;

/*====================*/
/* Colour settings    */
/*====================*/

/** combines className and prefix into a JUCE colour id name */
#define colourId2(className, prefix)                    \
    juce::className::ColourIds::prefix##ColourId

/** combines className, prefix and partText into a JUCE colour
 * id name */
#define colourId3(className, prefix, partText)                  \
    juce::className::ColourIds::prefix##Colour##partText##Id

/** the background color for this editor widget */
static juce::Colour _backgroundColour{0xffd0d0d0};

/** the color of the button in this editor widget */
static juce::Colour _buttonColour{0xffc0c0c0};

/** the color for labels in this editor widget */
static juce::Colour _labelTextColour = juce::Colours::white;

/** the color for labels in this editor widget */
static juce::Colour _errorInformationTextColour = juce::Colours::red;

/** the background color for the text edit field in this editor widget */
static juce::Colour _standardEditorBackgroundColour =
    juce::Colours::white;

/** the background color for the text in this editor widget when the data
  * has been confirmed by the user */
static juce::Colour _stableEditorBackgroundColour{0xffe0e0e0};

/** the background color for the text in this editor widget when the data
  * has been checked as erroneous by the system */
static juce::Colour _erroneousEditorBackgroundColour{0xffffe0e0};

/** the text color in the edit field of this editor widget */
static juce::Colour _textColour = juce::Colours::black;

/** no notification kind for some widget */
static juce::NotificationType _noNotification =
    juce::NotificationType::dontSendNotification;

/*====================*/

namespace Main::FluidSynthPlugin {

    struct _EditorDescriptor;

    /*====================*/

    /**
     * A listener class for button and text editor events
     */
    struct _WidgetListener : juce::Button::Listener,
                             juce::TextEditor::Listener
    {

        /**
         * Creates a listener with an editor descriptor reference.
         *
         * @param[in] editorDescriptor  an editor descriptor for
         *                              callbacks
         */
        _WidgetListener (IN _EditorDescriptor* editorDescriptor)
        {
            _editorDescriptor = (_EditorDescriptor*) editorDescriptor;
        }

        /*--------------------*/

        /**
         * Reacts on a button click
         */
        void buttonClicked (juce::Button*);

        /*--------------------*/

        /**
         * Reacts on a change in text
         */
        void textEditorTextChanged (juce::TextEditor &);

        /*--------------------*/

        private:

            /** the internal editor descriptor object */
            _EditorDescriptor* _editorDescriptor;

    };

    /*====================*/

    /**
     * The internal data for the editor combined into a type
     */
    struct _EditorDescriptor {

        /** the associated event processor*/
        FluidSynthPlugin_EventProcessor& processor;

        /** a text editor widget */
        juce::TextEditor& textEditorWidget;

        /** a button widget for confirming the change */
        juce::Button& buttonWidget;

        /** a label for showing the compilation information */
        juce::Label& labelWidget;

        /** a label for showing error information (if applicable) */
        juce::Label& errorInformationWidget;

        /** the current settings string stored in the editor */
        String settingsString;

        /** the current error string (if any) */
        String errorString;

        /*--------------------*/

        /**
         * Initializes the widgets in this editor descriptor
         *
         * @param[inout] processor  the associated event processor
         */
        _EditorDescriptor
            (INOUT FluidSynthPlugin_EventProcessor& processor)
            : processor{processor},
              textEditorWidget{*(new juce::TextEditor())},
              buttonWidget{*(new juce::TextButton())},
              labelWidget{*(new juce::Label())},
              errorInformationWidget{*(new juce::Label())},
              settingsString{processor.settings()},
              _widgetListener{*(new _WidgetListener(this))}
        {
            textEditorWidget.addListener(&_widgetListener);
            textEditorWidget.setMultiLine(true, false);
            textEditorWidget.setReturnKeyStartsNewLine(true);
            textEditorWidget.setColour(colourId2(TextEditor,background),
                                       _stableEditorBackgroundColour);
            textEditorWidget.setColour(colourId2(TextEditor, text),
                                       _textColour);
            textEditorWidget.setText(settingsString, false);

            buttonWidget.addListener(&_widgetListener);
            buttonWidget.setButtonText("Confirm");
            buttonWidget.setColour(colourId2(TextButton, button),
                                   _buttonColour);
            buttonWidget.setColour(colourId3(TextButton, text, Off),
                                   _textColour);

            const char* labelText =
                "FluidSynthPlugin (" __DATE__ " " __TIME__ ")";
            labelWidget.setText(labelText, _noNotification);
            labelWidget.setColour(colourId2(Label, text),
                                  _labelTextColour);
            errorInformationWidget.setText("", _noNotification);
            errorInformationWidget.setColour(colourId2(Label, text),
                                             _errorInformationTextColour);
        }

        /*--------------------*/

        /**
         * Destroys the widgets in this editor descriptor
         */
        ~_EditorDescriptor ()
        {
            delete &buttonWidget;
            delete &errorInformationWidget;
            delete &labelWidget;
            delete &textEditorWidget;
            delete &_widgetListener;
        }

        /*--------------------*/

        private:

            /** the listener for button and text editor */
            _WidgetListener& _widgetListener;

    };

}

/*====================*/

using Main::FluidSynthPlugin::_EditorDescriptor;
using Main::FluidSynthPlugin::_WidgetListener;

/*====================*/

/**
 * Updates text field and error field of <C>editorDescriptor</C>; when
 * <C>processorIsUpdated</C> is set, the contents of the text field
 * are written to the audio processor, otherwise only text field
 * background and error message are updated
 *
 * @param[inout] editorDescriptor    descriptor for editor object
 * @param[in]    processorIsUpdated  information whether text is
 *                                   written to underlying processor
 */
static void
_updateAfterValidation (INOUT _EditorDescriptor& editorDescriptor,
                        IN Boolean processorIsUpdated)
{
    
    juce::TextEditor& textEditorWidget =
        editorDescriptor.textEditorWidget;
    FluidSynthPlugin_EventProcessor& processor =
        editorDescriptor.processor;

    if (!processorIsUpdated) {
        textEditorWidget.setText(processor.settings(), false);
    } else {
        /* store this locally and update processor */
        String st{textEditorWidget.getText().toStdString()};
        processor.setSettings(st);
        editorDescriptor.settingsString = st;
    }

    String errorString = processor.errorString();
    editorDescriptor.errorString = errorString;

    juce::Colour editorWidgetBackgroundColour =
        (errorString == ""
         ? _stableEditorBackgroundColour
         : _erroneousEditorBackgroundColour);
    textEditorWidget.setColour(colourId2(TextEditor,background),
                               editorWidgetBackgroundColour);
    textEditorWidget.repaint();
}

/*====================*/

void _WidgetListener::buttonClicked (juce::Button*)
{
    _EditorDescriptor& editorDescriptor = *_editorDescriptor;
    _updateAfterValidation(editorDescriptor, true);
}

/*--------------------*/
    
/**
 * Reacts on a change in text
 */
void _WidgetListener::textEditorTextChanged (juce::TextEditor &)
{
    _EditorDescriptor& editorDescriptor = *_editorDescriptor;
    juce::TextEditor& textEditorWidget =
        editorDescriptor.textEditorWidget;
    textEditorWidget.setColour(colourId2(TextEditor,background),
                               _standardEditorBackgroundColour);
    textEditorWidget.repaint();
    editorDescriptor.errorString = "";
}

/*====================*/
/* PRIVATE FEATURES   */
/*====================*/

/**
 * Constructs a new rectangle as a subarea of <C>rectangle</C> with
 * a horizontal indentation of 5% and a vertical range (in percentages)
 * from <C>startY</C> to <C>endY</C>
 *
 * @param[in] rectangle  an enclosing rectangle
 * @param[in] startY     the top y-position as percentage from the top
 *                       of the enclosing rectangle
 * @param[in] endY       the bottom y-position as percentage from the
 *                       top of the enclosing rectangle
 * @return  rectangle with 5% horizontal indentation and given y-interval
 */
static
juce::Rectangle<int> _subArea (IN juce::Rectangle<int> rectangle,
                               IN Percentage startY,
                               IN Percentage endY)
{
    juce::Rectangle<int> result;
    const Percentage xIndent{5.0};
    Natural width{(size_t) rectangle.getWidth()};
    const Natural x = xIndent.of(width) + (size_t) rectangle.getX();
    const Percentage widthPercentage{90.0};
    width = widthPercentage.of(width);
    Natural height{(size_t) rectangle.getHeight()};
    const Natural y = startY.of(height) + (size_t) rectangle.getY();
    height = endY.of(height) - startY.of(height);
    result.setBounds((int) x, (int) y, (int) width, (int) height);
    return result;
}

/*====================*/
/* PUBLIC FEATURES    */
/*====================*/

/*-----------------------*/
/* setup and destruction */
/*-----------------------*/

FluidSynthPlugin_Editor
::FluidSynthPlugin_Editor
      (INOUT FluidSynthPlugin_EventProcessor& processor)
    : juce::AudioProcessorEditor(&processor),
    _descriptor{new _EditorDescriptor(processor)}
{
    Logging_trace(">>");

    /* set up editor descriptor */
    _EditorDescriptor& editorDescriptor =
        TOREFERENCE<_EditorDescriptor>(_descriptor);

    addAndMakeVisible(editorDescriptor.textEditorWidget);
    addAndMakeVisible(editorDescriptor.buttonWidget);
    addAndMakeVisible(editorDescriptor.labelWidget);
    addAndMakeVisible(editorDescriptor.errorInformationWidget);

    setResizable(true, true);
    setSize(400, 400);

    Logging_trace("<<");
}

/*--------------------*/

FluidSynthPlugin_Editor::~FluidSynthPlugin_Editor ()
{
    Logging_trace(">>");

    _EditorDescriptor& editorDescriptor =
        TOREFERENCE<_EditorDescriptor>(_descriptor);
    delete &editorDescriptor;
    
    Logging_trace("<<");
}

/*--------------------*/
/* event handling     */
/*--------------------*/

void FluidSynthPlugin_Editor::paint (INOUT juce::Graphics& context)
{
    Logging_trace(">>");

    _EditorDescriptor& editorDescriptor =
        TOREFERENCE<_EditorDescriptor>(_descriptor);
    juce::Label& errorWidget = editorDescriptor.errorInformationWidget;
    String errorWidgetString = errorWidget.getText().toStdString();
    String errorString = editorDescriptor.errorString;

    if (errorWidgetString != errorString) {
        errorWidget.setText(errorString, _noNotification);
        resized();
    }

    context.fillAll(_backgroundColour);

    Logging_trace("<<");
}

/*--------------------*/

void FluidSynthPlugin_Editor::resized ()
{
    Logging_trace(">>");

    _EditorDescriptor& editorDescriptor =
        TOREFERENCE<_EditorDescriptor>(_descriptor);

    juce::TextEditor& textEditorWidget =
        editorDescriptor.textEditorWidget;
    juce::Button& buttonWidget =
        editorDescriptor.buttonWidget;
    juce::Label& labelWidget =
        editorDescriptor.labelWidget;
    juce::Label& errorInformationWidget =
        editorDescriptor.errorInformationWidget;

    /* layout the widgets and the text */
    auto boundingBox = getLocalBounds();
    const Boolean errorsMustBeShown = (editorDescriptor.errorString > "");
    Percentage deltaY = 2.0;
    Percentage labelHeight = 5.0;
    Percentage buttonHeight = 8.0;

    Percentage buttonTopY = 85.0;
    Percentage textTopY = 5.0;
    Percentage labelBottomY = 100.0;
    Percentage textBottomY = (buttonTopY - deltaY
                              - (errorsMustBeShown
                                 ?  labelHeight + deltaY
                                 : 0.0));
    Percentage errorTopY = textBottomY + deltaY;
    Percentage errorBottomY = (errorTopY
                               + (errorsMustBeShown ? labelHeight : 0.0));
    Percentage buttonBottomY = buttonTopY + buttonHeight;
    Percentage labelTopY    = buttonBottomY + deltaY;

    textEditorWidget.setBounds(_subArea(boundingBox,
                                        textTopY, textBottomY));
    errorInformationWidget.setBounds(_subArea(boundingBox,
                                              errorTopY, errorBottomY));
    buttonWidget.setBounds(_subArea(boundingBox,
                                    buttonTopY, buttonBottomY));
    labelWidget.setBounds(_subArea(boundingBox,
                                   labelTopY, labelBottomY));


    Logging_trace("<<");
}

/*--------------------*/

void FluidSynthPlugin_Editor::update ()
{
    Logging_trace(">>");

    _EditorDescriptor& editorDescriptor =
        TOREFERENCE<_EditorDescriptor>(_descriptor);
    _updateAfterValidation(editorDescriptor, false);

    Logging_trace("<<");
}
