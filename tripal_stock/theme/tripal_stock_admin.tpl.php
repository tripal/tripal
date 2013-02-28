  <h3>Tripal Stock Administrative Tools Quick Links</h3>
    <ul>
      <li><?php print l(t('Configuration'), 'admin/tripal/tripal_stock/configuration') ?></li>
      <li><?php print l(t('Stock Listing'), 'stocks') ?></li>
    </ul>

  <h3>Module Description:</h3>
  <p>The Tripal Stock Module provides functionality for adding, editing, deleting and accessing chado stocks. The stock module was designed to store information about stock collections in a laboratory. What is called a stock could also be called a strain or an accession. There is a lot in common between a Drosophila stock and a Saccharomyces strain and an Arabidopsis line. They all come from some taxon, have genotypes, physical locations in the lab, some conceivable relationship with a publication, some conceivable relationship with a sequence feature (such as a transgene), and could be described by some ontology term. For more information about the chado Stock Module <a href="http://gmod.org/wiki/Chado_Stock_Module">see the GMOD Wiki Page</a></p>

  <h3>Setup Instructions:</h3>
  <ol>
  <li><b>Set Ontologies</b>: Since at the time of this modules developement there is no accepted ontology for
            describing stocks, their properties and relationships, this module allows you to select the controlled
            vocabularies (CVs) in your Chado Database you would like to govern these data. To Set the Controlled Vocabularies for Stocks:
            First, ensure your Controlled Vocabulary is in Chado. This can be done by either loading an existing Ontology into Chado using
            the <a href="tripal_cv/ontology_loader">Tripal Ontology Loader</a> OR create your ontology from scratch by first
            <a href="tripal_cv/add_cv">creating a controlled vocabulary</a> and then <a href="tripal_cv/add_cvterm">adding terms to it</a>.
            Then go to the <a href="tripal_stock/configuration">Configuration Page for Stocks</a> and, in the "Set Stock Controlled Vocabularies" Fieldset,
            select the Controlled Vocaulary name for Stock Types, Stock Properties and Stock Relationship Types.</li>
      <ol type="i">
        <li>Stock Types: When you are creating stocks, the type of each stock must be indicated. This might include "DNA extraction", "Individual Plant/Animal" or even "Progeny Population".</li>
        <li>Stock Properties: This module also allows you to assign properties to any stock. Each property has a type and a value where type is required an value is not. Therefore, if you want to say that a stock was grown at 23 degrees Celcius then the Type would be "temperature grown at" and the value would be 23 degrees Celcius. As such the Stock Properties controlled vocabulary might include "temperature grown at", "diet", "extraction date", "stock location", etc.</li>
        <li>Stock Relationship Types: You can also specify relationships between stocks. For example, a stock of type="DNA extraction" (Stock 1a) is related to the stock of type="Individual Plant/Animal" (Stock 1) that it was extracted from. Thus you might specify the relationship Stock 1 is the source material for Stock 1a where the relationship type is "is the source material for". As such Stock Relationship Types might include "is the source material for", "is maternal parent of", "is individual of population", etc.</li>
      </ol>

  <li><p><b>Set Permissions</b>: The stock module supports the Drupal user permissions interface for
               controlling access to stock content and functions. These permissions include viewing,
               creating, editing or administering of
               stock content. The default is that only the original site administrator has these
               permissions.  You can <a href="<?php url('admin/user/roles') ?>">add roles</a> for classifying users,
               <a href="<?php url('admin/user/user') ?>">assign users to roles</a> and
               <a href="<?php url('admin/user/permissions') ?>">assign permissions</a> for the stock content to
               those roles.  For a simple setup, allow anonymous users access to view organism content and
               allow the site administrator all other permissions.</p></li>

  <li><b>Sync Stocks</b>: if you chado database already contains stocks, they need to be sync'd with Drupal</b>. This creates Drupal Content including detail pages for each stock (known as nodes in Drupal). To sync' Chado with Drupal simply go to the <a href="tripal_stock/configuration">Configuration Page for Stocks</a> and in the "Sync Stocks" Fieldset select the Organisms whose associated stocks you would like to sync. If this list doesn't contain an organism which you know is in Chado go to the Organism Configuration Page and make sure it is sync'd with Drupal.</p>
  </ol>
  <h3>Features of this Module:</h3>
  <ul>
  <li><b><a href="../../node/add/chado_stock">Create a Generic Stock:</a></b>
  <p>This allows you to create content in your drupal and chado for a stock (only the unique stock identifier is duplicated). A Generic Stock must have a unique name, a type and an organism. In addition, you can optionally supply a more human-readable name, a primary database reference and even a short description. The Create Generic Stock form is a multistep form with the first step creating the Basic stock (stored in the stock table). All the remaining steps are optional and descriptions of each follow:</p>
      <ol type="i">
        <li>The Next Step is to Add Properties to the newly created stock. Properties allow you to specify additional details about a given stock. Since the types of properties you can add are goverened by a controlled vocaulary that you can create, you have complete control over what additional properties you want to allow.</li>
        <li>Then you can Add External Database References. A Database Reference can be thought of as a synonym for the current stock where you want to specify a source for that synonym. The source would then be thought of as the database where a database can either be online and provide automatic linking out to the synonymous record or offline and simply be a text name of the source. To create a database reference with a given source you must first add the database to chado <a href="tripal_db/add_db">here</a>.</li>
        <li>Finally you can Add Relationships between Stocks. This allows you to specify, for example, the source material of a stock or one of it's parents. To create a relationship between your newly added stock and another stock, the other stock must first be created as this one was. Also, since the types of relationships is governed by a controlled vocabulary, just like with properties you have complete control over which relationships you want to allow. Once you click "Finish" you will be re-directed to the Details Page of the new Stock.</li>
      </ol></li>

  <li><b>Details Page of a Stock:</b>
  <p>Each stock get's it's own page on this website. This page is meant to give an overall picture of the stock including listing the basic details, as well as, all properties, database references and relationships. To understand where it is -All page content in Drupal is known as a node and is given a unique identifier or nid. Thus every drupal page has a path of node/<nid>. You can get to the Details page for a given stock from either of the stock listings described below.</p>
  <p>If you want to customize the look of the stock Details page simply copy the PHP/HTML template node-chado_stock.tpl.php from theme_tripal to the base theme you are currently using. Then edit it as desired. There are plans to integrate this details page with Drupal Panels which will provide a much more user-friendly and no-programming-needed method to customize this page.</p>

  <li><b>Adding/Updating/Deleting Stocks and their Properties, Database References and Relationships:</b>
  <p>The Stock Details Page also acts as a landing pad for updating/deleting stocks. To <b>update a stock</b>, go to the stocks details page and click on the Edit tab near the top of the page. This tab will only be visable if you have permission to edit chado stock content (See post installation steps above for information on setting user permissions). If you want to <b>delete a stock</b>, click the Edit tab and then near the bottom of the form, click the Delete button. This will delete the entire stock including it's properties, database references and any relationships including it.</p>
  <p>To <b>update/delete a given property of a stock</b>, click the "Edit Properties" Tab near the top of the stock details page. This form provides a listing of all existing properties for the given stock with form elements allowing you to change their value. After editing the properties you wanted to, simply click the "Update Properties" button to update all the properties for that stock. To delete a given property simply click the "Delete" Button beside the property you want to delete. You cannot undo this action! To <b>add a property to the given stock</b> simply fill out the "Add Property" form at the bottom of the "Edit Properties" Tab.</p>
  <p><b>Adding, updating and deleting Database References and Relationships</b> for a given stock is exactly the same as the method for properties. To edit Database References, click the "Edit DB References" tab and to add/edit/update stock relationships, click the "Edit Relationships" tab.</p></li>

  <li><b><a href="../../stocks">Basic Listing of Stocks:</a></b>
  <p>This module also provides a basic listing of all stocks currently sync'd with Drupal. To access this listing, there should be a Stocks Primary Menu item which links you to <a href="../../stocks">this page</a>. This page lists each stock on it's own row and provides a link to each stock by clicking on it's name. Currently there is no way to easily customize this listing.</p></li>

  <li><b><a href="../build/views/">Flexible Listing of Stocks using Drupal Views:</a></b>
  <p>In order to access a more flexible listing of stocks you must first install the <a href="http://drupal.org/project/views">Drupal Views2 module</a>. You should then be able to access the default views <a href="../build/views/">here</a>. Essentially, Views is a module which allows you to create custom SQL queries completely through the web interface without knowing SQL. Furthermore, it also does some formatting of the results allowing you to display them as HTML lists, tables or grids. You can also expose filters to the user to let them customize the results they see and even implement various sorting.</p>
  <p>To use one of the Default Views simply click "Enable" and then "Edit" to change it to show exactly what you want. To view the current listing simply clikc "View Page" at the top of the Edit user interface. There are a number of good tutorials out there for Views2, any of which can be used to help you create your own custom listings of biological content. (Note: there aren't any tutorials specifically for tripal content but any tutorial for Views2 will show you how to use the views interface.</p></li>

  <h3>Page Customizations</h3>
  <p>There are several ways to customize the look-and-feel for the way Chado data is presented through Tripal.
     Below is a description of several methods.  These methods may be used in conjunction with one another to
     provide fine-grained control.
     <ul>

     <li><p><b>Integration with Drupal Panels</b>:  <a href="http://drupal.org/project/views">Drupal Panels</a>
      allows for customization of a page layout if you don't want to do PHP/Javascript/CSS programming.  Tripal comes with pre-set layouts for stock pages.  However,
      Panels become useful if you prefer a layout that is different from the pre-set layouts.  Chado content
      is provided to Panels in the form of Drupal "blocks" which you can then place anywhere on a page using the
      Panel's GUI.</p></li>

     <li><p><b>Drupal's Content Construction Kit (CCK)</b>: the
     <a href="http://drupal.org/project/cck">Content Construction Kit (CCK) </a> is a powerful way to add non-Chado content
     to any page without need to edit template files or knowing PHP.  You must first download and install CCK.
     With CCK, the site administartor can create a new field to appear on the page.  For example, currently,
     the Chado publication module is not yet supported by Tripal.  Therefore, the site administrator can add a text
     field to the stock pages.  This content is not stored in Chado, but will appear on the stock page.  A field
     added by CCK will also appear in the form when editing a stock to allow users to manually enter the appropriate
     text.  If the default pre-set layout and themeing for Tripal is used, it is better to create the CCK element,
     indicate that it is not to be shown (using the CCK interface), then manually add the new content type
     where desired by editing the templates (as described below).  If using Panels, the CCK field can be added to the
     location desired using the Panels interface.</p></li>

     <li><p><b>Drupal Node Templates</b>:  The Tripal packages comes with a "theme_tripal" directory that contains the
     themeing for Chado content.    The stock module has a template file for stock "nodes" (Tripal stock pages).  This file
     is named "node-chado_stock.tpl.php", and provides javascript, HTML and PHP code for display of the stock
     pages.  You can edit this file to control which types of information (or which stock "blocks") are displayed for stocks. Be sure to
     copy these template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
     future Tripal updates may overwrite your customizations. See the <a href="http://tripal.info/">Tripal website </a>
     for instructions on how to access variables and other Chado content within the template file.</p></li>

     <li><p><b>Stock "Block" Templates</b>:  In the "theme_tripal" directory is a subdirectory named "tripal_stock".
     Inside this directory is a set of templates that control distinct types of information for stocks.  For example,
     there is a "base" template for displaying of data directly from the Chado stock table, and a "references"
     template for showing external site references for a stock (data from the stock_dbxref table).  These templates are used both by Drupal blocks
     for use in Drupal Panels (as described above) or for use in the default pre-set layout that the node template
     provides (also desribed above).  You can customize this template as you desire.  Be sure to copy the
     template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
     future Tripal updates may overwrite your customizations.  See the <a href="http://tripal.info/">Tripal website </a>
     for instructions on how to access variables and other Chado content within the template files.</p></li>
     </li>

     <li><p><b>Adding Links to the "Resources" Sidebar</b>: If you use the pre-set default Tripal layout for theming, you
     will see a "Resources" sidebar on each page.  The links that appear on the sidebar are automatically generated
     using Javascript for all of the stock "Blocks" that appear on the page. If you want to add additional links
     (e.g. a dynamic link to additional stock content) and you want that link to appear in the
     "Resources" sidebar, simply edit the Drupal Node Template (as described above) and add the link to the
     section at the bottom of the template file where the resources section is found.</p></li>

     </ul>
   </p>