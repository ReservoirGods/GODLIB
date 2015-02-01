###############################################################################
#	CWMON DEPENDENCY GENERATOR
#	(c) 2011 Leon O'Reilly/CwmOcean
###############################################################################
#
#	generates dependency files (.d) for pc visual studio builds
#
#	usage: gawk -f dep_gen.awk -v file_src=SRCFILENAME -v file_dst=DSTFILENAME inputfile.pipe
#
###############################################################################


###############################################################################
#	START OF PROCESSING
#		make target/dependency line for make
###############################################################################

BEGIN {
	gLineLast = file_dst ": " file_src
}


###############################################################################
#	PROCESS EVERY LINE
#		input is pipe of vs build tty (including /showIncludes)
#		we find the lines of file from /showIncludes (start with Note: ... )
#		trim leading/trailing whitespace
#		escape spaces (go to slash space format for make)
#		absolute drive letters get changed to cygdrives
###############################################################################
{
	if( length(gLineLast) )
	{
		print gLineLast " \\";
	}
	gLineLast = ""
	lString = $0;
	if( sub( "Note: including file:", "", lString) )
	{
		gsub( /\\/, "/", lString );
		gsub(/^[ \t]+/, "", lString)
		gsub(/[ \t]+$/, "", lString)
		gsub(/[ \t]+/, "\\ ", lString)
		lSplitCount = split( lString, lArray, ":" );
		if( lSplitCount > 1 )
		{
			gLineLast = "/cygdrive/" lArray[1] lArray [2]
		}
		else
		{
			gLineLast =	lString;
		}
	}
}

###############################################################################
#	END OF PROCESSING
#		output final prerequisite without trailing slash
###############################################################################

END {
	print	gLineLast;
}
