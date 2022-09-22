#include <stdio.h>
#include <scene.hpp>
#include <hgdecoder.hpp>

#include <utils.hpp>


// Create SDL window and renderer
SceneManager::SceneManager()
{
    window = SDL_CreateWindow("FelineSystem2",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    printf("SceneManager initialized\n");
}

void SceneManager::displayTexture(SDL_Texture *texture)
{
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect DestR = {0, 0, w, h};
    SDL_RenderCopyEx(renderer, texture, NULL, &DestR, 0, 0, SDL_FLIP_VERTICAL);
    SDL_RenderPresent(renderer);
}

// Returns a pointer to a texture from a given frame
// Caller is responsible for freeing the texture
SDL_Texture *SceneManager::getTextureFromFrame(HGDecoder::Frame frame)
{
    auto rgbaVec = HGDecoder::getPixelsFromFrame(frame);

    // Pixel buffer must remain alive when using surface
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(rgbaVec.data(), frame.Stdinfo->Width, frame.Stdinfo->Height, frame.Stdinfo->BitDepth, PITCH(frame.Stdinfo->Width, frame.Stdinfo->BitDepth), RMASK, GMASK, BMASK, AMASK);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    return texture;
}

// Decode a raw HG buffer and display the first frame
void SceneManager::displayImage(byte *buf, size_t sz, ImageMeta ud)
{
    auto fpath = ud.fpath;
    HGHeader *hgHeader = reinterpret_cast<HGHeader *>(buf);
    FrameHeader *frameHeader = reinterpret_cast<FrameHeader *>(hgHeader + 1);
    std::vector<HGDecoder::Frame> frames = HGDecoder::getFrames(frameHeader);
    if (frames.empty())
    {
        printf("No frames found\n");
        return;
    }

    // Just handle the first frame for now
    SDL_Texture *texture = getTextureFromFrame(frames[0]);

    // Store texture in cache
    textureCache.insert({fpath, texture});

    // Do not free as it is in cache
    // SDL_DestroyTexture(texture);

    displayTexture(texture);
}

void SceneManager::setScene(const std::string fpath)
{
    if (textureCache.find(fpath) != textureCache.end())
    {
        displayTexture(textureCache[fpath]);
        return;
    }

    ImageMeta ud = {fpath};

    Utils::fetchFileAndProcess(fpath, this, &SceneManager::displayImage, ud);
}
