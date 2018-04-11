#include "TextureMerge.h"

//重载的sort比较函数
bool sortstandard(const ImageIN& m1,const ImageIN& m2)
{
	if(FreeImage_GetHeight(m1.imgdib)>FreeImage_GetHeight(m2.imgdib))
		return true;
	else if((FreeImage_GetHeight(m1.imgdib)==FreeImage_GetHeight(m2.imgdib)) && (FreeImage_GetWidth(m1.imgdib)>FreeImage_GetWidth(m2.imgdib)))
		return true;
	else return false;
}

TextureMerge::TextureMerge()
{
	FreeImage_Initialise();
	rimg_width=1024;
	rimg_height=1024;
	m_TexLoadPath="";
	m_TexSaveName="";
	m_TexSavePath="";
}

TextureMerge::~TextureMerge()
{
	for (int i=0;i<Image_Handled.size();i++)
	{	
		FreeImage_Unload(Image_Handled[i].imgdib);
	}
	FreeImage_DeInitialise();
}

bool TextureMerge::check(FIBITMAP* bitmap,OneTex c_tex,Search_point c_point,int x_limit,int y_limit,bool flag)
{
	//为高度边界，不同情况取值不同
	int y_board;
	if (flag)
	{
		y_board=y_limit;
	} 
	else
	{
		y_board=c_tex[0].yOrigin+c_tex[0].tHeight;
	}
	
	//先判断新纹理加入后是否超出条状纹理空间边界
	if (((c_point.xSearch+FreeImage_GetWidth(bitmap))>x_limit)||((c_point.ySearch+FreeImage_GetHeight(bitmap))>y_board))
	{
		return false;
	} 
	else
	{
		bool strip_flag=true;
		//判断是否与此空间内已经存入的纹理冲突
		for (size_t i=0;i<c_tex.size();i++)
		{
			if (((c_tex[i].xOrigin+c_tex[i].tWidth)>c_point.xSearch) && (c_tex[i].yOrigin<(c_point.ySearch+FreeImage_GetHeight(bitmap)))
				&& (c_tex[i].xOrigin<(c_point.xSearch+FreeImage_GetWidth(bitmap))) && ((c_tex[i].yOrigin+c_tex[i].tHeight)>c_point.ySearch))
			{
				strip_flag=false;
				break;
			}
		}
		return strip_flag;
	}
}

string TextureMerge::num2str(int i)
{
	stringstream ss;
	ss<<i;
	return ss.str();
}

void TextureMerge::Load(vector<string> inputImageName,vector<TEXBOUNDINGBOX> texBoundingBox,int r_width,int r_height)
{//1
	for (int i=0;i<inputImageName.size();i++)
	{//2
		//检查路径的正确性
		string stemp="";
		if(m_TexLoadPath!="")
		{//3
			if ((m_TexLoadPath[m_TexLoadPath.length()-1]=='\\'))
			{
				stemp=m_TexLoadPath+inputImageName[i];
			} 
			else
			{
				stemp=m_TexLoadPath+"\\"+inputImageName[i];	
			}
		}//3
		else stemp=(inputImageName[i]);
		
		FIBITMAP *dib=FreeImage_Load(FIF_DDS,stemp.c_str(),DDS_DEFAULT);
		//如果载入成功再进行以下操作
		if(dib)
		{//3

			//如果为截断模式(非重复模式)，则直接载入信息
			if (!texBoundingBox[i].is_repeat)
			{//4
				if (((ceil)(FreeImage_GetWidth(dib)*(texBoundingBox[i].x_max-texBoundingBox[i].x_min))<=r_width) 
					&& ((ceil)(FreeImage_GetHeight(dib)*(texBoundingBox[i].y_max-texBoundingBox[i].y_min))<=r_height))
				{
					ImageIN temp;
					temp.imgdib=dib;
					temp.imgindex=i;
					Image_Handled.push_back(temp);
					//FreeImage_Unload(dib);
				}
			}//4
			//重复模式
			else
			{//4
				//延展后图像符合输出要求，再进行以下操作
				if (!texBoundingBox[i].is_repeat_x)
				{
					texBoundingBox[i].x_max=1;
					texBoundingBox[i].x_min=0;
				}

				if (!texBoundingBox[i].is_repeat_y)
				{
					texBoundingBox[i].y_max=1;
					texBoundingBox[i].y_min=0;
				}
				if (((ceil)(FreeImage_GetWidth(dib)*(texBoundingBox[i].x_max-texBoundingBox[i].x_min))<=r_width) 
					&& ((ceil)(FreeImage_GetHeight(dib)*(texBoundingBox[i].y_max-texBoundingBox[i].y_min))<=r_height))
				{//5
					int re_width;
					int re_height;
					re_width=(ceil)(FreeImage_GetWidth(dib)*(texBoundingBox[i].x_max-texBoundingBox[i].x_min));
					re_height=(ceil)(FreeImage_GetHeight(dib)*(texBoundingBox[i].y_max-texBoundingBox[i].y_min));
					//将原图拓展
					FIBITMAP *new_dib=FreeImage_Allocate(re_width,re_height,32);

					if(!new_dib)
					{
						cerr<<"Load create error"<<endl;
						system("pause");
					}
					for (int i_x=0;i_x<re_width;i_x++)
					{
						for (int i_y=0;i_y<re_height;i_y++)
						{
							RGBQUAD rgbtemp;
							if(!FreeImage_GetPixelColor(dib,i_x%FreeImage_GetWidth(dib),i_y%FreeImage_GetHeight(dib),&rgbtemp))
							{
								cerr<<"Load getpixel error"<<endl;
								system("pause");
							}

							if(!FreeImage_SetPixelColor(new_dib,i_x,i_y,&rgbtemp))
							{
								cerr<<"Load setpixel error"<<endl;
								system("pause");
							}
						}
					}

					ImageIN temp;
					temp.imgdib=new_dib;
					temp.imgindex=i;
					Image_Handled.push_back(temp);
					
				}//5
				else
				{//5
					ImageIN temp;
					temp.imgdib=dib;
					temp.imgindex=i;
					Image_UnHandled.push_back(temp);
					cerr<<"too large to load"<<endl;
				}//5
			}//4
		}//3
		else
		{//3
			cerr<<"Load error"<<endl;
			system("pause");
		}//3
	}//2
}//1

void TextureMerge::Grouping(const vector<string >  &inputImageName,
			 const vector<TEXBOUNDINGBOX >  &texBoundingBox,
			 vector <vector<int> >  &groupingIndex,
			 vector<string>  &mergedImageName,
			 vector<OFFSET>  &offset)
{
	clock_t start,end;
	start=clock();
	
	//载入图像
	Load(inputImageName,texBoundingBox,rimg_width,rimg_height);
	
	for (size_t i=0;i<inputImageName.size();i++)
	{
		OFFSET offtemp;
		offset.push_back(offtemp);
	}

	//对载入的图像按高排序
	sort(Image_Handled.begin(),Image_Handled.end(),sortstandard);
	
	DouTex temp2Tex;
	OneTex temp1Tex;
	PointArray temp1Point;
	DouPointArray temp2Point;
	vector<int> tempgroupindex;
	
	Tex_info img1_info;
	Search_point point1_info;
	
	//如果存在载入图像
	if(Image_Handled.size()>=1)
	{

		//第一张图像的信息
		img1_info.xOrigin=0;
		img1_info.yOrigin=0;
		img1_info.tWidth=FreeImage_GetWidth(Image_Handled[0].imgdib);
		img1_info.tHeight=FreeImage_GetHeight(Image_Handled[0].imgdib);
		img1_info.nAtlas=0;
		
		temp1Tex.push_back(img1_info);
		temp2Tex.push_back(temp1Tex);
		TextureSet.push_back(temp2Tex);
		
		//由第一张图像得到的搜索点
		point1_info.xSearch=img1_info.tWidth;
		point1_info.ySearch=0;
		point1_info.IsUsed=false;
		temp1Point.push_back(point1_info);
		temp2Point.push_back(temp1Point);
		SearchPointSet.push_back(temp2Point);
		tempgroupindex.push_back(Image_Handled[0].imgindex);

		groupingIndex.push_back(tempgroupindex);
		
		//返回原图在新图中信息
		offset[Image_Handled[0].imgindex].is_rotate=false;
		offset[Image_Handled[0].imgindex].x_start=(float)(img1_info.xOrigin)/rimg_width;
		offset[Image_Handled[0].imgindex].y_start=(float)(rimg_height-img1_info.yOrigin-img1_info.tHeight)/rimg_height;
		offset[Image_Handled[0].imgindex].x_range=((float)(img1_info.tWidth))/rimg_width;
		offset[Image_Handled[0].imgindex].y_range=((float)(img1_info.tHeight))/rimg_height;
	}
	
	//依次对剩余图像处理
	for (size_t i=1;i<Image_Handled.size();i++)
	{
		//判断待插入纹理是否可以插入现有图层的变量
		bool IsInserted=false;
		size_t  atlas_i=0;
		while (atlas_i<TextureSet.size()&&(!IsInserted))
		{
			size_t strip_i=0;
			while (strip_i<TextureSet[atlas_i].size()&&(!IsInserted))
			{
				size_t point_i=0;
				
				while ((!IsInserted)&&(point_i<SearchPointSet[atlas_i][strip_i].size()))
				{
					if(!SearchPointSet[atlas_i][strip_i][point_i].IsUsed)
					{
						bool IsLastStrip=false;
						if((atlas_i<TextureSet.size()-1) && (strip_i==TextureSet[atlas_i].size()-1))
							IsLastStrip=true;
						if (check(Image_Handled[i].imgdib,TextureSet[atlas_i][strip_i],SearchPointSet[atlas_i][strip_i][point_i],rimg_width,rimg_height,IsLastStrip))
						{
							//如果可以插入现有纹理块，则添加信息
							IsInserted=true;
							Tex_info ttemp;
							Search_point ptemp1,ptemp2;

							ttemp.xOrigin=SearchPointSet[atlas_i][strip_i][point_i].xSearch;
							ttemp.yOrigin=SearchPointSet[atlas_i][strip_i][point_i].ySearch;
							SearchPointSet[atlas_i][strip_i][point_i].IsUsed=true;
							ttemp.tWidth=FreeImage_GetWidth(Image_Handled[i].imgdib);
							ttemp.tHeight=FreeImage_GetHeight(Image_Handled[i].imgdib);
							ttemp.nAtlas=i;

							TextureSet[atlas_i][strip_i].push_back(ttemp);

							ptemp1.xSearch=ttemp.xOrigin+ttemp.tWidth;
							ptemp1.ySearch=ttemp.yOrigin;
							ptemp1.IsUsed=false;

							ptemp2.xSearch=ttemp.xOrigin;
							ptemp2.ySearch=ttemp.yOrigin+ttemp.tHeight;
							ptemp2.IsUsed=false;

							SearchPointSet[atlas_i][strip_i].push_back(ptemp1);
							SearchPointSet[atlas_i][strip_i].push_back(ptemp2);

							groupingIndex[atlas_i].push_back(Image_Handled[i].imgindex);

							offset[Image_Handled[i].imgindex].is_rotate=false;
							offset[Image_Handled[i].imgindex].x_start=(float)(ttemp.xOrigin)/rimg_width;
							offset[Image_Handled[i].imgindex].y_start=(float)(rimg_height-ttemp.yOrigin-ttemp.tHeight)/rimg_height;
							offset[Image_Handled[i].imgindex].x_range=((float)(ttemp.tWidth))/rimg_width;
							offset[Image_Handled[i].imgindex].y_range=((float)(ttemp.tHeight))/rimg_height;
						}
					}
						point_i++;
					}
				strip_i++;
				}
			atlas_i++;
			}
			if (!IsInserted)
			{
				//如果不能插入现有纹理块，则判断应添加新图像还是在现有图像中添加纹理层
				int col_instant=0;
				for (int it=0;it<TextureSet[TextureSet.size()-1].size();it++)
				{
					col_instant +=TextureSet[TextureSet.size()-1][it][0].tHeight;
				}
				if ((col_instant+FreeImage_GetHeight(Image_Handled[i].imgdib))>rimg_height)
				{
					Tex_info ttemp;
					Search_point ptemp1,ptemp2;
					vector<int> gtemp;
					ttemp.xOrigin=0;
					ttemp.yOrigin=0;
					ttemp.nAtlas=i;
					ttemp.tWidth=FreeImage_GetWidth(Image_Handled[i].imgdib);
					ttemp.tHeight=FreeImage_GetHeight(Image_Handled[i].imgdib);

					ptemp1.xSearch=ttemp.tWidth;
					ptemp1.ySearch=0;
					ptemp1.IsUsed=false;
					ptemp2.xSearch=0;
					ptemp2.ySearch=TextureSet[TextureSet.size()-1][TextureSet[TextureSet.size()-1].size()-1][0].tHeight+TextureSet[TextureSet.size()-1][TextureSet[TextureSet.size()-1].size()-1][0].yOrigin;
					ptemp2.IsUsed=false;
					SearchPointSet[SearchPointSet.size()-1][SearchPointSet[SearchPointSet.size()-1].size()-1].push_back(ptemp2);

					OneTex textemp1;
					DouTex textemp2;
					PointArray pointtemp1;
					DouPointArray pointtemp2;

					textemp1.push_back(ttemp);
					textemp2.push_back(textemp1);
					TextureSet.push_back(textemp2);

					pointtemp1.push_back(ptemp1);
					pointtemp2.push_back(pointtemp1);
					SearchPointSet.push_back(pointtemp2);

					gtemp.push_back(Image_Handled[i].imgindex);
					groupingIndex.push_back(gtemp);

					offset[Image_Handled[i].imgindex].is_rotate=false;
					offset[Image_Handled[i].imgindex].x_start=(float)(ttemp.xOrigin)/rimg_width;
					offset[Image_Handled[i].imgindex].y_start=(float)(rimg_height-ttemp.yOrigin-ttemp.tHeight)/rimg_height;
					offset[Image_Handled[i].imgindex].x_range=((float)(ttemp.tWidth))/rimg_width;
					offset[Image_Handled[i].imgindex].y_range=((float)(ttemp.tHeight))/rimg_height;
				}
				else
				{
					Tex_info ttemp;
					Search_point ptemp;

					ttemp.xOrigin=0;
					ttemp.yOrigin=TextureSet[TextureSet.size()-1][TextureSet[TextureSet.size()-1].size()-1][0].tHeight+TextureSet[TextureSet.size()-1][TextureSet[TextureSet.size()-1].size()-1][0].yOrigin;
					ttemp.nAtlas=i;
					ttemp.tWidth=FreeImage_GetWidth(Image_Handled[i].imgdib);
					ttemp.tHeight=FreeImage_GetHeight(Image_Handled[i].imgdib);

					ptemp.xSearch=ttemp.tWidth;
					ptemp.ySearch=ttemp.yOrigin;
					ptemp.IsUsed=false;

					OneTex textemp;
					PointArray pointtemp;

					textemp.push_back(ttemp);
					TextureSet[TextureSet.size()-1].push_back(textemp);

					pointtemp.push_back(ptemp);
					SearchPointSet[SearchPointSet.size()-1].push_back(pointtemp);

					groupingIndex[groupingIndex.size()-1].push_back(Image_Handled[i].imgindex);

					offset[Image_Handled[i].imgindex].is_rotate=false;
					offset[Image_Handled[i].imgindex].x_start=(float)(ttemp.xOrigin)/rimg_width;
					offset[Image_Handled[i].imgindex].y_start=(float)(rimg_height-ttemp.yOrigin-ttemp.tHeight)/rimg_height;
					offset[Image_Handled[i].imgindex].x_range=((float)(ttemp.tWidth))/rimg_width;
					offset[Image_Handled[i].imgindex].y_range=((float)(ttemp.tHeight))/rimg_height;
				}
			}

		}

		//返回输出图像名称
		for (int it_tex=0;it_tex<TextureSet.size();it_tex++)
		{
			mergedImageName.push_back(m_TexSaveName+num2str(it_tex)+".png");
		}
		for(int i=0;i<Image_UnHandled.size();i++)
		{
			mergedImageName.push_back(inputImageName[Image_UnHandled[i].imgindex]);

			vector<int> gtemp;
			gtemp.push_back(Image_UnHandled[i].imgindex);
			groupingIndex.push_back(gtemp);
		}
		
	end=clock();
	double duration = static_cast<double>(end - start) / CLOCKS_PER_SEC;
	cout << "Time: " << duration << " seconds" << endl; 
	//cerr<<(float)(end-start)/CLOCKS_PER_SEC<<endl;	
}

void TextureMerge::Save()
{
	//按顺序保存图像
	for (int it_tex=0;it_tex<TextureSet.size();it_tex++)
	{
		FIBITMAP *resmap=FreeImage_Allocate(rimg_width,rimg_height,32);
		for(size_t i=0;i<TextureSet[it_tex].size();i++)
			for(size_t j=0;j<TextureSet[it_tex][i].size();j++)
				for(int it_x=0;it_x<FreeImage_GetWidth(Image_Handled[TextureSet[it_tex][i][j].nAtlas].imgdib);it_x++)
					for(int it_y=0;it_y<FreeImage_GetHeight(Image_Handled[TextureSet[it_tex][i][j].nAtlas].imgdib);it_y++)
					{
						RGBQUAD rgbtemp;
						if(!FreeImage_GetPixelColor(Image_Handled[TextureSet[it_tex][i][j].nAtlas].imgdib,it_x,FreeImage_GetHeight(Image_Handled[TextureSet[it_tex][i][j].nAtlas].imgdib)-it_y-1,&rgbtemp))
						{
							cerr<<"Save getpixel error"<<endl;
							system("pause");
						}
						if(!FreeImage_SetPixelColor(resmap,TextureSet[it_tex][i][j].xOrigin+it_x,TextureSet[it_tex][i][j].yOrigin+it_y,&rgbtemp))
						{
							cerr<<"Save setpixel error"<<endl;
							system("pause");
						}
					}
					string resname;
					resname=m_TexSaveName+num2str(it_tex)+".png";
					if(m_TexSavePath!="")
					{

						if ((m_TexSavePath[m_TexSavePath.length()-1]=='\\') )
						{
							resname=m_TexSavePath+resname;
						} 
						else
						{
							resname=m_TexSavePath+"\\"+resname;
							
						}
					}
					
					if(!FreeImage_Save(FIF_PNG,resmap,resname.c_str(),PNG_DEFAULT))
					{
						cerr<<"save error"<<endl;
						system("pause");
					}
		FreeImage_Unload(resmap);
	}
}