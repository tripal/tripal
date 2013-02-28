  

  <h3>Tripal Organism Administrative Tools Quick Links:</h3>
  <ul>
             <li><a href="<?php print url("admin/tripal/tripal_organism/configuration") ?>">Organism Configuration</a></li>
           </ul>

  <h3>Module Description:</h3>
  <p>The Tripal Organism module allows you to add, edit and/or delete chado organisms.
            Furthermore, it also provides listing of organisms and details page for each organism.
            Basically, the chado organism module is designed to hold information about a given species.
            For more information on the chado organism module see the
             <a href="http://gmod.org/wiki/Chado_Organism_Module">GMOD wiki page</a></p>

  <h3>Setup Instructions:</h3>
  <p>After installation of the organism module.  The following tasks should be performed.
            <ol>
              <li><p><b>Set Permissions</b>: The organism module supports the Drupal user permissions interface for
               controlling access to organism content and functions. These permissions include viewing,
               creating, editing or administering of
               organism content. The default is that only the original site administrator has these
               permissions.  You can <a href="<?php print url('admin/user/roles') ?>">add roles</a> for classifying users,
               <a href="<?php print url('admin/user/user') ?>">assign users to roles</a> and
               <a href="<?php print url('admin/user/permissions') ?>">assign permissions</a> for the organism content to
               those roles. For a simple setup, allow anonymous users access to view organism content and
               allow the site administrator all other permissions.</p></li>

              <li><p><b>Create Organisms</b>: Organism pages can be created in two ways:
                 <ol>
                  <li><b>Sync Organisms</b>: If your organism has been pre-loaded into Chado then you need to sync the organism.
                   This process is what creates the pages for viewing online.  Not all organisms need be synced, only those
                   that you want shown on the site.  Use the the
                   <a href="<?php print url('admin/tripal/tripal_organism/configuration') ?>">Organism Configuration page</a>
                   to sync organisms. </li>
                   <li><b>Manually Add An Organism</b>: If your organism is not already present in the Chado database
                   you can create an organism using the <a href="<?php print url('node/add/chado-organism') ?>">Create Organism page</a>.
                   Once saved, the organism will be present in Chado and also "synced".
                 </ol></p></li>

               <li><p><b>Indexing</b>:  Once organism pages are ready for public access,
               you can index the organism pages for searching if you want to ues the Drupal default search mechanism.
               Use the <a href="<?php print url('admin/tripal/tripal_organism/configuration') ?>">Organism Configuration page</a>
               to either Index (for the first time) or "Reindex" (after updating data)
               the organism pages for searching.  Once the site is 100% indexed the pages will be searchable using Drupal\'s
               full text searching.  You can find the percent indexed for the entire site by visiting the
               <a href="<?php print url('admin/settings/search') ?>">Search settings page</a>. Indexing
               can take quite a while if you have a lot of data</p></li>

               <li><p><b>Set Taxonomy</b>:  Drupal provides a mechanism for categorizing content to allow
               for advanced searching.  Drupal calls this "Taxonomy", but is essentially categorizing the pages.
               You can categorize feature pages by the
               organism to which they belong.  This allows for filtering of search results by organism.
               Use the <a href="<?php print url('admin/tripal/tripal_organism/configuration') ?>">Organism Configuration page</a> to
               set the Taxonomy.</p></li>
             </ol>

  <h3>Features of this Module:</h3>
  <p>Aside from organism page setup (as described in the Setup section above),
            The Tripal organism module also provides the following functionality
            <ul>
              <li><p><b>Integration with Drupal Views</b>: <a href="http://drupal.org/project/views">Drupal Views</a> is
              a powerful tool that allows the site administrator to create lists or basic searching forms of Chado content.
              It provides a graphical interface within Drupal to allow the site admin to directly query the Chado database
              and create custom lists without PHP programming or customization of Tripal source code.  Views can also
              be created to filter content that has not yet been synced with Druapl in order to protect access to non
              published data (only works if Chado was installed using Tripal).  You can see a list of available pre-existing
              Views <a href="<?php print url('admin/build/views/') ?>">here</a>, as well as create your own. </p></li>

              <li><p><b>Basic Organism List</b>: This module provides a basic <a href="<?php print url('organisms') ?>">organism list</a>
              for showing the list of organisms in Chado. <a href="http://drupal.org/project/views">Drupal Views</a> must be
              installed. You can use the Views interface to alter the appearance of this list.</p></li>
            </ul>
            </p>

  <h3>Page Customizations</h3>
  <p>There are several ways to customize the look-and-feel for the way Chado data is presented through Tripal.
             Below is a description of several methods.  These methods may be used in conjunction with one another to
             provide fine-grained control.
             <ul>

             <li><p><b>Integration with Drupal Panels</b>:  <a href="http://drupal.org/project/views">Drupal Panels</a>
              allows for customization of a page layout if you don\'t want to do PHP/Javascript/CSS programming.  Tripal comes with pre-set layouts for organism pages.  However,
              Panels become useful if you prefer a layout that is different from the pre-set layouts.  Chado content
              is provided to Panels in the form of Drupal "blocks" which you can then place anywhere on a page using the
              Panel\'s GUI.</p></li>

             <li><p><b>Drupal\'s Content Construction Kit (CCK)</b>: the
             <a href="http://drupal.org/project/cck">Content Construction Kit (CCK) </a> is a powerful way to add non-Chado content
             to any page without need to edit template files or knowing PHP.  You must first download and install CCK.
             With CCK, the site administartor can create a new field to appear on the page.  For example, currently,
             the Chado publication module is not yet supported by Tripal.  Therefore, the site administrator can add a text
             field to the organism pages.  This content is not stored in Chado, but will appear on the organism page.  A field
             added by CCK will also appear in the form when editing a organism to allow users to manually enter the appropriate
             text.  If the default pre-set layout and themeing for Tripal is used, it is better to create the CCK element,
             indicate that it is not to be shown (using the CCK interface), then manually add the new content type
             where desired by editing the templates (as described below).  If using Panels, the CCK field can be added to the
             location desired using the Panels interface.</p></li>

             <li><p><b>Drupal Node Templates</b>:  The Tripal packages comes with a "theme_tripal" directory that contains the
             themeing for Chado content.    The organism module has a template file for organism "nodes" (Tripal organism pages).  This file
             is named "node-chado_organism.tpl.php", and provides javascript, HTML and PHP code for display of the organism
             pages.  You can edit this file to control which types of information (or which organism "blocks") are displayed for organisms. Be sure to
             copy these template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
             future Tripal updates may overwrite your customizations. See the <a href="http://tripal.info/">Tripal website </a>
             for instructions on how to access variables and other Chado content within the template file.</p></li>

             <li><p><b>Organism "Block" Templates</b>:  In the "theme_tripal" directory is a subdirectory named "tripal_organism".
             Inside this directory is a set of templates that control distinct types of information for organisms.  For example,
             there is a "base" template for displaying of data directly from the Chado organism table, and a "references"
             template for showing external site references for a organism (data from the organism_dbxref table).  These templates are used both by Drupal blocks
             for use in Drupal Panels (as described above) or for use in the default pre-set layout that the node template
             provides (also desribed above).  You can customize this template as you desire.  Be sure to copy the
             template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
             future Tripal updates may overwrite your customizations.  See the <a href="http://tripal.info/">Tripal website </a>
             for instructions on how to access variables and other Chado content within the template files.</p></li>
             </li>

             <li><p><b>Adding Links to the "Resources" Sidebar</b>: If you use the pre-set default Tripal layout for theming, you
             will see a "Resources" sidebar on each page.  The links that appear on the sidebar are automatically generated
             using Javascript for all of the organism "Blocks" that appear on the page. If you want to add additional links
             (e.g. a dynamic link to GBrowse for the organism) and you want that link to appear in the
             "Resources" sidebar, simply edit the Drupal Node Template (as described above) and add the link to the
             section at the bottom of the template file where the resources section is found.</p></li>

             </ul>
             </p>
  