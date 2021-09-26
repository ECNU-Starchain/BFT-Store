#ifndef GFARITH_CPUDETECT_H
#define GFARITH_CPUDETECT_H

#if defined(__arm__) || defined(__TARGET_ARCH_ARM)
#if defined(__ARM_ARCH_7__) \\
        || defined(__ARM_ARCH_7A__) \\
        || defined(__ARM_ARCH_7R__) \\
        || defined(__ARM_ARCH_7M__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7)
#define GFARITH_ARCH_ARMV7
#elif defined(__ARM_ARCH_6__) \\
        || defined(__ARM_ARCH_6J__) \\
        || defined(__ARM_ARCH_6T2__) \\
        || defined(__ARM_ARCH_6Z__) \\
        || defined(__ARM_ARCH_6K__) \\
        || defined(__ARM_ARCH_6ZK__) \\
        || defined(__ARM_ARCH_6M__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 6)
#define GFARITH_ARCH_ARMV6
#elif defined(__ARM_ARCH_5TEJ__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 5)
#define GFARITH_ARCH_ARMV5
#else
#define GFARITH_ARCH_ARM
#endif
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
#define GFARITH_ARCH_I386
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
#define GFARITH_ARCH_X86_64
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#define GFARITH_ARCH_IA64

#else
#define GFARITH_ARCH_UNKNOWN
#endif

#endif
