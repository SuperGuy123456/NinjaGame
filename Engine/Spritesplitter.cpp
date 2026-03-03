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