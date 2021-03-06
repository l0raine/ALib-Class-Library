// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"


#if !defined (HPP_ALIB_TIME_TIMEPOINT)
    #include "alib/time/timestamp.hpp"
#endif

#if !defined (HPP_ALIB_THREADS_THREADLOCK)
    #include "alib/threads/threadlocknr.hpp"
#endif


using namespace aworx;

#include <iostream>

#define TESTCLASSNAME       CPP_ALib_Threads
#include "aworx_unittests.hpp"

using namespace std;

namespace ut_aworx {

UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- helper classes
//--------------------------------------------------------------------------------------------------

class Test_ThreadLock_SharedInt
{
    public:        int val= 0;
};

class Test_ThreadLock_TestThread : public Thread
{
    public:        AWorxUnitTesting* UT;
    public:        ThreadLock*       Mutex;
    public:        int               HoldTime;
    public:        int               Repeats;
    public:        bool              Verbose;
    public:        int               TResult= 1;
    public:        Test_ThreadLock_SharedInt* Shared;

    public:        Test_ThreadLock_TestThread( AWorxUnitTesting* pUT, const String& tname, ThreadLock* lock, int holdTime, int repeats, bool verbose, Test_ThreadLock_SharedInt* shared )
    :Thread( tname )
    {
        this->UT=        pUT;
        this->Mutex=     lock;
        this->HoldTime=  holdTime;
        this->Repeats=   repeats;
        this->Verbose=   verbose;
        this->Shared=    shared;
    }

    public: void Run()
    {
        AWorxUnitTesting &ut= *UT;
        UT_EQ( GetId(), lib::THREADS.CurrentThread()->GetId() );

        for ( int i= 0; i < Repeats ; i++ )
        {
            if (Verbose) { UT_PRINT( "Thread {!Q} acquiring lock...",GetName() ); }
            Mutex->Acquire(ALIB_CALLER_PRUNED);
            if (Verbose) { UT_PRINT( "Thread {!Q} has lock.", GetName() ); }

                int sVal= ++Shared->val;

                ALib::SleepMicros( HoldTime );

                Shared->val= sVal -1;

            if (Verbose) { UT_PRINT( "Thread {!Q} releasing lock.", GetName() ); }
            Mutex->Release();
            if (Verbose) { UT_PRINT( "Thread {!Q} released lock." , GetName() ); }
        }

        TResult= 0;
        UT_PRINT( "Thread {!Q} terminates.", GetName() );

    }
};

//--------------------------------------------------------------------------------------------------
//--- ThreadSimple
//--------------------------------------------------------------------------------------------------
UT_METHOD( ThreadSimple )
{
    UT_INIT();

    // create and delete
    {
        Thread t;
        UT_PRINT( "Thread object on stack, not started. Alive= ", t.IsAlive() );
    }
    {
        Thread* t= new Thread();
        UT_PRINT( "Thread object on heap, not started. Alive= ", t->IsAlive() );
        delete t;
    }
    {
        Thread* t= new Thread();
        t->Start();
        UT_PRINT( "Empty Thread object, started. Alive= ", t->IsAlive() );
        delete t;
    }

    // simple runnable
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wshadow"
    #endif
    class runner: public Runnable
    {
        public:
        AWorxUnitTesting *pUT;
        int a= 0;
        #if ALIB_FEAT_THREADS
            runner(AWorxUnitTesting *put) { this->pUT= put; }
        #endif
        virtual void Run()
        {
            AWorxUnitTesting& ut= *pUT;
            UT_PRINT( "Runnable running in thread ", lib::THREADS.CurrentThread()->GetId() );  ALib::SleepMillis(1); a++;
        }
    };
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif

    #if ALIB_FEAT_THREADS
    {
        runner r(&ut);
        {
            Thread t(&r);
            t.Start();
        }

        UT_EQ( 1, r.a ); // thread is deleted, runner should have executed here

        {
            Thread t(&r);
            t.Start();
            UT_EQ( 1, r.a ); // runner waits a millisec, we should be quicker
            int cntWait= 0;
            while( t.IsAlive() )
            {
                UT_PRINT( "  {} waiting for {} to finish", lib::THREADS.CurrentThread()->GetId(), t.GetId() );
                ALib::SleepMicros(250);
                cntWait++;
            }
            UT_TRUE( cntWait < 5 );
            UT_PRINT( "  Result should be 2: ", r.a  );
            UT_EQ( 2, r.a );
        }
    }
    #endif
}

//--------------------------------------------------------------------------------------------------
//--- ThreadLockSimple
//--------------------------------------------------------------------------------------------------
UT_METHOD( ThreadLockSimple )
{
    UT_INIT();

    lib::lang::Report::GetDefault().PushHaltFlags( false, false );

    // lock a recursive lock
    {
        ThreadLock aLock;
        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 1, aLock.CountAcquirements() );
        aLock.Release();                      UT_EQ ( 0, aLock.CountAcquirements() );

        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 1, aLock.CountAcquirements() );
        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 2, aLock.CountAcquirements() );
        aLock.Release();                      UT_EQ ( 1, aLock.CountAcquirements() );

        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 2, aLock.CountAcquirements() );
        aLock.Release();                            UT_EQ ( 1, aLock.CountAcquirements() );
        aLock.Release();                            UT_EQ ( 0, aLock.CountAcquirements() );

        // set unsafe
        aLock.SetSafeness( Safeness::Unsafe );      UT_EQ ( 0, aLock.CountAcquirements() );
        aLock.SetSafeness( Safeness::Safe   );      UT_EQ ( 0, aLock.CountAcquirements() );

        aLock.SetSafeness( Safeness::Unsafe );      UT_EQ ( 0, aLock.CountAcquirements() );
        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 1, aLock.CountAcquirements() );
        aLock.Release();                            UT_EQ ( 0, aLock.CountAcquirements() );

        // unsafe
        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 1, aLock.CountAcquirements() );
        UT_PRINT( "Expecting error: set unsafe when already locked" );
        aLock.SetSafeness( Safeness::Safe   );      UT_EQ ( 1, aLock.CountAcquirements() );
        UT_PRINT( "Expecting error: destruction while locked" );
    }

    // safe (new lock)
    {
        ThreadLock aLock;
        aLock.Acquire(ALIB_CALLER_PRUNED); UT_EQ ( 1, aLock.CountAcquirements() );
        UT_PRINT( "Expecting error: set unsafe when already locked" );
        aLock.SetSafeness( Safeness::Unsafe );   UT_EQ ( 1, aLock.CountAcquirements() );
        aLock.Release();                         UT_EQ ( 0, aLock.CountAcquirements() );
        UT_PRINT( "Expecting error: release without lock" );
        aLock.Release();                         UT_EQ (-1, aLock.CountAcquirements() );
    }

    // test warnings (10) locks:
    {
        ThreadLock aLock;
        UT_PRINT( "Two warnings should come now: " );
        for (int i= 0; i<20; i++)
            aLock.Acquire(ALIB_CALLER_PRUNED);
        UT_TRUE ( aLock.CountAcquirements() > 0 );
        for (int i= 0; i<20; i++)
            aLock.Release();
        UT_EQ ( 0, aLock.CountAcquirements() );
    }

    // test a non-recursive lock
    {
        ThreadLock aLock( LockMode::SingleLocks );
        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 1, aLock.CountAcquirements() );
        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 1, aLock.CountAcquirements() );
        aLock.Release();                            UT_EQ ( 0, aLock.CountAcquirements() );

        UT_PRINT( "One Error should come now: " );
        aLock.Release();                            UT_EQ ( 0, aLock.CountAcquirements() );
    }

    lib::lang::Report::GetDefault().PopHaltFlags();
}

//--------------------------------------------------------------------------------------------------
//--- ThreadLockThreaded
//--------------------------------------------------------------------------------------------------
UT_METHOD( ThreadLockThreaded )
{
    UT_INIT();

    lib::lang::Report::GetDefault().PushHaltFlags( false, false );

        ThreadLock aLock;
        Test_ThreadLock_SharedInt* shared= new Test_ThreadLock_SharedInt();
        UT_PRINT( "starting thread locked" );
        aLock.Acquire(ALIB_CALLER_PRUNED);
        Test_ThreadLock_TestThread* t= new Test_ThreadLock_TestThread( &ut, ASTR("A Thread"), &aLock, 10, 1, true, shared );
        t->Start();
        UT_PRINT( "We wait 1100 micro seconds. This should give a warning! " );
        ALib::SleepMicros( 1100 );
        aLock.Release();

        // wait until t ended
        while (t->IsAlive())
            ALib::SleepMillis( 1 );

        // now we do the same with a lower wait limit, no error should come
        aLock.WaitWarningTimeLimitInMillis= 5;
        aLock.Acquire(ALIB_CALLER_PRUNED);
        delete t;
        t= new Test_ThreadLock_TestThread( &ut, ASTR("A Thread"), &aLock, 10, 1, true, shared );
        t->Start();
        UT_PRINT( "We wait 10 micro seconds. This should NOT give a warning! " );
        ALib::SleepMicros( 10 );
        aLock.Release();

        // wait until t ended
        while (t->IsAlive())
            ALib::SleepMillis( 1 );
        delete t;
        delete shared;

    lib::lang::Report::GetDefault().PopHaltFlags();
}

//--------------------------------------------------------------------------------------------------
//--- SmartLockTest
//--------------------------------------------------------------------------------------------------
#if ALOX_DBG_LOG // in release, no ALIB report is sent.
UT_METHOD( SmartLockTest )
{
    UT_INIT();

    lib::lang::Report::GetDefault().PushHaltFlags( false, false );
    ut.lox.CntLogCalls= 0;

    // SmartLock with null-users
    {
        SmartLock sl;                          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        UT_PRINT( "One warning should follow" ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 1 ); ut.lox.CntLogCalls= 0;
    }

    // SmartLock with threadlocks
    {
        ThreadLock tl1, tl2, tl3;
        SmartLock  sl;                         UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        UT_PRINT( "One warning should follow" ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 1 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        UT_PRINT( "One warning should follow" ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 1 ); ut.lox.CntLogCalls= 0;
    }

    // mixed
    {
        ThreadLock tl1, tl2, tl3;
        SmartLock  sl;                         UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        UT_PRINT( "One warning should follow" ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 1 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.AddAcquirer   ( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        UT_PRINT( "One warning should follow" ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 1 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        UT_PRINT( "Three warnings should follow" ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 1 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 1 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 1 ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 0 ); ut.lox.CntLogCalls= 0;
        UT_PRINT( "One warning should follow" ); ut.lox.CntLogCalls= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe );  UT_TRUE( ut.lox.CntLogCalls== 1 ); ut.lox.CntLogCalls= 0;
    }
    lib::lang::Report::GetDefault().PopHaltFlags();
}
#endif

//--------------------------------------------------------------------------------------------------
//--- HeavyLoad
//--------------------------------------------------------------------------------------------------
UT_METHOD( HeavyLoad )
{
    UT_INIT();

    ThreadLock aLock;

    // uncomment this for unsafe mode
    //aLock.SetUnsafe( true ); aLock.UseAssertions= false;

    Test_ThreadLock_SharedInt   shared;
    int                         holdTime=    0;
    int                         repeats=    5000;
    bool                        verbose=    false;

    Test_ThreadLock_TestThread* t1= new Test_ThreadLock_TestThread( &ut, ASTR("A"), &aLock, holdTime, repeats, verbose, &shared );
    Test_ThreadLock_TestThread* t2= new Test_ThreadLock_TestThread( &ut, ASTR("B"), &aLock, holdTime, repeats, verbose, &shared );
    Test_ThreadLock_TestThread* t3= new Test_ThreadLock_TestThread( &ut, ASTR("C"), &aLock, holdTime, repeats, verbose, &shared );
    UT_PRINT( "starting three threads" );
    t1->Start();
    t2->Start();
    t3->Start();

    // wait until all ended
    while ( t1->IsAlive() || t2->IsAlive() || t3->IsAlive() )
        ALib::SleepMillis( 1 );

    UT_PRINT( "All threads ended. Shared value=", shared.val );
    UT_EQ( 0, shared.val );
    delete t1;
    delete t2;
    delete t3;
}

//--------------------------------------------------------------------------------------------------
//--- SpeedTest
//--------------------------------------------------------------------------------------------------
UT_METHOD( LockSpeedTest )
{
    UT_INIT();

    ThreadLock aLock;

    int        repeats=    10000;
    int        rrepeats=       3;

    Ticks stopwatch;
    for ( int run= 1; run <= rrepeats; run ++)
    {
        UT_PRINT( "Run {}/{}", run, rrepeats );

        aLock.SetSafeness( Safeness::Safe );
        stopwatch= Ticks::Now();
        for ( int i= 0; i < repeats; i++ )
        {
            aLock.Acquire(ALIB_CALLER_PRUNED);
            aLock.Release();
        }
        auto time= stopwatch.Age().InAbsoluteMicroseconds();
        UT_PRINT( "  Safe mode: {} lock/unlock ops: {}\u00B5s", repeats, time ); // greek 'm' letter;

        aLock.SetSafeness( Safeness::Unsafe );
        stopwatch= Ticks::Now();
        volatile int ii= 0;
        for ( int i= 0; i < repeats; i++ )
        {
            aLock.Acquire(ALIB_CALLER_PRUNED);
            aLock.Release();

            // in java, adding the following two loops, results in similar execution speed
            //for ( int tt= 0; tt < 70; tt++ )        i+= tt;
            //for ( int tt= 0; tt < 70; tt++ )        i-= tt;

            // in c++, adding the following two loops, results in similar execution speed
            for ( int tt= 0; tt < 80; tt++ )    ii+= tt; // in release it is rather 20. Strange enough!
            for ( int tt= 0; tt < 80; tt++ )    ii-= tt; // in release it is rather 20. Strange enough!
        }
        time= stopwatch.Age().InAbsoluteMicroseconds();
        UT_PRINT( "  Unsafe mode: {} lock/unlock ops: {}\u00B5s", repeats, time ); //greek 'm' letter;
        if (ii != 0 )
            UT_PRINT( "Never true! Just to stop compiler from pruning ii" ); //µs


        ThreadLockNR tNR;
        stopwatch= Ticks::Now();
        for ( int i= 0; i < repeats; i++ )
        {
            tNR.Acquire(ALIB_CALLER_PRUNED);
            tNR.Release();
        }
        time= stopwatch.Age().InAbsoluteMicroseconds();
        UT_PRINT( "  std::mutex: {} lock/unlock ops: {}\u00B5s", repeats, time ); //greek 'm' letter;
    }

}

UT_CLASS_END

}; //namespace
