// extradnf.cpp : Defines the entry point for the console application.
//
#pragma warning(disable:4996)
#include "stdafx.h"
#include <vector>

struct NImgF_Header
{
	char flag[16]; // �ļ���ʯ"Neople Img File"
	int index_size;	// �������С�����ֽ�Ϊ��λ
	int unknown1;
	int unknown2;
	int index_count;// ��������Ŀ
};

struct NImgF_Index
{
	unsigned int dwType; //Ŀǰ��֪�������� 0x0E(1555��ʽ) 0x0F(4444��ʽ) 0x10(8888��ʽ) 0x11(�������κ����ݣ�������ָ����ͬ��һ֡)
	unsigned int dwCompress; // Ŀǰ��֪�������� 0x06(zlibѹ��) 0x05(δѹ��)
	int width;        // ���
	int height;       // �߶�
	int size;         // ѹ��ʱsizeΪѹ�����С��δѹ��ʱsizeΪת����8888��ʽʱռ�õ��ڴ��С
	int key_x;        // X�ؼ��㣬��ǰͼƬ����ͼ�е�X����
	int key_y;        // Y�ؼ��㣬��ǰͼƬ����ͼ�е�Y����
	int max_width;    // ��ͼ�Ŀ��
	int max_height;   // ��ͼ�ĸ߶ȣ��д�������Ϊ�˶��侫��
};

int _tmain(int argc, _TCHAR* argv[])
{
	FILE* fp = fopen("1-dodge.img", "rb");
	NImgF_Header header;
	fread(header.flag, 16, 1, fp);
	fread(&header.index_size, 4, 1, fp);
	fread(&header.unknown1, 4, 1, fp);
	fread(&header.unknown2, 4, 1, fp);
	fread(&header.index_count, 4, 1, fp);

	std::vector<NImgF_Index> all_file_index;
	for (int i = 0; i < header.index_count; ++i) {
		NImgF_Index index;
		fread(&index.dwType, 4, 1, fp);
		fread(&index.dwCompress, 4, 1, fp);
		fread(&index.width, 4, 1, fp);
		fread(&index.height, 4, 1, fp);
		fread(&index.size, 4, 1, fp);
		fread(&index.key_x, 4, 1, fp);
		fread(&index.key_y, 4, 1, fp);
		fread(&index.max_width, 4, 1, fp);
		fread(&index.max_height, 4, 1, fp);
		all_file_index.push_back(index);
	}

	NImgF_Index _1stIndex = all_file_index.front();
	fseek(fp, header.index_size, SEEK_SET);
	char* data = new char[_1stIndex.size];
	fread(data, _1stIndex.size, 1, fp);

	FILE* fpw = fopen("111.zip", "wb");
	fwrite(data, _1stIndex.size, 1, fpw);
	fclose(fpw);
	fclose(fp);
	return 0;
}

