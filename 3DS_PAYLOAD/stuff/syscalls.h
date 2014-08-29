#ifndef SYSCALLS_H
#define SYSCALLS_H

#define CURRENT_PROCESS 0xFFFF8001
#define CURRENT_THREAD 	0xFFFF8000 
#define svc(code) asm volatile ("svc %[immediate]"::[immediate] "I" (code))

/*
Unknown Types
*/
typedef void* ArbitrationType;
typedef void* SystemInfoType;
typedef void* ProcessInfoType;
typedef void* ThreadInfoType;
typedef void* ResetType;
typedef void* func;
typedef void* Interrupt;
typedef void* DmaState;
typedef void* DmaConfig;
/*
End of Unknown Types
*/


/*__attribute__ ((noinline)) Result ConnectToPort(Handle* out, const char* portName)
{
    svc(0x2D);
}

/*__attribute__ ((noinline)) Result CreateThread(Handle* thread, func entrypoint, u32 arg, u32 stacktop, s32 threadpriority, s32 processorid)
{
    asm("str r4, [r11,#0x14]");
    asm("mov r5, [r11,#0x14]");
    svc(0x08);
}*/
/*__attribute__ ((noinline)) void ExitThread(void)
{
    svc(0x09);
}
__attribute__ ((noinline)) void SleepThread(s64 nanoseconds)
{
    svc(0x0A);
}
__attribute__ ((noinline)) Result GetThreadPriority(s32* priority, Handle thread)
{
    svc(0x0B);
}                                                                                                                                               
__attribute__ ((noinline)) Result SetThreadPriority(Handle thread, s32 priority)
{
    svc(0x0C);
}                                                                                                                                               
__attribute__ ((noinline)) Result CreateMutex(Handle* mutex, bool initialLocked)
{
    svc(0x13);
}                                                                                                                                               
__attribute__ ((noinline)) Result ReleaseMutex(Handle mutex)
{
    svc(0x14);
}                                                                                                                                               
__attribute__ ((noinline)) Result CreateSemaphore(Handle* semaphore, s32 initialCount, s32 maxCount)
{
    svc(0x15);
}                                                                                                                                               
__attribute__ ((noinline)) Result ReleaseSemaphore(s32* count, Handle semaphore, s32 releaseCount)
{
    svc(0x16);
}                                                                                                                                               
__attribute__ ((noinline)) Result CreateEvent(Handle* event, ResetType resettype)
{
    svc(0x17);
}                                                                                                                                               
__attribute__ ((noinline)) Result SignalEvent(Handle event)
{
    svc(0x18);
}                                                                                                                                               
__attribute__ ((noinline)) Result ClearEvent(Handle event)
{
    svc(0x19);
}                                                                                                                                               
__attribute__ ((noinline)) Result CreateTimer(Handle* timer, ResetType resettype)
{
    svc(0x1A);
}                                                                                                                                               
__attribute__ ((noinline)) Result SetTimer(Handle timer, s64 initial, s64 interval)
{
    svc(0x1B);
}                                                                                                                                               
__attribute__ ((noinline)) Result CancelTimer(Handle timer)
{
    svc(0x1C);
}                                                                                                                                               
__attribute__ ((noinline)) Result ClearTimer(Handle timer)
{
    svc(0x1D);
}                                                                                                                                               
__attribute__ ((noinline)) Result CreateAddressArbiter(Handle* arbiter)
{
    svc(0x21);
}                                                                                                                                               
__attribute__ ((noinline)) Result ArbitrateAddress(Handle arbiter, u32 addr, ArbitrationType type, s32 value)
{
    svc(0x22);
}                                                                                                                                               
__attribute__ ((noinline)) Result CloseHandle(Handle handle)
{
    svc(0x23);
}                                                                                                                                               
__attribute__ ((noinline)) Result WaitSynchronization1(Handle handle, s64 nanoseconds)
{
    svc(0x24);
}                                                                                                                                               
__attribute__ ((noinline)) Result WaitSynchronizationN(s32* out, Handle* handles, s32 handlecount, bool waitAll, s64 nanoseconds)
{
    svc(0x25);
}                                                                                                                                               
__attribute__ ((noinline)) Result DuplicateHandle(Handle* out, Handle original)
{
    svc(0x27);
}                                                                                                                                               
__attribute__ ((noinline)) s64 GetSystemTick(void)
{
    svc(0x28);
}                                                                                                                                               
__attribute__ ((noinline)) Result GetSystemInfo(s64* out, SystemInfoType type, s32 param)
{
    svc(0x2A);
}                                                                                                                                               
__attribute__ ((noinline)) Result GetProcessInfo(s64* out, Handle process, ProcessInfoType type)
{
    svc(0x2B);
}                                                                                                                                               
__attribute__ ((noinline)) Result GetThreadInfo(s64* out, Handle thread, ThreadInfoType type)
{
    svc(0x2C);
}                                                                                                                                               
__attribute__ ((noinline)) Result GetProcessId(u32* processId, Handle process)
{
    svc(0x35);
}                                                                                                                                               
__attribute__ ((noinline)) Result GetThreadId(u32* threadId, Handle thread)
{
    svc(0x37);
}                                                                                                                                               
__attribute__ ((noinline)) Break(char const* reason)
{
    svc(0x3D);
}                                                                                                                                               
__attribute__ ((noinline)) OutputDebugString(char const* string, int level) 
{
    svc(0x50);
}                                                                                                                                               
__attribute__ ((noinline)) Result BindInterrupt(Interrupt name, Handle syncObject, s32 priority, bool isManualClear)
{
    svc(0x51);
}                                                                                                                                               
__attribute__ ((noinline)) Result UnbindInterrupt(Interrupt name, Handle syncObject)
{
    svc(0x51);
}                                                                                                                                               
__attribute__ ((noinline)) Result InvalidateProcessDataCache(Handle process, void* addr, u32 size)
{
    svc(0x52);
}                                                                                                                                               
__attribute__ ((noinline)) Result StoreProcessDataCache(Handle process, void const* addr, u32 size)
{
    svc(0x53);
}                                                                                                                                               
__attribute__ ((noinline)) Result FlushProcessDataCache(Handle process, void const* addr, u32 size)
{
    svc(0x54);
}                                                                                                                                               
__attribute__ ((noinline)) Result StartInterProcessDma(Handle* dma, Handle dstProcess, void* dst, Handle srcProcess, const void* src, u32 size, const DmaConfig* config )
{
    svc(0x55);
}                                                                                                                                               
__attribute__ ((noinline)) Result StopDma(Handle dma)
{
    svc(0x56);
}                                                                                                                                               
__attribute__ ((noinline)) Result GetDmaState(DmaState* state, Handle dma)
{
    svc(0x57);
}                                                                                                                                               
__attribute__ ((noinline)) RestartDma(Handle nn, void* noidea, void const* noidea2, unsigned int huh, signed char what)
{
    svc(0x58);
}                                                                                                                                               
__attribute__ ((noinline)) KernelSetState(unsigned int Type, unsigned int Param0, unsigned int Param1, unsigned int Param2)
{
    svc(0x7C);
}                                                                                                                                               

volatile __attribute__ ((noinline)) Result CreateThread(Handle* thread, func entrypoint, u32 arg, u32 stacktop, s32 threadpriority, s32 processorid){
    Result r;
    Handle* h;
    __asm(
        "mov r0, %5\n"
        "mov r1, %2\n"
        "mov r2, %3\n"
        "mov r3, %4\n"
        "mov r4, %6\n"
        "svc #0x08\n"
        "mov %0, r0\n"
        :"=r"(r),"=r"(thread)
        :"r"(entrypoint),"r"(arg),"r"(stacktop),"r"(threadpriority),"r"(processorid)
        :);
    return r;
}
*/
#endif