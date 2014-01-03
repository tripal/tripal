  <h3>Module Description:</h3>
  <p>The Tripal Organism module allows you to add, edit and/or delete chado organisms.
    Furthermore, it also provides listing of organisms and details page for each organism.
    Basically, the chado organism module is designed to hold information about a given species.
    For more information on the chado organism module see the
     <a href="http://gmod.org/wiki/Chado_Organism_Module">GMOD wiki page</a></p>

  <h3>Setup Instructions:</h3>
  <p>After installation of the organism module.  The following tasks should be performed.</p>
  <ul>
      <li><p><b>Set Permissions</b>: By default only the site administrator account has access to create, edit, delete
       or administer features. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
       permissions under the 'tripal_organism' section as appropriate for your site. For a simple setup, allow anonymous
       users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>

      <li><p><b>Create an Organism</b>: Organism pages can be created in two ways:</p>
         <ol>
          <li><p><b>Sync Organisms</b>: If your organism has been pre-loaded into Chado then you need to sync the organism.
           This process is what creates the pages for viewing online.  Not all organisms need be synced, only those
           that you want shown on the site.  Use the the
           <a href="<?php print url('admin/tripal/tripal_organism/configuration') ?>">Organism Configuration page</a>
           to sync organisms. </p></li>
           <li><p><b>Manually Add An Organism</b>: If your organism is not already present in the Chado database
           you can create an organism using the <a href="<?php print url('node/add/chado-organism') ?>">Create Organism page</a>.
           Once saved, the organism will be present in Chado and also "synced".</p>
         </ol>
       </li>
   </ul>

  <h3>Features of this Module:</h3>
  <p>Aside from organism page setup (as described in the Setup section above),
            The Tripal organism module also provides the following functionality</p>
            <ul>

    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/organisms') ?> is provided for
    finding organisms. This tool relies on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool.  Look for it in the navigation menu under the item
    "Search Biological Data". </p></li>
            </ul>