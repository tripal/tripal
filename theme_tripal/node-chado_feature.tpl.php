<?php
// Purpose: This template provides the layout of the feature node (page)
//   using the same templates used for the various feature content blocks.
//
// To Customize the Featture Node Page:
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
 //print '<pre>'.print_r($variables,TRUE).'</pre>';
drupal_add_css('./tripal-node-templates.css');
$feature  = $variables['node']->feature;
//dpm($feature);
?>

<?php if ($teaser) { 
  include('tripal_feature/tripal_feature_teaser.tpl.php'); 
} else { ?>

<script type="text/javascript">
if (Drupal.jsEnabled) {
   $(document).ready(function() {
      // hide all tripal info boxes at the start
      $(".tripal-info-box").hide();
 
      // iterate through all of the info boxes and add their titles
      // to the table of contents
      $(".tripal-info-box-title").each(function(){
        var parent = $(this).parent();
        var id = $(parent).attr('id');
        var title = $(this).text();
        $('#tripal_feature_toc_list').append('<li><a href="#'+id+'" class="tripal_feature_toc_item">'+title+'</a></li>');
      });

      // when a title in the table of contents is clicked, then
      // show the corresponding item in the details box
      $(".tripal_feature_toc_item").click(function(){
         $(".tripal-info-box").hide();
         href = $(this).attr('href');
         $(href).fadeIn('slow');
         // we want to make sure our table of contents and the details
         // box stay the same height
         $("#tripal_feature_toc").height($(href).parent().height());
         return false;
      }); 

      // we want the base details to show up when the page is first shown 
      // unless the user specified a specific block
      var block = window.location.href.match(/\?block=.*/);
      if(block != null){
         block_title = block.toString().replace(/\?block=/g,'');
         $("#tripal_feature-"+block_title+"-box").show();
      } else {
         $("#tripal_feature-base-box").show();
      }

      $("#tripal_organism_toc").height($("#tripal_feature-base-box").parent().height());
   });
}
</script>

<div id="tripal_feature_details" class="tripal_details">

   <!-- Basic Details Theme -->
   <?php include('tripal_feature/tripal_feature_base.tpl.php'); ?>

   <!-- Database References -->
   <?php include('tripal_feature/tripal_feature_references.tpl.php'); ?>

   <!-- Properties -->
   <?php include('tripal_feature/tripal_feature_properties.tpl.php'); ?>

   <!-- Synonyms -->
   <?php include('tripal_feature/tripal_feature_synonyms.tpl.php'); ?>

   <!-- Sequence -->
   <?php 
   if(strcmp($feature->type_id->name,'scaffold')!=0 and 
      strcmp($feature->type_id->name,'chromosome')!=0 and
      strcmp($feature->type_id->name,'supercontig')!=0 and
      strcmp($feature->type_id->name,'pseudomolecule')!=0)
   {
      include('tripal_feature/tripal_feature_sequence.tpl.php'); 
   }
   ?>

   <!-- Formatted Sequences -->
   <?php include('tripal_feature/tripal_feature_featureloc_sequences.tpl.php'); ?>

   <!-- Relationships -->
   <?php include('tripal_feature/tripal_feature_relationships.tpl.php'); ?>

   <!-- Feature locations -->
   <?php 
   if(strcmp($feature->type_id->name,'scaffold')!=0 and 
      strcmp($feature->type_id->name,'chromosome')!=0 and
      strcmp($feature->type_id->name,'supercontig')!=0 and
      strcmp($feature->type_id->name,'pseudomolecule')!=0)
   {
      include('tripal_feature/tripal_feature_featurelocs.tpl.php'); 
   }
   ?>

   <?php print $content ?>
</div>

<!-- Table of contents -->
<div id="tripal_feature_toc" class="tripal_toc">
   <div id="tripal_feature_toc_title" class="tripal_toc_title">Resources</div>
   <ul id="tripal_feature_toc_list" class="tripal_toc_list">

   </ul>
</div>

<?php } ?>
