<h3>Tripal Natural Diversity Administrative Tools Quick Links:</h3>
<ul>
 <li><a href="<?php print url("admin/tripal/tripal_natural_diversity/configuration") ?>">Natural Diversity Configuration</a></li>
</ul>
<h3>Module Description:</h3>
<p>The Tripal Natural Diversity module provides support for visualization of "natural_diversity" pages, editing and updating.</p>

<h3>Setup Instructions:</h3>
<ol>
   <li><p><b>Set Permissions</b>: By default only the site administrator account has access to create, edit, delete
   or administer features. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
   permissions under the 'tripal_natural_diversity' section as appropriate for your site. For a simple setup, allow anonymous 
   users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>

<li><p><b>Sync any Existing Natural Diversity Geolocations</b>: Near the top of the <?php print l('Natural Diversity Configuration page', 'admin/tripal/tripal_natural_diversity/configuration') ?> there is
  a Sync Natural Diversity Geolocation section which provides list of nd_geolocation currently in chado which can be sync\'d.
  Simply select the nd_geolocation you would like to create Drupal/Tripal pages for and click Submit Sync Job.</p></li>
</ol>


<h3>Features of this Module:</h3>
<ul>
  <li><b>Add/Edit/Delete Natural Diversity Geolocation</b>: Natural Diversity Geolocation can be created <?php print l('here', 'node/add/chado-nd-geolocation') ?>. 
  After creation, nd_geolocation (regardless of the method used to create them) can be
  edited or deleted by clicking the Edit tab at the top of the Natural Diversity Geolocation Page.</li>
    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/nd_geolocation') ?> is provided for 
    finding nd_geolocation. This tool relies on Drupal Views.  <a href="http://drupal.org/natural_diversity/views">Drupal Views</a>
    which must be installed to see the search tool.  Look for it in the navigation menu under the item 
    "Search Biological Data". </p></li>

</ul>