#ifndef SORT_COUNT_OBJECT_HPP
#define SORT_COUNT_OBJECT_HPP

/**
 * Special object that counts assignments and comparisons
 */
class SortCountObject
{
public:
    SortCountObject(int value, int &cmpCounter, int &assnCounter) :
        mValue(value), mCmpCounter(cmpCounter), mAssnCounter(assnCounter) {}

    SortCountObject(const SortCountObject &other) :
        mCmpCounter(other.mCmpCounter), mAssnCounter(other.mAssnCounter), mValue(other.mValue)
    {
        mAssnCounter++;
    }

    SortCountObject &operator=(const SortCountObject &other)
    {
        mValue = other.mValue;
        mAssnCounter++;
        return *this;
    }

    bool operator<(const SortCountObject &other) const
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