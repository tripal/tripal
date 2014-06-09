<h3>Tripal Eimage Administrative Tools Quick Links:</h3>
<ul>
 <li><a href="<?php print url("admin/tripal/tripal_eimage/configuration") ?>">Eimage Configuration</a></li>
</ul>
<h3>Module Description:</h3>
<p>The Tripal Eimage module provides support for visualization of "eimage" pages, editing and updating.</p>

<h3>Setup Instructions:</h3>
<ol>
   <li><p><b>Set Permissions</b>: By default only the site administrator account has access to create, edit, delete
   or administer features. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
   permissions under the 'tripal_eimage' section as appropriate for your site. For a simple setup, allow anonymous 
   users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>

<li><p><b>Sync any Existing Eimages</b>: Near the top of the <?php print l('Eimage Configuration page', 'admin/tripal/tripal_eimage/configuration') ?> there is
  a Sync Eimages section which provides list of eimages currently in chado which can be sync\'d.
  Simply select the eimages you would like to create Drupal/Tripal pages for and click Sync Eimages.</p></li>
</ol>


<h3>Features of this Module:</h3>
<ul>
  <li><b>Add/Edit/Delete Eimages</b>: Eimages can be created <?php print l('here', 'node/add/chado-eimage') ?>. 
  After creation, eimages (regardless of the method used to create them) can be
  edited or deleted by clicking the Edit tab at the top of the Eimage Page.</li>
</ul>