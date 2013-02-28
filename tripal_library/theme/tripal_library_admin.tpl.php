<h3>Tripal Library Administrative Tools Quick Links:</h3>
<ul>
 <li><a href="<?php print url("admin/tripal/tripal_library/configuration") ?>">Library Configuration</a></li>
</ul>
<h3>Module Description:</h3>
<p>The Tripal Library module is an interface for the Chado Library module which groups features (sequences) into genetic libraries.
    This module provides support for visualization of "library" pages, editing and updating.</p>

<h3>Setup Instructions:</h3>
<ol>
<li><p><b>Set Permissions</b>: The library module supports the Drupal user permissions interface for
   controlling access to library content and functions. These permissions include viewing,
   creating, editing or administering of
   library content. The default is that only the original site administrator has these
   permissions.  You can <a href="<?php print url('admin/user/roles') ?>">add roles</a> for classifying users,
   <a href="<?php print url('admin/user/user') ?>">assign users to roles</a> and
   <a href="<?php print url('admin/user/permissions') ?>">assign permissions</a> for the library content to
   those roles.  For a simple setup, allow anonymous users access to view organism content and
   allow the site administrator all other permissions.</p></li>
<li><p><b>Sync any Existing Libraries</b>: Near the top of the ' . l('Library Configuration page', 'admin/tripal/tripal_library/configuration') ?> there is
  a Sync Libraries section which provides list of libraries currently in chado which can be sync\'d.
  Simply select the libraries you would like to create Drupal/Tripal pages for and click Sync Libraries.</p></li>
</ol>


<h3>Features of this Module:</h3>
<ul>
  <li><b>Add/Edit/Delete Libraries</b>: Libraries with no associated features can be created ' . l('here', 'node/add/chado-library') ?> but it is
  recommended to create the library using the feature loader. For example, when you load FASTA files using the Tripal loader you are
  given the option of specifying a library for all created features. Existing Libraries (regardless of the method used to create them) can be
  edited or deleted by clicking the Edit tab at the top of the Library Page.</li>
  <li><p><b>Integration with Drupal Views</b>: <a href="http://drupal.org/project/views">Drupal Views</a> is
  a powerful tool that allows the site administrator to create lists or basic searching forms of Chado content.
  It provides a graphical interface within Drupal to allow the site admin to directly query the Chado database
  and create custom lists without PHP programming or customization of Tripal source code.  Views can also
  be created to filter content that has not yet been synced with Druapl in order to protect access to non
  published data (only works if Chado was installed using Tripal).  You can see a list of available pre-existing
  Views <a href="<?php print url('admin/build/views/') ?>">here</a>, as well as create your own. </p></li>
  <li><b>Basic Listing</b>: This module provides a basic <a href="<?php print url('libraries') ?>">library display
  tool</a> for finding or listing libraries in Chado. It does not require indexing for Drupal searching but relies
  on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a> must be installed.</li>
</ul>

<h3>Page Customizations</h3>
<p>There are several ways to customize the look-and-feel for the way Chado data is presented through Tripal.
Below is a description of several methods.  These methods may be used in conjunction with one another to
provide fine-grained control.
<ul>

<li><p><b>Integration with Drupal Panels</b>:  <a href="http://drupal.org/project/views">Drupal Panels</a>
allows for customization of a page layout if you don\'t want to do PHP/Javascript/CSS programming.  Tripal comes with pre-set layouts for library pages.  However,
Panels become useful if you prefer a layout that is different from the pre-set layouts.  Chado content
is provided to Panels in the form of Drupal "blocks" which you can then place anywhere on a page using the
Panel\'s GUI.</p></li>

<li><p><b>Drupal\'s Content Construction Kit (CCK)</b>: the
<a href="http://drupal.org/project/cck">Content Construction Kit (CCK) </a> is a powerful way to add non-Chado content
to any page without need to edit template files or knowing PHP.  You must first download and install CCK.
With CCK, the site administartor can create a new field to appear on the page.  For example, currently,
the Chado publication module is not yet supported by Tripal.  Therefore, the site administrator can add a text
field to the library pages.  This content is not stored in Chado, but will appear on the library page.  A field
added by CCK will also appear in the form when editing a library to allow users to manually enter the appropriate
text.  If the default pre-set layout and themeing for Tripal is used, it is better to create the CCK element,
indicate that it is not to be shown (using the CCK interface), then manually add the new content type
where desired by editing the templates (as described below).  If using Panels, the CCK field can be added to the
location desired using the Panels interface.</p></li>

<li><p><b>Drupal Node Templates</b>:  The Tripal packages comes with a "theme_tripal" directory that contains the
themeing for Chado content.    The library module has a template file for library "nodes" (Tripal library pages).  This file
is named "node-chado_library.tpl.php", and provides javascript, HTML and PHP code for display of the library
pages.  You can edit this file to control which types of information (or which library "blocks") are displayed for libraries. Be sure to
copy these template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
future Tripal updates may overwrite your customizations. See the <a href="http://tripal.info">Tripal website </a>
for instructions on how to access variables and other Chado content within the template file.</p></li>

<li><p><b>Library "Block" Templates</b>:  In the "theme_tripal" directory is a subdirectory named "tripal_library".
Inside this directory is a set of templates that control distinct types of information for libraries.  For example,
there is a "base" template for displaying of data directly from the Chado library table.  These templates are used both by Drupal blocks
for use in Drupal Panels (as described above) or for use in the default pre-set layout that the node template
provides (also desribed above).  You can customize this template as you desire.  Be sure to copy the
template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
future Tripal updates may overwrite your customizations.  See the <a href="http://tripal.info">Tripal website </a>
for instructions on how to access variables and other Chado content within the template files.</p></li>
</li>

<li><p><b>Adding Links to the "Resources" Sidebar</b>: If you use the pre-set default Tripal layout for theming, you
will see a "Resources" sidebar on each page.  The links that appear on the sidebar are automatically generated
using Javascript for all of the library "Blocks" that appear on the page. If you want to add additional links
(e.g. a link to a views table showing all features of the current library) and you want that link to appear in the
"Resources" sidebar, simply edit the Drupal Node Template (as described above) and add the link to the
section at the bottom of the template file where the resources section is found.</p></li>

</ul>
</p>