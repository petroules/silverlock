/*
* High Resolution Timestamp Entropy Source
* (C) 1999-2009,2011 Jack Lloyd
*
* Distributed under the terms of the Botan license
*/

#include <botan/internal/hres_timer.h>
#include <botan/cpuid.h>
#include <botan/time.h>

#if defined(BOTAN_TARGET_OS_IS_WINDOWS)
  #include <windows.h>
#endif

namespace Botan {

/*
* Get the timestamp
*/
void High_Resolution_Timestamp::poll(Entropy_Accumulator& accum)
   {
   // If Windows, grab the Performance Counter (usually TSC or PIT)
#if defined(BOTAN_TARGET_OS_IS_WINDOWS)
   {
   LARGE_INTEGER tv;
   ::QueryPerformanceCounter(&tv);
   accum.add(tv.QuadPart, 0);
   }
#endif

#if defined(BOTAN_TARGET_OS_HAS_CLOCK_GETTIME)

#define CLOCK_POLL(src)                     \
   do {                                     \
     struct timespec ts;                    \
     clock_gettime(src, &ts);               \
     accum.add(&ts, sizeof(ts), 0);         \
   } while(0)

#if defined(CLOCK_REALTIME)
   CLOCK_POLL(CLOCK_REALTIME);
#endif

#if defined(CLOCK_MONOTONIC)
   CLOCK_POLL(CLOCK_MONOTONIC);
#endif

#if defined(CLOCK_MONOTONIC_RAW)
   CLOCK_POLL(CLOCK_MONOTONIC_RAW);
#endif

#if defined(CLOCK_PROCESS_CPUTIME_ID)
   CLOCK_POLL(CLOCK_PROCESS_CPUTIME_ID);
#endif

#if defined(CLOCK_THREAD_CPUTIME_ID)
   CLOCK_POLL(CLOCK_THREAD_CPUTIME_ID);
#endif

#undef CLOCK_POLL

#endif

#if BOTAN_USE_GCC_INLINE_ASM

   u64bit rtc = 0;

#if defined(BOTAN_TARGET_CPU_IS_X86_FAMILY)
   if(CPUID::has_rdtsc()) // not availble on all x86 CPUs
      {
      u32bit rtc_low = 0, rtc_high = 0;
      asm volatile("rdtsc" : "=d" (rtc_high), "=a" (rtc_low));
      rtc = (static_cast<u64bit>(rtc_high) << 32) | rtc_low;
      }

#elif defined(BOTAN_TARGET_CPU_IS_PPC_FAMILY)
   u32bit rtc_low = 0, rtc_high = 0;
   asm volatile("mftbu %0; mftb %1" : "=r" (rtc_high), "=r" (rtc_low));
   rtc = (static_cast<u64bit>(rtc_high) << 32) | rtc_low;

#elif defined(BOTAN_TARGET_ARCH_IS_ALPHA)
   asm volatile("rpcc %0" : "=r" (rtc));

#elif defined(BOTAN_TARGET_ARCH_IS_SPARC64) && !defined(BOTAN_TARGET_OS_IS_OPENBSD)
   asm volatile("rd %%tick, %0" : "=r" (rtc));

#elif defined(BOTAN_TARGET_ARCH_IS_IA64)
   asm volatile("mov %0=ar.itc" : "=r" (rtc));

#elif defined(BOTAN_TARGET_ARCH_IS_S390X)
   asm volatile("stck 0(%0)" : : "a" (&rtc) : "memory", "cc");

#elif defined(BOTAN_TARGET_ARCH_IS_HPPA)
   asm volatile("mfctl 16,%0" : "=r" (rtc)); // 64-bit only?

#endif

   // Don't count the timestamp as contributing entropy
   accum.add(rtc, 0);

#endif
   }

}
