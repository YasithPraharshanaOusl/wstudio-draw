#pragma once
#include "Layer.h"

class TextLayer : public Layer {
public:
    TextLayer(const wxString& text, const wxPoint& pos, const wxFont& f, const wxColour& c)
        : message(text), position(pos), font(f), colour(c) {
        name = "Text";
    }

    void Draw(wxDC& dc) override {
        dc.SetFont(font);
        dc.SetTextForeground(colour);
        dc.DrawText(message, position);
    }

private:
    wxString message;
    wxPoint position;
    wxFont font;
    wxColour colour;
};
