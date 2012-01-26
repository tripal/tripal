This README lists all patches that were used in developement of tripal_views_integration.

------------------------------------------------------
These are the patches needed to ahah_helper module:
Submit issue reported and solved here:
http://drupal.org/node/1231140
The original patch location:
http://drupal.org/files/issues/1231140.ahah_helper.form-submit-handler.patch
The patch file alexg used:
patches/1231140.ahah_helper.form-submit-handler.patch
Usage:
1. Find the global or local path to this module and substitue in the command below for*path_to_tripal_views_integration*
2. Navigate to the folder of contrib module ahah_helper and run:
ahah_helper$ patch -p1 < *path_to_tripal_views_integration*/patches/1231140.ahah_helper.form-submit-handler.patch

(alternatively you can just copy the patch file to the ahah_helper folder and run the command without *path_to_tripal_views_integration*)
NOTE: we have to use patch -p1 command, since this is a git patch, and the ahah_helper folder most likely will not be
	git inintialized. -p0 will not work. If folder is not git initialized then 'git apply -v *patchname*' will do nothing.
-----------------------------------------------------

