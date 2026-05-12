#pragma once
#include <wx/wx.h>
#include <wx/listbox.h>
#include "ImageDocument.h"

class LayerPanel : public wxPanel {
public:
    LayerPanel(wxWindow* parent, ImageDocument* doc)
        : wxPanel(parent), document(doc) {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        list = new wxListBox(this, wxID_ANY);
        sizer->Add(new wxStaticText(this, wxID_ANY, "Layers"), 0, wxALL, 6);
        sizer->Add(list, 1, wxEXPAND | wxALL, 6);
        SetSizer(sizer);

        list->Bind(wxEVT_LISTBOX, &LayerPanel::OnSelectLayer, this);
    }

    void RefreshLayers() {
        list->Clear();
        const auto& layers = document->GetLayers();
        for (size_t i = 0; i < layers.size(); ++i) {
            list->Append(wxString::Format("%zu: %s", i, layers[i]->GetName()));
        }
        int idx = document->GetActiveLayerIndex();
        if (idx >= 0 && idx < (int)layers.size()) {
            list->SetSelection(idx);
        }
    }

private:
    ImageDocument* document;
    wxListBox* list;

    void OnSelectLayer(wxCommandEvent& evt) {
        document->SetActiveLayerIndex(evt.GetSelection());
    }
};
