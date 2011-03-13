<?php
// Copyright 2010 University of Saskatchewan (Lacey-Anne Sanderson)
//
// Purpose: This template provides the layout of the stock node (page)
//   using the same templates used for the various stock content blocks.
//
// To Customize the Stock Node Page:
//   - This Template: customize basic layout and which elements are included
//   - Using Panels: Override the node page using Panels3 and place the blocks
//       of content as you please. This method requires no programming. See
//       the Tripal User Guide for more details
//   - Block Templates: customize the content/layout of each block of stock 
//       content. These templates are found in the tripal_stock subdirectory
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes including nid, type, title, taxonomy. It also includes stock
//       specific fields such as stock_name, uniquename, stock_type, synonyms,
//       properties, db_references, object_relationships, subject_relationships,
//       organism, etc.
//   NOTE: For a full listing of fields available in the node object the
//       print_r $node line below or install the Drupal Devel module which 
//       provides an extra tab at the top of the node page labelled Devel
?>

<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
?>

<!-- Base Theme -->
<?php include('tripal_stock/tripal_stock_base.tpl.php'); ?>

<?php if (!$teaser) { ?>
<!-- Database References -->
<?php include('tripal_stock/tripal_stock_references.tpl.php'); ?>

<!-- Properties -->
<?php include('tripal_stock/tripal_stock_properties.tpl.php'); ?>

<!-- Synonyms -->
<?php include('tripal_stock/tripal_stock_synonyms.tpl.php'); ?>

<!-- Object Relationships -->
<?php include('tripal_stock/tripal_stock_relationships_as_object.tpl.php'); ?>

<!-- Subject Relationships -->
<?php include('tripal_stock/tripal_stock_relationships_as_subject.tpl.php'); ?>
<?php } ?>