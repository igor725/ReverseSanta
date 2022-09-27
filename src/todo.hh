#pragma once

#ifdef DISABLE_TODO
#	define TODO(M)
#	define FIXME
#else
#	define _TOLITERAL(X) #X
#	define TOLITERAL(X) _TOLITERAL(X)
#	define FILE_LINE __FILE__ ":" TOLITERAL(__LINE__) " - "
#	define TODO(M) __pragma(message("** TODO " FILE_LINE M))
#	define FIXME TODO("Fix me!!")
#endif
