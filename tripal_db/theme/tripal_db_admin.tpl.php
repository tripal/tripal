<h3>Tripal External Database Administrative Tools Quick Links</h3>
<ul>
<li><?php print l('Add an external database for cross-refernces.', 'admin/tripal/tripal_db/add_db') ?></li>
<li><?php print l('Update or delete an external database.', 'admin/tripal/tripal_db/edit_db') ?></li>
</ul><br>

<h3>Module Description:</h3>
<p>The Tripal DB Module provides the ability to add database cross reference to the 
  data in your Tripal Website.  Typically an external database (such as NCBI Genbank, Gene Ontology (GO),
  stocks database) contains a collection of objects (genomic sequences, vocabulary terms, stocks) that are 
  uniquely identified using an accession number (or identifier).  Data loaded into Tripal can be a
  associated with these objects in remote databases, and links can appear on pages allowing site visitors
  to view the associated objects on the remote database\'s website </p>

<h3>Setup Instructions:</h3>
<ol>
<li><b>Set Permissions</b>: This module supports the Drupal user permissions interface for
             controlling administrative access for creating, editing and deleting database cross-reference resources. 
             The default is that only the site administrator has these
             permissions.  Best practice is to create <a href="<?php print url('admin/user/roles') ?>">a new role</a> 
             for administrative tasks, (such as a webmaster role),
             and then <a href="<?php print url('admin/user/user') ?>">assign users to the role</a>. Finally,
             <a href="<?php print url('admin/user/permissions') ?>">assign the permission</a> titled "administer db cross-reference".
             to the new role.</li>
<li><b>Adding or Editing an External Databases</b>. Many resources such as NCBI nr or ExPASy SwissProt (to name a few) 
          come pre-loaded with Chado.  However, you can add new entries or edit existing entries. Also, when loading 
          ontologies (controlled vocabularies) using the Tripal CV module new databases are added automaticaly for 
          each ontology.  To enable linking of accession on a page to the page for that accession on the external
          database, simply add the URL and the URL prefix when adding or editing a database.</li>

<li><b>Associate Data with Accessions</b>.  The Tripal loaders (e.g. GFF, OBO) can associate accessions from
          remote data to genomic features and controlled vocabularies automatically.  Use the loaders to load genomic
          features and controlled vocabularies respectively.  Additionally, the bulk loader can be used to create
          loading templates for associating external database accessions.  
          </li>
</ol>


<br><h3>Features of this Module:</h3>
<ul>
<li><b>Add an External Databases</b>:
By entering the name and any additional details into the <a href="tripal_db/add_db">add database form</a> you register an external database with your website. This allows you to specify that a sequence feature or other data is also stored in an external database. This is escpecially useful if the external database may contain additional details not stored in yours. If the external database is online you can even provide a URL prefix which will automatically link any data in your website to theirs via a web link.</li>

<li><b>Update or Delete and External Databases</b>
To edit the details of an external database record or to delete an already existing external database, go to the <a href="tripal_db/edit_db">Update/Delete DBs form</a>. This will allow you to change details or enter new details.</li>

</ul>
