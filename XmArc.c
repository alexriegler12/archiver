#include <stdio.h>
#include <Xm/XmAll.h>
#include "archiver.h"
Widget toplevel,mainW,menubar,list,filemenu,editmenu,opendialog,extractdialog,adddialog,newdialog;
XtAppContext     app;
Arg args[5];
unsigned char** fileList;
struct archive* arc;
int fileNum;
int fillList();
void open_cb(Widget w, XtPointer client_data, XtPointer call_data){
	char* filename;
	XmFileSelectionBoxCallbackStruct *cbs;
	cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
	filename = XmStringUnparse (cbs->value, XmFONTLIST_DEFAULT_TAG,XmCHARSET_TEXT, XmCHARSET_TEXT, NULL,0, XmOUTPUT_ALL);
	arc=open_existing_archive(filename);
	fileList=getFileList(arc,&fileNum);
	/*printf("%c\n",fileList[0][0]);*/
	fillList();
	XtUnmanageChild(w);
}
void new_cb(Widget w, XtPointer client_data, XtPointer call_data){
	char* filename;
	XmFileSelectionBoxCallbackStruct *cbs;
	cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
	filename = XmStringUnparse (cbs->value, XmFONTLIST_DEFAULT_TAG,XmCHARSET_TEXT, XmCHARSET_TEXT, NULL,0, XmOUTPUT_ALL);
	arc=create_archive(filename);
	fileList=getFileList(arc,&fileNum);
	/*printf("%c\n",fileList[0][0]);*/
	fillList();
	XtUnmanageChild(w);
}

void open_exit_cb(Widget w, XtPointer client_data, XtPointer call_data){
	XtUnmanageChild(w);
}

void filehandler (Widget w, XtPointer client_data, XtPointer call_data)
{

	int item_no = (int) client_data;
	switch(item_no){
		case 0:
			/*XtSetArg(args[0],XmNpathMode, XmPATH_MODE_FULL);*/
			opendialog=XmCreateFileSelectionDialog (mainW, "Open archive", NULL,0);
			XtAddCallback (opendialog, XmNokCallback, open_cb,NULL);
			XtAddCallback (opendialog, XmNcancelCallback, open_exit_cb,NULL);
			XtManageChild (opendialog);
			break;
		case 1:
			newdialog=XmCreateFileSelectionDialog (mainW, "New archive", NULL,0);
			XtAddCallback (newdialog, XmNokCallback, new_cb,NULL);
			XtAddCallback (newdialog, XmNcancelCallback, open_exit_cb,NULL);
			XtManageChild (newdialog);

			break;
		case 2:
			exit (0);
			break;
	}
    		
}
void extract_cb(Widget w, XtPointer client_data, XtPointer call_data){
	char* filename;
	XmFileSelectionBoxCallbackStruct *cbs;
	cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
	filename = XmStringUnparse (cbs->value, XmFONTLIST_DEFAULT_TAG,XmCHARSET_TEXT, XmCHARSET_TEXT, NULL,0, XmOUTPUT_ALL);
	extract_allFiles(arc,filename);
	XtUnmanageChild(w);

}
void add_cb(Widget w, XtPointer client_data, XtPointer call_data){
	char* filename;
	XmFileSelectionBoxCallbackStruct *cbs;
	cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
	filename = XmStringUnparse (cbs->value, XmFONTLIST_DEFAULT_TAG,XmCHARSET_TEXT, XmCHARSET_TEXT, NULL,0, XmOUTPUT_ALL);
	add_archive(arc,filename);
	XtUnmanageChild(w);
	fileList=getFileList(arc,&fileNum);
	/*printf("%c\n",fileList[0][0]);*/
	fillList();

}

void edithandler (Widget w, XtPointer client_data, XtPointer call_data)
{
	int item_no = (int) client_data;
	switch(item_no){
		case 0:
			adddialog=XmCreateFileSelectionDialog (mainW, "New archive", NULL,0);
			XtAddCallback (adddialog, XmNokCallback, add_cb,NULL);
			XtAddCallback (adddialog, XmNcancelCallback, open_exit_cb,NULL);
			XtManageChild (adddialog);

			break;
		case 1:
			extractdialog=XmCreateFileSelectionDialog (mainW, "Extract to", NULL, 0);
			XtAddCallback (extractdialog, XmNokCallback, extract_cb,NULL);
			XtAddCallback (extractdialog, XmNcancelCallback, open_exit_cb,NULL);
			XtUnmanageChild(XtNameToWidget(extractdialog,"Items"));

			XtUnmanageChild(XtParent(XtNameToWidget(extractdialog,"*ItemsList")));
						XtManageChild (extractdialog);



			break;
	}
    		
}

int fillList(){
	XmListDeleteAllItems(list);
	XmString str;
	for(int i=0;i<fileNum;i++){
		//printf("%s\n",fileList[i]);
		str=XmStringCreateLocalized(fileList[i]);
		XmListAddItemUnselected (list, str, i+1);

	}
	//XmStringFree(str);
}
int main(int argc,char** argv){
	XmString file,edit,open,new,exit,add,extract;
	file=XmStringCreateLocalized("File");
	edit=XmStringCreateLocalized("Edit");
	open=XmStringCreateLocalized("Open");
	new=XmStringCreateLocalized("New");
	exit=XmStringCreateLocalized("Exit");
	add=XmStringCreateLocalized("Add");
	extract=XmStringCreateLocalized("Extract All");

	toplevel = XtVaOpenApplication (&app, "XmArchiver", NULL, 0, &argc, argv,NULL,sessionShellWidgetClass, NULL);
	mainW=XmVaCreateMainWindow(toplevel,"mw",XmNwidth,300,XmNheight,200,NULL);
	menubar=XmVaCreateSimpleMenuBar(mainW,"menubar",XmVaCASCADEBUTTON,file,'F',XmVaCASCADEBUTTON,edit,'E',NULL);
	filemenu=XmVaCreateSimplePulldownMenu(menubar,"filemenu",0,filehandler,XmVaPUSHBUTTON,open,'O',NULL,NULL,XmVaPUSHBUTTON,new,'N',NULL,NULL,XmVaPUSHBUTTON,exit,'X',NULL,NULL,NULL);
	editmenu=XmVaCreateSimplePulldownMenu(menubar,"editmenu",1,edithandler,XmVaPUSHBUTTON,add,'A',NULL,NULL,XmVaPUSHBUTTON,extract,'E',NULL,NULL,NULL);

	list=XmCreateList(mainW,"filelist",NULL,0);
	XtManageChild(list);
	XtManageChild(menubar);
	XtManageChild(mainW);
	XtRealizeWidget(toplevel);
	XtAppMainLoop(app);
}
