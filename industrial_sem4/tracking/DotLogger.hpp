#ifndef DOT_LOGGER_HPP
#define DOT_LOGGER_HPP

#include <cstdio>
#include <stdexcept>
#include <unordered_map>
#include "BaseLogger.hpp"

const char *DOT_RED = "red";
const char *DOT_GREEN = "chartreuse4";

const int GRAY_STEP = 40;
const int OPER_ID_START = 1 << 30;

class DotLogger : public BaseLogger
{
public:
    virtual ~DotLogger() {}

    virtual void functionEnter(const std::string &prettyName) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        fprintf(mDotNodesFile, "subgraph cluster_%d {\n", mClusterCounter++);
        fprintf(mDotNodesFile, "label=\"%s\"\n", prettyName.c_str());

        int grayStrength = 0xFF - (((mCurrDepth + 1) * GRAY_STEP) % 0x80);
        fprintf(mDotNodesFile, "style=filled; color=\"#%2x%2x%2x\"\n", grayStrength, grayStrength, grayStrength);

        mCurrDepth++;
    }

    virtual void functionLeave() override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        mCurrDepth--;
        fprintf(mDotNodesFile, "}\n");
    }

    virtual void simpleCtor(const TrackedInt &obj) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        logNewObject(obj);
        logExecFlow(obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
    }

    virtual void parentPresentCtor(const TrackedInt &obj, const TrackedInt &parent,
                                   const std::string &oper) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        logNewObject(obj);
        int operId = logOper(oper);

        logEdge(mAssnHistory[parent.mObjIndex], operId);
        logEdge(operId, obj.mObjIndex);

        logExecFlow(operId);
        logExecFlow(obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
    }

    virtual void parentsPresentCtor(const TrackedInt &obj, const TrackedInt &parent1, const TrackedInt &parent2,
                                    const std::string &oper) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        logNewObject(obj);
        int operId = logOper(oper);

        logEdge(mAssnHistory[parent1.mObjIndex], operId);
        logEdge(mAssnHistory[parent2.mObjIndex], operId);
        logEdge(operId, obj.mObjIndex);

        logExecFlow(operId);
        logExecFlow(obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
    }

    virtual void dtor(const TrackedInt &obj) override
    {
        // int operId = logDtor(obj);
        // logExecFlow(operId);
    }

    virtual void copyCtor(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mDotNodesFile && mDotEdgesFile);
            
        logNewObject(obj);
        logEdge(mAssnHistory[parent.mObjIndex], obj.mObjIndex, DOT_RED);
        logExecFlow(obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
    }

    virtual void moveCtor(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mDotNodesFile && mDotEdgesFile);
            
        logNewObject(obj);
        logEdge(mAssnHistory[parent.mObjIndex], obj.mObjIndex, DOT_GREEN);
        logExecFlow(obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
    }

    virtual void copyAssignment(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        int operId = logAssnOper("\\\"" + obj.mName + "\\\" = \\\"" + parent.mName + "\\\"", obj.mValue);

        logEdge(mAssnHistory[obj.mObjIndex], operId);
        logEdge(mAssnHistory[parent.mObjIndex], operId, DOT_RED);

        logExecFlow(operId);

        mAssnHistory[obj.mObjIndex] = operId;
    }

    virtual void moveAssignment(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        int operId = logAssnOper("\\\"" + obj.mName + "\\\" = \\\"" + parent.mName + "\\\"", obj.mValue);

        logEdge(mAssnHistory[obj.mObjIndex], operId);
        logEdge(mAssnHistory[parent.mObjIndex], operId, DOT_GREEN);

        logExecFlow(operId);

        mAssnHistory[obj.mObjIndex] = operId;
    }

    virtual void assignment(const TrackedInt &obj, const TrackedInt &parent, const std::string &oper) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        int operId = logAssnOper("\\\"" + obj.mName + "\\\" " + oper +  "= \\\"" + parent.mName + "\\\"", obj.mValue);

        logEdge(mAssnHistory[obj.mObjIndex], operId);
        logEdge(mAssnHistory[parent.mObjIndex], operId);

        logExecFlow(operId);

        mAssnHistory[obj.mObjIndex] = operId;
    }

    void start()
    {
        mDotNodesFile = fopen("log.dotpart1", "w");
        if (!mDotNodesFile)
            throw std::runtime_error("Unable to open log file");

        mDotEdgesFile = fopen("log.dotpart2", "w");
        if (!mDotEdgesFile)
        {
            fclose(mDotNodesFile);
            throw std::runtime_error("Unable to open log file");
        }

        fprintf(mDotNodesFile, "digraph {\n");
        fprintf(mDotNodesFile, "nodesep=0.5\n");
    }

    void finish()
    {
        fprintf(mDotEdgesFile, "}\n");

        fclose(mDotNodesFile);
        fclose(mDotEdgesFile);

        system("cat log.dotpart1 log.dotpart2 > log.dot");
        system("dot -Tpng -o log.png log.dot");
    }

    static DotLogger &getInstance()
    {
        static DotLogger instance;
        return instance;
    }

protected:
    DotLogger() : mDotNodesFile(nullptr), mDotEdgesFile(nullptr),
        mCurrDepth(0), mOperCounter(OPER_ID_START), mClusterCounter(0), mLastNodeIdx(0) {}

    DotLogger(const DotLogger&) = delete;
    DotLogger& operator=(const DotLogger&) = delete;

    void logNewObject(const TrackedInt &obj)
    {
        fprintf(mDotNodesFile, "%d [shape=record, label=\"\\\"%s\\\" | addr: %p | { val: %d | idx: %d }\"]\n",
            obj.mObjIndex, obj.mName.c_str(), &obj, obj.mValue, obj.mObjIndex);
    }

    void logEdge(int from, int to, const std::string &color = "black")
    {
        fprintf(mDotEdgesFile, "%d -> %d [color=%s]\n", from, to, color.c_str());
    }

    int logOper(const std::string &label)
    {
        fprintf(mDotNodesFile, "%d [shape=ellipse, label=\"%s\"]\n", mOperCounter, label.c_str());
        return mOperCounter++;
    }

    int logAssnOper(const std::string &label, int newVal)
    {
        fprintf(mDotNodesFile, "%d [shape=record, label=\"%s | new val: %d\"]\n",
            mOperCounter, label.c_str(), newVal);
        return mOperCounter++;
    }

    void logExecFlow(int currNodeIdx)
    {
        fprintf(mDotEdgesFile, "%d -> %d [weight=10, style=dotted, arrowhead=none, color=blue]\n",
            mLastNodeIdx, currNodeIdx);

        mLastNodeIdx = currNodeIdx;
    }

    int logDtor(const TrackedInt &obj)
    {
        fprintf(mDotEdgesFile, "%d [label=\"EOL: %s\"]\n", mOperCounter, obj.mName.c_str());
        return mOperCounter++;
    }

protected:
    int mCurrDepth, mOperCounter, mClusterCounter;
    FILE *mDotNodesFile, *mDotEdgesFile;
    
    std::unordered_map<int, int> mAssnHistory;
    int mLastNodeIdx;
    
    static const int NEST_PAD = 4;
};

#endif