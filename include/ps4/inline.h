#ifndef LibPS4InlineH
#define LibPS4InlineH

#ifndef PS4Inline
	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199409L
		#define PS4Inline inline
	#else
		#ifdef __GNUC__
			#define PS4Inline __inline__
		#else
			#define PS4Inline
		#endif
	#endif
#endif

#endif
