This module is provided as a template for creating a custom Tripal Extension 
Module for Drupal 7 and Tripal 2.x.

Drupal does provide quite a bit of documentation on its website at 
http://www.drupal.org but the easiest way to learn to program a Drupal module
is to purchase the following book:

  Tomlinson, VanDyk.  Pro Drupal Development. 2010. ISBN-13: 978-1430228387

But this quick link can help get you started:

  https://drupal.org/developing/modules/7

Briefly, to create a Drupal module you must
 
1) Create a directory to house all of your module's files
2) Create a .info inside of the directory which provides information about your
   module to Drupal
3) Create a .module file which contains the functions used by your module
4) Create a .install file which contains the functions used for installation,
   enabling, disabling and uninstallation of your module.

Examine the example functions and documentation in each of the files in this
example module to learn how Tripal uses the Drupal API.

-------------------------
DIRECTORY AND FILE NAMING
-------------------------
When creating your Tripal Extension module, the following directory structure
and file naming is suggested:

For the required files:
[module dir]/[module name].info
[module dir]/[module name].module
[module dir]/[module name].install

If you want to include Drush commands for your module
[module dir]/[module name].drush.inc

If you want to integrate with Drupal Views 3.x:
[module dir]/[module name].views.inc
[module dir]/[module name].views_default.inc


Include Files
-------------
To limit the size of files, some functionality can be placed inside of 
"include" files. Include files are placed inside of an 'includes' directory.
[module dir]/includes

If your module creates a node type that uses data housed in Chado, you should 
place all of the Drupal hooks for nodes inside of an include named:
[module dir]/includes/[module name].chado_node.inc

If your module has an administrative interface, all of the functions related to
administration should go in an include file named:
[module dir]/includes/[module name].admin.inc

All other include files should be named in the following way:
[module dir]/includes/[module name].[function].inc

where [function] is a brief description of the functionality provided by the
include file.  Please only use underscores inside of the [function] (no dashes
or periods).


Theme Files
-------------
Tripal primarily uses template files for displaying content from Chado. This 
allows anyone to easily change the way data is displayed without needing to
delve into the module's source code.  A template typically provides data for
a single data type (e.g. feature) or association (e.g. properties associated to
features, or publications associated with featurmaps, etc.).  These template
files and any JavaScript, CSS or images needed to suppport them are all
housed inside of a 'theme' directory with the following structure:
[module dir]/theme
[module dir]/theme/css       (for CSS files)
[module dir]/theme/js        (for JS files)
[module dir]/theme/images    (for images)
[module dir]/theme/templates (for all Drupal template files)

All Drupal hooks and functions related to theming of content should go in the
file named:
[module dir]/theme/[module name].theme.inc

The functions in that file will typically be functions which directly
generate content for a page or "preprocess" hooks that prepare variables that
are passed to templates.

Template files are named in the following way
[module dir]/theme/templates/[module name]_[function].tpl.php.

Notice that templates have an underscore separating the [module name] from the
[function].  Typically a period is used (as with include files) but for
backwards compatibility the undescores are kept.  

API Files
---------
If your module will provide a set of functions that can be used as an 
Application Programming Intervace (API), then those functions should be placed 
in files housed in the 'api' directory:
[module dir]/api

When creating API functions try to organize them into groups of related function
and separate them into files by function with the following nameing:
[module dir]/api/[module name].[function].api.inc




