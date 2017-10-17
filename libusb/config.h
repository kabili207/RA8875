
#if defined(_CONFIG_WINDOWS)
#  include "config_windows.h"
#  if defined (_DEBUG)
#    define _ITERATOR_DEBUG_LEVEL 0
#  endif
#elif defined(_CONFIG_LINUX)
#  include "config_linux.h"
#endif
