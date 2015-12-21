<h3>Module Description:</h3>
<p>The Tripal DB Module provides the ability to add database cross reference to the
  data in your Tripal Website.  Typically an external database (such as NCBI Genbank, Gene Ontology (GO),
  stocks database) contains a collection of objects (genomic sequences, vocabulary terms, stocks) that are
  uniquely identified using an accession number (or identifier).  Data loaded into Tripal can be a
  associated with these objects in remote databases, and links can appear on pages allowing site visitors
  to view the associated objects on the remote database\'s website </p>

<h3>Setup Instructions:</h3>
<ol>
    <li><p><b>Set Permissions</b>: By default only the site administrator account has access to
   or administer databases. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
   permissions under the 'tripal_db' section as appropriate for your site. For a simple setup, allow anonymous
   users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>

<li><p><b>Adding or Editing an External Databases</b>. Many resources such as NCBI nr or ExPASy SwissProt (to name a few)
          come pre-loaded with Chado.  However, you can add new entries or edit existing entries. Also, when loading
          ontologies (controlled vocabularies) using the Tripal CV module new databases are added automaticaly for
          each ontology.  To enable linking of accession on a page to the page for that accession on the external
          database, simply add the URL and the URL prefix when adding or editing a database.</p></li>

<li><p><b>Associate Data with Accessions</b>.  The Tripal loaders (e.g. GFF, OBO) can associate accessions from
          remote data to genomic features and controlled vocabularies automatically.  Use the loaders to load genomic
          features and controlled vocabularies respectively.  Additionally, the bulk loader can be used to create
          loading templates for associating external database accessions.
         </p></li>
</ol>


