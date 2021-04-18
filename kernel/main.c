/* DayViewer (C) 2010, Total_Noob */

#include <pspkernel.h>
#include "dayviewer_user.h"

PSP_MODULE_INFO("DayViewer", 0x1006, 7, 0);

SceUID loadthreaduid;

int DayViewerSelfUnload(SceSize args, void* argp)
{
 	while(SysMemForKernel_452E3696() != 0x20000)
 	{
 		sceKernelDelayThread(1000);
 	}
	sceKernelSelfStopUnloadModule(0, 0, NULL);
 	return sceKernelExitDeleteThread(0);
}

int DayViewerLoadThread(SceSize args, void* argp)
{
    //Wait to find scePaf_Module. If don't, you'll get an error(paf lib not found)
    while(!sceKernelFindModuleByName("scePaf_Module")) sceKernelDelayThread(50000);

    //scePaf_Module founded, now load the user module from the buffer

    SceUID module = sceKernelLoadModuleBuffer(size_dayviewer_user, dayviewer_user, 0, NULL);
    if(module < 0) return 0;
    module = sceKernelStartModule(module, 0, NULL, NULL, NULL);
    if(module < 0) return 0;

    //Give some time...
    sceKernelDelayThread(1000);

    SceUID uid = sceKernelCreateThread("DayViewerSelfUnload", DayViewerSelfUnload, 0x10, 0x1000, 0, NULL);
    if(uid >= 0) sceKernelStartThread(uid, 0, NULL);

    return 0;
}

int module_start(SceSize args, void* argp)
{
    if(sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH)
    {
        //Create & start a high priority thread
        loadthreaduid = sceKernelCreateThread("DayViewerLoadThread", DayViewerLoadThread, 0x30, 0x1000, 0, NULL);
        if(loadthreaduid >= 0) sceKernelStartThread(loadthreaduid, 0, NULL);
    }

    return 0;
}

int module_stop(SceSize args, void* argp)
{
    if(loadthreaduid >= 0) sceKernelTerminateDeleteThread(loadthreaduid);
    return 0;
}
