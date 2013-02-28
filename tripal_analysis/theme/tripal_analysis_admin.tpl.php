
  <h3>Tripal Analysis Administrative Tools Quick Links:</h3>
  <ul>
             <li><a href="<?php print url("admin/tripal/tripal_analysis/configuration") ?>">Analysis Configuration</a></li>
           </ul>


  <h3>Module Description:</h3>
  <p>The Tripal Analysis module provides a generic analysis content type that is intended to be
            used when a more specialized analysis module does not exist.  Because it is generic, it does not
            provide any visualization or data loading tools for analysis data.  Specialized analysis modules,
            such as the Tripal Analysis Blast or Tripal Analysis KEGG modules provide loading and custom
            visualizations for the analysis results.  These modules must be installed separately.
            </p>

  <h3>Setup Instructions:</h3>
  <p>After installation of the analysis module or any specialized analysis module.  The following tasks should be performed
            <ol>
              <li><p><b>Set Permissions</b>: Each analysis module supports the Drupal user permissions interface for
               controlling access to the content and functions. These permissions include viewing,
               creating, editing or administering of
               analysis content. The default is that only the original site administrator has these
               permissions.  You can <a href="<?php print url('admin/user/roles') ?>">add roles</a> for classifying users,
               <a href="<?php print url('admin/user/user') ?>">assign users to roles</a> and
               <a href="<?php print url('admin/user/permissions') ?>">assign permissions</a> for the analysis content to
               those roles.  For a simple setup, allow anonymous users access to view organism content and
               allow the site administrator all other permissions.</p></li>


               <li><p><b>Create Analysis</b>:  An analysis should be <a href="<?php print url('node/add') ?>">created</a> before data is imported into
               chado.  The generic analysis type should only be used when a more specialized analysis module
               (e.g. Tripal Analysis Blast module) does not already exists.  All data imported into
               Chado should be associated with an analysis.

               <li><p><b>Sync Analyses</b>:  If Chado has preloaded analyses then you can sync those.  This process is what
               creates the pages for viewing an analysis on the site.  Analyses can be synced using the
               <a href="<?php print url('admin/tripal/tripal_analysis/configuration') ?>">Analysis Configuration page</a>.
               However, syncing an analyses will always create a generic analysis content type.  If you would like
               to use a specialized analysis module for visualization of data then do not sync the analysis but recreate it
               using the appropriate specialized analysis content type.</p></li>

            </ol>
            </p>


  <h3>Features of this Module:</h3>
  <p>Aside from providing a generic content type the Tripal Analysis module also provides the following functionality
            <ul>

              <li><p><b>Basic Analysis Lookup View</b>: This module provides a basic <a href="<?php print url('analyses') ?>">analysis search
              tool</a> for finding or listing analyses in Chado. It does not require indexing for Drupal searching but relies
              on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a> must be installed. </p></li>

            </ul>
            </p>

  <h3>Page Customizations</h3>
  <p>There are several ways to customize the look-and-feel for the way Chado data is presented through Tripal.
             Below is a description of several methods.  These methods may be used in conjunction with one another to
             provide fine-grained control.
             <ul>

             <li><p><b>Integration with Drupal Panels</b>:  <a href="http://drupal.org/project/views">Drupal Panels</a>
              allows for customization of a page layout if you don\'t want to do PHP/Javascript/CSS programming.
              Tripal comes with pre-set layouts for analysis pages.  However,
              Panels become useful if you prefer a layout that is different from the pre-set layouts.  Chado content
              is provided to Panels in the form of Drupal "blocks" which you can then place anywhere on a page using the
              Panel\'s GUI.</p></li>

             <li><p><b>Drupal\'s Content Construction Kit (CCK)</b>: the
             <a href="http://drupal.org/project/cck">Content Construction Kit (CCK) </a> is a powerful way to add non-Chado content
             to any page without need to edit template files or knowing PHP.  You must first download and install CCK.
             With CCK, the site administartor can create a new field to appear on the page.  For example, currently,
             the Chado publication module is not yet supported by Tripal.  Therefore, the site administrator can add a text
             field to the analysis pages.  This content is not stored in Chado, but will appear on the analysis page.  A field
             added by CCK will also appear in the form when editing a analysis to allow users to manually enter the appropriate
             text.  If the default pre-set layout and themeing for Tripal is used, it is better to create the CCK element,
             indicate that it is not to be shown (using the CCK interface), then manually add the new content type
             where desired by editing the templates (as described below).  If using Panels, the CCK field can be added to the
             location desired using the Panels interface.</p></li>

             <li><p><b>Drupal Node Templates</b>:  The Tripal packages comes with a "theme_tripal" directory that contains the
             themeing for Chado content.    The analysis module has a template file for analysis "nodes" (Tripal analysis pages).  This file
             is named "node-chado_analysis.tpl.php", and provides javascript, HTML and PHP code for display of the analysis
             pages.  Specialized analysis modules will have their own template files as well, such as "node-chado_analysis-blast.tpl.php" for the
             Tripal Analysis Blast module.  You can edit the template file to control which types of information (or which analysis "blocks") are displayed
             for analysis. Be sure to
             copy these template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
             future Tripal updates may overwrite your customizations. See the <a href="http://tripal.sourceforge.net/">Tripal website </a>
             for instructions on how to access variables and other Chado content within the template file.</p></li>

             <li><p><b>Analysis "Block" Templates</b>:  In the "theme_tripal" directory are subdirectories named after each tripal module (e.g. "tripal_feature", "tripal_library", etc.).
             Inside each directory is a set of templates that control distinct types of information for each content type.  For example,
             there is a "base" template for displaying of data directly from the Chado feature table, and a "references"
             template for showing external site references for a feature (data from the feature_dbxref table).
              These templates are used both by Drupal blocks
             for use in Drupal Panels (as described above) or for use in the default pre-set layout that the node template
             provides (also desribed above).  Analyses block templates can exist in any of these directories.  For example, the Tripal Analysis Unigene
             module uses templates in the tripal_analysis_unigene, tripal_organism, and tripal_feature directories.  Content for a unigene is then
             cusotmizable within each of these contexts.
             You can customize block template as you desire.  Be sure to copy the
             template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
             future Tripal updates may overwrite your customizations.  See the <a href="http://tripal.sourceforge.net/">Tripal website </a>
             for instructions on how to access variables and other Chado content within the template files.</p></li>
             </li>

             <li><p><b>Adding Links to the "Resources" Sidebar</b>: If you use the pre-set default Tripal layout for theming, you
             will see a "Resources" sidebar on each page.  The links that appear on the sidebar are automatically generated
             using Javascript for all of the analysis "Blocks" that appear on the page. If you want to add additional links
             (e.g. a dynamic link to GBrowse for the analysis) and you want that link to appear in the
             "Resources" sidebar, simply edit the Drupal Node Template (as described above) and add the link to the
             section at the bottom of the template file where the resources section is found.</p></li>

             </ul>
             </p>
