/**
 * \file enum-generator.h
 *
 * This comes from:
 * https://github.com/gerbaudo/SusyTest0/blob/master/SusyTest0/enumFactory.h,
 * http://stackoverflow.com/questions/147267/easy-way-to-use-variables-of-enum-types-as-string-in-c (http://stackoverflow.com/a/202511/2148414)
 *
 * But slightly modified to allow defining access functions names as methods for a specific class named CLASS
 */

#include <string>
#include <sstream>
#include <stdexcept>
#include <cstring>   // For strcmp()

/*!
 * \brief Expansion macro for enum value definition
 *
 * Allows to define an entry named 'name' with an optional value assignment 'assign' in the enum currently built
 * For example ENUM_VALUE(blabla,=2) will create a new enum member blabla=2
 */
#define ENUM_VALUE(name,assign) name assign,

/*!
 * \brief Expansion macro for enum to string conversion
 *
 * Allows to build one case entry (part of a larger switch block) converting an enum name to a string containing the name
 */
#define ENUM_CASE(name,assign) case name: return #name;

/*!
 * \brief Expansion macro for string to enum conversion
 *
 * Allows to build one if comparison (part of a larger serie of if statements) returning an enum value if the local variable str contains a string matching with name
 */
#define ENUM_STRCMP(name,assign) if (!strcmp(str,#name)) return name;

/*!
 * \brief Declare the access function and define enum values
 *
 * Macro to declare the getString() function and get*Value() functions for the enum, as well as the full content of the enum possible values
 */
#define DECLARE_ENUM(EnumType,ENUM_DEF) \
  enum EnumType { \
    ENUM_DEF(ENUM_VALUE) \
  }; \
  static const char* getString(EnumType value); \
  static EnumType get##EnumType##Value(const char* str); \

/*!
 * \brief Define the access function names
 *
 * Marco to implement (define) the functions for the prototypes previously declared using macro #DECLARE_ENUM(EnumType,ENUM_DEF)
 */
#define DEFINE_ENUM(EnumType,ENUM_DEF,Scope) \
  const char* Scope::getString(EnumType value) \
  { \
    switch(value) \
    { \
      ENUM_DEF(ENUM_CASE) \
      default: std::stringstream ss; \
               ss << "Value out of range " << int(value); \
               throw std::range_error(ss.str());; /* handle input error */ \
    } \
  } \
  Scope::EnumType Scope::get##EnumType##Value(const char* str) \
  { \
    ENUM_DEF(ENUM_STRCMP) \
    throw std::range_error("String " + std::string(str) + " does not match any value"); /* handle input error */ \
  }
  

