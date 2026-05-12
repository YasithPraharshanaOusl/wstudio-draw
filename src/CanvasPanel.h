#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "ImageDocument.h"

class CanvasPanel : public wxPanel {
public:
    CanvasPanel(wxWindow* parent, ImageDocument* doc)
        : wxPanel(parent), document(doc) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &CanvasPanel::OnPaint, this);
    }

private:
    ImageDocument* document;

    void OnPaint(wxPaintEvent&) {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();
        if (document) {
            document->Render(dc);
        }
    }
};
