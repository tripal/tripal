<h3>Module Description:</h3>
<p>The Tripal Contact module is an interface for the Chado Contact module which provides information about
   people or organizations.  This module provides support for visualization of "contact" pages, editing and updating.
</p>

<h3>Setup Instructions:</h3>
<ol>
   <li><p><b>Set Permissions</b>: By default only the site administrator account has access to create, edit, delete
   or administer contacts. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
   permissions under the 'tripal_contact' section as appropriate for your site. For a simple setup, allow anonymous 
   users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>
 
 <li><p><b>Create a Contact</b>:  An contact can be <?php print l('created', 'node/add/chado-contact')?> 
 manually in the same way any other data type.  Contacts may also be created automatically by using the 
 publication module publication importers. Alternatively, the Tripal bulk loader may also be used to load contacts.</p></li>
 
 <li><p><b>Sync any Existing Contacts</b>: Before Contacts can be viewed on the website, they must first be <?php print l('created manually', 'node/add/chado-contact')  ?> 
          or if they already exist in the Chado database they <?php print l('must be synced', 'admin/tripal/tripal_contact/sync') ?> with Drupal.
          Once synced, contact pages will appear on the site.</p></li> 
</ol>


<h3>Features of this Module:</h3>
<ul>
  <li><p><b>Edit or Delete Contacts</b>: Contacts can be manually edited or deleted by navigating to the map page and clicking the "Edit" button.</p></li>  
    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/contacts') ?> is provided for 
    finding contacts. This tool relies on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool.  Look for it in the navigation menu under the item 
    "Search Biological Data". </p></li>
</ul>