#include <stdio.h>
#include "archiver.h"
int createArchive(int argc,char **argv){
	U_INT32 i;
	struct archive *arc;
	arc=create_archive(argv[2]);
	for(i=3;i<argc;i++){
		printf("adding \"%s\"\n",argv[i]);
		add_archive(arc,argv[i]);
	}
	close_archive(arc);

}
int extractAll(int argc,char** argv){
	struct archive *arc;
	arc=open_existing_archive(argv[2]);
	extract_allFiles(arc,argv[3]);
}
int main(int argc,char **argv){
	if(argc<2){
		printf("Error: Too few Arguments");
		return 1;
	}
	switch(argv[1][0]){
		case 'c':
			createArchive(argc,argv);
			break;
		case 'a':
			extractAll(argc,argv);
	}
	return 0;

}
