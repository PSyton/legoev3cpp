//
//  re_dump.h
//
//  Created by https://github.com/18446744073709551615/reDroid
//

#ifndef __re_dump__
#define __re_dump__

#ifdef __cplusplus
extern "C" {
#endif
	
	void log_dump(const void*addr,int len,int linelen);
	void log_dumpf(const char*fmt,const void*addr,int len,int linelen);
	void log_rdumpf(const char*fmt,const void*addr, int offset, int len, int levels, int linelen);
	
#ifdef __cplusplus
}
#endif

#endif /* defined(__re_dump__) */
