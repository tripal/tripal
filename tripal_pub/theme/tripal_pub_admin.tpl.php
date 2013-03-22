<br /><h3>Tripal Publication Administrative Tools Quick Links</h3>
<ul>
<li><?php print l('Configuration', 'admin/tripal/tripal_pub/configuration') ?></li>
</ul>

<h3>Module Description:</h3>
<p>The Tripal Publication Module provides the functionality for adding, editing, deleting and
accessing academic publications, entered by the user.This module also allows a time limited search,
specified by the user, which searches the PubMed extracts and saves acedemic puplications.
 </p>
<h3>Setup Instructions:</h3>
<ol>
<li><p><b>Set Permissions</b>: The publication module supports the Drupal user permissions interface for
               controlling access to publlication content and functions. These permissions include viewing,
               creating, editing or administering of
               publication content. The default is that only the original site administrator has these
               permissions.  You can <a href="<?php print url('admin/user/roles') ?>">add roles</a> for classifying users,
               <a href="<?php print url('admin/user/user') ?>">assign users to roles</a> and
               <a href="<?php print url('admin/user/permissions') ?>">assign permissions</a> for the publication content to
               those roles.  For a simple setup, allow anonymous users access to view publication content and
               allow the site administrator all other permissions.</p></li>
<li><p><b>Set Publication Type Controlled Vocabulary</b>: The select list for setting the publication 
                type is controlled be a controlled vocabulary (cv)
                <ul><li>Before you can add any publications you need 
                to create/load this cv. There is a limited cv included in this module. To use it, you need to 
                load it using the <a href="<?php print url('admin/tripal/tripal_cv/obo_loader') ?>">OBO Loader included with Tripal</a>.</li>
                <li>After the controlled vocabulary is loaded you need to set it to be used for the publication 
                module. To do this, go to <a href="<?php print url('admin/tripal/tripal_pub/configuration') ?>">Publication->Configuration</a>, select it in the controlled vocabulary '
                select list and click save configuration.</p></li></ul>

</ol>
<h3>Features of this Module:</h3>
<ul>
<li><p><b>Configuration (Search For Academic Publications):</b>  The search capability implemented in
  this module allows a user to search, by remote connection , the PubMEd database for articles
  that relate to key search words, chosen by the user.The "search keys" are used to search through
  Publication titles and when one of the key words is matched in a title, the recognized article will
  be saved to the database.

      <ul>

      <li><b>Choose a Controlled Vocabulary:</b>The controlled vocabulary list is a set of terms

      <li><b>Set Search Key Words:</b>The search keywords, are the user entered key terms, in which
      the publications in the PubMed database can be recognized by. The user may enter any number
      of search terms, as by adding more search terms, the search will limit the results to those
      in which all of the search terms appear in the publication title.

      <li><b>Set a time search interval:</b>The search term interval represents a pre-set ammount
      of time for the search. The time search interval must be entered in minutes. This allows
      the module to automatically search the PubMed database in a predetermined time interval.
    </ul>
  </p></li>

<li><b>Creating a Publication:</b>

<p>To <b>Create,update/delete a given property of a publication</b>:When Creating a Publication
  it is neccessary to enter the requried fields in the form. These are marked with an astrix and
  if they are not entered upon completion a warning will be issued and the user is forced to fill
  in these entries. The author field, requires a given/surname/suffix to be entered. To add the
  author to the publication, the add author button is to be pushed. The user is able to add as
  many authors to the publication as needed.
  </p>

