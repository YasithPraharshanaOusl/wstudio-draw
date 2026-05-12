#pragma once
#include <wx/image.h>
#include "RasterLayer.h"

class Command {
public:
    virtual ~Command() {}
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual wxString Name() const = 0;
};

class RasterImageCommand : public Command {
public:
    RasterImageCommand(RasterLayer* target, const wxImage& beforeImg, const wxImage& afterImg, const wxString& name)
        : layer(target), before(beforeImg), after(afterImg), label(name) {}

    void Execute() override { if (layer) layer->SetImage(after); }
    void Undo() override { if (layer) layer->SetImage(before); }
    wxString Name() const override { return label; }

private:
    RasterLayer* layer;
    wxImage before;
    wxImage after;
    wxString label;
};
