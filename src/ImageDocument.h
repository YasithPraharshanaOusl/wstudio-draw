#pragma once
#include <wx/wx.h>
#include <vector>
#include "Layer.h"
#include "RasterLayer.h"

class ImageDocument {
public:
    ~ImageDocument() {
        for (auto* layer : layers) delete layer;
    }

    void Render(wxDC& dc) {
        for (auto* layer : layers) {
            if (layer->IsVisible()) {
                layer->Draw(dc);
            }
        }
    }

    void AddLayer(Layer* layer) {
        layers.push_back(layer);
        activeIndex = static_cast<int>(layers.size()) - 1;
    }

    void RemoveLayer(int index) {
        if (index < 0 || index >= static_cast<int>(layers.size())) return;
        delete layers[index];
        layers.erase(layers.begin() + index);
        if (activeIndex >= static_cast<int>(layers.size())) {
            activeIndex = static_cast<int>(layers.size()) - 1;
        }
    }

    bool HasLayers() const { return !layers.empty(); }

    const std::vector<Layer*>& GetLayers() const { return layers; }

    int GetActiveLayerIndex() const { return activeIndex; }

    void SetActiveLayerIndex(int index) {
        if (index >= 0 && index < static_cast<int>(layers.size())) {
            activeIndex = index;
        }
    }

    Layer* GetActiveLayer() {
        if (activeIndex < 0 || activeIndex >= static_cast<int>(layers.size())) return nullptr;
        return layers[activeIndex];
    }

    RasterLayer* GetActiveRasterLayer() {
        return dynamic_cast<RasterLayer*>(GetActiveLayer());
    }

    wxSize GetCanvasSize() const { return canvasSize; }
    void SetCanvasSize(const wxSize& size) { canvasSize = size; }

    wxImage CompositeImage() {
        if (canvasSize.GetWidth() <= 0 || canvasSize.GetHeight() <= 0) return wxImage();
        wxBitmap bmp(canvasSize.GetWidth(), canvasSize.GetHeight());
        wxMemoryDC mem(dcBitmap);
        mem.SetBackground(*wxWHITE_BRUSH);
        mem.Clear();
        Render(mem);
        mem.SelectObject(wxNullBitmap);
        return bmp.ConvertToImage();
    }

private:
    std::vector<Layer*> layers;
    int activeIndex = -1;
    wxSize canvasSize = wxSize(0, 0);
    wxBitmap dcBitmap;
};
