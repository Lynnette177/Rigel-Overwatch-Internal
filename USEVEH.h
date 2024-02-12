#pragma once
#include <Windows.h>
#include <TlHelp32.h>

#define MAKEULONGLONG(ldw, hdw) ((ULONGLONG(hdw) << 32) | ((ldw) & 0xFFFFFFFF))

class UseVEH
{
public:
    DWORD GetMainThreadId()
    {
        DWORD mainThreadId = 0;
        ULONGLONG minCreateTime = MAXULONGLONG;

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot != INVALID_HANDLE_VALUE)
        {
            THREADENTRY32 entry;
            entry.dwSize = sizeof(THREADENTRY32);
            BOOL success = Thread32First(snapshot, &entry);
            while (success)
            {
                if (entry.th32OwnerProcessID == GetCurrentProcessId())
                {
                    HANDLE thread = OpenThread(THREAD_QUERY_INFORMATION, TRUE, entry.th32ThreadID);
                    if (thread)
                    {
                        FILETIME times[4] = { 0 };
                        if (GetThreadTimes(thread, &times[0], &times[1], &times[2], &times[3]))
                        {
                            ULONGLONG test = MAKEULONGLONG(times[0].dwLowDateTime, times[0].dwHighDateTime);
                            if (test && test < minCreateTime)
                            {
                                minCreateTime = test;
                                mainThreadId = entry.th32ThreadID;
                            }
                        }
                        CloseHandle(thread);
                    }
                }
                success = Thread32Next(snapshot, &entry);
            }
            CloseHandle(snapshot);
        }
        return mainThreadId;
    }

    HANDLE GetMainThreadHandle()
    {
        DWORD threadId = GetMainThreadId();
        return OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, TRUE, threadId);
    }
};

extern UseVEH* USEVEH = new UseVEH();