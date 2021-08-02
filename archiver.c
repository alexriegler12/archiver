#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "port.h"
#include "archiver.h"
U_INT32 swapu32( U_INT32 val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}
U_CHAR* getFnameWoPath(U_CHAR* fname){
	U_CHAR* name;
	name=strrchr(fname,DIRSEP);
	if(name==NULL){
		return fname;
	}else{
		return name+1;
	}
}
U_INT32 loadHeader(FILE* fil,struct archeader* hd){
	U_INT32 size;
	fread(hd->id,4,1,fil);
	fread(&size,4,1,fil);
	#ifdef BIG_EN
	hd->numfiles=swapu32(size);
	#else
	hd->numfiles=size;
	#endif
	return 0;
}
U_INT32 saveHeader(FILE* fil, struct archeader* hd){
	U_INT32 size;
	fwrite(hd->id,4,1,fil);
	#ifdef BIG_EN
	size=swapu32(hd->numfiles);
	#else
	size=hd->numfiles;
	#endif
	fwrite(&size,4,1,fil);
	return 0;

}
U_INT32 loadFileHeader(FILE* fil, struct fileheader* hd){
	U_INT32 size;
	fread(hd->id,4,1,fil);
	fread(&size,4,1,fil);
	#ifdef BIG_EN
	hd->filelen=swapu32(size);
	#else
	hd->filelen=size;
	#endif
	fread(&hd->namelen,1,1,fil);
	return 0;

}
U_INT32 saveFileHeader(FILE* fil, struct fileheader* hd){
	U_INT32 size;
	fwrite(hd->id,4,1,fil);
	#ifdef BIG_EN
	size=swapu32(hd->filelen);
	#else
	size=hd->filelen;
	#endif
	fwrite(&size,4,1,fil);

	fwrite(&hd->namelen,1,1,fil);
	return 0;

}

struct archive* create_archive(U_CHAR* path){
	struct archive* arc;
	struct archeader hd;
	arc=(struct archive*)malloc(sizeof(struct archive));
	arc->arcfile=fopen(path,"w+b");
	hd.id[0]='A';
	hd.id[1]='R';
	hd.id[2]='C';
	hd.id[3]='H';
	hd.numfiles=0;
	/*fwrite(&hd,sizeof(struct archive),1,arc->arcfile);*/
	saveHeader(arc->arcfile,&hd);
	return arc;

}
U_INT32 add_archive(struct archive* arc,U_CHAR* fname){
	FILE* fil;
	U_CHAR *nam;
	U_CHAR buf;
	struct archeader hd;
	struct fileheader fh;
	U_INT32 fsize;
	fseek(arc->arcfile,0,SEEK_SET);
	/*fread(&hd,sizeof(struct archeader),1,arc->arcfile);*/
	loadHeader(arc->arcfile,&hd);
	fseek(arc->arcfile,0,SEEK_SET);
	hd.numfiles++;
	/*fwrite(&hd,sizeof(struct archeader),1,arc->arcfile);*/
	saveHeader(arc->arcfile,&hd);
	fseek(arc->arcfile,0,SEEK_END);
	fil=fopen(fname,"rb");
	fh.id[0]='A';
	fh.id[1]='F';
	fh.id[2]='I';
	fh.id[3]='L';
	fseek(fil, 0, SEEK_END);
	fsize = ftell(fil);
	fseek(fil,0,SEEK_SET);
	fh.filelen=fsize;
	nam=getFnameWoPath(fname);
	fh.namelen=strlen(nam);
	/*fwrite(&fh,sizeof(struct fileheader),1,arc->arcfile);*/
	saveFileHeader(arc->arcfile,&fh);
	fwrite(nam,strlen(nam),1,arc->arcfile);
	while(fread(&buf,1,1,fil)==1){
		fwrite(&buf,1,1,arc->arcfile);
	}
	fclose(fil);



}
U_INT32 extract_allFiles(struct archive* arc,U_CHAR* path){
	U_CHAR filePath[1024];
	U_INT32 pathlen;
	U_INT32 i,j;
	U_CHAR buf;
	FILE* extract;
	U_CHAR nambuf[257];
	struct archeader hd;
	struct fileheader fh;
	fseek(arc->arcfile,0,SEEK_SET);
	/*fread(&hd,sizeof(struct archeader),1,arc->arcfile);*/
	loadHeader(arc->arcfile,&hd);
	/*printf("%i\n",hd.numfiles);*/
	for(i=0;i<hd.numfiles;i++){
		/*printf("%i\n",i);*/
		memset(filePath,0,1024);
		/*fread(&fh,sizeof(struct fileheader),1,arc->arcfile);*/
		loadFileHeader(arc->arcfile,&fh);
		fread(nambuf,1,fh.namelen,arc->arcfile);
		nambuf[fh.namelen]=0;
		strcat(filePath,path);
		pathlen=strlen(path);
		if(path[pathlen-1]!=DIRSEP){
			filePath[pathlen]=DIRSEP;
			filePath[pathlen+1]=0;
		}
		strcat(filePath,nambuf);
		/*printf("Name: %s\n",filePath);*/
		extract=fopen(filePath,"wb");
		for(j=0;j<fh.filelen;j++){
			fread(&buf,1,1,arc->arcfile);
			fwrite(&buf,1,1,extract);
		}
		fclose(extract);

	}
	/*printf("End\n");*/
}
struct archive* open_existing_archive(U_CHAR* path){
	struct archive* arc;
	struct archeader hd;
	arc=(struct archive*)malloc(sizeof(struct archive));
	arc->arcfile=fopen(path,"r+b");
	return arc;

}
U_INT32 close_archive(struct archive* arc){
	fclose(arc->arcfile);
	free(arc);
}

