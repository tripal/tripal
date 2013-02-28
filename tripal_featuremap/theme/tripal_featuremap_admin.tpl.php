<h3>Tripal Feature Map Administrative Tools Quick Links:</h3>
<ul>
  <li><a href="<?php print url('admin/tripal/tripal_featuremap/configuration') ?>">Map Configuration</a></li>
</ul>

<h3>Module Description:</h3>
<p>The Tripal Map module is an interface for the Chado Map module which groups features (sequences) into 
   maps (typically genetic maps).
   This module provides support for visualization of "map" pages, editing and updating.
</p>

<h3>Setup Instructions:</h3>
<ol>
  <li><p>
    <b>Set Permissions</b>: The map module supports the Drupal user permissions interface for
    controlling access to map content and functions. These permissions include viewing,
    creating, editing or administering of
    map content. The default is that only the  site administrator has these
    permissions.  You can <a href="<?php print url('admin/user/roles') ?>">add roles</a> for classifying users,
    <a href="<?php print url('admin/user/user') ?>">assign users to roles</a> and
    <a href="<?php print url('admin/user/permissions') ?>">assign permissions</a> for the map content to
    those roles.  For a simple setup, allow anonymous users access to view map content and
    allow the site administrator all other permissions.
  </p></li>  
</ol>


<h3>Features of this Module:</h3>
<ul>
  <li><p><b>Add a map</b>: Maps can be manually created <?php  print l('here', 'node/add/chado-featuremap') ?></p></li>
  <li><p><b>Edit or Delete Maps</b>: Maps can be manually edited or deleted by navigating to the map page and clicking the "Edit" button.</p></li>  
  <li><p><b>Sync any Existing Maps</b>: Before Maps can be viewed on the website, they must first be <?php print l('created manually', 'node/add/chado-featuremap')  ?> 
            or if they already exist in the Chado database they must be synced. Near the top of the <?php print l('Map Configuration page', 'admin/tripal/tripal_featuremap/configuration') ?>
            there is
            a Sync Maps section which provides list of maps currently in chado which can be sync\'d.
            Simply select the maps you would like to create Drupal/Tripal pages for and click Sync Maps. Once synced, 
            pages will appear on the site for maps.</p></li>
  <li><p><b>Integration with Drupal Views</b>: <a href="http://drupal.org/project/views">Drupal Views</a> is
            a powerful tool that allows the site administrator to create lists or basic search forms of Chado content.
            It provides a graphical interface within Drupal to allow the site admin to directly query the Chado database
            and create custom lists without PHP programming or customization of Tripal source code.  Views can also
            be created to filter out records that has not yet been synced with Drupal in order to protect access to non
            published data (only works if Chado was installed using Tripal).  You can see a list of available pre-existing
            Views <a href="<?php print url('admin/build/views/') ?>">here</a>, as well as create your own. </p></li>
</ul>

<h3>Page Customizations</h3>
<p>There are several ways to customize the look-and-feel for the way Chado data is presented through Tripal.
   See the <a href="http://www.gmod.org/wiki/Tripal_Developer's_Handbook">Developers Handbook</a> for further infromation 
   to customize Map pages.
</p>