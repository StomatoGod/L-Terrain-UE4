#pragma once
#include "LTerrainEditor.h"
#include "LNoise.h"

class LPatch;
class LRule;
class LSymbol;
class LGroundTexture;
class LPaintWeight;
class LMeshAsset;
class LObjectScatter;

typedef TSharedPtr<LSymbol, ESPMode::ThreadSafe> LSymbolPtr;
typedef TSharedPtr<LRule, ESPMode::ThreadSafe> LRulePtr;
typedef TSharedPtr<LPatch, ESPMode::ThreadSafe> LPatchPtr;
typedef TSharedPtr<LGroundTexture, ESPMode::ThreadSafe> LGroundTexturePtr;
typedef TSharedPtr<LPaintWeight, ESPMode::ThreadSafe> LPaintWeightPtr;
typedef TSharedPtr<LMeshAsset, ESPMode::ThreadSafe> LMeshAssetPtr;
typedef TSharedPtr<LObjectScatter, ESPMode::ThreadSafe> LObjectScatterPtr;
typedef TArray<TArray<TSharedPtr<LSymbol, ESPMode::ThreadSafe>>> LSymbol2DMap;
typedef TSharedPtr<LSymbol2DMap, ESPMode::ThreadSafe> LSymbol2DMapPtr;

class LSystem
{
public:
	void Reset();
	void GenerateSomeDefaults();
	LSymbol2DMapPtr IterateLString(LSymbol2DMapPtr source);
	LRulePtr GetLRuleMatch(LSymbol2DMapPtr map, int xIdx, int yIdx);
	LPatchPtr GetLPatchMatch(LSymbolPtr toMatch);
	static LSymbolPtr GetMapSymbolFrom01Coords(LSymbol2DMapPtr map, float xPercCoord, float yPercCoord);
	LSymbolPtr GetDefaultSymbol();

public:
	TArray<LRulePtr> rules;
	TArray<LSymbolPtr> symbols;
	TArray<LPatchPtr> patches;
	TArray<LSymbol2DMapPtr> lSystemLoDs;
	TArray<LGroundTexturePtr> groundTextures;
	TArray<LMeshAssetPtr> meshAssets;

	static const int DIMS = 5;
};

class LSymbol
{
public:
	LSymbol(char symbol, FString name);
	static LSymbol2DMapPtr CreateLSymbolMap(int inner, int outer);

	//special static instance of LSymbol to represent "any symbol"
	static LSymbolPtr MatchAny() { return _matchAny; }

	char symbol;
	FString name;
	FAssetData texture;

protected:
	static LSymbolPtr _matchAny;
};

class LRule
{
public:
	static LRulePtr CreateRule(LSymbolPtr matchVal, LSymbol2DMapPtr replacementVals);
	static LRulePtr CreatePropegateRule(LSymbolPtr matchVal, LSymbolPtr propegateVal);


protected:
	LRule(LSymbolPtr matchVal, LSymbol2DMapPtr replacementVals)
	{
		this->matchVal = matchVal;
		this->replacementVals = replacementVals;

		bMatchNeighbors = false;
		matchNeighborsMap = LSymbol::CreateLSymbolMap(3, 3);
		(*matchNeighborsMap)[0] = { LSymbol::MatchAny(), LSymbol::MatchAny(), LSymbol::MatchAny() };
		(*matchNeighborsMap)[1] = { LSymbol::MatchAny(),            matchVal, LSymbol::MatchAny() };
		(*matchNeighborsMap)[2] = { LSymbol::MatchAny(), LSymbol::MatchAny(), LSymbol::MatchAny() };
	}

public:
	FString name;
	LSymbolPtr matchVal;
	LSymbol2DMapPtr replacementVals;
	bool bMatchNeighbors;
	LSymbol2DMapPtr matchNeighborsMap;

};

class LPatch
{
public:
	LPatch() :
		name("default patch"),
		matchVal(LSymbolPtr()),
		minHeight(0.f),
		maxHeight(0.f),
		bHeightMatch(true),
		heightSmoothFactor(0.5f),
		noiseMaps(TArray<LNoisePtr>()),
		paintWeights(TArray<LPaintWeightPtr>()),
		objectScatters(TArray<LObjectScatterPtr>())
	{}

	FString name;
	LSymbolPtr matchVal;
	float minHeight;
	float maxHeight;
	bool bHeightMatch;
	float heightSmoothFactor;
	TArray<LNoisePtr> noiseMaps;
	TArray<LPaintWeightPtr> paintWeights; //first entry should be used as base
	TArray<LObjectScatterPtr> objectScatters;
};

//just a mesh asset with a name
class LMeshAsset
{
public:
	FString name;
	FAssetData foliageType;
	FAssetData object;
};

//controls distribution of a mesh asset
class LObjectScatter
{
public:
	LObjectScatter() :
		maxRadius(2.f),
		minRadius(1.f)
	{}

	LMeshAssetPtr meshAsset;
	float maxRadius; //per 1kuu; values that are too high relative to minRadius will not result in more instances
	float minRadius; //between placements
};

class LGroundTexture
{
public:
	FString name;
	FAssetData layerInfo;
	FAssetData texture;
	FAssetData normalMap;
};

class LPaintWeight
{
public:
	LGroundTexturePtr texture;
	float weight;
	LNoisePtr noiseMap;
	bool bUseAboveThreshold;
	float threshold;
	float thresholdFeather;
};