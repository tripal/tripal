<h3>Module Description:</h3>
<p>The Tripal Analysis module provides a new analysis content type that is intended to be
  used when a more specialized analysis module does not exist.  Because it is generic, it does not
  provide any visualization or data loading tools for analysis data.  Specialized analysis modules,
  such as the Tripal Analysis Blast or Tripal Analysis KEGG modules provide loading and custom
  visualizations for the analysis results.  These modules must be installed separately.
</p>

<h3>Setup Instructions:</h3>
<p>After installation of the analysis module or any specialized analysis module.  The following tasks should be performed</p>
<ol>
  <li><p><b>Set Permissions</b>: By default only the site administrator account has access to create, edit, delete
   or administer analyses. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
   permissions under the 'tripal_analysis' section as appropriate for your site. For a simple setup, allow anonymous
   users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>


  <li><p><b>Create an Analysis</b>:  An analysis should be <?php print l('created', 'node/add/chado-analysis')?> before
  data is imported into chado.  The generic analysis type should only be used when a more specialized analysis module
  (e.g. Tripal Analysis Blast module) does not already exists.  All data imported into Chado should be associated with
  an analysis to help keep track of the source for data.</p></li>

  <li><p><b>Sync Analyses</b>:  If you have analyses already stored in Chado and would like to create pages for them, then
  analyses can be synced using the
   <?php print l('analysis configuration page', 'admin/tripal/tripal_analysis/configuration') ?>. The process of 'syncing'
   automatically creates the pages as if you had created them using the step above.
  </p></li>
</ol>


<h3>Additional Features of this Module:</h3>
  <ul>
    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/analyses') ?> is provided for
    finding analyses. This tool relies on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool.  Look for it in the navigation menu under the item
    "Search Biological Data". </p></li>
</ul>

