#ifndef VISUAL_SORT_OBJECT_HPP
#define VISUAL_SORT_OBJECT_HPP

#include <mutex>

class VisualSortObject;

struct VisualSortData
{
    VisualSortObject *array;
    int arrSize;
    std::mutex &mutex;
    bool &ready;
    bool &cancelPending;
    int &highlIdx1;
    int &highlIdx2;
};

class SortInterruptException : public std::exception {};

class VisualSortObject
{
public:
    VisualSortObject(int value, int idx, const VisualSortData data) : mValue(value), mIdx(idx), mData(data) {}

    VisualSortObject &operator=(const VisualSortObject &other)
    {
        checkCancel();

        mValue = other.mValue;
        if (isThisInArray())
            yield();
        
        return *this;
    }

    bool operator<(const VisualSortObject &other) const
    {
        checkCancel();

        mData.highlIdx1 = mIdx;
        mData.highlIdx2 = other.mIdx;
        yield();
    
        return mValue < other.mValue;
    }

    int getValue() const { return mValue; };

protected:
    bool isThisInArray() const
    {
        return (this - mData.array) < mData.arrSize;
    }

    /// Wait main thread to redraw sorting picture
    void yield() const
    {
        mData.ready = true;
        mData.mutex.lock();
    }

    void checkCancel() const
    {
        if (mData.cancelPending)
        {
            throw SortInterruptException();
        }
    }

protected:
    int mValue, mIdx;
    const VisualSortData mData;
};

#endif