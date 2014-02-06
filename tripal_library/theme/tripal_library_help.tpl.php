<h3>Module Description:</h3>
<p>The Tripal Library module is an interface for the Chado Library module which groups features (sequences) into genetic libraries.
    This module provides support for visualization of "library" pages, editing and updating.</p>

<h3>Setup Instructions:</h3>
<ol>
  <li><p><b>Set Permissions</b>: By default only the site administrator account has access to create, edit, delete
   or administer features. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
   permissions under the 'tripal_library' section as appropriate for your site. For a simple setup, allow anonymous 
   users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>

<li><p><b>Sync any Existing Libraries</b>: Near the top of the <?php  l('Library Configuration page', 'admin/tripal/tripal_library/configuration') ?> there is
  a Sync Libraries section which provides list of libraries currently in chado which can be synced.
  Simply select the libraries you would like to create Drupal/Tripal pages for and click Sync Libraries.</p></li>
</ol>


<h3>Features of this Module:</h3>
<ul>
  <li><b>Add/Edit/Delete Libraries</b>: Libraries with no associated features can be created <?php print l('here', 'node/add/chado-library') ?>.  
  Existing Libraries (regardless of the method used to create them) can be
  edited or deleted by clicking the Edit tab at the top of the Library Page.</li>
  
    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/libraries') ?> is provided for 
    finding libraries. This tool relies on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool.  Look for it in the navigation menu under the item 
    "Search Biological Data". </p></li>
</ul>
