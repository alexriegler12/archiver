#include <stdio.h>
#include "port.h"
struct archive{
	FILE* arcfile;

};
#pragma pack(push,1)
struct archeader{
	U_CHAR id[4];
	U_INT32 numfiles;
};
struct fileheader{
	U_CHAR id[4];
	U_INT32 filelen;
	U_CHAR namelen;
};
#pragma pack(pop)
struct entry{
	char *fname;
	int size;
};
struct archive* create_archive(U_CHAR* path);
U_INT32 add_archive(struct archive* arc,U_CHAR* fname);
struct archive* open_existing_archive(U_CHAR* path);
U_INT32 close_archive(struct archive* arc);
U_INT32 extract_allFiles(struct archive* arc,U_CHAR* path);


