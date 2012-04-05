// extradnf.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <boost/filesystem.hpp>

#include "zlib.h"
#pragma comment(lib, "zlib.lib")

#include "FreeImage.h"
#pragma comment(lib, "FreeImage.lib")


void extract_npk(const char* file);
void extract_img(const char* file);
void extract_image(const char* file);
void convert_to_png(const char* file_name, int width, int height, int type, unsigned char* data, int size);

#pragma warning(disable:4996)

struct NPK_Header
{
    char flag[16]; // �ļ���ʶ "NeoplePack_Bill"
    int count;     // �����ļ�����Ŀ
};

struct NPK_Index
{
    unsigned int offset;  // �ļ��İ���ƫ����
    unsigned int size;    // �ļ��Ĵ�С
    char name[256];// �ļ���
};

char decord_flag[256] = "puchikon@neople dungeon and fighter DNF";

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
	int max_height;   // ��ͼ�ĸ߶ȣ��д�������Ϊ�˶��뾫��
};

int _tmain(int argc, _TCHAR* argv[])
{
	int xx = sizeof(decord_flag);
	int len = strlen(decord_flag);
	for (int i = len; i < 256; ++i) {
		
		if ((i - len) % 3 == 0) {
			decord_flag[i] = 'D';
		} else if ((i - len) % 3 == 1) {
			decord_flag[i] = 'N';
		} else if ((i - len) % 3 == 2) {
			decord_flag[i] = 'F';
		}
	}
	decord_flag[sizeof(decord_flag) - 1] = 0;

	//extract_npk("sprite_common_commoneffect_cure.NPK");
	extract_img("back.img");

	system("PAUSE");
	return 0;
}

void extract_npk(const char* file)
{
	FILE* fp = fopen(file, "rb");
	if (!fp) {
		printf("error %s not found!!", file);
	}
	NPK_Header header;
	fread(header.flag, 16, 1, fp);
	fread(&header.count, 4, 1, fp);

	std::vector<NPK_Index> all_file_index;
	for (int i = 0; i < header.count; ++i) {
		NPK_Index index;
		fread(&index.offset, 4, 1, fp);
		fread(&index.size, 4, 1, fp);

		char temp[256] = {0};
		fread(temp, 256, 1, fp);
		for (int i = 0; i < 256; ++i) {
			index.name[i] = temp[i] ^ decord_flag[i];
		}
		all_file_index.push_back(index);
	}

	for (std::vector<NPK_Index>::const_iterator itr = all_file_index.begin();
		itr != all_file_index.end(); ++itr ) {
		NPK_Index index = *itr;
		//fseek();
//		printf("%s\n", index.name);
		fseek(fp, index.offset, SEEK_SET);
		char* temp = new char[index.size];
		memset(temp, 0, index.size);
		fread(temp, index.size, 1, fp);

		// �����ļ���
		boost::filesystem3::path file_path(index.name);
		file_path.remove_filename();
		boost::filesystem3::create_directories(file_path);

		// дimg�ļ�
		FILE* fpw = fopen(index.name, "wb");
		fwrite(temp, index.size, 1, fpw);
		fclose(fpw);
		delete[] temp;

		// ��ѹimg�ļ�
		extract_img(index.name);
	}

	fclose(fp);
}

void test_zlib()
{
		// test zlib
	 //ԭʼ���� 
     const unsigned char strSrc[]="hello world!/n/\
aaaaa bbbbb ccccc ddddd aaaaa bbbbb ccccc ddddd���Ĳ��� ���Ĳ���/\
aaaaa bbbbb ccccc ddddd aaaaa bbbbb ccccc ddddd���Ĳ��� ���Ĳ���/\
aaaaa bbbbb ccccc ddddd aaaaa bbbbb ccccc ddddd���Ĳ��� ���Ĳ���/\
aaaaa bbbbb ccccc ddddd aaaaa bbbbb ccccc ddddd���Ĳ��� ���Ĳ���";

     unsigned char buf[1024]={0},strDst[1024]={0};
     unsigned long srcLen=sizeof (strSrc),bufLen=sizeof (buf),dstLen=sizeof (strDst);

     //ѹ�� 
      compress(buf,&bufLen,strSrc,srcLen);
      printf("After Compressed Length:%d\n",bufLen);
     //��ѹ�� 
      int xrxr = uncompress(strDst,&dstLen,buf,bufLen);
      printf("UnCompressed String:%s\n",strDst);
}

void extract_img(const char* file)
{
	FILE* fp = fopen(file, "rb");
	if (!fp) {
		printf("error %s not found!!", file);
	}
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

		// ռλ�ļ����������������ݣ���ʵ��ͼƬ����
		if (index.dwType == 0x11) {
			all_file_index.push_back(index);
			continue;;
		}
		fread(&index.width, 4, 1, fp);
		fread(&index.height, 4, 1, fp);
		fread(&index.size, 4, 1, fp);
		fread(&index.key_x, 4, 1, fp);
		fread(&index.key_y, 4, 1, fp);
		fread(&index.max_width, 4, 1, fp);
		fread(&index.max_height, 4, 1, fp);
		all_file_index.push_back(index);
	}

	const int buffer_size = 1024 * 1024;
	unsigned char* temp_zlib_data = new unsigned char[buffer_size];
	
	// �����ļ�ͷ��������
	fseek(fp, header.index_size + 32, SEEK_SET);

	for (std::vector<NImgF_Index>::const_iterator itr = all_file_index.begin();
		itr != all_file_index.end(); ++itr) {
		NImgF_Index index = *itr;
		printf("%d  type:%d   compress:%d \n", itr - all_file_index.begin(), index.dwType, index.dwCompress);

		// ռλ�ļ�����ʵ����Դ
		if (index.dwType == 0x11) {
			continue;
		}

		unsigned char* temp = new unsigned char[index.size];
		memset(temp, 0, index.size);
		fread(temp, index.size, 1, fp);

		unsigned char temptemp[256] = {0};
		memcpy(temptemp, temp, index.size);
		unsigned long zlib_len = buffer_size;

		memset(temp_zlib_data, 0, buffer_size);
		int ret = uncompress(temp_zlib_data, &zlib_len, temp, index.size); //uncompress(temp_zlib_data, &zlib_len, temp, index.size);
		delete[] temp;

		if (ret != Z_OK) {
			printf("compress %s %d error!\n", file, itr - all_file_index.begin());
			continue;;
		}

		// �����ļ���
		std::string file_path_noextern(file);
		file_path_noextern = file_path_noextern.substr(0, file_path_noextern.find_last_of('.'));
		boost::filesystem3::path file_path(file_path_noextern);
		boost::filesystem3::create_directories(file_path);

		char file_name[256] = {0};
		_snprintf(file_name, sizeof(file_name) - 1, "%s/%d.dip", file_path_noextern.c_str(), itr - all_file_index.begin());
		convert_to_png(file_name, index.width, index.height, index.dwType, temp_zlib_data, zlib_len);
	}

	fclose(fp);
}

void convert_to_png(const char* file_name, int width, int height, int type, unsigned char* data, int size)
{
	const int buffer_size = 1024 * 1024 * 5;
	static unsigned char s_png_temp_data[buffer_size];
	memset(s_png_temp_data, 0, buffer_size);

	unsigned char* dest_pointer = s_png_temp_data;
	unsigned char* src_pointer = data;
	
	switch (type)
	{
		case 0x0e://1555
			{
				for (int i = 0; i < height; ++i) {
					for (int j = 0; j < width; ++j) {
						dest_pointer[0] = src_pointer[0] & 0x001f;
						dest_pointer[1] = ((src_pointer[0] >> 5) & 0x0007) | ((src_pointer[1] & 0x0003) << 2);
						dest_pointer[2] = (src_pointer[1] >> 2) & 0x001f;
						dest_pointer[3] = (src_pointer[1] >> 7) == 0 ? 0 : 255;
						dest_pointer += 4;
						src_pointer += 2;
					}
				}
			}
			break;
		case 0x0f://4444
			{
				for (int i = 0; i < height; ++i) {
					for (int j = 0; j < width; ++j) {
						dest_pointer[0] = src_pointer[0] & 0x0f;
						dest_pointer[1] = (src_pointer[0] & 0xf0) >> 4;
						dest_pointer[2] = src_pointer[1] & 0x0f;
						dest_pointer[3] = (src_pointer[1] & 0xf0) >> 4;
						dest_pointer += 4;
						src_pointer += 2;
					}
				}
			}
			break;
		case 0x10://8888
			{
				for (int i = 0; i < height; ++i) {
					for (int j = 0; j < width; ++j) {
						dest_pointer[0] = src_pointer[0];
						dest_pointer[1] = src_pointer[1];
						dest_pointer[2] = src_pointer[2];
						dest_pointer[3] = src_pointer[3];
						dest_pointer += 4;
						src_pointer += 4;
					}
				}
			}
			break;
		case 0x11:// ռλ����ͼƬ��Դ
			break;
		default:
			printf("error known type:%d\n", type);
			break;
	}
	
	FIMEMORY* memory = FreeImage_OpenMemory(s_png_temp_data, height * width * 4);
	FIBITMAP* bmpData = FreeImage_LoadFromMemory(FIF_PNG, memory);
	FreeImage_Save(FIF_PNG, bmpData, file_name);
	FreeImage_Unload(bmpData);
}