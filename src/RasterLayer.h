#pragma once
#include "Layer.h"
#include <wx/image.h>
#include <wx/bitmap.h>

class RasterLayer : public Layer {
public:
    RasterLayer(const wxImage& img, const wxString& n) : image(img) { name = n; }

    void Draw(wxDC& dc) override {
        if (!image.IsOk()) return;
        wxBitmap bmp(image);
        dc.DrawBitmap(bmp, 0, 0, true);
    }

    const wxImage& GetImage() const { return image; }
    wxImage& GetImage() { return image; }
    void SetImage(const wxImage& img) { image = img; }

private:
    wxImage image;
};
