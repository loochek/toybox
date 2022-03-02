#ifndef DOT_LOGGER_HPP
#define DOT_LOGGER_HPP

#include <cstdio>
#include <stdexcept>
#include <unordered_map>
#include "BaseLogger.hpp"

const char *DOT_COPY_COLOR     = "red";
const char *DOT_MOVE_COLOR     = "chartreuse4";
const char *DOT_EXECFLOW_COLOR = "blue";
const char *DOT_LIFETIME_COLOR = "indianred4";

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
        int operId = logDtor(obj);
        logExecFlow(operId);
    }

    virtual void copyCtor(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mDotNodesFile && mDotEdgesFile);
            
        logNewObject(obj);
        logEdge(mAssnHistory[parent.mObjIndex], obj.mObjIndex, DOT_COPY_COLOR);
        logExecFlow(obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
    }

    virtual void moveCtor(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mDotNodesFile && mDotEdgesFile);
            
        logNewObject(obj);
        logEdge(mAssnHistory[parent.mObjIndex], obj.mObjIndex, DOT_MOVE_COLOR);
        logExecFlow(obj.mObjIndex);

        mAssnHistory[obj.mObjIndex] = obj.mObjIndex;
    }

    virtual void copyAssignment(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        int operId = logAssnOper("\"" + obj.mName + "\" = \"" + parent.mName + "\"", obj.mValue);

        logEdge(mAssnHistory[obj.mObjIndex], operId);
        logEdge(mAssnHistory[parent.mObjIndex], operId, DOT_COPY_COLOR);

        logExecFlow(operId);

        mAssnHistory[obj.mObjIndex] = operId;
    }

    virtual void moveAssignment(const TrackedInt &obj, const TrackedInt &parent) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        int operId = logAssnOper("\"" + obj.mName + "\" = \"" + parent.mName + "\"", obj.mValue);

        logEdge(mAssnHistory[obj.mObjIndex], operId);
        logEdge(mAssnHistory[parent.mObjIndex], operId, DOT_MOVE_COLOR);

        logExecFlow(operId);

        mAssnHistory[obj.mObjIndex] = operId;
    }

    virtual void assignment(const TrackedInt &obj, const TrackedInt &parent, const std::string &oper) override
    {
        assert(mDotNodesFile && mDotEdgesFile);

        int operId = logAssnOper("\"" + obj.mName + "\" " + oper +  "= \"" + parent.mName + "\"", obj.mValue);

        logEdge(mAssnHistory[obj.mObjIndex], operId);
        logEdge(mAssnHistory[parent.mObjIndex], operId);

        logExecFlow(operId);

        mAssnHistory[obj.mObjIndex] = operId;
    }

    virtual void start() override
    {
        mOperCounter = OPER_ID_START;
        mClusterCounter = 0;
        mLastNodeIdx = -1;

        TrackedInt::sCopyCount = 0;
        TrackedInt::sTmpObjectsCount = 0;

        mDotNodesFile = fopen("log.dotpart1", "w");
        if (!mDotNodesFile)
            throw std::runtime_error("Unable to open log file");

        mDotEdgesFile = fopen("log.dotpart2", "w");
        if (!mDotEdgesFile)
        {
            fclose(mDotNodesFile);
            mDotNodesFile = nullptr;
            throw std::runtime_error("Unable to open log file");
        }

        fprintf(mDotNodesFile, "digraph {\n");
        //fprintf(mDotNodesFile, "nodesep=1\n");
    }

    virtual void finish() override
    {
        if (!(mDotNodesFile && mDotEdgesFile))
            return;
        
        logLegend();
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
        mCurrDepth(0), mOperCounter(OPER_ID_START), mClusterCounter(0), mLastNodeIdx(-1) {}

    DotLogger(const DotLogger&) = delete;
    DotLogger& operator=(const DotLogger&) = delete;

    void logNewObject(const TrackedInt &obj)
    {
        fprintf(mDotNodesFile, "%d [shape=none, label=<\n"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
            "<TR>\n"
            "<TD ROWSPAN=\"3\">\"%s\"</TD>\n"
            "<TD ROWSPAN=\"3\">addr: %p</TD>\n"
            "</TR>\n"
            "<TR><TD>val: %d</TD></TR>\n"
            "<TR><TD>idx: %d</TD></TR>\n"
            "</TABLE>>]\n",
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
        fprintf(mDotNodesFile, "%d [shape=none, label=<\n"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
            "<TR>\n"
            "<TD>%s</TD>\n"
            "<TD>new val: %d</TD>\n"
            "</TR>\n"
            "</TABLE>>]\n",
            mOperCounter, label.c_str(), newVal);
        return mOperCounter++;
    }

    void logExecFlow(int currNodeIdx)
    {
        if (mLastNodeIdx != -1)
        {
            fprintf(mDotEdgesFile, "%d -> %d [weight=10, style=dashed, arrowhead=none, color=%s]\n",
                mLastNodeIdx, currNodeIdx, DOT_EXECFLOW_COLOR);
        }

        mLastNodeIdx = currNodeIdx;
    }

    int logDtor(const TrackedInt &obj)
    {
        fprintf(mDotNodesFile, "%d [shape=ellipse, label=\"EOL: \\\"%s\\\"\"]\n",
            mOperCounter, obj.mName.c_str());

        fprintf(mDotEdgesFile, "%d -> %d [style=dashed, arrowhead=none, color=%s]\n",
            obj.mObjIndex, mOperCounter, DOT_LIFETIME_COLOR);

        return mOperCounter++;
    }

    void logLegend()
    {
        fprintf(mDotEdgesFile,
            "subgraph cluster_edge\n"
            "{\n"
            "node [shape=plaintext]\n"
            "label=\"Total: %d tmp objects, %d copies\"\n"
            "{\n"
            "rank=same\n"
            "key1 [label=<<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\" cellborder=\"0\">\n"
            "<tr><td align=\"right\" port=\"e1\">Operands/result</td></tr>\n"
            "<tr><td align=\"right\" port=\"e2\">Copy</td></tr>\n"
            "<tr><td align=\"right\" port=\"e3\">Move</td></tr>\n"
            "<tr><td align=\"right\" port=\"e4\">Execution flow</td></tr>\n"
            "<tr><td align=\"right\" port=\"e5\">Lifecycle</td></tr>\n"
            "</table>>]\n"
            "key2 [label=<<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\" cellborder=\"0\">\n"
            "<tr><td port=\"e1\">&nbsp;</td></tr>\n"
            "<tr><td port=\"e2\">&nbsp;</td></tr>\n"
            "<tr><td port=\"e3\">&nbsp;</td></tr>\n"
            "<tr><td port=\"e4\">&nbsp;</td></tr>\n"
            "<tr><td port=\"e5\">&nbsp;</td></tr>\n"
            "</table>>]\n"
            "}\n"
            "key1:e1:e -> key2:e1:w\n"
            "key1:e2:e -> key2:e2:w [color=%s]\n"
            "key1:e3:e -> key2:e3:w [color=%s]\n"
            "key1:e4:e -> key2:e4:w [style=dashed, arrowhead=none, color=%s]\n"
            "key1:e5:e -> key2:e5:w [style=dashed, arrowhead=none, color=%s]\n"
            "}\n",
            TrackedInt::sTmpObjectsCount, TrackedInt::sCopyCount,
            DOT_COPY_COLOR, DOT_MOVE_COLOR, DOT_EXECFLOW_COLOR, DOT_LIFETIME_COLOR);
    }

protected:
    int mCurrDepth, mOperCounter, mClusterCounter;
    FILE *mDotNodesFile, *mDotEdgesFile;
    
    std::unordered_map<int, int> mAssnHistory;
    int mLastNodeIdx;
    
    static const int NEST_PAD = 4;
};

#endif
