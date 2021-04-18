#ifndef __SCEPAF_H__
#define __SCEPAF_H__

//I can't crack NID names, so I use my own names xD

int scePafWcslen(wchar_t* str);
int scePafWcsncmp(wchar_t* str1, wchar_t* str2, int len);
wchar_t* scePafMalloc(int size);
int scePafFree(wchar_t* addr);
int scePafWcscpy(wchar_t* str1, wchar_t* str2);
int scePafGetCurrentClockLocalTime(pspTime* time);
int scePafWcsprintf(wchar_t* string, int len, wchar_t* buf, ...);
int scePafStrcmp(char* str1, char* str2);
//int scePafGetIconName(void* rcoID, char* textName);

#endif
