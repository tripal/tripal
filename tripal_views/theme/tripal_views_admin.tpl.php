<h3>Views Integration Description:</h3>
<p>Tripal Views provides an interface for integrating <a href="http://drupal.org/project/views">Drupal Views</a>
   with Chado tables, custom tables and materialized views.  This allows site administrators to create custom pages
   and forms for tables in the Chado schema (if Chado was installed by Tripal).  All tables in Chado are integrated
   automatically with Drupal Views but custom tables and materialized views are not.  After creating a new materialized
   view or custom table you can follow the links above to integrate the table with Drupal Views.  The interface allows
   you to specify which fields the table can be joined with other tables and also specify field, sort and filter
   handlers for views.  Different handlers provide different functionality.
</p>

<br>
<h3>Setup Instructions:</h3>
<p>After installation of the Tripal core module.  The following tasks should be performed</p>
<ol>
  <li><b>Set Permissions</b>: To allow access to site administrators for this module, simply
    <?php print l('assign permissions', 'admin/user/permissions') ?> to the appropriate user roles for the
     permission type "manage tripal_views_integration". </li>
</ol>

<br>
<h3>Usage Instructions:</h3>
<p>To use Tripal Views integration follow these steps:</p>
<ol>
   <li><b>Identify or create a materialized view or custom table:</b> Using the <?php print l('Tripal materialized View interface', "admin/tripal/mviews") ?>
     identify the view you would like to integrate or create a new one.  Or, using the <?php print l('Tripal custom table interface', 'admin/tripal/custom_tables')?></li>
   <li><b>Integration a new table</b>: Navigate to the <?php print l('new integration page', "admin/tripal/views/integration/new") ?>
     to integrate the new table.  Provide a user friendly name
     and description to help you remember the purpose for integrating the view.  Next, select the table  you want to integrate
     from the provided select box.  If your table has fields that can join with other Chado tables, you may
     provide those relationships in the provided form.  Finally, if your fields require a special handlers, you
     may select them from the drop downs provided</li>
   <li><b>Alter an existing integration</b>:  If a table is already integrated you can alter its integration configuration by navigating
     to the <?php print l('list of integrated tables', 'admin/tripal/views/integration/list')?>, select the table from the list and alter it accordingly.
     You can create new integration configurations for tables that are already integrated by lowering the priority setting.  The configuration setting
     with the lowest priority will be selected.</li>
   <li><b>Create custom pages/block/search form</b>:  After saving conifguration settings from either step above, you can navigate to the
     Drupal Views interface where you can create a custom page, block or search form.</li>
   <li><b>Review your integrated views</b>:  A page providing a
     <?php print l('list of all integrated views', "admin/tripal/views/integration/list") ?> is provided. You may
     view this page to see all integrated views, but also to remove any unwanted integrations.</li>
</ol>