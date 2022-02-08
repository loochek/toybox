#ifndef VISUAL_SORT_WINDOW_CONTROLLER_HPP
#define VISUAL_SORT_WINDOW_CONTROLLER_HPP

#include <vector>
#include <random>
#include <cassert>
#include <thread>
#include <mutex>
#include <chrono>
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
    VisualSortWindowController(VisualSortWindow *window) : mWindow(window), mElemCnt(DEFAULT_ELEM_CNT),
        mReady(false), mCancelPending(false), mRunning(false), mHighlIdx1(-1), mHighlIdx2(-1) {};

    virtual void onClick(uint64_t userData) override
    {
        assert(1 <= userData && userData < SORT_ALGO_MAX);

        if (mRunning)
        {
            mCancelPending = true;
            while (mRunning);
        }

        mReady = false;
        mMutex.unlock();
        mMutex.lock();

        std::thread sortThread(&VisualSortWindowController::sortStarter, std::ref(*this),
                               (SortAlgorithm)userData, mElemCnt);
        sortThread.detach();
    }

    virtual void onTextChange(const char *newText, uint64_t userData) override
    {
        mElemCnt = atoi(newText);
    }

    void update()
    {
        if (mReady)
        {
            mWindow->mVisualSort->redraw(mArray, mHighlIdx1, mHighlIdx2);
            mReady = false;
            mMutex.unlock();
        }
    }

private:
    void sortStarter(SortAlgorithm algo, int elemCnt)
    {
        mRunning = true;

        try
        {
            sortRoutine(algo, elemCnt);
        }
        catch (SortInterruptException) {}

        mCancelPending = false;
        mRunning = false;
    }

    void sortRoutine(SortAlgorithm algo, int elemCnt)
    {
        VisualSortData data = { mArray.data(), elemCnt, mMutex, mReady, mCancelPending, mHighlIdx1, mHighlIdx2 };

        mArray.clear();
        mArray.reserve(elemCnt);
        for (int i = 0; i < elemCnt; i++)
            mArray.push_back(VisualSortObject(i + 1, i, data));

        std::shuffle(mArray.begin(), mArray.end(), std::default_random_engine());

        switch (algo)
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

        mHighlIdx1 = -1, mHighlIdx2 = -1;
        mReady = true;
        mMutex.lock();
    }

protected:
    VisualSortWindow *mWindow;

    std::vector<VisualSortObject> mArray;
    int mElemCnt;
    int mHighlIdx1, mHighlIdx2;

    std::mutex mMutex;
    bool mReady, mCancelPending, mRunning;
};

#endif