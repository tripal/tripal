<?php
// Purpose: This template provides the layout of the pub node (page)
//   using the same templates used for the various pub content blocks.
//
// To Customize the Libray Node Page:
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

$pub  = $variables['node']->pub;

// get the template settings
$template_settings = theme_get_setting('tripal');

// toggle the sidebar if desired
$no_sidebar = 0;
if (is_array($template_settings['tripal_no_sidebar']) and 
   $template_settings['tripal_no_sidebar']['pub']) {
  $no_sidebar = 1;
}

if ($teaser) { 
  print theme('tripal_pub_teaser',$node); 
} 
else { ?>

<script type="text/javascript">
// hide the node title. It is not informative to the viewer
$(".title").hide();
   
(function ($) {
  Drupal.behaviors.pubBehavior = {
    attach: function (context, settings){ <?php 
      if ($no_sidebar) { ?>    
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
        $('#tripal_pub_toc_list').append('<li><a href="#'+id+'" class="tripal_pub_toc_item">'+title+'</a></li>');
      });

      // when a title in the table of contents is clicked, then
      // show the corresponding item in the details box
      $(".tripal_pub_toc_item").click(function(){
         $(".tripal-info-box").hide();
         href = $(this).attr('href');
         $(href).fadeIn('slow');
         // we want to make sure our table of contents and the details
         // box stay the same height
         $("#tripal_pub_toc").height($(href).parent().height());
         return false;
      }); 

      // we want the base details to show up when the page is first shown 
      // unless the user specified a specific block
      var block = window.location.href.match(/[\?|\&]block=(.+?)\&/)
      if(block == null){
         block = window.location.href.match(/[\?|\&]block=(.+)/)
      }
      if(block != null){
         $("#tripal_pub-"+block[1]+"-box").show();
      } else {
         $("#tripal_pub-base-box").show();
      }

      $("#tripal_pub_toc").height($("#tripal_pub-base-box").parent().height());
    }     
  };
})(jQuery);
</script>

<div id="tripal_pub_details" class="tripal_details">

   <!-- Basic Details Theme -->   
   <?php print theme('tripal_pub_base', $node); ?>
   
   <!-- Properties Theme -->
   <?php print theme('tripal_pub_properties', $node); ?>
   
   <!-- Authors Theme -->
   <?php print theme('tripal_pub_authors', $node); ?>   
   
   <!-- References Theme -->
   <?php print theme('tripal_pub_references', $node); ?>
        
   <!-- Relationships Theme -->
   <?php print theme('tripal_pub_relationships', $node); ?>

   <!-- FeatureMaps Theme -->
   <?php print theme('tripal_pub_featuremaps', $node); ?>

   <!-- Features Theme -->
   <?php print theme('tripal_pub_features', $node); ?>

   <!-- Libraries Theme -->
   <?php print theme('tripal_pub_libraries', $node); ?>

   <!-- Projects Theme -->
   <?php print theme('tripal_pub_projects', $node); ?>

   <!-- Stocks Theme -->
   <?php print theme('tripal_pub_stocks', $node); ?>


   <!-- Resource Blocks CCK elements --><?php
   for($i = 0; $i < count($node->field_resource_titles); $i++){
     if($node->field_resource_titles[$i]['value']){ ?>
       <div id="tripal_pub-resource_<?php print $i?>-box" class="tripal_pub-info-box tripal-info-box">
         <div class="tripal_pub-info-box-title tripal-info-box-title"><?php print $node->field_resource_titles[$i]['value'] ?></div>
         <?php print $node->field_resource_blocks[$i]['value']; ?>
       </div><?php
     }
   }?>
   
   <!-- Let modules add more content -->

   <?php print $content ?>
</div>

<!-- Table of contents -->
<div id="tripal_pub_toc" class="tripal_toc">
   <div id="tripal_pub_toc_title" class="tripal_toc_title">Resources</div>
   <ul id="tripal_pub_toc_list" class="tripal_toc_list">
   
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
