#pragma once
#include "boundingbox.h"
#include "offset.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <cmath>
#include "../inc/FreeImage.h"

using namespace std;

#pragma comment(lib,"../lib/FreeImage.lib")

class Tex_info
{
public:
	int xOrigin;
	int yOrigin;
	int tWidth;
	int tHeight;
	int nAtlas;
};

class Search_point
{
public:
	int xSearch;
	int ySearch;
	int IsUsed;
};

class ImageIN
{ 
public:
	FIBITMAP * imgdib;
	int imgindex;
};

typedef vector<Tex_info> OneTex;
typedef vector<vector<Tex_info >> DouTex;
typedef vector<vector<vector<Tex_info >>> TriTex;

typedef vector<Search_point> PointArray;
typedef vector<vector<Search_point >> DouPointArray;
typedef vector<vector<vector<Search_point >>> TriPointArray;

class TextureMerge
{
public:
	TextureMerge();
	~TextureMerge();

	int rimg_width;
	int rimg_height;

	void Grouping(
		const vector<string> &inputImageName,
		const vector<TEXBOUNDINGBOX> &texBoundingBox,
		vector<vector<int>> &groupingIndex,
		vector<string> &mergedImageName,
		vector<OFFSET> &offset);

	void SetTexLoadPath(string texpath){m_TexLoadPath = texpath;}
	void SetTexSavePath(string texpath){m_TexSavePath = texpath;}
	void SetTexSaveName(string texname){m_TexSaveName = texname;}

	void Save();

private:
	string m_TexLoadPath;
	string m_TexSavePath;
	string m_TexSaveName;


	TriTex TextureSet;
	TriPointArray SearchPointSet;

	vector<ImageIN> Image_Handled;
	vector<ImageIN> Image_UnHandled;

	bool check(FIBITMAP * bitmap,OneTex c_tex,Search_point c_point,int x_limit,int y_limit, bool flag);
	string num2str(int i);
	void Load(vector<string> inputImageName,vector<TEXBOUNDINGBOX> texBoundingBox,int r_width,int r_height);
};