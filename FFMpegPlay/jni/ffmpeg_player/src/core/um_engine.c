#include <stdio.h>
#include "um_engine.h"

const char* um_engine_getConfigure()
{
   
	return  (const char*)avcodec_configuration();
	
}
