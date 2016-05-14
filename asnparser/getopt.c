#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef HAVE_UNISTD_H
#include <string.h>
#include <stdio.h>

char* optarg;
int optind=1;

/// Callers store zero here to inhibit the error message for
/// unrecognized options.
int opterr;
char *nextchar_;

int getopt(int argc_, char** argv_, const char* optstring_) {
	int opt; // Character checked for validity.
	const char *oli; // Option letter index.
	if (argv_ == 0) {
		// It can happen, e.g., on VxWorks.
		optind = 0;
		return -1;
	}


	if (nextchar_ == 0 || *nextchar_ == '\0') {
		// Update scanning pointer.

		if (optind >= argc_
				|| *(nextchar_ = argv_[optind]) != '-') {
			nextchar_ = "";
			return -1;
		}

		if (nextchar_[1] != 0
				&& *++nextchar_ == '-') {
			// Found "--".
			++optind;
			nextchar_ = "";
			return -1;
		}
	}

	// Option letter okay?
	opt = (int) *nextchar_++;

	if (opt == (int) ':'
			|| ((oli = strchr (optstring_, opt)) == 0)) {
		// If the user didn't specify '-' as an option, assume it means
		// -1.
		if (opt == (int) '-')
			return -1;

		if (*nextchar_ == 0)
			++optind;

		if (opterr && *optstring_ != ':')
			fprintf(stderr,"%s: illegal option -- %c\n",argv_[0],opt);
		return '?';
	}

	if (*++oli != ':') {
		// Don't need argument.
		optarg = 0;
		if (!*nextchar_)
			++optind;
	} else {
		// Need an argument.
		if (*nextchar_) // No white space.
			optarg = nextchar_;
		else if (argc_ <= ++optind) {
			// No arg.
			nextchar_ = "";

			if (*optstring_ == ':')
				return ':';
			if (opterr)
				fprintf(stderr, "%s: option requires an argument -- %c\n",argv_[0], opt);
			return '?';
		} else // White space.
			optarg = argv_[optind];

		nextchar_ = "";
		++optind;
	}

	return opt; // Dump back option letter.
}
#endif

