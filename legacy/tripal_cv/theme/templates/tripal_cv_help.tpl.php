  <h3>Module Description:</h3>
  <p>The Tripal CV (Controlled Vocabularies) Module provides
    functionality for managing controlled vocabularies and the terms they are
    comprised of. The flexibility and extendibility of the chado schema depends
    on controlled vocabularies. For example, by using a controlled vocabulary for
    feature types the chado schema can describe features of any type, even those
    we have not concieved of yet.</p>

  <h3>Setup Instructions:</h3>
  <p>After installation of the controlled vocabulary module, the following tasks should be performed:</p>
    <ol>
        <li><p><b>Set Permissions</b>: By default only the site administrator account has access to create, edit, delete
   or administer vocabularies and terms. Navigate to the <?php print l('permissions page', 'admin/user/permissions')?> and set the
   permissions under the 'tripal_cv' section as appropriate for your site. For a simple setup, allow anonymous
   users access to view content and create a special role for creating, editing and other administrative tasks.</p></li>

      <li><p><b>Loading of Ontologies/Controlled Vocabularies</b>: You can access this loader at <?php
        print l('Admin->Tripal Management->Tripal CV->Load Ontology With OBO File', 'admin/tripal/tripal_cv/obo_loader')
        ?>. This loader allows you to choose from a list of common ontologies or
        enter the URL or location to an OBO file. Even the list of common
        ontologies is using a URL ensuring you get the most up to date ontology.</p>
      <p>NOTE: in some cases, community developed ontologies for your
        data may not yet be developed. In this case, it is suggested that you begin
        developement of an ontology using one of the online tools. You might find
        that many researchers are trying to deal with the same data and are willing
        to help you in this endevor. </p></li>
    </ol>

  <h3>Features of this Module:</h3>
  <p>Aside from the data loading described above, the Tripal Controlled Vocabulary (CV) module also provides the following functionality:</p>
    <ul>
      <li><p><b>Create/Update/Delete A Controlled Vocaulbulary</b>: to create your own controlled vocabulary go to
        <?php l('the page for adding a vocbulary', 'admin/tripal/tripal_cv/cv/add') ?> and
        fill out the form provided. To Update/Delete a controlled vocabulary go to
        <?php print l('the page for editing a vocuabulary', 'admin/tripal/tripal_cv/cv/edit') ?>,
        select the existing vocabulary you want to modify and then
        edit it as desired. This only modifies the vocabulary itself. See the next section for adding, removing, editing
        the terms of a vocabulary.</p></li>

      <li><p><b>Create a Controlled Vocaulbulary Term</b>: Use
        <?php print l('the page for adding a new CV term', 'admin/tripal/tripal_cv/cvterm/add') ?>,
        select the controlled vocabulary you want to add terms to and then fill
        out the form.</p></li>

    </ul>
