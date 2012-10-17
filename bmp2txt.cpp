#include "defs.hpp"

int main(int argc, char* argv[]) {
	if(argc != 3) {
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

	BITMAPINFOHEADER *bih = new BITMAPINFOHEADER;
	BITMAPFILEHEADER *bfh = new BITMAPFILEHEADER;
	memset(bih, 0, sizeof(BITMAPINFOHEADER));
	memset(bfh, 0, sizeof(BITMAPFILEHEADER));

	if(!fread(bfh, sizeof(BITMAPFILEHEADER), 1, fIO) || !fread(bih, sizeof(BITMAPINFOHEADER), 1, fIO)) {
		printf("\nCould not read bitmap headers from %s.", argv[2]);
		return 1;
	}

	if(bfh->bfType != '\x42\x4d') {
		printf("\n%s is not a valid bmp image.", argv[2]);
		return 1;
	}

	if(bih->biCompression != BI_RGB || bih->biPlanes != 1) {
		printf("\n%s is not an uncompressed bmp image.", argv[2]);
		return 1;
	}
	// Each bitmap row must have a number of bytes divisible by 4. If not, padding must be appended to each row.
	DWORD dwPadding;
	if((bih->biWidth * (bih->biBitCount / 8)) % 4 != 0) {
		dwPadding = 4 - (bih->biWidth * (bih->biBitCount / 8) % 4);
	}
	// (height * bpp) * (width * bpp) = total number of pixels
	char* pPixelArray = new char[(bih->biHeight * (bih->biBitCount / 8)) * (bih->biWidth * (bih->biBitCount / 8))];
	
	for(DWORD i = 0, dwCurrent = 0; i < bih->biHeight; ++i) {
		if(!fread(&pPixelArray[dwCurrent], bih->biWidth * (bih->biBitCount / 8), 1, fIO)) {
			printf("\nCould not read from %s.", argv[2]);
			return 1;
		}
		fseek(fIO, dwPadding, SEEK_CUR);
		dwCurrent += bih->biWidth;
	}
	fclose(fIO);

	fIO = fopen(argv[1], "wb+");
	if(fIO == NULL) {
		printf("\nCould not create %s.", argv[1]);
		return 1;
	}

	if(!fwrite(pPixelArray, 1, strlen(pPixelArray), fIO)) {
		printf("\nCould not write to %s.", argv[1]);
		return 1;
	}

	fclose(fIO);
	delete[] pPixelArray;
	return 0;
}