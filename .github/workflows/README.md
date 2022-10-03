
GitHub Actions
=================

This directory contains automated workflows executed by GitHub Actions.

Naming Conventions
--------------------

### File Name: [scope]-[purpose]-[extra_info].yml

	- [scope] should be one of the following:
		- MAIN for only against the master branch
		- ALL for against all branches
		- PR if specific to pull requests
	- [purpose] should be a one word description of the purpose (e.g. phpunit, coverage)
		- use snake-case if you must use more then one word (e.g. buildDocker)
	- [extra_info] should be any additional information needed to make the file
		name unique. For multiple words, separate words using an underscore.
