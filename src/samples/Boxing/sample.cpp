// #################################################################################################
//  ALib - A-Worx Utility Library
//  Boxing Sample
//
//  Copyright 2018 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"
#include "alib/compatibility/std_string.hpp"

#include <iostream>

using namespace std;
using namespace aworx;


// method that accepts any type of object, reference, pointer, fundamental type,....
void AcceptAny( const Box&  box );
void AcceptAny( const Box&  box )
{
    const char* t= "  Type: ";
    const char* v= "  Value: ";

    // fundamental types
         if( box.IsType<bool             >() ) { cout << t << "bool      "; cout << v << (box.Unbox<bool             >() ? "true" : "false" ); }
    else if( box.IsType<aworx::boxed_int >() ) { cout << t << "boxed_int "; cout << v <<  box.Unbox<aworx::boxed_int >(); }
    else if( box.IsType<aworx::boxed_uint>() ) { cout << t << "boxed_uint"; cout << v <<  box.Unbox<aworx::boxed_uint>(); }
    else if( box.IsType<double           >() ) { cout << t << "double    "; cout << v <<  box.Unbox<double           >(); }
    else if( box.IsType<char             >() ) { cout << t << "char      "; cout << v <<  box.Unbox<char             >(); }
    else if( box.IsType<wchar_t          >() ) { cout << t << "wchar_t   "; cout << v; wcout <<  box.Unbox<wchar_t   >(); }


    // character arrays
    else if( box.IsArrayOf<char          >() ) { cout << t << "char[]    ";  cout << v <<  box.Unbox<std::string      >(); }
    else if( box.IsArrayOf<wchar_t       >() ) { cout << t << "wchar_t[] "; wcout << v <<  box.Unbox<std::wstring     >(); }
    else
    {
        // in this sample, no other types are covered
        cout << "  Type not known in this sample. " << std::endl;
        #if ALIB_DEBUG
            cout << "  Dbg info: Type name=" << aworx::lib::debug::TypeDemangler( box.DbgGetReferenceType() ).Get() << endl;
            cout << "  Note:     More convenient debug options are found when module ALib Strings is bundled with ALib Boxing!" << endl;
        #endif
    }

    cout << endl;
}

// method that accepts a list of arbitrary objects, values, pointers...
template <typename... T> void AcceptMany( T&&... args )
{
    // fetch the arguments into an array of boxes
    Boxes boxes( std::forward<T>( args )... );

    // process one by one
    for( const Box& box : boxes )
        AcceptAny( box );
}

// a test type
class MyType {};

// test the methods above
int main()
{
    std::cout << "bool:" << std::endl;
    { bool      val= true;
        AcceptAny(val);
        AcceptAny(&val); }

    std::cout << std::endl << "integer types:" << std::endl;
    {         int8_t   val=  -1;    AcceptAny(val); AcceptAny(&val); }
    {         int16_t  val=  -2;    AcceptAny(val); AcceptAny(&val); }
    {         int32_t  val=  -3;    AcceptAny(val); AcceptAny(&val); }
    {         int64_t  val=  -4;    AcceptAny(val); AcceptAny(&val); }
    {  aworx::intGap_t val=  -5;    AcceptAny(val); AcceptAny(&val); }
    {        uint8_t   val=   1;    AcceptAny(val); AcceptAny(&val); }
    {        uint16_t  val=   2;    AcceptAny(val); AcceptAny(&val); }
    {        uint32_t  val=   3;    AcceptAny(val); AcceptAny(&val); }
    {        uint64_t  val=   4;    AcceptAny(val); AcceptAny(&val); }
    { aworx::uintGap_t val=   5;    AcceptAny(val); AcceptAny(&val); }


    std::cout << std::endl << "float/double:" << std::endl;
    { float     val= 3.14f; AcceptAny(val); AcceptAny(&val); }
    { double    val= 3.14;  AcceptAny(val); AcceptAny(&val); }


    std::cout << std::endl
              << "Character types: Must not be passed as pointers!"
              << std::endl;
    char      c=    'a';            AcceptAny(c);
    wchar_t   wc=  L'\u03B1';       AcceptAny(wc);


    std::cout << std::endl << "...instead character pointer types get boxed to character arrays!"
              << std::endl;

    const  char    *     cString=   "abc";                  AcceptAny(cString);
    const wchar_t  *    wCString=  L"\u03B1\u03B2\u03B3";   AcceptAny(wCString);

    std::cout << "  Note: Wide character output is probably broken. Would be fixed with using module ALib Strings"
              << std::endl;


    std::cout << std::endl << "A type not known to the method:" << std::endl;
    MyType myType;
    AcceptAny(myType);

    std::cout << std::endl << "Finally, pass a list of arbitrary objects:" << std::endl;
    AcceptMany( "Hello", 42, 3.1415 );

    return 0;
}

