//***************************************************************************

//

//  MAINDLL.CPP

// 

//  Module: WMI Method provider sample code

//

//  Purpose: Contains DLL entry points.  Also has code that controls

//           when the DLL can be unloaded by tracking the number of

//           objects and locks as well as routines that support

//           self registration.

//

// Copyright (c)  Microsoft Corporation, All Rights Reserved
//
//***************************************************************************

#include <objbase.h>
#include <initguid.h>
#include <strsafe.h>

#include "methprov.h"

HMODULE ghModule;

// TODO, GuidGen should be used to generate a unique number for any 
// providers that are going to be used for anything more extensive 
// than just testing.
                                 
DEFINE_GUID(CLSID_methodprovider,0xe30ec6a0, 0x23cf, 0x11d1, 0x8f, 0xde, 0x0, 0x0, 0xf8, 0x4, 0xaa, 0x5c);
//{E30EC6A0-23CF-11d1-8FDE-0000F804AA5C}

//Count number of objects and number of locks.

long       g_cObj=0;
long       g_cLock=0;

//***************************************************************************
//
// LibMain32
//
// Purpose: Entry point for DLL.
//
// Return: TRUE if OK.
//
//***************************************************************************


BOOL WINAPI LibMain32(HINSTANCE hInstance, ULONG ulReason
    , LPVOID pvReserved)
{
    if (DLL_PROCESS_DETACH==ulReason)
    {
        return TRUE;
    }
    else
    {
        if (DLL_PROCESS_ATTACH!=ulReason)
            return TRUE;
    }

    ghModule = hInstance;
    return TRUE;
}

//***************************************************************************
//
//  DllGetClassObject
//
//  Purpose: Called by Ole when some client wants a class factory.  Return 
//           one only if it is the sort of class this DLL supports.
//
//***************************************************************************


STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, PPVOID ppv)
{
    HRESULT hr;
    CProvFactory *pObj;

    if (CLSID_methodprovider!=rclsid)
        return E_FAIL;

    pObj=new CProvFactory();

    if (NULL==pObj)
        return E_OUTOFMEMORY;

    hr=pObj->QueryInterface(riid, ppv);

    if (FAILED(hr))
        delete pObj;

    return hr;
}

//***************************************************************************
//
// DllCanUnloadNow
//
// Purpose: Called periodically by Ole in order to determine if the
//          DLL can be freed.
//
// Return:  S_OK if there are no objects in use and the class factory 
//          isn't locked.
//
//***************************************************************************

STDAPI DllCanUnloadNow(void)
{
    SCODE   sc;

    //It is OK to unload if there are no objects or locks on the 
    // class factory.
    
    sc=(0L==g_cObj && 0L==g_cLock) ? S_OK : S_FALSE;
    return sc;
}

//***************************************************************************
//
//  Is4OrMore
//
//  Returns true if win95 or any version of NT > 3.51
//
//***************************************************************************

BOOL Is4OrMore(void)
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if(!GetVersionEx(&os))
        return FALSE;           // should never happen
    return os.dwMajorVersion >= 4;
}

//***************************************************************************
//
// DllRegisterServer
//
// Purpose: Called during setup or by regsvr32.
//
// Return:  NOERROR if registration successful, error otherwise.
//***************************************************************************

STDAPI DllRegisterServer(void)
{   
    char       szID[128];
    WCHAR      wcID[128];
    char       szCLSID[128];
    TCHAR      szModule[MAX_PATH + 1];
    char * pName = "WMI Method Provider Test";
    char * pModel;
	size_t * intReturnValue = NULL;
    HKEY hKey1, hKey2;

    // Normally we want to use "Both" as the threading model since
    // the DLL is free threaded, but NT 3.51 Ole doesnt work unless
    // the model is "Aparment"

    if(Is4OrMore())
        pModel = "Both";
    else
        pModel = "Apartment";

  // Create the path.

	memset(wcID, NULL, sizeof(wcID));
	memset(szID, NULL, sizeof(szID));	

    StringFromGUID2(CLSID_methodprovider, wcID, sizeof(wcID)/sizeof(WCHAR) - 1);
    wcstombs_s(intReturnValue, szID, sizeof(szID), wcID,  sizeof(szID));
	StringCbCopy(szCLSID, sizeof(szCLSID), "Software\\classes\\CLSID\\");
	StringCbCat(szCLSID, sizeof(szCLSID), (LPCTSTR)szID);


    // Create entries under CLSID

	LONG lRet = RegCreateKeyEx( HKEY_LOCAL_MACHINE, szCLSID, 0, NULL,
                    REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, 
                    &hKey1, NULL );
	if (lRet != ERROR_SUCCESS)
	{
		return E_FAIL;
	}

    lRet = RegSetValueEx(hKey1, NULL, 0, REG_SZ, (BYTE *)pName, strlen(pName)+1);
	if (lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey1);
		return E_FAIL;
	}

	lRet = RegCreateKeyEx( hKey1, "InprocServer32", 0, NULL,
                    REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, 
                    &hKey2, NULL );
	if (lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey1);
		return E_FAIL;
	}

	memset(&szModule, NULL, sizeof(szModule));
    GetModuleFileName(ghModule, szModule,  sizeof(szModule)/sizeof(TCHAR) - 1);
    lRet = RegSetValueEx(hKey2, NULL, 0, REG_SZ, (BYTE *)szModule, 
                                        strlen(szModule)+1);
	if (lRet != ERROR_SUCCESS)
	{
	    RegCloseKey(hKey2);
		RegCloseKey(hKey1);
		return E_FAIL;
	}
    lRet = RegSetValueEx(hKey2, "ThreadingModel", 0, REG_SZ, 
                                        (BYTE *)pModel, strlen(pModel)+1);
	if (lRet != ERROR_SUCCESS)
	{
	    RegCloseKey(hKey2);
		RegCloseKey(hKey1);
		return E_FAIL;
	}
    RegCloseKey(hKey1);
    RegCloseKey(hKey2);

    return NOERROR;
	
}

//***************************************************************************
//
// DllUnregisterServer
//
// Purpose: Called when it is time to remove the registry entries.
//
// Return:  NOERROR if registration successful, error otherwise.
//***************************************************************************

STDAPI DllUnregisterServer(void)
{
	char       szID[128];
    WCHAR      wcID[128];
    char  szCLSID[128];
	size_t * intReturnValue = NULL;
    HKEY hKey;

    // Create the path using the CLSID
	memset(wcID, NULL, sizeof(wcID));
	memset(szID, NULL, sizeof(szID));	
    StringFromGUID2(CLSID_methodprovider, wcID, sizeof(wcID)/sizeof(WCHAR));

    wcstombs_s(intReturnValue, szID, sizeof(szID), wcID, sizeof(szID));

	StringCbCopy(szCLSID, sizeof(szCLSID), "Software\\classes\\CLSID\\");
	StringCbCat(szCLSID, sizeof(szCLSID), (LPCTSTR)szID);

     // First delete the InProcServer subkey.

    DWORD dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szCLSID, 0, KEY_WRITE, &hKey);
    if(dwRet == NO_ERROR)
    {
        RegDeleteKey(hKey, "InProcServer32");
        RegCloseKey(hKey);
    }

    dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\CLASSES\\CLSID\\"), 0, KEY_WRITE, &hKey);
    if(dwRet == NO_ERROR)
    {
        RegDeleteKey(hKey,szID);
        RegCloseKey(hKey);
    }

    return NOERROR;

}


