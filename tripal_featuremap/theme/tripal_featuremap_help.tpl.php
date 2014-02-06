<h3>Module Description:</h3>
<p>The Tripal Map module is an interface for the Chado Map module which groups features (sequences) into 
   maps (typically genetic maps).
   This module provides support for visualization of "map" pages, editing and updating.
</p>

<h3>Setup Instructions:</h3>
<ol>
   <li><p><b>Set Permissions</b>: By default only the site administrator account has access to create, edit, delete
   or administer maps. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
   permissions under the 'tripal_featuremap' section as appropriate for your site. For a simple setup, allow anonymous 
   users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>
 
 <li><p><b>Create a Map</b>:  An map (called a feature map in Chado) can be <?php print l('created', 'node/add/chado-featuremap')?> 
 manually in the same way any other data type.  There is no loader for loading large files of map information including
 features that may be associated with these maps (e.g. genetic markers).  The Tripal bulk loader is the only way
 to load this type of data.</p></li>
 
 <li><p><b>Sync any Existing Maps</b>: Before Maps can be viewed on the website, they must first be <?php print l('created manually', 'node/add/chado-featuremap')  ?> 
          or if they already exist in the Chado database they must be synced. Near the top of the <?php print l('Map Configuration page', 'admin/tripal/tripal_featuremap/configuration') ?>
          there is
          a Sync Maps section which provides list of maps currently in chado which can be sync\'d.
          Simply select the maps you would like to create Drupal/Tripal pages for and click Sync Maps. Once synced, 
          pages will appear on the site for maps.</p></li>
 
</ol>


<h3>Features of this Module:</h3>
<ul>
  <li><p><b>Edit or Delete Maps</b>: Maps can be manually edited or deleted by navigating to the map page and clicking the "Edit" button.</p></li>  
    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/featuremaps') ?> is provided for 
    finding maps. This tool relies on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool.  Look for it in the navigation menu under the item 
    "Search Biological Data". </p></li>
</ul>