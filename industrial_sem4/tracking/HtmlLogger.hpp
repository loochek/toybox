#ifndef HTML_LOGGER_HPP
#define HTML_LOGGER_HPP

#include <cstdio>
#include <stdexcept>
#include "BaseLogger.hpp"

#define HL_PADDED_PRINT(fmt) fprintf(mLogFile, "%*s" fmt, mCurrDepth * NEST_PAD, "")
#define HL_PADDED_PRINTF(fmt, ...) fprintf(mLogFile, "%*s" fmt, mCurrDepth * NEST_PAD, "", __VA_ARGS__)

class HtmlLogger : public BaseLogger
{
public:
    virtual ~HtmlLogger() {}

    virtual void functionEnter(const std::string &prettyName) override
    {
        assert(mLogFile);

        HL_PADDED_PRINTF("%s\n", prettyName.c_str());
        HL_PADDED_PRINT("{\n");
        
        mCurrDepth++;
    }

    virtual void functionLeave() override
    {
        assert(mLogFile);

        mCurrDepth--;
        HL_PADDED_PRINT("}\n");
    }

    virtual void simpleCtor(const TrackedInt &obj) override
    {
        assert(mLogFile);

        HL_PADDED_PRINTF("[New] \"%s\" (%d|%p) <-- %d\n", obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue);
    }

    virtual void parentPresentCtor(const TrackedInt &obj, const TrackedInt &parent,
                                   const std::string &oper) override
    {
        assert(mLogFile);

        HL_PADDED_PRINTF("[New] \"%s\" (%d|%p) <--(%d)-- %s\"%s\" (%d|%p)\n", 
            obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue, oper.c_str(),
            parent.mName.c_str(), parent.mObjIndex, &parent);
    }

    virtual void parentsPresentCtor(const TrackedInt &obj, const TrackedInt &parent1, const TrackedInt &parent2,
                                    const std::string &oper) override
    {
        assert(mLogFile);

        HL_PADDED_PRINTF("[New] \"%s\" (%d|%p) <--(%d)-- \"%s\" (%d|%p) %s \"%s\" (%d|%p)\n",
            obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue,
            parent1.mName.c_str(), parent1.mObjIndex, &parent1, oper.c_str(),
            parent2.mName.c_str(), parent2.mObjIndex, &parent2);
    }

    virtual void copyCtor(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mLogFile);

        HL_PADDED_PRINTF("[<font color=#FF0000>COPY</font>] \"%s\" (%d|%p) <--(%d)-- \"%s\" (%d|%p)\n",
            obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue,
            parent.mName.c_str(), parent.mObjIndex, &parent);
    }

    virtual void simpleAssignment(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mLogFile);

        HL_PADDED_PRINTF("[Assign] \"%s\" (%d|%p) <--(%d)-- \"%s\" (%d|%p)\n",
            obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue,
            parent.mName.c_str(), parent.mObjIndex, &parent);
    }

    virtual void assignment(const TrackedInt &obj, const TrackedInt &parent, const std::string &oper) override
    {
        assert(mLogFile);

        HL_PADDED_PRINTF("[Assign] \"%s\" (%d|%p) <--(%d)-- \"%s\" (%d|%p) %s \"%s\" (%d|%p)\n",
            obj.mName.c_str(), obj.mObjIndex, &obj, obj.mValue,
            obj.mName.c_str(), obj.mObjIndex, &obj, oper.c_str(),
            parent.mName.c_str(), parent.mObjIndex, &parent);
    }

    void start(const std::string &fileName)
    {
        mLogFile = fopen(fileName.c_str(), "w");
        if (!mLogFile) throw std::runtime_error("Unable to open log file");

        fprintf(mLogFile, "<pre>\n");
    }

    void finish()
    {
        fprintf(mLogFile, "</pre>\n");
        fclose(mLogFile);
    }

    static HtmlLogger &getInstance()
    {
        static HtmlLogger instance;
        return instance;
    }

protected:
    HtmlLogger() : mCurrDepth(0), mLogFile(nullptr) {}

    HtmlLogger(const HtmlLogger&) = delete;
    HtmlLogger& operator=(const HtmlLogger&) = delete;

protected:
    int mCurrDepth;
    FILE *mLogFile;

    static const int NEST_PAD = 4;
};

#endif