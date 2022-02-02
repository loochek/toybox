#ifndef BASE_CHART_HPP
#define BASE_CHART_HPP

/**
 * Base class for chart
 */
class BaseChart
{
public:
    virtual void addValue(const int value) = 0;
    virtual int operator[](int idx) const = 0;
    virtual int getValuesCount() const = 0;
};

#endif