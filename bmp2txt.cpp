#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <WinGDI.h>

int main(int argc, char* argv[]) {
/* 
	Format for input: output input
					  out.txt
							 in.bmp
	Etc, etc. Only 24 bit bitmaps accepted atm (sry, too lazy
												and this was actually
												for a project for 
												someone speshul.
												might add later)
	Strips padding and outputs byte array.
*/
	if(argc < 3) {
		char szExeName[256] = {0};
		int endPos = strlen(argv[0]);
		for(int i = endPos; i > 0; --i) {
			if(argv[0][i] == '\\') {
				endPos = ++i;
				break;
				
			}
		}
		strcpy(szExeName, &argv[0][endPos]);

		printf("bmp2txt by {im}plicit\n\nUsage: %s out.txt in.bmp\n\n\x9out.txt - output file\n\x9in.bmp  - image source", szExeName);
		return 1;
	}

	FILE *fIO = fopen(argv[2], "rb+");
	if(fIO == NULL) {
		printf("\nCould not open %s.", argv[2]);
		return 1;
	}
 // populate bitmap header structs
	BITMAPINFOHEADER *bih = new BITMAPINFOHEADER;
	BITMAPFILEHEADER *bfh = new BITMAPFILEHEADER;
	ZeroMemory(bih, sizeof(BITMAPINFOHEADER));
	ZeroMemory(bfh, sizeof(BITMAPFILEHEADER));

	if(!fread(bfh, sizeof(BITMAPFILEHEADER), 1, fIO)) {
		printf("\nCould not read from %s.", argv[2]);
		return 1;
	}
	if(!fread(bih, sizeof(BITMAPINFOHEADER), 1, fIO)) {
		printf("\Could not read from %s.", argv[2]);
		return 1;
	}

 // check for bitmap validity
	if(bfh->bfType != '\x42\x4d') {
		printf("\n%s is not a valid bmp image.", argv[2]);
		return 1;
	}
	if(bih->biBitCount != 24 || bih->biCompression != BI_RGB || bih->biPlanes != 1) {
		printf("\n%s is not a 24-bit uncompressed bmp image.", argv[2]);
		return 1;
	}
 // read the image into our buffer
	// i started thinking about future compatibility here
	// so some calculations may seem unnecesary but will let me update later
	char* pPixelArray = new char[(bih->biHeight * (bih->biBitCount / 8)) * (bih->biWidth * (bih->biBitCount / 8))];
 // calculate the padding after each row
	DWORD dwPitch = bih->biWidth * (bih->biBitCount / 8);
	if(dwPitch % 4 != 0) {
		dwPitch += 4 - (dwPitch % 4);
	}
	DWORD dwPadding = dwPitch - bih->biWidth * (bih->biBitCount / 8);
	DWORD dwCurrent = 0;
 // now read the file line by line
	for(DWORD i = 0; i < bih->biHeight; ++i) {
		if(!fread(&pPixelArray[dwCurrent], bih->biWidth * (bih->biBitCount / 8), 1, fIO)) {
			printf("\nCould not read from %s.", argv[2]);
			return 1;
		}
		fseek(fIO, dwPadding, SEEK_CUR);
		dwCurrent += bih->biWidth;
	}
	fclose(fIO);
 // write the output file
	fIO = fopen(argv[1], "wb+");
	if(fIO == NULL) {
		printf("\nCould not create %s.", argv[1]);
		return 1;
	}

	if(!fwrite(pPixelArray, 1, strlen(pPixelArray), fIO)) {
		printf("\nCould not write to %s.", argv[1]);
		return 1;
	}
 // cleanup
	fclose(fIO);
	delete[] pPixelArray;
	return 0;
}