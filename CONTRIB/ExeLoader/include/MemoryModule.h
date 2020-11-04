/*
 * Memory DLL loading code
 * Version 0.0.4
 *
 * Copyright (c) 2004-2015 by Joachim Bauch / mail@joachim-bauch.de
 * http://www.joachim-bauch.de
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is MemoryModule.h
 *
 * The Initial Developer of the Original Code is Joachim Bauch.
 *
 * --> Adaptation for Cpcdos OSx by Michael BANVILLE and Sebastien FAVIER
 *      Updated: 31/10/2016
 *
 * Portions created by Joachim Bauch are Copyright (C) 2004-2015
 * Joachim Bauch. All Rights Reserved.
 *
 */

extern void _EXE_LOADER_DEBUG(int alert, const char* format_FR, const char* format_EN, ...);

#ifndef __MEMORY_MODULE_HEADER
#define __MEMORY_MODULE_HEADER

#undef CpcDos
#define CpcDos ////////////////////////////////////////////



#ifdef CpcDos
    #include "win.h"
	#define  SetLastError(x)
#endif




#define GET_HEADER_DICTIONARY(module, idx)  &(module)->headers->OptionalHeader.DataDirectory[idx]
#define ALIGN_DOWN(address, alignment)      (LPVOID)((uintptr_t)(address) & ~((alignment) - 1))
#define ALIGN_VALUE_UP(value, alignment)    (((value) + (alignment) - 1) & ~((alignment) - 1))

#include "ManagedAlloc.h"


typedef BOOL (WINAPI *DllEntryProc)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);
typedef int (WINAPI *ExeEntryProc)(void);


typedef void *HMEMORYMODULE;

typedef void *HMEMORYRSRC;

typedef void *HCUSTOMMODULE;

#ifdef __cplusplus
extern "C" {
#endif

typedef LPVOID (*CustomAllocFunc)(LPVOID, SIZE_T, DWORD, DWORD, void*, ManagedAlloc_exe&);
typedef BOOL (*CustomFreeFunc)(LPVOID, SIZE_T, DWORD, void*, ManagedAlloc_exe&);
typedef HCUSTOMMODULE (*CustomLoadLibraryFunc)(LPCSTR, void *, ManagedAlloc_exe&);
typedef FARPROC (*CustomGetProcAddressFunc)(HCUSTOMMODULE, LPCSTR, void *);
typedef void (*CustomFreeLibraryFunc)(HCUSTOMMODULE, void *);

class MemoryModule
{
	
	public:
	ManagedAlloc_exe instance_AllocManager;
	
	
	MemoryModule(){ 
		_EXE_LOADER_DEBUG(2, "CONSTRUCTEUR: MemoryModule instancie avec succes!\n", "CONSTRUCTOR: MemoryModule instancied with success!"); 
	
	};
	
	void Fin_instance();

	/**
	 * Load EXE/DLL from memory location with the given size.
	 *
	 * All dependencies are resolved using default LoadLibrary/GetProcAddress
	 * calls through the Windows API.
	 */
	HMEMORYMODULE MemoryLoadLibrary(const void *, size_t);

	/**
	 * Load EXE/DLL from memory location with the given size using custom dependency
	 * resolvers.
	 *
	 * Dependencies will be resolved using passed callback methods.
	 */
	HMEMORYMODULE MemoryLoadLibraryEx(const void *, size_t,
		CustomAllocFunc,
		CustomFreeFunc,
		CustomLoadLibraryFunc,
		CustomGetProcAddressFunc,
		CustomFreeLibraryFunc,
		void *);

	/**
	 * Get address of exported method. Supports loading both by name and by
	 * ordinal value.
	 */
	FARPROC MemoryGetProcAddress(HMEMORYMODULE, LPCSTR);

	/**
	 * Free previously loaded EXE/DLL.
	 */
	void MemoryFreeLibrary(HMEMORYMODULE);

	/**
	 * Execute entry point (EXE only). The entry point can only be executed
	 * if the EXE has been loaded to the correct base address or it could
	 * be relocated (i.e. relocation information have not been stripped by
	 * the linker).
	 *
	 * Important: calling this function will not return, i.e. once the loaded
	 * EXE finished running, the process will terminate.
	 *
	 * Returns a negative value if the entry point could not be executed.
	 */
	int MemoryCallEntryPoint(HMEMORYMODULE);

	/**
	 * Find the location of a resource with the specified type and name.
	 */
	HMEMORYRSRC MemoryFindResource(HMEMORYMODULE, LPCTSTR, LPCTSTR);

	/**
	 * Find the location of a resource with the specified type, name and language.
	 */
	HMEMORYRSRC MemoryFindResourceEx(HMEMORYMODULE, LPCTSTR, LPCTSTR, WORD);

	/**
	 * Get the size of the resource in bytes.
	 */
	DWORD MemorySizeofResource(HMEMORYMODULE, HMEMORYRSRC);

	/**
	 * Get a pointer to the contents of the resource.
	 */
	LPVOID MemoryLoadResource(HMEMORYMODULE, HMEMORYRSRC);
 
	/**
	 * Load a string resource.
	 */
	int MemoryLoadString(HMEMORYMODULE, UINT, LPTSTR, int);

	/**
	 * Load a string resource with a given language.
	 */
	int MemoryLoadStringEx(HMEMORYMODULE, UINT, LPTSTR, int, WORD);

	/**
	* Default implementation of CustomAllocFunc that calls VirtualAlloc
	* internally to allocate memory for a library
	*
	* This is the default as used by MemoryLoadLibrary.
	*/
	LPVOID MemoryDefaultAlloc(LPVOID, SIZE_T, DWORD, DWORD, void *);

	/**
	* Default implementation of CustomFreeFunc that calls VirtualFree
	* internally to free the memory used by a library
	*
	* This is the default as used by MemoryLoadLibrary.
	*/
	BOOL MemoryDefaultFree(LPVOID, SIZE_T, DWORD, void *);

	/**
	 * Default implementation of CustomLoadLibraryFunc that calls LoadLibraryA
	 * internally to load an additional libary.
	 *
	 * This is the default as used by MemoryLoadLibrary.
	 */
	HCUSTOMMODULE MemoryDefaultLoadLibrary(LPCSTR, void *);

	/**
	 * Default implementation of CustomGetProcAddressFunc that calls GetProcAddress
	 * internally to get the address of an exported function.
	 *
	 * This is the default as used by MemoryLoadLibrary.
	 */
	FARPROC MemoryDefaultGetProcAddress(HCUSTOMMODULE, LPCSTR, void *);

	/**
	 * Default implementation of CustomFreeLibraryFunc that calls FreeLibrary
	 * internally to release an additional libary.
	 *
	 * This is the default as used by MemoryLoadLibrary.
	 */
	void MemoryDefaultFreeLibrary(HCUSTOMMODULE, void *);
	
}; // Class 

#ifdef __cplusplus
}
#endif

#endif  // __MEMORY_MODULE_HEADER
