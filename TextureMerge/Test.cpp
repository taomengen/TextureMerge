#include "TextureMerge.h"
#include <io.h>
using namespace std;

void getFiles(string path, vector<string> &files)
{
	long hFile = 0;
	struct _finddata_t fileinfo;
	string str;
	if((hFile = _findfirst(str.assign(path).append("\\*").c_str(),&fileinfo)) != -1)
	{
		do
		{
			if(fileinfo.attrib & _A_SUBDIR)
			{
				if(strcmp(fileinfo.name,".") != 0 && strcmp(fileinfo.name,"..") != 0)
				{
					getFiles(str.assign(path).append("\\").append(fileinfo.name),files);
				}
			}
			else
			{
				files.push_back(str.assign(path).append("\\").append(fileinfo.name));
			}

		}while(_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
	}
}

int main()
{
	char * filePath = "..\\dds合并\\dds";
	vector<string> imagenamelist;
	vector<TEXBOUNDINGBOX> tex;
	getFiles(filePath,imagenamelist);
	for(size_t i = 0;i != imagenamelist.size();++i)
	{
		cout << imagenamelist.at(i) << endl;
	}
	for(size_t i = 0;i != imagenamelist.size();++i)
	{
		TEXBOUNDINGBOX ttt;
		tex.push_back(ttt);
	}
	vector<vector<int>> gindex;
	vector<string> mname;
	vector<OFFSET> offset;
	TextureMerge texture;

	texture.rimg_height = 1024;
	texture.rimg_width = 1024;
	texture.SetTexSavePath("..\\dds合并\\png");
	texture.Grouping(imagenamelist,tex,gindex,mname,offset);
	cerr << mname.size() << endl;
	texture.Save();
	return 0;
}