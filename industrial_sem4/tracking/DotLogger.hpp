#ifndef DOT_LOGGER_HPP
#define DOT_LOGGER_HPP

#include <cstdio>
#include <stdexcept>
#include <unordered_map>
#include "BaseLogger.hpp"

const int HISTORY_ID_STEP = 10000;

class DotLogger : public BaseLogger
{
public:
    virtual ~DotLogger() {}

    virtual void functionEnter(const std::string &prettyName) override
    {        
        // fprintf(mDotFile, "subgraph cluster_ {\n");
        // fprintf(mDotFile, "label=\"%s\"\n", prettyName.c_str());
    }

    virtual void functionLeave() override
    {
        //fprintf(mDotFile, "}\n");
    }

    virtual void simpleCtor(const TrackedInt &obj) override
    {
        if (!mDotFile)
            return;

        fprintf(mDotFile, "obj%d [shape=record, label=\"\\\"%s\\\" | addr: %p | { val: %d | idx: %d }\"]\n",
            obj.mObjIndex, obj.mName.c_str(), &obj, obj.mValue, obj.mObjIndex);

        fprintf(mDotFile, "obj%d -> obj%d [weight=100, style=invis]\n", mLastObj, obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
        mLastObj = obj.mObjIndex;
    }

    virtual void parentPresentCtor(const TrackedInt &obj, const TrackedInt &parent,
                                   const std::string &oper) override
    {
        if (!mDotFile)
            return;

        fprintf(mDotFile, "obj%d [shape=record, label=\"\\\"%s\\\" | addr: %p | { val: %d | idx: %d }\"]\n",
            obj.mObjIndex, obj.mName.c_str(), &obj, obj.mValue, obj.mObjIndex);

        fprintf(mDotFile, "oper%d [shape=ellipse, label=\"%s]\n", mOperCount, oper.c_str());

        fprintf(mDotFile, "obj%d -> oper%d\n", mAssnHistory[parent.mObjIndex], mOperCount);
        fprintf(mDotFile, "oper%d -> obj%d\n", mOperCount, obj.mObjIndex);

        fprintf(mDotFile, "obj%d -> oper%d [weight=100, style=invis]\n", mLastObj, mOperCount);
        fprintf(mDotFile, "oper%d -> obj%d [weight=100, style=invis]\n", mOperCount, obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
        mOperCount++;
        mLastObj = obj.mObjIndex;
    }

    virtual void parentsPresentCtor(const TrackedInt &obj, const TrackedInt &parent1, const TrackedInt &parent2,
                                    const std::string &oper) override
    {
        if (!mDotFile)
            return;

        fprintf(mDotFile, "obj%d [shape=record, label=\"\\\"%s\\\" | addr: %p | { val: %d | idx: %d }\"]\n",
            obj.mObjIndex, obj.mName.c_str(), &obj, obj.mValue, obj.mObjIndex);

        fprintf(mDotFile, "oper%d [shape=ellipse, label=\"%s\"]\n", mOperCount, oper.c_str());

        fprintf(mDotFile, "obj%d -> oper%d\n", mAssnHistory[parent1.mObjIndex], mOperCount);
        fprintf(mDotFile, "obj%d -> oper%d\n", mAssnHistory[parent2.mObjIndex], mOperCount);
        fprintf(mDotFile, "oper%d -> obj%d\n", mOperCount, obj.mObjIndex);

        fprintf(mDotFile, "obj%d -> oper%d [weight=100, style=invis]\n", mLastObj, mOperCount);
        fprintf(mDotFile, "oper%d -> obj%d [weight=100, style=invis]\n", mOperCount, obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
        mOperCount++;
        mLastObj = obj.mObjIndex;
    }

    virtual void copyCtor(const TrackedInt &obj, const TrackedInt &parent) override
    {
        if (!mDotFile)
            return;

        fprintf(mDotFile, "obj%d [shape=record, label=\"\\\"%s\\\" | addr: %p | { val: %d | idx: %d }\"]\n",
            obj.mObjIndex, obj.mName.c_str(), &obj, obj.mValue, obj.mObjIndex);
            
        fprintf(mDotFile, "obj%d -> obj%d\n", mAssnHistory[parent.mObjIndex], obj.mObjIndex);
        fprintf(mDotFile, "obj%d -> obj%d [weight=100, style=invis]\n", mLastObj, obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
        mLastObj = obj.mObjIndex;
    }

    virtual void simpleAssignment(const TrackedInt &obj, const TrackedInt &parent) override
    {
        if (!mDotFile)
            return;

        int assnNodeId = mAssnHistory[obj.mObjIndex] + HISTORY_ID_STEP;

        fprintf(mDotFile, "obj%d [shape=ellipse, label=\"\\\"%s\\\" = \\\"%s\\\"\"]\n",
            assnNodeId, obj.mName.c_str(), parent.mName.c_str());

        fprintf(mDotFile, "obj%d -> obj%d\n", obj.mObjIndex, assnNodeId);
        fprintf(mDotFile, "obj%d -> obj%d\n", parent.mObjIndex, assnNodeId);

        fprintf(mDotFile, "obj%d -> obj%d [weight=100, style=invis]\n", mLastObj, assnNodeId);

        mAssnHistory[obj.mObjIndex] = assnNodeId;
        mLastObj = assnNodeId;
    }

    virtual void assignment(const TrackedInt &obj, const TrackedInt &parent, const std::string &oper) override
    {
        if (!mDotFile)
            return;

        int assnNodeId = mAssnHistory[obj.mObjIndex] + HISTORY_ID_STEP;

        fprintf(mDotFile, "obj%d [shape=ellipse, label=\"\\\"%s\\\" %s= \\\"%s\\\"\"]\n",
            assnNodeId, obj.mName.c_str(), oper.c_str(), parent.mName.c_str());

        fprintf(mDotFile, "obj%d -> obj%d\n", obj.mObjIndex, assnNodeId);
        fprintf(mDotFile, "obj%d -> obj%d\n", parent.mObjIndex, assnNodeId);

        fprintf(mDotFile, "obj%d -> obj%d [weight=100, style=invis]\n", mLastObj, assnNodeId);

        mAssnHistory[obj.mObjIndex] = assnNodeId;
        mLastObj = assnNodeId;
    }

    void start()
    {
        mDotFile = fopen("log.dot", "w");
        if (!mDotFile) throw std::runtime_error("Unable to open log file");

        fprintf(mDotFile, "digraph {\n");
    }

    void finish()
    {
        fprintf(mDotFile, "}\n");
        fclose(mDotFile);

        system("dot -Tpng -o log.png log.dot");
    }

    static DotLogger &getInstance()
    {
        static DotLogger instance;
        return instance;
    }

protected:
    DotLogger() : mDotFile(nullptr), mOperCount(0), mLastObj(0) {}

    DotLogger(const DotLogger&) = delete;
    DotLogger& operator=(const DotLogger&) = delete;

protected:
    int mOperCount;
    FILE *mDotFile;

    std::unordered_map<int, int> mAssnHistory;
    int mLastObj;
    
    static const int NEST_PAD = 4;
};

#endif