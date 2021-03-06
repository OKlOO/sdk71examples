RegisterFile Sample
===================

Demonstrates
------------
This sample demonstrates how to use the WerRegisterFile API.

The WerRegisterFile API allows an application to include a custom file that is collected when WER
creates an error report in the event that an application crashes or hangs.

In this example, we'll create a mini log file and register it for collection by WER. The contents
of the registered file are picked up at the time of the crash, not at the time of registration.

Languages
---------
This sample is implemented in the C programming language.

Files
-----
     RegisterFile.c             Source code for the sample
     RegisterFile.vcproj        Visual C++ 2008 project file
     RegisterFile.sln           Visual Studio 2008 solution file

Prerequisites
-------------
Windows SDK v6.0 or newer.
Windows Vista or Windows Server 2008 operating system, or newer.

Building the Sample
-------------------
To build the sample using the command prompt:
     1. Open the Windows SDK command line shell and navigate to the directory.
     2. Type: msbuild RegisterFile.sln

To build the sample using Visual Studio 2008 or Visual C++ 2008 Express:
     1. Open Windows Explorer and navigate to the RegisterFile directory.
     2. Double-click the icon for the RegisterFile.sln file to open the file in Visual Studio.
     3. Add the path to the Windows SDK Include folder to the "Additional Include Directories" setting of the project.
     4. Add the path to the Windows SDK Lib folder to the "Additional Library Directories" setting of the project.
     5. In the Build menu, select Build Solution. The application will be built in the default \Debug or \Release directory.

Running the Sample
------------------
1. Open a command prompt and navigate to the Release or Debug directory under RegisterFile.
2. Run RegisterFile.exe (no command-line arguments).

Expected outcome:   The application crashes.
                    Windows Error Reporting UI comes up asking to check for solutions to the problem.
                    (If you are not seeing any UI, then confirm that the DontShowUI and ForceQueue settings are both set
                    to 0. The list of WER settings can be found in the SDK documentation under the "WER Settings" topic).

                    Choosing to check for solutions will collect the file (mylogfile.txt) and add it to the report.

                    The file should be present in cab files downloaded from WinQual.

                    There are many conditions which can cause WerRegisterFile to succeed, but for the file not to be
                    collected when crash reporting actually happens. As a developer debugging your code, you can make sure
                    that the registered file is present in the report locally, before it is sent out to the WER services
                    backend. You should not try to code the following checks in your code as these are implementation
                    details that are subject to change, and are only meant to help diagnose local issues.
                    To confirm the presence of the registered file (mylogfile.txt) in the report locally,
                    1) Force the system to queue the report instead of sending it out. This can be done by:
                        1.1) Removing network connectivity.
                        1.2) OR, setting the ForceQueue setting to 1. Once it is set to 1, there will be no UI when
                             running the sample. This is expected, and the report will be submitted straight to the queue.

                             Be sure to reset the setting to its original value after, as it affects all the applications
                             on your system.

                    2) Check the Problem Details of the report that was just submitted by the sample code.
                       The Problem Details view can be reached by double-clicking on the report in the Problem Reports
                       and Solutions control panel applet on Windows Vista/Windows Server 2008, or Action Center on
                       Windows 7/Windows Server 2008 R2 (under the Maintenance, View reliability history, View problem
                       details selection). The problem details should list the registered file (mylogfile.txt) as one of
                       the files in the report, like so:

                        Description
                            Faulting Application Path:  C:\SDKSamples\RegisterFile\Debug\RegisterFile.exe

                        Problem signature
                            Problem Event Name: APPCRASH
                            Application Name:   RegisterFile.exe
                            Application Version:    0.0.0.0
                            Application Timestamp:  4a11da86
                            Fault Module Name:  RegisterFile.exe
                            Fault Module Version:   0.0.0.0
                            Fault Module Timestamp: 4a11da86
                            Exception Code: c0000005
                            Exception Offset:   000115b5
                            OS Version: 6.1.7105.2.0.0.256.1
                            Locale ID:  1033
                            Additional Information 1:   0a9e
                            Additional Information 2:   0a9e372d3b4ad19135b953a78882e789
                            Additional Information 3:   0a9e
                            Additional Information 4:   0a9e372d3b4ad19135b953a78882e789

                        Files that help describe the problem
                            mylogfile.txt
                            AppCompat.txt
                            WERInternalMetadata.xml
                            memory.hdmp
                            minidump.mdmp


THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Copyright (c) Microsoft Corporation. All rights reserved.
