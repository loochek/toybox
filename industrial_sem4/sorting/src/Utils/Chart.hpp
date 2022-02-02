#ifndef CHART_HPP
#define CHART_HPP

#include <vector>
#include "BaseChart.hpp"

/**
 * Simple chart - vector of values
 */
class Chart : public BaseChart
{
public:
    Chart() {};

    virtual void addValue      (const int value) override { mValues.push_back(value); }
    virtual int  operator[]    (int idx)   const override { return mValues[idx]; }
    virtual int  getValuesCount()          const override { return mValues.size(); }

    void clear() { mValues.clear(); }

protected:
    std::vector<int> mValues;
};

#endif