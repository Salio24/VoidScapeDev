#include "Texture.hpp"

#include <iostream>
#include <SDL3/SDL_image.h>

// add some sprite, some struct that will hold, sprite offset in samplerArray index etc, and apply it in draw func

TextureHandler::TextureHandler() {
	
}

TextureHandler::~TextureHandler() {
	
}

SDL_Surface* TextureHandler::LoadSurface(const char* filepath) {
	SDL_Surface* surface = IMG_Load(filepath);
	if (!surface) {
		std::cerr << "IMG_Load Error: Cannot load " << filepath << std::endl;
		exit(1);
	}
	return surface;
}

void TextureHandler::InitTextureArray(const GLenum internalformat, const GLsizei resolution, const GLsizei depth) {
    mTextureArrays.emplace(resolution, std::make_pair(0, 0));

	glGenTextures(1, &mTextureArrays[resolution].first);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArrays[resolution].first);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalformat, resolution, resolution, depth);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void TextureHandler::LoadTexture(SDL_Surface* surface, const GLenum& internalformat, int& layer) {
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, surface->w, surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_DestroySurface(surface);
    layer++;
}

std::vector<SDL_Surface*> TextureHandler::CutTileset(SDL_Surface* tileset, const int& tileWidth, const int& tileHeight) {

    std::vector<SDL_Surface*> tiles;

    // Calculate the number of rows and columns in the tileset
    int rows = tileset->h / tileHeight;
    int cols = tileset->w / tileWidth;

    tileset = SDL_ConvertSurface(tileset, SDL_PIXELFORMAT_ABGR8888);
    if (!tileset) {
        SDL_Log("Failed to convert tileset format: %s", SDL_GetError());
    }

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            // Define the rectangle for the current tile
            SDL_Rect srcRect = { col * tileWidth, row * tileHeight, tileWidth, tileHeight };

            // Create a new surface for the tile
            SDL_Surface* tile = SDL_CreateSurface(tileWidth, tileHeight, tileset->format);
            if (!tile) {
                std::cerr << "Failed to create tile surface: " << SDL_GetError() << std::endl;
                continue;
            }

            SDL_BlitSurface(tileset, &srcRect, tile, NULL);
            tiles.push_back(FlipSurfaceVertically(tile));
            SDL_DestroySurface(tile);
        }
    }
    SDL_DestroySurface(tileset);

    return tiles;
}

SDL_Surface* TextureHandler::FlipSurfaceVertically(SDL_Surface* surface) {
    // Create a new surface with the same dimensions as the original surface
    SDL_Surface* flipped = SDL_CreateSurface(surface->w, surface->h, surface->format);

    if (flipped == nullptr) {
        SDL_Log("Unable to create surface: %s", SDL_GetError());
        return nullptr;
    }

    // Create source and destination rectangles
    SDL_Rect srcRect = { 0, 0, surface->w, 1 };
    SDL_Rect dstRect = { 0, 0, surface->w, 1 };

    for (int y = 0; y < surface->h; ++y) {
        srcRect.y = surface->h - y - 1;
        dstRect.y = y;

        SDL_BlitSurface(surface, &srcRect, flipped, &dstRect);
    }

    return flipped;
}

void TextureHandler::CutAndLoad(const char* filepath, std::pair<int, int>& tilesetLocation, const int& resolution) {
    SDL_Surface* tilesetSurface = LoadSurface(filepath);
    std::vector<SDL_Surface*> cutTileset = CutTileset(tilesetSurface, resolution, resolution);
    SDL_DestroySurface(tilesetSurface);
    if (mTextureArrays.find(resolution) != mTextureArrays.end()) {
        tilesetLocation.first = resolution;
    }
    else {
        std::cerr << "ERROR: CutAndLoad no texture array found for resolution: " << resolution << std::endl;
    }
    tilesetLocation.second = mTextureArrays[resolution].second;
	glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArrays[resolution].first);
    for (int i = 0; i < cutTileset.size(); i++) {
        LoadTexture(cutTileset[i], GL_RGBA, mTextureArrays[resolution].second);
    }
}

void TextureHandler::LoadAll() {
    CutAndLoad((const char*)"assets/Level/tiles128upA.png", mTilesetLocations.mBaseTileset, 128);
    //CutAndLoad((const char*)"assets/UI/test.png", mUI_BordersT_Offset, arrayIndex, 16);
	CutAndLoad((const char*)"assets/UI/Borders32p.png", mTilesetLocations.mUIBorderTileset, 32);

    CutAndLoad((const char*)"assets/UI/Arrows32p.png", mTilesetLocations.mUIArrowsTileset, 32);

    CutAndLoad((const char*)"assets/Actor/Orb128_2.png", mTilesetLocations.mDoubejumpOrbTileset, 128);
}