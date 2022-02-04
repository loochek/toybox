#ifndef VISUAL_SORT_WINDOW_CONTROLLER_HPP
#define VISUAL_SORT_WINDOW_CONTROLLER_HPP

#include <vector>
#include <random>
#include <cassert>
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../../BaseGUILogic/TextBox/TextBoxDelegate.hpp"
#include "../../SortingWidgets/VisualSortWindow.hpp"
#include "../../SortingWidgets/VisualSortWidget.hpp"
#include "../../Sorting/VisualSortObject.hpp"
#include "../../Sorting/SortAlgorithms.hpp"

const int DEFAULT_ELEM_CNT = 50;

class VisualSortWindowController : public ButtonDelegate, public TextBoxDelegate
{
public:
    VisualSortWindowController() = delete;
    VisualSortWindowController(VisualSortWindow *window, App *app) :
        mWindow(window), mApp(app), mElemCnt(DEFAULT_ELEM_CNT) {};

    virtual void onClick(uint64_t userData) override
    {
        assert(1 <= userData && userData < SORT_ALGO_MAX);
        int algoIdx = userData - 1;

        mWindow->mVisualSort->setArray(&mArray);

        mArray.clear();
        for (int i = 0; i < mElemCnt; i++)
            mArray.push_back(VisualSortObject(i + 1, i, mWindow->mVisualSort, mArray.data(), mElemCnt, mApp));

        std::shuffle(mArray.begin(), mArray.end(), std::default_random_engine());

        switch (userData)
        {
        case SORT_STD_SORT:
            std::sort(mArray.begin(), mArray.end());
            break;

        case SORT_STD_STABLE_SORT:
            std::stable_sort(mArray.begin(), mArray.end());
            break;

        case SORT_BUBBLE:
            bubbleSort(mArray.data(), mArray.size());
            break;

        case SORT_SELECTION:
            selectionSort(mArray.data(), mArray.size());
            break;

        case SORT_QUICK:
            quickSort(mArray.data(), mArray.size());
            break;
        }

        mWindow->mVisualSort->highlight(-1, -1);
    }

    virtual void onTextChange(const char *newText, uint64_t userData) override
    {
        mElemCnt = atoi(newText);
    }

protected:
    VisualSortWindow *mWindow;
    App *mApp;

    std::vector<VisualSortObject> mArray;
    int mElemCnt;
};

#endif