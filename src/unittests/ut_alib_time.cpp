// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"

#include "alib/compatibility/std_string.hpp"

#if !defined (HPP_ALIB_TIME_CALENDAR)
    #include "alib/time/calendar.hpp"
#endif
#if !defined (HPP_ALIB_TIME_STOPWATCH)
    #include "alib/time/stopwatch.hpp"
#endif

#define TESTCLASSNAME       CPP_ALib_Time
#include "aworx_unittests.hpp"

#include <numeric>

using namespace std;
using namespace aworx;

namespace ut_aworx {

namespace  {
template<typename TClock>
void print_clock_info(AWorxUnitTesting& ut, const NString& name)
{
	typename TClock::duration unit(1);
	using TPeriod= typename TClock::period ;

	// general info
	UT_PRINT( "Clock info for:        {}\\n"
		      "period:                {} ns\\n"
		      "unit:                  {} ns\\n"
		      "Steady:                {}\\n",
              name,
              TPeriod::num*1000000000ull / TPeriod::den,
              chrono::duration_cast<chrono::nanoseconds>(unit).count(),
              (TClock::is_steady?"true":"false") );

    // take n measurements
	const long long qtyIterations = 3;
	vector<typename TClock::time_point> timePoints(qtyIterations);
	Ticks timer;
	    for(size_t i=0; i<qtyIterations; ++i)
		    timePoints[i] = TClock::now();
	auto duration= timer.Age();

	UT_PRINT( "Time per measure:      {} ns", duration.InNanoseconds() / qtyIterations );

	auto minDuration= timePoints[1] - timePoints[0];
	for( size_t i= 2 ; i < qtyIterations ; ++i )
    {
    	auto actDuration= timePoints[i] - timePoints[i-1];
    	if( actDuration < minDuration )
    	    actDuration=  minDuration;
    }
	UT_PRINT( "Min measurement delta: {} ns", std::chrono::duration_cast<std::chrono::nanoseconds>( minDuration ).count() );
}
}
UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- Basics
//--------------------------------------------------------------------------------------------------
UT_METHOD(Basics)
{
    UT_INIT();

    UT_PRINT("") UT_PRINT( "### TicksBasics ###" );

    // durations
    {
        Ticks::Duration t;
        int64_t  i;
        t= Ticks::Duration::FromSeconds( 42 );
            i= t.InNanoseconds();                     UT_NEAR ( 42000000000L , i     , 500L ) ;
            i= t.InAbsoluteMicroseconds ();           UT_EQ   ( 42000000LL   , i            ) ;
            i= t.InAbsoluteMilliseconds ();           UT_EQ   ( 42000LL      , i            ) ;
            i= t.InAbsoluteSeconds();                 UT_EQ   ( 42LL         , i            ) ;
        t= Ticks::Duration::FromMilliseconds( 42 );
            i= t.InNanoseconds  ();                   UT_NEAR ( 42000000L    , i     , 500L ) ;
            i= t.InAbsoluteMicroseconds ();           UT_EQ   ( 42000LL      , i            ) ;
            i= t.InAbsoluteMilliseconds ();           UT_EQ   ( 42LL         , i            ) ;
            i= t.InAbsoluteSeconds();                 UT_EQ   ( 0LL          , i            ) ;
        t= Ticks::Duration::FromMicroseconds( 42 );
            i= t.InNanoseconds  ();                   UT_NEAR ( 42000L       , i     , 500L ) ;
            i= t.InAbsoluteMicroseconds ();           UT_EQ   ( 42LL         , i            ) ;
            i= t.InAbsoluteMilliseconds ();           UT_EQ   ( 0LL          , i            ) ;
            i= t.InAbsoluteSeconds();                 UT_EQ   ( 0LL          , i            ) ;
        t= Ticks::Duration::FromNanoseconds( 42 );
            i= t.InNanoseconds  ();                   UT_NEAR ( 42L          , i     , 500L ) ;
            i= t.InAbsoluteMicroseconds ();           UT_EQ   ( 0LL          , i            ) ;
            i= t.InAbsoluteMilliseconds ();           UT_EQ   ( 0LL          , i            ) ;
            i= t.InAbsoluteSeconds();                 UT_EQ   ( 0LL          , i            ) ;
        t= Ticks::Duration::FromNanoseconds( 123456789 );
            i= t.InNanoseconds  ();                   UT_NEAR ( 123456789L   , i     , 500L ) ;
            i= t.InAbsoluteMicroseconds ();           UT_EQ   ( 123456LL     , i            ) ;
            i= t.InAbsoluteMilliseconds ();           UT_EQ   ( 123LL        , i            ) ;
            i= t.InAbsoluteSeconds();                UT_EQ    ( 0LL          , i            ) ;


        t=  Ticks::Duration::FromMilliseconds( 100 ) + Ticks::Duration::FromSeconds( 42 );
            i= t.InNanoseconds();              UT_NEAR ( 42100000000L                  , i         , 500L ) ;
            i= t.InAbsoluteMicroseconds ();    UT_EQ   ( 42100000LL                    , i                ) ;
            i= t.InAbsoluteMilliseconds ();    UT_EQ   ( 42100LL                       , i                ) ;
            i= t.InAbsoluteSeconds();          UT_EQ   ( 42LL                          , i                ) ;
        t-= Ticks::Duration::FromMilliseconds( 100 );
            i= t.InNanoseconds();              UT_NEAR ( 42000000000L                  , i         , 500L ) ;
            i= t.InAbsoluteMicroseconds ();    UT_EQ   ( 42000000LL                    , i                ) ;
            i= t.InAbsoluteMilliseconds ();    UT_EQ   ( 42000LL                       , i                ) ;
            i= t.InAbsoluteSeconds();          UT_EQ   ( 42LL                          , i                ) ;

        t= Ticks::Duration::FromMilliseconds( 100 );  UT_NEAR ( t.InHertz(    ) , 10.0      , 0.0001 ) ;
        t= Ticks::Duration::FromMilliseconds( 300 );  UT_EQ   ( t.InHertz( 0  ) , 3.0                ) ;
                                               UT_EQ   ( t.InHertz( 1  ) , 3.3                ) ;
                                               UT_EQ   ( t.InHertz( 2  ) , 3.33               ) ;
                                               UT_EQ   ( t.InHertz( 5  ) , 3.33333            ) ;
    }

    // check time library creation time
    {
        auto creationTimeDiff= lib::TIME.CreationTime().Age();
        UT_PRINT( "Time library creation was: {} ns ago"        , creationTimeDiff.InNanoseconds()  );
        UT_PRINT( "Time library creation was: {} \u00B5s ago"   , creationTimeDiff.InAbsoluteMicroseconds() );
        UT_PRINT( "Time library creation was: {} ms ago"        , creationTimeDiff.InAbsoluteMilliseconds() );
        UT_PRINT( "Time library creation was: {} s  ago"        , creationTimeDiff.InAbsoluteSeconds());
        UT_TRUE ( creationTimeDiff.InNanoseconds() > 100  ); // It should really take 100 nanoseconds to get here!
        UT_TRUE ( creationTimeDiff.InAbsoluteSeconds() < 3600 ); // these tests will probably not last an hour
    }

    // check if we could sleep for 100ms
    {
        Ticks start;
            ALib::SleepMillis( 30 );
        auto sleepTime= start.Age();
        UT_PRINT( "Time diff after 30ms sleep: {}\u00B5s ago", sleepTime.InAbsoluteMicroseconds() );
        UT_TRUE ( sleepTime.InAbsoluteMilliseconds() > 25 );
        UT_TRUE ( sleepTime.InAbsoluteMilliseconds() < 150 ); // should work even on heavily loaded machines
    }

    // check initialization
    {
        Ticks     ticksI;                               UT_EQ( true , ticksI   .IsSet() );
        Ticks     ticksU(Initialization::Suppress);     UT_EQ( false, ticksU   .IsSet() );
        DateTime dateTimeI;                             UT_EQ( true , dateTimeI.IsSet() );
        DateTime dateTimeU(Initialization::Suppress);   UT_EQ( false, dateTimeU.IsSet() );
    }

    // check boxing
    {
        Ticks ticks;
        Box b= ticks;
        Ticks ticksBack= b.Unbox<Ticks>();
        UT_TRUE( ticks == ticksBack )
        UT_EQ( ticks.Raw(), ticksBack.Raw() );

        DateTime dateTime;
        b= dateTime;
        DateTime dateTimeBack= b.Unbox<DateTime>();
        UT_TRUE( dateTime == dateTimeBack )
        UT_EQ( dateTime.Raw(), dateTimeBack.Raw() );
    }
}


//--------------------------------------------------------------------------------------------------
//--- SpeedTest
//--------------------------------------------------------------------------------------------------
UT_METHOD(SpeedTest)
{
    UT_INIT();

    UT_PRINT(""); UT_PRINT( "### TicksSpeedTest ###" );

    UT_PRINT( "# Clock information #" );

    print_clock_info<DateTime::TTimePoint::clock>(ut, "DateTime::TClock" );
    print_clock_info<Ticks   ::TTimePoint::clock>(ut, "   Ticks::TClock" );
//    print_clock_info<std::chrono::steady_clock>(ut, "std" );
//    print_clock_info<std::chrono::system_clock>(ut, "sys" );
//    print_clock_info<std::chrono::high_resolution_clock>(ut, "hpc" );


    for (int runs= 0; runs < 5; runs ++ )
    {
        int aLotOf= 100;

        Ticks tsMeasure;
        Ticks setTest;
        for (int i= 0; i < aLotOf; i++ )
            setTest= Ticks::Now();

        auto nanos= tsMeasure.Age().InNanoseconds();
        auto averageNanos= nanos / aLotOf ;
        UT_PRINT(  "{} x    Ticks::Now() took {} ns. This is an average of {} nanoseconds per call", aLotOf, nanos, averageNanos );
        UT_TRUE ( averageNanos < 10000 );
    }

    for (int runs= 0; runs < 5; runs ++ )
    {
        int aLotOf= 100;

        DateTime tsMeasure;
        DateTime setTest;
        for (int i= 0; i < aLotOf; i++ )
            setTest= DateTime::Now();

        auto nanos= tsMeasure.Age().InNanoseconds();
        auto averageNanos= nanos / aLotOf ;
        UT_PRINT(  "{} x DateTime::Now() took {} ns. This is an average of {} nanoseconds per call", aLotOf, nanos, averageNanos );
        #if !ALIB_AVOID_ANALYZER_WARNINGS
        UT_TRUE ( averageNanos < 10000 );
        #endif
    }
}

//--------------------------------------------------------------------------------------------------
//--- DateTimeConversion
//--------------------------------------------------------------------------------------------------
UT_METHOD(DateTimeConversion)
{
    UT_INIT();
    UT_PRINT("")  UT_PRINT( "### TickSpeedTest ###" );

    #if defined (__GLIBCXX__) || defined(__APPLE__)

        // check epoc seconds
        {
            DateTime ticksNowOrig;

            // first we get time_t from system and from ticks and compare
            time_t timetNowFromTicks=      ticksNowOrig.InEpochSeconds();
            time_t timetNowFromSystem;     std::time(&timetNowFromSystem);
            UT_PRINT( "time_t from DateTime:  {} (seconds after 1/1/1970 GMT)", timetNowFromTicks );
            UT_PRINT( "time_t from system:     {} -> Diff: {}", timetNowFromSystem, (timetNowFromTicks - timetNowFromSystem) );
            UT_PRINT( "Today from DateTime:   {}", ctime( &timetNowFromTicks )  );
            UT_PRINT( "Today from System:      {}", ctime( &timetNowFromSystem )   );
            UT_TRUE ( abs ( timetNowFromTicks - timetNowFromSystem ) <= 1 );

            // now we convert time_t back to ticks
            {
                DateTime ticksNowRoundtrip= DateTime::FromEpochSeconds( timetNowFromTicks );

                int64_t ns1= std::chrono::duration_cast<std::chrono::nanoseconds>( ticksNowOrig     .NativeValue().time_since_epoch() ).count();
                int64_t ns2= std::chrono::duration_cast<std::chrono::nanoseconds>( ticksNowRoundtrip.NativeValue().time_since_epoch() ).count();
                UT_PRINT( "DateTime in nanos since epoch, original:  ", ns1 );
                UT_PRINT( "DateTime in nanos since epoch, roundtrip: ", ns2 );
                UT_NEAR( (ticksNowOrig - ticksNowRoundtrip).InAbsoluteSeconds(), 1, 1 );
            }

            // now we add a day
            {
                time_t           timetTomorrowTime= timetNowFromTicks + 3600*24 + 2*3600 + 3*60 + 4;
                DateTime        ticksTomorrow( ticksNowOrig );
                CalendarDuration   converter;
                converter.Days=       1;
                converter.Hours=      2;
                converter.Minutes=    3;
                converter.Seconds=    4;
                ticksTomorrow+= converter.ToDateTimeDuration();

                time_t  timetTomorrowTicks= ticksTomorrow.InEpochSeconds();
                UT_PRINT( "Tomorrow time_t from DateTime: {}", timetTomorrowTicks           );
                UT_PRINT( "Tomorrow time_t from system:    {}  Diff: {}", timetTomorrowTime, (timetTomorrowTicks - timetTomorrowTime) );
                UT_PRINT( "Tomorrow from DateTime:        {}", ctime( &timetTomorrowTicks ) );
                UT_PRINT( "Tomorrow from System:           {}", ctime( &timetTomorrowTime )  );

                UT_EQ( timetTomorrowTicks ,  timetTomorrowTime );
            }
        }

    #elif defined(_WIN32)
        // check epoch seconds
        {
            // get ticks now and system now
            DateTime       ticksNow;
            FILETIME        timetNowFromSystem;      GetSystemTimeAsFileTime( &timetNowFromSystem );

            ULARGE_INTEGER  timetNowFromTicks= ticksNow.ToFileTimeLI();

            // first we check that they are not too far apart
            ULARGE_INTEGER timetNowFromSystemUL;
            timetNowFromSystemUL.HighPart= timetNowFromSystem.dwHighDateTime;
            timetNowFromSystemUL.LowPart=  timetNowFromSystem.dwLowDateTime;
            int64_t diff= timetNowFromSystemUL.QuadPart >= timetNowFromTicks.QuadPart ? (int64_t) timetNowFromSystemUL.QuadPart - timetNowFromTicks.QuadPart
                                                                                      : (int64_t) timetNowFromTicks   .QuadPart - timetNowFromSystemUL.QuadPart;


            UT_PRINT( "FILETIME from DateTime: {}", timetNowFromTicks.QuadPart   );
            UT_PRINT( "FILETIME from system:    {} -> Diff: {}", timetNowFromSystemUL.QuadPart, diff );
            UT_TRUE ( diff <= 100000L );


            // now, we convert back to ticks
            {
                DateTime nowBack;
                nowBack= DateTime::FromFileTime( timetNowFromTicks );
                auto a= ticksNow.Age().InMilliseconds();
                auto b=  nowBack.Age().InMilliseconds();
                UT_PRINT("Back to DateTime. Diff: ", a-b )
                UT_TRUE( abs( a - b ) < 10 );
            }


            // now we convert to system time
            {
                SYSTEMTIME st=  ticksNow.ToSystemTime();

                UT_PRINT( "Year/M/D  HH:MM:SS:  {}/{}/{}  {}:{}:{}",
                          st.wYear,st.wMonth,st.wDay, st.wHour,st.wMinute,st.wSecond );
            }

            // Test DateTime::Duration
            {
                CalendarDuration  span;
                span.Days=       1;
                span.Hours=      2;
                span.Minutes=    3;
                span.Seconds=    4;
                DateTime ticksTomorrow= ticksNow + span.ToDateTimeDuration();
                ULARGE_INTEGER timeTomorrowFromTicks= ticksTomorrow.ToFileTimeLI();

                timetNowFromSystemUL.QuadPart+=  static_cast<int64_t>( (24 + 2) * 3600   +   3 * 60   + 4) * 10000000L;

                auto x= timeTomorrowFromTicks.QuadPart ;
                auto y= timetNowFromSystemUL.QuadPart  ;
                UT_NEAR( (int64_t) x, (int64_t) y, (int64_t) 100000 );
            }
        }

    #else
        #pragma message ( "Warning: Warning: Unknown Platform" )
    #endif


    // CalendarDateTime
    {
        DateTime   tNow;

        // cut fractional part (below seconds). Otherwise a rounding error might occur
        #if defined (__GLIBCXX__) || defined(__APPLE__)
            tNow= DateTime::FromEpochSeconds( tNow.InEpochSeconds() );
        #elif defined( _WIN32 )
            SYSTEMTIME st= tNow.ToSystemTime( Timezone::Local );
            tNow= DateTime::FromSystemTime( st, Timezone::Local );
        #endif
        CalendarDateTime   cNow( tNow );

        UT_PRINT( "Today local is: {}/{}/{} {}:{:02}:{:02}", cNow.Year, cNow.Month, cNow.Day,
                                                       cNow.Hour, cNow.Minute, cNow.Second     );

        DateTime   tNowBack( cNow.Get() );

        auto diff= (tNow-tNowBack).InAbsoluteSeconds();
        #if defined(_WIN32) // currently, this does not do better than this!
            UT_NEAR( diff, 0, 1 );
        #else
            UT_EQ( 0, diff );
        #endif
    }

    // CalendarDateTime UTC
    {
        DateTime   tNow;
        // cut fractional part (below seconds). Otherwise a rounding error might occur
        #if defined (__GLIBCXX__) || defined(__APPLE__)
            tNow= DateTime::FromEpochSeconds( tNow.InEpochSeconds() );
        #elif defined( _WIN32 )
            SYSTEMTIME st= tNow.ToSystemTime( Timezone::Local );
            tNow= DateTime::FromSystemTime(  st, Timezone::Local );
        #endif
        CalendarDateTime   cNow( tNow, Timezone::UTC );

        UT_PRINT( "Today UTC is:  {}/{}/{} {}:{:02}:{:02}", cNow.Year, cNow.Month, cNow.Day,
                                                      cNow.Hour, cNow.Minute, cNow.Second     );

        DateTime   tNowBack( cNow.Get( Timezone::UTC ) );

        auto diff= (tNow-tNowBack).InAbsoluteSeconds();
        #if defined(_WIN32) // currently, this does not do better than this!
            UT_NEAR( diff, 0, 1 );
        #else
            UT_EQ( 0, diff );
        #endif
    }

    // Ticks converter
    {
        TicksConverter converter;
        DateTime    dateTimeNow;
        Ticks       ticksNow;

        DateTime    ticksConverted= converter.ToDateTime( ticksNow );
        UT_PRINT("Diff: ", (dateTimeNow.Raw()  - ticksConverted.Raw()) )
        UT_TRUE( abs( (dateTimeNow - ticksConverted).InNanoseconds()) < 10000 || ALIB_AVOID_ANALYZER_WARNINGS )

        Ticks       ticksBack=      converter.ToTicks( ticksConverted );
        UT_TRUE( abs( (ticksBack - ticksNow).InNanoseconds()) < 10000         || ALIB_AVOID_ANALYZER_WARNINGS )
    }

}

//--------------------------------------------------------------------------------------------------
//--- Ages
//--------------------------------------------------------------------------------------------------
UT_METHOD(Ages)
{
    UT_INIT();
    //UT_PRINT( "Nanos per 2013 years:    "                << (nanosPerYear*2013) );

    StopWatch tt;

    // minimum time measuring
    {
        auto minimum= Ticks::Duration::FromAbsoluteHours(1);
        for( int i= 0; i< 100; ++i)
        {
            tt.Start();
            Ticks::Duration actual= tt.Sample();
            if( minimum > actual )
                minimum=  actual;
        }
        UT_PRINT( "Ticks::Duration minimum measurement: {} ns", minimum.InNanoseconds()            );
        UT_PRINT( "Ticks::Duration average measurement: {} ns", tt.GetAverage().InNanoseconds()    );

        UT_TRUE ( tt.GetAverage().InAbsoluteMilliseconds()  < 10 || ALIB_AVOID_ANALYZER_WARNINGS   );
    }
    // minimum sleep time measuring
    {
        tt.Reset();
        for (int i= 0 ; i< 100 ; i++)
        {
            ALib::SleepNanos( 1 );
            tt.Sample();
        }

        UT_PRINT( "{} probes of 1 ns of sleep leads to average sleep time of {} ns",
                  tt.GetSampleCnt(),
                  tt.GetAverage().InNanoseconds() );

        tt.Reset();
        for (int i= 0 ; i< 100 ; i++)
        {
            ALib::SleepMicros( 1 );
            tt.Sample();
        }
        UT_PRINT( "{} probes of 1 microsecond of sleep leads to average sleep time of {} ns",
                  tt.GetSampleCnt(),
                  tt.GetAverage().InNanoseconds() );

        tt.Reset();
        for (int i= 0 ; i< 100 ; i++)
        {
            ALib::SleepMicros( 20 );
            tt.Sample();
        }
        UT_PRINT( "{} probes of 20 microseconds of sleep leads to average sleep time of {} microseconds",
                  tt.GetSampleCnt(),
                  tt.GetAverage().InAbsoluteMicroseconds() );

        tt.Reset();
        for (int i= 0 ; i< 10 ; i++)
        {
            ALib::SleepMillis( 1 );
            tt.Sample();
        }
        UT_PRINT( "{} probes of 1 ms of sleep leads to average sleep time of {} microseconds",
                  tt.GetSampleCnt(),
                  tt.GetAverage().InAbsoluteMicroseconds() );
    }

    // sleep two times 20 ms and probe it to an average
    {
        tt.Reset();  ALib::SleepMillis( 20 );   tt.Sample();
        tt.Start();  ALib::SleepMillis( 20 );   tt.Sample();
        tt.Start();  ALib::SleepMillis( 20 );   tt.Sample();
        tt.Start();  ALib::SleepMillis( 20 );   tt.Sample();
        tt.Start();  ALib::SleepMillis( 20 );   tt.Sample();

        auto cum=    tt.GetCumulated().InAbsoluteMilliseconds();
        auto cnt=    tt.GetSampleCnt();
        auto avg=    tt.GetAverage().InAbsoluteMilliseconds();
        auto hertz=  tt.GetAverage().InHertz(1);
        UT_PRINT(  "{} probes of 20 ms sleep leads to sleep time of {} ms", cnt, cum );
        UT_PRINT(  "  average is: {} ms", avg  );
        UT_PRINT(  "  in Hertz  : {}", hertz );
        UT_TRUE ( hertz < 55 );
        UT_TRUE ( hertz > 45 );
        UT_TRUE ( avg   > 15 );
        UT_TRUE ( avg   < 25 );
    }

    // simple calculation
    {
        Ticks ts1;
        Ticks ts2(ts1);
        ts1-= Ticks::Duration::FromSeconds( 1001 );
        ts2-= Ticks::Duration::FromSeconds( 1000 );

        UT_TRUE ( (ts2-ts1).InAbsoluteMilliseconds() == 1000L );
        UT_TRUE ( (ts2-ts1).InAbsoluteMicroseconds() == 1000L * 1000L );
        UT_TRUE ( (ts2-ts1).InNanoseconds ()         == 1000L * 1000L * 1000L);
    }
}


//--------------------------------------------------------------------------------------------------
//--- SpeedTestIndexOf
//--------------------------------------------------------------------------------------------------
UT_METHOD(SpeedTestIndexOf)
{
    UT_INIT();

    // Note: Compares std::string against AString. In debug, std is faster, in release AString
    //       is faster.

    std::string testString("-------------------------------------------------------------------------------------------*#");
    AString     testAString( testString );

    StopWatch   ttString;
    StopWatch   ttAString;
    AString        output;
    for (int run= 4; run < 18; run++ )
    {
        int qtyLoops= 1 << run;
        integer nonOptimizableUsedResultValue= 0;

        // use String.IndexOf()
        ttString.Reset();
            for (int i= 0; i < qtyLoops; i++ )
                nonOptimizableUsedResultValue+= testString.find( (i & 1) ? '*' : '#' );
        Ticks::Duration stringSample= ttString.Sample();


        // use AString.IndexOf()
        ttAString.Reset();
            for (int i= 0; i < qtyLoops; i++ )
                nonOptimizableUsedResultValue+= testAString.IndexOf( (i & 1) ? '*' : '#' );
        Ticks::Duration aStringSample= ttAString.Sample();


        // this is always true, just for the sake that the compiler does not optimize the whole code!
        if ( nonOptimizableUsedResultValue > -1 )
        {
            output.Clear();
            output  ._( "Search loops " )            ._( Format::Field( String32( qtyLoops                        ), 6 ) )
                    ._( ":  time needed: " )         ._( Format::Field( String32( stringSample.InNanoseconds()    ), 8 ) )
                    ._( " / "  )                     ._( Format::Field( String32( aStringSample.InNanoseconds()   ), 8 ) )
                    ._( "   Ratio String/AString: " )._( static_cast<double>(stringSample.InNanoseconds()) / static_cast<double>(aStringSample.InNanoseconds()) );

            UT_PRINT( output );
        }
    }
}

void dateFormatCheck( AWorxUnitTesting& ut, CalendarDateTime& ct,  const character * fmt, const character * expected )
{
    String128 res;
    ct.Format( fmt, res );
    UT_PRINT( (String128("CalendarDateTime.Format: ") << fmt << " ->")._(Format::Tab(20)) << res );
    UT_EQ( expected, String(res) );
}


UT_METHOD(DateFormat)
{
    UT_INIT();

    CalendarDateTime ct;
    ct.Year     = 2015;
    ct.Day      =    3;
    ct.Month    =    4;
    ct.Hour     =    5;
    ct.Minute   =    6;
    ct.Second   =    7;
    ct.DayOfWeek=    2;

    dateFormatCheck( ut, ct,     ASTR("y")        ,        ASTR("2015") );
    dateFormatCheck( ut, ct,    ASTR("yy")        ,          ASTR("15") );
    dateFormatCheck( ut, ct,   ASTR("yyy")        ,        ASTR("2015") );
    dateFormatCheck( ut, ct,  ASTR("yyyy")        ,        ASTR("2015") );
    dateFormatCheck( ut, ct, ASTR("yyyyy")        ,       ASTR("02015") );

    dateFormatCheck( ut, ct,    ASTR("M")         ,           ASTR("4") );
    dateFormatCheck( ut, ct,   ASTR("MM")         ,          ASTR("04") );
    dateFormatCheck( ut, ct,  ASTR("MMM")         ,         ASTR("Apr") );
    dateFormatCheck( ut, ct, ASTR("MMMM")         ,        ASTR("April") );

    dateFormatCheck( ut, ct, ASTR("d")            ,           ASTR("3") );
    dateFormatCheck( ut, ct, ASTR("dd")           ,          ASTR("03") );
    dateFormatCheck( ut, ct, ASTR("ddd")          ,         ASTR("Tue") );
    dateFormatCheck( ut, ct, ASTR("dddd")         ,     ASTR("Tuesday") );

    dateFormatCheck( ut, ct, ASTR("H")            ,           ASTR("5") );
    dateFormatCheck( ut, ct, ASTR("HH")           ,          ASTR("05") );
    dateFormatCheck( ut, ct, ASTR("HHH")          ,         ASTR("005") );
    dateFormatCheck( ut, ct, ASTR("HHHH")         ,        ASTR("0005") );

    dateFormatCheck( ut, ct, ASTR("m")            ,           ASTR("6") );
    dateFormatCheck( ut, ct, ASTR("mm")           ,          ASTR("06") );
    dateFormatCheck( ut, ct, ASTR("mmm")          ,         ASTR("006") );
    dateFormatCheck( ut, ct, ASTR("mmmm")         ,        ASTR("0006") );

    dateFormatCheck( ut, ct, ASTR("s")            ,           ASTR("7") );
    dateFormatCheck( ut, ct, ASTR("ss")           ,          ASTR("07") );
    dateFormatCheck( ut, ct, ASTR("sss")          ,         ASTR("007") );
    dateFormatCheck( ut, ct, ASTR("ssss")         ,        ASTR("0007") );

    //error: dateFormatCheck( ut, ct, ASTR("'")            ,           ASTR("") );
    //error: dateFormatCheck( ut, ct, ASTR("'msH")         ,        ASTR("msH") );
    dateFormatCheck( ut, ct, ASTR("''")           ,          ASTR("'") );
    dateFormatCheck( ut, ct, ASTR("''''")         ,         ASTR("''") );
    dateFormatCheck( ut, ct, ASTR("''m''")        ,        ASTR("'6'") );
    dateFormatCheck( ut, ct, ASTR("'''m'''")        ,      ASTR("'m'") );
    dateFormatCheck( ut, ct, ASTR("s'msH's")      ,     ASTR("7msH7") );
    dateFormatCheck( ut, ct, ASTR("'someone''''s quote'"),  ASTR("someone's quote") );

    dateFormatCheck( ut, ct, ASTR("yyyy-MM-dd HH:mm:ss"),  ASTR("2015-04-03 05:06:07") );
}

UT_METHOD(DurationConversion)
{
    UT_INIT();

    double  d= 3.14;
    integer i= 3;
    Ticks::Duration ts;
    ts= Ticks::Duration::FromDays        ( d );   UT_NEAR( d, ts.InDays        () ,0.0001);   UT_EQ( i, ts.InAbsoluteDays        () );
    ts= Ticks::Duration::FromHours       ( d );   UT_NEAR( d, ts.InHours       () ,0.0001);   UT_EQ( i, ts.InAbsoluteHours       () );
    ts= Ticks::Duration::FromMinutes     ( d );   UT_NEAR( d, ts.InMinutes     () ,0.0001);   UT_EQ( i, ts.InAbsoluteMinutes     () );
    ts= Ticks::Duration::FromSeconds     ( d );   UT_NEAR( d, ts.InSeconds     () ,0.0001);   UT_EQ( i, ts.InAbsoluteSeconds     () );
    ts= Ticks::Duration::FromMilliseconds( d );   UT_NEAR( d, ts.InMilliseconds() ,0.0001);   UT_EQ( i, ts.InAbsoluteMilliseconds() );
    ts= Ticks::Duration::FromMicroseconds( d );   UT_NEAR( d, ts.InMicroseconds() ,0.1   );   UT_EQ( i, ts.InAbsoluteMicroseconds() );
    #if !defined( _WIN32 )
    ts= Ticks::Duration::FromNanoseconds ( i );   UT_EQ( i, ts.InNanoseconds () );
    #endif
}

void durationToStringCheck( AWorxUnitTesting& ut, Ticks::Duration& ts, const String& expected )
{
    String128 res;
    res << ts;
    UT_EQ( expected, String(res) );
}

UT_METHOD(DurationApply)
{
    UT_INIT();
    NumberFormat::Global.SetComputational();

    Ticks::Duration ts;                             durationToStringCheck( ut, ts, ASTR("zero time")           );
    ts=  Ticks::Duration::FromDays        (-15 );   durationToStringCheck( ut, ts, ASTR("- 15.00 days")        );
    ts=  Ticks::Duration::FromDays        ( 15 );   durationToStringCheck( ut, ts, ASTR("15.00 days")          );
    ts+= Ticks::Duration::FromHours       ( 12 );   durationToStringCheck( ut, ts, ASTR("15.50 days")          );

    ts=  Ticks::Duration::FromDays        (  5 );   durationToStringCheck( ut, ts, ASTR("5 days 0.00 hours")   );
    ts+= Ticks::Duration::FromHours       ( 12 );   durationToStringCheck( ut, ts, ASTR("5 days 12.00 hours")  );


    ts=  Ticks::Duration::FromHours       (  5 );   durationToStringCheck( ut, ts, ASTR("5 hours 0 minutes")   );
    ts+= Ticks::Duration::FromMinutes     ( 12 );   durationToStringCheck( ut, ts, ASTR("5 hours 12 minutes")  );
    ts-= Ticks::Duration::FromMinutes     ( 11 );   durationToStringCheck( ut, ts, ASTR("5 hours 1 minute")    );


    ts=  Ticks::Duration::FromMinutes     (  5 );   durationToStringCheck( ut, ts, ASTR("5 minutes 0 seconds") );
    ts+= Ticks::Duration::FromSeconds     ( 12 );   durationToStringCheck( ut, ts, ASTR("5 minutes 12 seconds"));
    ts-= Ticks::Duration::FromSeconds     ( 11 );   durationToStringCheck( ut, ts, ASTR("5 minutes 1 second")  );

    ts=  Ticks::Duration::FromSeconds     (   5 );  durationToStringCheck( ut, ts, ASTR("5.00 seconds")        );
    ts+= Ticks::Duration::FromMilliseconds( 500 );  durationToStringCheck( ut, ts, ASTR("5.50 seconds")        );
    ts-= Ticks::Duration::FromMilliseconds( 250 );  durationToStringCheck( ut, ts, ASTR("5.25 seconds")        );

    ts=  Ticks::Duration::FromMilliseconds(   5 );  durationToStringCheck( ut, ts, ASTR("005 ms")              );
    ts=  Ticks::Duration::FromMicroseconds( 500 );  durationToStringCheck( ut, ts, ASTR("500 \u00B5s")         );
    ts=  Ticks::Duration::FromNanoseconds ( 250 );  durationToStringCheck( ut, ts, ASTR("250 ns")              );
}



UT_CLASS_END

}; //namespace
