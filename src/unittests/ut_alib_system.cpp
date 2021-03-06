// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"


#if !defined (HPP_ALIB_SYSTEM_ENVIRONMENT)
    #include "alib/system/environment.hpp"
#endif

#if !defined (HPP_ALIB_SYSTEM_PROCESSINFO)
    #include "alib/system/process.hpp"
#endif

#if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
    #include "alib/system/directory.hpp"
#endif


#define TESTCLASSNAME       CPP_ALib_System
#include "aworx_unittests.hpp"

using namespace std;
using namespace aworx;

namespace ut_aworx {


UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- CurrentDir
//--------------------------------------------------------------------------------------------------
UT_METHOD(DirectorySpecial)
{
    UT_INIT();

    UT_PRINT(""); UT_PRINT( "### Directory::SpecialFolders ###" );

    {
        String512 cwd;
        Directory::CurrentDirectory( cwd );
        UT_PRINT( String512() << "The current directory is:     "  << cwd );
        UT_TRUE( cwd.IsNotEmpty() );   UT_TRUE( Directory::Exists( cwd ) );
    }

    {
        Directory dir( Directory::SpecialFolder::Current );
        UT_PRINT( String512() << "The current directory is:     " << dir.Path );
        UT_TRUE( dir.Path.IsNotEmpty() );    UT_TRUE( Directory::Exists( dir.Path ) );
    }

    {
        Directory dir( Directory::SpecialFolder::Home );
        UT_PRINT( String512() << "The home directory is:        " << dir.Path );
        UT_TRUE( dir.Path.IsNotEmpty() );    UT_TRUE( Directory::Exists( dir.Path ) );
    }

    {
        Directory dir( Directory::SpecialFolder::HomeConfig );
        UT_PRINT( String512() << "The HomeConfig directory is:  " << dir.Path );
        UT_TRUE( dir.Path.IsNotEmpty() );    UT_TRUE( Directory::Exists( dir.Path ) );
    }

    {
        Directory dir( Directory::SpecialFolder::Module );
        UT_PRINT( String512() << "The Module directory is:      " << dir.Path );
        UT_TRUE( dir.Path.IsNotEmpty() );    UT_TRUE( Directory::Exists( dir.Path ) );
    }

    {
        Directory dir( Directory::SpecialFolder::Root );
        UT_PRINT( String512() << "The Root directory is:        " << dir.Path );
        UT_TRUE( dir.Path.IsNotEmpty() );    UT_TRUE( Directory::Exists( dir.Path ) );
    }

    {
        Directory dir( Directory::SpecialFolder::Temp );
        UT_PRINT( String512() << "The Temp directory is:        " << dir.Path );
        UT_TRUE( dir.Path.IsNotEmpty() );    UT_TRUE( Directory::Exists( dir.Path ) );
    }

    {
        Directory dir( Directory::SpecialFolder::VarTemp );
        UT_PRINT( String512() << "The VarTemp directory is:     " << dir.Path );
        UT_TRUE( dir.Path.IsNotEmpty() );    UT_TRUE( Directory::Exists( dir.Path ) );
    }
}



//--------------------------------------------------------------------------------------------------
//--- GetVariable
//--------------------------------------------------------------------------------------------------
UT_METHOD(GetVariable)
{
    UT_INIT();

    UT_PRINT(""); UT_PRINT( "### Environment::GetVariable###" );
    aworx::AString aString;
    bool result;
    #if defined(_WIN32)
        result=  lib::system::GetEnvironmentVariable( ASTR("HOMEDRIVE"), aString );
        result|= lib::system::GetEnvironmentVariable( ASTR("HOMEPATH") , aString, CurrentData::Keep );
    #else
        result=  lib::system::GetEnvironmentVariable( ASTR("HOME")    , aString );
    #endif

    UT_PRINT("The aString directory is:" );
    UT_PRINT(aString);
    UT_TRUE( Directory::Exists( aString ) );
    UT_TRUE( result );

    result=  lib::system::GetEnvironmentVariable( ASTR("Nonexistingenvvar")  , aString );
    UT_FALSE( result );
    UT_TRUE( aString.IsEmpty() );
}

//--------------------------------------------------------------------------------------------------
//--- Processes
//--------------------------------------------------------------------------------------------------
UT_METHOD(Processes)
{
    UT_INIT();

    UT_PRINT(""); UT_PRINT( "### Environment::GetProcessInfo###" );


    String2K output;
    const ProcessInfo& currentProcess= ProcessInfo::Current();
    UT_TRUE( currentProcess.PID != 0 );

    #if defined (__GLIBC__) || defined(__APPLE__)
        // print process tree of us
        int indent= 0;
        uinteger nextPID= currentProcess.PPID;
        while ( nextPID != 0 )
        {
            ProcessInfo pi( nextPID );
            output.Clear().InsertChars(' ', 2* indent); output  << "PID:          " << pi.PID;            UT_PRINT( output )
            output.Clear().InsertChars(' ', 2* indent); output  << "PPID:         " << pi.PPID;           UT_PRINT( output )
            output.Clear().InsertChars(' ', 2* indent); output  << "Name:         " << pi.Name;           UT_PRINT( output )
            output.Clear().InsertChars(' ', 2* indent); output  << "ExecFileName: " << pi.ExecFileName;   UT_PRINT( output )
            output.Clear().InsertChars(' ', 2* indent); output  << "ExecFilePath: " << pi.ExecFilePath;   UT_PRINT( output )
            output.Clear().InsertChars(' ', 2* indent); output  << "CmdLine:      " << pi.CmdLine;        UT_PRINT( output )
            #if !defined(__APPLE__)
            output.Clear().InsertChars(' ', 2* indent); output  << "StatState:    " << pi.StatState;      UT_PRINT( output )
            output.Clear().InsertChars(' ', 2* indent); output  << "StatPGRP:     " << pi.StatPGRP;       UT_PRINT( output )
            #endif
            //output.Clear()._(' ', 2* indent); output  << "Stat:      " << pi.Stat;      UT_PRINT( output )

            indent++;
            nextPID= pi.PPID;
        }


    #elif defined(_WIN32)


        output.Clear(); output  << "PID:               " << currentProcess.PID;                      UT_PRINT( output )
        output.Clear(); output  << "CmdLine:           " << currentProcess.CmdLine;                  UT_PRINT( output )
        output.Clear(); output  << "ConsoleTitle:      " << currentProcess.ConsoleTitle;             UT_PRINT( output )

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}


UT_CLASS_END

}; //namespace



