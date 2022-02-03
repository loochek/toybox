#include "AnalysisWindow.hpp"
#include "ChartWidget.hpp"
#include "../BaseGUI/Button.hpp"
#include "../BaseGUI/Label.hpp"
#include "../BaseGUI/TextBox.hpp"
#include "../SortingLogic/AnalysisWindow/AnalysisWindowController.hpp"

const Vec2i WINDOW_SIZE(630, 320);
const Vec2i COMP_CHART_POS(18, 50);
const Vec2i ASSN_CHART_POS(328, 50);
const Vec2i CHART_SIZE(300, 200);

const Vec2i COMP_LABEL_POS(18, 40);
const Vec2i ASSN_LABEL_POS(328, 40);

const Vec2i STD_SORT_BUTTON_POS(14, 260);
const Vec2i STABLE_SORT_BUTTON_POS(140, 260);
const Vec2i BUBBLE_SORT_BUTTON_POS(266, 260);
const Vec2i SELECTION_SORT_BUTTON_POS(392, 260);
const Vec2i QUICK_SORT_BUTTON_POS(518, 260);
const Vec2i BUTTON_SIZE(116, 50);

const Vec2i TEXT_BOX_LABEL_POS(18, 330);
const Vec2i TEXT_BOX_POS(130, 320);
const int TEXT_BOX_SIZE = 100;

AnalysisWindow::AnalysisWindow(const Vec2i &windowPos, Widget *parent) :
    Window(IntRect(windowPos, WINDOW_SIZE), parent)
{
    setTitle("Sorting analysis");
    mController = new AnalysisWindowController(this);

    mCompChart = new ChartWidget(IntRect(COMP_CHART_POS, CHART_SIZE), this);
    addChild(mCompChart);

    mAssnChart = new ChartWidget(IntRect(ASSN_CHART_POS, CHART_SIZE), this);
    addChild(mAssnChart);

    Label *compLabel = new Label(COMP_LABEL_POS, this);
    compLabel->setText("Comparisons:");
    addChild(compLabel);

    Label *assnLabel = new Label(ASSN_LABEL_POS, this);
    assnLabel->setText("Assignments:");
    addChild(assnLabel);

    Button *stdSortButton = new Button(IntRect(STD_SORT_BUTTON_POS, BUTTON_SIZE), this,
                                       CHARTS_COLORS[SORT_STD_SORT - 1]);
    stdSortButton->setLabel("std::sort");
    stdSortButton->setDelegate(mController);
    stdSortButton->setUserData(SORT_STD_SORT);
    addChild(stdSortButton);

    Button *stableSortButton = new Button(IntRect(STABLE_SORT_BUTTON_POS, BUTTON_SIZE), this,
                                          CHARTS_COLORS[SORT_STD_STABLE_SORT - 1]);
    stableSortButton->setLabel("std::stable_sort");
    stableSortButton->setDelegate(mController);
    stableSortButton->setUserData(SORT_STD_STABLE_SORT);
    addChild(stableSortButton);

    Button *bubbleSortButton = new Button(IntRect(BUBBLE_SORT_BUTTON_POS, BUTTON_SIZE), this,
                                          CHARTS_COLORS[SORT_BUBBLE - 1]);
    bubbleSortButton->setLabel("Bubble sort");
    bubbleSortButton->setDelegate(mController);
    bubbleSortButton->setUserData(SORT_BUBBLE);
    addChild(bubbleSortButton);

    Button *selectionSortButton = new Button(IntRect(SELECTION_SORT_BUTTON_POS, BUTTON_SIZE), this,
                                             CHARTS_COLORS[SORT_SELECTION - 1]);
    selectionSortButton->setLabel("Selection sort");
    selectionSortButton->setDelegate(mController);
    selectionSortButton->setUserData(SORT_SELECTION);
    addChild(selectionSortButton);

    Button *quickSortButton = new Button(IntRect(QUICK_SORT_BUTTON_POS, BUTTON_SIZE), this,
                                             CHARTS_COLORS[SORT_QUICK - 1]);
    quickSortButton->setLabel("Quick sort");
    quickSortButton->setDelegate(mController);
    quickSortButton->setUserData(SORT_QUICK);
    addChild(quickSortButton);

    Label *textBoxLabel = new Label(TEXT_BOX_LABEL_POS, this);
    textBoxLabel->setText("Max array size:");
    addChild(textBoxLabel);

    TextBox *textBox = new TextBox(TEXT_BOX_POS, TEXT_BOX_SIZE, this);
    textBox->setDelegate(mController);

    char newText[MAX_TEXT_BOX_LEN + 1] = {0};
    snprintf(newText, MAX_TEXT_BOX_LEN, "%d", DEFAULT_MAX_ELEM_CNT);
    textBox->setText(newText);

    addChild(textBox);
}

AnalysisWindow::~AnalysisWindow()
{
    delete mController;
}