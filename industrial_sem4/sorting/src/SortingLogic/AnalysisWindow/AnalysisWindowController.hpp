#ifndef ANALYSIS_WINDOW_CONTROLLER_HPP
#define ANALYSIS_WINDOW_CONTROLLER_HPP

#include <vector>
#include <random>
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../../SortingWidgets/AnalysisWindow.hpp"
#include "../../SortingWidgets/ChartWidget.hpp"
#include "../../Sorting/SortObject.hpp"
#include "../../Utils/Chart.hpp"

class AnalysisWindowController : public ButtonDelegate
{
public:
    AnalysisWindowController() = delete;
    AnalysisWindowController(AnalysisWindow *window) : mWindow(window) {};

    virtual void onClick(uint64_t userData) override
    {
        cmpChart.clear();
        assnChart.clear();

        for (int elemCnt = 0; elemCnt < 10000; elemCnt++)
        {
            int cmpCounter = 0, assnCounter = 0;

            std::vector<SortObject> arr;
            for (int i = 0; i < elemCnt; i++)
                arr.push_back(SortObject(i, cmpCounter, assnCounter));

            std::mt19937 rng;
            std::shuffle(arr.begin(), arr.end(), rng);

            cmpCounter = 0;
            assnCounter = 0;
            std::sort(arr.begin(), arr.end());
            cmpChart.addValue(cmpCounter);
            assnChart.addValue(assnCounter);
        }

        mWindow->mCompChart->addChart(&cmpChart, LGL::Color::Magenta);
        mWindow->mCompChart->addChart(&assnChart, LGL::Color::Cyan);
    }

protected:
    AnalysisWindow *mWindow;
    Chart cmpChart, assnChart;
};

#endif