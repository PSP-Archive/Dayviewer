/* DayViewer (C) 2010, Total_Noob */

#include <systemctrl.h>
#include <psppower.h>
#include <psprtc.h>
#include "psppaf.h"

PSP_MODULE_INFO("DayViewer_User", 0x0007, 7, 0);

#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a);
#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a);
#define REDIRECT_FUNCTION(a, f) _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a); _sw(0, a + 4);
#define PATCH_IMPORT(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a); _sw(0, a + 4);

STMOD_HANDLER previous;

wchar_t clockstring[128];
wchar_t config[20][128];
int mode[13];
int modecount = 0;

int AddThing(wchar_t* out, wchar_t* thing, int j)
{
    int i;
    for(i = 0; thing[i]; i++) out[j++] = thing[i];
    return i;
}

void SetClockString(wchar_t* out, wchar_t* buf)
{
    typedef struct
    {
        wchar_t* name;
        int len;
        wchar_t* format;
    } ClockConfig;

    ClockConfig conf[] =
    {
        { L"weekday", 7, L"s" },
        { L"day", 3, L"d" },
        { L"monthname", 9, L"s" },
        { L"month", 5, L"d" },
        { L"year", 4, L"d" },
        { L"hour12", 6, L"d" },
        { L"hour", 4, L"d" },
        { L"min", 3, L"02d" },
        { L"sec", 3, L"02d" },
        { L"ampm", 4, L"s" },
        { L"batpercent", 10, L"d" },
        { L"batlifehour", 11, L"d" },
        { L"batlifemin", 10, L"02d" },
    };

    int i, p, j = 0;

    for(i = 0; buf[i]; i++)
    {
        if(buf[i] == 0x5C)
        {
            out[j++] = '\n';
            i++;
        }

        out[j++] = buf[i];

        if(modecount <= 12)
        {
            if(buf[i] == '%')
            {
                for(p = 0; p <= 12; p++)
                {
                    if(scePafWcsncmp(buf + i + 1, conf[p].name, conf[p].len) == 0)
                    {
                        mode[modecount++] = p;
                        j += AddThing(out, conf[p].format, j);
                        i += conf[p].len;
                    }
                }
            }
        }
    }

    out[j] = 0;
}

int ReadLine(SceUID fd, wchar_t* buf, int txtmode)
{
    int i = 0, read = 0, res = 1;

    if(txtmode == 1)
    {
        char ch;
        while(i < 128 && (read = sceIoRead(fd, &ch, txtmode)) == txtmode)
        {
            if(ch == '\r' || ch == '\n') break;
            buf[i++] = ch;
        }
    }
    else
    {
        wchar_t ch;
        while(i < 128 && (read = sceIoRead(fd, &ch, txtmode)) == txtmode)
        {
            if(ch == '\r' || ch == '\n') break;
            buf[i++] = ch;
        }
    }

    buf[i] = 0;

    if(i == 0 && read != txtmode) res = 0;

    return res;
}

int ReadConfig()
{
    SceUID fd = sceIoOpen("ms0:/seplugins/dayviewer_config.txt", PSP_O_RDONLY, 0777);
    if(fd < 0) return -1;

    int size = sceIoLseek32(fd, 0, PSP_SEEK_END);
    sceIoLseek32(fd, 0, PSP_SEEK_SET);

    wchar_t offset;
    int i = 0, txtmode = 2;

    sceIoRead(fd, &offset, 2);

    if(offset != 0xFEFF)
    {
        txtmode = 1;
        sceIoLseek32(fd, 0, PSP_SEEK_SET);
    }

    wchar_t* buf = (wchar_t*)scePafMalloc(size / txtmode);

    while(i < 20 && ReadLine(fd, buf, txtmode))
    {
        if(buf[0] == '#' || buf[0] == 0) continue;

        if(i == 0)
        {
            wchar_t* out = (wchar_t*)scePafMalloc(256);
            SetClockString(out, buf);
            scePafWcscpy(config[i], out);
            scePafFree(out);
        }
        else scePafWcscpy(config[i], buf);

        i++;
    }

    scePafFree(buf);
    sceIoClose(fd);

    return 0;
}

int DayViewerThread(SceSize args, void* argp)
{
    pspTime time;

    int i, hour12, batterypercent = 0, batterylifetime = 0;

    wchar_t* formats[13];

    while(1)
    {
        scePafGetCurrentClockLocalTime(&time);

        int weekday = sceRtcGetDayOfWeek(time.year, time.month, time.day);
        if(weekday == 0) weekday = 7;

        if(time.hour > 12) hour12 = time.hour - 12;
        else if(time.hour == 0) hour12 = 12;
        else hour12 = time.hour;

        int percent = scePowerGetBatteryLifePercent();
        if(percent >= 0) batterypercent = percent;

        int lifetime = scePowerGetBatteryLifeTime();
        if(lifetime >= 0) batterylifetime = lifetime;

        for(i = 0; i < modecount; i++)
        {
            switch(mode[i])
            {
                case 0:
                formats[i] = config[weekday];
                break;

                case 1:
                formats[i] = time.day;
                break;

                case 2:
                formats[i] = config[7 + time.month];
                break;

                case 3:
                formats[i] = time.month;
                break;

                case 4:
                formats[i] = time.year;
                break;

                case 5:
                formats[i] = hour12;
                break;

                case 6:
                formats[i] = time.hour;
                break;

                case 7:
                formats[i] = time.minutes;
                break;

                case 8:
                formats[i] = time.seconds;
                break;

                case 9:
                formats[i] = time.hour >= 12 ? L"PM" : L"AM";
                break;

                case 10:
                formats[i] = batterypercent;
                break;

                case 11:
                formats[i] = batterylifetime / 60;
                break;

                case 12:
                formats[i] = batterylifetime - (batterylifetime / 60 * 60);
                break;
            }
        }

        scePafWcsprintf(clockstring, 100, config[0], formats[0], formats[1], formats[2], formats[3], formats[4], formats[5], formats[6], formats[7], formats[8], formats[9], formats[10], formats[11], formats[12]);

        sceKernelDelayThread(1000000); //Don't use the processor for 1 second
    }

    return 0;
}

int scePafAddClockPatched(pspTime* time, wchar_t* string, int max_len, wchar_t* format)
{
    return scePafWcscpy(string, clockstring);
}

int OnModuleStart(SceModule2* mod)
{
    if(scePafStrcmp(mod->modname, "vsh_module") == 0)
    {
        //Search offset to patch
        u32 text_end = mod->text_addr + mod->text_size;
        u32 text_addr = mod->text_addr;

        for(; text_addr < text_end; text_addr += 4)
        {
            if(_lw(text_addr) == 0x24060064) break;
        }

        _sh(0, text_addr - 0x48);
        MAKE_CALL(text_addr + 4, scePafAddClockPatched);

	    sceKernelDcacheWritebackAll();
    }

    if(!previous)
        return 0;

    return previous(mod);
}

int module_start(SceSize args, void* argp)
{
    ReadConfig();

    //Create & start a high priority thread
    SceUID thid = sceKernelCreateThread("DayViewerThread", DayViewerThread, 0x18, 0x1000, 0, NULL);
    if(thid >= 0) sceKernelStartThread(thid, 0, NULL);

    previous = sctrlHENSetStartModuleHandler(OnModuleStart);

    return 0;
}
