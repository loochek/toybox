#ifndef ANALYSIS_WINDOW_CONTROLLER_HPP
#define ANALYSIS_WINDOW_CONTROLLER_HPP

#include <vector>
#include <random>
#include <cassert>
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../../SortingWidgets/AnalysisWindow.hpp"
#include "../../SortingWidgets/ChartWidget.hpp"
#include "../../Sorting/SortCountObject.hpp"
#include "../../Sorting/SortAlgorithms.hpp"
#include "../../Utils/Chart.hpp"

const int MAX_ELEM_CNT = 1000;

enum SortAlgorithm
{
    SORT_STD_SORT = 1,
    SORT_STD_STABLE_SORT,
    SORT_BUBBLE,
    SORT_SELECTION,
    SORT_ALGO_MAX
};

const LGL::Color CHARTS_COLORS[SORT_ALGO_MAX] =
{
    LGL::Color(1.0f, 0.0f, 0.0f),
    LGL::Color(0.0f, 1.0f, 0.0f),
    LGL::Color(0.0f, 0.0f, 1.0f),
    LGL::Color(0.0f, 1.0f, 1.0f)
};

class AnalysisWindowController : public ButtonDelegate
{
public:
    AnalysisWindowController() = delete;
    AnalysisWindowController(AnalysisWindow *window) : mWindow(window) {};

    virtual void onClick(uint64_t userData) override
    {
        assert(1 <= userData && userData < SORT_ALGO_MAX);
        int algoIdx = userData - 1;

        mCompCharts[algoIdx].clear();
        mAssnCharts[algoIdx].clear();

        for (int elemCnt = 0; elemCnt < MAX_ELEM_CNT; elemCnt++)
        {
            int cmpCounter = 0, assnCounter = 0;

            std::vector<SortCountObject> arr;
            for (int i = 0; i < elemCnt; i++)
                arr.push_back(SortCountObject(i, cmpCounter, assnCounter));

            std::mt19937 rng;
            std::shuffle(arr.begin(), arr.end(), rng);

            cmpCounter = 0;
            assnCounter = 0;

            switch (userData)
            {
            case SORT_STD_SORT:
                std::sort(arr.begin(), arr.end());
                break;

            case SORT_STD_STABLE_SORT:
                std::stable_sort(arr.begin(), arr.end());
                break;

            case SORT_BUBBLE:
                bubbleSort(arr.data(), arr.size());
                break;

            case SORT_SELECTION:
                selectionSort(arr.data(), arr.size());
            }

            mCompCharts[algoIdx].addValue(cmpCounter);
            mAssnCharts[algoIdx].addValue(assnCounter);
        }

        mWindow->mCompChart->putChart(&mCompCharts[algoIdx], CHARTS_COLORS[algoIdx]);
        mWindow->mAssnChart->putChart(&mAssnCharts[algoIdx], CHARTS_COLORS[algoIdx]);
    }

protected:
    AnalysisWindow *mWindow;

    Chart mCompCharts[SORT_ALGO_MAX], mAssnCharts[SORT_ALGO_MAX];
};

#endif