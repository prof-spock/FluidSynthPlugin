/**
 * @file
 * The <C>FluidSynthPlugin_EditorSupport</C> module implements support
 * services for the editor of the FluidSynthPlugin audio processor.
 *
 * @author Dr. Thomas Tensi
 * @date   2024-03
 */

/*=========*/
/* IMPORTS */
/*=========*/

#include "JuceHeaders.h"

#include "BuildInformation.h"
#include "GenericMap.h"
#include "Logging.h"
#include "NaturalList.h"
#include "NaturalSet.h"
#include "FluidSynthPlugin_EditorSupport.h"

using BaseTypes::GenericTypes::GenericMap;
using BaseTypes::Containers::NaturalList;
using BaseTypes::Containers::NaturalSet;
using Main::BuildInformation;
using Main::FluidSynthPlugin::FluidSynthPlugin_EditorSupport;

using STR = BaseModules::StringUtil;

/*====================*/
/* PROTOTYPES         */
/*====================*/

static Natural _splitOffLeadingNumber (IN String& st);

/*====================*/
/* LOCAL FEATURES     */
/*====================*/

/** label for cancel button in preset selection component */
static const juce::String _ButtonLabel_cancel = "Cancel";

/** label for okay button in preset selection component */
static const juce::String _ButtonLabel_okay = "Ok";

/** color of buttons */
static const juce::Colour _Colour_button{0xFFC0C0C0};

/** colour for text */
static const juce::Colour _Colour_text{0xFF000000};

/** colour for list box item background when selected */
static const juce::Colour _Colour_lboxItemBkgSelected{0xFF000000};

/** colour for list box item background when unselected */
static const juce::Colour _Colour_lboxItemBkgUnselected{0xFFFFFFFF};

/** colour for list box item text when selected */
static const juce::Colour _Colour_lboxItemTxtSelected{0xFFFFFF00};

/** colour for list box item text when unselected */
static const juce::Colour _Colour_lboxItemTxtUnselected{_Colour_text};

/** colour for list box background */
static const juce::Colour _Colour_listBoxBackground{0xFFE8E8E8};

/** colour for list box outline */
static const juce::Colour _Colour_listBoxOutline{0xFF000000};

/** colour for background of preset selection component */
static const juce::Colour _Colour_presetSelectionComponentBkg{0xFFD0D0D0};

/** label for bank in preset selection component */
static const String _Identification_bank = "Bank";

/** label for program in preset selection component */
static const String _Identification_program = "Program";

/** title for file selection component */
static const juce::String _Title_fileSelection = "Select Soundfont";

/** title for preset selection component */
static const juce::String _Title_presetSelection = "Select Preset";

/** number of digits for bank number and program number in list
 * boxes */
static const Natural _listBoxNumberDigitCount = 3;

/*--------------------*/

/** mapping from natural to a list of strings */
using NaturalToStringListMap =
    GenericMap<Natural, StringList,
               Natural::toString, StringList::toString>;

/*--------------------*/
/*--------------------*/

namespace Main::FluidSynthPlugin {

    struct _PresetSelectionComponent;

    /*====================*/

    /**
     * A very elementary list box model for a string list
     */
    struct _StringListBoxModel : juce::ListBoxModel {

        /**
         * Initializes list box model as subcomponent of
         * <C>component</C> named <C>name</C>.
         *
         * @param[in] component  enclosing component
         */
        _StringListBoxModel (IN _PresetSelectionComponent* component,
                             IN String& name);

        /*--------------------*/

        /**
         * Returns number of rows.
         *
         * @return  list length
         */
        int getNumRows () override;
        
        /*--------------------*/

        /**
         * Returns the list of strings in this model
         *
         * @return list of strings
         */
        const StringList& list () const;

        /*--------------------*/

        /**
         * Tells that <C>row</C> has been double clicked.
         *
         * @param[in] row         double-clicked row
         * @param[in] mouseEvent  associated mouse event
         */
        void listBoxItemDoubleClicked (int row,
                                       const juce::MouseEvent& mouseEvent)
            override;

        /*--------------------*/
    
        /**
         * Returns currently selected item.
         *
         * @return  selected item
         */
        Natural markedItem () const;

        /*--------------------*/
        
        /**
         * Returns name of list
         *
         * @return  list name
         */
        String name () const;

        /*--------------------*/

        /**
         * Draws a row of the list.
         */
        void paintListBoxItem (int rowNumber,
                               juce::Graphics &context,
                               int width,
                               int height,
                               bool rowIsSelected) override;

        /*--------------------*/

        /**
         * Handle change of selection
         */
        void selectedRowsChanged (int lastRowSelected) override;

        /*--------------------*/
        
        /**
         * Sets data of list box model to <C>list</C>.
         *
         * @param[in] list  list of strings to be displayed
         */
        void setData (IN StringList& list);

        /*--------------------*/
        
        /**
         * Sets marked item to <C>item</C>
         *
         * @param[in] item  marked item
         */
        void setMarkedItem (IN Natural item);

        /*--------------------*/

        private:

            /** the name of this list */
            String _name;

            /** the parent component */
            _PresetSelectionComponent* _parent;
        
            /** the string list to be displayed */
            StringList _list;

            /** number of marked item */
            Natural _markedItem;

    };

    /*====================*/
    
    /**
     * A selection component for soundfont presets
     */
    struct _PresetSelectionComponent
        : juce::Component, juce::Button::Listener {

        /**
         * Initializes a preset selection component with presets
         * given by <C>presetList</C>
         *
         * @param[in]     presetList            list of programs with
         *                                      entries consisting of
         *                                      bank, program and name
         *                                      separated by tabulators
         * @param[in]     presetIdentification  bank and program (when
         *                                      selected)
         * @param[inout]  processor             the underlying event
         *                                      processor
         */
        _PresetSelectionComponent
            (IN StringList& presetList,
             IN MidiPresetIdentification& presetIdentification,
             INOUT FluidSynthPlugin_EventProcessor& processor);

        /*--------------------*/

        /**
         * Returns selected presetIdentification with bank and program
         *
         * @return bank and program
         */
        MidiPresetIdentification presetIdentification () const;

        /*--------------------*/

        /**
         * Confirms selection in component
         */
        void confirm ();
        
        /*--------------------*/

        /**
         * Handles click event of button.
         *
         * @param button  button clicked
         */
        void buttonClicked (juce::Button* button) override;
        
        /*--------------------*/

        /**
         * Handles list box selection in some child list box.
         *
         * @param[in} listBoxName    name of list box
         * @param[in} selectedIndex  index of selected item in list
         */
        void handleListBoxSelection (IN String& listBoxName,
                                     IN Natural selectedIndex);

        /*--------------------*/

        /**
         * Tells whether program column is displaying the bank with
         * the selected preset.
         *
         * @return  information whether marked bank is currently
         *          displayed
         */
        Boolean isDisplayingMarkedBank () const;

        /*--------------------*/

        /**
         * Returns the font to be used for list boxes
         *
         * @return list box font
         */
        juce::Font listBoxFont () const;

        /*--------------------*/

        /**
         * Handles resizing of the component
         */
        void resized () override;
        
        /*--------------------*/

        private:

            /** the underlying event processor */
            FluidSynthPlugin_EventProcessor& _processor;

            /** the bank and program selected */
            MidiPresetIdentification _presetIdentification;

            /** the list of bank numbers */
            NaturalList _bankNumberList;

            /** the mapping from bank numbers to associated programs */
            NaturalToStringListMap _bankNumberToProgramListMap;

            /** the list of bank numbers */
            _StringListBoxModel _bankNumberListModel;

            /** the list of programs */
            _StringListBoxModel _programListModel;

            /** the label for the bank column */
            juce::Label _bankColumnHeadingWidget;

            /** the label for the program column */
            juce::Label _programColumnHeadingWidget;

            /** the bank column with preset bank numbers */
            juce::ListBox _bankListBoxWidget;

            /** the program column with preset program numbers and
             * texts */
            juce::ListBox _programListBoxWidget;

            /** the font to be used for list boxes */
            juce::Font _listBoxFont;

            /** the okay button */
            juce::TextButton _okayButtonWidget;

            /** the cancel button */
            juce::TextButton _cancelButtonWidget;
    };

}

/*====================*/

using Main::FluidSynthPlugin::_PresetSelectionComponent;
using Main::FluidSynthPlugin::_StringListBoxModel;

/*====================*/
/* LOCAL FEATURES     */
/*====================*/

/**
 * Ensures that entry in <C>listBoxModel</C> which is either identical
 * to marked number of that model or is maximum one less than that
 * number is visible and also selects the row of the marked number
 * (when visible)
 *
 * @param[inout] listBoxWidget  widget of list box
 * @param[in]    listBoxModel   model for list box
 */

static void
_highlightMarkedRowInListBox (INOUT juce::ListBox& listBoxWidget,
                              IN _StringListBoxModel& listBoxModel)
{
    Logging_trace(">>");

    const StringList& dataList = listBoxModel.list();
    Natural markedItem = listBoxModel.markedItem();

    Logging_trace2("--: markedItem = %1, dataList = %2",
                   TOSTRING(markedItem), dataList.toString());

    /* find row with marked item */
    Boolean markedItemIsFound = false;
    Natural row = 0;

    for (Natural i = dataList.length();  i-- > 0;) {
        const String& st = dataList.at(i);
        Natural item = _splitOffLeadingNumber(st);

        if (item <= markedItem) {
            markedItemIsFound = (item == markedItem);
            row = i;
            break;
        }
    }

    listBoxWidget.scrollToEnsureRowIsOnscreen((int) row);

    if (markedItemIsFound
        && listBoxModel.name() == _Identification_bank) {
        listBoxWidget.selectRow((int) row);
    }

    Logging_trace("<<");
}

/*--------------------*/

/**
 * Defines layout for button given by <C>buttonWidget</C>.
 *
 * @param[inout] buttonWidget  button widget to be formatted
 */
static void _setLayoutForButton (INOUT juce::TextButton& buttonWidget)
{
    String buttonName = buttonWidget.getButtonText().toStdString();
    Logging_trace1(">>: %1", buttonName);

    buttonWidget.setColour(colourId2(TextButton, button), _Colour_button);
    buttonWidget.setColour(colourId3(TextButton, text, Off), _Colour_text);

    Logging_trace("<<");
}

/*--------------------*/

/**
 * Defines layout for label given by <C>labelWidget</C>.
 *
 * @param[inout] labelWidget  label widget to be formatted
 */
static void _setLayoutForHeading (INOUT juce::Label& labelWidget)
{
    String labelName = labelWidget.getText().toStdString();
    Logging_trace1(">>: %1", labelName);

    juce::Justification hCenterAligned =
        juce::Justification::horizontallyCentred;
    labelWidget.setJustificationType(hCenterAligned);
    labelWidget.setColour(colourId2(Label, text), _Colour_text);

    Logging_trace("<<");
}

/*--------------------*/

/**
 * Defines layout for list box given by <C>listBoxWidget</C>.
 *
 * @param[inout] listBoxWidget  list box widget to be formatted
 */
static void _setLayoutForListBox (INOUT juce::ListBox& listBoxWidget)
{
    String listBoxName = listBoxWidget.getName().toStdString();
    Logging_trace1(">>: %1", listBoxName);

    listBoxWidget.setColour(colourId2(ListBox, background),
                            _Colour_listBoxBackground);
    listBoxWidget.setColour(colourId2(ListBox, outline),
                            _Colour_listBoxOutline);

    Logging_trace("<<");
}

/*--------------------*/

/**
 * Returns leading bank or program number in a string <C>st</C>.
 *
 * @param[in] st  string with bank or program number
 * @return  number extracted from string
 */
static Natural _splitOffLeadingNumber (IN String& st)
{
    Natural result;

    if (st.length() < _listBoxNumberDigitCount) {
        result = 0;
    } else {
        String prefix = STR::prefix(st, _listBoxNumberDigitCount);
        result = STR::toNatural(prefix);
    }

    return result;
}

/*--------------------*/

/**
 * Splits <C>presetList</C> into list of bank numbers
 * <C>bankNumberList</C> and map from bank number to lists of combined
 * program number and program name in
 * <C>bankNumberToProgramListMap</C>.
 *
 * @param[in]  presetList                  list of tab separated bank
 *                                         number, program number and
 *                                         program name
 * @param[out] bankNumberList              list of bank numbers
 * @param[out] bankNumberToProgramListMap  map from bank numbers to
 *                                         lists of program numbers
 *                                         and program names
 */
static void
_splitPresetList (IN StringList& presetList,
                  OUT NaturalList& bankNumberList,
                  OUT NaturalToStringListMap& bankNumberToProgramListMap)
{
    Logging_trace1(">>: %1", presetList.toString());

    const String columnSeparator = "\t";
    NaturalSet bankNumberSet;
    bankNumberToProgramListMap.clear();

    for (String presetString : presetList) {
        StringList partList =
            StringList::makeBySplit(presetString, columnSeparator);

        if (partList.length() != 3) {
            Logging_traceError1("cannot split %1 into three parts",
                                presetString);
        } else {
            Natural bankNumber    = STR::toNatural(partList.at(0));
            Natural programNumber = STR::toNatural(partList.at(1));
            String st =
                STR::expand("%1: %2",
                            STR::toString(programNumber,
                                          _listBoxNumberDigitCount),
                            partList.at(2));

            bankNumberSet.add(bankNumber);

            if (!bankNumberToProgramListMap.contains(bankNumber)) {
                bankNumberToProgramListMap.set(bankNumber, StringList());
            }

            StringList programDataList =
                bankNumberToProgramListMap.at(bankNumber);
            programDataList.append(st);
            bankNumberToProgramListMap.set(bankNumber,
                                           programDataList);
        }
    }
    
    /* collect the bank numbers */
    bankNumberList.clear();
    
    for (Natural bankNumber : bankNumberSet) {
        bankNumberList.append(bankNumber);
    }
    
    Logging_trace2("<<: bankNumberList = %1,"
                   " bankNumberToProgListMap = %2",
                   bankNumberList.toString(),
                   bankNumberToProgramListMap.toString());
}

/*--------------------*/

/**
 * Returns device height for <C>component</C> from percentage
 * <C>height</C>
 *
 * @param[in] component  the enclosing component
 * @param[in] height     the height as percentage of the enclosing
 *                       component
 * @return  height (in device coordinates)
 */
static int _toDeviceHeight (IN juce::Component* component,
                            IN Percentage height)
{
    juce::Rectangle<int> rectangle = component->getLocalBounds();
    Natural rectangleHeight{(size_t) rectangle.getHeight()};
    const int result = (double) height.of(rectangleHeight);
    return result;
}

/*--------------------*/

/**
 * Constructs a new rectangle as a subarea of <C>component</C> from
 * percentages <C>(startX, startY)</C> with size <C>(width,
 * height)</C>
 *
 * @param[in] component  the enclosing component
 * @param[in] startX     the left x-position as percentage from the
 *                       left of the enclosing rectangle
 * @param[in] startY     the top y-position as percentage from the top
 *                       of the enclosing rectangle
 * @param[in] width      the width as percentage of the enclosing
 *                       rectangle
 * @param[in] height     the height as percentage of the enclosing
 *                       rectangle
 * @return  rectangle (in device coordinates)
 */
static
juce::Rectangle<int> _toDeviceRectangle (IN juce::Component* component,
                                         IN Percentage startX,
                                         IN Percentage startY,
                                         IN Percentage width,
                                         IN Percentage height)
{
    juce::Rectangle<int> rectangle = component->getLocalBounds();
    Natural rectangleHeight{(size_t) rectangle.getHeight()};
    Natural rectangleWidth{(size_t) rectangle.getWidth()};
    const int deviceX = (double) startX.of(rectangleWidth);
    const int deviceY = (double) startY.of(rectangleHeight);
    const int deviceWidth = (double) width.of(rectangleWidth);
    const int deviceHeight = (double) height.of(rectangleHeight);

    juce::Rectangle<int> result{deviceX, deviceY,
                                deviceWidth, deviceHeight};
    return result;
}

/*===========================*/
/* _PresetSelectionComponent */
/*===========================*/

_PresetSelectionComponent
::_PresetSelectionComponent
      (IN StringList& presetList,
       IN MidiPresetIdentification& presetIdentification,
       INOUT FluidSynthPlugin_EventProcessor& processor)
    : _processor{processor},
      _presetIdentification{presetIdentification},
      _bankNumberList{},
      _bankNumberToProgramListMap{},
      _bankNumberListModel{this, _Identification_bank},
      _programListModel{this, _Identification_program},
      _bankColumnHeadingWidget{"",
                             juce::String{_Identification_bank}},
      _programColumnHeadingWidget{"",
                                juce::String{_Identification_program}},
      _bankListBoxWidget{"", &_bankNumberListModel},
      _programListBoxWidget{"", &_programListModel},
      _listBoxFont{JuceFont_make(10, juce::Font::plain)},
      _okayButtonWidget{_ButtonLabel_okay},
      _cancelButtonWidget{_ButtonLabel_cancel}
{
    Logging_trace2(">>: presetIdentification = %1, presetList = %2",
                   presetIdentification.toString(),
                   presetList.toString());

    /* update the data of the list box models */
    _splitPresetList(presetList,
                     _bankNumberList, _bankNumberToProgramListMap);

    Boolean someBanksExist = (_bankNumberList.length() > 0);
    StringList bankNumberStringList;

    for (Natural b : _bankNumberList) {
        bankNumberStringList.append(STR::toString(b,
                                                  _listBoxNumberDigitCount));
    }

    Natural bankNumber;
    Natural programNumber;
    Boolean isOkay = presetIdentification.split(bankNumber, programNumber);
    Logging_trace4("--: isOkay = %1, presetId = '%2',"
                   " bankNumber = %3, programNumber = %4",
                   TOSTRING(isOkay), presetIdentification.toString(),
                   TOSTRING(bankNumber), TOSTRING(programNumber));

    _bankNumberListModel.setData(bankNumberStringList);
    _bankNumberListModel.setMarkedItem(bankNumber);

    StringList programDataList;
    
    if (!_bankNumberList.contains(bankNumber)) {
        /* use minimum number for initial selection */
        bankNumber = (someBanksExist
                      ? _bankNumberList.at(0)
                      : 0);
    }

    if (_bankNumberList.contains(bankNumber)) {
        programDataList = _bankNumberToProgramListMap.at(bankNumber);
    }

    _programListModel.setData(programDataList);
    _programListModel.setMarkedItem(programNumber);

    /* add widgets */
    addAndMakeVisible(_bankColumnHeadingWidget);
    addAndMakeVisible(_programColumnHeadingWidget);
    addAndMakeVisible(_bankListBoxWidget);
    addAndMakeVisible(_programListBoxWidget);
    addAndMakeVisible(_okayButtonWidget);
    addAndMakeVisible(_cancelButtonWidget);
    
    /* set widget characteristics */
    _setLayoutForButton(_cancelButtonWidget);
    _setLayoutForButton(_okayButtonWidget);
    _setLayoutForListBox(_bankListBoxWidget);
    _setLayoutForListBox(_programListBoxWidget);
    _setLayoutForHeading(_bankColumnHeadingWidget);
    _setLayoutForHeading(_programColumnHeadingWidget);
        
    /* connect buttons to current component as button handler */
    _cancelButtonWidget.addListener(this);
    _okayButtonWidget.addListener(this);

    setSize(400, 400);

    Logging_trace("<<");
}

/*--------------------*/

MidiPresetIdentification
_PresetSelectionComponent::presetIdentification () const
{
    return _presetIdentification;
}

/*--------------------*/

void _PresetSelectionComponent::buttonClicked (juce::Button* button)
{
    String buttonName = button->getButtonText().toStdString();
    Logging_trace1(">>: %1", buttonName);

    Boolean isCancelled = (buttonName == _ButtonLabel_cancel);

    if (isCancelled) {
        /* clear bank and program to indicate cancellation */
        _presetIdentification.clear();
    }

    juce::DialogWindow* dialog{(juce::DialogWindow*) getParentComponent()};
    dialog->exitModalState((int) isCancelled);

    Logging_trace("<<");
}

/*--------------------*/

void _PresetSelectionComponent::confirm ()
{
    Logging_trace(">>");
    buttonClicked(&_okayButtonWidget);
    Logging_trace("<<");
}
        
/*--------------------*/

void
_PresetSelectionComponent::handleListBoxSelection (IN String& listBoxName,
                                                   IN Natural selectedIndex)
{
    Logging_trace2(">>: listBox = '%1', index = %2",
                   listBoxName, TOSTRING(selectedIndex));

    Natural markedBankNumber    = _bankNumberListModel.markedItem();
    Natural markedProgramNumber = _programListModel.markedItem();
    Boolean isOkay = true;

    if (listBoxName == _Identification_bank) {
        /* a new bank: change the associated list of programs */
        Natural bankNumber = _bankNumberList.at(selectedIndex);
        StringList programDataList =
            _bankNumberToProgramListMap.at(bankNumber);
        _programListModel.setData(programDataList);
        _programListBoxWidget.updateContent();
    } else if (listBoxName == _Identification_program) {
        /* a new program selection: change marked program and also
         * marked bank*/
        int bankRow = _bankListBoxWidget.getSelectedRow();
        bankRow = (bankRow < 0 ? 0 : bankRow);
        markedBankNumber = _bankNumberList.at((size_t) bankRow);
        _bankNumberListModel.setMarkedItem(markedBankNumber);
        String st = _programListModel.list().at(selectedIndex);
        markedProgramNumber = _splitOffLeadingNumber(st);
        _presetIdentification =
            MidiPresetIdentification{markedBankNumber, markedProgramNumber};
        _bankListBoxWidget.repaint();
    } else {
        Logging_traceError1("unknown list box name - '%1'", listBoxName);
        isOkay = false;
    }

    if (isOkay) {
        _programListModel.setMarkedItem(markedProgramNumber);
        _highlightMarkedRowInListBox(_programListBoxWidget,
                                     _programListModel);
        _processor.setPreset(_presetIdentification);
        _programListBoxWidget.deselectAllRows();
        _programListBoxWidget.repaint();
    }
    
    Logging_trace("<<");
}

/*--------------------*/

Boolean _PresetSelectionComponent::isDisplayingMarkedBank () const
{
    int bankRow = _bankListBoxWidget.getSelectedRow();
    bankRow = (bankRow < 0 ? 0 : bankRow);
    Natural selectedBankNumber = _bankNumberList.at(bankRow);
    Natural markedBankNumber   = _bankNumberListModel.markedItem();
    return markedBankNumber == selectedBankNumber;
}

/*--------------------*/

juce::Font _PresetSelectionComponent::listBoxFont () const
{
    return _listBoxFont;
}

/*--------------------*/

void _PresetSelectionComponent::resized ()
{
    Logging_trace(">>");

    const Percentage hundredPercent = 100.0;
    /* vertical dimensions */
    Percentage spaceY = 2.0;
    Percentage buttonHeight = 8.0;
    Percentage buttonTopY = 90.0;
    Percentage headerTopY = spaceY;
    Percentage listBoxRowHeight = 6.0;
    Percentage listBoxTopY = 10.0;

    /* horizontal dimensions */
    Percentage buttonWidth = 30.0;
    Percentage buttonGap = (hundredPercent - buttonWidth * 2.0) / 5.0;
    Percentage listBoxBorderMargin = 3.0;
    Percentage listBoxGap = 3.0;

    /* derived settings */
    Percentage bankNumberBoxRelativeSize = 20.0;
    Percentage totalListBoxWidth =
        hundredPercent - listBoxGap - listBoxBorderMargin * 2.0;
    Percentage programNumberBoxWidth =
        (hundredPercent - bankNumberBoxRelativeSize).of(totalListBoxWidth);
    Percentage bankNumberBoxWidth =
        bankNumberBoxRelativeSize.of(totalListBoxWidth);
    Percentage headerHeight = listBoxTopY - headerTopY - spaceY;
    Percentage listBoxHeight = buttonTopY - listBoxTopY - spaceY;
    
    int deviceHeaderHeight = _toDeviceHeight(this, headerHeight);
    int deviceRowHeight = _toDeviceHeight(this, listBoxRowHeight);
    
    juce::Font headerFont =
        JuceFont_make(deviceHeaderHeight, juce::Font::plain);
    _listBoxFont =
        JuceFont_make(deviceRowHeight * 0.75, juce::Font::plain);

    Percentage x = listBoxBorderMargin;
    _bankColumnHeadingWidget.setBounds(
        _toDeviceRectangle(this,
                           x, headerTopY,
                           bankNumberBoxWidth, headerHeight));
    _bankColumnHeadingWidget.setFont(headerFont);

    x += bankNumberBoxWidth + listBoxGap;
    _programColumnHeadingWidget.setBounds(
        _toDeviceRectangle(this,
                           x, headerTopY,
                           programNumberBoxWidth, headerHeight));
    _programColumnHeadingWidget.setFont(headerFont);

    x = listBoxBorderMargin;
    _bankListBoxWidget.setBounds(
        _toDeviceRectangle(this,
                           x, listBoxTopY,
                           bankNumberBoxWidth, listBoxHeight));
    _bankListBoxWidget.setRowHeight(deviceRowHeight);

    x += bankNumberBoxWidth + listBoxGap;
    _programListBoxWidget.setBounds(
        _toDeviceRectangle(this,
                           x, listBoxTopY,
                           programNumberBoxWidth, listBoxHeight));
    _programListBoxWidget.setRowHeight(deviceRowHeight);

    x = buttonGap * 2.0;
    _okayButtonWidget.setBounds(
        _toDeviceRectangle(this,
                           x, buttonTopY,
                           buttonWidth, buttonHeight));

    x += buttonWidth + buttonGap;
    _cancelButtonWidget.setBounds(
        _toDeviceRectangle(this,
                           x, buttonTopY,
                           buttonWidth, buttonHeight));


    /* ensure that highlighting is done correctly when nothing is so
       far selected */
    if (_bankListBoxWidget.getSelectedRow() < 0) {
        _highlightMarkedRowInListBox(_bankListBoxWidget,
                                     _bankNumberListModel);
        _highlightMarkedRowInListBox(_programListBoxWidget,
                                     _programListModel);
    }
    
    Logging_trace("<<");
}

/*=====================*/
/* _StringListBoxModel */
/*=====================*/

_StringListBoxModel::_StringListBoxModel
                         (IN _PresetSelectionComponent* component,
                          IN String& name)
    : _name{name},
      _parent{(_PresetSelectionComponent*) component}
{
}

/*--------------------*/

int _StringListBoxModel::getNumRows ()
{
    return (int) _list.length();
}

/*--------------------*/

const StringList& _StringListBoxModel::list () const
{
    return _list;
}

/*--------------------*/

void _StringListBoxModel::listBoxItemDoubleClicked
                              (int row,
                               const juce::MouseEvent& mouseEvent)
{
    setMarkedItem(row);
    _parent->confirm();
}

/*--------------------*/

Natural _StringListBoxModel::markedItem () const
{
    return _markedItem;
}

/*--------------------*/

String _StringListBoxModel::name () const
{
    return _name;
}

/*--------------------*/

void _StringListBoxModel::paintListBoxItem (int rowNumber,
                                            juce::Graphics &context,
                                            int width,
                                            int height,
                                            bool rowIsSelected)
{
    Boolean rowIsMarked;
    Boolean isProgramListBox = (_name == _Identification_program);
    Boolean markedBankIsDisplayed = (isProgramListBox
                                     && _parent->isDisplayingMarkedBank());
    String st;
    
    if (rowNumber >= _list.length()) {
        rowIsMarked = false;
        st = "";
    } else {
        st = _list.at(rowNumber);
        Natural number = _splitOffLeadingNumber(st);
        rowIsMarked = (number == _markedItem);

        if (isProgramListBox) {
            /* do not mark any entries in other banks */
            rowIsMarked = rowIsMarked && markedBankIsDisplayed;

            if (rowIsMarked) {
                /* the marked program is also selected (to be more
                   visible) */
                rowIsSelected = true;
            }
        }
    }

    context.setFont(_parent->listBoxFont());
    juce::Justification leftAligned = juce::Justification::left;
    juce::String selectionMarker = ">>";
    juce::Colour textColour;
    juce::Colour backgroundColour;
    juce::Font font = context.getCurrentFont();
    int xOffset = JuceFont_getStringWidth(font, selectionMarker + " ");

    if (rowIsSelected) {
        textColour = _Colour_lboxItemTxtSelected;
        backgroundColour = _Colour_lboxItemBkgSelected;
    } else {
        textColour = _Colour_lboxItemTxtUnselected;
        backgroundColour = _Colour_lboxItemBkgUnselected;
    }

    /* draw the background */
    context.setColour(backgroundColour);
    context.fillRect(0, 0, width, height);

    /* draw text */
    context.setColour(textColour);

    if (rowIsMarked) {
        context.drawText(selectionMarker, 0, 0, xOffset, height,
                         leftAligned);
    }

    context.drawText(juce::String{st}, xOffset, 0, width, height,
                     leftAligned);
}

/*--------------------*/

void _StringListBoxModel::selectedRowsChanged (int lastRowSelected)
{
    if (lastRowSelected >= 0) {
        _parent->handleListBoxSelection(_name,
                                        Natural{lastRowSelected});
    }
}

/*--------------------*/

void _StringListBoxModel::setData (IN StringList& list)
{
    _list = list;
}

/*--------------------*/

void _StringListBoxModel::setMarkedItem (IN Natural item)
{
    _markedItem = item;
}

/*====================*/
/* EXPORTED FEATURES  */
/*====================*/

Boolean FluidSynthPlugin_EditorSupport::selectFileByDialog
            (IN String& startDirectory,
             OUT String& fileName)
{
    Logging_trace1(">>: %1", startDirectory);
    Boolean isOkay;

    juce::File startPosition{startDirectory};
    juce::FileChooser dialog{_Title_fileSelection,
                             startPosition,
                             "*.sf2;*.sf3"};
    isOkay = dialog.browseForFileToOpen();

    if (isOkay) {
        juce::File selectedFile = dialog.getResult();
        fileName = selectedFile.getFullPathName().toStdString();
    }
    
    Logging_trace2("<<: result = %1, fileName = %2",
                   TOSTRING(isOkay), fileName);
    return isOkay;
}

/*--------------------*/

Boolean FluidSynthPlugin_EditorSupport::selectPresetByDialog
            (IN StringList& presetList,
             INOUT MidiPresetIdentification& presetIdentification,
             INOUT FluidSynthPlugin_EventProcessor& eventProcessor)
{
    Logging_trace2(">>: presetIdentification = %1, presetList = %2",
                   presetIdentification.toString(),
                   presetList.toString());

    /* store previous preset value */
    MidiPresetIdentification previousPresetIdentification =
        eventProcessor.preset();
    presetIdentification =
        (presetIdentification.isEmpty()
         ? previousPresetIdentification
         : presetIdentification);

    _PresetSelectionComponent presetSelector{presetList,
                                             presetIdentification,
                                             eventProcessor};

    juce::DialogWindow::showModalDialog(_Title_presetSelection,
                                        &presetSelector,
                                        NULL,
                                        _Colour_presetSelectionComponentBkg,
                                        false, true, true);
    
    presetIdentification = presetSelector.presetIdentification();
    Boolean result = !presetIdentification.isEmpty();

    if (!result) {
        eventProcessor.setPreset(previousPresetIdentification);
    }

    Logging_trace2("<<: result = %1, presetIdentification = %2",
                   TOSTRING(result), presetIdentification.toString());
    return result;
}

/*--------------------*/

void FluidSynthPlugin_EditorSupport::showInformationDialog
         (IN String& fluidSynthLibraryVersion)
{
    Logging_trace1(">>: %1", fluidSynthLibraryVersion);

    String buildTimestamp =
        (BuildInformation::date()
         + (BuildInformation::isDebugBuild() ?
            " " + BuildInformation::time()
            : ""));
    
    String messageText =
        STR::expand("FluidSynthPlugin v%1\n"
                    "by Dr. Thomas Tensi\n\n"
                    "build: %2\n"
                    "FluidSynthVersion: %3",
                    BuildInformation::version(),
                    buildTimestamp,
                    fluidSynthLibraryVersion);

    Logging_trace1("--: messageText = %1", messageText);

    String title = "About FluidSynthPlugin";
    juce::MessageBoxIconType messageBoxKind =
        juce::MessageBoxIconType::InfoIcon;
   
    juce::NativeMessageBox::showMessageBox(messageBoxKind,
                                           title, messageText);
                                           
    Logging_trace("<<");
}
