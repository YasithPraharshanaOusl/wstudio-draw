#pragma once
#include <wx/wx.h>

class Layer {
public:
    virtual ~Layer() {}
    virtual void Draw(wxDC& dc) = 0;

    bool IsVisible() const { return visible; }
    void SetVisible(bool v) { visible = v; }

    const wxString& GetName() const { return name; }
    void SetName(const wxString& n) { name = n; }

protected:
    bool visible = true;
    wxString name = "Layer";
};
