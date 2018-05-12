// #################################################################################################
//  ALib - A-Worx Utility Library
//  String Sample
//
//  Copyright 2018 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#include "alib/compatibility/std_string.hpp"
#include "alib/compatibility/std_iostream.hpp"


//  ALib string classes are comparable to similar classes from other C++ libraries. Therefor
//  many features could be demonstrated here.
//
//  There is one feature which makes ALib Strings quite unique: Using template meta programming (TMP),
//  ALib Strings can be constructed from anything that "smells like a string".
//  With only a short piece of code (provided somewhere in a header) any custom string type
//  becomes "compatible".
//
//  Consequently, a method which expects an ALib String as a parameter, accepts just any string type!
//  This is why we call ALib strings to be "non-intrusive".

void AcceptAnyString( const aworx::String& s );
void AcceptAnyString( const aworx::String& s )
{
    std::cout << "  The original string type was: " << s  << std::endl;
}


int main()
{
    // test strings
    std::string      stdString ( "std::string"      );
    std::string      stdStringP( "std::string *"    );

    aworx::AString   aString   ( "aworx::AString"   );
    aworx::AString   aStringP  ( "aworx::AString *" );



    AcceptAnyString(               "char[7]"      );
    AcceptAnyString( static_cast<const char*>("const char*")  );

    AcceptAnyString(   stdString  );
    AcceptAnyString(  &stdStringP );   // even passing pointers: same result

    AcceptAnyString(     aString  );
    AcceptAnyString(    &aStringP );

    // QTString compatibility becomes available in QT projects with
    //   #include "alib/compatibility/qt.hpp"

    //...and you can quickly make any other C++ string type compatible as well.
}
