<?php
// Purpose: This template provides the layout of the organism node (page)
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

$organism  = $variables['node']->organism;

// get the template settings
$template_settings = theme_get_setting('tripal');

// toggle the sidebar if desired
$no_sidebar = 0;
if (is_array($template_settings['tripal_no_sidebar']) and 
   $template_settings['tripal_no_sidebar']['organism']) {
  $no_sidebar = 1;
}

if ($teaser) { 
  print theme('tripal_organism_teaser', $variables); 
} 
else { ?>

<script type="text/javascript">
(function ($) {
  Drupal.behaviors.organismBehavior = {
    attach: function (context, settings){<?php
      // hide the resource sidbar if requested and strech the details section
      if ($no_sidebar) { ?>    
        $(".tripal_toc").hide();
        $(".tripal_details").addClass("tripal_details_full");
        $(".tripal_details_full").removeClass("tripal_details"); <?php
      } 
      // use default resource sidebar
      else { ?>        
        $(".tripal-info-box").hide();

        // set the widths of the details and sidebar sections so they can work 
        // seemlessly with any theme.
        total_width = $(".tripal_contents").width();
        details_width = (total_width * 0.70) - 52; // 52 == 20  x 2 left/right padding + 10 right margin + 2pt border
        toc_width = (total_width * 0.30) - 42;  // 42 == 20 x 2 left/right padding + 2pt border
        // don't let sidebar get wider than 200px
        if (toc_width > 200) {
          details_width += toc_width - 200;
          toc_width = 200;
        }
        $('#tripal_organism_toc').width(toc_width);
        $('#tripal_organism_details').width(details_width); <?php
      } ?>
 
      // iterate through all of the info boxes and add their titles
      // to the table of contents
      $(".tripal-info-box-title").each(function(){
        var parent = $(this).parent();
        var id = $(parent).attr('id');
        var title = $(this).text();
        $('#tripal_organism_toc_list').append('<li><a href="#'+id+'" class="tripal_organism_toc_item">'+title+'</a></li>');
      });

      // when a title in the table of contents is clicked, then
      // show the corresponding item in the details box
      $(".tripal_organism_toc_item").click(function(){
         $(".tripal-info-box").hide();
         href = $(this).attr('href');
         if(href.match(/^#/)){
            //alert("correct: " + href);
         }
         else{
            tmp = href.replace(/^.*?#/, "#");
            href = tmp;
            //alert("fixed: " + href);
         }
         $(href).fadeIn('slow');
         // we want to make sure our table of contents and the details
         // box stay the same height
         $("#tripal_organism_toc").height($(href).parent().height());
         return false;
      }); 

      // we want the base details to show up when the page is first shown 
      // unless we're using the feature browser then we want that page to show
      var block = window.location.href.match(/[\?|\&]block=(.+?)\&/)
      if(block == null){
         block = window.location.href.match(/[\?|\&]block=(.+)/)
      }
      if(block != null){
         $("#tripal_organism-"+block[1]+"-box").show();
      } 
      else if(window.location.href.match(/\?page=\d+/)){
         $("#tripal_organism-feature_browser-box").show();
      } 
      else {
         $("#tripal_organism-base-box").show();
      }

      $("#tripal_organism_toc").height($("#tripal_organism-base-box").parent().height());
    }     
  };
})(jQuery);
</script>

<div id="tripal_organism_content" class="tripal_contents">
  <div id="tripal_organism_details" class="tripal_details">
  
     <!-- Resource Blocks CCK elements --> <?php
     if (property_exists($node, 'field_resource_titles')) {
       for ($i = 0; $i < count($node->field_resource_titles); $i++){
         if ($node->field_resource_titles[$i]['value']){ ?>
           <div id="tripal_organism-resource_<?php print $i?>-box" class="tripal_organism-info-box tripal-info-box">
             <div class="tripal_organism-info-box-title tripal-info-box-title"><?php print $node->field_resource_titles[$i]['value'] ?></div>
             <?php print $node->field_resource_blocks[$i]['value']; ?>
           </div> <?php
         }
       } 
     }?>
     <!-- Let modules add more content -->
     <?php
       foreach ($content as $key => $values) {
         if (array_key_exists('#value', $values)) {
           print $content[$key]['#value'];
         }
       }
     ?>
     
  </div>
  
  <!-- Table of contents -->
  <div id="tripal_organism_toc" class="tripal_toc">
     <div id="tripal_organism_toc_title" class="tripal_toc_title">Resources</i></div>
     <span id="tripal_organism_toc_desc" class="tripal_toc_desc"></span>
     <ul id="tripal_organism_toc_list" class="tripal_toc_list">
     
       <!-- Resource Links CCK elements --><?php
       if(property_exists($node, 'field_resource_links')) {
         for($i = 0; $i < count($node->field_resource_links); $i++){
           if($node->field_resource_links[$i]['value']){
             $matches = preg_split("/\|/",$node->field_resource_links[$i]['value']);?>
             <li><a href="<?php print $matches[1] ?>" target="_blank"><?php print $matches[0] ?></a></li><?php
           }
         }
       }
       ?> 
     </ul>
  </div>
</div> 
<?php 
} ?>

