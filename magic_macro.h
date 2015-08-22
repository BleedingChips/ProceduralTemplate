#ifndef MAGIC_MACRO_H_INCLUDED
#define MAGIC_MACRO_H_INCLUDED
#ifdef _MSC_VER
    #define MAGIC_TYPENAME
    #define MAGIC_TEMPLATE
#else
    #define MAGIC_TYPENAME typename
    #define MAGIC_TEMPLATE template
#endif // _MSC_VER
#endif // MAGIC_MACRO_H_INCLUDED
