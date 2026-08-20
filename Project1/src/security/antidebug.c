#include "security/antidebug.h"

#if defined(__linux__)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int antidebug_linux(void)
{
    FILE *file =
        fopen(
            "/proc/self/status",
            "r"
        );

    if (file == NULL)
    {
        return 0;
    }

    char line[256];

    int detected =
        0;

    while (fgets(
               line,
               sizeof(line),
               file
           ) != NULL)
    {
        if (strncmp(
                line,
                "TracerPid:",
                10
            ) == 0)
        {
            int tracer_pid =
                atoi(
                    line + 10
                );

            if (tracer_pid != 0)
            {
                detected =
                    1;
            }

            break;
        }
    }

    fclose(
        file
    );

    return detected;
}

#elif defined(__APPLE__)

#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>

static int antidebug_macos(void)
{
    int mib[4];

    struct kinfo_proc info;

    size_t size =
        sizeof(info);

    mib[0] =
        CTL_KERN;

    mib[1] =
        KERN_PROC;

    mib[2] =
        KERN_PROC_PID;

    mib[3] =
        getpid();

    info.kp_proc.p_flag =
        0;

    if (sysctl(
            mib,
            4,
            &info,
            &size,
            NULL,
            0
        ) != 0)
    {
        return 0;
    }

    return
        (info.kp_proc.p_flag &
         P_TRACED) != 0;
}

#endif

int antidebug_detected(void)
{
#if defined(__linux__)

    return
        antidebug_linux();

#elif defined(__APPLE__)

    return
        antidebug_macos();

#else

    return 0;

#endif
}
