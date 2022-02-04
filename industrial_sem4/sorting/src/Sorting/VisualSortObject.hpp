#ifndef VISUAL_SORT_OBJECT_HPP
#define VISUAL_SORT_OBJECT_HPP

#include "../App.hpp"
#include "../SortingWidgets/VisualSortWidget.hpp"

class VisualSortObject
{
public:
    VisualSortObject(int value, int idx, VisualSortWidget *widget, VisualSortObject *array, int arrSize, App *app) :
        mValue(value), mIdx(idx), mWidget(widget), mArray(array), mArrSize(arrSize), mApp(app){}

    // VisualSortObject(const VisualSortObject &other) : mValue(other.mValue), mIdx(other.mIdx),
    //     mWidget(other.mWidget), mArray(other.mArray), mArrSize(other.mArrSize), mApp(other.mApp) {}

    VisualSortObject &operator=(const VisualSortObject &other)
    {
        mValue = other.mValue;
        if (isThisInArray())
            mApp->redraw();
        
        return *this;
    }

    bool operator<(const VisualSortObject &other) const
    {
        mWidget->highlight(mIdx, other.mIdx);
        mApp->redraw();
    
        return mValue < other.mValue;
    }

    int getValue() const { return mValue; };

protected:
    bool isThisInArray() const
    {
        return (this - mArray) < mArrSize;
    }

protected:
    int mValue, mIdx;

    VisualSortObject *mArray;
    int mArrSize;

    VisualSortWidget *mWidget;
    App *mApp;
};

#endif