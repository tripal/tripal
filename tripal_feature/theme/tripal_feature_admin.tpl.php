 <h3>Tripal Feature Administrative Tools Quick Links:</h3>
  <ul>
   <li><a href="<?php print url("admin/tripal/tripal_feature/configuration") ?>">Feature Configuration</a></li>
   <li><a href="<?php print url("admin/tripal/tripal_feature/fasta_loader") ?>">Import a multi-FASTA file</a></li>
   <li><a href="<?php print url("admin/tripal/tripal_feature/gff3_load") ?>">Import a GFF3 file</a></li>
   <li><a href="<?php print url("admin/tripal/tripal_feature/sync") ?>">Sync Features</a></li>
   <li><a href="<?php print url("admin/tripal/tripal_feature/delete") ?>">Delete Features</a></li>
 </ul>

  <h3>Module Description:</h3>
  <p>This module provides an interface for the Chado feature module which stores information
    related to genomic features.  This module provides support for bulk loading of data in
    FASTA or GFF format, visualization of "feature" pages, editing and updating.
    </p>

  <h3>Setup Instructions:</h3>
  <p>After installation of the feature module.  The following tasks should be performed</p>
  <ol>
    <li><p><b>Set Permissions</b>: The feature module supports the Drupal user permissions interface for
     controlling access to feature content and functions. These permissions include viewing,
     creating, editing or administering of
     feature content. The default is that only the original site administrator has these
     permissions.  You can <a href="<?php print url('admin/user/roles') ?>">add roles</a> for classifying users,
     <a href="<?php print url('admin/user/user') ?>">assign users to roles</a> and
     <a href="<?php print url('admin/user/permissions') ?>">assign permissions</a> for the feature content to
     those roles.  For a simple setup, allow anonymous users access to view organism content and
     allow the site administrator all other permissions.</p></li>

     <li><p><b>Themeing</b>:  Before content from Chado can be visualized the Tripal base theme must
     be installed.  This should have been done prior to this point.  But is mentioned here in the event you
     follow the instructions below and cannot see content.  In this case, if you do not see content
     check that Tripal theming is properly installed</p></li>

     <li><p><b>Loading of Ontologies</b>:  If you
     used Tripal to create the Chado database, then you must load ontologies before proceeding.  Visit the
     page to <a href="<?php print url('admin/tripal/tripal_cv/obo_loader') ?>">load ontologies</a> and load at
     least the following ontologies:
     <ul>
        <li>Chado Feature Properties</li>
        <li>Relationship Ontology</li>
        <li>Sequence Ontology</li>
        <li>Gene Ontology (if loading GO terms for features)</li>
     </ul></p></li>

     <li><p><b>Create Organisms</b>:  Before adding feature data you must already have the
     organisms loaded in the database.  See the
     <a href="<?php print url('admin/tripal/tripal_organism') ?>">Tripal Organism Admin page</a> for
     instructions for adding and Syncing organisms.</p></li>

     <li><p><b>Create Analysis</b>:  Tripal requires that feature data loaded using the Tripal loaders
     be associated with an analyis.  This provides a grouping for the feature data and can be used
     later to visualize data pipelines.  Before loading feature data through the FASTA or GFF loaders
     you will need to <a href="<?php print url('node/add') ?>">create an analysis</a> for the data.</p></li>

     <li><p><b>Create Referring Database Entries</b>:  If you would like to associate your feature data with an
     external reference database, check to ensure that the <a href="<?php print url('admin/tripal/tripal_db/edit_db') ?>">
     database record already exists</a>.  If not you should <a href="<?php print url('admin/tripal/tripal_db/add_db') ?>">add a new database record</a> before importing
     feature data.</p></li>

     <li><p><b>Data Import</b>:  if you do not already have an existing Chado database with preloaded data
     then you will want
     to import data.  You can do so using the Chado perl scripts that come with the normal
     <a href="http://gmod.org/wiki/Chado">distribution of Chado</a> or you can use the <a href="<?php print url('admin/tripal/tripal_feature/fasta_loader') ?>">FASTA loader</a> and
     <a href="<?php print url('admin/tripal/tripal_feature/gff3_load') ?>">GFF loader</a> provided here.  If you
     created the Chado database using Tripal then you\'ll most likely want to use the Tripal loaders.  If your data
     is not condusive for loading with these loaders you may have to write your own loaders.
     </p></li>

     <li><p><b>Sync Features</b>:  After data is loaded you need to sync features.  This process is what
     creates the pages for viewing online.  Not all features need be synced.  For instance, if you
     have loaded whole genome sequence with fully defined gene models with several features to define
     a gene and its products (e.g. gene, mRNA, CDS, 5\'UTR, 3\'UTR, etc) you probably only want to create
     pages for genes or genes and mRNA.  You probably do not want a page for a 5\'UTR.
     Using the <a href="<?php print url('admin/tripal/tripal_feature/configuration/sync') ?>">Feature Sync page</a>
     you can sync (or create pages) for the desired feature types. </p></li>

     <li><p><b>Set Feature URL</b>:  It is often convenient to have a simple URL for each feature page.
     For example, http://www.mygenomesite.org/[feature], where [feature] is a unique identifier for a feature page.
     With this, people can easily include links to feature pages of interest. Use the
     <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Feature Configuration page</a>
     to specify whether to use the feature name, unique name or internal ID as the [feature] portion of the
     URL.  Select the one that will guarantee a unique identifier for feature pages.</p></li>

     <li><p><b>Indexing</b>:  Once all data has been loaded (including analysis data--e.g. blast, interpro, etc.)
     you can index all feature pages for searching if you want to ues the Drupal default search mechanism.
     Use the <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Feature Configuration page</a>
     to either Index (for the first time) or "Reindex" (after adding new data)
     the feature pages for searching.  Once the site is 100% indexed the pages will be searchable using Drupal\'s
     full text searching.  You can find the percent indexed for the entire site by visiting the
     <a href="<?php print url('admin/settings/search') ?>">Search settings page</a>. Indexing
     can take quite a while if you have a lot of data</p></li>

     <li><p><b>Set Taxonomy</b>:  Drupal provides a mechanism for categorizing content to allow
     for advanced searching.  Drupal calls this "Taxonomy", but is essentially categorizing the pages.
     You can categorize feature pages by their type (e.g. gene, mRNA, contig, EST, etc.) and by the
     organism to which they belong.  This allows for filtering of search results by organism and feature type.
     Use the <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Feature Configuration page</a> to
     set the Taxonomy.</p></li>
  </ol>
  </p>


  <h3>Features of this Module:</h3>
  <p>Aside from data loading and feature page setup (as described in the Setup section above),
  The Tripal feature module also provides the following functionality
  <ul>
    <li><p><b>Feature Browser:</b>  The feature browser is a tabular list of features with links to their
     feature pages which appears on the organism
     page.  It was created to provide a mechanism to allow site visitors to quickly
     accesss feature pages when they do not know what to search for.  For sites with large numbers of features, this
     method for finding a specific pages is inadequate, but may still be included to aid new site
     visitors.    This browser can be toggled on or off using the
     <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Feature Configuration page</a></p></li>

    <li><p><b>Feature Summary Report:</b>  The feature summary report is a pie chart that indicates the types and quantities
    of feature types (Sequence Ontology terms) that are loaded in the database. It appears on the organism
    page.  The summary can be toggled on or off using the
    <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Feature Configuration page</a></p></li>

    <li><p><b>Integration with Drupal Views</b>: <a href="http://drupal.org/project/views">Drupal Views</a> is
    a powerful tool that allows the site administrator to create lists or basic searching forms of Chado content.
    It provides a graphical interface within Drupal to allow the site admin to directly query the Chado database
    and create custom lists without PHP programming or customization of Tripal source code.  Views can also
    be created to filter content that has not yet been synced with Druapl in order to protect access to non
    published data (only works if Chado was installed using Tripal).  You can see a list of available pre-existing
    Views <a href="<?php print url('admin/build/views/') ?>">here</a>, as well as create your own. </p></li>

    <li><p><b>Basic Feature Lookup View</b>: This module provides a basic <a href="<?php print url('features') ?>">feature search
    tool</a> for finding or listing features in Chado. It does not require indexing for Drupal searching but relies
    on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a> must be installed. </p></li>

    <li><p><b>Delete Features</b>: This module provides a <a href="<?php print url('admin/tripal/tripal_feature/delete') ?>">Delete Feature page</a>
    for bulk deltions of features. You may delete features using a list of feature names, or for a specific organism
    or for a specific feature type.</p></li>

  </ul>
            </p>

  <h3>Page Customizations</h3>
  <p>There are several ways to customize the look-and-feel for the way Chado data is presented through Tripal.
 Below is a description of several methods.  These methods may be used in conjunction with one another to
 provide fine-grained control.
 <ul>

 <li><p><b>Integration with Drupal Panels</b>:  <a href="http://drupal.org/project/views">Drupal Panels</a>
  allows for customization of a page layout if you don\'t want to do PHP/Javascript/CSS programming.  Tripal comes with pre-set layouts for feature pages.  However,
  Panels become useful if you prefer a layout that is different from the pre-set layouts.  Chado content
  is provided to Panels in the form of Drupal "blocks" which you can then place anywhere on a page using the
  Panel\'s GUI.</p></li>

 <li><p><b>Drupal\'s Content Construction Kit (CCK)</b>: the
 <a href="http://drupal.org/project/cck">Content Construction Kit (CCK) </a> is a powerful way to add non-Chado content
 to any page without need to edit template files or knowing PHP.  You must first download and install CCK.
 With CCK, the site administartor can create a new field to appear on the page.  For example, currently,
 the Chado publication module is not yet supported by Tripal.  Therefore, the site administrator can add a text
 field to the feature pages.  This content is not stored in Chado, but will appear on the feature page.  A field
 added by CCK will also appear in the form when editing a feature to allow users to manually enter the appropriate
 text.  If the default pre-set layout and themeing for Tripal is used, it is better to create the CCK element,
 indicate that it is not to be shown (using the CCK interface), then manually add the new content type
 where desired by editing the templates (as described below).  If using Panels, the CCK field can be added to the
 location desired using the Panels interface.</p></li>

 <li><p><b>Drupal Node Templates</b>:  The Tripal packages comes with a "theme_tripal" directory that contains the
 themeing for Chado content.    The feature module has a template file for feature "nodes" (Tripal feature pages).  This file
 is named "node-chado_feature.tpl.php", and provides javascript, HTML and PHP code for display of the feature
 pages.  You can edit this file to control which types of information (or which feature "blocks") are displayed for features. Be sure to
 copy these template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
 future Tripal updates may overwrite your customizations. See the <a href="http://tripal.info/">Tripal website </a>
 for instructions on how to access variables and other Chado content within the template file.</p></li>

 <li><p><b>Feature "Block" Templates</b>:  In the "theme_tripal" directory is a subdirectory named "tripal_feature".
 Inside this directory is a set of templates that control distinct types of information for features.  For example,
 there is a "base" template for displaying of data directly from the Chado feature table, and a "references"
 template for showing external site references for a feature (data from the feature_dbxref table).  These templates are used both by Drupal blocks
 for use in Drupal Panels (as described above) or for use in the default pre-set layout that the node template
 provides (also desribed above).  You can customize this template as you desire.  Be sure to copy the
 template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
 future Tripal updates may overwrite your customizations.  See the <a href="http://tripal.info/">Tripal website </a>
 for instructions on how to access variables and other Chado content within the template files.</p></li>
 </li>

 <li><p><b>Adding Links to the "Resources" Sidebar</b>: If you use the pre-set default Tripal layout for theming, you
 will see a "Resources" sidebar on each page.  The links that appear on the sidebar are automatically generated
 using Javascript for all of the feature "Blocks" that appear on the page. If you want to add additional links
 (e.g. a dynamic link to GBrowse for the feature) and you want that link to appear in the
 "Resources" sidebar, simply edit the Drupal Node Template (as described above) and add the link to the
 section at the bottom of the template file where the resources section is found.</p></li>

 </ul>
 </p>