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

#include "Assertion.h"
#include "Environment.h"
#include "Logging.h"
#include "FluidSynthPlugin_Editor.h"
#include "FluidSynthPlugin_EditorSupport.h"

/*-----------------------*/

using BaseModules::Environment;
using Main::FluidSynthPlugin::FluidSynthPlugin_Editor;
using Main::FluidSynthPlugin::FluidSynthPlugin_EditorSupport;

/** abbreviation for StringUtil */
using STR = BaseModules::StringUtil;

/*====================*/

/*--------------------*/
/* Colour settings    */
/*--------------------*/

/** the background color for this editor widget */
static const juce::Colour _Colour_background{0xffd0d0d0};

/** the color of the button in this editor widget */
static const juce::Colour _Colour_button{0xffc0c0c0};

/** the color for labels in this editor widget */
static const juce::Colour _Colour_labelText = juce::Colours::white;

/** the color for labels in this editor widget */
static const juce::Colour _Colour_errorInformationText = juce::Colours::red;

/** the background color for the text edit field in this editor widget */
static const juce::Colour _Colour_standardEditorBackground =
    juce::Colours::white;

/** the background color for the text in this editor widget when the data
  * has been confirmed by the user */
static const juce::Colour _Colour_stableEditorBackground{0xffe0e0e0};

/** the background color for the text in this editor widget when the data
  * has been checked as erroneous by the system */
static const juce::Colour _Colour_erroneousEditorBackground{0xffffe0e0};

/** the text color in the edit field of this editor widget */
static const juce::Colour _Colour_text = juce::Colours::black;

/*--------------------*/
/* Misc Variables     */
/*--------------------*/

/** no notification kind for some widget */
static const juce::NotificationType _noNotification =
    juce::NotificationType::dontSendNotification;

/** key strings used for soundfont specification */
static const StringList _KeyNameList_soundFont =
    StringList::makeBySplit("soundfont", ",");

/** key strings used for preset specification */
static const StringList _KeyNameList_preset =
    StringList::makeBySplit("preset,program", ",");

/** context menu item index for about dialog */
static const int _ContextItemIndex_about = -1002;

/** context menu item index for soundfont selection */
static const int _ContextItemIndex_file = -1001;

/** context menu item index for preset selection */
static const int _ContextItemIndex_preset = -1000;

/** context menu item text for about dialog */
static const String _ContextItemText_about = "About...";

/** context menu item text for soundfont selection */
static const String _ContextItemText_file = "Select SoundFont File...";

/** context menu item text for preset selection */
static const String _ContextItemText_preset = "Select Preset...";

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
        _WidgetListener (IN _EditorDescriptor* editorDescriptor);

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
     * A redefinition of the text editor adding popup menu items
     */
    struct _TextWidget : public juce::TextEditor {

        /**
         * Initializes text widget
         *
         * @param[in] editorDescriptor  containing editor descriptor
         */
        _TextWidget (IN _EditorDescriptor* editorDescriptor);

        /*--------------------*/

        /**
         * Adds items to popup menu
         */
        void addPopupMenuItems (juce::PopupMenu& menu,
                                const juce::MouseEvent* event) override;

        /*--------------------*/

        /**
         * Handles popup menu selection
         */
        void performPopupMenuAction (int menuItem) override;

        /*--------------------*/

        private:

            /** the editor descriptor containing the text widget */
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
        _TextWidget& textEditorWidget;

        /** a button widget for confirming the change */
        juce::Button& buttonWidget;

        /** a label for showing error information (if applicable) */
        juce::Label& errorInformationWidget;

        /** the current settings string stored in the editor */
        String settingsString;

        /** the current error string (if any) */
        String errorString;

        /*--------------------*/
        /* con-/destruction   */
        /*--------------------*/

        /**
         * Initializes the widgets in this editor descriptor
         *
         * @param[inout] processor  the associated event processor
         */
        _EditorDescriptor
            (INOUT FluidSynthPlugin_EventProcessor& processor);

        /*--------------------*/

        /**
         * Destroys the widgets in this editor descriptor
         */
        ~_EditorDescriptor ();

        /*--------------------*/
        /* event handling     */
        /*--------------------*/

        /**
         * Handles callback event from context menu
         *
         * @param[in] menuItemIndex  index of context menu item
         */
        void handleContextMenuSelection (Integer menuItemIndex);

        /*--------------------*/

        private:

            /** the listener for button and text editor */
            _WidgetListener& _widgetListener;

    };

}

/*====================*/

using Main::FluidSynthPlugin::_EditorDescriptor;
using Main::FluidSynthPlugin::_TextWidget;
using Main::FluidSynthPlugin::_WidgetListener;

/*====================*/
/* STATIC ROUTINES    */
/*====================*/

/**
 * Replaces occurences of <C>key</C> in dictionary string <C>st</C> by
 * <C>value</C>; record separator is a newline, field separator an
 * equals sign
 *
 * @param[inout] st     dictionary string to be adapted
 * @param[in]    key    key to be searched for
 * @param[in]    value  new value for key
 * @return  information whether some change was done
 */
static Boolean _adaptSettingsString (INOUT String& st,
                                     IN String& key,
                                     IN String& value)
{
    Logging_trace3(">>: st = %1, key = %2, value = %3",
                   st, key, value);

    const String recordSeparator{"#"};
    const String fieldSeparator{"="};
    const String nlSt = STR::newlineReplacedString(st, recordSeparator);

    Boolean isChanged = false;
    Boolean isFound = false;
    StringList recordList = StringList::makeBySplit(nlSt, recordSeparator);

    for (String& record : recordList) {
        String recordKey;
        String recordValue;
        Boolean hasSeparator = STR::splitAt(record, fieldSeparator,
                                            recordKey, recordValue);

        if (hasSeparator) {
            recordKey   = STR::strip(recordKey);
            recordValue = STR::strip(recordValue);

            if (recordKey == key) {
                isFound = true;

                if (recordValue != value) {
                    isChanged = true;
                    STR::replace(record, recordValue, value);
                }
            }
        }
    }

    // if the key is not found, add a new record
    if (!isFound) {
        String record = key + " = " + value;
        recordList.append(record);
        isChanged = true;
    }

    // when there is some change, update st
    if (isChanged) {
        st = recordList.join("\n");
    }

    Logging_trace1("<<: %1", TOSTRING(isChanged));
    return isChanged;
}

/*--------------------*/

/**
 * Adapts file name <C>st</C> by replacing backslashes and trying to
 * match environment variables as possible prefices
 *
 * @param[in] st  original file name
 * @return  adapted version of file name with backlashes replaced and
 *          possible prefix environment variable inserted
 */
static String _normalizedFileName (IN String& st)
{
    Logging_trace1(">>: %1", st);

    String result = st;
    Environment::replacePrefix(result);
    STR::replace(result, "\\", "/");
    
    Logging_trace1("<<: %1", result);
    return result;
}

/*--------------------*/
    
/**
 * Repaints text widget of <C>editorDescriptor</C> with background
 * color <C>backgroundColour</C>
 *
 * @param[inout] editorDescriptor  descriptor for editor object
 * @param[in]    backgroundColour  new background color for text
 *                                 widget
 */
static
void _repaintTextWidget (INOUT _EditorDescriptor& editorDescriptor,
                         IN juce::Colour backgroundColour)
{
    juce::TextEditor& textEditorWidget =
        editorDescriptor.textEditorWidget;
    textEditorWidget.setColour(colourId2(TextEditor,background),
                               backgroundColour);
    textEditorWidget.repaint();
}

/*--------------------*/
    
/**
 * Selectively updates string in text editor with information
 * <C>key</C> and <C>value</C>
 *
 * @param[inout] editorDescriptor  descriptor for editor object
 * @param[in]    key               key to be updated
 * @param[in]    value             new value for key
 */
static void _setKeyInWidget (INOUT _EditorDescriptor& editorDescriptor,
                             IN String& key,
                             IN String& value)
{
    Logging_trace2(">>: key = %1, value = %2", key, value);

    Boolean isChanged =
        _adaptSettingsString(editorDescriptor.settingsString, key, value);

    if (isChanged) {
        juce::TextEditor& textEditorWidget =
            editorDescriptor.textEditorWidget;
        textEditorWidget.setText(editorDescriptor.settingsString, false);
        _repaintTextWidget(editorDescriptor,
                           _Colour_standardEditorBackground);
    }
    
    Logging_trace("<<");
}

/*--------------------*/
    
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
    Logging_trace(">>");

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
    juce::Colour backgroundColour =
        (errorString == ""
         ? _Colour_stableEditorBackground
         : _Colour_erroneousEditorBackground);
    _repaintTextWidget(editorDescriptor, backgroundColour);

    Logging_trace("<<");
}

/*--------------------*/
    
/**
 * Returns value from first match of <C>keyList</C> in settings string
 * <C>st</C>
 *
 * @param[in] st            settings string (with newline as record separator)
 * @param[in] keyList       list of keys to be found (first match wins)
 * @param[in] effectiveKey  key effectively used for value
 * @return  value for key (or empty string when not found)
 */
static String _valueFromKeyList (IN String& st,
                                 IN StringList& keyList,
                                 OUT String& effectiveKey)
{
    Logging_trace2(">>: keyList = '%1', st = '%2'",
                   keyList.toString(), st);

    String result;
    const String entrySeparator{"##"};
    const String nlSt = STR::newlineReplacedString(st, entrySeparator);
    Dictionary d =
        Dictionary::makeFromString(nlSt, entrySeparator, "=");

    for (String key : keyList) {
        if (d.contains(key)) {
            String value = d.at(key);
            effectiveKey = key;
            result = Environment::expand(value);
        }
    }

    Logging_trace2("<<: result = %1, effectiveKey = %2",
                   result, effectiveKey);
    return result;
}

/*--------------------*/
    
/**
 * Returns value from first matching key from <C>keyList</C> in
 * <C>editorDescriptor</C>.
 *
 * @param[in] editorDescriptor  descriptor for editor object
 * @param[in] keyList           list of keys to be found (first match wins)
 * @param[in] effectiveKey      key effectively used for value
 * @return  value for key (or empty string when not found)
 */
static String _valueFromWidget (IN _EditorDescriptor& editorDescriptor,
                                IN StringList& keyList,
                                OUT String& effectiveKey)
{
    Logging_trace1(">>: keyList = %1", keyList.toString());

    String result = _valueFromKeyList(editorDescriptor.settingsString,
                                      keyList, effectiveKey);
    Logging_trace2("<<: result = %1, effectiveKey = %2",
                   result, effectiveKey);
    return result;
}

/*====================*/
/* _EditorDescriptor  */
/*====================*/

_EditorDescriptor::_EditorDescriptor
    (INOUT FluidSynthPlugin_EventProcessor& processor)
    : processor{processor},
      textEditorWidget{*(new _TextWidget(this))},
      buttonWidget{*(new juce::TextButton())},
      errorInformationWidget{*(new juce::Label())},
      settingsString{processor.settings()},
      _widgetListener{*(new _WidgetListener(this))}
{
    textEditorWidget.addListener(&_widgetListener);
    textEditorWidget.setMultiLine(true, false);
    textEditorWidget.setReturnKeyStartsNewLine(true);
    textEditorWidget.setColour(colourId2(TextEditor, background),
                               _Colour_stableEditorBackground);
    textEditorWidget.setColour(colourId2(TextEditor, text),
                               _Colour_text);
    textEditorWidget.setText(settingsString, false);

    buttonWidget.addListener(&_widgetListener);
    buttonWidget.setButtonText("Confirm");
    buttonWidget.setColour(colourId2(TextButton, button),
                           _Colour_button);
    buttonWidget.setColour(colourId3(TextButton, text, Off),
                           _Colour_text);

    errorInformationWidget.setText("", _noNotification);
    errorInformationWidget.setColour(colourId2(Label, text),
                                     _Colour_errorInformationText);
}

/*--------------------*/
    
_EditorDescriptor::~_EditorDescriptor ()
{
    delete &buttonWidget;
    delete &errorInformationWidget;
    delete &textEditorWidget;
    delete &_widgetListener;
}

/*--------------------*/

void
_EditorDescriptor::handleContextMenuSelection (Integer menuItemIndex)
{
    Logging_trace1(">>: %1", TOSTRING(menuItemIndex));

    if (menuItemIndex == _ContextItemIndex_about) {
        String fsLibraryVersion = processor.fsLibraryVersion();
        FluidSynthPlugin_EditorSupport
            ::showInformationDialog(fsLibraryVersion);
    } else {
        Boolean isFileSelection =
            menuItemIndex == _ContextItemIndex_file;
        Boolean isChanged;
        String st;
        String previousValue;
        String variableName = "";
        StringList variableNameList = (isFileSelection
                                       ? _KeyNameList_soundFont
                                       : _KeyNameList_preset);

        previousValue = _valueFromWidget(*this,
                                         variableNameList,
                                         variableName);
        
        if (isFileSelection) {
            isChanged =
                FluidSynthPlugin_EditorSupport
                    ::selectFileByDialog(previousValue, st);
        } else {
            MidiPresetIdentification identification{previousValue};
            StringList presetList = processor.presetList();
            isChanged =
                FluidSynthPlugin_EditorSupport
                    ::selectPresetByDialog(presetList, identification);
            st = (isChanged ? identification.toString() : previousValue);
        }


        if (isChanged) {
            st = (isFileSelection ? _normalizedFileName(st) : st);
            _setKeyInWidget(*this, variableName, st);
        }
    }

    Logging_trace("<<");
}

/*====================*/
/* _TextWidget        */
/*====================*/

_TextWidget::_TextWidget (IN _EditorDescriptor* editorDescriptor)
    : _editorDescriptor{(_EditorDescriptor*) editorDescriptor}
{
}

/*--------------------*/

void _TextWidget::addPopupMenuItems (juce::PopupMenu& menu,
                                     const juce::MouseEvent* event)
{
    /* add standard items */
    juce::TextEditor::addPopupMenuItems(menu, event);

    /* add separator and file selection menu item*/
    menu.addSeparator();
    menu.addItem(_ContextItemIndex_file, _ContextItemText_file);

    /* add preset selection menu item only when soundfont in
       processor settings is equal to soundfont in text widget i.e. it
       has been confirmed before */
    String editorSettings = getText().toStdString();
    String processorSettings = _editorDescriptor->processor.settings();

    String variableName;
    String sfA = _valueFromKeyList(editorSettings, _KeyNameList_soundFont,
                                   variableName);
    String sfB = _valueFromKeyList(processorSettings, _KeyNameList_soundFont,
                                   variableName);

    Boolean itemIsEnabled = (sfA > "" && sfA == sfB);
    menu.addItem(_ContextItemIndex_preset, _ContextItemText_preset,
                 itemIsEnabled);

    /* add about dialog */
    menu.addSeparator();
    menu.addItem(_ContextItemIndex_about, _ContextItemText_about);
}

/*--------------------*/

void _TextWidget::performPopupMenuAction (int menuItem)
{
    if (menuItem >= _ContextItemIndex_about
        && menuItem <= _ContextItemIndex_preset) {
        _editorDescriptor->handleContextMenuSelection(menuItem);
    } else {
        juce::TextEditor::performPopupMenuAction(menuItem);
    }
}

/*====================*/
/* _WidgetListener    */
/*====================*/

_WidgetListener::_WidgetListener (IN _EditorDescriptor* editorDescriptor)
    : _editorDescriptor{(_EditorDescriptor*) editorDescriptor}
{
}

/*--------------------*/
    
void _WidgetListener::buttonClicked (juce::Button*)
{
    _EditorDescriptor& editorDescriptor = *_editorDescriptor;
    _updateAfterValidation(editorDescriptor, true);
}

/*--------------------*/
    
void _WidgetListener::textEditorTextChanged (juce::TextEditor &)
{
    _EditorDescriptor& editorDescriptor = *_editorDescriptor;
    editorDescriptor.errorString = "";
    _repaintTextWidget(editorDescriptor,
                       _Colour_standardEditorBackground);
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
/* queries            */
/*--------------------*/

bool FluidSynthPlugin_Editor::isInterestedInFileDrag
         (const juce::StringArray &fileNameList)
{
    Logging_trace(">>");

    Boolean result;

    if (fileNameList.size() != 1) {
        result = false;
    } else {
        String fileName = fileNameList[0].toStdString();
        Logging_trace1("--: fileName = %1", fileName);
        fileName = STR::toLowercase(fileName);
        result = (STR::endsWith(fileName, ".sf2")
                  || STR::endsWith(fileName, ".sf3"));
    }

    Logging_trace1("<<: %1", TOSTRING(result));
    return (bool) result;
}

/*--------------------*/
/* event handling     */
/*--------------------*/

void FluidSynthPlugin_Editor::filesDropped
         (const juce::StringArray &fileNameList,
          int x,
          int y)
{
    Logging_trace(">>");

    Assertion_check(fileNameList.size() == 1,
                    "only a single soundfont file may be dropped");
    String fileName = _normalizedFileName(fileNameList[0].toStdString());
    _EditorDescriptor& editorDescriptor =
        TOREFERENCE<_EditorDescriptor>(_descriptor);
    _setKeyInWidget(editorDescriptor, _KeyNameList_soundFont.at(0), fileName);

    Logging_trace("<<");
}

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

    context.fillAll(_Colour_background);

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
    Percentage textBottomY = (buttonTopY - deltaY
                              - (errorsMustBeShown
                                 ?  labelHeight + deltaY
                                 : 0.0));
    Percentage errorTopY = textBottomY + deltaY;
    Percentage errorBottomY = (errorTopY
                               + (errorsMustBeShown ? labelHeight : 0.0));
    Percentage buttonBottomY = buttonTopY + buttonHeight;

    textEditorWidget.setBounds(_subArea(boundingBox,
                                        textTopY, textBottomY));
    errorInformationWidget.setBounds(_subArea(boundingBox,
                                              errorTopY, errorBottomY));
    buttonWidget.setBounds(_subArea(boundingBox,
                                    buttonTopY, buttonBottomY));


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
