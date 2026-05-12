#pragma once
#include <stack>
#include "Command.h"

class History {
public:
    ~History() { Clear(); }

    void Execute(Command* cmd) {
        cmd->Execute();
        undoStack.push(cmd);
        while (!redoStack.empty()) {
            delete redoStack.top();
            redoStack.pop();
        }
    }

    void Undo() {
        if (undoStack.empty()) return;
        Command* cmd = undoStack.top();
        undoStack.pop();
        cmd->Undo();
        redoStack.push(cmd);
    }

    void Redo() {
        if (redoStack.empty()) return;
        Command* cmd = redoStack.top();
        redoStack.pop();
        cmd->Execute();
        undoStack.push(cmd);
    }

    void Clear() {
        while (!undoStack.empty()) { delete undoStack.top(); undoStack.pop(); }
        while (!redoStack.empty()) { delete redoStack.top(); redoStack.pop(); }
    }

private:
    std::stack<Command*> undoStack;
    std::stack<Command*> redoStack;
};
