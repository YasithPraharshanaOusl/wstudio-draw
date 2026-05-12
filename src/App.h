#pragma once
#include <wx/wx.h>
#include <wx/image.h>
#include "MainFrame.h"

class App : public wxApp {
public:
    bool OnInit() override {
        wxInitAllImageHandlers();
        MainFrame* frame = new MainFrame("Photo Editor (C++/wxWidgets)");
        frame->Show(true);
        return true;
    }
};
