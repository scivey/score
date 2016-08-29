#include "aliens/vendored/smhasher/Platform.h"
#include <stdio.h>

#define NS_BEGIN \
namespace aliens { namespace vendored { namespace smhasher {

#define NS_END \
}}} // aliens::vendored::smhasher


NS_BEGIN
void testRDTSC ( void )
{
  int64_t temp = rdtsc();

  printf("%d",(int)temp);
}
NS_END

#if defined(_MSC_VER)

#include <windows.h>

NS_BEGIN
void SetAffinity ( int cpu )
{
  SetProcessAffinityMask(GetCurrentProcess(),cpu);
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
}
NS_END

#else

#include <sched.h>

NS_BEGIN
void SetAffinity ( int /*cpu*/ )
{
#if !defined(__CYGWIN__) && !defined(__APPLE__)
  cpu_set_t mask;

  CPU_ZERO(&mask);

  CPU_SET(2,&mask);

  if( sched_setaffinity(0,sizeof(mask),&mask) == -1)
  {
    printf("WARNING: Could not set CPU affinity\n");
  }
#endif
}

NS_END
#endif
