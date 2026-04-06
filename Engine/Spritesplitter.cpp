#include "Spritesplitter.h"

vector<Texture2D> SpriteSplitter::SplitByHorizontal(const char* path, float slicewidth, float scaleFactor)
{
    Texture2D unsplittex = LoadTexture(path);
    Image unsplitimage = LoadImageFromTexture(unsplittex);
    UnloadTexture(unsplittex); // Free GPU memory

    vector<Texture2D> splittedtxs;

    float texturewidth = static_cast<float>(unsplitimage.width);
    float textureheight = static_cast<float>(unsplitimage.height);

    for (float x = 0; x < texturewidth; x += slicewidth)
    {
        Image splitpart = ImageCopy(unsplitimage);
        ImageCrop(&splitpart, Rectangle{ x, 0, slicewidth, textureheight });

        if (scaleFactor != 1.0f)
        {
            int newWidth = static_cast<int>(slicewidth * scaleFactor);
            int newHeight = static_cast<int>(textureheight * scaleFactor);
            ImageResizeNN(&splitpart, newWidth, newHeight); // Nearest-neighbor scaling
        }

        Texture2D frame = LoadTextureFromImage(splitpart);
        SetTextureFilter(frame, TEXTURE_FILTER_POINT); // Preserve pixel sharpness
        splittedtxs.push_back(frame);

        UnloadImage(splitpart);
    }

    UnloadImage(unsplitimage); // Free CPU memory
    return splittedtxs;
}

std::vector<Texture2D> SpriteSplitter::SplitGrid(
    const std::string& path,
    int cellWidth,
    int cellHeight,
    int rows,
    int cols,
    float scale
) {
    std::vector<Texture2D> out;
    out.reserve(rows * cols);

    // Load the full sheet
    Image sheetImg = LoadImage(path.c_str());

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {

            Rectangle src = {
                (float)(x * cellWidth),
                (float)(y * cellHeight),
                (float)cellWidth,
                (float)cellHeight
            };

            // Extract the frame
            Image frame = ImageFromImage(sheetImg, src);

            // Scale using nearest-neighbor
            if (scale != 1.0f) {
                int newW = (int)(cellWidth * scale);
                int newH = (int)(cellHeight * scale);
                ImageResizeNN(&frame, newW, newH);
            }

            // Convert to texture
            Texture2D tex = LoadTextureFromImage(frame);
            SetTextureFilter(tex, TEXTURE_FILTER_POINT); // <-- CRITICAL
            out.push_back(tex);

            UnloadImage(frame);
        }
    }

    UnloadImage(sheetImg);
    return out;
}
