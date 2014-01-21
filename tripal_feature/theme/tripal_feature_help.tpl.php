  <h3>Module Description:</h3>
  <p>The Tripal Feature module provides a new feature content type and interface for genomic features. </p>

  <h3>Setup Instructions:</h3>
  <p>After installation of the feature module.  The following tasks should be performed</p>
  <ol>
    <li><p><b>Set Permissions</b>: By default only the site administrator account has access to create, edit, delete
   or administer features. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
   permissions under the 'tripal_feature' section as appropriate for your site. For a simple setup, allow anonymous 
   users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>
   
   <li><p><b>Loading of Ontologies</b>:  
     Before loading genomic features you must also have several vocabularies loaded as well. Using the
     <?php print l('OGO loader','admin/tripal/tripal_cv/obo_loader')?> you should load the following
     ontologies:</p>
     <ul>
        <li>Chado Feature Properties</li>
        <li>Relationship Ontology</li>
        <li>Sequence Ontology</li>
        <li>Gene Ontology (if loading GO terms for features)</li>
     </ul>
   </li>

     <li><p><b>Create Organisms</b>:  Before adding feature data you must already have the
     organisms loaded in the database.  See the
     <?php print l('Tripal Organism Admin Page','admin/tripal/tripal_organism') ?> for
     instructions for adding and Syncing organisms.</p></li>

     <li><p><b>Create an Analysis</b>:  Tripal requires that feature data loaded using the Tripal loaders
     be associated with an analyis.  This provides a grouping for the feature data and can be used
     later to visualize data pipelines.  Before loading feature data through the FASTA or GFF loaders
     you will need to <?php print l('create an analysis','node/add/chado-analysis') ?> for the data.</p></li>

     <li><p><b>Create Database Cross References</b>:  If you would like to associate your feature data with an
     external database, check to ensure that the <?php print l('database already exists','admin/tripal/tripal_db/edit_db') ?>.
     If not you should <?php print l('add a new database record','admin/tripal/tripal_db/add_db') ?> before importing
     feature data. Be sure to set the URL and URL prefix for the database if you would like accessions (e.g. GO terms, NCBI
     accession) to link out to the external database.</p></li>

     <li><p><b>Data Import</b>:  if you do not already have an existing Chado database with preloaded data
     then you will want
     to import data.  You can do so using the Chado perl scripts that come with the normal
     <a href="http://gmod.org/wiki/Chado">distribution of Chado</a> or you can use 
     the <a href="<?php print url('admin/tripal/tripal_feature/fasta_loader') ?>">FASTA loader</a> and
     <a href="<?php print url('admin/tripal/tripal_feature/gff3_load') ?>">GFF loader</a> provided here.  If you
     created the Chado database using Tripal then you'll most likely want to use the Tripal loaders.  If your data
     is not condusive for loading with these loaders and you can get your data into a tab-delimited format you can
     use Tripals' bulk loader.
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

  </ol>


  <h3>Features of this Module:</h3>
  <p>Aside from data loading and feature page setup (as described in the Setup section above),
  The Tripal feature module also provides the following functionality</p>
  <ul>
    <li><p><b>Retrieve Sequences</b>: A tool to <?php print l('retrieve sequences','find/sequences') ?> is provided 
     which allows end-users to download sequences in FASTA format.  The site admin must first load sequence residues
     as well as alignments.  The <?php  print l('organism_feature_count', 'admin/tripal/mviews') ?> and 
     <?php print l('analysis_organism', 'admin/tripal/mviews') ?> materialized
     views must be populated before using this tool.  Those views should be re-populated 
     when new data is added.  If you use the <?php print l('jquery_update module', 'http://drupal.org/project/jquery_update') ?>
     the tool may break.  You will need to update the jquery_update/replace/jquery.form.js file with <?php 
     print l('a more recent version','https://raw.github.com/malsup/form/master/jquery.form.js') ?>. </p></li>
    
     <li><p><b>Generic Feature URL</b>:  As described in the setup instructions above, it is often convenient to have a 
     simple URL for each feature page. For example, http://www.mygenomesite.org/[feature], where [feature] is a 
     unique identifier for a feature page.  The
     <?php print l('Feature Configuration page','admin/tripal/tripal_feature/configuration') ?> allows a 
     site admin to generate unique URLs for all feature.  The unique URL is necessary, however, sometimes
     it is easier to allow for links to the feature name without knowing the unique URL.  This is possible
     using the URL: http://[site url]/feature/[feature name], where [site url] is the URL for the site and 
     [feature name] is the name of the feature.  If the feature name is not unique then a page will be
     presented listing all of the features with the same name and allow the user to choose which one to 
     view.  If the feature name is unique then the user will automatically be redirected to the 
     unique URL for the feature.</p></li>
     
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

    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/features') ?> is provided for 
    finding features. This tool relies on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool.  Look for it in the navigation menu under the item 
    "Search Biological Data". </p></li>

    <li><p><b>Delete Features</b>: You can  <a href="<?php print url('admin/tripal/tripal_feature/delete') ?>">bulk delete features</a>
    by providing a list of feature names, or for a specific organism or for a specific feature type. Be sure you have
    a full backup of your site before performing a bulk delete.</p></li>
  </ul>