/*
Copyright (C) 2001 Paul Davis
Copyright (C) 2004-2008 Grame
Copyright (C) 2013 Samsung Electronics

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#ifndef __JackAndroidThread__
#define __JackAndroidThread__

#include "JackThread.h"
#include <pthread.h>

namespace Jack
{

/* use 512KB stack per thread - the default is way too high to be feasible
 * with mlockall() on many systems */
#define THREAD_STACK 524288

enum
{
  PTHREAD_CANCEL_DEFERRED,
  PTHREAD_CANCEL_ASYNCHRONOUS
};

/*!
\brief The POSIX thread base class.
*/

class SERVER_EXPORT JackAndroidThread : public detail::JackThreadInterface
{

    protected:

        jack_native_thread_t fThread;
        static void* ThreadHandler(void* arg);
        static void thread_exit_handler(int sig);
		static void RequestPriority(int pid, int tid, int priority);

    public:

        JackAndroidThread(JackRunnableInterface* runnable, bool real_time, int priority, int cancellation)
                : JackThreadInterface(runnable, priority, real_time, cancellation), fThread((jack_native_thread_t)NULL)
        {}
        JackAndroidThread(JackRunnableInterface* runnable, int cancellation = PTHREAD_CANCEL_ASYNCHRONOUS)
                : JackThreadInterface(runnable, 0, false, cancellation), fThread((jack_native_thread_t)NULL)
        {}

        int Start();
        int StartSync();
        int Kill();
        int Stop();
        void Terminate();

        int AcquireRealTime();                  // Used when called from another thread
        int AcquireSelfRealTime();              // Used when called from thread itself

        int AcquireRealTime(int priority);      // Used when called from another thread
        int AcquireSelfRealTime(int priority);  // Used when called from thread itself

        int DropRealTime();                     // Used when called from another thread
        int DropSelfRealTime();                 // Used when called from thread itself

        jack_native_thread_t GetThreadID();
        bool IsThread();

        static int AcquireRealTimeImp(jack_native_thread_t thread, int priority);
        static int AcquireRealTimeImp(jack_native_thread_t thread, int priority, UInt64 period, UInt64 computation, UInt64 constraint)
        {
            return JackAndroidThread::AcquireRealTimeImp(thread, priority);
        }
        static int DropRealTimeImp(jack_native_thread_t thread);
        static int StartImp(jack_native_thread_t* thread, int priority, int realtime, void*(*start_routine)(void*), void* arg);
        static int StopImp(jack_native_thread_t thread);
        static int KillImp(jack_native_thread_t thread);
};

SERVER_EXPORT void ThreadExit();

} // end of namespace


#endif
