#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <iostream>
#include <sstream>
#include <3ds.h>
#include <vector>
#include "utils.h"
#include "application.h"
#include "input.h"
#include "gui.h"
#include "main.h"
#include "download.h"
/* Images */
#include "offline_bin.h"

using namespace std;

navBar_s navbar;
u8* screenTopLeft = 0;
u8* screenTopRight = 0;
u8* screenBottom = 0;
extern char superStr[];
/* SCENE */
unsigned int scene = 0;
unsigned int maxScene = 2;
char* sceneTitle = "";
vector<Application_s> tAppList;
int FPS;
u8* cimg;

/*
TopScreen: w400 h240
BottomScreen: w320 h240
*/

/* Used for the so called "scrolling" */
int VSPX = 0; 
int VSPY = 0;
int VSTX = 0; 
int VSTY = 0;

int butPos = 0;

void guiInit(){
	//ToDo
}

void setAppList(vector<Application_s> AppList){
	tAppList = AppList;
	//Reset the cords
	VSPY = 0;
	VSTY = 0;
}

void renderGUI(){
	screenTopLeft = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL); 
	screenTopRight = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
	screenBottom = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL); 
	
	/* Background */
	background();
	
	/* UI: TOP */
	drawTopBar();
	navbar.Title = sceneTitle;
	
	//TODO: Added a switch check for diffrent menus
	switch(currentMenu){
		case 0: //Overview
			renderStoreFront();
			break;
		case 1: //Settings
			renderSettings();
			break;
		case 2: //App Page
			renderAppPage();
			break;
		case 3: //Downloads
			break;
		case 4: //by dev
			break;
	}
	
	
	/* DEBUG */
	#ifdef DEBUG
	renderDebug();
	#endif
	
	/* Buffers */
	gfxFlushBuffers();
	gfxSwapBuffers();
}


void renderStoreFront(){
	static int appn = 0;
	
	/* UI: TOP */
	if(cimg != NULL){
		gfxDrawSprite(GFX_TOP, GFX_LEFT, cimg, 192, 400, 0, 0);
		gfxDrawSprite(GFX_TOP, GFX_RIGHT, cimg, 192, 400, 0, 0);
	}
	/* UI: BOTTOM */
	
	/* Screen related UI(Changes based on scene) */
	switch(scene){
		case 0:
			for(auto app : tAppList){
				appn++;
				drawAppEntry(app, appn);
			}
			appn = 0;
			break;
		case 1:
			
			break;
		case 2:
			
			break;
		default:
			
			break;
	}
}

void renderSettings(){
	/* UI: TOP */
	if(cimg != NULL){ //ToDo: Render settings banner
		gfxDrawSprite(GFX_TOP, GFX_LEFT, cimg, 192, 400, 0, 0);
		gfxDrawSprite(GFX_TOP, GFX_RIGHT, cimg, 192, 400, 0, 0);
	}
	/* UI: BOTTOM */
	//ToDo
}

void renderAppPage(){
	/* UI: TOP */
	if(cimg != NULL){ //ToDo: Render app page banner
		gfxDrawSprite(GFX_TOP, GFX_LEFT, cimg, 192, 400, 0, 0);
		gfxDrawSprite(GFX_TOP, GFX_RIGHT, cimg, 192, 400, 0, 0);
	}
	/* UI: BOTTOM */
	
	//Text
	char buffer[310];
	sprintf(buffer, "%s %s", currentApp.name, currentApp.version);
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontBlackHeader,  buffer, 240 - fontBlackHeader.height,5);
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontBlackSubHeader, currentApp.publisher , (240 - fontBlackHeader.height) - fontBlackSubHeader.height,5);
	
	//Download Button
	drawFillRect(0,190,320,240, 0,148,255, screenBottom); //Todo: use button as download status bar
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontWhiteHeader,  "Download", 15,113);
	
	clearVButtons();
	vButton_s but;
	but.ID = 0;
	but.x = 0;
	but.y = 190;
	but.x2 = 320;
	but.y2 = 240;
	but.menu = 0;
	addVButton(but);
}

void fadeOut(){
	for(int x = 255; x >= 0; x = x - 15){
		gfxFadeScreen(GFX_BOTTOM, GFX_LEFT, x);
		gfxFadeScreen(GFX_TOP, GFX_LEFT, x);
		gfxFadeScreen(GFX_TOP, GFX_RIGHT, x);
		gfxFlushBuffers(); 
		gfxSwapBuffers();
	}
}

void doKeyboard(char* inputText){
	//ToDo
}

/* Scenes */
int getOnScreenY(int vsy){
    return (vsy-VSPY);
}

/* UIs */
void drawAppEntry(Application_s app, int place){
	
    int y = 0;
	int butY = 0, butY2 = 0, butX2 = 302, butX = 200;
	if(place == 1){
		VSTY = APPLICATION_ENTRY_H;
		clearVButtons();
	}
	else{
		VSTY += APPLICATION_ENTRY_H;
	}
		
    y = (MARGIN * (place)) + (APPLICATION_ENTRY_H * (place - 1));
	
    if((getOnScreenY(y)>=240 || getOnScreenY(y)+APPLICATION_ENTRY_H <= 0) || VSPY >= VSTY){
		butPos = 0;
        return; //Outside screen dont draw
    }
    else if(getOnScreenY(y)+APPLICATION_ENTRY_H >= 240)/*The entry is partly offscreen*/
    {
        drawFillRect( 0,getOnScreenY(y), 320,239, 255,/*y/(float)VSTY**/255,255, screenBottom);
		
		//Button
		int x =  getOnScreenY(y)+(APPLICATION_ENTRY_H/4)*3 < 239 ? getOnScreenY(y)+(APPLICATION_ENTRY_H/4)*3 : 239;
		butX2 = x;
		int z = getOnScreenY(y) + APPLICATION_ENTRY_H/4 < 239 ? getOnScreenY(y) + APPLICATION_ENTRY_H/4 : 239;
		butX = z;
        drawFillRect( 200, z, 302,x, 0,148,255, screenBottom);
    }
    else if(getOnScreenY(y)<0)
    {
        drawLine(0,0,320,0,0,255,0,screenBottom);
        drawFillRect( 0,0, 320,getOnScreenY(y)+APPLICATION_ENTRY_H, 255,/*y/(float)VSTY**/255,255, screenBottom);
        drawLine( 0, getOnScreenY(y) + APPLICATION_ENTRY_H -1 , 320, getOnScreenY(y) + APPLICATION_ENTRY_H -1, 224,224,224, screenBottom);
		
		//Button
		butY = getOnScreenY(y) + APPLICATION_ENTRY_H/4 -1;
		butY2 = getOnScreenY(y)+(APPLICATION_ENTRY_H/4)*3 -1;
		drawFillRect( 200,butY, 302,butY2, 0,148,255, screenBottom);
    }
    else{
        drawFillRect( 0,getOnScreenY(y), 320,getOnScreenY(y)+APPLICATION_ENTRY_H, 255,/*y/(float)VSTY**/255,255, screenBottom);
        drawLine( 0, getOnScreenY(y) + APPLICATION_ENTRY_H -1 , 320, getOnScreenY(y) + APPLICATION_ENTRY_H -1, 224,224,224, screenBottom);
		
		//Button
		butY = getOnScreenY(y)+APPLICATION_ENTRY_H/4;
		butY2 = getOnScreenY(y)+(APPLICATION_ENTRY_H/4)*3;
		drawFillRect( 200,butY, 302,butY2, 0,148,255, screenBottom);
    }
	
    gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontBlackHeader, app.name,240-getOnScreenY( APPTITLE_MARGIN + y ), 5);
    gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontBlackSubHeader, app.version,240-getOnScreenY( APPVERSION_MARGIN + y ), 5);
	//Button
	gfxDrawText(GFX_BOTTOM, GFX_LEFT, &fontWhiteHeader, "Download",240-getOnScreenY( ((APPLICATION_ENTRY_H/4)*2 + fontWhiteHeader.height/2) + y ), 212);
	
	vButton_s but;
	but.ID = butPos;
	but.x = butX;
	but.y = butY;
	but.x2 = butX2;
	but.y2 = butY2;
	but.app = app;
	but.menu = 2;
	addVButton(but);
	butPos++;
}


void renderDebug(){
	int i = countLines(superStr); 
 	while(i>200/fontDefault.height-3){cutLine(superStr);i--;} 
	gfxDrawText(GFX_TOP, GFX_LEFT, &fontBlack, superStr, (240-fontDefault.height*5)+6, 6); 
	gfxDrawText(GFX_TOP, GFX_RIGHT, &fontBlack, superStr, (240-fontDefault.height*5)+6, 6); 
}
void background(){
	drawFillRect( 0, 0, 320, 240, 227,242,253, screenBottom);
	drawFillRect( 0, 0, 400, 240, 227,242,253, screenTopLeft);
	drawFillRect( 0, 0, 400, 240, 227,242,253, screenTopRight);
}
void setStoreFrontImg(char* url){
	//w: 400
	//h: 192
	print("Downloading banner...\n");
	cimg = (u8*)downloadFile(url);
	print("Banner downloaded!\n");
	if(cimg[0] == 'e'){
		//Set banner to offline banner
		printf("Failed to dowload and set banner, defaulting to offline one\n");
		cimg = (u8*) offline_bin;
	}
}
void drawTopBar(){
	char buffer[100];
	sprintf(buffer, navbar.Title.c_str());
	drawFillRect(0,0,400,NAVBAR_H, 0,126,216, screenTopLeft);
	drawFillRect(0,0,400,NAVBAR_H, 0,126,216, screenTopRight);
	drawFillRect(0,12,400,SECONDARY_NAVBAR_H + 12, 0,148,255, screenTopRight);
	drawFillRect(0,12,400,SECONDARY_NAVBAR_H + 12, 0,148,255, screenTopRight);
	
	drawString(APPLICATION_NAME, (400-strlen(APPLICATION_NAME)*8)/2,2, 255,255,255, screenTopLeft,GFX_TOP);
	drawString(APPLICATION_NAME, (400-strlen(APPLICATION_NAME)*8)/2,2, 255,255,255, screenTopRight,GFX_TOP);
	
	gfxDrawText(GFX_TOP, GFX_LEFT, &fontWhiteHeader, buffer, 240 - (((SECONDARY_NAVBAR_H/2) + fontWhiteHeader.height)), 13); 
	gfxDrawText(GFX_TOP, GFX_RIGHT, &fontWhiteHeader, buffer, 240 - (((SECONDARY_NAVBAR_H/2) + fontWhiteHeader.height)), 13); 
	
	u64 timeInSeconds = osGetTime() / 1000; 
	u64 dayTime = timeInSeconds % SECONDS_IN_DAY; 
	sprintf(buffer, "%llu:%llu:%llu, FPS: %d",dayTime / SECONDS_IN_HOUR,(dayTime % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE,dayTime % SECONDS_IN_MINUTE, FPS);
	drawString(buffer, 2,2, 255,255,255, screenTopLeft,GFX_TOP);
	drawString(buffer, 2,2, 255,255,255, screenTopRight,GFX_TOP);
}
int countLines(char* str)
{
	if(!str)return 0;
	int cnt; for(cnt=1;*str=='\n'?++cnt:*str;str++);
	return cnt;
}
void cutLine(char* str)
{
	if(!str || !*str)return;
	char* str2=str;	for(;*str2&&*(str2+1)&&*str2!='\n';str2++);	str2++;
	memmove(str,str2,strlen(str2)+1);
}
