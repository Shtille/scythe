#ifndef __SCYTHE_PLATFORM_INNER_H__
#define __SCYTHE_PLATFORM_INNER_H__

// Basic platform functions that shouldn't be visible from includes

bool PlatformInit();
void PlatformTerminate();
bool PlatformWindowCreate();
void PlatformWindowDestroy();
bool PlatformNeedQuit();
void PlatformPollEvents();

#endif