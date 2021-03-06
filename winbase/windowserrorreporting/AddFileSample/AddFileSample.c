/*++

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
    ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.

    Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    AddFileSample.c

Abstract:

    This sample demonstrates how to use the Windows Error Reporting APIs to send
    a report that contains a custom file.

--*/

#include <stdio.h>

#include <windows.h>
#include <werapi.h>
#include <strsafe.h>

int
wmain (
    int argc,
    const wchar_t* argv[],
    const wchar_t* envp[]
)
{
    HRESULT hr = E_FAIL;
    HREPORT ReportHandle = NULL;
    WER_REPORT_INFORMATION ReportInformation = {0};
    WER_SUBMIT_RESULT SubmitResult;


    UNREFERENCED_PARAMETER (argv);
    UNREFERENCED_PARAMETER (argc);
    UNREFERENCED_PARAMETER (envp);


    wprintf (L"WerReportAddFile API Sample\n");
    wprintf (L"---------------------------\n");

    //
    // Report creation
    //

    //
    // First, populate the WER_REPORT_INFORMATION structure.
    //
    ReportInformation.dwSize = sizeof(WER_REPORT_INFORMATION);

    //
    // To report on the current process, set the hProcess field to NULL.
    //
    ReportInformation.hProcess = NULL;

    //
    // Set the Consent Key, i.e. the name used to look up consent settings
    //
    hr = StringCchCopy (ReportInformation.wzConsentKey,
                        ARRAYSIZE(ReportInformation.wzConsentKey),
                        L"Sample Consent Key");

    if (FAILED(hr)) {
        wprintf (L"Failed to set the consent key, error: 0x%08X\n", hr);
        goto End;
    }

    //
    // Set the name of the application we are reporting on.
    //
    hr = StringCchCopy (ReportInformation.wzApplicationName,
                        ARRAYSIZE(ReportInformation.wzApplicationName),
                        L"Add File Sample");

    if (FAILED(hr)) {
        wprintf (L"Failed to set the application name, error: 0x%08X\n", hr);
        goto End;
    }

    //
    // Set the application path.
    //
    hr = StringCchCopy (ReportInformation.wzApplicationPath,
                        ARRAYSIZE(ReportInformation.wzApplicationPath),
                        L"Sample Application Path");

    if (FAILED(hr)) {
        wprintf (L"Failed to set the application path, error: 0x%08X\n", hr);
        goto End;
    }

    //
    // Set a description of the problem.
    //
    hr = StringCchCopy (ReportInformation.wzDescription,
                        ARRAYSIZE(ReportInformation.wzDescription),
                        L"Sample Problem Description");

    if (FAILED(hr)) {
        wprintf (L"Failed to set the description, error: 0x%08X\n", hr);
        goto End;
    }

    //
    // Set a friendly name for the event - this is the display name.
    //
    hr = StringCchCopy (ReportInformation.wzFriendlyEventName,
                        ARRAYSIZE(ReportInformation.wzFriendlyEventName),
                        L"Sample Problem Friendly Name");

    if (FAILED(hr)) {
        wprintf (L"Failed to set the friendly event name, error: 0x%08X\n", hr);
        goto End;
    }

    //
    // Now actually call into WER to create the report with the above structure.
    // We get back a handle to the report.
    //
    hr = WerReportCreate (L"SampleGenericReport",
                          WerReportCritical,
                          &ReportInformation,
                          &ReportHandle);

    if (FAILED(hr)) {
        wprintf (L"WerReportCreate failed, error: 0x%08X\n", hr);
        goto End;
    }

    wprintf (L"Created the report.\n");

    //
    // Now set the report parameters (3 parameters in this case).
    //
    // These parameters as used as bucketing parameters on the WER server.
    //
    hr = WerReportSetParameter(ReportHandle, 0, L"Param1", L"Value1");

    if (FAILED(hr)) {
        wprintf (L"Set parameter failed for parameter 0, error: 0x%08X\n", hr);
        goto End;
    }

    hr = WerReportSetParameter(ReportHandle, 1, L"Param2", L"Value2");

    if (FAILED(hr)) {
        wprintf (L"Set parameter failed for parameter 1, error: 0x%08X\n", hr);
        goto End;
    }

    hr = WerReportSetParameter(ReportHandle, 2, L"Param3", L"Value3");

    if (FAILED(hr)) {
        wprintf (L"Set parameter failed for parameter 3, error: 0x%08X\n", hr);
        goto End;
    }

    //
    // Add a custom file to the report.
    //
    // Here we will add the Notepad program executable, from the current
    // Windows system32 directory (by using the %WINDIR% environment variable).
    //
    hr = WerReportAddFile (ReportHandle,
                           L"%WINDIR%\\system32\\notepad.exe",
                           WerFileTypeOther,
                           WER_FILE_ANONYMOUS_DATA);

    if (FAILED(hr)) {
        wprintf (L"WerReportAddfile failed, error: 0x%08X\n", hr);
        goto End;
    }

    wprintf (L"Added a custom file (notepad.exe) to the report.\n");

    //
    // Submit the report.
    //
    // After the application calls this API, WER collects the specified data.
    //
    hr = WerReportSubmit (ReportHandle, WerConsentNotAsked, 0, &SubmitResult);

    if (FAILED(hr)) {
        wprintf (L"WerReportSubmit failed, error: 0x%08X\n", hr);
        goto End;
    }

    wprintf (L"Submission result was %u (WER_SUBMIT_RESULT enum).\n", SubmitResult);

    hr = S_OK;


End:

    if (ReportHandle) {
        //
        // Close the handle to the report. Call this API after report has been
        // submitted, or the handle is no longer needed.
        //
        hr = WerReportCloseHandle (ReportHandle);

        if (FAILED(hr)) {
            wprintf (L"WerReportCloseHandle failed, error: 0x%08X\n", hr);
        }
    }

    if (FAILED(hr)) {
        //
        // Return -1 to indicate that an error occured
        //
        return -1;
    }

    return 0;
}
