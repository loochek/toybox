#ifndef SORT_OBJECT_HPP
#define SORT_OBJECT_HPP

class SortObject
{
public:
    SortObject(int value, int &cmpCounter, int &assnCounter) :
        mValue(value), mCmpCounter(cmpCounter), mAssnCounter(assnCounter) {}

    SortObject(const SortObject &other) :
        mCmpCounter(other.mCmpCounter), mAssnCounter(other.mAssnCounter), mValue(other.mValue)
    {
        mAssnCounter++;
    }

    SortObject &operator=(const SortObject &other)
    {
        mValue = other.mValue;
        mAssnCounter++;
        return *this;
    }

    bool operator<(const SortObject &other)
    {
        mCmpCounter++;
        return mValue < other.mValue;
    }

protected:
    int mValue;

    int &mCmpCounter;
    int &mAssnCounter;
};

#endif