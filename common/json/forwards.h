#ifndef JSON_FORWARm_pDataSaverH_INCLUDED
# define JSON_FORWARm_pDataSaverH_INCLUDED

# include "config.h"

namespace Json {

   class FastWriter;
   class Reader;
   class StyledWriter;

   // features.h
   class Features;

   // value.h
   class StaticString;
   class Path;
   class PathArgument;
   class Value;
   class ValueIteratorBase;
   class ValueIterator;
   class ValueConstIterator;
#ifdef JSON_VALUE_USE_INTERNAL_MAP
   class ValueAllocator;
   class ValueMapAllocator;
   class ValueInternalLink;
   class ValueInternalArray;
   class ValueInternalMap;
#endif // #ifdef JSON_VALUE_USE_INTERNAL_MAP

} // namespace Json


#endif // JSON_FORWARm_pDataSaverH_INCLUDED
