	.text
	.syntax unified
	.file	"mangling.cc"
	.def	 "?get_GetLastError@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetLastError@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetLastError@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetLastError@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetLastError@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetLastError@win32@fast_io@@YAIXZ"
	movt	r0, :upper16:"__imp_?GetLastError@win32@fast_io@@YAIXZ"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetLastError@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetLastError@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetLastError@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetLastError@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetLastError@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetLastError
	movt	r0, :upper16:__imp_GetLastError
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_LoadLibraryA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_LoadLibraryA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_LoadLibraryA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_LoadLibraryA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_LoadLibraryA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?LoadLibraryA@win32@fast_io@@YAPAXPBD@Z"
	movt	r0, :upper16:"__imp_?LoadLibraryA@win32@fast_io@@YAPAXPBD@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_LoadLibraryA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_LoadLibraryA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_LoadLibraryA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_LoadLibraryA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_LoadLibraryA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_LoadLibraryA
	movt	r0, :upper16:__imp_LoadLibraryA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_LoadLibraryW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_LoadLibraryW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_LoadLibraryW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_LoadLibraryW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_LoadLibraryW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?LoadLibraryW@win32@fast_io@@YAPAXPB_W@Z"
	movt	r0, :upper16:"__imp_?LoadLibraryW@win32@fast_io@@YAPAXPB_W@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_LoadLibraryW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_LoadLibraryW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_LoadLibraryW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_LoadLibraryW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_LoadLibraryW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_LoadLibraryW
	movt	r0, :upper16:__imp_LoadLibraryW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_LoadLibraryExA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_LoadLibraryExA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_LoadLibraryExA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_LoadLibraryExA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_LoadLibraryExA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?LoadLibraryExA@win32@fast_io@@YAPAXPBDPAXI@Z"
	movt	r0, :upper16:"__imp_?LoadLibraryExA@win32@fast_io@@YAPAXPBDPAXI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_LoadLibraryExA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_LoadLibraryExA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_LoadLibraryExA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_LoadLibraryExA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_LoadLibraryExA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_LoadLibraryExA
	movt	r0, :upper16:__imp_LoadLibraryExA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_LoadLibraryExW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_LoadLibraryExW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_LoadLibraryExW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_LoadLibraryExW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_LoadLibraryExW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?LoadLibraryExW@win32@fast_io@@YAPAXPB_WPAXI@Z"
	movt	r0, :upper16:"__imp_?LoadLibraryExW@win32@fast_io@@YAPAXPB_WPAXI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_LoadLibraryExW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_LoadLibraryExW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_LoadLibraryExW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_LoadLibraryExW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_LoadLibraryExW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_LoadLibraryExW
	movt	r0, :upper16:__imp_LoadLibraryExW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_FormatMessageA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_FormatMessageA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_FormatMessageA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_FormatMessageA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_FormatMessageA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?FormatMessageA@win32@fast_io@@YAIIPBDIIPADIPAX@Z"
	movt	r0, :upper16:"__imp_?FormatMessageA@win32@fast_io@@YAIIPBDIIPADIPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_FormatMessageA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_FormatMessageA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_FormatMessageA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_FormatMessageA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_FormatMessageA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_FormatMessageA
	movt	r0, :upper16:__imp_FormatMessageA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_FormatMessageW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_FormatMessageW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_FormatMessageW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_FormatMessageW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_FormatMessageW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?FormatMessageW@win32@fast_io@@YAIIPB_WIIPA_WIPAX@Z"
	movt	r0, :upper16:"__imp_?FormatMessageW@win32@fast_io@@YAIIPB_WIIPA_WIPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_FormatMessageW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_FormatMessageW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_FormatMessageW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_FormatMessageW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_FormatMessageW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_FormatMessageW
	movt	r0, :upper16:__imp_FormatMessageW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_CreateFileMappingA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_CreateFileMappingA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_CreateFileMappingA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_CreateFileMappingA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_CreateFileMappingA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?CreateFileMappingA@win32@fast_io@@YAPAXPAXPAUsecurity_attributes@12@IIIPBD@Z"
	movt	r0, :upper16:"__imp_?CreateFileMappingA@win32@fast_io@@YAPAXPAXPAUsecurity_attributes@12@IIIPBD@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_CreateFileMappingA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_CreateFileMappingA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_CreateFileMappingA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_CreateFileMappingA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_CreateFileMappingA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_CreateFileMappingA
	movt	r0, :upper16:__imp_CreateFileMappingA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_CreateFileMappingW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_CreateFileMappingW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_CreateFileMappingW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_CreateFileMappingW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_CreateFileMappingW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?CreateFileMappingW@win32@fast_io@@YAPAXPAXPAUsecurity_attributes@12@IIIPB_W@Z"
	movt	r0, :upper16:"__imp_?CreateFileMappingW@win32@fast_io@@YAPAXPAXPAUsecurity_attributes@12@IIIPB_W@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_CreateFileMappingW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_CreateFileMappingW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_CreateFileMappingW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_CreateFileMappingW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_CreateFileMappingW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_CreateFileMappingW
	movt	r0, :upper16:__imp_CreateFileMappingW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_MapViewOfFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_MapViewOfFile@@YA?A?<auto>@@XZ" ; -- Begin function ?get_MapViewOfFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_MapViewOfFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_MapViewOfFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?MapViewOfFile@win32@fast_io@@YAPAXPAXIIII@Z"
	movt	r0, :upper16:"__imp_?MapViewOfFile@win32@fast_io@@YAPAXPAXIIII@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_MapViewOfFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_MapViewOfFile@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_MapViewOfFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_MapViewOfFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_MapViewOfFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_MapViewOfFile
	movt	r0, :upper16:__imp_MapViewOfFile
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_SetEndOfFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_SetEndOfFile@@YA?A?<auto>@@XZ" ; -- Begin function ?get_SetEndOfFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_SetEndOfFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_SetEndOfFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?SetEndOfFile@win32@fast_io@@YAHPAX@Z"
	movt	r0, :upper16:"__imp_?SetEndOfFile@win32@fast_io@@YAHPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_SetEndOfFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_SetEndOfFile@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_SetEndOfFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_SetEndOfFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_SetEndOfFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_SetEndOfFile
	movt	r0, :upper16:__imp_SetEndOfFile
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_UnmapViewOfFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_UnmapViewOfFile@@YA?A?<auto>@@XZ" ; -- Begin function ?get_UnmapViewOfFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_UnmapViewOfFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_UnmapViewOfFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?UnmapViewOfFile@win32@fast_io@@YAHPBX@Z"
	movt	r0, :upper16:"__imp_?UnmapViewOfFile@win32@fast_io@@YAHPBX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_UnmapViewOfFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_UnmapViewOfFile@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_UnmapViewOfFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_UnmapViewOfFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_UnmapViewOfFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_UnmapViewOfFile
	movt	r0, :upper16:__imp_UnmapViewOfFile
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WriteFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WriteFile@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WriteFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WriteFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WriteFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WriteFile@win32@fast_io@@YAHPAXPBXIPAIPAUoverlapped@12@@Z"
	movt	r0, :upper16:"__imp_?WriteFile@win32@fast_io@@YAHPAXPBXIPAIPAUoverlapped@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WriteFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WriteFile@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WriteFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WriteFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WriteFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WriteFile
	movt	r0, :upper16:__imp_WriteFile
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_ReadFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_ReadFile@@YA?A?<auto>@@XZ" ; -- Begin function ?get_ReadFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_ReadFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_ReadFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?ReadFile@win32@fast_io@@YAHPAXPBXIPAIPAUoverlapped@12@@Z"
	movt	r0, :upper16:"__imp_?ReadFile@win32@fast_io@@YAHPAXPBXIPAIPAUoverlapped@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_ReadFile@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_ReadFile@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_ReadFile@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_ReadFile@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_ReadFile@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_ReadFile
	movt	r0, :upper16:__imp_ReadFile
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_SetFilePointerEx@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_SetFilePointerEx@@YA?A?<auto>@@XZ" ; -- Begin function ?get_SetFilePointerEx@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_SetFilePointerEx@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_SetFilePointerEx@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?SetFilePointerEx@win32@fast_io@@YAHPAX_JPA_JI@Z"
	movt	r0, :upper16:"__imp_?SetFilePointerEx@win32@fast_io@@YAHPAX_JPA_JI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_SetFilePointerEx@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_SetFilePointerEx@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_SetFilePointerEx@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_SetFilePointerEx@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_SetFilePointerEx@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_SetFilePointerEx
	movt	r0, :upper16:__imp_SetFilePointerEx
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_DuplicateHandle@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_DuplicateHandle@@YA?A?<auto>@@XZ" ; -- Begin function ?get_DuplicateHandle@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_DuplicateHandle@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_DuplicateHandle@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?DuplicateHandle@win32@fast_io@@YAHPAX00PAPAXIHI@Z"
	movt	r0, :upper16:"__imp_?DuplicateHandle@win32@fast_io@@YAHPAX00PAPAXIHI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_DuplicateHandle@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_DuplicateHandle@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_DuplicateHandle@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_DuplicateHandle@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_DuplicateHandle@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_DuplicateHandle
	movt	r0, :upper16:__imp_DuplicateHandle
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetStdHandle@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetStdHandle@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetStdHandle@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetStdHandle@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetStdHandle@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetStdHandle@win32@fast_io@@YAPAXI@Z"
	movt	r0, :upper16:"__imp_?GetStdHandle@win32@fast_io@@YAPAXI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetStdHandle@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetStdHandle@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetStdHandle@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetStdHandle@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetStdHandle@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetStdHandle
	movt	r0, :upper16:__imp_GetStdHandle
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_CreatePipe@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_CreatePipe@@YA?A?<auto>@@XZ" ; -- Begin function ?get_CreatePipe@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_CreatePipe@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_CreatePipe@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?CreatePipe@win32@fast_io@@YAHPAPAX0PAUsecurity_attributes@12@I@Z"
	movt	r0, :upper16:"__imp_?CreatePipe@win32@fast_io@@YAHPAPAX0PAUsecurity_attributes@12@I@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_CreatePipe@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_CreatePipe@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_CreatePipe@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_CreatePipe@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_CreatePipe@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_CreatePipe
	movt	r0, :upper16:__imp_CreatePipe
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_FreeLibrary@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_FreeLibrary@@YA?A?<auto>@@XZ" ; -- Begin function ?get_FreeLibrary@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_FreeLibrary@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_FreeLibrary@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?FreeLibrary@win32@fast_io@@YAHPAX@Z"
	movt	r0, :upper16:"__imp_?FreeLibrary@win32@fast_io@@YAHPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_FreeLibrary@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_FreeLibrary@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_FreeLibrary@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_FreeLibrary@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_FreeLibrary@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_FreeLibrary
	movt	r0, :upper16:__imp_FreeLibrary
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetProcAddress@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetProcAddress@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetProcAddress@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetProcAddress@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetProcAddress@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetProcAddress@win32@fast_io@@YAP6AHXZPAXPBD@Z"
	movt	r0, :upper16:"__imp_?GetProcAddress@win32@fast_io@@YAP6AHXZPAXPBD@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetProcAddress@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetProcAddress@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetProcAddress@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetProcAddress@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetProcAddress@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetProcAddress
	movt	r0, :upper16:__imp_GetProcAddress
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetModuleHandleA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetModuleHandleA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetModuleHandleA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetModuleHandleA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetModuleHandleA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetModuleHandleA@win32@fast_io@@YAPAXPBD@Z"
	movt	r0, :upper16:"__imp_?GetModuleHandleA@win32@fast_io@@YAPAXPBD@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetModuleHandleA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetModuleHandleA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetModuleHandleA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetModuleHandleA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetModuleHandleA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetModuleHandleA
	movt	r0, :upper16:__imp_GetModuleHandleA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetModuleHandleW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetModuleHandleW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetModuleHandleW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetModuleHandleW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetModuleHandleW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetModuleHandleW@win32@fast_io@@YAPAXPB_W@Z"
	movt	r0, :upper16:"__imp_?GetModuleHandleW@win32@fast_io@@YAPAXPB_W@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetModuleHandleW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetModuleHandleW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetModuleHandleW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetModuleHandleW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetModuleHandleW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetModuleHandleW
	movt	r0, :upper16:__imp_GetModuleHandleW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WaitForSingleObject@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WaitForSingleObject@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WaitForSingleObject@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WaitForSingleObject@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WaitForSingleObject@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WaitForSingleObject@win32@fast_io@@YAIPAXI@Z"
	movt	r0, :upper16:"__imp_?WaitForSingleObject@win32@fast_io@@YAIPAXI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WaitForSingleObject@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WaitForSingleObject@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WaitForSingleObject@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WaitForSingleObject@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WaitForSingleObject@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WaitForSingleObject
	movt	r0, :upper16:__imp_WaitForSingleObject
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_CancelIo@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_CancelIo@@YA?A?<auto>@@XZ" ; -- Begin function ?get_CancelIo@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_CancelIo@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_CancelIo@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?CancelIo@win32@fast_io@@YAIPAX@Z"
	movt	r0, :upper16:"__imp_?CancelIo@win32@fast_io@@YAIPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_CancelIo@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_CancelIo@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_CancelIo@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_CancelIo@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_CancelIo@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_CancelIo
	movt	r0, :upper16:__imp_CancelIo
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetFileInformationByHandle@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetFileInformationByHandle@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetFileInformationByHandle@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetFileInformationByHandle@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetFileInformationByHandle@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetFileInformationByHandle@win32@fast_io@@YAHPIAXPIAUby_handle_file_information@12@@Z"
	movt	r0, :upper16:"__imp_?GetFileInformationByHandle@win32@fast_io@@YAHPIAXPIAUby_handle_file_information@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetFileInformationByHandle@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetFileInformationByHandle@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetFileInformationByHandle@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetFileInformationByHandle@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetFileInformationByHandle@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetFileInformationByHandle
	movt	r0, :upper16:__imp_GetFileInformationByHandle
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetUserDefaultLocaleName@win32@fast_io@@YAHPA_WH@Z"
	movt	r0, :upper16:"__imp_?GetUserDefaultLocaleName@win32@fast_io@@YAHPA_WH@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetUserDefaultLocaleName@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetUserDefaultLocaleName
	movt	r0, :upper16:__imp_GetUserDefaultLocaleName
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetSystemTimePreciseAsFileTime@win32@fast_io@@YAXPAUfiletime@12@@Z"
	movt	r0, :upper16:"__imp_?GetSystemTimePreciseAsFileTime@win32@fast_io@@YAXPAUfiletime@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetSystemTimePreciseAsFileTime@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetSystemTimePreciseAsFileTime
	movt	r0, :upper16:__imp_GetSystemTimePreciseAsFileTime
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetSystemTimeAsFileTime@win32@fast_io@@YAXPAUfiletime@12@@Z"
	movt	r0, :upper16:"__imp_?GetSystemTimeAsFileTime@win32@fast_io@@YAXPAUfiletime@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetSystemTimeAsFileTime@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetSystemTimeAsFileTime
	movt	r0, :upper16:__imp_GetSystemTimeAsFileTime
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ" ; -- Begin function ?get_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?QueryUnbiasedInterruptTime@win32@fast_io@@YAHPA_K@Z"
	movt	r0, :upper16:"__imp_?QueryUnbiasedInterruptTime@win32@fast_io@@YAHPA_K@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_QueryUnbiasedInterruptTime@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_QueryUnbiasedInterruptTime
	movt	r0, :upper16:__imp_QueryUnbiasedInterruptTime
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_QueryPerformanceCounter@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_QueryPerformanceCounter@@YA?A?<auto>@@XZ" ; -- Begin function ?get_QueryPerformanceCounter@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_QueryPerformanceCounter@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_QueryPerformanceCounter@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?QueryPerformanceCounter@win32@fast_io@@YAHPA_J@Z"
	movt	r0, :upper16:"__imp_?QueryPerformanceCounter@win32@fast_io@@YAHPA_J@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_QueryPerformanceCounter@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_QueryPerformanceCounter@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_QueryPerformanceCounter@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_QueryPerformanceCounter@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_QueryPerformanceCounter@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_QueryPerformanceCounter
	movt	r0, :upper16:__imp_QueryPerformanceCounter
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_QueryPerformanceFrequency@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_QueryPerformanceFrequency@@YA?A?<auto>@@XZ" ; -- Begin function ?get_QueryPerformanceFrequency@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_QueryPerformanceFrequency@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_QueryPerformanceFrequency@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?QueryPerformanceFrequency@win32@fast_io@@YAHPA_J@Z"
	movt	r0, :upper16:"__imp_?QueryPerformanceFrequency@win32@fast_io@@YAHPA_J@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_QueryPerformanceFrequency@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_QueryPerformanceFrequency@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_QueryPerformanceFrequency@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_QueryPerformanceFrequency@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_QueryPerformanceFrequency@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_QueryPerformanceFrequency
	movt	r0, :upper16:__imp_QueryPerformanceFrequency
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetProcessTimes@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetProcessTimes@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetProcessTimes@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetProcessTimes@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetProcessTimes@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetProcessTimes@win32@fast_io@@YAHPAXPAUfiletime@12@111@Z"
	movt	r0, :upper16:"__imp_?GetProcessTimes@win32@fast_io@@YAHPAXPAUfiletime@12@111@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetProcessTimes@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetProcessTimes@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetProcessTimes@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetProcessTimes@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetProcessTimes@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetProcessTimes
	movt	r0, :upper16:__imp_GetProcessTimes
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetThreadTimes@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetThreadTimes@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetThreadTimes@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetThreadTimes@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetThreadTimes@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetThreadTimes@win32@fast_io@@YAHPAXPAUfiletime@12@111@Z"
	movt	r0, :upper16:"__imp_?GetThreadTimes@win32@fast_io@@YAHPAXPAUfiletime@12@111@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetThreadTimes@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetThreadTimes@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetThreadTimes@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetThreadTimes@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetThreadTimes@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetThreadTimes
	movt	r0, :upper16:__imp_GetThreadTimes
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetHandleInformation@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetHandleInformation@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetHandleInformation@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetHandleInformation@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetHandleInformation@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetHandleInformation@win32@fast_io@@YAHPAXPAI@Z"
	movt	r0, :upper16:"__imp_?GetHandleInformation@win32@fast_io@@YAHPAXPAI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetHandleInformation@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetHandleInformation@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetHandleInformation@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetHandleInformation@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetHandleInformation@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetHandleInformation
	movt	r0, :upper16:__imp_GetHandleInformation
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_SetHandleInformation@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_SetHandleInformation@@YA?A?<auto>@@XZ" ; -- Begin function ?get_SetHandleInformation@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_SetHandleInformation@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_SetHandleInformation@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?SetHandleInformation@win32@fast_io@@YAHPAXII@Z"
	movt	r0, :upper16:"__imp_?SetHandleInformation@win32@fast_io@@YAHPAXII@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_SetHandleInformation@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_SetHandleInformation@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_SetHandleInformation@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_SetHandleInformation@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_SetHandleInformation@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_SetHandleInformation
	movt	r0, :upper16:__imp_SetHandleInformation
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetTempPathA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetTempPathA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetTempPathA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetTempPathA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetTempPathA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetTempPathA@win32@fast_io@@YAIIPAD@Z"
	movt	r0, :upper16:"__imp_?GetTempPathA@win32@fast_io@@YAIIPAD@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetTempPathA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetTempPathA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetTempPathA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetTempPathA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetTempPathA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetTempPathA
	movt	r0, :upper16:__imp_GetTempPathA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetTempPathW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetTempPathW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetTempPathW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetTempPathW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetTempPathW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetTempPathW@win32@fast_io@@YAIIPA_W@Z"
	movt	r0, :upper16:"__imp_?GetTempPathW@win32@fast_io@@YAIIPA_W@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetTempPathW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetTempPathW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetTempPathW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetTempPathW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetTempPathW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetTempPathW
	movt	r0, :upper16:__imp_GetTempPathW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_CreateFileA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_CreateFileA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_CreateFileA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_CreateFileA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_CreateFileA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?CreateFileA@win32@fast_io@@YAPAXPBDIIPAUsecurity_attributes@12@IIPAX@Z"
	movt	r0, :upper16:"__imp_?CreateFileA@win32@fast_io@@YAPAXPBDIIPAUsecurity_attributes@12@IIPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_CreateFileA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_CreateFileA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_CreateFileA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_CreateFileA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_CreateFileA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_CreateFileA
	movt	r0, :upper16:__imp_CreateFileA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_CreateFileW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_CreateFileW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_CreateFileW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_CreateFileW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_CreateFileW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?CreateFileW@win32@fast_io@@YAPAXPB_WIIPAUsecurity_attributes@12@IIPAX@Z"
	movt	r0, :upper16:"__imp_?CreateFileW@win32@fast_io@@YAPAXPB_WIIPAUsecurity_attributes@12@IIPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_CreateFileW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_CreateFileW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_CreateFileW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_CreateFileW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_CreateFileW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_CreateFileW
	movt	r0, :upper16:__imp_CreateFileW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_CreateIoCompletionPort@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_CreateIoCompletionPort@@YA?A?<auto>@@XZ" ; -- Begin function ?get_CreateIoCompletionPort@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_CreateIoCompletionPort@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_CreateIoCompletionPort@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?CreateIoCompletionPort@win32@fast_io@@YAPAXPAX0II@Z"
	movt	r0, :upper16:"__imp_?CreateIoCompletionPort@win32@fast_io@@YAPAXPAX0II@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_CreateIoCompletionPort@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_CreateIoCompletionPort@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_CreateIoCompletionPort@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_CreateIoCompletionPort@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_CreateIoCompletionPort@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_CreateIoCompletionPort
	movt	r0, :upper16:__imp_CreateIoCompletionPort
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_SystemFunction036@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_SystemFunction036@@YA?A?<auto>@@XZ" ; -- Begin function ?get_SystemFunction036@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_SystemFunction036@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_SystemFunction036@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?SystemFunction036@win32@fast_io@@YAHPAXI@Z"
	movt	r0, :upper16:"__imp_?SystemFunction036@win32@fast_io@@YAHPAXI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_SystemFunction036@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_SystemFunction036@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_SystemFunction036@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_SystemFunction036@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_SystemFunction036@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_SystemFunction036
	movt	r0, :upper16:__imp_SystemFunction036
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_CloseHandle@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_CloseHandle@@YA?A?<auto>@@XZ" ; -- Begin function ?get_CloseHandle@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_CloseHandle@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_CloseHandle@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?CloseHandle@win32@fast_io@@YAHPAX@Z"
	movt	r0, :upper16:"__imp_?CloseHandle@win32@fast_io@@YAHPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_CloseHandle@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_CloseHandle@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_CloseHandle@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_CloseHandle@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_CloseHandle@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_CloseHandle
	movt	r0, :upper16:__imp_CloseHandle
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_LockFileEx@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_LockFileEx@@YA?A?<auto>@@XZ" ; -- Begin function ?get_LockFileEx@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_LockFileEx@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_LockFileEx@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?LockFileEx@win32@fast_io@@YAHPAXIIIIPAUoverlapped@12@@Z"
	movt	r0, :upper16:"__imp_?LockFileEx@win32@fast_io@@YAHPAXIIIIPAUoverlapped@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_LockFileEx@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_LockFileEx@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_LockFileEx@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_LockFileEx@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_LockFileEx@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_LockFileEx
	movt	r0, :upper16:__imp_LockFileEx
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_UnlockFileEx@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_UnlockFileEx@@YA?A?<auto>@@XZ" ; -- Begin function ?get_UnlockFileEx@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_UnlockFileEx@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_UnlockFileEx@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?UnlockFileEx@win32@fast_io@@YAHPAXIIIPAUoverlapped@12@@Z"
	movt	r0, :upper16:"__imp_?UnlockFileEx@win32@fast_io@@YAHPAXIIIPAUoverlapped@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_UnlockFileEx@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_UnlockFileEx@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_UnlockFileEx@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_UnlockFileEx@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_UnlockFileEx@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_UnlockFileEx
	movt	r0, :upper16:__imp_UnlockFileEx
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_DeviceIoControl@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_DeviceIoControl@@YA?A?<auto>@@XZ" ; -- Begin function ?get_DeviceIoControl@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_DeviceIoControl@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_DeviceIoControl@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?DeviceIoControl@win32@fast_io@@YAHPAXI0I0I0PAUoverlapped@12@@Z"
	movt	r0, :upper16:"__imp_?DeviceIoControl@win32@fast_io@@YAHPAXI0I0I0PAUoverlapped@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_DeviceIoControl@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_DeviceIoControl@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_DeviceIoControl@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_DeviceIoControl@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_DeviceIoControl@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_DeviceIoControl
	movt	r0, :upper16:__imp_DeviceIoControl
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetFileType@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetFileType@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetFileType@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetFileType@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetFileType@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetFileType@win32@fast_io@@YAIPAX@Z"
	movt	r0, :upper16:"__imp_?GetFileType@win32@fast_io@@YAIPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetFileType@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetFileType@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetFileType@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetFileType@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetFileType@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetFileType
	movt	r0, :upper16:__imp_GetFileType
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetACP@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetACP@@YA?A?<auto>@@XZ"  ; -- Begin function ?get_GetACP@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetACP@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetACP@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetACP@win32@fast_io@@YAIXZ"
	movt	r0, :upper16:"__imp_?GetACP@win32@fast_io@@YAIXZ"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetACP@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetACP@@YA?A?<auto>@@XZ"  ; -- Begin function ?sys_GetACP@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetACP@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetACP@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetACP
	movt	r0, :upper16:__imp_GetACP
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_getenv_s@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_getenv_s@@YA?A?<auto>@@XZ" ; -- Begin function ?get_getenv_s@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_getenv_s@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_getenv_s@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?getenv_s@win32@fast_io@@YAHPAIPADIPBD@Z"
	movt	r0, :upper16:"__imp_?getenv_s@win32@fast_io@@YAHPAIPADIPBD@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_getenv_s@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_getenv_s@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_getenv_s@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_getenv_s@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_getenv_s@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_getenv_s
	movt	r0, :upper16:__imp_getenv_s
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_MessageBoxA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_MessageBoxA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_MessageBoxA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_MessageBoxA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_MessageBoxA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?MessageBoxA@win32@fast_io@@YAIPAXPBD1I@Z"
	movt	r0, :upper16:"__imp_?MessageBoxA@win32@fast_io@@YAIPAXPBD1I@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_MessageBoxA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_MessageBoxA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_MessageBoxA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_MessageBoxA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_MessageBoxA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_MessageBoxA
	movt	r0, :upper16:__imp_MessageBoxA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_MessageBoxW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_MessageBoxW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_MessageBoxW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_MessageBoxW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_MessageBoxW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?MessageBoxW@win32@fast_io@@YAIPAXPB_W1I@Z"
	movt	r0, :upper16:"__imp_?MessageBoxW@win32@fast_io@@YAIPAXPB_W1I@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_MessageBoxW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_MessageBoxW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_MessageBoxW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_MessageBoxW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_MessageBoxW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_MessageBoxW
	movt	r0, :upper16:__imp_MessageBoxW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetConsoleMode@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetConsoleMode@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetConsoleMode@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetConsoleMode@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetConsoleMode@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetConsoleMode@win32@fast_io@@YAHPAXPAI@Z"
	movt	r0, :upper16:"__imp_?GetConsoleMode@win32@fast_io@@YAHPAXPAI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetConsoleMode@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetConsoleMode@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetConsoleMode@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetConsoleMode@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetConsoleMode@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetConsoleMode
	movt	r0, :upper16:__imp_GetConsoleMode
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_SetConsoleMode@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_SetConsoleMode@@YA?A?<auto>@@XZ" ; -- Begin function ?get_SetConsoleMode@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_SetConsoleMode@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_SetConsoleMode@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?SetConsoleMode@win32@fast_io@@YAHPAXI@Z"
	movt	r0, :upper16:"__imp_?SetConsoleMode@win32@fast_io@@YAHPAXI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_SetConsoleMode@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_SetConsoleMode@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_SetConsoleMode@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_SetConsoleMode@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_SetConsoleMode@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_SetConsoleMode
	movt	r0, :upper16:__imp_SetConsoleMode
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_ReadConsoleA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_ReadConsoleA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_ReadConsoleA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_ReadConsoleA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_ReadConsoleA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?ReadConsoleA@win32@fast_io@@YAHPAX0IPAI0@Z"
	movt	r0, :upper16:"__imp_?ReadConsoleA@win32@fast_io@@YAHPAX0IPAI0@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_ReadConsoleA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_ReadConsoleA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_ReadConsoleA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_ReadConsoleA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_ReadConsoleA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_ReadConsoleA
	movt	r0, :upper16:__imp_ReadConsoleA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_ReadConsoleW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_ReadConsoleW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_ReadConsoleW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_ReadConsoleW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_ReadConsoleW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?ReadConsoleW@win32@fast_io@@YAHPAX0IPAI0@Z"
	movt	r0, :upper16:"__imp_?ReadConsoleW@win32@fast_io@@YAHPAX0IPAI0@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_ReadConsoleW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_ReadConsoleW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_ReadConsoleW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_ReadConsoleW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_ReadConsoleW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_ReadConsoleW
	movt	r0, :upper16:__imp_ReadConsoleW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WriteConsoleA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WriteConsoleA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WriteConsoleA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WriteConsoleA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WriteConsoleA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WriteConsoleA@win32@fast_io@@YAHPAXPBXIPAI0@Z"
	movt	r0, :upper16:"__imp_?WriteConsoleA@win32@fast_io@@YAHPAXPBXIPAI0@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WriteConsoleA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WriteConsoleA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WriteConsoleA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WriteConsoleA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WriteConsoleA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WriteConsoleA
	movt	r0, :upper16:__imp_WriteConsoleA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WriteConsoleW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WriteConsoleW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WriteConsoleW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WriteConsoleW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WriteConsoleW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WriteConsoleW@win32@fast_io@@YAHPAXPBXIPAI0@Z"
	movt	r0, :upper16:"__imp_?WriteConsoleW@win32@fast_io@@YAHPAXPBXIPAI0@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WriteConsoleW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WriteConsoleW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WriteConsoleW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WriteConsoleW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WriteConsoleW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WriteConsoleW
	movt	r0, :upper16:__imp_WriteConsoleW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetConsoleScreenBufferInfo@win32@fast_io@@YAHPAXPAUconsole_screen_buffer_info@12@@Z"
	movt	r0, :upper16:"__imp_?GetConsoleScreenBufferInfo@win32@fast_io@@YAHPAXPAUconsole_screen_buffer_info@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetConsoleScreenBufferInfo@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetConsoleScreenBufferInfo
	movt	r0, :upper16:__imp_GetConsoleScreenBufferInfo
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?ScrollConsoleScreenBufferA@win32@fast_io@@YAHPAXPBUsmall_rect@12@1Ucoord@12@PBUchar_info@12@@Z"
	movt	r0, :upper16:"__imp_?ScrollConsoleScreenBufferA@win32@fast_io@@YAHPAXPBUsmall_rect@12@1Ucoord@12@PBUchar_info@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_ScrollConsoleScreenBufferA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_ScrollConsoleScreenBufferA
	movt	r0, :upper16:__imp_ScrollConsoleScreenBufferA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?ScrollConsoleScreenBufferW@win32@fast_io@@YAHPAXPBUsmall_rect@12@1Ucoord@12@PBUchar_info@12@@Z"
	movt	r0, :upper16:"__imp_?ScrollConsoleScreenBufferW@win32@fast_io@@YAHPAXPBUsmall_rect@12@1Ucoord@12@PBUchar_info@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_ScrollConsoleScreenBufferW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_ScrollConsoleScreenBufferW
	movt	r0, :upper16:__imp_ScrollConsoleScreenBufferW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_SetConsoleCursorPosition@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_SetConsoleCursorPosition@@YA?A?<auto>@@XZ" ; -- Begin function ?get_SetConsoleCursorPosition@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_SetConsoleCursorPosition@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_SetConsoleCursorPosition@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?SetConsoleCursorPosition@win32@fast_io@@YAHPAXUcoord@12@@Z"
	movt	r0, :upper16:"__imp_?SetConsoleCursorPosition@win32@fast_io@@YAHPAXUcoord@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_SetConsoleCursorPosition@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_SetConsoleCursorPosition@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_SetConsoleCursorPosition@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_SetConsoleCursorPosition@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_SetConsoleCursorPosition@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_SetConsoleCursorPosition
	movt	r0, :upper16:__imp_SetConsoleCursorPosition
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_InitializeCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_InitializeCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?get_InitializeCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_InitializeCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_InitializeCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?InitializeCriticalSection@win32@fast_io@@YAXPAX@Z"
	movt	r0, :upper16:"__imp_?InitializeCriticalSection@win32@fast_io@@YAXPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_InitializeCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_InitializeCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_InitializeCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_InitializeCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_InitializeCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_InitializeCriticalSection
	movt	r0, :upper16:__imp_InitializeCriticalSection
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_EnterCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_EnterCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?get_EnterCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_EnterCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_EnterCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?EnterCriticalSection@win32@fast_io@@YAXPAX@Z"
	movt	r0, :upper16:"__imp_?EnterCriticalSection@win32@fast_io@@YAXPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_EnterCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_EnterCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_EnterCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_EnterCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_EnterCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_EnterCriticalSection
	movt	r0, :upper16:__imp_EnterCriticalSection
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_TryEnterCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_TryEnterCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?get_TryEnterCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_TryEnterCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_TryEnterCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?TryEnterCriticalSection@win32@fast_io@@YAHPAX@Z"
	movt	r0, :upper16:"__imp_?TryEnterCriticalSection@win32@fast_io@@YAHPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_TryEnterCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_TryEnterCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_TryEnterCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_TryEnterCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_TryEnterCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_TryEnterCriticalSection
	movt	r0, :upper16:__imp_TryEnterCriticalSection
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_LeaveCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_LeaveCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?get_LeaveCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_LeaveCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_LeaveCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?LeaveCriticalSection@win32@fast_io@@YAXPAX@Z"
	movt	r0, :upper16:"__imp_?LeaveCriticalSection@win32@fast_io@@YAXPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_LeaveCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_LeaveCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_LeaveCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_LeaveCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_LeaveCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_LeaveCriticalSection
	movt	r0, :upper16:__imp_LeaveCriticalSection
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_DeleteCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_DeleteCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?get_DeleteCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_DeleteCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_DeleteCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?DeleteCriticalSection@win32@fast_io@@YAXPAX@Z"
	movt	r0, :upper16:"__imp_?DeleteCriticalSection@win32@fast_io@@YAXPAX@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_DeleteCriticalSection@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_DeleteCriticalSection@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_DeleteCriticalSection@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_DeleteCriticalSection@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_DeleteCriticalSection@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_DeleteCriticalSection
	movt	r0, :upper16:__imp_DeleteCriticalSection
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSADuplicateSocketA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSADuplicateSocketA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSADuplicateSocketA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSADuplicateSocketA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSADuplicateSocketA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSADuplicateSocketA@win32@fast_io@@YAHPAXIPAUwsaprotocol_infoa@12@@Z"
	movt	r0, :upper16:"__imp_?WSADuplicateSocketA@win32@fast_io@@YAHPAXIPAUwsaprotocol_infoa@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSADuplicateSocketA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSADuplicateSocketA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSADuplicateSocketA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSADuplicateSocketA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSADuplicateSocketA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSADuplicateSocketA
	movt	r0, :upper16:__imp_WSADuplicateSocketA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSADuplicateSocketW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSADuplicateSocketW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSADuplicateSocketW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSADuplicateSocketW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSADuplicateSocketW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSADuplicateSocketW@win32@fast_io@@YAXPAXIPAUwsaprotocol_infow@12@@Z"
	movt	r0, :upper16:"__imp_?WSADuplicateSocketW@win32@fast_io@@YAXPAXIPAUwsaprotocol_infow@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSADuplicateSocketW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSADuplicateSocketW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSADuplicateSocketW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSADuplicateSocketW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSADuplicateSocketW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSADuplicateSocketW
	movt	r0, :upper16:__imp_WSADuplicateSocketW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSACleanup@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSACleanup@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSACleanup@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSACleanup@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSACleanup@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSACleanup@win32@fast_io@@YAHXZ"
	movt	r0, :upper16:"__imp_?WSACleanup@win32@fast_io@@YAHXZ"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSACleanup@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSACleanup@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSACleanup@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSACleanup@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSACleanup@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSACleanup
	movt	r0, :upper16:__imp_WSACleanup
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSAStartup@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSAStartup@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSAStartup@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSAStartup@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSAStartup@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSAStartup@win32@fast_io@@YAHIPAUwsadata@12@@Z"
	movt	r0, :upper16:"__imp_?WSAStartup@win32@fast_io@@YAHIPAUwsadata@12@@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSAStartup@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSAStartup@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSAStartup@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSAStartup@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSAStartup@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSAStartup
	movt	r0, :upper16:__imp_WSAStartup
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSAGetLastError@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSAGetLastError@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSAGetLastError@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSAGetLastError@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSAGetLastError@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSAGetLastError@win32@fast_io@@YAHXZ"
	movt	r0, :upper16:"__imp_?WSAGetLastError@win32@fast_io@@YAHXZ"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSAGetLastError@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSAGetLastError@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSAGetLastError@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSAGetLastError@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSAGetLastError@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSAGetLastError
	movt	r0, :upper16:__imp_WSAGetLastError
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_closesocket@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_closesocket@@YA?A?<auto>@@XZ" ; -- Begin function ?get_closesocket@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_closesocket@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_closesocket@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?closesocket@win32@fast_io@@YAHI@Z"
	movt	r0, :upper16:"__imp_?closesocket@win32@fast_io@@YAHI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_closesocket@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_closesocket@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_closesocket@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_closesocket@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_closesocket@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_closesocket
	movt	r0, :upper16:__imp_closesocket
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSASocketW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSASocketW@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSASocketW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSASocketW@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSASocketW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSASocketW@win32@fast_io@@YAIHHHPAUwsaprotocol_infow@12@II@Z"
	movt	r0, :upper16:"__imp_?WSASocketW@win32@fast_io@@YAIHHHPAUwsaprotocol_infow@12@II@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSASocketW@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSASocketW@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSASocketW@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSASocketW@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSASocketW@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSASocketW
	movt	r0, :upper16:__imp_WSASocketW
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSASocketA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSASocketA@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSASocketA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSASocketA@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSASocketA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSASocketA@win32@fast_io@@YAIHHHPAUwsaprotocol_infoa@12@II@Z"
	movt	r0, :upper16:"__imp_?WSASocketA@win32@fast_io@@YAIHHHPAUwsaprotocol_infoa@12@II@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSASocketA@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSASocketA@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSASocketA@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSASocketA@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSASocketA@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSASocketA
	movt	r0, :upper16:__imp_WSASocketA
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_bind@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_bind@@YA?A?<auto>@@XZ"    ; -- Begin function ?get_bind@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_bind@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_bind@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?bind@win32@fast_io@@YAHIPBXH@Z"
	movt	r0, :upper16:"__imp_?bind@win32@fast_io@@YAHIPBXH@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_bind@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_bind@@YA?A?<auto>@@XZ"    ; -- Begin function ?sys_bind@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_bind@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_bind@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_bind
	movt	r0, :upper16:__imp_bind
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_listen@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_listen@@YA?A?<auto>@@XZ"  ; -- Begin function ?get_listen@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_listen@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_listen@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?listen@win32@fast_io@@YAHIH@Z"
	movt	r0, :upper16:"__imp_?listen@win32@fast_io@@YAHIH@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_listen@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_listen@@YA?A?<auto>@@XZ"  ; -- Begin function ?sys_listen@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_listen@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_listen@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_listen
	movt	r0, :upper16:__imp_listen
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSAAccept@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSAAccept@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSAAccept@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSAAccept@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSAAccept@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSAAccept@win32@fast_io@@YAIIPBXPAHP6AXPAUwsabuf@12@2PAUqualityofservice@12@322PAII@ZI@Z"
	movt	r0, :upper16:"__imp_?WSAAccept@win32@fast_io@@YAIIPBXPAHP6AXPAUwsabuf@12@2PAUqualityofservice@12@322PAII@ZI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSAAccept@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSAAccept@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSAAccept@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSAAccept@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSAAccept@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSAAccept
	movt	r0, :upper16:__imp_WSAAccept
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_ioctlsocket@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_ioctlsocket@@YA?A?<auto>@@XZ" ; -- Begin function ?get_ioctlsocket@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_ioctlsocket@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_ioctlsocket@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?ioctlsocket@win32@fast_io@@YAHIJPAI@Z"
	movt	r0, :upper16:"__imp_?ioctlsocket@win32@fast_io@@YAHIJPAI@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_ioctlsocket@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_ioctlsocket@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_ioctlsocket@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_ioctlsocket@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_ioctlsocket@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_ioctlsocket
	movt	r0, :upper16:__imp_ioctlsocket
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSASend@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSASend@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSASend@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSASend@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSASend@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSASend@win32@fast_io@@YAHIPAUwsabuf@12@IPAIIPAUoverlapped@12@P6AXII2I@Z@Z"
	movt	r0, :upper16:"__imp_?WSASend@win32@fast_io@@YAHIPAUwsabuf@12@IPAIIPAUoverlapped@12@P6AXII2I@Z@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSASend@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSASend@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSASend@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSASend@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSASend@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSASend
	movt	r0, :upper16:__imp_WSASend
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSASendMsg@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSASendMsg@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSASendMsg@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSASendMsg@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSASendMsg@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSASendMsg@win32@fast_io@@YAHIPAUwsamsg@12@IPAIPAUoverlapped@12@P6AXII2I@Z@Z"
	movt	r0, :upper16:"__imp_?WSASendMsg@win32@fast_io@@YAHIPAUwsamsg@12@IPAIPAUoverlapped@12@P6AXII2I@Z@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSASendMsg@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSASendMsg@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSASendMsg@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSASendMsg@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSASendMsg@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSASendMsg
	movt	r0, :upper16:__imp_WSASendMsg
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSASendTo@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSASendTo@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSASendTo@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSASendTo@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSASendTo@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSASendTo@win32@fast_io@@YAHIPAUwsabuf@12@IPAIIPBXHPAUoverlapped@12@P6AXII3I@Z@Z"
	movt	r0, :upper16:"__imp_?WSASendTo@win32@fast_io@@YAHIPAUwsabuf@12@IPAIIPBXHPAUoverlapped@12@P6AXII3I@Z@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSASendTo@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSASendTo@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSASendTo@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSASendTo@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSASendTo@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSASendTo
	movt	r0, :upper16:__imp_WSASendTo
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSARecv@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSARecv@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSARecv@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSARecv@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSARecv@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSARecv@win32@fast_io@@YAHIPAUwsabuf@12@IPAI1PAUoverlapped@12@P6AXII2I@Z@Z"
	movt	r0, :upper16:"__imp_?WSARecv@win32@fast_io@@YAHIPAUwsabuf@12@IPAI1PAUoverlapped@12@P6AXII2I@Z@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSARecv@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSARecv@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSARecv@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSARecv@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSARecv@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSARecv
	movt	r0, :upper16:__imp_WSARecv
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSARecvFrom@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSARecvFrom@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSARecvFrom@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSARecvFrom@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSARecvFrom@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSARecvFrom@win32@fast_io@@YAHIPAUwsabuf@12@IPAI1PBXPAHPAUoverlapped@12@P6AXII4I@Z@Z"
	movt	r0, :upper16:"__imp_?WSARecvFrom@win32@fast_io@@YAHIPAUwsabuf@12@IPAI1PBXPAHPAUoverlapped@12@P6AXII4I@Z@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSARecvFrom@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSARecvFrom@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSARecvFrom@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSARecvFrom@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSARecvFrom@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSARecvFrom
	movt	r0, :upper16:__imp_WSARecvFrom
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_connect@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_connect@@YA?A?<auto>@@XZ" ; -- Begin function ?get_connect@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_connect@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_connect@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?connect@win32@fast_io@@YAHIPBXH@Z"
	movt	r0, :upper16:"__imp_?connect@win32@fast_io@@YAHIPBXH@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_connect@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_connect@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_connect@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_connect@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_connect@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_connect
	movt	r0, :upper16:__imp_connect
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_WSAConnect@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_WSAConnect@@YA?A?<auto>@@XZ" ; -- Begin function ?get_WSAConnect@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_WSAConnect@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_WSAConnect@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?WSAConnect@win32@fast_io@@YAHIPBXHPAUwsabuf@12@1PAUqualityofservice@12@2@Z"
	movt	r0, :upper16:"__imp_?WSAConnect@win32@fast_io@@YAHIPBXHPAUwsabuf@12@1PAUqualityofservice@12@2@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_WSAConnect@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_WSAConnect@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_WSAConnect@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_WSAConnect@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_WSAConnect@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_WSAConnect
	movt	r0, :upper16:__imp_WSAConnect
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_shutdown@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_shutdown@@YA?A?<auto>@@XZ" ; -- Begin function ?get_shutdown@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_shutdown@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_shutdown@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?shutdown@win32@fast_io@@YAHIPBXH@Z"
	movt	r0, :upper16:"__imp_?shutdown@win32@fast_io@@YAHIPBXH@Z"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_shutdown@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_shutdown@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_shutdown@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_shutdown@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_shutdown@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_shutdown
	movt	r0, :upper16:__imp_shutdown
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?get_GetCurrentProcessId@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?get_GetCurrentProcessId@@YA?A?<auto>@@XZ" ; -- Begin function ?get_GetCurrentProcessId@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?get_GetCurrentProcessId@@YA?A?<auto>@@XZ"
	.thumb_func
"?get_GetCurrentProcessId@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:"__imp_?GetCurrentProcessId@win32@fast_io@@YAIXZ"
	movt	r0, :upper16:"__imp_?GetCurrentProcessId@win32@fast_io@@YAIXZ"
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.def	 "?sys_GetCurrentProcessId@@YA?A?<auto>@@XZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sys_GetCurrentProcessId@@YA?A?<auto>@@XZ" ; -- Begin function ?sys_GetCurrentProcessId@@YA?A?<auto>@@XZ
	.p2align	1
	.code16                                 ; @"?sys_GetCurrentProcessId@@YA?A?<auto>@@XZ"
	.thumb_func
"?sys_GetCurrentProcessId@@YA?A?<auto>@@XZ":
; %bb.0:                                ; %entry
	movw	r0, :lower16:__imp_GetCurrentProcessId
	movt	r0, :upper16:__imp_GetCurrentProcessId
	ldr	r0, [r0]
	bx	lr
                                        ; -- End function
	.section	.drectve,"yn"
	.ascii	" /DEFAULTLIB:uuid.lib"
	.ascii	" /DEFAULTLIB:uuid.lib"
	.addrsig
