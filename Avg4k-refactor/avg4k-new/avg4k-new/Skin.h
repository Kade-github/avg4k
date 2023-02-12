#pragma once
#include "ConfigReader.h"
#include "includes.h"
#include "Texture.h"

namespace Average4k::Skin
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

	public:
		External::ConfigReader skinConfig{};

		std::string name = "";
		std::string fullPath = "";

		Skin() = default;

		Skin(std::string _name, std::string _p)
		{
			fullPath = _p;
			name = _name;

			skinConfig = External::ConfigReader(fullPath + _name + "/config.skin");
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
			std::string p = fullPath + asset + ".png";
			const CachedAsset& a = Cache(p);
			if (a.id != -1)
				return a.texture;
			AvgEngine::OpenGL::Texture* t = AvgEngine::OpenGL::Texture::createWithImage(p);
			CachedAsset as = { t,p, static_cast<int>(t->id) };
			_cache.push_back(as);
			return t;
		}

		std::vector<std::string> GetText(std::string asset)
		{
			std::string p = fullPath + asset;
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


		std::string GetPath(std::string asset)
		{
			return fullPath + asset;
		}
	};
}
