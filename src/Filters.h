#pragma once
#include <wx/image.h>
#include <algorithm>

class Filters {
public:
    static void Grayscale(wxImage& img) {
        unsigned char* data = img.GetData();
        int size = img.GetWidth() * img.GetHeight() * 3;
        for (int i = 0; i < size; i += 3) {
            unsigned char r = data[i];
            unsigned char g = data[i + 1];
            unsigned char b = data[i + 2];
            unsigned char gray = static_cast<unsigned char>((r + g + b) / 3);
            data[i] = data[i + 1] = data[i + 2] = gray;
        }
    }

    static void Sepia(wxImage& img) {
        unsigned char* data = img.GetData();
        int size = img.GetWidth() * img.GetHeight() * 3;
        for (int i = 0; i < size; i += 3) {
            int r = data[i];
            int g = data[i + 1];
            int b = data[i + 2];
            int tr = std::min(255, (int)(0.393 * r + 0.769 * g + 0.189 * b));
            int tg = std::min(255, (int)(0.349 * r + 0.686 * g + 0.168 * b));
            int tb = std::min(255, (int)(0.272 * r + 0.534 * g + 0.131 * b));
            data[i] = (unsigned char)tr;
            data[i + 1] = (unsigned char)tg;
            data[i + 2] = (unsigned char)tb;
        }
    }

    static void BoxBlur(wxImage& img, int radius) {
        if (radius <= 0) return;
        wxImage copy = img.Copy();
        int w = img.GetWidth();
        int h = img.GetHeight();
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int r = 0, g = 0, b = 0, count = 0;
                for (int dy = -radius; dy <= radius; ++dy) {
                    for (int dx = -radius; dx <= radius; ++dx) {
                        int nx = x + dx;
                        int ny = y + dy;
                        if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                            unsigned char* p = copy.GetData() + (ny * w + nx) * 3;
                            r += p[0]; g += p[1]; b += p[2];
                            count++;
                        }
                    }
                }
                unsigned char* d = img.GetData() + (y * w + x) * 3;
                d[0] = (unsigned char)(r / count);
                d[1] = (unsigned char)(g / count);
                d[2] = (unsigned char)(b / count);
            }
        }
    }

    static void Sharpen(wxImage& img) {
        wxImage copy = img.Copy();
        int w = img.GetWidth();
        int h = img.GetHeight();
        int kernel[3][3] = {{0,-1,0},{-1,5,-1},{0,-1,0}};
        for (int y = 1; y < h - 1; ++y) {
            for (int x = 1; x < w - 1; ++x) {
                int r = 0, g = 0, b = 0;
                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        unsigned char* p = copy.GetData() + ((y + ky) * w + (x + kx)) * 3;
                        int k = kernel[ky + 1][kx + 1];
                        r += p[0] * k;
                        g += p[1] * k;
                        b += p[2] * k;
                    }
                }
                unsigned char* d = img.GetData() + (y * w + x) * 3;
                d[0] = (unsigned char)std::clamp(r, 0, 255);
                d[1] = (unsigned char)std::clamp(g, 0, 255);
                d[2] = (unsigned char)std::clamp(b, 0, 255);
            }
        }
    }

    static void BrightnessContrast(wxImage& img, int brightness, int contrast) {
        double b = brightness / 100.0 * 255.0;
        double c = (contrast + 100) / 100.0;
        unsigned char* data = img.GetData();
        int size = img.GetWidth() * img.GetHeight() * 3;
        for (int i = 0; i < size; i += 3) {
            for (int j = 0; j < 3; ++j) {
                int v = static_cast<int>((data[i + j] - 128) * c + 128 + b);
                data[i + j] = (unsigned char)std::clamp(v, 0, 255);
            }
        }
    }
};
