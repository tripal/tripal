<?php
// Purpose: This template provides a generic layout for all Tripal nodes (page)

// get the template type of node (e.g. if type == chado_organism then template type == organism)
$ttype = $variables['type'];
$ttype = preg_replace('/chado_/','', $ttype);

// get the template settings
$template_settings = theme_get_setting('tripal');

// toggle the sidebar if desired
$no_sidebar = 0;
if (is_array($template_settings['tripal_no_sidebar']) and 
   $template_settings['tripal_no_sidebar'][$ttype]) {
  $no_sidebar = 1;
}

if ($teaser) { 
  print theme('tripal_' . $ttype . '_teaser', $variables); 
} 
else { ?>

<script type="text/javascript">
(function ($) {
  Drupal.behaviors.<?php print $ttype?>Behavior = {
    attach: function (context, settings){ <?php
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
        $('#tripal_<?php print $ttype?>_toc').width(toc_width);
        $('#tripal_<?php print $ttype?>_details').width(details_width);
        <?php
      } ?>
 
      // iterate through all of the info boxes and add their titles
      // to the table of contents
      $(".tripal-info-box-title").each(function(){
        var parent = $(this).parent();
        var id = $(parent).attr('id');
        var title = $(this).text();
        $('#tripal_<?php print $ttype?>_toc_list').append('<li><a href="#'+id+'" class="tripal_<?php print $ttype?>_toc_item">'+title+'</a></li>');
      });

      // when a title in the table of contents is clicked, then
      // show the corresponding item in the details box
      $(".tripal_<?php print $ttype?>_toc_item").click(function(){
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
        $("#tripal_<?php print $ttype?>_toc").height($(href).parent().height());
        return false;
      }); 

      // we want the base details to show up when the page is first shown 
      // unless we're using the feature browser then we want that page to show
      var block = window.location.href.match(/[\?|\&]block=(.+?)\&/)
      if(block == null){
        block = window.location.href.match(/[\?|\&]block=(.+)/)
      }
      if(block != null){
        $("#tripal_<?php print $ttype?>-"+block[1]+"-box").show();
      }
      else {
        $("#tripal_<?php print $ttype?>-base-box").show();
      }

      // make sure the sidebar and details sections are the same hieght
      base_height = $("#tripal_<?php print $ttype?>_details").height();
      toc_height = $("#tripal_<?php print $ttype?>_toc").height();
      if (toc_height > base_height) {
        $("#tripal_<?php print $ttype?>_details").height(toc_height);
      }
      else {
        $("#tripal_<?php print $ttype?>_toc").height(base_height);
      }
    }
  };
})(jQuery);
</script>

<div id="tripal_<?php print $ttype?>_content" class="tripal_contents">
  <div id="tripal_<?php print $ttype?>_details" class="tripal_details">
  
     <!-- Resource Blocks CCK elements --> <?php
     if (property_exists($node, 'field_resource_titles')) {
       for ($i = 0; $i < count($node->field_resource_titles); $i++){
         if ($node->field_resource_titles[$i]['value']){ ?>
           <div id="tripal_<?php print $ttype?>-resource_<?php print $i?>-box" class="tripal_<?php print $ttype?>-info-box tripal-info-box">
             <div class="tripal_<?php print $ttype?>-info-box-title tripal-info-box-title"><?php print $node->field_resource_titles[$i]['value'] ?></div>
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
  <div id="tripal_<?php print $ttype?>_toc" class="tripal_toc">
     <div id="tripal_<?php print $ttype?>_toc_title" class="tripal_toc_title">Resources</i></div>
     <span id="tripal_<?php print $ttype?>_toc_desc" class="tripal_toc_desc"></span>
     <ul id="tripal_<?php print $ttype?>_toc_list" class="tripal_toc_list">
     
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

