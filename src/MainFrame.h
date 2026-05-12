#pragma once
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/filedlg.h>
#include <wx/numdlg.h>
#include <wx/textdlg.h>
#include <wx/fontdlg.h>
#include <wx/colordlg.h>
#include <functional>
#include "CanvasPanel.h"
#include "ImageDocument.h"
#include "History.h"
#include "Filters.h"
#include "LayerPanel.h"
#include "RasterLayer.h"
#include "TextLayer.h"

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1200, 800)) {
        doc = new ImageDocument();
        history = new History();

        wxSplitterWindow* splitter = new wxSplitterWindow(this);
        layerPanel = new LayerPanel(splitter, doc);
        canvas = new CanvasPanel(splitter, doc);
        splitter->SplitVertically(layerPanel, canvas, 240);
        splitter->SetSashGravity(0.0);

        CreateMenuBar();
        CreateStatusBar();
        BindEvents();
    }

    ~MainFrame() override {
        delete history;
        delete doc;
    }

private:
    enum MenuId {
        ID_Open = 1001,
        ID_Save = 1002,
        ID_Undo = 2001,
        ID_Redo = 2002,
        ID_Crop = 3001,
        ID_Resize = 3002,
        ID_Rotate = 3003,
        ID_BrightnessContrast = 3004,
        ID_Filter_Grayscale = 4001,
        ID_Filter_Sepia = 4002,
        ID_Filter_Blur = 4003,
        ID_Filter_Sharpen = 4004,
        ID_AddText = 5001,
        ID_AddLayer = 5002
    };

    CanvasPanel* canvas;
    LayerPanel* layerPanel;
    ImageDocument* doc;
    History* history;

    void CreateMenuBar() {
        wxMenu* fileMenu = new wxMenu;
        fileMenu->Append(ID_Open, "Open...");
        fileMenu->Append(ID_Save, "Save As...");

        wxMenu* editMenu = new wxMenu;
        editMenu->Append(ID_Undo, "Undo");
        editMenu->Append(ID_Redo, "Redo");

        wxMenu* imageMenu = new wxMenu;
        imageMenu->Append(ID_Crop, "Crop");
        imageMenu->Append(ID_Resize, "Resize");
        imageMenu->Append(ID_Rotate, "Rotate");
        imageMenu->Append(ID_BrightnessContrast, "Brightness/Contrast");

        wxMenu* filterMenu = new wxMenu;
        filterMenu->Append(ID_Filter_Grayscale, "Grayscale");
        filterMenu->Append(ID_Filter_Sepia, "Sepia");
        filterMenu->Append(ID_Filter_Blur, "Blur");
        filterMenu->Append(ID_Filter_Sharpen, "Sharpen");

        wxMenu* layerMenu = new wxMenu;
        layerMenu->Append(ID_AddLayer, "Add Empty Layer");
        layerMenu->Append(ID_AddText, "Add Text Layer");

        wxMenuBar* menuBar = new wxMenuBar;
        menuBar->Append(fileMenu, "File");
        menuBar->Append(editMenu, "Edit");
        menuBar->Append(imageMenu, "Image");
        menuBar->Append(filterMenu, "Filters");
        menuBar->Append(layerMenu, "Layers");

        SetMenuBar(menuBar);
    }

    void BindEvents() {
        Bind(wxEVT_MENU, &MainFrame::OnOpen, this, ID_Open);
        Bind(wxEVT_MENU, &MainFrame::OnSave, this, ID_Save);
        Bind(wxEVT_MENU, &MainFrame::OnUndo, this, ID_Undo);
        Bind(wxEVT_MENU, &MainFrame::OnRedo, this, ID_Redo);
        Bind(wxEVT_MENU, &MainFrame::OnCrop, this, ID_Crop);
        Bind(wxEVT_MENU, &MainFrame::OnResize, this, ID_Resize);
        Bind(wxEVT_MENU, &MainFrame::OnRotate, this, ID_Rotate);
        Bind(wxEVT_MENU, &MainFrame::OnBrightnessContrast, this, ID_BrightnessContrast);
        Bind(wxEVT_MENU, &MainFrame::OnFilterGrayscale, this, ID_Filter_Grayscale);
        Bind(wxEVT_MENU, &MainFrame::OnFilterSepia, this, ID_Filter_Sepia);
        Bind(wxEVT_MENU, &MainFrame::OnFilterBlur, this, ID_Filter_Blur);
        Bind(wxEVT_MENU, &MainFrame::OnFilterSharpen, this, ID_Filter_Sharpen);
        Bind(wxEVT_MENU, &MainFrame::OnAddText, this, ID_AddText);
        Bind(wxEVT_MENU, &MainFrame::OnAddLayer, this, ID_AddLayer);
    }

    void RefreshAll() {
        layerPanel->RefreshLayers();
        canvas->Refresh();
    }

    RasterLayer* GetActiveRasterLayer() {
        return doc->GetActiveRasterLayer();
    }

    void ApplyRasterOp(const wxString& name, const std::function<void(wxImage&)>& op) {
        RasterLayer* layer = GetActiveRasterLayer();
        if (!layer) {
            wxMessageBox("No active raster layer.", "Error", wxOK | wxICON_ERROR);
            return;
        }
        wxImage before = layer->GetImage();
        wxImage after = before.Copy();
        op(after);
        history->Execute(new RasterImageCommand(layer, before, after, name));
        RefreshAll();
    }

    void OnOpen(wxCommandEvent&) {
        wxFileDialog dlg(this, "Open Image", "", "", "Image files (*.png;*.jpg;*.jpeg)|*.png;*.jpg;*.jpeg", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (dlg.ShowModal() == wxID_OK) {
            wxImage img;
            if (!img.LoadFile(dlg.GetPath())) {
                wxMessageBox("Failed to load image.", "Error", wxOK | wxICON_ERROR);
                return;
            }
            doc->AddLayer(new RasterLayer(img, "Background"));
            doc->SetCanvasSize(wxSize(img.GetWidth(), img.GetHeight()));
            RefreshAll();
        }
    }

    void OnSave(wxCommandEvent&) {
        if (!doc->HasLayers()) {
            wxMessageBox("Nothing to save.", "Info", wxOK | wxICON_INFORMATION);
            return;
        }
        wxFileDialog dlg(this, "Save Image", "", "", "PNG file (*.png)|*.png|JPEG file (*.jpg)|*.jpg", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (dlg.ShowModal() == wxID_OK) {
            wxImage out = doc->CompositeImage();
            if (!out.SaveFile(dlg.GetPath())) {
                wxMessageBox("Failed to save image.", "Error", wxOK | wxICON_ERROR);
            }
        }
    }

    void OnUndo(wxCommandEvent&) {
        history->Undo();
        RefreshAll();
    }

    void OnRedo(wxCommandEvent&) {
        history->Redo();
        RefreshAll();
    }

    void OnCrop(wxCommandEvent&) {
        RasterLayer* layer = GetActiveRasterLayer();
        if (!layer) return;
        wxNumberEntryDialog dx(this, "Crop X", "X:", "Crop", 0, 0, layer->GetImage().GetWidth());
        if (dx.ShowModal() != wxID_OK) return;
        wxNumberEntryDialog dy(this, "Crop Y", "Y:", "Crop", 0, 0, layer->GetImage().GetHeight());
        if (dy.ShowModal() != wxID_OK) return;
        wxNumberEntryDialog dw(this, "Crop Width", "Width:", "Crop", layer->GetImage().GetWidth(), 1, layer->GetImage().GetWidth());
        if (dw.ShowModal() != wxID_OK) return;
        wxNumberEntryDialog dh(this, "Crop Height", "Height:", "Crop", layer->GetImage().GetHeight(), 1, layer->GetImage().GetHeight());
        if (dh.ShowModal() != wxID_OK) return;

        int x = dx.GetValue();
        int y = dy.GetValue();
        int w = dw.GetValue();
        int h = dh.GetValue();

        ApplyRasterOp("Crop", [=](wxImage& img) {
            img = img.GetSubImage(wxRect(x, y, w, h));
        });
        doc->SetCanvasSize(wxSize(w, h));
    }

    void OnResize(wxCommandEvent&) {
        RasterLayer* layer = GetActiveRasterLayer();
        if (!layer) return;
        wxNumberEntryDialog dw(this, "Resize Width", "Width:", "Resize", layer->GetImage().GetWidth(), 1, 8000);
        if (dw.ShowModal() != wxID_OK) return;
        wxNumberEntryDialog dh(this, "Resize Height", "Height:", "Resize", layer->GetImage().GetHeight(), 1, 8000);
        if (dh.ShowModal() != wxID_OK) return;
        int w = dw.GetValue();
        int h = dh.GetValue();

        ApplyRasterOp("Resize", [=](wxImage& img) {
            img = img.Scale(w, h, wxIMAGE_QUALITY_HIGH);
        });
        doc->SetCanvasSize(wxSize(w, h));
    }

    void OnRotate(wxCommandEvent&) {
        RasterLayer* layer = GetActiveRasterLayer();
        if (!layer) return;
        wxNumberEntryDialog dd(this, "Rotate Degrees", "Degrees:", "Rotate", 90, -360, 360);
        if (dd.ShowModal() != wxID_OK) return;
        double degrees = dd.GetValue();
        ApplyRasterOp("Rotate", [=](wxImage& img) {
            img = img.Rotate(degrees * 3.1415926535 / 180.0, wxPoint(img.GetWidth() / 2, img.GetHeight() / 2));
        });
    }

    void OnBrightnessContrast(wxCommandEvent&) {
        wxNumberEntryDialog db(this, "Brightness (-100..100)", "Brightness:", "Brightness/Contrast", 0, -100, 100);
        if (db.ShowModal() != wxID_OK) return;
        wxNumberEntryDialog dc(this, "Contrast (-100..100)", "Contrast:", "Brightness/Contrast", 0, -100, 100);
        if (dc.ShowModal() != wxID_OK) return;
        int brightness = db.GetValue();
        int contrast = dc.GetValue();

        ApplyRasterOp("Brightness/Contrast", [=](wxImage& img) {
            Filters::BrightnessContrast(img, brightness, contrast);
        });
    }

    void OnFilterGrayscale(wxCommandEvent&) {
        ApplyRasterOp("Grayscale", [](wxImage& img) { Filters::Grayscale(img); });
    }

    void OnFilterSepia(wxCommandEvent&) {
        ApplyRasterOp("Sepia", [](wxImage& img) { Filters::Sepia(img); });
    }

    void OnFilterBlur(wxCommandEvent&) {
        ApplyRasterOp("Blur", [](wxImage& img) { Filters::BoxBlur(img, 2); });
    }

    void OnFilterSharpen(wxCommandEvent&) {
        ApplyRasterOp("Sharpen", [](wxImage& img) { Filters::Sharpen(img); });
    }

    void OnAddLayer(wxCommandEvent&) {
        wxImage img(doc->GetCanvasSize().GetWidth(), doc->GetCanvasSize().GetHeight());
        img.SetRGB(wxRect(0, 0, img.GetWidth(), img.GetHeight()), 255, 255, 255);
        doc->AddLayer(new RasterLayer(img, "Layer"));
        RefreshAll();
    }

    void OnAddText(wxCommandEvent&) {
        wxTextEntryDialog textDlg(this, "Enter text:", "Add Text Layer");
        if (textDlg.ShowModal() != wxID_OK) return;
        wxString text = textDlg.GetValue();

        wxFontData fontData;
        wxFontDialog fontDlg(this, fontData);
        if (fontDlg.ShowModal() != wxID_OK) return;
        wxFont font = fontDlg.GetFontData().GetChosenFont();

        wxColourData colourData;
        wxColourDialog colourDlg(this, &colourData);
        if (colourDlg.ShowModal() != wxID_OK) return;
        wxColour colour = colourDlg.GetColourData().GetColour();

        wxNumberEntryDialog dx(this, "Text X", "X:", "Text Position", 10, 0, 8000);
        if (dx.ShowModal() != wxID_OK) return;
        wxNumberEntryDialog dy(this, "Text Y", "Y:", "Text Position", 10, 0, 8000);
        if (dy.ShowModal() != wxID_OK) return;

        doc->AddLayer(new TextLayer(text, wxPoint(dx.GetValue(), dy.GetValue()), font, colour));
        RefreshAll();
    }
};
