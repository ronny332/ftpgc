#include <stdio.h>
#include <ogcsys.h>
#include <gccore.h>
#include <network.h>

#include "ftpgc.h"

void *initialise();
  
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv)
{
    xfb = initialise();

    printf("%d\n", ftpgc_init());

    return 0;
}

//---------------------------------------------------------------------------------
void *initialise() {
//---------------------------------------------------------------------------------

	void *framebuffer;

	VIDEO_Init();
	PAD_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);
	framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(framebuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(framebuffer);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	return framebuffer;

}