	.set noreorder

#include "pspstub.s"

	STUB_START	"scePaf",0x00010011,0x00070005
	STUB_FUNC	0x2BE8DDBB,scePafWcscpy
	STUB_FUNC	0xE8CCC611,scePafWcsncmp
	STUB_FUNC	0xCDDCFFB3,scePafWcsprintf
	STUB_FUNC	0x48BB05D5,scePafMalloc
	STUB_FUNC	0x22FB4177,scePafFree
	STUB_FUNC	0xBC8DC92B,scePafGetCurrentClockLocalTime
	STUB_FUNC	0xE3D530AE,scePafStrcmp
	STUB_END