#pragma once
#include "ConfigReader.h"
#include "includes.h"
#include "Texture.h"

namespace Average4k
{
	struct CachedAsset
	{
		AvgEngine::OpenGL::Texture* texture = NULL;
		std::string path = "";
		int id = -1;
		bool operator==(const CachedAsset& other) {
			return id == other.id || path == other.path;
		}
	};
	class Skin
	{
		std::vector<CachedAsset> _cache{};
		std::string _assetPath = "";

	public:
		float upscale = 1.0f;

		External::ConfigReader skinConfig{};

		std::string name = "";
		std::string fullPath = "";

		// basic values

		bool rotate = false;
		bool shrink = false;
		bool bounce = false;
		bool rotateMine = false;

		std::string sparrow = "";

		Skin() = default;

		Skin(std::string _name, std::string _p)
		{
			fullPath = _p + _name;
			name = _name;

			skinConfig = External::ConfigReader(fullPath + "/config.skin");
			std::string p = skinConfig.Value("path");
			if (p != "default")
				_assetPath = fullPath + "/" + p + "/";
			else
				_assetPath = "assets/skinDefaults/";

			rotate = skinConfig.Bool("rotate");
			shrink = skinConfig.Bool("shrink");
			bounce = skinConfig.Bool("bounce");
			rotateMine = skinConfig.Bool("rotateMine");

			sparrow = skinConfig.Value("sparrow");

			#ifdef _DEBUG
			AvgEngine::Logging::writeLog("[Skin] [Debug] Loaded " + name + " successfully!");
			#endif
		}

		~Skin()
		{
			EmptyCache();
		}

		CachedAsset Cache(std::string path)
		{
			for(CachedAsset& asset : _cache)
			{
				if (asset.path == path)
					return asset;
			}
			return {};
		}

		CachedAsset Cache(int id)
		{
			for (CachedAsset& asset : _cache)
			{
				if (asset.id == id)
					return asset;
			}
			return {};
		}

		void EmptyCache()
		{
			for (CachedAsset& asset : _cache)
				delete asset.texture;
			_cache.clear();
		}

		AvgEngine::OpenGL::Texture* GetTexture(std::string asset)
		{
			std::string p = _assetPath + asset + ".png";
			const CachedAsset& a = Cache(p);
			if (a.id != -1)
				return a.texture;
			AvgEngine::OpenGL::Texture* t = AvgEngine::OpenGL::Texture::createWithImage(p);
			t->dontDelete = true;
			CachedAsset as = { t,p, static_cast<int>(t->id) };
			_cache.push_back(as);
			return t;
		}

		std::vector<std::string> GetText(std::string asset)
		{
			std::string p = _assetPath + asset;
			std::ifstream s;
			s.open(p);
			if (!s.good())
			{
				AvgEngine::Logging::writeLog("[Skin] Failed to read " + p);
				return {};
			}
			std::vector<std::string> lines{};
			std::string line;

			while (std::getline(s, line)) {
				lines.push_back(line);
			}
			return lines;
		}

		std::string GetLua(std::string asset)
		{
			return _assetPath + "Lua/" + asset + ".lua";
		}


		std::string GetPath(std::string asset)
		{
			return _assetPath + asset;
		}

		std::string GetFontPath()
		{
			return _assetPath + "Fonts";
		}

		
	};
}
