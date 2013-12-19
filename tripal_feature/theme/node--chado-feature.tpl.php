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

$feature  = $variables['node']->feature;

// get the template settings
$template_settings = theme_get_setting('tripal');

// toggle the sidebar if desired
$no_sidebar = 0;
if (is_array($template_settings['tripal_no_sidebar']) and 
   $template_settings['tripal_no_sidebar']['feature']) {
  $no_sidebar = 1;
}
$feature_no_sidebar = preg_split('/\n/', $template_settings['tripal_feature_no_sidebar']);

if ($teaser) { 
  print theme('tripal_feature_teaser', $variables); 
} 
else { ?>

<script type="text/javascript">
(function ($) {
  Drupal.behaviors.featureBehavior = {
    attach: function (context, settings){ <?php 
      if ($no_sidebar or in_array($feature->type_id->name, $feature_no_sidebar)) { ?>    
        // hide the resource side bar and strech the details section    
        $(".tripal_toc").hide();
        $(".tripal_details").addClass("tripal_details_full");
        $(".tripal_details_full").removeClass("tripal_details"); <?php
      } else { ?>
        // use default resource sidebar
        $(".tripal-info-box").hide(); <?php
      } ?>
 
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
      var block = window.location.href.match(/[\?|\&]block=(.+?)\&/)
      if(block == null){
         block = window.location.href.match(/[\?|\&]block=(.+)/)
      }
      if(block != null){
         $("#tripal_feature-"+block[1]+"-box").show();
      } else {
         $("#tripal_feature-base-box").show();
      }

      $("#tripal_organism_toc").height($("#tripal_feature-base-box").parent().height());
    }     
  };
})(jQuery);
</script>

<div id="tripal_feature_details" class="tripal_details">

   <!-- Basic Details Theme -->
   <?php print theme('tripal_feature_base',$node); ?>

   <!-- Database References -->
   <?php print theme('tripal_feature_references', $node); ?>

   <!-- Properties -->
   <?php print theme('tripal_feature_properties', $node); ?>

   <!-- Annotated Terms -->
   <?php print theme('tripal_feature_terms', $node); ?>

   <!-- Synonyms -->
   <?php print theme('tripal_feature_synonyms', $node); ?>
   
   <!-- Phenotypes -->
   <?php print theme('tripal_feature_phenotypes', $node); ?>
   
   <!-- Maps -->   
   <?php print theme('tripal_feature_featurepos', $node); ?>

   <!-- Sequence --> <?php 
   if(strcmp($feature->type_id->name,'scaffold')!=0 and 
      strcmp($feature->type_id->name,'chromosome')!=0 and
      strcmp($feature->type_id->name,'supercontig')!=0 and
      strcmp($feature->type_id->name,'pseudomolecule')!=0)
   {
      print theme('tripal_feature_sequence', $node); 
   } ?>

   <!-- Formatted Sequences -->
   <?php print theme('tripal_feature_featureloc_sequences', $node); ?>

   <!-- Relationships -->
   <?php print theme('tripal_feature_relationships', $node); ?>
   
   <!-- Feature locations --> <?php 
   if(strcmp($feature->type_id->name,'scaffold')!=0 and 
      strcmp($feature->type_id->name,'chromosome')!=0 and
      strcmp($feature->type_id->name,'supercontig')!=0 and
      strcmp($feature->type_id->name,'pseudomolecule')!=0)
   {
      print theme('tripal_feature_alignments', $node); 
   } ?>
     
  
   <!-- Resource Blocks CCK elements --><?php
   for($i = 0; $i < count($node->field_resource_titles); $i++){
     if($node->field_resource_titles[$i]['value']){ ?>
       <div id="tripal_feature-resource_<?php print $i?>-box" class="tripal_feature-info-box tripal-info-box">
         <div class="tripal_feature-info-box-title tripal-info-box-title"><?php print $node->field_resource_titles[$i]['value'] ?></div>
         <?php print $node->field_resource_blocks[$i]['value']; ?>
       </div><?php
     }
   }?>
   
   <!-- Let modules add more content -->
   <?php print $content ?>
</div>

<!-- Table of contents -->
<div id="tripal_feature_toc" class="tripal_toc">
   <div id="tripal_feature_toc_title" class="tripal_toc_title">Resources</div>
   <ul id="tripal_feature_toc_list" class="tripal_toc_list">
   
     <!-- Resource Links CCK elements --><?php
     for($i = 0; $i < count($node->field_resource_links); $i++){
       if($node->field_resource_links[$i]['value']){
         $matches = preg_split("/\|/",$node->field_resource_links[$i]['value']);?>
         <li><a href="<?php print $matches[1] ?>" target="_blank"><?php print $matches[0] ?></a></li><?php
       }
     }?>
     
     <?php // ADD CUSTOMIZED <li> LINKS HERE ?>
   </ul>
</div>

<?php } ?>
